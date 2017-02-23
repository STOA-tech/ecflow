#ifndef QUEUE_PARSER_HPP_
#define QUEUE_PARSER_HPP_
//============================================================================
// Name        :
// Author      : Avi
// Revision    : $Revision: #10 $
//
// Copyright 2009-2017 ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//============================================================================

#include "Parser.hpp"

class QueueParser : public Parser {
public:
   QueueParser(DefsStructureParser* p) : Parser(p){}
   virtual const char* keyword() const { return "queue"; }
   virtual bool doParse( const std::string& line, std::vector<std::string >& lineTokens );
};

#endif
