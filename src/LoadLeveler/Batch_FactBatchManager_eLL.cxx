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
 *  Batch_FactBatchManager_eLL.cxx :
 *
 *  Created on: 25 nov. 2010
 *  Author : Renaud BARATE - EDF R&D
 */

#include "Batch_ParameterTypeMap.hxx"
#include "Batch_BatchManager_eLL.hxx"
#include "Batch_FactBatchManager_eLL.hxx"

namespace Batch {

  def_Constant(LL_JOBTYPE);

  static FactBatchManager_eLL sFBM_eLL;

  FactBatchManager_eLL::FactBatchManager_eLL() : FactBatchManager_eClient("eLL")
  {
    // Add specific parameters
    ParameterTypeMap::getInstance().addParameter(LL_JOBTYPE, STRING, 1);
  }

  FactBatchManager_eLL::~FactBatchManager_eLL()
  {
    // Nothing to do
  }

  // Functor
  BatchManager * FactBatchManager_eLL::operator() (const char * hostname) const
  {
    // MESSAGE("Building new BatchManager_eLL on host '" << hostname << "'");
    return new BatchManager_eLL(this, hostname);
  }

  BatchManager_eClient * FactBatchManager_eLL::operator() (const char * hostname,
                                                           const char * username,
                                                           CommunicationProtocolType protocolType,
                                                           const char * mpiImpl,
                                                           int nb_proc_per_node) const
  {
    // MESSAGE("Building new BatchManager_eLL on host '" << hostname << "'");
    return new BatchManager_eLL(this, hostname, username, protocolType, mpiImpl, nb_proc_per_node);
  }

}
