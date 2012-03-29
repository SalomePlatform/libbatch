//  Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
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
 * BatchManager_eCCC.cxx : emulation of CCC client for CCRT machines
 *
 * Auteur : Bernard SECHER - CEA DEN
 * Mail   : mailto:bernard.secher@cea.fr
 * Date   : Thu Apr 24 10:17:22 2010
 * Projet : PAL Salome
 *
 */

#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <sys/stat.h>

#include <stdlib.h>
#include <string.h>

#ifdef WIN32
#include <io.h>
#else
#include <libgen.h>
#endif

#include "Batch_Constants.hxx"
#include "Batch_BatchManager_eCCC.hxx"
#include "Batch_JobInfo_eCCC.hxx"

using namespace std;

namespace Batch {

  BatchManager_eCCC::BatchManager_eCCC(const FactBatchManager * parent, const char * host,
                                       const char * username,
                                       CommunicationProtocolType protocolType, const char * mpiImpl)
  : BatchManager(parent, host),
    BatchManager_eClient(parent, host, username, protocolType, mpiImpl)
  {
    // Nothing to do
  }

  // Destructeur
  BatchManager_eCCC::~BatchManager_eCCC()
  {
    // Nothing to do
  }

  // Methode pour le controle des jobs : soumet un job au gestionnaire
  const JobId BatchManager_eCCC::submitJob(const Job & job)
  {
    int status;
    Parametre params = job.getParametre();
    const std::string workDir = params[WORKDIR];
    const string fileToExecute = params[EXECUTABLE];
    string::size_type p1 = fileToExecute.find_last_of("/");
    string::size_type p2 = fileToExecute.find_last_of(".");
    std::string fileNameToExecute = fileToExecute.substr(p1+1,p2-p1-1);

    // export input files on cluster
    cerr << "Export des fichiers en entree" << endl;
    exportInputFiles(job);

    // build batch script for job
    cerr << "Construction du script de batch" << endl;
    buildBatchScript(job);
    cerr << "Script envoye" << endl;

    // define name of log file (local)
    string logFile = generateTemporaryFileName("CCC-submitlog");

    // define command to submit batch
    string subCommand = string("bash -l -c \"cd ") + workDir + "; ccc_msub " + fileNameToExecute + "_Batch.sh\"";
    string command = _protocol.getExecCommand(subCommand, _hostname, _username);
    command += " > ";
    command += logFile;
    command += " 2>&1";
    cerr << command.c_str() << endl;
    status = system(command.c_str());
    if(status)
    {
      ifstream error_message(logFile.c_str());
      std::string mess;
      std::string temp;
      while(std::getline(error_message, temp))
	mess += temp;
      error_message.close();
      throw EmulationException("Error of connection on remote host, error was: " + mess);
    }

    // read id of submitted job in log file
    ifstream idfile(logFile.c_str());
    string sidj;
    idfile >> sidj;
    idfile >> sidj;
    idfile >> sidj;
    idfile >> sidj;
    idfile.close();
    if (sidj.size() == 0)
      throw EmulationException("Error in the submission of the job on the remote host");

    JobId id(this, sidj);
    return id;
  }

  // Ce manager permet de faire de la reprise
  const Batch::JobId
  BatchManager_eCCC::addJob(const Batch::Job & job, const std::string reference)
  {
    return JobId(this, reference);
  }

  // Methode pour le controle des jobs : retire un job du gestionnaire
  void BatchManager_eCCC::deleteJob(const JobId & jobid)
  {
    int status;
    int ref;
    istringstream iss(jobid.getReference());
    iss >> ref;

    // define command to delete batch
    string subCommand = string("bash -l -c \"ccc_mdel ") + iss.str() + string("\"");
    string command = _protocol.getExecCommand(subCommand, _hostname, _username);
    cerr << command.c_str() << endl;
    status = system(command.c_str());
    if (status)
      throw EmulationException("Error of connection on remote host");

    cerr << "jobId = " << ref << "killed" << endl;
  }

  // Methode pour le controle des jobs : suspend un job en file d'attente
  void BatchManager_eCCC::holdJob(const JobId & jobid)
  {
    throw EmulationException("Not yet implemented");
  }

  // Methode pour le controle des jobs : relache un job suspendu
  void BatchManager_eCCC::releaseJob(const JobId & jobid)
  {
    throw EmulationException("Not yet implemented");
  }


  // Methode pour le controle des jobs : modifie un job en file d'attente
  void BatchManager_eCCC::alterJob(const JobId & jobid, const Parametre & param, const Environnement & env)
  {
    throw EmulationException("Not yet implemented");
  }

  // Methode pour le controle des jobs : modifie un job en file d'attente
  void BatchManager_eCCC::alterJob(const JobId & jobid, const Parametre & param)
  {
    alterJob(jobid, param, Environnement());
  }

  // Methode pour le controle des jobs : modifie un job en file d'attente
  void BatchManager_eCCC::alterJob(const JobId & jobid, const Environnement & env)
  {
    alterJob(jobid, Parametre(), env);
  }

  // Methode pour le controle des jobs : renvoie l'etat du job
  JobInfo BatchManager_eCCC::queryJob(const JobId & jobid)
  {
    int id;
    istringstream iss(jobid.getReference());
    iss >> id;

    // define name of log file (local)
    string logFile = generateTemporaryFileName(string("CCC-querylog-id") + jobid.getReference());

    // define command to query batch
    string subCommand = string("bash -l -c \"bjobs ") + iss.str() + string("\"");
    string command = _protocol.getExecCommand(subCommand, _hostname, _username);
    command += " > ";
    command += logFile;
    cerr << command.c_str() << endl;
    int status = system(command.c_str());
    if (status)
      throw EmulationException("Error of connection on remote host");

    JobInfo_eCCC ji = JobInfo_eCCC(id,logFile);
    return ji;
  }



