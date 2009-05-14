#  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
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

MACRO(FIND_LOCAL_COMMAND VAR COMMAND)
    FIND_PROGRAM(${VAR} NAMES ${COMMAND} ${ARGN})
    IF (${VAR})
        MESSAGE(STATUS "${COMMAND} found : ${${VAR}}")
    ELSE (${VAR})
        MESSAGE(STATUS "${COMMAND} not found, local submission might not work properly")
        SET(${VAR} /bin/false)
    ENDIF (${VAR})
    MARK_AS_ADVANCED(${VAR})
ENDMACRO(FIND_LOCAL_COMMAND)

MESSAGE(STATUS "Looking for commands needed for local submission...")

FIND_PROGRAM(RM rm)
IF (RM)
    MESSAGE(STATUS "rm found : ${RM}")
ELSE (RM)
    IF (WIN32)
        MESSAGE(STATUS "using 'del' as rm command")
        SET(RM del)
    ELSE (WIN32)
        MESSAGE(STATUS "rm not found, local submission might not work properly")
        SET(RM /bin/false)
    ENDIF (WIN32)
ENDIF (RM)
MARK_AS_ADVANCED(RM)

FIND_PROGRAM(SH sh)
IF (SH)
    MESSAGE(STATUS "sh found : ${SH}")
ELSE (SH)
    FIND_PROGRAM(CMD cmd)
    IF (CMD)
        MESSAGE(STATUS "cmd found : ${CMD}")
        SET(SH ${CMD})
        SET(SH_COMMAND_IS_CMD TRUE)
    ELSE (CMD)
        MESSAGE(STATUS "sh not found, local submission might not work properly")
        SET(SH /bin/false)
    ENDIF (CMD)
ENDIF (SH)
MARK_AS_ADVANCED(CMD SH SH_COMMAND_IS_CMD)

FIND_PROGRAM(CP cp)
IF (CP)
    MESSAGE(STATUS "cp found : ${CP}")
ELSE (CP)
    IF (WIN32)
        MESSAGE(STATUS "using 'copy' as cp command")
        SET(CP copy)
    ELSE (WIN32)
        MESSAGE(STATUS "cp not found, local submission might not work properly")
        SET(CP /bin/false)
    ENDIF (WIN32)
ENDIF (CP)
MARK_AS_ADVANCED(CP)

FIND_LOCAL_COMMAND(RSH rsh)
FIND_LOCAL_COMMAND(RCP rcp)
FIND_LOCAL_COMMAND(SSH ssh plink)
FIND_LOCAL_COMMAND(SCP scp pscp)
