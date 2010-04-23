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
 * BatchManager_ePBS.cxx : emulation of PBS client
 *
 * Auteur : Bernard SECHER - CEA DEN, André RIBES - EDF R&D
 * Mail   : mailto:bernard.secher@cea.fr
 * Date   : Thu Apr 24 10:17:22 2008
 * Projet : PAL Salome
 *
 */

#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/stat.h>

#include <stdlib.h>
#include <string.h>
#include <Batch_config.h>

#ifdef MSVC
#include <io.h>
#else
#include <libgen.h>
#endif

#include "Batch_BatchManager_ePBS.hxx"
#include "Batch_JobInfo_ePBS.hxx"

using namespace std;

namespace Batch {

  BatchManager_ePBS::BatchManager_ePBS(const FactBatchManager * parent, const char * host,
                                       CommunicationProtocolType protocolType, const char * mpiImpl, 
				       int nb_proc_per_node)
    : BatchManager_eClient(parent, host, protocolType, mpiImpl),
    BatchManager(parent, host)
  {
    // Nothing to do
    _nb_proc_per_node = nb_proc_per_node;
  }

  // Destructeur
  BatchManager_ePBS::~BatchManager_ePBS()
  {
    // Nothing to do
  }

  // Methode pour le controle des jobs : soumet un job au gestionnaire
  const JobId BatchManager_ePBS::submitJob(const Job & job)
  {
    int status;
    Parametre params = job.getParametre();
    const std::string workDir = params[WORKDIR];
    const string fileToExecute = params[EXECUTABLE];
    string::size_type p1 = fileToExecute.find_last_of("/");
    string::size_type p2 = fileToExecute.find_last_of(".");
    std::string fileNameToExecute = fileToExecute.substr(p1+1,p2-p1-1);

    // export input files on cluster
    exportInputFiles(job);

    // build batch script for job
    buildBatchScript(job);

    // define name of log file (local)
    string logFile = generateTemporaryFileName("PBS-submitlog");

    // define command to submit batch
    string subCommand = string("cd ") + workDir + "; qsub " + fileNameToExecute + "_Batch.sh";
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
    string sline;
    idfile >> sline;
    idfile.close();
    if (sline.size() == 0)
      throw EmulationException("Error in the submission of the job on the remote host");

    JobId id(this, sline);
    return id;
  }

  // Methode pour le controle des jobs : retire un job du gestionnaire
  void BatchManager_ePBS::deleteJob(const JobId & jobid)
  {
    int status;
    int ref;
    istringstream iss(jobid.getReference());
    iss >> ref;

    // define command to delete batch
    string subCommand = string("qdel ") + iss.str();
    string command = _protocol.getExecCommand(subCommand, _hostname, _username);
    cerr << command.c_str() << endl;
    status = system(command.c_str());
    if (status)
      throw EmulationException("Error of connection on remote host");

    cerr << "jobId = " << ref << "killed" << endl;
  }

  // Methode pour le controle des jobs : suspend un job en file d'attente
  void BatchManager_ePBS::holdJob(const JobId & jobid)
  {
    throw EmulationException("Not yet implemented");
  }

  // Methode pour le controle des jobs : relache un job suspendu
  void BatchManager_ePBS::releaseJob(const JobId & jobid)
  {
    throw EmulationException("Not yet implemented");
  }


  // Methode pour le controle des jobs : modifie un job en file d'attente
  void BatchManager_ePBS::alterJob(const JobId & jobid, const Parametre & param, const Environnement & env)
  {
    throw EmulationException("Not yet implemented");
  }

  // Methode pour le controle des jobs : modifie un job en file d'attente
  void BatchManager_ePBS::alterJob(const JobId & jobid, const Parametre & param)
  {
    alterJob(jobid, param, Environnement());
  }

  // Methode pour le controle des jobs : modifie un job en file d'attente
  void BatchManager_ePBS::alterJob(const JobId & jobid, const Environnement & env)
  {
    alterJob(jobid, Parametre(), env);
  }

  // Methode pour le controle des jobs : renvoie l'etat du job
  JobInfo BatchManager_ePBS::queryJob(const JobId & jobid)
  {
    int id;
    istringstream iss(jobid.getReference());
    iss >> id;

    // define name of log file (local)
    string logFile = generateTemporaryFileName(string("PBS-querylog-id") + jobid.getReference());

    // define command to query batch
    string subCommand = string("qstat -f ") + iss.str();
    string command = _protocol.getExecCommand(subCommand, _hostname, _username);
    command += " > ";
    command += logFile;
    cerr << command.c_str() << endl;
    int status = system(command.c_str());
    if(status && status != 153 && status != 256*153)
      throw EmulationException("Error of connection on remote host");

    JobInfo_ePBS ji = JobInfo_ePBS(id,logFile);
    return ji;
  }

  // Methode pour le controle des jobs : teste si un job est present en machine
  bool BatchManager_ePBS::isRunning(const JobId & jobid)
  {
    throw EmulationException("Not yet implemented");
  }

  void BatchManager_ePBS::buildBatchScript(const Job & job)
  {
    std::cerr << "BuildBatchScript" << std::endl;
    Parametre params = job.getParametre();
    Environnement env = job.getEnvironnement();

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
      edt = params[MAXWALLTIME];
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
    std::string TmpFileName = createAndOpenTemporaryFile("PBS-script", tempOutputFile);

    tempOutputFile << "#! /bin/sh -f" << endl;
    if (nbproc > 0)
    {
      // Division - arrondi supérieur
      int nodes_requested = (nbproc + _nb_proc_per_node -1) / _nb_proc_per_node;
      tempOutputFile << "#PBS -l nodes=" << nodes_requested << ":ppn=" << _nb_proc_per_node << endl;
    }
    if (queue != "")
      tempOutputFile << "#PBS -q " << queue << endl;
    if( edt > 0 )
      tempOutputFile << "#PBS -l walltime=" << edt*60 << endl;
    if( mem > 0 )
      tempOutputFile << "#PBS -l mem=" << mem << "kb" << endl;
    tempOutputFile << "#PBS -o " << workDir << "/logs/output.log." << rootNameToExecute << endl;
    tempOutputFile << "#PBS -e " << workDir << "/logs/error.log."  << rootNameToExecute << endl;

    // Define environment for the job
    if (!env.empty()) {
      tempOutputFile << "#PBS -v ";
      Environnement::const_iterator iter;
      for (iter = env.begin() ; iter != env.end() ; ++iter) {
        tempOutputFile << iter->first << "=" << iter->second << ",";
      }
      tempOutputFile << endl;
    }

    // Abstraction of PBS_NODEFILE - TODO
    tempOutputFile << "export LIBBATCH_NODEFILE=$PBS_NODEFILE" << endl;

    // Launch the executable
    tempOutputFile << "cd " << workDir << endl;
    tempOutputFile << "./" + fileNameToExecute << endl;
    tempOutputFile.flush();
    tempOutputFile.close();

    BATCH_CHMOD(TmpFileName.c_str(), 0x1ED);
    cerr << "Batch script file generated is: " << TmpFileName.c_str() << endl;

    int status = _protocol.copyFile(TmpFileName, "", "",
                                    workDir + "/" + rootNameToExecute + "_Batch.sh",
                                    _hostname, _username);
    if (status)
      throw EmulationException("Error of connection on remote host, cannot copy batch submission file");
  }
}
