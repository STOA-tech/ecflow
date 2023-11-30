/*
 * Copyright 2009- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include "InLimit.hpp"

#include <stdexcept>

#include "Converter.hpp"
#include "Indentor.hpp"
#include "Limit.hpp"
#include "PrintStyle.hpp"
#include "Serialization.hpp"
#include "Str.hpp"
#ifdef DEBUG
    #include "Ecf.hpp"
#endif

using namespace std;
using namespace ecf;

/////////////////////////////////////////////////////////////////////////////////////////////

InLimit::InLimit(const std::string& name,
                 const std::string& pathToNode,
                 int tokens,
                 bool limit_this_node_only,
                 bool limit_submission,
                 bool check)
    : n_(name),
      path_(pathToNode),
      tokens_(tokens),
      limit_this_node_only_(limit_this_node_only),
      limit_submission_(limit_submission) {
    if (check && !Str::valid_name(name)) {
        throw std::runtime_error("InLimit::InLimit: Invalid InLimit name: " + name);
    }
    if (limit_this_node_only_ && limit_submission_) {
        throw std::runtime_error(
            "InLimit::InLimit: can't limit family only(-n) and limit submission(-s) at the same time");
    }
}

bool InLimit::operator==(const InLimit& rhs) const {
    if (path_ != rhs.path_) {
#ifdef DEBUG
        if (Ecf::debug_equality())
            std::cout << "InLimit::operator==   path_ != rhs.path_\n";
#endif
        return false;
    }
    if (n_ != rhs.n_) {
#ifdef DEBUG
        if (Ecf::debug_equality())
            std::cout << "InLimit::operator==     n_ != rhs.n_\n";
#endif
        return false;
    }
    if (tokens_ != rhs.tokens_) {
#ifdef DEBUG
        if (Ecf::debug_equality())
            std::cout << "InLimit::operator==    tokens_(" << tokens_ << ") != rhs.tokens_(" << rhs.tokens_ << ")\n";
#endif
        return false;
    }

    if (limit_this_node_only_ != rhs.limit_this_node_only_) {
#ifdef DEBUG
        if (Ecf::debug_equality())
            std::cout << "InLimit::operator==    limit_this_node_only_(" << limit_this_node_only_
                      << ") != rhs.limit_this_node_only_(" << rhs.limit_this_node_only_ << ")\n";
#endif
        return false;
    }
    if (limit_submission_ != rhs.limit_submission_) {
#ifdef DEBUG
        if (Ecf::debug_equality())
            std::cout << "InLimit::operator==     limit_submission_(" << limit_submission_
                      << ") != rhs.limit_submission_ (" << rhs.limit_submission_ << ")\n";
#endif
        return false;
    }
    if (incremented_ != rhs.incremented_) {
#ifdef DEBUG
        if (Ecf::debug_equality())
            std::cout << "InLimit::operator==    incremented_(" << incremented_ << ") != rhs.incremented_("
                      << rhs.incremented_ << ")\n";
#endif
        return false;
    }

    // Note: comparison does not look at Limit pointers
    return true;
}

void InLimit::print(std::string& os) const {
    Indentor in;
    Indentor::indent(os);
    write(os);

    if (!PrintStyle::defsStyle()) {

        // write state; See InlimitParser::doParse for read state part
        if (incremented_)
            os += " # incremented:1";

        if (PrintStyle::getStyle() == PrintStyle::STATE) {
            Limit* the_limit = limit();
            if (the_limit) {
                os += " # referenced limit(value) ";
                os += ecf::convert_to<std::string>(the_limit->theLimit());
                os += "(";
                os += ecf::convert_to<std::string>(the_limit->value());
                os += ")";
            }
        }
    }

    os += "\n";
}

std::string InLimit::toString() const {
    std::string ret;
    write(ret);
    return ret;
}

void InLimit::write(std::string& ret) const {
    ret += "inlimit ";
    if (limit_this_node_only_)
        ret += "-n ";
    if (limit_submission_)
        ret += "-s ";
    if (path_.empty())
        ret += n_;
    else {
        ret += path_;
        ret += Str::COLON();
        ret += n_;
    }
    if (tokens_ != 1) {
        ret += " ";
        ret += ecf::convert_to<std::string>(tokens_);
    }
}

template <class Archive>
void InLimit::serialize(Archive& ar) {
    ar(CEREAL_NVP(n_));
    CEREAL_OPTIONAL_NVP(ar, path_, [this]() { return !path_.empty(); }); // conditionally save
    CEREAL_OPTIONAL_NVP(ar, tokens_, [this]() { return tokens_ != 1; }); // conditionally save
    CEREAL_OPTIONAL_NVP(
        ar, limit_this_node_only_, [this]() { return limit_this_node_only_; }); // conditionally save new to 5.0.0
    CEREAL_OPTIONAL_NVP(
        ar, limit_submission_, [this]() { return limit_submission_; });       // conditionally save new to 5.0.0
    CEREAL_OPTIONAL_NVP(ar, incremented_, [this]() { return incremented_; }); // conditionally save new to 5.0.0
}
CEREAL_TEMPLATE_SPECIALIZE(InLimit);
