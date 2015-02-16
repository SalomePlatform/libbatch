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
 * Versatile.cxx : 
 *
 * Author : Ivan DUTKA-MALEN - EDF R&D
 * Date   : September 2003
 *
 */

#include <iostream>
#include <list>
#include <string>
#include <sstream>

#include "GenericType.hxx"
#include "BoolType.hxx"
#include "LongType.hxx"
#include "StringType.hxx"
#include "Versatile.hxx"
#include "TypeMismatchException.hxx"
#include "ListIsFullException.hxx"

using namespace std;

namespace Batch {

  Versatile::Versatile(DiscriminatorType discriminator, size_type maxsize, std::string name)
    : _discriminator(discriminator),
      _maxsize(maxsize),
      _name(name)
  {
  }

  Versatile::Versatile(const Versatile & V)
   : _discriminator(V._discriminator),
     _maxsize(V._maxsize),
     _name(V._name)
  {
    Versatile::const_iterator it;
    for(it=V.begin(); it!=V.end(); it++)
      push_back( (*it)->clone() );
  }

  Versatile::~Versatile()
  {
    eraseAll();
  }

  Versatile & Versatile::operator = (const long l) throw(TypeMismatchException)
  {
    checkType(LONG);
    eraseAll();
    push_back(new LongType(l));
    return *this;
  }

  Versatile & Versatile::operator = (const string & ch) throw(TypeMismatchException)
  {
    checkType(STRING);
    eraseAll();
    push_back(new StringType(ch));
    return *this;
  }

  Versatile & Versatile::operator +=(const string & ch) throw(TypeMismatchException,ListIsFullException)
  {
    checkType(STRING);

	// If max size is reached, throw a ListIsFullException
    if (_maxsize == 0 || size() < _maxsize)
      push_back(new StringType(ch));
    else {
      ostringstream msg;
      msg << "Maximum size for \"" << _name << "\" is reached: " << _maxsize;
      throw(ListIsFullException(msg.str()));
    }
    return *this;
  }

  Versatile & Versatile::operator , (const string & ch) throw(TypeMismatchException,ListIsFullException)
  {
    *this += ch;
    return *this;
  }

  Versatile & Versatile::operator = (const char * ch) throw(TypeMismatchException)
  {
    return operator=(string(ch));
  }

  Versatile & Versatile::operator +=(const char * ch) throw(TypeMismatchException,ListIsFullException)
  {
    return operator+=(string(ch));
  }

  Versatile & Versatile::operator , (const char * ch) throw(TypeMismatchException,ListIsFullException)
  {
    return operator,(string(ch));
  }

  Versatile & Versatile::operator = (const Couple & cp) throw(TypeMismatchException)
  {
    checkType(COUPLE);
    eraseAll();
    push_back(new CoupleType(cp));
    return *this;
  }

  Versatile & Versatile::operator +=(const Couple & cp) throw(TypeMismatchException,ListIsFullException)
  {
    checkType(COUPLE);
    // If max size is reached, throw a ListIsFullException
    if (_maxsize == 0 || size() < _maxsize)
      push_back(new CoupleType(cp));
    else {
      ostringstream msg;
      msg << "Maximum size for \"" << _name << "\" is reached: " << _maxsize;
      throw(ListIsFullException(msg.str()));
    }
    return *this;
  }

  Versatile & Versatile::operator , (const Couple & cp) throw(TypeMismatchException,ListIsFullException)
  {
    *this += cp;
    return *this;
  }

  ostream & operator << (ostream & os, const Versatile & V)
  {
    Versatile::const_iterator it;
    const char * sep = "";

    for(it=V.begin(); it!=V.end(); it++, sep=" ") {
      string s = (*it)->affiche();
      os << sep << s;
    }
    return os;
  }

  Versatile & Versatile::operator = (const int i) throw(TypeMismatchException)
  {
    checkType(LONG);
    eraseAll();
    push_back(new LongType((long)i));
    return *this;
  }

  Versatile & Versatile::operator = (const bool b) throw(TypeMismatchException)
  {
    checkType(BOOL);
    eraseAll();
    push_back(new BoolType(b));
    return *this;
  }

  void Versatile::checkType(DiscriminatorType t) const throw(TypeMismatchException)
  {
    if (_discriminator != t)
      throw (TypeMismatchException("Trying to change type of Versatile object \"" + _name + "\""));
  }

  Versatile::operator long() const throw(TypeMismatchException)
  {
    // If the type does not correspond or if the list has more than one element,
    // throw a TypeMismatchException
    if ( _maxsize != 1 || _discriminator != LONG || size() == 0 ) {
      ostringstream sst;
      sst << "Cannot cast Versatile object \"" << _name << "\" to long";
      throw (TypeMismatchException(sst.str()));
    }
	return *( static_cast<LongType *>(this->front()) );
  }

  Versatile::operator bool() const throw(TypeMismatchException)
  {
    // If the type does not correspond or if the list has more than one element,
    // throw a TypeMismatchException
    if ( _maxsize != 1 || _discriminator != BOOL || size() == 0 ) {
      ostringstream sst;
      sst << "Cannot cast Versatile object \"" << _name << "\" to bool";
      throw(TypeMismatchException(sst.str()));
    }
    return *( static_cast<BoolType *>(this->front()) );
  }

  Versatile::operator int() const throw(TypeMismatchException)
  {
    return operator long();
  }

  Versatile::operator Couple() const throw(TypeMismatchException)
  {
    // If the type does not correspond or if the list has more than one element,
    // throw a TypeMismatchException
    if ( _maxsize != 1 || _discriminator != COUPLE || size() == 0 ) {
      ostringstream sst;
      sst << "Cannot cast Versatile object \"" << _name << "\" to Couple";
      throw(TypeMismatchException(sst.str()));
    }
    return *( static_cast<CoupleType *>(this->front()) );
  }

  string Versatile::str() const throw(TypeMismatchException)
  {
    // If the type does not correspond, throw a TypeMismatchException
    if ( _discriminator != STRING || size() == 0 ) {
      ostringstream sst;
      sst << "Cannot cast Versatile object \"" << _name << "\" to string";
      throw(TypeMismatchException(sst.str()));
    }

	// The returned string is the concatenation of internal strings
    string s;
    Versatile::const_iterator it;
    const char * sep = "";
    for(it=begin(); it!=end(); it++, s+=sep, sep=" ")
      s += *( static_cast<StringType *>(*it));

    return s;
  }

  Versatile::operator string () const throw(TypeMismatchException)
  {
    return str();
  }

  void Versatile::eraseAll()
  {
    while(!empty()) {
      delete back();
      pop_back();
    }
  }

  DiscriminatorType Versatile::getType() const
  {
    return _discriminator;
  }

  Versatile::size_type Versatile::getMaxSize() const
  {
    return _maxsize;
  }

  const string & Versatile::getName() const
  {
    return _name;
  }

}
