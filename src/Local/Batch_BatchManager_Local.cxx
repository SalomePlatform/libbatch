//  Copyright (C) 2007-2010  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
/*
* BatchManager_Local.cxx :
*
* Auteur : Ivan DUTKA-MALEN - EDF R&D
* Mail   : mailto:ivan.dutka-malen@der.edf.fr
* Date   : Thu Nov  6 10:17:22 2003
* Projet : Salome 2
*
* Refactored by Renaud Barate (EDF R&D) in September 2009 to use
* CommunicationProtocol classes and merge Local_SH, Local_RSH and Local_SSH batch
* managers.
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <limits.h>

#include <sys/types.h>
#ifdef WIN32
#include <direct.h>
#else
#include <sys/wait.h>
#include <unistd.h>
#endif
#include <ctime>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include "Batch_IOMutex.hxx"
#include "Batch_BatchManager_Local.hxx"
#include "Batch_RunTimeException.hxx"

using namespace std;

namespace Batch {


  // Constructeur
  BatchManager_Local::BatchManager_Local(const FactBatchManager * parent, const char * host,
                                         CommunicationProtocolType protocolType)
    : BatchManager(parent, host), _connect(0),
      _protocol(CommunicationProtocol::getInstance(protocolType)),
      _idCounter(0)
  {
    pthread_mutex_init(&_threads_mutex, NULL);
    pthread_cond_init(&_threadSyncCondition, NULL);
  }

  // Destructeur
  BatchManager_Local::~BatchManager_Local()
  {
    for (map<Id, Child>::iterator iter = _threads.begin() ; iter != _threads.end() ; iter++) {
      pthread_mutex_lock(&_threads_mutex);
      string state = iter->second.param[STATE];
      if (state != FINISHED && state != FAILED) {
        UNDER_LOCK( cout << "Warning: Job " << iter->first <<
                            " is not finished, it will now be canceled." << endl );
        pthread_cancel(iter->second.thread_id);
        pthread_cond_wait(&_threadSyncCondition, &_threads_mutex);
      }
      pthread_mutex_unlock(&_threads_mutex);
    }
    pthread_mutex_destroy(&_threads_mutex);
    pthread_cond_destroy(&_threadSyncCondition);
  }

  const CommunicationProtocol & BatchManager_Local::getProtocol() const
  {
    return _protocol;
  }

  // Methode pour le controle des jobs : soumet un job au gestionnaire
  const JobId BatchManager_Local::submitJob(const Job & job)
  {
    Job_Local jobLocal = job;
    Id id = _idCounter++;
    ThreadAdapter * p_ta = new ThreadAdapter(*this, job, id);

    // Les attributs du thread a sa creation
    pthread_attr_t thread_attr;
    pthread_attr_init(&thread_attr);
    pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);

    // Creation du thread qui va executer la commande systeme qu'on lui passe
    pthread_t thread_id;
    pthread_mutex_lock(&_threads_mutex);
    int rc = pthread_create(&thread_id,
      &thread_attr,
      &ThreadAdapter::run,
      static_cast<void *>(p_ta));

    // Liberation des zones memoire maintenant inutiles occupees par les attributs du thread
    pthread_attr_destroy(&thread_attr);

    if (rc != 0) {
      pthread_mutex_unlock(&_threads_mutex);
      throw RunTimeException("Can't create new thread in BatchManager_Local");
    }

    pthread_cond_wait(&_threadSyncCondition, &_threads_mutex);
    pthread_mutex_unlock(&_threads_mutex);

    ostringstream id_sst;
    id_sst << id;
    return JobId(this, id_sst.str());
  }

  // Methode pour le controle des jobs : retire un job du gestionnaire
  void BatchManager_Local::deleteJob(const JobId & jobid)
  {
    Id id;

    istringstream iss(jobid.getReference());
    iss >> id;

    // On retrouve le thread_id du thread
    pthread_t thread_id;

    // @@@ --------> SECTION CRITIQUE <-------- @@@
    pthread_mutex_lock(&_threads_mutex);
    if (_threads.find(id) != _threads.end())
      thread_id = _threads[id].thread_id;
    pthread_mutex_unlock(&_threads_mutex);
    // @@@ --------> SECTION CRITIQUE <-------- @@@

    cancel(thread_id);
  }

  // Methode pour le controle des jobs : suspend un job en file d'attente
  void BatchManager_Local::holdJob(const JobId & jobid)
  {
    Id id;
    istringstream iss(jobid.getReference());
    iss >> id;

    UNDER_LOCK( cout << "BatchManager is sending HOLD command to the thread " << id << endl );

    // On introduit une commande dans la queue du thread
    // @@@ --------> SECTION CRITIQUE <-------- @@@
    pthread_mutex_lock(&_threads_mutex);
    if (_threads.find(id) != _threads.end())
      _threads[id].command_queue.push(HOLD);
    pthread_mutex_unlock(&_threads_mutex);
    // @@@ --------> SECTION CRITIQUE <-------- @@@
  }

  // Methode pour le controle des jobs : relache un job suspendu
  void BatchManager_Local::releaseJob(const JobId & jobid)
  {
    Id id;
    istringstream iss(jobid.getReference());
    iss >> id;

    UNDER_LOCK( cout << "BatchManager is sending RELEASE command to the thread " << id << endl );

    // On introduit une commande dans la queue du thread
    // @@@ --------> SECTION CRITIQUE <-------- @@@
    pthread_mutex_lock(&_threads_mutex);
    if (_threads.find(id) != _threads.end())
      _threads[id].command_queue.push(RELEASE);
    pthread_mutex_unlock(&_threads_mutex);
    // @@@ --------> SECTION CRITIQUE <-------- @@@
  }


  // Methode pour le controle des jobs : modifie un job en file d'attente
  void BatchManager_Local::alterJob(const JobId & jobid, const Parametre & param, const Environnement & env)
  {
  }

  // Methode pour le controle des jobs : modifie un job en file d'attente
  void BatchManager_Local::alterJob(const JobId & jobid, const Parametre & param)
  {
    alterJob(jobid, param, Environnement());
  }

  // Methode pour le controle des jobs : modifie un job en file d'attente
  void BatchManager_Local::alterJob(const JobId & jobid, const Environnement & env)
  {
    alterJob(jobid, Parametre(), env);
  }



  // Methode pour le controle des jobs : renvoie l'etat du job
  JobInfo BatchManager_Local::queryJob(const JobId & jobid)
  {
    Id id;
    istringstream iss(jobid.getReference());
    iss >> id;

    Parametre param;
    Environnement env;

    //UNDER_LOCK( cout << "JobInfo BatchManager_Local::queryJob(const JobId & jobid) : AVANT section critique" << endl );
    // @@@ --------> SECTION CRITIQUE <-------- @@@
    pthread_mutex_lock(&_threads_mutex);
    std::map<Id, Child >::iterator pos = _threads.find(id);
    bool found = (pos != _threads.end());
    if (found) {
      param = pos->second.param;
      env   = pos->second.env;
    }
    pthread_mutex_unlock(&_threads_mutex);
    // @@@ --------> SECTION CRITIQUE <-------- @@@
    //UNDER_LOCK( cout << "JobInfo BatchManager_Local::queryJob(const JobId & jobid) : APRES section critique" << endl );

    if (!found) throw InvalidArgumentException("Invalid JobId argument for queryJob");

    JobInfo_Local ji(param, env);
    return ji;
  }



  // Methode pour le controle des jobs : teste si un job est present en machine
  bool BatchManager_Local::isRunning(const JobId & jobid)
  {
    Id id;
    istringstream iss(jobid.getReference());
    iss >> id;

    //UNDER_LOCK( cout << "JobInfo BatchManager_Local::queryJob(const JobId & jobid) : AVANT section critique" << endl );
    // @@@ --------> SECTION CRITIQUE <-------- @@@
    pthread_mutex_lock(&_threads_mutex);
    bool running = (_threads[id].param[STATE].str() == RUNNING);
    pthread_mutex_unlock(&_threads_mutex);
    // @@@ --------> SECTION CRITIQUE <-------- @@@
    //UNDER_LOCK( cout << "JobInfo BatchManager_Local::queryJob(const JobId & jobid) : APRES section critique" << endl );

    return running;
  }

  // Methode de destruction d'un job
  void BatchManager_Local::cancel(pthread_t thread_id)
  {
    pthread_mutex_lock(&_threads_mutex);
    pthread_cancel(thread_id);
    pthread_cond_wait(&_threadSyncCondition, &_threads_mutex);
    pthread_mutex_unlock(&_threads_mutex);
  }


  vector<string> BatchManager_Local::exec_command(const Parametre & param) const
  {
    ostringstream exec_sub_cmd;

#ifdef WIN32
    char drive[_MAX_DRIVE];
    _splitpath(string(param[WORKDIR]).c_str(), drive, NULL, NULL, NULL);
    if (strlen(drive) > 0) exec_sub_cmd << drive << " && ";
#endif

    exec_sub_cmd << "cd " << param[WORKDIR] << " && " << param[EXECUTABLE];

    if (param.find(ARGUMENTS) != param.end()) {
      Versatile V = param[ARGUMENTS];
      for(Versatile::const_iterator it=V.begin(); it!=V.end(); it++) {
        StringType argt = * static_cast<StringType *>(*it);
        string     arg  = argt;
        exec_sub_cmd << " " << arg;
      }
    }

    if (param.find(INFILE) != param.end()) {
      Versatile V = param[INFILE];
      for(Versatile::const_iterator it=V.begin(); it!=V.end(); it++) {
        Couple cpl = * static_cast<CoupleType*>(*it);
        string remote = cpl.getRemote();
        if (remote == "stdin")
        exec_sub_cmd << " <stdin";
      }
    }

    if (param.find(OUTFILE) != param.end()) {
      Versatile V = param[OUTFILE];
      for(Versatile::const_iterator it=V.begin(); it!=V.end(); it++) {
        Couple cpl = * static_cast<CoupleType*>(*it);
        string remote = cpl.getRemote();
        if (remote == "stdout") exec_sub_cmd << " 1>stdout";
        if (remote == "stderr") exec_sub_cmd << " 2>stderr";
      }
    }

    string user;
    Parametre::const_iterator it = param.find(USER);
    if (it != param.end()) {
      user = string(it->second);
    }

    return _protocol.getExecCommandArgs(exec_sub_cmd.str(), param[EXECUTIONHOST], user);
  }



  // Constructeur de la classe ThreadAdapter
  BatchManager_Local::ThreadAdapter::ThreadAdapter(BatchManager_Local & bm, const Job_Local & job, Id id) :
  _bm(bm), _job(job), _id(id)
  {
    // Nothing to do
  }



  // Methode d'execution du thread
  void * BatchManager_Local::ThreadAdapter::run(void * arg)
  {
    ThreadAdapter * p_ta = static_cast<ThreadAdapter *>(arg);

#ifndef WIN32
    // On bloque tous les signaux pour ce thread
    sigset_t setmask;
    sigfillset(&setmask);
    pthread_sigmask(SIG_BLOCK, &setmask, NULL);
#endif

    // On autorise la terminaison differee du thread
    // (ces valeurs sont les valeurs par defaut mais on les force par precaution)
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,  NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);

    // On enregistre la fonction de suppression du fils en cas d'arret du thread
    // Cette fontion sera automatiquement appelee lorsqu'une demande d'annulation
    // sera prise en compte par pthread_testcancel()
    Process child;
    pthread_cleanup_push(BatchManager_Local::setFailedOnCancel, arg);
    pthread_cleanup_push(BatchManager_Local::kill_child_on_exit, static_cast<void *> (&child));
    pthread_cleanup_push(BatchManager_Local::delete_on_exit, arg);


    // Le code retour cumule (ORed) de tous les appels
    // Nul en cas de reussite de l'ensemble des operations
    int rc = 0;

    // Cette table contient la liste des fichiers a detruire a la fin du processus
    std::vector<string> files_to_delete;



    // On copie les fichiers d'entree pour le fils
    const Parametre param   = p_ta->_job.getParametre();
    Parametre::const_iterator it;

    // On initialise la variable workdir a la valeur du Current Working Directory
    char * cwd =
#ifdef WIN32
      _getcwd(NULL, 0);
#else
      new char [PATH_MAX];
    getcwd(cwd, PATH_MAX);
#endif
    string workdir = cwd;
    delete [] cwd;

    if ( (it = param.find(WORKDIR)) != param.end() ) {
      workdir = static_cast<string>( (*it).second );
    }

    string executionhost = string(param[EXECUTIONHOST]);
    string user;
    if ( (it = param.find(USER)) != param.end() ) {
      user = string(it->second);
    }

    if ( (it = param.find(INFILE)) != param.end() ) {
      Versatile V = (*it).second;
      Versatile::iterator Vit;

      for(Vit=V.begin(); Vit!=V.end(); Vit++) {
        CoupleType cpt  = *static_cast< CoupleType * >(*Vit);
        Couple cp       = cpt;
        string local    = cp.getLocal();
        string remote   = cp.getRemote();

	std::cerr << workdir << std::endl;
	std::cerr << remote << std::endl;

        int status = p_ta->getBatchManager().getProtocol().copyFile(local, "", "",
                                                                    workdir + "/" + remote,
                                                                    executionhost, user);
        if (status) {
          // Echec de la copie
          rc |= 1;
        } else {
          // On enregistre le fichier comme etant a detruire
          files_to_delete.push_back(workdir + "/" + remote);
        }

      }
    }




    // On forke/exec un nouveau process pour pouvoir controler le fils
    // (plus finement qu'avec un appel system)
    // int rc = system(commande.c_str());
    //char *const parmList[] = {"/usr/bin/ssh", "localhost", "-l", "aribes", "sleep 10 && echo end", NULL};
    //execv("/usr/bin/ssh", parmList);
#ifdef WIN32
    child = p_ta->launchWin32ChildProcess();
    p_ta->pere(child);
#else
    child = fork();
    if (child < 0) { // erreur
      UNDER_LOCK( cerr << "Fork impossible (rc=" << child << ")" << endl );

    } else if (child > 0) { // pere
      p_ta->pere(child);

    } else { // fils
      p_ta->fils();
    }
#endif


    // On copie les fichiers de sortie du fils
    if ( (it = param.find(OUTFILE)) != param.end() ) {
      Versatile V = (*it).second;
      Versatile::iterator Vit;

      for(Vit=V.begin(); Vit!=V.end(); Vit++) {
        CoupleType cpt  = *static_cast< CoupleType * >(*Vit);
        Couple cp       = cpt;
        string local    = cp.getLocal();
        string remote   = cp.getRemote();

        int status = p_ta->getBatchManager().getProtocol().copyFile(workdir + "/" + remote,
                                                                    executionhost, user,
                                                                    local, "", "");
        if (status) {
          // Echec de la copie
          rc |= 1;
        } else {
          // On enregistre le fichier comme etant a detruire
          files_to_delete.push_back(workdir + "/" + remote);
        }

      }
    }

    // On efface les fichiers d'entree et de sortie du fils si les copies precedentes ont reussi
    // ou si la creation du fils n'a pu avoir lieu
    if ( (rc == 0) || (child < 0) ) {
      std::vector<string>::const_iterator it;
      for(it=files_to_delete.begin(); it!=files_to_delete.end(); it++) {
        p_ta->getBatchManager().getProtocol().removeFile(*it, executionhost, user);
/*        string remove_cmd = p_ta->getBatchManager().remove_command(user, executionhost, *it);
        UNDER_LOCK( cout << "Removing : " << remove_cmd << endl );
#ifdef WIN32
        remove_cmd = string("\"") + remove_cmd + string("\"");
#endif
        system(remove_cmd.c_str());*/
      }
    }

    pthread_mutex_lock(&p_ta->_bm._threads_mutex);

    // Set the job state to FINISHED or FAILED
    p_ta->_bm._threads[p_ta->_id].param[STATE] = (p_ta->_bm._threads[p_ta->_id].hasFailed) ? FAILED : FINISHED;

    // On retire la fonction de nettoyage de la memoire
    pthread_cleanup_pop(0);

    // On retire la fonction de suppression du fils
    pthread_cleanup_pop(0);

    // remove setFailedOnCancel function from cancel stack
    pthread_cleanup_pop(0);

    pthread_mutex_unlock(&p_ta->_bm._threads_mutex);

    // On invoque la fonction de nettoyage de la memoire
    delete_on_exit(arg);

    UNDER_LOCK( cout << "Father is leaving" << endl );
    pthread_exit(NULL);
    return NULL;
  }




  void BatchManager_Local::ThreadAdapter::pere(Process child)
  {
    time_t child_starttime = time(NULL);

    // On enregistre le fils dans la table des threads
    pthread_t thread_id = pthread_self();

    Parametre param   = _job.getParametre();
    Environnement env = _job.getEnvironnement();

    ostringstream id_sst;
    id_sst << _id;
    param[ID]         = id_sst.str();
    param[STATE]      = Batch::RUNNING;
#ifndef WIN32
    param[PID]        = child;
#endif

    _bm._threads[_id].thread_id = thread_id;
#ifndef WIN32
    _bm._threads[_id].pid       = child;
#endif
    _bm._threads[_id].hasFailed = false;
    _bm._threads[_id].param     = param;
    _bm._threads[_id].env       = env;
    _bm._threads[_id].command_queue.push(NOP);

    // Unlock the master thread. From here, all shared variables must be protected
    // from concurrent access
    pthread_cond_signal(&_bm._threadSyncCondition);


    // on boucle en attendant que le fils ait termine
    while (1) {
#ifdef WIN32
      DWORD exitCode;
      BOOL res = GetExitCodeProcess(child, &exitCode);
      if (exitCode != STILL_ACTIVE) {
        UNDER_LOCK( cout << "Father sees his child is DONE: exit code = " << exitCode << endl );
        break;
      }
#else
      int child_rc = 0;
      pid_t child_wait_rc = waitpid(child, &child_rc, WNOHANG /* | WUNTRACED */);
      if (child_wait_rc > 0) {
	 UNDER_LOCK( cout << "Status is: " << WIFEXITED( child_rc) << endl);
	 UNDER_LOCK( cout << "Status is: " << WEXITSTATUS( child_rc) << endl);
	 UNDER_LOCK( cout << "Status is: " << WIFSIGNALED( child_rc) << endl);
	 UNDER_LOCK( cout << "Status is: " << WTERMSIG( child_rc) << endl);
	 UNDER_LOCK( cout << "Status is: " << WCOREDUMP( child_rc) << endl);
	 UNDER_LOCK( cout << "Status is: " << WIFSTOPPED( child_rc) << endl);
	 UNDER_LOCK( cout << "Status is: " << WSTOPSIG( child_rc) << endl);
#ifdef WIFCONTINUED
	 UNDER_LOCK( cout << "Status is: " << WIFCONTINUED( child_rc) << endl); // not compilable on sarge
#endif
        if (WIFSTOPPED(child_rc)) {
          // NOTA : pour rentrer dans cette section, il faut que le flag WUNTRACED
          // soit positionne dans l'appel a waitpid ci-dessus. Ce flag est couramment
          // desactive car s'il est possible de detecter l'arret d'un process, il est
          // plus difficile de detecter sa reprise.

          // Le fils est simplement stoppe
          // @@@ --------> SECTION CRITIQUE <-------- @@@
          pthread_mutex_lock(&_bm._threads_mutex);
          _bm._threads[_id].param[STATE] = Batch::PAUSED;
          pthread_mutex_unlock(&_bm._threads_mutex);
          // @@@ --------> SECTION CRITIQUE <-------- @@@
          UNDER_LOCK( cout << "Father sees his child is STOPPED : " << child_wait_rc << endl );

        }
        else {
          // Le fils est termine, on sort de la boucle et du if englobant
          UNDER_LOCK( cout << "Father sees his child is DONE : " << child_wait_rc << " (child_rc=" << (WIFEXITED(child_rc) ? WEXITSTATUS(child_rc) : -1) << ")" << endl );
          break;
        }
      }
      else if (child_wait_rc == -1) {
        // Le fils a disparu ...
        // @@@ --------> SECTION CRITIQUE <-------- @@@
        pthread_mutex_lock(&_bm._threads_mutex);
        _bm._threads[_id].hasFailed = true;
        pthread_mutex_unlock(&_bm._threads_mutex);
        // @@@ --------> SECTION CRITIQUE <-------- @@@
        UNDER_LOCK( cout << "Father sees his child is DEAD : " << child_wait_rc << " (Reason : " << strerror(errno) << ")" << endl );
        break;
      }
#endif

      // On teste si le thread doit etre detruit
      pthread_testcancel();



      // On regarde si le fils n'a pas depasse son temps (wallclock time)
      time_t child_currenttime = time(NULL);
      time_t child_elapsedtime = child_currenttime - child_starttime;
      if (param.find(MAXWALLTIME) != param.end()) {
        int maxwalltime = param[MAXWALLTIME];
        // 	  cout << "child_starttime          = " << child_starttime        << endl
        // 	       << "child_currenttime        = " << child_currenttime      << endl
        // 	       << "child_elapsedtime        = " << child_elapsedtime      << endl
        // 	       << "maxwalltime              = " << maxwalltime            << endl
        // 	       << "int(maxwalltime * 1.1)   = " << int(maxwalltime * 1.1) << endl;
        if (child_elapsedtime > int(maxwalltime * 1.1) ) { // On se donne 10% de marge avant le KILL
          UNDER_LOCK( cout << "Father is sending KILL command to the thread " << _id << endl );
          // On introduit une commande dans la queue du thread
          // @@@ --------> SECTION CRITIQUE <-------- @@@
          pthread_mutex_lock(&_bm._threads_mutex);
          if (_bm._threads.find(_id) != _bm._threads.end())
            _bm._threads[_id].command_queue.push(KILL);
          pthread_mutex_unlock(&_bm._threads_mutex);
          // @@@ --------> SECTION CRITIQUE <-------- @@@


        } else if (child_elapsedtime > maxwalltime ) {
          UNDER_LOCK( cout << "Father is sending TERM command to the thread " << _id << endl );
          // On introduit une commande dans la queue du thread
          // @@@ --------> SECTION CRITIQUE <-------- @@@
          pthread_mutex_lock(&_bm._threads_mutex);
          if (_bm._threads.find(_id) != _bm._threads.end())
            _bm._threads[_id].command_queue.push(TERM);
          pthread_mutex_unlock(&_bm._threads_mutex);
          // @@@ --------> SECTION CRITIQUE <-------- @@@
        }
      }



      // On regarde s'il y a quelque chose a faire dans la queue de commande
      // @@@ --------> SECTION CRITIQUE <-------- @@@
      pthread_mutex_lock(&_bm._threads_mutex);
      if (_bm._threads.find(_id) != _bm._threads.end()) {
        while (_bm._threads[_id].command_queue.size() > 0) {
          Commande cmd = _bm._threads[_id].command_queue.front();
          _bm._threads[_id].command_queue.pop();

          switch (cmd) {
    case NOP:
      UNDER_LOCK( cout << "Father does nothing to his child" << endl );
      break;
#ifndef WIN32
    case HOLD:
      UNDER_LOCK( cout << "Father is sending SIGSTOP signal to his child" << endl );
      kill(child, SIGSTOP);
      break;

    case RELEASE:
      UNDER_LOCK( cout << "Father is sending SIGCONT signal to his child" << endl );
      kill(child, SIGCONT);
      break;

    case TERM:
      UNDER_LOCK( cout << "Father is sending SIGTERM signal to his child" << endl );
      kill(child, SIGTERM);
      break;

    case KILL:
      UNDER_LOCK( cout << "Father is sending SIGKILL signal to his child" << endl );
      kill(child, SIGKILL);
      break;
#endif
    case ALTER:
      break;

    default:
      break;
          }
        }

      }
      pthread_mutex_unlock(&_bm._threads_mutex);
      // @@@ --------> SECTION CRITIQUE <-------- @@@

      // On fait une petite pause pour ne pas surcharger inutilement le processeur
#ifdef WIN32
      Sleep(1000);
#else
      sleep(1);
#endif

    }

  }



