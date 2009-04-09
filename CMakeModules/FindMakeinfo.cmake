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

IF (NOT Makeinfo_FIND_QUIETLY)
    MESSAGE(STATUS "Looking for Makeinfo...")
ENDIF (NOT Makeinfo_FIND_QUIETLY)

FIND_PROGRAM(MAKEINFO_EXECUTABLE makeinfo DOC "Makeinfo executable")

SET(Makeinfo_FOUND ${MAKEINFO_EXECUTABLE})

IF (Makeinfo_FOUND)

    IF (NOT Makeinfo_FIND_QUIETLY)
        MESSAGE(STATUS "Found Makeinfo: ${MAKEINFO_EXECUTABLE}")
    ENDIF (NOT Makeinfo_FIND_QUIETLY)

ELSE (Makeinfo_FOUND)

    IF (Makeinfo_FIND_REQUIRED)
        MESSAGE(FATAL_ERROR "Makeinfo not found")
    ELSE (Makeinfo_FIND_REQUIRED)
        IF (NOT Makeinfo_FIND_QUIETLY)
            MESSAGE(STATUS "Makeinfo not found")
        ENDIF (NOT Makeinfo_FIND_QUIETLY)
    ENDIF (Makeinfo_FIND_REQUIRED)

ENDIF (Makeinfo_FOUND)
