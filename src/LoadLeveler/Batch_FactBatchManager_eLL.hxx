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
 *  Batch_FactBatchManager_eLL.hxx :
 *
 *  Created on: 25 nov. 2010
 *  Author : Renaud BARATE - EDF R&D
 */

#ifndef _FACTBATCHMANAGER_ELL_H_
#define _FACTBATCHMANAGER_ELL_H_

#include <Batch_Defines.hxx>
#include <Batch_Constants.hxx>

#include "Batch_BatchManager_eClient.hxx"
#include "Batch_FactBatchManager_eClient.hxx"

namespace Batch {

  decl_extern_Constant(LL_JOBTYPE);

  class BatchManager_eLL;

  class BATCH_EXPORT FactBatchManager_eLL : public FactBatchManager_eClient
  {
  public:
    // Constructeur et destructeur
    FactBatchManager_eLL();
    virtual ~FactBatchManager_eLL();

    virtual BatchManager * operator() (const char * hostname) const;
    virtual BatchManager_eClient * operator() (const char * hostname,
                                               const char * username,
                                               CommunicationProtocolType protocolType,
                                               const char * mpiImpl,
                                               int nb_proc_per_node = 1) const;

  };

}

#endif
