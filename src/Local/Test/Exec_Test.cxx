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
 * Exec_Test.cxx :
 *
 * Author : Renaud BARATE - EDF R&D
 * Date   : May 2009
 *
 */

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

using namespace std;

int main(int argc, char** argv)
{
  if (argc != 4) {
    cerr << "Exec_Test expects three parameters, usage: Exec_Test <scriptA> <scriptB> <result>" << endl;
    return 1;
  }

  const char * scriptAFileName = argv[1];
  const char * scriptBFileName = argv[2];
  const char * resultFileName = argv[3];

  ifstream scriptAStream(scriptAFileName);
  std::string line;
  int a = 0;
  while (getline(scriptAStream, line)) {
    if (line.compare(0, 2, string("a=")) == 0) {
      a = strtol(line.substr(2).c_str(), NULL, 10);
    }
  }
  scriptAStream.close();
  if (a == 0) {
    cerr << "Exec_Test couldn't parse value \"a\" in " << scriptAFileName << endl;
    return 1;
  }

  ifstream scriptBStream(scriptBFileName);
  int b = 0;
  while (getline(scriptBStream, line)) {
    if (line.compare(0, 2, string("b=")) == 0) {
      b = strtol(line.substr(2).c_str(), NULL, 10);
    }
  }
  scriptBStream.close();
  if (b == 0) {
    cerr << "Exec_Test couldn't parse value \"b\" in " << scriptBFileName << endl;
    return 1;
  }

  int c = a * b;
  ofstream resultStream(resultFileName);
  resultStream << "c = " << c;
  resultStream.close();
  return 0;
}
