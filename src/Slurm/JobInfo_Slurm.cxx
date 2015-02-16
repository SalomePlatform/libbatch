// Copyright (C) 2007-2015  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
/*
 *  JobInfo_Slurm.cxx :
 *
 *  Created on: 12 may 2011
 *  Author : Renaud BARATE - EDF R&D
 */

#include <sstream>

#include <RunTimeException.hxx>
#include <Constants.hxx>

#include "JobInfo_Slurm.hxx"

using namespace std;

namespace Batch {

  JobInfo_Slurm::JobInfo_Slurm(const std::string & id, const std::string & queryOutput)
    : JobInfo()
  {
    _param[ID] = id;

    // read query output, status should be on the second line
    istringstream iss(queryOutput);
    string status;
    for (int i=0 ; i<2 ; i++)
      getline(iss, status);

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

  JobInfo_Slurm::~JobInfo_Slurm()
  {
  }

}
