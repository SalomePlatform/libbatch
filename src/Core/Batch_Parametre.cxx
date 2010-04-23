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
 * Parametre.cxx :
 *
 * Auteur : Ivan DUTKA-MALEN - EDF R&D
 * Date   : Septembre 2003
 * Projet : SALOME 2
 *
 */

#include "Batch_Versatile.hxx"
#include "Batch_InvalidKeyException.hxx"
#include "Batch_Parametre.hxx"

using namespace std;

namespace Batch {

  // Constructeur standard
  // La map interne TypeMap possede les memes clefs que la map principale, mais les
  // valeurs associees contiennent le type des clefs de la map principale ainsi que
  // le nombre de valeurs autorisees dans l'objet Versatile (0=nombre quelconque,
  // sinon valeur precisee)
  Parametre::Parametre() : map< string, Versatile >()
  {
    TypeMap[ACCOUNT].type = STRING;
    TypeMap[ACCOUNT].maxelem = 1;

    TypeMap[ARGUMENTS].type = STRING;
    TypeMap[ARGUMENTS].maxelem = 0;

    TypeMap[CHECKPOINT].type = LONG;
    TypeMap[CHECKPOINT].maxelem = 1;

    TypeMap[CKPTINTERVAL].type = LONG;
    TypeMap[CKPTINTERVAL].maxelem = 1;

    TypeMap[CREATIONTIME].type = LONG;
    TypeMap[CREATIONTIME].maxelem = 1;

    TypeMap[EGROUP].type = STRING;
    TypeMap[EGROUP].maxelem = 1;

    TypeMap[ELIGIBLETIME].type = LONG;
    TypeMap[ELIGIBLETIME].maxelem = 1;

    TypeMap[ENDTIME].type = LONG;
    TypeMap[ENDTIME].maxelem = 1;

    TypeMap[EUSER].type = STRING;
    TypeMap[EUSER].maxelem = 1;

    TypeMap[EXECUTABLE].type = STRING;
    TypeMap[EXECUTABLE].maxelem = 1;

    TypeMap[EXECUTIONHOST].type = STRING;
    TypeMap[EXECUTIONHOST].maxelem = 0;

    TypeMap[EXITCODE].type = LONG;
    TypeMap[EXITCODE].maxelem = 1;

    TypeMap[HOLD].type = LONG;
    TypeMap[HOLD].maxelem = 1;

    TypeMap[ID].type = STRING;
    TypeMap[ID].maxelem = 1;

    TypeMap[INFILE].type = COUPLE;
    TypeMap[INFILE].maxelem = 0;

    TypeMap[MAIL].type = STRING;
    TypeMap[MAIL].maxelem = 1;

    TypeMap[MAXCPUTIME].type = LONG;
    TypeMap[MAXCPUTIME].maxelem = 1;

    TypeMap[MAXDISKSIZE].type = LONG;
    TypeMap[MAXDISKSIZE].maxelem = 1;

    TypeMap[MAXRAMSIZE].type = LONG;
    TypeMap[MAXRAMSIZE].maxelem = 1;

    TypeMap[MAXWALLTIME].type = LONG;
    TypeMap[MAXWALLTIME].maxelem = 1;

    TypeMap[MODIFICATIONTIME].type = LONG;
    TypeMap[MODIFICATIONTIME].maxelem = 1;

    TypeMap[NAME].type = STRING;
    TypeMap[NAME].maxelem = 1;

    TypeMap[NBPROC].type = LONG;
    TypeMap[NBPROC].maxelem = 1;

    TypeMap[OUTFILE].type = COUPLE;
    TypeMap[OUTFILE].maxelem = 0;

    TypeMap[PID].type = LONG;
    TypeMap[PID].maxelem = 1;

    TypeMap[QUEUE].type = STRING;
    TypeMap[QUEUE].maxelem = 1;

    TypeMap[QUEUEDTIME].type = LONG;
    TypeMap[QUEUEDTIME].maxelem = 1;

    TypeMap[SERVER].type = STRING;
    TypeMap[SERVER].maxelem = 1;

    TypeMap[STARTTIME].type = LONG;
    TypeMap[STARTTIME].maxelem = 1;

    TypeMap[STATE].type = STRING;
    TypeMap[STATE].maxelem = 1;

    TypeMap[TEXT].type = STRING;
    TypeMap[TEXT].maxelem = 1;

    TypeMap[TMPDIR].type = STRING;
    TypeMap[TMPDIR].maxelem = 1;

    TypeMap[USEDCPUTIME].type = LONG;
    TypeMap[USEDCPUTIME].maxelem = 1;

    TypeMap[USEDDISKSIZE].type = LONG;
    TypeMap[USEDDISKSIZE].maxelem = 1;

    TypeMap[USEDRAMSIZE].type = LONG;
    TypeMap[USEDRAMSIZE].maxelem = 1;

    TypeMap[USEDWALLTIME].type = LONG;
    TypeMap[USEDWALLTIME].maxelem = 1;

    TypeMap[USER].type = STRING;
    TypeMap[USER].maxelem = 1;

    TypeMap[WORKDIR].type = STRING;
    TypeMap[WORKDIR].maxelem = 1;

    TypeMap[HOMEDIR].type = STRING;
    TypeMap[HOMEDIR].maxelem = 1;
  }

