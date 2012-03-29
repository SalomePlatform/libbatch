#  Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
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

# We use a custom FindSWIG.cmake because the one shipped with cmake doesn't search
# properly in the $PATH env var.

IF (NOT SWIG_FIND_QUIETLY)
    MESSAGE(STATUS "Looking for SWIG...")
ENDIF (NOT SWIG_FIND_QUIETLY)

FIND_PROGRAM(SWIG_EXECUTABLE NAMES swig swig-1.3 DOC "SWIG executable")

SET(SWIG_FOUND ${SWIG_EXECUTABLE})

IF (SWIG_FOUND)

    IF (NOT SWIG_FIND_QUIETLY)
        MESSAGE(STATUS "Found SWIG: ${SWIG_EXECUTABLE}")
    ENDIF (NOT SWIG_FIND_QUIETLY)

ELSE (SWIG_FOUND)

    IF (SWIG_FIND_REQUIRED)
        MESSAGE(FATAL_ERROR "SWIG not found")
    ELSE (SWIG_FIND_REQUIRED)
        IF (NOT SWIG_FIND_QUIETLY)
            MESSAGE(STATUS "SWIG not found")
        ENDIF (NOT SWIG_FIND_QUIETLY)
    ENDIF (SWIG_FIND_REQUIRED)

ENDIF (SWIG_FOUND)
