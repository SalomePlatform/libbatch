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
 * BatchManager_eSSH.cxx : emulation of SSH client
 *
 * Auteur : André RIBES - EDF R&D
 * Date   : Octobre 2009
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/stat.h>

#include <stdlib.h>
#include <string.h>
#include "Batch_config.h"

#ifdef MSVC
#include <io.h>
#else
#include <libgen.h>
#endif

#include "Batch_Constants.hxx"
#include "Batch_BatchManager_eSSH.hxx"
#include "Batch_JobInfo_eSSH.hxx"

using namespace std;

namespace Batch {

  BatchManager_eSSH::BatchManager_eSSH(const FactBatchManager * parent, const char * host,
                                       const char * username,
                                       CommunicationProtocolType protocolType, const char * mpiImpl)
    : BatchManager(parent, host),
      BatchManager_eClient(parent, host, username, protocolType, mpiImpl),
      BatchManager_Local(parent, host, protocolType)
  {
    // Nothing to do
  }

  // Destructeur
  BatchManager_eSSH::~BatchManager_eSSH()
  {
    // Nothing to do
  }

  // Methode pour le controle des jobs : soumet un job au gestionnaire
  const JobId BatchManager_eSSH::submitJob(const Job & job)
  {
    // export input files on cluster
    std::cerr << "BatchManager_eSSH::submitJob exportInputFiles" << std::endl;
    Parametre param = job.getParametre();

    // Input files copy
    exportInputFiles(job);

    // Launch job
    // Patch until Local Manager is patched
    std::string executable = param[EXECUTABLE].str();
    std::string::size_type p1 = executable.find_last_of("/");
    std::string fileNameToExecute = "./" + executable.substr(p1+1);
    Parametre new_param(param);
    new_param[INFILE].eraseAll(); 
    new_param[OUTFILE].eraseAll();
    new_param[EXECUTABLE] = fileNameToExecute;
    new_param[EXECUTIONHOST] = _hostname;
    Job * j = new Job(new_param);


    std::cerr << "BatchManager_eSSH::submitJob Local submitJob" << std::endl;
    JobId id = BatchManager_Local::submitJob(*j);
    delete j;
    return id;
  }

  // Methode pour le controle des jobs : retire un job du gestionnaire
  void BatchManager_eSSH::deleteJob(const JobId & jobid)
  {
    BatchManager_Local::deleteJob(jobid);
  }
  
  // Methode pour le controle des jobs : renvoie l'etat du job
  JobInfo BatchManager_eSSH::queryJob(const JobId & jobid)
  {
    return BatchManager_Local::queryJob(jobid);
  }

  // Methode pour le controle des jobs : suspend un job en file d'attente
  void BatchManager_eSSH::holdJob(const JobId & jobid)
  {
    BatchManager_Local::holdJob(jobid);
  }

  // Methode pour le controle des jobs : relache un job suspendu
  void BatchManager_eSSH::releaseJob(const JobId & jobid)
  {
    BatchManager_Local::releaseJob(jobid);
  }

  // Methode pour le controle des jobs : modifie un job en file d'attente
  void BatchManager_eSSH::alterJob(const JobId & jobid, const Parametre & param, const Environnement & env)
  {
    BatchManager_Local::alterJob(jobid, param, env);
  }

  // Methode pour le controle des jobs : modifie un job en file d'attente
  void BatchManager_eSSH::alterJob(const JobId & jobid, const Parametre & param)
  {
    BatchManager_Local::alterJob(jobid, param);
  }

  // Methode pour le controle des jobs : modifie un job en file d'attente
  void BatchManager_eSSH::alterJob(const JobId & jobid, const Environnement & env)
  {
    BatchManager_Local::alterJob(jobid, env); 
  }

  void BatchManager_eSSH::buildBatchScript(const Job & job)
  {
    Parametre params = job.getParametre();
    Environnement env = job.getEnvironnement();
    const long nbproc = params[NBPROC];
    const long edt = params[MAXWALLTIME];
    const long mem = params[MAXRAMSIZE];
    const string workDir = params[WORKDIR];
    const std::string dirForTmpFiles = params[TMPDIR];
    const string fileToExecute = params[EXECUTABLE];
    const string home = params[HOMEDIR];
    const std::string queue = params[QUEUE];
    std::string rootNameToExecute;
    std::string fileNameToExecute;
    std::string filelogtemp;
    if( fileToExecute.size() > 0 ){
      string::size_type p1 = fileToExecute.find_last_of("/");
      string::size_type p2 = fileToExecute.find_last_of(".");
      rootNameToExecute = fileToExecute.substr(p1+1,p2-p1-1);

#ifdef MSVC
      char fname[_MAX_FNAME];
      char ext[_MAX_EXT];
      _splitpath_s(fileToExecute.c_str(), NULL, 0, NULL, 0, fname, _MAX_FNAME, ext, _MAX_EXT);
      string execBaseName = string(fname) + ext;
#else
      char* basec=strdup(fileToExecute.c_str());
      string execBaseName = string(basename(basec));
      free(basec);
#endif

      fileNameToExecute = "~/" + dirForTmpFiles + "/" + execBaseName;

      int idx = dirForTmpFiles.find("Batch/");
      filelogtemp = dirForTmpFiles.substr(idx+6, dirForTmpFiles.length());
    }
    else{
      rootNameToExecute = "command";
    }

    ofstream tempOutputFile;
    std::string TmpFileName = createAndOpenTemporaryFile("SSH-script", tempOutputFile);

    tempOutputFile << "#! /bin/sh -f" << endl;
    if (queue != "")
      tempOutputFile << "#BSUB -q " << queue << endl;
    if( edt > 0 )
      tempOutputFile << "#SSH -l walltime=" << edt*60 << endl ;
    if( mem > 0 )
      tempOutputFile << "#SSH -l mem=" << mem << "mb" << endl ;
    if( fileToExecute.size() > 0 ){
      tempOutputFile << "#SSH -o " << home << "/" << dirForTmpFiles << "/output.log." << filelogtemp << endl ;
      tempOutputFile << "#SSH -e " << home << "/" << dirForTmpFiles << "/error.log." << filelogtemp << endl ;
    }
    else{
      tempOutputFile << "#SSH -o " << dirForTmpFiles << "/" << env["LOGFILE"] << ".output.log" << endl ;
      tempOutputFile << "#SSH -e " << dirForTmpFiles << "/" << env["LOGFILE"] << ".error.log" << endl ;
    }
    if( workDir.size() > 0 )
      tempOutputFile << "cd " << workDir << endl ;
    if( fileToExecute.size() > 0 ){
      tempOutputFile << _mpiImpl->boot("${SSH_NODEFILE}",nbproc);
      tempOutputFile << _mpiImpl->run("${SSH_NODEFILE}",nbproc,fileNameToExecute);
      tempOutputFile << _mpiImpl->halt();
    }
    else{
      tempOutputFile << "source " << env["SOURCEFILE"] << endl ;
      tempOutputFile << env["COMMAND"];
    }

    tempOutputFile.flush();
    tempOutputFile.close();
#ifdef WIN32
    _chmod(
#else
    chmod(
#endif
      TmpFileName.c_str(), 0x1ED);
    cerr << TmpFileName.c_str() << endl;

    int status = Batch::BatchManager_eClient::_protocol.copyFile(TmpFileName, "", "",
                                    dirForTmpFiles + "/" + rootNameToExecute + "_Batch.sh",
                                    _hostname, _username);
    if (status)
      throw EmulationException("Error of connection on remote host");

    remove(TmpFileName.c_str());
  }

}
