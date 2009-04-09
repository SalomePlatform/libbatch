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

MESSAGE("")
MESSAGE("**************** Summary ****************")
MESSAGE("")
IF (BUILD_LSF_INTERFACE)
    IF (LSF_FOUND)
        MESSAGE("LSF ................................. Yes")
    ELSE (LSF_FOUND)
        MESSAGE("LSF ........................... Not found")
    ENDIF (LSF_FOUND)
ELSE (BUILD_LSF_INTERFACE)
        MESSAGE("LSF ....................... Not requested")
ENDIF (BUILD_LSF_INTERFACE)

IF (BUILD_PBS_INTERFACE)
    IF (PBS_FOUND)
        MESSAGE("PBS ................................. Yes")
    ELSE (PBS_FOUND)
        MESSAGE("PBS ........................... Not found")
    ENDIF (PBS_FOUND)
ELSE (BUILD_PBS_INTERFACE)
        MESSAGE("PBS ....................... Not requested")
ENDIF (BUILD_PBS_INTERFACE)

IF (BUILD_LOCAL_SUBMISSION)
        MESSAGE("Local submission .................... Yes")
ELSE (BUILD_LOCAL_SUBMISSION)
        MESSAGE("Local submission .......... Not requested")
ENDIF (BUILD_LOCAL_SUBMISSION)

IF (BUILD_PYTHON_WRAPPING)
    IF (PYTHON_INCLUDE_PATH AND PYTHON_LIBRARIES AND SWIG_FOUND)
        MESSAGE("Python wrapping ..................... Yes")
    ELSE (PYTHON_INCLUDE_PATH AND PYTHON_LIBRARIES AND SWIG_FOUND)
        MESSAGE("Python wrapping ............... Not found")
    ENDIF (PYTHON_INCLUDE_PATH AND PYTHON_LIBRARIES AND SWIG_FOUND)
ELSE (BUILD_PYTHON_WRAPPING)
        MESSAGE("Python wrapping ........... Not requested")
ENDIF (BUILD_PYTHON_WRAPPING)

IF (Makeinfo_FOUND)
    MESSAGE("Makeinfo ............................ Yes")
ELSE (Makeinfo_FOUND)
    MESSAGE("Makeinfo ...................... Not found")
ENDIF (Makeinfo_FOUND)

MESSAGE("")
MESSAGE("************** End Summary **************")
MESSAGE("")
