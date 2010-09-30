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
 * BatchManager_eSSH.hxx : emulation of SSH client
 *
 * Auteur : André RIBES - EDF R&D
 * Date   : Octobre 2009
 */

#ifndef _BATCHMANAGER_ESSH_H_
#define _BATCHMANAGER_ESSH_H_

#include "Batch_Defines.hxx"
#include "Batch_JobId.hxx"
#include "Batch_JobInfo.hxx"
#include "Batch_FactBatchManager.hxx"
#include "Batch_BatchManager_eClient.hxx"
#include "Batch_BatchManager_Local.hxx"

namespace Batch {

  class BATCH_EXPORT BatchManager_eSSH : 
    virtual public BatchManager_eClient,
    virtual public BatchManager_Local
  {
  public:
    // Constructeur et destructeur
    BatchManager_eSSH(const FactBatchManager * parent, const char * host="localhost",
                      const char * username="",
                      CommunicationProtocolType protocolType = SSH, const char * mpiImpl="nompi"); // connexion a la machine host
    virtual ~BatchManager_eSSH();

    // Recupere le nom du serveur par defaut
    // static string BatchManager_LSF::getDefaultServer();

    // Methodes pour le controle des jobs
    virtual const JobId submitJob(const Job & job); // soumet un job au gestionnaire
    virtual void deleteJob(const JobId & jobid);    // retire un job du gestionnaire
    virtual JobInfo queryJob(const JobId & jobid);  // renvoie l'etat du job

    // Non implanté...
    virtual void holdJob(const JobId & jobid); // suspend un job en file d'attente
    virtual void releaseJob(const JobId & jobid); // relache un job suspendu
    virtual void alterJob(const JobId & jobid, const Parametre & param, const Environnement & env); // modifie un job en file d'attente
    virtual void alterJob(const JobId & jobid, const Parametre & param); // modifie un job en file d'attente
    virtual void alterJob(const JobId & jobid, const Environnement & env); // modifie un job en file d'attente


  protected:
    void buildBatchScript(const Job & job);

  private:

#ifdef SWIG
  public:
    // Recupere le l'identifiant d'un job deja soumis au BatchManager
    //virtual const JobId getJobIdByReference(const string & ref) { return BatchManager::getJobIdByReference(ref); }
    virtual const JobId getJobIdByReference(const char * ref) { return BatchManager::getJobIdByReference(ref); }
#endif
  };
}

#endif
