//  Copyright (C) 2007-2011  CEA/DEN, EDF R&D, OPEN CASCADE
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
 * Batch_ParameterTypeMap.cxx :
 *
 * Auteur : Renaud Barate - EDF R&D
 * Date   : Mars 2011
 *
 */

#include <string>
#include <map>

#include "Batch_Constants.hxx"
#include "Batch_InvalidKeyException.hxx"
#include "Batch_ParameterTypeMap.hxx"

using namespace std;

namespace Batch {

  /*!
   * The values of this type map contain the type of the parameters and the maximum
   * number of items in the corresponding Versatile object (0 means any number).
   */
  ParameterTypeMap::ParameterTypeMap()
  {
    // Don't use the string constants in this constructor because they might be uninitialized
    addParameter("ACCOUNT", STRING, 1);
    addParameter("ARGUMENTS", STRING, 0);
    addParameter("CHECKPOINT", LONG, 1);
    addParameter("CKPTINTERVAL", LONG, 1);
    addParameter("CREATIONTIME", LONG, 1);
    addParameter("EGROUP", STRING, 1);
    addParameter("ELIGIBLETIME", LONG, 1);
    addParameter("ENDTIME", LONG, 1);
    addParameter("EUSER", STRING, 1);
    addParameter("EXECUTABLE", STRING, 1);
    addParameter("EXECUTIONHOST", STRING, 0);
    addParameter("EXITCODE", LONG, 1);
    addParameter("HOLD", LONG, 1);
    addParameter("ID", STRING, 1);
    addParameter("INFILE", COUPLE, 0);
    addParameter("MAIL", STRING, 1);
    addParameter("MAXCPUTIME", LONG, 1);
    addParameter("MAXDISKSIZE", LONG, 1);
    addParameter("MAXRAMSIZE", LONG, 1);
    addParameter("MAXWALLTIME", LONG, 1);
    addParameter("MODIFICATIONTIME", LONG, 1);
    addParameter("NAME", STRING, 1);
    addParameter("NBPROC", LONG, 1);
    addParameter("OUTFILE", COUPLE, 0);
    addParameter("PID", LONG, 1);
    addParameter("QUEUE", STRING, 1);
    addParameter("QUEUEDTIME", LONG, 1);
    addParameter("SERVER", STRING, 1);
    addParameter("STARTTIME", LONG, 1);
    addParameter("STATE", STRING, 1);
    addParameter("TEXT", STRING, 1);
    addParameter("TMPDIR", STRING, 1);
    addParameter("USEDCPUTIME", LONG, 1);
    addParameter("USEDDISKSIZE", LONG, 1);
    addParameter("USEDRAMSIZE", LONG, 1);
    addParameter("USEDWALLTIME", LONG, 1);
    addParameter("USER", STRING, 1);
    addParameter("WORKDIR", STRING, 1);
    addParameter("HOMEDIR", STRING, 1);
  }

  ParameterTypeMap::~ParameterTypeMap()
  {
  }

  /*!
   * Returns the ParameterTypeMap singleton.
   * We use the construct-on-first-use idiom here because otherwise there could be a problem with
   * the initialization order between the factory singletons and this type map. Note that since the
   * destructors do not depend on the other objects, the order is not important for destruction.
   */
  ParameterTypeMap& ParameterTypeMap::getInstance () {
    static ParameterTypeMap instance;
    return instance;
  }

  bool ParameterTypeMap::hasKey(const string & key) const
  {
    return (_map.find(key) != _map.end());
  }

  const ParameterType & ParameterTypeMap::operator[](const string & key) const
  {
    map<string, ParameterType>::const_iterator it = _map.find(key);
    if (it == _map.end()) throw InvalidKeyException(key);
    return it->second;
  }

  void ParameterTypeMap::addParameter(const std::string & key, DiscriminatorType type, int maxelem)
  {
    if (hasKey(key)) throw InvalidKeyException(key + " is already present in type map");
    _map[key].type = type;
    _map[key].maxelem = maxelem;
  }

}
