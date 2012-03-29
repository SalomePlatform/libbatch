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
 * FactBatchManager_eCCC.cxx : 
 *
 * Auteur : Bernard SECHER - CEA DEN
 * Date   : Avril 2010
 * Projet : PAL Salome
 *
 */

#include "Batch_BatchManager_eCCC.hxx"
#include "Batch_FactBatchManager_eCCC.hxx"

namespace Batch {

  static FactBatchManager_eCCC sFBM_eCCC;

  // Constructeur
  FactBatchManager_eCCC::FactBatchManager_eCCC() : FactBatchManager_eClient("eCCC")
  {
    // Nothing to do
  }

  // Destructeur
  FactBatchManager_eCCC::~FactBatchManager_eCCC()
  {
    // Nothing to do
  }

  // Functor
  BatchManager * FactBatchManager_eCCC::operator() (const char * hostname) const
  {
    // MESSAGE("Building new BatchManager_CCC on host '" << hostname << "'");
    return new BatchManager_eCCC(this, hostname);
  }

  BatchManager_eClient * FactBatchManager_eCCC::operator() (const char * hostname,
                                                            const char * username,
                                                            CommunicationProtocolType protocolType,
                                                            const char * mpiImpl,
							    int nb_proc_per_node) const
  {
    // MESSAGE("Building new BatchManager_CCC on host '" << hostname << "'");
    return new BatchManager_eCCC(this, hostname, username, protocolType, mpiImpl);
  }

}
