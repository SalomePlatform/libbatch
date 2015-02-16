// Copyright (C) 2007-2015  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
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

#include "Defines.hxx"

#include <iostream>
#include <list>
#include <string>
#include "GenericType.hxx"
#include "BoolType.hxx"
#include "LongType.hxx"
#include "StringType.hxx"
#include "CoupleType.hxx"
#include "TypeMismatchException.hxx"
#include "ListIsFullException.hxx"
#include "RunTimeException.hxx"

namespace Batch {

  // Authorized types
  enum DiscriminatorType { BOOL, LONG, STRING, COUPLE };

  class BATCH_EXPORT Versatile : public std::list< GenericType * >
  {
  public:

    // Constructors
    Versatile(DiscriminatorType discriminator, size_type maxsize, std::string name);
    Versatile(const Versatile & V);

    // Use of default constructor is forbidden but we cannot declare it private because
    // compilation fails with VC8 on Windows
    Versatile() { throw RunTimeException("Forbidden constructor"); }

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

    // Forbid the use of affectation operator
    void operator= (const Versatile & V) {}

  };

}

#endif
