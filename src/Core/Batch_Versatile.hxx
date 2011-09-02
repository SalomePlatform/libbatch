//  Copyright (C) 2007-2011  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
/*
 * Versatile.hxx :
 *
 * Author : Ivan DUTKA-MALEN - EDF R&D
 * Date   : September 2003
 *
 */

#ifndef _VERSATILE_H_
#define _VERSATILE_H_

#include "Batch_Defines.hxx"

#include <iostream>
#include <list>
#include <string>
#include "Batch_GenericType.hxx"
#include "Batch_BoolType.hxx"
#include "Batch_CharType.hxx"
#include "Batch_LongType.hxx"
#include "Batch_StringType.hxx"
#include "Batch_CoupleType.hxx"
#include "Batch_TypeMismatchException.hxx"
#include "Batch_ListIsFullException.hxx"

namespace Batch {

  // Authorized types
  enum DiscriminatorType { BOOL, LONG, STRING, COUPLE };

  class BATCH_EXPORT Versatile : public std::list< GenericType * >
  {
  public:

    // Constructors
    Versatile(DiscriminatorType discriminator, size_type maxsize, std::string name);
    Versatile(const Versatile & V);

    // Destructor
    virtual ~Versatile();

    // Affectation and concatenation operators from base types
    Versatile & operator = (const long     l)    throw(TypeMismatchException);
    Versatile & operator = (const std::string & ch)   throw(TypeMismatchException);
    Versatile & operator +=(const std::string & ch)   throw(TypeMismatchException,ListIsFullException);
    Versatile & operator , (const std::string & ch)   throw(TypeMismatchException,ListIsFullException);
    Versatile & operator = (const char * ch)   throw(TypeMismatchException);
    Versatile & operator +=(const char * ch)   throw(TypeMismatchException,ListIsFullException);
    Versatile & operator , (const char * ch)   throw(TypeMismatchException,ListIsFullException);
    Versatile & operator = (const Couple & cp)   throw(TypeMismatchException);
    Versatile & operator +=(const Couple & cp)   throw(TypeMismatchException,ListIsFullException);
    Versatile & operator , (const Couple & cp)   throw(TypeMismatchException,ListIsFullException);
    Versatile & operator = (const int i) throw(TypeMismatchException);
    Versatile & operator = (const bool b) throw(TypeMismatchException);

    // Type conversion to base types
    operator long() const throw(TypeMismatchException);
    operator std::string() const throw(TypeMismatchException);
    operator Couple() const throw(TypeMismatchException);
    std::string str() const throw(TypeMismatchException);
    operator bool() const throw(TypeMismatchException);
    operator int() const throw(TypeMismatchException);

    // Display on a stream
    BATCH_EXPORT friend std::ostream & operator << (std::ostream & os, const Versatile & );

    // Check the type
    void checkType(DiscriminatorType t) const throw (TypeMismatchException);

    // Getter methods
    DiscriminatorType getType() const;
    size_type getMaxSize() const;
    const std::string & getName() const;

    // Erase all internal elements
    void eraseAll();

  protected:

    DiscriminatorType _discriminator; // Internal element type
    size_type _maxsize; // Maximum number of internal elements
    std::string _name; // Object name (used for exceptions)

  private:

    // Forbid the use of default constructor and affectation operator
    Versatile() {}
    void operator= (const Versatile & V) {}

  };

}

#endif