  // Methode pour le controle des jobs : teste si un job est present en machine
  bool BatchManager_eCCC::isRunning(const JobId & jobid)
  {
    throw EmulationException("Not yet implemented");
  }

  void BatchManager_eCCC::buildBatchScript(const Job & job)
  {
#ifndef WIN32 //TODO: need for porting on Windows
    Parametre params = job.getParametre();

    // Job Parameters
    string workDir       = "";
    string fileToExecute = "";
    int nbproc		 = 0;
    int edt		 = 0;
    int mem              = 0;
    string queue         = "";

    // Mandatory parameters
    if (params.find(WORKDIR) != params.end()) 
      workDir = params[WORKDIR].str();
    else 
      throw EmulationException("params[WORKDIR] is not defined ! Please defined it, cannot submit this job");
    if (params.find(EXECUTABLE) != params.end()) 
      fileToExecute = params[EXECUTABLE].str();
    else 
      throw EmulationException("params[EXECUTABLE] is not defined ! Please defined it, cannot submit this job");

    // Optional parameters
    if (params.find(NBPROC) != params.end()) 
      nbproc = params[NBPROC];
    if (params.find(MAXWALLTIME) != params.end()) 
      edt = (long)params[MAXWALLTIME] * 60;
    if (params.find(MAXRAMSIZE) != params.end()) 
      mem = params[MAXRAMSIZE];
    if (params.find(QUEUE) != params.end()) 
      queue = params[QUEUE].str();

    string::size_type p1 = fileToExecute.find_last_of("/");
    string::size_type p2 = fileToExecute.find_last_of(".");
    string rootNameToExecute = fileToExecute.substr(p1+1,p2-p1-1);
    string fileNameToExecute = fileToExecute.substr(p1+1);
 
    // Create batch submit file
    ofstream tempOutputFile;
    std::string TmpFileName = createAndOpenTemporaryFile("LSF-script", tempOutputFile);

    tempOutputFile << "#!/bin/bash" << endl ;
    if (queue != "")
      tempOutputFile << "#MSUB -q " << queue << endl;
    if( edt > 0 )
      tempOutputFile << "#MSUB -T " << edt << endl ;
    if( mem > 0 )
      tempOutputFile << "#MSUB -M " << mem << endl ;
    tempOutputFile << "#MSUB -n " << nbproc << endl ;
    size_t pos = workDir.find("$HOME");
    string baseDir;
    if( pos != string::npos )
      baseDir = getHomeDir(workDir) + workDir.substr(pos+5,workDir.length()-5);
    else{
      pos = workDir.find("~");
      if( pos != string::npos )
	baseDir = getHomeDir(workDir) + workDir.substr(pos+1,workDir.length()-1);
      else
	baseDir = workDir;
    }
    tempOutputFile << "#MSUB -o " << baseDir << "/logs/output.log." << rootNameToExecute << endl ;
    tempOutputFile << "#MSUB -e " << baseDir << "/logs/error.log." << rootNameToExecute << endl ;

    tempOutputFile << "cd " << workDir << endl ;

    // generate nodes file
    tempOutputFile << "bool=0" << endl;
    tempOutputFile << "for i in $LSB_MCPU_HOSTS; do" << endl;
    tempOutputFile << "  if test $bool = 0; then" << endl;
    tempOutputFile << "    n=$i" << endl;
    tempOutputFile << "    bool=1" << endl;
    tempOutputFile << "  else" << endl;
    tempOutputFile << "    for ((j=0;j<$i;j++)); do" << endl;
    tempOutputFile << "      echo $n >> nodesFile." << rootNameToExecute << endl;
    tempOutputFile << "    done" << endl;
    tempOutputFile << "    bool=0" << endl;
    tempOutputFile << "  fi" << endl;
    tempOutputFile << "done" << endl;

    // Abstraction of PBS_NODEFILE - TODO
    tempOutputFile << "export LIBBATCH_NODEFILE=nodesFile." << rootNameToExecute << endl;

    // Allow resource sharing in CCRT nodes
    tempOutputFile << "export OMPI_MCA_orte_process_binding=none" << endl;

    // Launch the executable
    tempOutputFile << "./" + fileNameToExecute << endl;
    tempOutputFile.flush();
    tempOutputFile.close();

    BATCH_CHMOD(TmpFileName.c_str(), 0x1ED);
    cerr << "Batch script file generated is: " << TmpFileName.c_str() << endl;

    int status = _protocol.copyFile(TmpFileName, "", "",
                                    workDir + "/" + rootNameToExecute + "_Batch.sh",
                                    _hostname, _username);
    if (status)
      throw EmulationException("Error of connection on remote host");

#endif

  }

  std::string BatchManager_eCCC::getHomeDir(std::string tmpdir)
  {
    std::string home;
    int idx = tmpdir.find("Batch/");
    std::string filelogtemp = tmpdir.substr(idx+6, tmpdir.length());
    filelogtemp = "/tmp/logs" + filelogtemp + "_home";

    string subCommand = string("echo $HOME");
    string command = _protocol.getExecCommand(subCommand, _hostname, _username) + " > " + filelogtemp;
    cerr << command.c_str() << endl;
    int status = system(command.c_str());
    if (status)
      throw EmulationException("Error of launching home command on remote host");

    std::ifstream file_home(filelogtemp.c_str());
    std::getline(file_home, home);
    file_home.close();
    return home;
  }

}
