//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
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
 * BatchManager_eSGE.cxx : emulation of SGE client
 *
 * Auteur : Bernard SECHER - CEA DEN
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

#ifdef WIN32
#include <io.h>
#else
#include <libgen.h>
#endif

#include "Batch_BatchManager_eSGE.hxx"
#include "Batch_JobInfo_eSGE.hxx"

using namespace std;

namespace Batch {

  BatchManager_eSGE::BatchManager_eSGE(const FactBatchManager * parent, const char * host,
                                       CommunicationProtocolType protocolType, const char * mpiImpl)
  : BatchManager_eClient(parent, host, protocolType, mpiImpl),
    BatchManager(parent, host)
  {
    // Nothing to do
  }

  // Destructeur
  BatchManager_eSGE::~BatchManager_eSGE()
  {
    // Nothing to do
  }

  // Methode pour le controle des jobs : soumet un job au gestionnaire
  const JobId BatchManager_eSGE::submitJob(const Job & job)
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
    string logFile = generateTemporaryFileName("SGE-submitlog");

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
    char line[128];
    FILE *fp = fopen(logFile.c_str(),"r");
    fgets( line, 128, fp);
    fclose(fp);

    string strjob;
    istringstream iss(line);
    iss >> strjob >> strjob >> strjob;

    JobId id(this, strjob);
    return id;
  }

  // Methode pour le controle des jobs : retire un job du gestionnaire
  void BatchManager_eSGE::deleteJob(const JobId & jobid)
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
    if(status)
      throw EmulationException("Error of connection on remote host");

    cerr << "jobId = " << ref << "killed" << endl;
  }

  // Methode pour le controle des jobs : suspend un job en file d'attente
  void BatchManager_eSGE::holdJob(const JobId & jobid)
  {
    throw EmulationException("Not yet implemented");
  }

  // Methode pour le controle des jobs : relache un job suspendu
  void BatchManager_eSGE::releaseJob(const JobId & jobid)
  {
    throw EmulationException("Not yet implemented");
  }


  // Methode pour le controle des jobs : modifie un job en file d'attente
  void BatchManager_eSGE::alterJob(const JobId & jobid, const Parametre & param, const Environnement & env)
  {
    throw EmulationException("Not yet implemented");
  }

  // Methode pour le controle des jobs : modifie un job en file d'attente
  void BatchManager_eSGE::alterJob(const JobId & jobid, const Parametre & param)
  {
    alterJob(jobid, param, Environnement());
  }

  // Methode pour le controle des jobs : modifie un job en file d'attente
  void BatchManager_eSGE::alterJob(const JobId & jobid, const Environnement & env)
  {
    alterJob(jobid, Parametre(), env);
  }

  // Methode pour le controle des jobs : renvoie l'etat du job
  JobInfo BatchManager_eSGE::queryJob(const JobId & jobid)
  {
    int id;
    istringstream iss(jobid.getReference());
    iss >> id;

    // define name of log file (local)
    string logFile = generateTemporaryFileName(string("SGE-querylog-id") + jobid.getReference());

    // define command to query batch
    string subCommand = string("qstat | grep ") + iss.str();
    string command = _protocol.getExecCommand(subCommand, _hostname, _username);
    command += " > ";
    command += logFile;
    cerr << command.c_str() << endl;
    int status = system(command.c_str());
    if (status && status != 256)
      throw EmulationException("Error of connection on remote host");

    JobInfo_eSGE ji = JobInfo_eSGE(id,logFile);
    return ji;
  }

  // Methode pour le controle des jobs : teste si un job est present en machine
  bool BatchManager_eSGE::isRunning(const JobId & jobid)
  {
    throw EmulationException("Not yet implemented");
  }

  void BatchManager_eSGE::buildBatchScript(const Job & job)
  {
#ifndef WIN32
    //TODO porting on Win32 platform
    std::cerr << "BuildBatchScript" << std::endl;
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
    std::string TmpFileName = createAndOpenTemporaryFile("SGE-script", tempOutputFile);

    tempOutputFile << "#! /bin/sh -f" << endl;
    if (queue != "")
      tempOutputFile << "#$ -q " << queue << endl;
    tempOutputFile << "#$ -pe mpich " << nbproc << endl;
    if( edt > 0 )
      tempOutputFile << "#$ -l h_rt=" << getWallTime(edt) << endl ;
    if( mem > 0 )
      tempOutputFile << "#$ -l h_vmem=" << mem << "M" << endl ;
    tempOutputFile << "#$ -o " << workDir << "/logs/output.log." << rootNameToExecute << endl ;
    tempOutputFile << "#$ -e " << workDir << "/logs/error.log." << rootNameToExecute << endl ;

    // Abstraction of PBS_NODEFILE - TODO
    tempOutputFile << "export LIBBATCH_NODEFILE=$TMPDIR/machines" << endl;

    // Launch the executable
    tempOutputFile << "cd " << workDir << endl ;
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

#endif //WIN32
  }

  std::string BatchManager_eSGE::getWallTime(const long edt)
  {
    long h, m;
    h = edt / 60;
    m = edt - h*60;
    ostringstream oss;
    if( m >= 10 )
      oss << h << ":" << m;
    else
      oss << h << ":0" << m;
    return oss.str();
  }

}
