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
 * FactBatchManager_eClient.hxx : emulation of client
 *
 * Auteur : Bernard SECHER - CEA DEN
 * Mail   : mailto:bernard.secher@cea.fr
 * Date   : Thu Apr 24 10:17:22 2008
 * Projet : PAL Salome 
 *
 */

#ifndef _FACTBATCHMANAGER_ECLIENT_H_
#define _FACTBATCHMANAGER_ECLIENT_H_

#include <string>

#include "Batch_FactBatchManager.hxx"
#include "Batch_BatchManager_eClient.hxx"
#include "Batch_CommunicationProtocol.hxx"

namespace Batch {

  class BATCH_EXPORT FactBatchManager_eClient : public FactBatchManager
  {
  public:
    // Constructeur et destructeur
    FactBatchManager_eClient(const std::string & type);
    virtual ~FactBatchManager_eClient();

    virtual Batch::BatchManager_eClient * operator() (const char * hostname,
                                                      CommunicationProtocolType protocolType,
                                                      const char * mpi,
						      int nb_proc_per_node = 1) const = 0;

  protected:

  private:

  };

}

#endif
