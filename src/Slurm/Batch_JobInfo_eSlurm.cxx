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
 *  Batch_JobInfo_eSlurm.cxx :
 *
 *  Created on: 12 may 2011
 *  Author : Renaud BARATE - EDF R&D
 */

#include <iostream>
#include <fstream>
#include <sstream>

#include <Batch_RunTimeException.hxx>
#include <Batch_Constants.hxx>

#include "Batch_JobInfo_eSlurm.hxx"

using namespace std;

namespace Batch {

  JobInfo_eSlurm::JobInfo_eSlurm(const std::string & id, const std::string & logFile)
    : JobInfo()
  {
    _param[ID] = id;

    // read log file
    ifstream log(logFile.c_str());
    string line;

    // status should be on the second line
    for (int i=0 ; i<2 ; i++)
      getline(log, line);
    log.close();
    string status;
    istringstream iss(line);
    iss >> status;

    if (status.size() == 0) {
      // On some batch managers, the job is deleted as soon as it is finished,
      // so we have to consider that an unknown job is a finished one, even if
      // it is not always true.
      _param[STATE] = FINISHED;
    } else if (status == "CA") { // Canceled
      _param[STATE] = FAILED;
    } else if (status == "CD") { // Completed
      _param[STATE] = FINISHED;
    } else if (status == "CF") { // Configuring
      _param[STATE] = QUEUED;
    } else if (status == "CG") { // Completing
      _param[STATE] = RUNNING;
    } else if (status == "F") {  // Failed
      _param[STATE] = FAILED;
    } else if (status == "NF") { // Node Fail
      _param[STATE] = FAILED;
    } else if (status == "PD") { // Pending
      _param[STATE] = QUEUED;
    } else if (status == "R") {  // Running
      _param[STATE] = RUNNING;
    } else if (status == "S") {  // Suspended
      _param[STATE] = PAUSED;
    } else if (status == "TO") { // Timeout
      _param[STATE] = FAILED;
    } else {
      throw RunTimeException("Unknown job state code: \"" + status + "\"");
    }
  }

  JobInfo_eSlurm::~JobInfo_eSlurm()
  {
  }

}
