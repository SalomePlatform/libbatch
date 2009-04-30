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
 * BatchManager_Local_SSH.cxx :
 *
 * Auteur : Ivan DUTKA-MALEN - EDF R&D
 * Mail   : mailto:ivan.dutka-malen@der.edf.fr
 * Date   : Thu Nov  6 10:17:22 2003
 * Projet : Salome 2
 *
 */

#ifdef HAVE_CONFIG_H
#  include <SALOMEconfig.h>
#endif

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <sys/types.h>
#ifndef WIN32
#include <sys/wait.h>
#include <unistd.h>
#endif
#include <ctime>

#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include "Batch_IOMutex.hxx"
#include "Batch_BatchManager_Local_SSH.hxx"

#include "Batch_config.h"

#ifndef RM
#error "RM undefined. You must set RM to a valid path to a rm-like command."
#endif

#ifndef SCP
#error "SCP undefined. You must set SCP to a valid path to a scp-like command."
#endif

#ifndef SSH
#error "SSH undefined. You must set SSH to a valid path to a ssh-like command."
#endif

using namespace std;

namespace Batch {


  // Constructeur
  BatchManager_Local_SSH::BatchManager_Local_SSH(const FactBatchManager * parent, const char * host) throw(InvalidArgumentException,ConnexionFailureException) : BatchManager_Local(parent, host)
  {
  }

  // Destructeur
  BatchManager_Local_SSH::~BatchManager_Local_SSH()
  {
  }


  // Methode abstraite qui renvoie la commande de copie du fichier source en destination
  string BatchManager_Local_SSH::copy_command(const std::string & user_source,
                                              const std::string & host_source,
                                              const std::string & source,
                                              const std::string & user_destination,
                                              const std::string & host_destination,
                                              const std::string & destination) const
  {
    ostringstream fullsource;
    if (host_source.size() != 0) {
      fullsource << host_source << ":";
    }
    fullsource << source;

    ostringstream fulldestination;
    if (host_destination.size() != 0) {
      fulldestination << host_destination << ":";
    }
    fulldestination << destination;

    ostringstream copy_cmd;
    // Option -p is used to keep the same permissions for the destination file (particularly useful to keep scripts
    // executable when copying them)
    copy_cmd << SCP << " -p " << fullsource.str() << " " << fulldestination.str();
    return copy_cmd.str();
  }

  // Methode abstraite qui renvoie la commande a executer
  string BatchManager_Local_SSH::exec_command(Parametre & param) const
  {
    ostringstream exec_sub_cmd;
    exec_sub_cmd << "cd " << param[WORKDIR] << ";";
    exec_sub_cmd << param[EXECUTABLE];

    if (param.find(ARGUMENTS) != param.end()) {
      Versatile V = param[ARGUMENTS];
      for(Versatile::const_iterator it=V.begin(); it!=V.end(); it++) {
        StringType argt = * static_cast<StringType *>(*it);
        string     arg  = argt;
        exec_sub_cmd << " " << arg;
      }
    }


    Versatile new_arguments;
    new_arguments.setMaxSize(0);
    new_arguments = string(param[EXECUTIONHOST]);


    if (param.find(USER) != param.end()) {
      new_arguments += "-l";
      new_arguments += string(param[USER]);
    }

    new_arguments += exec_sub_cmd.str();

    param[ARGUMENTS] = new_arguments;

    // Sous Linux on est oblige de modifier ces deux parametres pour faire fonctionner la commande rsh
    param[EXECUTABLE] = SSH;
    param.erase(NAME);

    return SSH;
  }

  // Methode qui renvoie la commande d'effacement du fichier
  string BatchManager_Local_SSH::remove_command(const std::string & user_destination,
                                                const std::string & host_destination,
                                                const std::string & destination) const
  {
    string host = (host_destination.size()) ? host_destination : "localhost:";

    ostringstream remove_cmd;
    remove_cmd << SSH << " " << host << " \"" << RM << " " << destination << "\"";
    return remove_cmd.str();
  }
}
