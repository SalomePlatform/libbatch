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

IF (NOT LSF_FIND_QUIETLY)
    MESSAGE(STATUS "Looking for LSF...")
ENDIF (NOT LSF_FIND_QUIETLY)

FIND_PATH(LSF_INCLUDE_DIR lsf/lsbatch.h)
FIND_LIBRARY(LSF_LIBRARY lsf) 
FIND_LIBRARY(LSF_BAT_LIBRARY bat) 

SET(LSF_FOUND ${LSF_INCLUDE_DIR} AND ${LSF_LIBRARY} AND ${LSF_BAT_LIBRARY})

IF (LSF_FOUND)

    IF (NOT LSF_FIND_QUIETLY)
        MESSAGE(STATUS "Found LSF: ${LSF_LIBRARY}")
    ENDIF (NOT LSF_FIND_QUIETLY)

ELSE (LSF_FOUND)

    IF (LSF_FIND_REQUIRED)
        MESSAGE(FATAL_ERROR "LSF not found")
    ELSE (LSF_FIND_REQUIRED)
        IF (NOT LSF_FIND_QUIETLY)
            MESSAGE(STATUS "LSF not found")
        ENDIF (NOT LSF_FIND_QUIETLY)
    ENDIF (LSF_FIND_REQUIRED)

ENDIF (LSF_FOUND)
