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
 *  SimpleParser.hxx
 *
 *  Created on: 23 juil. 2009
 *  Author: Renaud BARATE - EDF R&D
 */

#ifndef SIMPLEPARSER_H_
#define SIMPLEPARSER_H_

#include <map>
#include <string>
#include <ostream>

class ParserException : public std::exception
{
public:
  ParserException(std::string msg) throw();
  virtual ~ParserException() throw();

  virtual const char *what() const throw();

private:
  std::string _msg;
};

class SimpleParser
{
public:
  SimpleParser() throw();
  virtual ~SimpleParser() throw();

  void parse(const std::string & filename) throw(ParserException);
  void parseTestConfigFile() throw(ParserException);
  const std::string & getValue(const std::string & key) const throw(ParserException);
  const std::string & getTestValue(const std::string & bmType, const std::string & protocolStr,
                                   const std::string & key) const throw(ParserException);
  int getValueAsInt(const std::string & key) const throw(ParserException);
  int getTestValueAsInt(const std::string & bmType, const std::string & protocolStr,
                        const std::string & key) const throw(ParserException);

  friend std::ostream & operator <<(std::ostream & os, const SimpleParser & parser) throw();

private:
  std::string trim(const std::string & str) const throw();

  std::map<std::string, std::string> _configmap;
};

#endif /* SIMPLEPARSER_H_ */