#ifndef WIN32

  void BatchManager_Local::ThreadAdapter::fils()
  {
    Parametre param = _job.getParametre();
    Parametre::iterator it;

      //char *const parmList[] = {"/usr/bin/ssh", "localhost", "-l", "aribes", "sleep 1 && echo end", NULL};
      //int result = execv("/usr/bin/ssh", parmList);
      //UNDER_LOCK( cout << "*** debug_command = " << result << endl );
      //UNDER_LOCK( cout << "*** debug_command = " << strerror(errno) << endl );
    try {

      // EXECUTABLE is MANDATORY, if missing, we exit with failure notification
      vector<string> command;
      if (param.find(EXECUTABLE) != param.end()) {
        command = _bm.exec_command(param);
      } else exit(1);

      // Build the argument array argv from the command
      char ** argv = new char * [command.size() + 1];
      string comstr;
      for (string::size_type i=0 ; i<command.size() ; i++) {
        argv[i] = new char[command[i].size() + 1];
        strncpy(argv[i], command[i].c_str(), command[i].size() + 1);
        if (i>0) comstr += " # ";
        comstr += command[i];
      }

      argv[command.size()] = NULL;

      UNDER_LOCK( cout << "*** debug_command = " << comstr << endl );
      UNDER_LOCK( cout << "*** debug_command = " << argv[0] << endl );

      // Create the environment for the new process. Note (RB): Here we change the environment for
      // the process launched in local. It would seem more logical to set the environment for the
      // remote process.
      Environnement env = _job.getEnvironnement();

      char ** envp = NULL;
      if(env.size() > 0) {
        envp = new char * [env.size() + 1]; // 1 pour le NULL terminal
        int i = 0;
        for(Environnement::const_iterator it=env.begin(); it!=env.end(); it++, i++) {
          const string  & key   = (*it).first;
          const string  & value = (*it).second;
          ostringstream oss;
          oss << key << "=" << value;
          envp[i]         = new char [oss.str().size() + 1];
          strncpy(envp[i], oss.str().c_str(), oss.str().size() + 1);
        }

        // assert (i == env.size())
        envp[i] = NULL;
      }

      //char *const parmList[] = {"/usr/bin/ssh", "localhost", "-l", "aribes", "sleep 1 && echo end", NULL};
      //int result = execv("/usr/bin/ssh", parmList);
      //UNDER_LOCK( cout << "*** debug_command = " << result << endl );
      //UNDER_LOCK( cout << "*** debug_command = " << strerror(errno) << endl );



      // On positionne les limites systeme imposees au fils
      if (param.find(MAXCPUTIME) != param.end()) {
        int maxcputime = param[MAXCPUTIME];
        struct rlimit limit;
        limit.rlim_cur = maxcputime;
        limit.rlim_max = int(maxcputime * 1.1);
        setrlimit(RLIMIT_CPU, &limit);
      }

      if (param.find(MAXDISKSIZE) != param.end()) {
        int maxdisksize = param[MAXDISKSIZE];
        struct rlimit limit;
        limit.rlim_cur = maxdisksize * 1024;
        limit.rlim_max = int(maxdisksize * 1.1) * 1024;
        setrlimit(RLIMIT_FSIZE, &limit);
      }

      if (param.find(MAXRAMSIZE) != param.end()) {
        int maxramsize = param[MAXRAMSIZE];
        struct rlimit limit;
        limit.rlim_cur = maxramsize * 1024 * 1024;
        limit.rlim_max = int(maxramsize * 1.1) * 1024 * 1024;
        setrlimit(RLIMIT_AS, &limit);
      }


      //char *const parmList[] = {"/usr/bin/ssh", "localhost", "-l", "aribes", "sleep 1 && echo end", NULL};
      //int result = execv("/usr/bin/ssh", parmList);
      //UNDER_LOCK( cout << "*** debug_command = " << result << endl );
      //UNDER_LOCK( cout << "*** debug_command = " << strerror(errno) << endl );

      // On cree une session pour le fils de facon a ce qu'il ne soit pas
      // detruit lorsque le shell se termine (le shell ouvre une session et
      // tue tous les process appartenant a la session en quittant)
      setsid();


      // On ferme les descripteurs de fichiers standards
      //close(STDIN_FILENO);
      //close(STDOUT_FILENO);
      //close(STDERR_FILENO);


      // On execute la commande du fils
      int result = execve(argv[0], argv, envp);
      UNDER_LOCK( cout << "*** debug_command = " << strerror(errno) << endl );
      // No need to deallocate since nothing happens after a successful exec

      // Normalement on ne devrait jamais arriver ici
      ofstream file_err("error.log");
      UNDER_LOCK( file_err << "Echec de l'appel a execve" << endl );

    } catch (GenericException & e) {

      std::cerr << "Caught exception : " << e.type << " : " << e.message << std::endl;
    }

    exit(99);
  }

