//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
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
 * Parametre.hxx :
 *
 * Auteur : Ivan DUTKA-MALEN - EDF R&D
 * Date   : Septembre 2003
 * Projet : SALOME 2
 *
 */

#ifndef _PARAMETRE_H_
#define _PARAMETRE_H_

#include <map>
#include <string>

#include "Batch_Defines.hxx"
#include "Batch_Constants.hxx"
#include "Batch_InvalidKeyException.hxx"
#include "Batch_Versatile.hxx"

// TODO: These declarations are here only to avoid breaking Kernel compilation for now.
// They must be removed as soon as possible.
#define def_extern_MapKey(mk) extern BATCH_EXPORT const std::string & mk;
#define def_static_MapKey(mk) const std::string & mk = #mk;

namespace Batch {

  class BATCH_EXPORT Parametre : public std::map< std::string, Versatile >
  {
  public:
    // Constructeur standard
    Parametre();

    // Constructeur par recopie
    Parametre(const Parametre & PM);

    // Operateur de recherche dans la map
    Versatile & operator [] (const std::string &);
    const Versatile & operator [] (const std::string &) const;

    // Operateur d'affectation
    Parametre & operator =(const Parametre & PM);

  protected:
    std::map< std::string, TypeParam > TypeMap; // map interne servant a controler le type de la valeur associee a chaque clef

  private:

  };

}

// TODO: These declarations in global namespace are here only to avoid breaking Kernel compilation for now.
// They must be removed as soon as possible.
def_extern_MapKey(ACCOUNT)
def_extern_MapKey(ARGUMENTS)
def_extern_MapKey(CHECKPOINT)
def_extern_MapKey(CKPTINTERVAL)
def_extern_MapKey(CREATIONTIME)
def_extern_MapKey(EGROUP)
def_extern_MapKey(ELIGIBLETIME)
def_extern_MapKey(ENDTIME)
def_extern_MapKey(EUSER)
def_extern_MapKey(EXECUTABLE)
def_extern_MapKey(EXECUTIONHOST)
def_extern_MapKey(EXITCODE)
def_extern_MapKey(HOLD)
def_extern_MapKey(ID)
def_extern_MapKey(INFILE)
def_extern_MapKey(MAIL)
def_extern_MapKey(MAXCPUTIME)
def_extern_MapKey(MAXDISKSIZE)
def_extern_MapKey(MAXRAMSIZE)
def_extern_MapKey(MAXWALLTIME)
def_extern_MapKey(MODIFICATIONTIME)
def_extern_MapKey(NAME)
def_extern_MapKey(NBPROC)
def_extern_MapKey(OUTFILE)
def_extern_MapKey(PID)
def_extern_MapKey(QUEUE)
def_extern_MapKey(QUEUEDTIME)
def_extern_MapKey(SERVER)
def_extern_MapKey(STARTTIME)
def_extern_MapKey(STATE)
def_extern_MapKey(TEXT)
def_extern_MapKey(TMPDIR)
def_extern_MapKey(USEDCPUTIME)
def_extern_MapKey(USEDDISKSIZE)
def_extern_MapKey(USEDRAMSIZE)
def_extern_MapKey(USEDWALLTIME)
def_extern_MapKey(USER)
def_extern_MapKey(WORKDIR)
def_extern_MapKey(HOMEDIR)

#endif
