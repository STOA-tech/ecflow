//============================================================================
// Name        :
// Author      : Avi
// Revision    : $Revision: #23 $
//
// Copyright 2009- ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//
// Description :
//============================================================================

#include <unordered_map>
#include <iostream>

#include "ExprDuplicate.hpp"
#include "ExprAst.hpp"
#include "Ecf.hpp"

////////////////////////////////////////////////////////////////////////////
using namespace std;

static std::unordered_map< std::string, AstTop* > duplicate_expr;
typedef std::unordered_map< std::string, AstTop* > my_map;


ExprDuplicate::~ExprDuplicate()
{
   //cout << "ExprDuplicate::~ExprDuplicate: server(" << Ecf::server() << ") " << duplicate_expr.size() << " *****************************************************************\n";
   for(my_map::value_type i: duplicate_expr) {
      //cout << " deleting: " << i.first << " :" << i.second << "\n";
      delete i.second;
      i.second = NULL;
   }
   duplicate_expr.clear();
}

void ExprDuplicate::dump(const std::string& msg )
{
   cout << "ExprDuplicate::dump server(" << Ecf::server() << ") " << msg << "\n";
   for(const my_map::value_type& i: duplicate_expr) {
      cout << "   " << i.first << " :" << i.second << "\n";
   }
}

std::unique_ptr<AstTop> ExprDuplicate::find(const std::string& expr)
{
   my_map::const_iterator it = duplicate_expr.find(expr);
   if (it != duplicate_expr.end()) {
      return std::unique_ptr<AstTop>((*it).second->clone());
   }
   return std::unique_ptr<AstTop>();
}

void ExprDuplicate::add(const std::string& expr,AstTop* ast)
{
   assert(!expr.empty() && ast);
   AstTop* clone = ast->clone();
   duplicate_expr.insert( std::make_pair(expr,clone));

   //cout << "ExprDuplicate::add: server(" << Ecf::server() << ") " << expr << " :" << clone << "   " << duplicate_expr.size() << "\n";
}
