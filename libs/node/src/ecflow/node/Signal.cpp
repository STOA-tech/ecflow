/*
 * Copyright 2009- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#include "ecflow/node/Signal.hpp"

#include <csignal>

namespace ecf {

// SIGCHLD : Child status has changed (POSIX)
// Signal sent to parent process whenever one of its child processes terminates or stops.

Signal::Signal() = default;

Signal::~Signal() {
    // Unblock SIGCHLD. This will call the signal-handler in System.cpp,
    // *IF* we have pending SIGCHLD
    // This will not return until we have handled all the pending SIGCHLD signal
    unblock_sigchild();

    // Once the signals are processed, block them until we come in here again
    // Now block again.
    block_sigchild();
}

void Signal::block_sigchild() {
    // Now block again.
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGCHLD);
    sigprocmask(SIG_BLOCK, &set, nullptr);
}

void Signal::unblock_sigchild() {
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGCHLD);
    sigprocmask(SIG_UNBLOCK, &set, nullptr);
}

} // namespace ecf
