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
 * JobInfo_ePBS.hxx :  emulation of PBS client
 *
 * Auteur : Bernard SECHER - CEA DEN
 * Mail   : mailto:bernard.secher@cea.fr
 * Date   : Thu Apr 24 10:17:22 2008
 * Projet : PAL Salome 
 *
 */

#ifndef _JOBINFO_PBS_H_
#define _JOBINFO_PBS_H_

#include <string>
#include "Batch_RunTimeException.hxx"
#include "Batch_JobInfo.hxx"

namespace Batch {

  class JobInfo_ePBS : public JobInfo
  {
  public:
    // Constructeurs et destructeur
    JobInfo_ePBS() {};
    JobInfo_ePBS(int id,std::string logFile);
    virtual ~JobInfo_ePBS();

    // Constructeur par recopie
    JobInfo_ePBS(const JobInfo_ePBS & jinfo) : JobInfo(jinfo) {};

    // Methodes pour l'interfacage avec Python (SWIG)
    // TODO : supprimer ces methodes et transferer leur definitions dans SWIG
    std::string  __str__() const; // SWIG : affichage en Python
    std::string  __repr__() const { return __str__(); }; // SWIG : affichage en Python

  private:
    // Convertit une date HH:MM:SS en secondes
    long HMStoLong(const std::string &);

  };

}

#endif
