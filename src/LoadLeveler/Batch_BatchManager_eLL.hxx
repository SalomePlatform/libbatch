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
 *  Batch_BatchManager_eLL.hxx :
 *
 *  Created on: 25 nov. 2010
 *  Author : Renaud BARATE - EDF R&D
 */

#ifndef _BATCHMANAGER_ELL_H_
#define _BATCHMANAGER_ELL_H_

#include <string>

#include "Batch_Defines.hxx"
#include "Batch_JobId.hxx"
#include "Batch_JobInfo.hxx"
#include "Batch_FactBatchManager.hxx"
#include "Batch_BatchManager_eClient.hxx"

namespace Batch {

  class BATCH_EXPORT BatchManager_eLL : public BatchManager_eClient
  {
  public:
    BatchManager_eLL(const FactBatchManager * parent, const char * host = "localhost",
                     const char * username = "",
                     CommunicationProtocolType protocolType = SSH, const char * mpiImpl = "nompi",
                     int nb_proc_per_node=1);
    virtual ~BatchManager_eLL();

    // Methods to control jobs
    virtual const JobId submitJob(const Job & job);
    virtual void deleteJob(const JobId & jobid);
    virtual void holdJob(const JobId & jobid);
    virtual void releaseJob(const JobId & jobid);
    virtual void alterJob(const JobId & jobid, const Parametre & param, const Environnement & env);
    virtual void alterJob(const JobId & jobid, const Parametre & param);
    virtual void alterJob(const JobId & jobid, const Environnement & env);
    virtual JobInfo queryJob(const JobId & jobid);
    virtual const JobId addJob(const Job & job, const std::string reference);

  protected:
    std::string buildCommandFile(const Job & job);

    int _nb_proc_per_node;

  };

}

#endif
