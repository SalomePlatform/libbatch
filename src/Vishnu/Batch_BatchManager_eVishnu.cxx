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
 *  Batch_BatchManager_eVishnu.cxx :
 *
 *  Created on: 24 june 2011
 *  Author : Renaud BARATE - EDF R&D
 */

#ifndef WIN32
#include <unistd.h>
#endif

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>

#include <Batch_NotYetImplementedException.hxx>
#include <Batch_Constants.hxx>
#include <Batch_Utils.hxx>

#include "Batch_BatchManager_eVishnu.hxx"
#include "Batch_JobInfo_eVishnu.hxx"

using namespace std;

namespace Batch {

  BatchManager_eVishnu::BatchManager_eVishnu(const FactBatchManager * parent,
                                             const char * host,
                                             const char * username,
                                             CommunicationProtocolType protocolType,
                                             const char * mpiImpl,
                                             int nb_proc_per_node)
    : BatchManager(parent, host),
      // Force SH protocol for Vishnu
      BatchManager_eClient(parent, host, username, SH, mpiImpl),
      _nb_proc_per_node(nb_proc_per_node)
  {
  }

  BatchManager_eVishnu::~BatchManager_eVishnu()
  {
  }

  // Method to submit a job to the batch manager
  const JobId BatchManager_eVishnu::submitJob(const Job & job)
  {
    // export input files on cluster
    exportInputFiles(job);

    // build command file to submit the job
    string cmdFile = buildCommandFile(job);

    // define extra parameters (that can not be defined in the command file)
    Parametre params = job.getParametre();
    ostringstream extraParams;
    if (params.find(NBPROC) != params.end())
      extraParams << "-P " << params[NBPROC] << " ";
    if (params.find(MAXRAMSIZE) != params.end())
      extraParams << "-m " << params[MAXRAMSIZE] << " ";

    // define command to submit batch
    string subCommand = string("export OMNIORB_CONFIG=$VISHNU_CONFIG_FILE; ");
    subCommand += "vishnu_connect -p 2 && ";
    subCommand += "vishnu_submit_job " + extraParams.str() + _hostname + " " + cmdFile;
    string command = _protocol.getExecCommand(subCommand, _hostname, _username);
    command += " 2>&1";
    cerr << command.c_str() << endl;

    // submit job
    string output;
    int status = Utils::getCommandOutput(command, output);
    cout << output;
    if (status != 0) throw EmulationException("Can't submit job, error was: " + output);

    // find id of submitted job in output
    string search = "Job Id     : ";
    string::size_type pos = output.find(search);
    if (pos == string::npos)
      throw EmulationException("Error in the submission of the job on the remote host");
    pos += search.size();
    string::size_type endl_pos = output.find('\n', pos);
    string::size_type count = (endl_pos == string::npos)? string::npos : endl_pos - pos;
    string jobref = output.substr(pos, count);
    if (jobref.size() == 0)
      throw EmulationException("Error in the submission of the job on the remote host");

    JobId id(this, jobref);
    return id;
  }


  void BatchManager_eVishnu::exportInputFiles(const Job& job)
  {
    Parametre params = job.getParametre();
    string subCommand = string("export OMNIORB_CONFIG=$VISHNU_CONFIG_FILE; ");
    subCommand += "vishnu_connect -p 2 && ";

    // create remote directories
    subCommand += "vishnu_create_dir -p " + _hostname + ":" + params[WORKDIR].str() + "/logs && ";

    // copy executable
    string executeFile = params[EXECUTABLE];
    if (executeFile.size() != 0) {
      subCommand += "vishnu_copy_file " + executeFile + " " +
                    _hostname + ":" + params[WORKDIR].str() + "/ && ";
    }

    // copy filesToExportList
    const Versatile & V = params[INFILE];
    Versatile::const_iterator Vit;
    for (Vit=V.begin(); Vit!=V.end(); Vit++) {
      CoupleType cpt  = *static_cast< CoupleType * >(*Vit);
      Couple inputFile = cpt;

      // Get absolute paths
      char * buf = getcwd(NULL, 0);
      string cwd = buf;
      free(buf);

      string absremote = (Utils::isAbsolutePath(inputFile.getRemote()))?
                         inputFile.getRemote() :
                         params[WORKDIR].str() + "/" + inputFile.getRemote();
      string abslocal = (Utils::isAbsolutePath(inputFile.getLocal()))?
                        inputFile.getLocal() :
                        cwd + "/" + inputFile.getLocal();

      if (Vit != V.begin())
        subCommand += " && ";
      subCommand += "vishnu_copy_file " + abslocal + " " + _hostname + ":" + absremote;
    }

    // Execute command
    string command = _protocol.getExecCommand(subCommand, _hostname, _username);
    command += " 2>&1";
    cerr << command.c_str() << endl;
    string output;
    int status = Utils::getCommandOutput(command, output);
    cout << output;
    if (status != 0)
      throw EmulationException("Can't copy input files, error was: " + output);
  }

  /**
   * Create Vishnu command file and copy it on the server.
   * Return the name of the remote file.
   */
  string BatchManager_eVishnu::buildCommandFile(const Job & job)
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
    string tmpFileName = createAndOpenTemporaryFile("vishnu-script", tempOutputFile);

    tempOutputFile << "#!/bin/sh" << endl;
    tempOutputFile << "#% vishnu_output=" << workDir << "/logs/output.log." << rootNameToExecute << endl;
    tempOutputFile << "#% vishnu_error=" << workDir << "/logs/error.log." << rootNameToExecute << endl;

    if (params.find(NAME) != params.end())
      tempOutputFile << "#% vishnu_job_name=\"" << params[NAME] << "\"" << endl;

