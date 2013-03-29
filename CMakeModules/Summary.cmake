#  Copyright (C) 2007-2013  CEA/DEN, EDF R&D, OPEN CASCADE
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

MESSAGE(STATUS "")
MESSAGE(STATUS "**************** Summary ****************")
MESSAGE(STATUS "")
IF (BUILD_LOCAL_SUBMISSION)
        MESSAGE(STATUS "Local submission .................... Yes")
ELSE (BUILD_LOCAL_SUBMISSION)
        MESSAGE(STATUS "Local submission .......... Not requested")
ENDIF (BUILD_LOCAL_SUBMISSION)

IF (BUILD_PYTHON_WRAPPING)
    IF (PYTHONINTERP_FOUND AND PYTHONLIBS_FOUND AND SWIG_FOUND)
        MESSAGE(STATUS "Python wrapping ..................... Yes")
    ELSE (PYTHONINTERP_FOUND AND PYTHONLIBS_FOUND AND SWIG_FOUND)
        MESSAGE(STATUS "Python wrapping ............... Not found")
    ENDIF (PYTHONINTERP_FOUND AND PYTHONLIBS_FOUND AND SWIG_FOUND)
ELSE (BUILD_PYTHON_WRAPPING)
        MESSAGE(STATUS "Python wrapping ........... Not requested")
ENDIF (BUILD_PYTHON_WRAPPING)

MESSAGE(STATUS "")
MESSAGE(STATUS "************** End Summary **************")
MESSAGE(STATUS "")
