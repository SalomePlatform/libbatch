//  Copyright (C) 2012-2015  INRIA
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

#ifndef _JOBINFO_OAR_H_
#define _JOBINFO_OAR_H_

#include <string.h>
#include "RunTimeException.hxx"
#include "JobInfo.hxx"

namespace Batch
{
	class JobInfo_OAR : public JobInfo
	{
		public:
			// Constructeur
			JobInfo_OAR() {};

			// Constructeur
			JobInfo_OAR(const std::string & id, const std::string & queryOutput);

			// Destructeur
			virtual ~JobInfo_OAR();

			// Constructeur par recopie
			JobInfo_OAR(const JobInfo_OAR & jinfo) : JobInfo(jinfo) {};

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