    // Optional parameters
    if (params.find(MAXWALLTIME) != params.end()) {
      long totalMinutes = params[MAXWALLTIME];
      long h = totalMinutes / 60;
      long m = totalMinutes - h * 60;
      tempOutputFile << "#% vishnu_wallclocklimit=" << h << ":";
      if (m < 10)
        tempOutputFile << "0";
      tempOutputFile << m << ":00" << endl;
    }
    if (params.find(QUEUE) != params.end())
      tempOutputFile << "#% vishnu_queue=" << params[QUEUE] << endl;

    // Define environment for the job
    Environnement env = job.getEnvironnement();
    for (Environnement::const_iterator iter = env.begin() ; iter != env.end() ; ++iter) {
      tempOutputFile << "export " << iter->first << "=" << iter->second << endl;
    }

    // Node file
    tempOutputFile << "export LIBBATCH_NODEFILE=$VISHNU_BATCHJOB_NODEFILE" << endl;

    // Launch the executable
    tempOutputFile << "cd " << workDir << endl;
    tempOutputFile << "./" + fileNameToExecute << endl;

    tempOutputFile.flush();
    tempOutputFile.close();

    cerr << "Batch script file generated is: " << tmpFileName << endl;
    return tmpFileName;
  }

  void BatchManager_eVishnu::deleteJob(const JobId & jobid)
  {
    // define command to delete job
    string subCommand = string("export OMNIORB_CONFIG=$VISHNU_CONFIG_FILE; ");
    subCommand += "vishnu_connect -p 2 && ";
    subCommand += "vishnu_cancel_job " + _hostname + " " + jobid.getReference();
    string command = _protocol.getExecCommand(subCommand, _hostname, _username);
    cerr << command.c_str() << endl;

    int status = system(command.c_str());
    if (status)
      throw EmulationException("Can't delete job " + jobid.getReference());

    cerr << "job " << jobid.getReference() << " killed" << endl;
  }

  void BatchManager_eVishnu::holdJob(const JobId & jobid)
  {
    throw NotYetImplementedException("BatchManager_eVishnu::holdJob");
  }

  void BatchManager_eVishnu::releaseJob(const JobId & jobid)
  {
    throw NotYetImplementedException("BatchManager_eVishnu::releaseJob");
  }

  void BatchManager_eVishnu::alterJob(const JobId & jobid, const Parametre & param, const Environnement & env)
  {
    throw NotYetImplementedException("BatchManager_eVishnu::alterJob");
  }

  void BatchManager_eVishnu::alterJob(const JobId & jobid, const Parametre & param)
  {
    throw NotYetImplementedException("BatchManager_eVishnu::alterJob");
  }

  void BatchManager_eVishnu::alterJob(const JobId & jobid, const Environnement & env)
  {
    throw NotYetImplementedException("BatchManager_eVishnu::alterJob");
  }

  JobInfo BatchManager_eVishnu::queryJob(const JobId & jobid)
  {
    // define command to query batch
    string subCommand = string("export OMNIORB_CONFIG=$VISHNU_CONFIG_FILE; ");
    subCommand += "vishnu_connect -p 2 && ";
    subCommand += "vishnu_get_job_info " + _hostname + " " + jobid.getReference();
    string command = _protocol.getExecCommand(subCommand, _hostname, _username);
    cerr << command.c_str() << endl;

    string output;
    int status = Utils::getCommandOutput(command, output);
    if (status != 0)
      throw EmulationException("Can't query job " + jobid.getReference());
    JobInfo_eVishnu jobinfo = JobInfo_eVishnu(jobid.getReference(), output);
    return jobinfo;
  }

  const JobId BatchManager_eVishnu::addJob(const Job & job, const string reference)
  {
    return JobId(this, reference);
  }

  void BatchManager_eVishnu::importOutputFiles(const Job & job, const std::string directory)
  {
    // Create local result directory
    char * buf = getcwd(NULL, 0);
    string cwd = buf;
    free(buf);
    string absdir = (Utils::isAbsolutePath(directory))? directory : cwd + "/" + directory;
    int status = CommunicationProtocol::getInstance(SH).makeDirectory(absdir, "", "");
    if (status != 0) {
      throw EmulationException("Can't create result directory");
    }

    string subCommand = string("export OMNIORB_CONFIG=$VISHNU_CONFIG_FILE; ");
    subCommand += "vishnu_connect -p 2 && ";

    // Copy output files
    Parametre params = job.getParametre();
    const Versatile & V = params[OUTFILE];
    Versatile::const_iterator Vit;
    for (Vit=V.begin(); Vit!=V.end(); Vit++) {
      CoupleType cpt  = *static_cast< CoupleType * >(*Vit);
      Couple outputFile = cpt;

      // Get absolute paths
      string absremote = (Utils::isAbsolutePath(outputFile.getRemote()))?
                         outputFile.getRemote() :
                         params[WORKDIR].str() + "/" + outputFile.getRemote();
      string abslocal = (Utils::isAbsolutePath(outputFile.getLocal()))?
                        outputFile.getLocal() :
                        absdir + "/" + outputFile.getLocal();

      subCommand += "vishnu_copy_file " + _hostname + ":" + absremote + " " + abslocal + " && ";
    }

    // Copy logs
    subCommand += "vishnu_copy_file -r " +_hostname + ":" + params[WORKDIR].str() + "/logs" + " " + absdir;

    // Execute command
    string command = _protocol.getExecCommand(subCommand, _hostname, _username);
    command += " 2>&1";
    cerr << command.c_str() << endl;
    string output;
    status = Utils::getCommandOutput(command, output);
    cout << output;
    if (status != 0)
      throw EmulationException("Can't import output files, error was: " + output);
  }

}
