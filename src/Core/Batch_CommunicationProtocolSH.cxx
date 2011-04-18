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
 *  Batch_CommunicationProtocolSH.cxx :
 *
 *  Created on: 16 sept. 2009
 *  Author : Renaud BARATE - EDF R&D
 */

#include "Batch_config.h"

#include "Batch_CommunicationProtocolSH.hxx"

using namespace std;

namespace Batch {

  // Simple method to fix path strings depending on the platform. On Windows, it will replace
  // forward slashes '/' by backslashes '\'. On Unix, the path is just copied without change.
  string CommunicationProtocolSH::fixPath(const string & path) const
  {
    string fixedPath = path;
  #ifdef WIN32
    for (unsigned int i=0 ; i<fixedPath.size() ; i++) {
      if (fixedPath[i] == '/') fixedPath[i] = '\\';
    }
  #endif
    return fixedPath;
  }

  vector<string> CommunicationProtocolSH::getExecCommandArgs(const string & subCommand,
                                                             const string & host,
                                                             const string & user) const
  {
    vector<string> cmd;

    cmd.push_back(fixPath(SH_COMMAND));

#ifdef WIN32
    cmd.push_back("/c");
#else
    cmd.push_back("-c");
#endif

    cmd.push_back(fixPath(subCommand));

    return cmd;
  }

  vector<string> CommunicationProtocolSH::getCopyCommandArgs(const string & sourcePath,
                                                             const string & sourceHost,
                                                             const string & sourceUser,
                                                             const string & destinationPath,
                                                             const string & destinationHost,
                                                             const string & destinationUser) const
  {
    vector<string> cmd;
    cmd.push_back(CP_COMMAND);
#ifndef WIN32
    cmd.push_back("-r");
#endif
    cmd.push_back(fixPath(sourcePath));
    cmd.push_back(fixPath(destinationPath));
    return cmd;
  }

  string CommunicationProtocolSH::getRemoveSubCommand(const string & path) const
  {
    return string(RM_COMMAND) + " " + fixPath(path);
  }

  string CommunicationProtocolSH::getMakeDirectorySubCommand(const string & path) const
  {
    string subCommand = MKDIR_COMMAND;
#ifndef WIN32
    subCommand += " -p";
#endif
    subCommand += " " + fixPath(path);
    return subCommand;
  }

}
