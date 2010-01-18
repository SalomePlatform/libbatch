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
 * Batch_Constants.hxx :
 *
 * Auteur : Renaud BARATE - EDF R&D
 * Date   : January 2010
 *
 */

#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

#include <string>

#include "Batch_Defines.hxx"

// These macros simplify the creation of new string constants for keys and
// values in parameters map.
// TODO: replace those static declarations by an external description file (XML for instance)
#define decl_extern_Constant(constant) extern BATCH_EXPORT const std::string constant
#define def_Constant(constant) const std::string constant = #constant

namespace Batch {

  // Declaration of the keys for the parameter map
  decl_extern_Constant(ACCOUNT);
  decl_extern_Constant(ARGUMENTS);
  decl_extern_Constant(CHECKPOINT);
  decl_extern_Constant(CKPTINTERVAL);
  decl_extern_Constant(CREATIONTIME);
  decl_extern_Constant(EGROUP);
  decl_extern_Constant(ELIGIBLETIME);
  decl_extern_Constant(ENDTIME);
  decl_extern_Constant(EUSER);
  decl_extern_Constant(EXECUTABLE);
  decl_extern_Constant(EXECUTIONHOST);
  decl_extern_Constant(EXITCODE);
  decl_extern_Constant(HOLD);
  decl_extern_Constant(ID);
  decl_extern_Constant(INFILE);
  decl_extern_Constant(MAIL);
  decl_extern_Constant(MAXCPUTIME);
  decl_extern_Constant(MAXDISKSIZE);
  decl_extern_Constant(MAXRAMSIZE);
  decl_extern_Constant(MAXWALLTIME);
  decl_extern_Constant(MODIFICATIONTIME);
  decl_extern_Constant(NAME);
  decl_extern_Constant(NBPROC);
  decl_extern_Constant(OUTFILE);
  decl_extern_Constant(PID);
  decl_extern_Constant(QUEUE);
  decl_extern_Constant(QUEUEDTIME);
  decl_extern_Constant(SERVER);
  decl_extern_Constant(STARTTIME);
  decl_extern_Constant(STATE);
  decl_extern_Constant(TEXT);
  decl_extern_Constant(TMPDIR);
  decl_extern_Constant(USEDCPUTIME);
  decl_extern_Constant(USEDDISKSIZE);
  decl_extern_Constant(USEDRAMSIZE);
  decl_extern_Constant(USEDWALLTIME);
  decl_extern_Constant(USER);
  decl_extern_Constant(WORKDIR);
  decl_extern_Constant(HOMEDIR);

  // These constants define the status of a job (parameter STATE)
  decl_extern_Constant(CREATED);
  decl_extern_Constant(IN_PROCESS);
  decl_extern_Constant(QUEUED);
  decl_extern_Constant(RUNNING);
  decl_extern_Constant(PAUSED);
  decl_extern_Constant(FINISHED);
  decl_extern_Constant(FAILED);

}

#endif
