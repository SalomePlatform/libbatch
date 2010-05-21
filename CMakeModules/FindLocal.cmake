#  Copyright (C) 2007-2010  CEA/DEN, EDF R&D, OPEN CASCADE
#
#  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
#  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
#  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

MACRO(FIND_LOCAL_COMMAND VAR UNIX_COMMAND WIN32_COMMAND)
    IF (WIN32)
        FIND_PROGRAM(${VAR} ${WIN32_COMMAND})
    ELSE (WIN32)
        FIND_PROGRAM(${VAR} ${UNIX_COMMAND})
    ENDIF (WIN32)
    IF (${VAR})
        MESSAGE(STATUS "${VAR} found : ${${VAR}}")
    ELSE (${VAR})
        MESSAGE(STATUS "${VAR} not found, local submission might not work properly")
    ENDIF (${VAR})
ENDMACRO(FIND_LOCAL_COMMAND)

MACRO(FIND_LOCAL_COMMAND_WIN32_DEF VAR UNIX_COMMAND WIN32_DEFAULT)
    IF (WIN32)
        SET(${VAR} ${WIN32_DEFAULT} CACHE STRING "${VAR}")
        MESSAGE(STATUS "${VAR} found : using '${${VAR}}' (Windows default)")
    ELSE (WIN32)
        FIND_LOCAL_COMMAND(${VAR} ${UNIX_COMMAND} ${WIN32_DEFAULT})
    ENDIF (WIN32)
ENDMACRO(FIND_LOCAL_COMMAND_WIN32_DEF)

MACRO(EVAL VAR)
   IF(${ARGN})
     SET(${VAR} TRUE)
   ELSE(${ARGN})
     SET(${VAR} FALSE)
   ENDIF(${ARGN})
ENDMACRO(EVAL)

MESSAGE(STATUS "Looking for commands needed for local submission...")

FIND_LOCAL_COMMAND_WIN32_DEF(RM_COMMAND rm del)
FIND_LOCAL_COMMAND(SH_COMMAND sh cmd.exe)
FIND_LOCAL_COMMAND_WIN32_DEF(CP_COMMAND cp copy)
FIND_LOCAL_COMMAND_WIN32_DEF(MKDIR_COMMAND mkdir mkdir)
FIND_LOCAL_COMMAND(RSH_COMMAND rsh rsh)
FIND_LOCAL_COMMAND(RCP_COMMAND rcp rcp)
FIND_LOCAL_COMMAND(SSH_COMMAND ssh plink)
FIND_LOCAL_COMMAND(SCP_COMMAND scp pscp)

EVAL (HAS_SH SH_COMMAND AND CP_COMMAND AND RM_COMMAND AND MKDIR_COMMAND)
EVAL (HAS_RSH RSH_COMMAND AND RCP_COMMAND)
EVAL (HAS_SSH SSH_COMMAND AND SCP_COMMAND)