#else

  BatchManager_Local::Process BatchManager_Local::ThreadAdapter::launchWin32ChildProcess()
  {
    Parametre param = _job.getParametre();
    Parametre::iterator it;
    PROCESS_INFORMATION pi;

    try {

      // EXECUTABLE is MANDATORY, if missing, we throw an exception
      vector<string> exec_command;
      if (param.find(EXECUTABLE) != param.end()) {
        exec_command = _bm.exec_command(param);
      } else {
        throw RunTimeException("Parameter \"EXECUTABLE\" is mandatory for local batch submission");
      }

      // Build the command string from the command argument vector
      string comstr;
      for (unsigned int i=0 ; i<exec_command.size() ; i++) {
        if (i>0) comstr += " ";
        comstr += exec_command[i];
      }

      UNDER_LOCK( cout << "*** debug_command = " << comstr << endl );

      // Create the environment for the new process. Note (RB): Here we change the environment for
      // the process launched in local. It would seem more logical to set the environment for the
      // remote process.
      // Note that if no environment is specified, we reuse the current environment.
      Environnement env = _job.getEnvironnement();
      char * chNewEnv = NULL;

      if(env.size() > 0) {
        chNewEnv = new char[4096];
        LPTSTR lpszCurrentVariable = chNewEnv;
        for(Environnement::const_iterator it=env.begin() ; it!=env.end() ; it++) {
          const string  & key   = (*it).first;
          const string  & value = (*it).second;
          string envvar = key + "=" + value;
          envvar.copy(lpszCurrentVariable, envvar.size());
          lpszCurrentVariable[envvar.size()] = '\0';
          lpszCurrentVariable += lstrlen(lpszCurrentVariable) + 1;
        }
        // Terminate the block with a NULL byte.
        *lpszCurrentVariable = '\0';
      }


      STARTUPINFO si;
      ZeroMemory( &si, sizeof(si) );
      si.cb = sizeof(si);
      ZeroMemory( &pi, sizeof(pi) );

      // Copy the command to a non-const buffer
      char * buffer = strdup(comstr.c_str());

      // launch the new process
      BOOL res = CreateProcess(NULL, buffer, NULL, NULL, FALSE,
                               CREATE_NO_WINDOW, chNewEnv, NULL, &si, &pi);

      if (buffer) free(buffer);
      if (!res) throw RunTimeException("Error while creating new process");

      CloseHandle(pi.hThread);

    } catch (GenericException & e) {

      std::cerr << "Caught exception : " << e.type << " : " << e.message << std::endl;
    }

    return pi.hProcess;
  }