  // Operateur de recherche dans la map
  // Cet operateur agit sur les objets NON CONSTANTS, il autorise la modification de
  // la valeur associ�e � la clef car il retourne une reference non constante
  Versatile & Parametre::operator [] (const string & mk)
  {
    // On controle que la clef est valide
    if (TypeMap.find(mk) == TypeMap.end()) throw InvalidKeyException(mk.c_str());

    // On recherche la valeur associee...
    Versatile & V = map< string, Versatile >::operator [] (mk);

    // ... et on l'initialise systematiquement
    // ATTENTION : si un probleme de type survient (ie, on stocke une valeur d'un type
    // different de celui inscrit dans TypeMap) une exception TypeMismatchException est
    // levee
    V.setName(mk);
    V.setType(TypeMap[mk].type);
    V.setMaxSize(TypeMap[mk].maxelem);

    return V;
  }

  // Operateur de recherche dans la map
  // Cet operateur agit sur les objets CONSTANTS
  const Versatile & Parametre::operator [] (const string & mk) const
  {
    // On controle que la clef est valide
    if (TypeMap.find(mk) == TypeMap.end()) throw InvalidKeyException(mk.c_str());

    // On recherche la valeur associee
    Parametre::const_iterator it = find(mk);
    if (it == end()) throw InvalidKeyException(mk.c_str());
    const Versatile & V = (*it).second;

    return V;
  }

  // Operateur d'affectation
  Parametre & Parametre::operator =(const Parametre & PM)
  {
    // On ne reaffecte pas l'objet a lui-meme, sinon aie, aie, aie
    if (this == &PM) return *this;

    // On efface toute la map
    erase(begin(), end());

    // On recopie la map interne
    // Meme si cela ne sert a rien pour le moment car les maps internes sont identiques,
    // il n'est pas exclu que dans un avenir proche elles puissent etre differentes
    (*this).TypeMap = PM.TypeMap;

    // On recree la structure interne de la map avec les valeurs de celle passee en argument
    Parametre::const_iterator it;
    for(it=PM.begin(); it!=PM.end(); it++)
      insert(make_pair( (*it).first ,
			Versatile( (*it).second)
			) );

    return *this;
  }

  // Constructeur par recopie
  Parametre::Parametre(const Parametre & PM) : map< string, Versatile >()
  {
    // inutile car l'objet est vierge : il vient d'etre cree
    // On efface toute la map
    // erase(begin(), end());

    // On recopie la map interne
    (*this).TypeMap = PM.TypeMap;

    // On cree la structure interne de la map avec les valeurs de celle passee en argument
    Parametre::const_iterator it;
    for(it=PM.begin();
	it!=PM.end();
	it++)
      insert(
	     make_pair(
		       (*it).first ,
		       Versatile( (*it).second)
		       ) );
  }

  //   map< string, TypeParam > Parametre::getTypeMap() const
  //   {
  //     return TypeMap;
  //   }

}
