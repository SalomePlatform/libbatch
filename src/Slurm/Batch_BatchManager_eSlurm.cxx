//  Copyright (C) 2007-2011  CEA/DEN, EDF R&D, OPEN CASCADE
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
 *  Batch_BatchManager_eSlurm.cxx :
 *
 *  Created on: 12 may 2011
 *  Author : Renaud BARATE - EDF R&D
 */

#include <cstdlib>
#include <iostream>
#include <fstream>

#include <Batch_NotYetImplementedException.hxx>
#include <Batch_Constants.hxx>

#include "Batch_FactBatchManager_eSlurm.hxx"
#include "Batch_BatchManager_eSlurm.hxx"
#include "Batch_JobInfo_eSlurm.hxx"

using namespace std;

namespace Batch {

  BatchManager_eSlurm::BatchManager_eSlurm(const FactBatchManager * parent,
                                           const char * host,
                                           const char * username,
                                           CommunicationProtocolType protocolType,
                                           const char * mpiImpl,
                                           int nb_proc_per_node)
    : BatchManager(parent, host),
      BatchManager_eClient(parent, host, username, protocolType, mpiImpl),
      _nb_proc_per_node(nb_proc_per_node)
  {
  }

  BatchManager_eSlurm::~BatchManager_eSlurm()
  {
  }

  // Method to submit a job to the batch manager
  const JobId BatchManager_eSlurm::submitJob(const Job & job)
  {
    int status;
    Parametre params = job.getParametre();
    const string workDir = params[WORKDIR];

    // export input files on cluster
    exportInputFiles(job);

    // build command file to submit the job and copy it on the server
    string cmdFile = buildCommandFile(job);

    // define name of log file (local)
    string logFile = generateTemporaryFileName("slurm-submitlog");

    // define command to submit batch
    string subCommand = string("cd ") + workDir + "; sbatch " + cmdFile;
    string command = _protocol.getExecCommand(subCommand, _hostname, _username);
    command += " > ";
    command += logFile;
    cerr << command.c_str() << endl;
    status = system(command.c_str());
    if (status)
    {
      ifstream error_message(logFile.c_str());
      string mess;
      string temp;
      while(getline(error_message, temp))
        mess += temp;
      error_message.close();
      throw EmulationException("Error of connection on remote host, error was: " + mess);
    }

    // read id of submitted job in log file
    string jobref;
    ifstream idfile(logFile.c_str());
    string line;
    while (idfile && line.compare(0, 20, "Submitted batch job ") != 0)
      getline(idfile, line);
    idfile.close();
    if (line.compare(0, 20, "Submitted batch job ") == 0)
      jobref = line.substr(20);
    if (jobref.size() == 0)
      throw EmulationException("Error in the submission of the job on the remote host");

    JobId id(this, jobref);
    return id;
  }

