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
 * FactBatchManager_Local.cxx : 
 *
 * Auteur : Ivan DUTKA-MALEN - EDF R&D
 * Date   : Septembre 2004
 * Projet : SALOME 2
 *
 * Refactored by Renaud Barate (EDF R&D) in September 2009 to use
 * CommunicationProtocol classes and merge Local_SH, Local_RSH and Local_SSH batch
 * managers.
 *
 */

#include "Batch_config.h"

#include "Batch_BatchManager_Local.hxx"
#include "Batch_FactBatchManager_Local.hxx"

namespace Batch {

#ifdef HAS_SH
  static FactBatchManager_Local sFBM_Local_SH("SH", SH);
#endif

#ifdef HAS_RSH
  static FactBatchManager_Local sFBM_Local_RSH("RSH", RSH);
#endif

#ifdef HAS_SSH
  static FactBatchManager_Local sFBM_Local_SSH("SSH", SSH);
#endif

  // Constructeur
  FactBatchManager_Local::FactBatchManager_Local(const char * name,
                                                 CommunicationProtocolType protocolType)
    : FactBatchManager(name),
      _protocolType(protocolType)
  {
    // Nothing to do
  }

  // Destructeur
  FactBatchManager_Local::~FactBatchManager_Local()
  {
    // Nothing to do
  }

  // Functor
  BatchManager * FactBatchManager_Local::operator() (const char * hostname) const
  {
    // MESSAGE("Building new BatchManager_Local on host '" << hostname << "'");
    return new BatchManager_Local(this, hostname, _protocolType);
  }

}
