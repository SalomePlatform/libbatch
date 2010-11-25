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
 *  Batch_BatchManager_eLL.cxx :
 *
 *  Created on: 25 nov. 2010
 *  Author : Renaud BARATE - EDF R&D
 */

#include <iostream>
#include <fstream>

#include <Batch_NotYetImplementedException.hxx>

#include "Batch_BatchManager_eLL.hxx"

using namespace std;

namespace Batch {

  BatchManager_eLL::BatchManager_eLL(const FactBatchManager * parent, const char * host,
                                     const char * username,
                                     CommunicationProtocolType protocolType, const char * mpiImpl,
                                     int nb_proc_per_node)
    : BatchManager(parent, host),
      BatchManager_eClient(parent, host, username, protocolType, mpiImpl)
  {
    // Nothing to do
  }

  BatchManager_eLL::~BatchManager_eLL()
  {
    // Nothing to do
  }

  // Method to submit a job to the batch manager
  const JobId BatchManager_eLL::submitJob(const Job & job)
  {
    int status;
    Parametre params = job.getParametre();
    const string workDir = params[WORKDIR];

    // export input files on cluster
    exportInputFiles(job);

    // build command file to submit the job and copy it on the server
    string cmdFile = buildCommandFile(job);

    // define name of log file (local)
    string logFile = generateTemporaryFileName("LL-submitlog");

    // define command to submit batch
    string subCommand = string("cd ") + workDir + "; llsubmit " + cmdFile;
    string command = _protocol.getExecCommand(subCommand, _hostname, _username);
    command += " > ";
    command += logFile;
    cerr << command.c_str() << endl;
    status = system(command.c_str());
    if(status)
    {
      ifstream error_message(logFile.c_str());
      string mess;
      string temp;
      while(std::getline(error_message, temp))
          mess += temp;
      error_message.close();
      throw EmulationException("Error of connection on remote host, error was: " + mess);
    }

    // read id of submitted job in log file
    string jobref;
    ifstream idfile(logFile.c_str());
    unsigned int linebufsize = 1024;
    char linebuf[linebufsize];
    idfile.getline(linebuf, linebufsize);
    while (!idfile.eof() && strncmp(linebuf, "llsubmit:", 9) != 0)
      idfile.getline(linebuf, linebufsize);
    idfile.close();
    if (strncmp(linebuf, "llsubmit:", 9) == 0)
    {
      string line(linebuf);
      string::size_type p1 = line.find_first_of("\"");
      string::size_type p2 = line.find_last_of("\"");
      if (p1 != p2)
        jobref = line.substr(p1 + 1, p2 - p1 - 1);
    }
    if (jobref.size() == 0)
      throw EmulationException("Error in the submission of the job on the remote host");

    JobId id(this, jobref);
    return id;
  }

  /**
   * Create LoadLeveler command file and copy it on the server.
   * Return the name of the remote file.
   */
  string BatchManager_eLL::buildCommandFile(const Job & job)
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
      throw EmulationException("params[WORKDIR] is not defined ! Please defined it, cannot submit this job");
    if (params.find(EXECUTABLE) != params.end()) 
      fileToExecute = params[EXECUTABLE].str();
    else 
      throw EmulationException("params[EXECUTABLE] is not defined ! Please defined it, cannot submit this job");

    // Optional parameters
    if (params.find(QUEUE) != params.end()) 
      queue = params[QUEUE].str();

    string::size_type p1 = fileToExecute.find_last_of("/");
    string::size_type p2 = fileToExecute.find_last_of(".");
    string rootNameToExecute = fileToExecute.substr(p1+1,p2-p1-1);
    string fileNameToExecute = fileToExecute.substr(p1+1);

    // Create batch submit file
    ofstream tempOutputFile;
    std::string tmpFileName = createAndOpenTemporaryFile("LL-script", tempOutputFile);

    tempOutputFile << "# @ executable = " << fileNameToExecute << endl;
    tempOutputFile << "# @ output = " << workDir << "/logs/output.log." << rootNameToExecute << endl;
    tempOutputFile << "# @ error = " << workDir << "/logs/error.log." << rootNameToExecute << endl;
    if (queue != "")
      tempOutputFile << "# @ class = " << queue << endl;
    tempOutputFile << "# @ job_type = bluegene" << endl;
    tempOutputFile << "# @ queue" << endl;

    tempOutputFile.flush();
    tempOutputFile.close();

    cerr << "Batch script file generated is: " << tmpFileName.c_str() << endl;

    string remoteFileName = rootNameToExecute + "_LL.cmd";
    int status = _protocol.copyFile(tmpFileName, "", "",
                                    workDir + "/" + remoteFileName,
                                    _hostname, _username);
    if (status)
      throw EmulationException("Error of connection on remote host, cannot copy batch submission file");

    return remoteFileName;
  }

  void BatchManager_eLL::deleteJob(const JobId & jobid)
  {
    throw NotYetImplementedException("BatchManager_eLL::deleteJob");
  }

  void BatchManager_eLL::holdJob(const JobId & jobid)
  {
    throw NotYetImplementedException("BatchManager_eLL::holdJob");
  }

  void BatchManager_eLL::releaseJob(const JobId & jobid)
  {
    throw NotYetImplementedException("BatchManager_eLL::releaseJob");
  }

  void BatchManager_eLL::alterJob(const JobId & jobid, const Parametre & param, const Environnement & env)
  {
    throw NotYetImplementedException("BatchManager_eLL::alterJob");
  }

  void BatchManager_eLL::alterJob(const JobId & jobid, const Parametre & param)
  {
    throw NotYetImplementedException("BatchManager_eLL::alterJob");
  }

  void BatchManager_eLL::alterJob(const JobId & jobid, const Environnement & env)
  {
    throw NotYetImplementedException("BatchManager_eLL::alterJob");
  }

  JobInfo BatchManager_eLL::queryJob(const JobId & jobid)
  {
    throw NotYetImplementedException("BatchManager_eLL::queryJob");
  }

  const JobId BatchManager_eLL::addJob(const Job & job, const string reference)
  {
    throw NotYetImplementedException("BatchManager_eLL::addJob");
  }

}
