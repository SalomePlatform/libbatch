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
 *  Batch_FactBatchManager_eSlurm.cxx :
 *
 *  Created on: 12 may 2011
 *  Author : Renaud BARATE - EDF R&D
 */

#include <Batch_ParameterTypeMap.hxx>

#include "Batch_BatchManager_eSlurm.hxx"
#include "Batch_FactBatchManager_eSlurm.hxx"

namespace Batch {

  static FactBatchManager_eSlurm sFBM_eSlurm;

  FactBatchManager_eSlurm::FactBatchManager_eSlurm() : FactBatchManager_eClient("eSLURM")
  {
  }

  FactBatchManager_eSlurm::~FactBatchManager_eSlurm()
  {
  }

  BatchManager * FactBatchManager_eSlurm::operator() (const char * hostname) const
  {
    // MESSAGE("Building new BatchManager_eSlurm on host '" << hostname << "'");
    return new BatchManager_eSlurm(this, hostname);
  }

  BatchManager_eClient * FactBatchManager_eSlurm::operator() (const char * hostname,
                                                              const char * username,
                                                              CommunicationProtocolType protocolType,
                                                              const char * mpiImpl,
                                                              int nb_proc_per_node) const
  {
    // MESSAGE("Building new BatchManager_eSlurm on host '" << hostname << "'");
    return new BatchManager_eSlurm(this, hostname, username, protocolType, mpiImpl, nb_proc_per_node);
  }

}
