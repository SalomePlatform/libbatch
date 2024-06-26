// Copyright (C) 2007-2021  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
/*
 * Couple.hxx :
 *
 * Auteur : Ivan DUTKA-MALEN - EDF R&D
 * Mail   : mailto:ivan.dutka-malen@der.edf.fr
 * Date   : Tue Dec  9 15:00:35 2003
 * Projet : Salome 2
 *
 */

#ifndef _COUPLE_H_
#define _COUPLE_H_

#include "Defines.hxx"

#include <string>

namespace Batch {

  class BATCH_EXPORT Couple
  {
  public:
    // Constructeur standard
    Couple(const std::string & local="", const std::string & remote="") : _local(local), _remote(remote) {}

    // Constructeur par recopie
    Couple(const Couple & C) : _local(C._local), _remote(C._remote) {}

    // Destructeur
    virtual ~Couple() {}

    // Operateur pour l'affichage sur un stream
    BATCH_EXPORT friend std::ostream & operator << (std::ostream & os, const Couple & cp);

    // Operateur d'affectation
    virtual Couple & operator =(const Couple &);

    // Conversion en chaine
    virtual std::string str() const;

    // Accesseurs
    virtual std::string getLocal() const { return _local; }
    virtual std::string getRemote() const { return _remote; }

  protected:
    std::string _local;  // chemin d'acces au fichier local
    std::string _remote; // chemin d'acees au fichier distant

  private:

  };

}

#endif
