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
 * JobInfo_eSSH.cxx :  emulation of SSH client
 *
 * Auteur : André RIBES - EDF R&D
 * Date   : Octobre 2009
 */

#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream>

#include "Batch_Constants.hxx"
#include "Batch_Parametre.hxx"
#include "Batch_Environnement.hxx"
#include "Batch_RunTimeException.hxx"
#include "Batch_APIInternalFailureException.hxx"
#include "Batch_JobInfo_eSSH.hxx"

using namespace std;

namespace Batch {

  // Constructeurs
  JobInfo_eSSH::JobInfo_eSSH(int id, string status) : JobInfo()
  {
    // On remplit les membres _param et _env
    ostringstream oss;
    oss << id;
    _param[ID] = oss.str();
    _param[STATE] = status;
  }

  // Destructeur
  JobInfo_eSSH::~JobInfo_eSSH()
  {
    // Nothing to do
  }

  // Methode pour l'interfacage avec Python (SWIG) : affichage en Python
  string JobInfo_eSSH::__str__() const
  {
    ostringstream sst;
    sst << "<JobInfo_eSSH (" << this << ") :" << endl;
    sst << " ID = " <<_param[ID] << endl;
    sst << " STATE = " <<_param[STATE] << endl;

    return sst.str();
  }


}
