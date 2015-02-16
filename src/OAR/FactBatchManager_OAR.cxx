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

#include <string.h>
#include "BatchManager_OAR.hxx"
#include "FactBatchManager_OAR.hxx"

namespace Batch
{
	static FactBatchManager_OAR sFBM_eOAR;

	// Constructeur
	FactBatchManager_OAR::FactBatchManager_OAR() : FactBatchManager("OAR")
	{
		// Nothing to do
	}

	// Destructeur
	FactBatchManager_OAR::~FactBatchManager_OAR()
	{
		// Nothing to do
	}

	BatchManager * FactBatchManager_OAR::operator() (const char * hostname,
			const char * username,
			CommunicationProtocolType protocolType,
			const char * mpiImpl) const
	{
		return new BatchManager_OAR(this, hostname, username, protocolType, mpiImpl);
	}
}
