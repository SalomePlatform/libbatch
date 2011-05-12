#  Copyright (C) 2007-2011  CEA/DEN, EDF R&D, OPEN CASCADE
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

IF (NOT PThread_FIND_QUIETLY)
    MESSAGE(STATUS "Looking for PThread...")
ENDIF (NOT PThread_FIND_QUIETLY)

FIND_PATH(PTHREAD_INCLUDE_DIR pthread.h)
FIND_LIBRARY(PTHREAD_LIBRARY NAMES pthread pthreadVC2) 

IF (PTHREAD_INCLUDE_DIR AND PTHREAD_LIBRARY)
    SET(PThread_FOUND True)
ENDIF (PTHREAD_INCLUDE_DIR AND PTHREAD_LIBRARY)

IF (PThread_FOUND)

    IF (NOT PThread_FIND_QUIETLY)
        MESSAGE(STATUS "Found PThread:")
        MESSAGE(STATUS "PThread include directory: ${PTHREAD_INCLUDE_DIR}")
        MESSAGE(STATUS "PThread library: ${PTHREAD_LIBRARY}")
    ENDIF (NOT PThread_FIND_QUIETLY)

ELSE (PThread_FOUND)

    IF (PThread_FIND_REQUIRED)
        MESSAGE(FATAL_ERROR "PThread not found")
    ELSE (PThread_FIND_REQUIRED)
        IF (NOT PThread_FIND_QUIETLY)
            MESSAGE(STATUS "PThread not found")
        ENDIF (NOT PThread_FIND_QUIETLY)
    ENDIF (PThread_FIND_REQUIRED)

ENDIF (PThread_FOUND)