  /**
   * Create Slurm command file and copy it on the server.
   * Return the name of the remote file.
   */
  string BatchManager_eSlurm::buildCommandFile(const Job & job)
  {
    Parametre params = job.getParametre();

    // Job Parameters
    string workDir = "";
    string fileToExecute = "";
    string queue = "";

    // Mandatory parameters
    if (params.find(WORKDIR) != params.end()) 
      workDir = params[WORKDIR].str();
    else 
      throw EmulationException("params[WORKDIR] is not defined. Please define it, cannot submit this job.");
    if (params.find(EXECUTABLE) != params.end()) 
      fileToExecute = params[EXECUTABLE].str();
    else 
      throw EmulationException("params[EXECUTABLE] is not defined. Please define it, cannot submit this job.");

    string::size_type p1 = fileToExecute.find_last_of("/");
    string::size_type p2 = fileToExecute.find_last_of(".");
    string rootNameToExecute = fileToExecute.substr(p1+1,p2-p1-1);
    string fileNameToExecute = fileToExecute.substr(p1+1);

    // Create batch submit file
    ofstream tempOutputFile;
    string tmpFileName = createAndOpenTemporaryFile("slurm-script", tempOutputFile);

    tempOutputFile << "#!/bin/bash" << endl;
    tempOutputFile << "#SBATCH --output=" << workDir << "/logs/output.log." << rootNameToExecute << endl;
    tempOutputFile << "#SBATCH --error=" << workDir << "/logs/error.log." << rootNameToExecute << endl;

    if (params.find(NAME) != params.end())
      tempOutputFile << "#SBATCH --job-name=\"" << params[NAME] << "\"" << endl;

    // Optional parameters
    int nbproc = 1;
    if (params.find(NBPROC) != params.end())
      nbproc = params[NBPROC];
    tempOutputFile << "#SBATCH --ntasks=" << nbproc << endl;

    if (params.find(EXCLUSIVE) != params.end()) {
      if (params[EXCLUSIVE])
        tempOutputFile << "#SBATCH --exclusive" << endl;
      else
        tempOutputFile << "#SBATCH --share" << endl;
    }

    if (params.find(MAXWALLTIME) != params.end())
      tempOutputFile << "#SBATCH --time=" << params[MAXWALLTIME] << endl;
    if (params.find(MAXRAMSIZE) != params.end())
      tempOutputFile << "#SBATCH --mem=" << params[MAXRAMSIZE] << endl;
    if (params.find(QUEUE) != params.end())
      tempOutputFile << "#SBATCH --partition=" << params[QUEUE] << endl;

    // Define environment for the job
    Environnement env = job.getEnvironnement();
    for (Environnement::const_iterator iter = env.begin() ; iter != env.end() ; ++iter) {
      tempOutputFile << "export " << iter->first << "=" << iter->second << endl;
    }

    // generate nodes file
    tempOutputFile << "LIBBATCH_NODEFILE=`mktemp nodefile-XXXXXXXXXX`" << endl;
    tempOutputFile << "srun hostname > $LIBBATCH_NODEFILE" << endl;
    tempOutputFile << "export LIBBATCH_NODEFILE" << endl;

    // Launch the executable
    tempOutputFile << "cd " << workDir << endl;
    tempOutputFile << "./" + fileNameToExecute << endl;

    // Remove the node file
    tempOutputFile << "rm $LIBBATCH_NODEFILE" << endl;

    tempOutputFile.flush();
    tempOutputFile.close();

    cerr << "Batch script file generated is: " << tmpFileName << endl;

    string remoteFileName = rootNameToExecute + "_slurm.cmd";
    int status = _protocol.copyFile(tmpFileName, "", "",
                                    workDir + "/" + remoteFileName,
                                    _hostname, _username);
    if (status)
      throw EmulationException("Cannot copy command file on host " + _hostname);

    return remoteFileName;
  }

  void BatchManager_eSlurm::deleteJob(const JobId & jobid)
  {
    // define command to delete job
    string subCommand = "scancel " + jobid.getReference();
    string command = _protocol.getExecCommand(subCommand, _hostname, _username);
    cerr << command.c_str() << endl;

    int status = system(command.c_str());
    if (status)
      throw EmulationException("Can't delete job " + jobid.getReference());

    cerr << "job " << jobid.getReference() << " killed" << endl;
  }

  void BatchManager_eSlurm::holdJob(const JobId & jobid)
  {
    throw NotYetImplementedException("BatchManager_eSlurm::holdJob");
  }

  void BatchManager_eSlurm::releaseJob(const JobId & jobid)
  {
    throw NotYetImplementedException("BatchManager_eSlurm::releaseJob");
  }

  void BatchManager_eSlurm::alterJob(const JobId & jobid, const Parametre & param, const Environnement & env)
  {
    throw NotYetImplementedException("BatchManager_eSlurm::alterJob");
  }

  void BatchManager_eSlurm::alterJob(const JobId & jobid, const Parametre & param)
  {
    throw NotYetImplementedException("BatchManager_eSlurm::alterJob");
  }

  void BatchManager_eSlurm::alterJob(const JobId & jobid, const Environnement & env)
  {
    throw NotYetImplementedException("BatchManager_eSlurm::alterJob");
  }

  JobInfo BatchManager_eSlurm::queryJob(const JobId & jobid)
  {
    // define name of log file (local)
    string logFile = generateTemporaryFileName("slurm-querylog-" + jobid.getReference());

    // define command to query batch
    string subCommand = "squeue -o %t -j " + jobid.getReference();
    string command = _protocol.getExecCommand(subCommand, _hostname, _username);
    command += " > ";
    command += logFile;
    cerr << command.c_str() << endl;
    system(command.c_str());
    // We don't test the return code here because with jobs finished since a long time Slurm
    // returns an error and a message like "slurm_load_jobs error: Invalid job id specified".
    // So we consider that the job is finished when we get an error.

    JobInfo_eSlurm jobinfo = JobInfo_eSlurm(jobid.getReference(), logFile);
    return jobinfo;
  }

  const JobId BatchManager_eSlurm::addJob(const Job & job, const string reference)
  {
    return JobId(this, reference);
  }

}
