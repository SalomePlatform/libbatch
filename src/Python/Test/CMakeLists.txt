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

IF (BUILD_LOCAL_SUBMISSION AND HAS_SH)
    # Configure the config file for the test script
    CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.py.in ${CMAKE_CURRENT_BINARY_DIR}/config.py)
    
    # Just copy the test scripts to the binary dir
    CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/seta.sh ${CMAKE_CURRENT_BINARY_DIR}/seta.sh COPYONLY)
    CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/setb.sh ${CMAKE_CURRENT_BINARY_DIR}/setb.sh COPYONLY)
    CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/Test_Python_Local_SH.py
                   ${CMAKE_CURRENT_BINARY_DIR}/Test_Python_Local_SH.py COPYONLY)
    
    # Add the test
    ADD_TEST(Python_Local_SH ${PYTHON_EXECUTABLE} Test_Python_Local_SH.py)
ENDIF (BUILD_LOCAL_SUBMISSION AND HAS_SH)
