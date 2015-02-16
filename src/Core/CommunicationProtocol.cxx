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
 *  CommunicationProtocol.cxx :
 *
 *  Created on: 14 sept. 2009
 *  Author : Renaud BARATE - EDF R&D
 */

#include <stdlib.h>

#include <libbatch_config.h>

#include "CommunicationProtocol.hxx"
#ifdef HAS_RSH
 #include "CommunicationProtocolRSH.hxx"
#endif
#ifdef HAS_SH
 #include "CommunicationProtocolSH.hxx"
#endif
#ifdef HAS_SSH
 #include "CommunicationProtocolSSH.hxx"
#endif
#include "APIInternalFailureException.hxx"
#include "RunTimeException.hxx"
#include "Log.hxx"

using namespace std;

namespace Batch {

  CommunicationProtocol::CommunicationProtocol(CommunicationProtocolType type)
  : _type(type)
  {
  }

  CommunicationProtocol::~CommunicationProtocol()
  {
  }

  const CommunicationProtocol & CommunicationProtocol::getInstance(CommunicationProtocolType protocolType)
  {
    if (protocolType == SH) {
#ifdef HAS_SH
      static CommunicationProtocolSH instanceSH;
      return instanceSH;
#else
      throw RunTimeException("Can't use SH protocol (SH tools were "
                             "not found on the system at compile time).");
#endif
    } else if (protocolType == RSH) {
#ifdef HAS_RSH
      static CommunicationProtocolRSH instanceRSH;
      return instanceRSH;
#else
      throw RunTimeException("Can't use RSH protocol (RSH tools were "
                             "not found on the system at compile time).");
#endif
    } else if (protocolType == SSH) {
#ifdef HAS_SSH
      static CommunicationProtocolSSH instanceSSH;
      return instanceSSH;
#else
      throw RunTimeException("Can't use SSH protocol (SSH tools were "
                             "not found on the system at compile time).");
#endif
    } else
      throw APIInternalFailureException("Unknown communication protocol.");
  }

  string CommunicationProtocol::getExecCommand(const string & subCommand,
                                               const string & host,
                                               const string & user) const
  {
    return commandStringFromArgs(getExecCommandArgs(subCommand, host, user));
  }

  int CommunicationProtocol::copyFile(const std::string & sourcePath,
                                      const std::string & sourceHost,
                                      const std::string & sourceUser,
                                      const std::string & destinationPath,
                                      const std::string & destinationHost,
                                      const std::string & destinationUser) const
  {
    string command = commandStringFromArgs(getCopyCommandArgs(sourcePath, sourceHost, sourceUser,
                                                              destinationPath, destinationHost,
                                                              destinationUser));
    LOG(command);
    int status = system(command.c_str());
    return status;
  }

  string CommunicationProtocol::getRemoveSubCommand(const string & path) const
  {
    return string("rm ") + path;
  }

  string CommunicationProtocol::getMakeDirectorySubCommand(const string & path) const
  {
    return string("mkdir -p ") + path;
  }

  int CommunicationProtocol::removeFile(const std::string & path,
                                        const std::string & host,
                                        const std::string & user) const
  {
    string command = getExecCommand(getRemoveSubCommand(path), host, user);
    LOG(command);
    int status = system(command.c_str());
    return status;
  }

  int CommunicationProtocol::makeDirectory(const std::string & path,
                                           const std::string & host,
                                           const std::string & user) const
  {
    string command = getExecCommand(getMakeDirectorySubCommand(path), host, user);
    LOG(command);
    int status = system(command.c_str());
    return status;
  }

  string CommunicationProtocol::commandStringFromArgs(const vector<string> & commandArgs) const
  {
    string commandStr;

    // On Windows we surround the whole command with quotes to avoid problems when
    // we have several quoted arguments.
#ifdef WIN32
    commandStr += "\"";
#endif

    for (unsigned int i=0 ; i<commandArgs.size() ; i++) {
      if (i != 0) commandStr += " ";

      // if the argument contains spaces, we surround it with simple quotes (Linux)
      // or double quotes (Windows)
      if (commandArgs[i].find(' ') != string::npos) {
        commandStr += string("\"") + commandArgs[i] + "\"";
      } else {
        commandStr += commandArgs[i];
      }
    }

#ifdef WIN32
    commandStr += "\"";
#endif

    return commandStr;
  }

  CommunicationProtocolType CommunicationProtocol::getType() const
  {
    return _type;
  }

}
