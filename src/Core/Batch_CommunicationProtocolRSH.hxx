//  Copyright (C) 2007-2009  CEA/DEN, EDF R&D, OPEN CASCADE
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
 *  Batch_CommunicationProtocolRSH.hxx :
 *
 *  Created on: 14 sept. 2009
 *  Author : Renaud BARATE - EDF R&D
 */

#ifndef _BATCHCOMMUNICATIONPROTOCOLRSH_H_
#define _BATCHCOMMUNICATIONPROTOCOLRSH_H_

#include <string>
#include <vector>

#include "Batch_Defines.hxx"
#include "Batch_CommunicationProtocol.hxx"

namespace Batch {

  class BATCH_EXPORT CommunicationProtocolRSH : public CommunicationProtocol
  {
  public:
    std::vector<std::string> getExecCommandArgs(const std::string & subCommand,
                                                const std::string & host,
                                                const std::string & user) const;

    std::vector<std::string> getCopyCommandArgs(const std::string & sourcePath,
                                                const std::string & sourceHost,
                                                const std::string & sourceUser,
                                                const std::string & destinationPath,
                                                const std::string & destinationHost,
                                                const std::string & destinationUser) const;

#ifdef WIN32

    // On Windows, we must reimplement copyFile because there are problems with absolute
    // paths containing a colon.
    int copyFile(const std::string & sourcePath,
                 const std::string & sourceHost,
                 const std::string & sourceUser,
                 const std::string & destinationPath,
                 const std::string & destinationHost,
                 const std::string & destinationUser) const;

  protected:

    char getDriveLetter(const std::string & path, std::string * pathWithoutDrive = NULL) const;

#endif

  };

}

#endif
