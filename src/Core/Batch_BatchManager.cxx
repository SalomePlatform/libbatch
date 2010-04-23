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
 * BatchManager.cxx :
 *
 * Auteur : Ivan DUTKA-MALEN - EDF R&D
 * Date   : Septembre 2003
 * Projet : SALOME 2
 *
 */

#include <iostream>
#include <sstream>
#include <string>
#ifdef WIN32
# include<winsock2.h>
#else
# include <netdb.h>
#endif

#include "Batch_Job.hxx"
#include "Batch_JobId.hxx"
#include "Batch_JobInfo.hxx"
#include "Batch_InvalidArgumentException.hxx"
#include "Batch_FactBatchManager.hxx"
#include "Batch_BatchManager.hxx"

#ifdef WIN32
#define sleep(seconds) Sleep((seconds)*1000)
#endif

using namespace std;

namespace Batch {

  // Constructeur
//   BatchManager::BatchManager(string host) throw(InvalidArgumentException) : _hostname(host), jobid_map()
//   {
//     // On verifie que le hostname est correct
//     if (!gethostbyname(_hostname.c_str())) { // hostname unknown from network
//       string msg = "hostname \"";
//       msg += _hostname;
//       msg += "\" unknown from the network";
//       throw InvalidArgumentException(msg.c_str());
//     }
//   }
  BatchManager::BatchManager(const FactBatchManager * parent, const char * host) throw(InvalidArgumentException) : _hostname(host), jobid_map(), _parent(parent)
  {
#ifdef WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);  // Initialize Winsock
#endif

    // On verifie que le hostname est correct
    struct hostent* res = gethostbyname(_hostname.c_str());

#ifdef WIN32
    WSACleanup();  // Finalize Winsock
#endif

    if (!res) { // hostname unknown from network
      string msg = "hostname \"";
      msg += _hostname;
      msg += "\" unknown from the network";
      throw InvalidArgumentException(msg.c_str());
    }
  }

  // Destructeur
  BatchManager::~BatchManager()
  {
    // Nothing to do
  }

  string BatchManager::__repr__() const
  {
    ostringstream oss;
    oss << "<BatchManager of type '" << (_parent ? _parent->getType() : "unknown (no factory)") << "' connected to server '" << _hostname << "'>";
    return oss.str();
  }

  // Recupere le l'identifiant d'un job deja soumis au BatchManager
//   const JobId BatchManager::getJobIdByReference(const string & ref)
//   {
//     return JobId(this, ref);
//   }
  const JobId BatchManager::getJobIdByReference(const char * ref)
  {
    return JobId(this, ref);
  }

//   // Methode pour le controle des jobs : soumet un job au gestionnaire
//   const JobId BatchManager::submitJob(const Job & job)
//   {
//     static int idx = 0;
//     //MEDMEM::STRING sst;
//     ostringstream sst;
//     sst << "Jobid_" << idx++;
//     JobId id(this, sst.str());
//     return id;
//   }

//   // Methode pour le controle des jobs : retire un job du gestionnaire
//   void BatchManager::deleteJob(const JobId & jobid)
//   {
//     // Nothing to do
//   }

//   // Methode pour le controle des jobs : suspend un job en file d'attente
//   void BatchManager::holdJob(const JobId & jobid)
//   {
//     // Nothing to do
//   }

//   // Methode pour le controle des jobs : relache un job suspendu
//   void BatchManager::releaseJob(const JobId & jobid)
//   {
//     // Nothing to do
//   }

//   // Methode pour le controle des jobs : modifie un job en file d'attente
//   void BatchManager::alterJob(const JobId & jobid, const Parametre & param, const Environnement & env)
//   {
//     // Nothing to do
//   }

//   // Methode pour le controle des jobs : modifie un job en file d'attente
//   void BatchManager::alterJob(const JobId & jobid, const Parametre & param)
//   {
//     // Nothing to do
//   }

//   // Methode pour le controle des jobs : modifie un job en file d'attente
//   void BatchManager::alterJob(const JobId & jobid, const Environnement & env)
//   {
//     // Nothing to do
//   }

//   // Methode pour le controle des jobs : renvoie l'etat du job
//   JobInfo BatchManager::queryJob(const JobId & jobid)
//   {
//     return JobInfo();
//   }

  //! Wait for the end of a job
  /*!
   *  This method is a simple way to wait for a job to end. It will query the job state at
   *  increasing intervals and return when the job is finished (whether successfully or not) or
   *  when the timeout is reached. This method is not intended to be generic. In many cases you
   *  will have to write your own loop to wait for the end of a job.
   *  \param jobid ID of the job to wait for.
   *  \param timeout Maximum time to wait in seconds. If -1 (default), wait indefinitely.
   *  \param initSleepTime Initial time in seconds between two queries for the job state (default is 1).
   *  \param maxSleepTime Maximum time in seconds between two queries for the job state (default is 600).
   *  \return The job state as returned by the last query.
   */
  string BatchManager::waitForJobEnd(const JobId & jobid, long timeout,
                                     long initSleepTime, long maxSleepTime)
  {
    int time = 0;
    int sleeptime = initSleepTime;
    bool testTimeout = (timeout > -1);
    bool timeoutReached = (testTimeout && time >= timeout);
    JobInfo jinfo = jobid.queryJob();
    string state = jinfo.getParametre()[STATE].str();
    cout << "State is \"" << state << "\"";
    while (!timeoutReached && state != FINISHED && state != FAILED) {
      cout << ", sleeping " << sleeptime << "s..." << endl;
      sleep(sleeptime);
      time += sleeptime;
      timeoutReached = (testTimeout && time >= timeout);
      sleeptime *= 2;
      if (testTimeout && sleeptime > timeout - time)
        sleeptime = timeout - time;
      if (sleeptime > maxSleepTime)
        sleeptime = maxSleepTime;
      jinfo = jobid.queryJob();
      state = jinfo.getParametre()[STATE].str();
      cout << "State is \"" << state << "\"";
    }
    cout << endl;
    return state;
  }

}
