// Copyright (C) 2007-2021  CEA/DEN, EDF R&D, OPEN CASCADE
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
 * CommandsOverloader.cxx :
 *
 * Author : Margarita KARPUNINA - OCC
 * Date   : October 2022
 *
 */

#include <fstream>
#include <set>
#include <sstream>

#include <libbatch_config.h>

#include "CommandsOverloader.hxx"

#include "RunTimeException.hxx"

using namespace std;


namespace Batch {

  static set<string> supportedCmds({"RM", "SH", "CP", "MKDIR", "RSH", "RCP", "SSH", "SCP", "RSYNC"});

  /*!
   * Constructor.
   */
  CommandsOverloader::CommandsOverloader()
  {    
  }

  /*!
   * Destructor.
   */
  CommandsOverloader::~CommandsOverloader()
  {
  }

  /*!
   * Return the CommandsOverloader singleton.
   */
  CommandsOverloader& CommandsOverloader::getInstance () {
    static CommandsOverloader instance;
    return instance;
  }

  /*!
   * Return an overridden definition of RM command.
   */
  string CommandsOverloader::RM_Command() {
    string cmd = CMD_Command("RM");
    return (cmd.empty() ? RM_COMMAND : cmd);
  }

  /*!
   * Return an overridden definition of SH command.
   */
  string CommandsOverloader::SH_Command() {
    string cmd = CMD_Command("SH");
    return (cmd.empty() ? SH_COMMAND : cmd);
  }

  /*!
   * Return an overridden definition of CP command.
   */
  string CommandsOverloader::CP_Command() {
    string cmd = CMD_Command("CP");
    return (cmd.empty() ? CP_COMMAND : cmd);
  }

  /*!
   * Return an overridden definition of MKDIR command.
   */
  string CommandsOverloader::MKDIR_Command() {
    string cmd = CMD_Command("MKDIR");
    return (cmd.empty() ? MKDIR_COMMAND : cmd);
  }

  /*!
   * Return an overridden definition of RSH command.
   */
  string CommandsOverloader::RSH_Command() {
    string cmd = CMD_Command("RSH");
    return (cmd.empty() ? RSH_COMMAND : cmd);
  }

  /*!
   * Return an overridden definition of RCP command.
   */
  string CommandsOverloader::RCP_Command() {
    string cmd = CMD_Command("RCP");
    return (cmd.empty() ? RCP_COMMAND : cmd);
  }

  /*!
   * Return an overridden definition of SSH command.
   */
  string CommandsOverloader::SSH_Command() {
    string cmd = CMD_Command("SSH");
    return (cmd.empty() ? SSH_COMMAND : cmd);
  }

  /*!
   * Return an overridden definition of SCP command.
   */
  string CommandsOverloader::SCP_Command() {
    string cmd = CMD_Command("SCP");
    return (cmd.empty() ? SCP_COMMAND : cmd);
  }

  /*!
   * Return an overridden definition of RSYNC command.
   */
  string CommandsOverloader::RSYNC_Command() {
    string cmd = CMD_Command("RSYNC");
     return (cmd.empty() ? RSYNC_COMMAND : cmd);
  }

  /*!
   * Parse text file with the given file name and fill in the map of 
   * <CMD> - <associated command desired by the user at runtime>.
   * \param theFilename the name of text file to be parsed
   */
  void CommandsOverloader::parse(const std::string & theFilename) {
    ifstream fileStream(theFilename.c_str());
    if (!fileStream) {
      stringstream errMsg;
      errMsg << "Can't open file with overridden commands definitions " << theFilename;
      throw RunTimeException(errMsg.str());
    }
    string line;
    int lineNumber = 1;
    while (getline(fileStream, line)) {
      string str = line;
      if (!str.empty()) {
        // Find ' ' symbol and split the line
        size_t pos = str.find_first_of(' ');
        if (pos == string::npos) {
          stringstream errMsg;
          errMsg << "Wrong format of " << theFilename << " file on line " << lineNumber
                 << ": Syntax error (missing ' ' character between key and value): " << line;
          throw RunTimeException(errMsg.str());
        } else {
          string key = str.substr(0, pos);
          string value = str.substr(pos+1);

          // Check non-completeness of the file.
          string trimmedKey = trim(key);
          string trimmedValue = trim(value);
          if (trimmedKey.empty() || trimmedValue.empty()) {
            stringstream errMsg;
            errMsg << "Wrong format of " << theFilename << " file on line " << lineNumber
                   << ": The non-completeness of the file: " << line;
            throw RunTimeException(errMsg.str());
          }

          // Check the presence of an unsupported command.
          if (supportedCmds.find(trimmedKey) == supportedCmds.end()) {
            stringstream errMsg;
            errMsg << "Wrong format of " << theFilename << " file on line " << lineNumber
                   << ": The presence of an unsupported command: " << trimmedKey;
            throw RunTimeException(errMsg.str());
          }

          if (!hasKey(trimmedKey)) {
            _cmdmap[trimmedKey] = trimmedValue;
          }
          else {
            // Redifinition of already overloaded command is found.
            stringstream errMsg;
            errMsg << "Wrong format of " << theFilename << " file on line " << lineNumber
                   <<": A repetition of the " << trimmedKey << " key in several lines.";
            throw RunTimeException(errMsg.str());
          }
        }
      }
      ++lineNumber;
    }
    fileStream.close();
  }

  /*!
   * Strip leading and trailing spaces in the given string.
   * \param theStr string to be stripped
   * \return stripped string
   */
  std::string CommandsOverloader::trim(const std::string & theStr) const noexcept
  {
    size_t beg = theStr.find_first_not_of(" \t");
    if (beg == string::npos) beg = 0;
    size_t end = theStr.find_last_not_of(" \t");
    return theStr.substr(beg, end-beg+1);
  }

  /*!
   * Return an overridden definition of the given command.
   * \param theKey command name
   * \return overridden definition of the given command
   */
  string CommandsOverloader::CMD_Command(const std::string & theKey) {
    if (_cmdmap.empty()) {
      const char * filename = getenv("LIBBATCH_OVERRIDE_CMDS");
      if (filename != NULL) {
        // Environment variable LIBBATCH_OVERRIDE_CMDS is defined.
        // Parse text file pointed by LIBBATCH_OVERRIDE_CMDS environment variable.
        parse(filename);
        // Note: If environment variable LIBBATCH_OVERRIDE_CMDS is not defined,
        // use command configuration established at compile time.
      }
    }

    return (hasKey(theKey) ? _cmdmap[theKey] : string());
  }

  /*!
   * Check, if commands map contains the given command key.
   * \param theKey command name
   * \return true, if command key exists in the map
   */
  bool CommandsOverloader::hasKey(const string & theKey) const
  {
    return (_cmdmap.find(theKey) != _cmdmap.end());
  }

}
