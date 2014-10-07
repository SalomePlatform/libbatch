//  Copyright (C) 2007-2014  CEA/DEN, EDF R&D, OPEN CASCADE
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
 *  JobInfo_LL.hxx :
 *
 *  Created on: 30 nov. 2010
 *  Author : Renaud BARATE - EDF R&D
 */

#ifndef _JOBINFO_ELL_H_
#define _JOBINFO_ELL_H_

#include <string>

#include <JobInfo.hxx>

namespace Batch {

  class JobInfo_LL : public JobInfo
  {
  public:
    JobInfo_LL(const std::string & id, const std::string & output);
    virtual ~JobInfo_LL();

  };

}

#endif
