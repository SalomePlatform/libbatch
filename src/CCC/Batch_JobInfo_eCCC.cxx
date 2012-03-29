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
 * JobInfo_eCCC.cxx :  emulation of CCC client for CCRT machines
 *
 * Auteur : Bernard SECHER - CEA DEN
 * Mail   : mailto:bernard.secher@cea.fr
 * Date   : Thu Apr 24 10:17:22 2010
 * Projet : PAL Salome 
 *
 */

#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream>

#include "Batch_Constants.hxx"
#include "Batch_Parametre.hxx"
#include "Batch_Environnement.hxx"
#include "Batch_RunTimeException.hxx"
#include "Batch_APIInternalFailureException.hxx"
#include "Batch_JobInfo_eCCC.hxx"

using namespace std;

namespace Batch {



  // Constructeurs
  JobInfo_eCCC::JobInfo_eCCC(int id, string logFile) : JobInfo()
  {
    // On remplit les membres _param et _env
    ostringstream oss;
    oss << id;
    _param[ID] = oss.str();

    // read status of job in log file
    char line[128];
    ifstream fp(logFile.c_str(),ios::in);
    fp.getline(line,80,'\n');
    
    string sjobid, username, status;
    fp >> sjobid;
    fp >> username;
    fp >> status;

    if (status == "PEND") {         // Pending
      _param[STATE] = QUEUED;
    } else if (status == "PSUSP") { // Suspended while pending
      _param[STATE] = PAUSED;
    } else if (status == "RUN") {   // Running
      _param[STATE] = RUNNING;
    } else if (status == "USUSP") { // Suspended while running
      _param[STATE] = PAUSED;
    } else if (status == "SSUSP") { // Suspended by CCC
      _param[STATE] = PAUSED;
    } else if (status == "DONE") {  // Finished successfully
      _param[STATE] = FINISHED;
    } else if (status == "EXIT") {  // Finished successfully
      _param[STATE] = FINISHED;
    } else if (status == "UNKWN") { // Lost contact
      _param[STATE] = FAILED;
    } else if (status == "ZOMBI") { // Zombie
      _param[STATE] = FAILED;
    } else {
      cerr << "Unknown job state code: " << status << endl;
    }

    if( status.find("RUN") != string::npos)
      _running = true;

  }

  // Teste si un job est present en machine
  bool JobInfo_eCCC::isRunning() const
  {
    return _running;
  }


  // Destructeur
  JobInfo_eCCC::~JobInfo_eCCC()
  {
    // Nothing to do
  }

  // Convertit une date HH:MM:SS en secondes
  long JobInfo_eCCC::HMStoLong(const string & s)
  {
    long hour, min, sec;

    sscanf( s.c_str(), "%ld:%ld:%ld", &hour, &min, &sec);
    return ( ( ( hour * 60L ) + min ) * 60L ) + sec;
  }

  // Methode pour l'interfacage avec Python (SWIG) : affichage en Python
  string JobInfo_eCCC::__str__() const
  {
    ostringstream sst;
    sst << "<JobInfo_eCCC (" << this << ") :" << endl;
    sst << " ID = " <<_param[ID] << endl;
    sst << " STATE = " <<_param[STATE] << endl;

    return sst.str();
  }


}
