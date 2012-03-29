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

IF (NOT PBS_FIND_QUIETLY)
    MESSAGE(STATUS "Looking for PBS...")
ENDIF (NOT PBS_FIND_QUIETLY)

FIND_PATH(PBS_INCLUDE_DIR pbs_ifl.h)
FIND_LIBRARY(PBS_LIBRARY NAMES pbs torque) 

IF (PBS_INCLUDE_DIR AND PBS_LIBRARY)
    SET(PBS_FOUND True)
ENDIF (PBS_INCLUDE_DIR AND PBS_LIBRARY)

IF (PBS_FOUND)

    IF (NOT PBS_FIND_QUIETLY)
        MESSAGE(STATUS "Found PBS: ${PBS_LIBRARY}")
    ENDIF (NOT PBS_FIND_QUIETLY)

ELSE (PBS_FOUND)

    IF (PBS_FIND_REQUIRED)
        MESSAGE(FATAL_ERROR "PBS not found")
    ELSE (PBS_FIND_REQUIRED)
        IF (NOT PBS_FIND_QUIETLY)
            MESSAGE(STATUS "PBS not found")
        ENDIF (NOT PBS_FIND_QUIETLY)
    ENDIF (PBS_FIND_REQUIRED)

ENDIF (PBS_FOUND)