#endif


  void BatchManager_Local::kill_child_on_exit(void * p_pid)
  {
#ifndef WIN32
    //TODO: porting of following functionality
    pid_t child = * static_cast<pid_t *>(p_pid);

    // On tue le fils
    kill(child, SIGTERM);

    // Nota : on pourrait aussi faire a la suite un kill(child, SIGKILL)
    // mais cette option n'est pas implementee pour le moment, car il est
    // preferable de laisser le process fils se terminer normalement et seul.
#endif
  }

  void BatchManager_Local::delete_on_exit(void * arg)
  {
    ThreadAdapter * p_ta = static_cast<ThreadAdapter *>(arg);
    delete p_ta;
  }

  void BatchManager_Local::setFailedOnCancel(void * arg)
  {
    ThreadAdapter * p_ta = static_cast<ThreadAdapter *>(arg);
    pthread_mutex_lock(&p_ta->getBatchManager()._threads_mutex);
    p_ta->getBatchManager()._threads[p_ta->getId()].param[STATE] = FAILED;
    pthread_mutex_unlock(&p_ta->getBatchManager()._threads_mutex);

    // Unlock the master thread. From here, the batch manager instance should not be used.
    pthread_cond_signal(&p_ta->getBatchManager()._threadSyncCondition);
  }

}
