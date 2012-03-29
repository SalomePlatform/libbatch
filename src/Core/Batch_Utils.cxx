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
 * Batch_Utils.cxx
 *
 *  Created on: 30 jan. 2012
 *  Author : Renaud BARATE - EDF R&D
 */

#include <cstdio>

#include <Batch_config.h>
#include "Batch_Utils.hxx"

#ifdef MSVC
#define popen _popen
#define pclose _pclose
#endif

using namespace std;
namespace Batch {

int Utils::getCommandOutput(const string & command, string & output)
{
  // Reinitialize output
  output = "";

  // Call command
  FILE * fp = popen(command.c_str(), "r");
  if (fp == NULL) {
    return -1;
  }

  // Read the output and store it
  char buf[1024];
  while (fgets(buf, sizeof(buf), fp) != NULL) {
    output += buf;
  }

  // close and get status
  int status = pclose(fp);
  return status;
}

bool Utils::isAbsolutePath(const string & path)
{
  if (path.size() == 0)
    return false;
  return path[0] == '/';
}

}
