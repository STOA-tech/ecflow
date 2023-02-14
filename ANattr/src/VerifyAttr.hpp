#ifndef VERIFYATTR_HPP_
#define VERIFYATTR_HPP_
//============================================================================
// Name        :
// Author      : Avi
// Revision    : $Revision: #10 $
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
#include "NState.hpp"

// Class VerifyAttr:
// This class is only used for testing/verification purposes. It allows us to
// embed expected number of states, within the definition file and so
// reduce the need for golden log files.
// Use compiler , generated destructor, assignment, copy constructor
class VerifyAttr {
public:
    VerifyAttr(NState::State state, int expected, int actual = 0)
        : state_(state), expected_(expected), actual_(actual), state_change_no_(0) {}
    VerifyAttr() = default;

    bool operator==(const VerifyAttr& rhs) const;
    void print(std::string&) const;

    NState::State state() const { return state_; }
    int expected() const { return expected_; }
    int actual() const { return actual_; }
    void incrementActual();
    void reset();

    // The state_change_no is never reset. Must be incremented if it can affect equality
    unsigned int state_change_no() const { return state_change_no_; }

    std::string toString() const;
    std::string dump() const;

private:
    NState::State state_{NState::UNKNOWN};
    int expected_{0};
    int actual_{0};
    unsigned int state_change_no_{0}; // *not* persisted, only used on server side

    friend class cereal::access;
    template <class Archive>
    void serialize(Archive& ar);
};

#endif
