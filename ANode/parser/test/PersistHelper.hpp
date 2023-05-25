#ifndef PERSISTHELPER_HPP_
#define PERSISTHELPER_HPP_
//============================================================================
// Name        :
// Author      : Avi
// Revision    : $Revision$
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

#include <string>

#include "PrintStyle.hpp"
class Defs;

/// Given a in memory defs file, this class will write it to disk
/// and reload the definition file structure and will then make a comparison
/// to ensure they are the same
class PersistHelper {
public:
    explicit PersistHelper(bool compare_edit_history = false) : compare_edit_history_(compare_edit_history) {}

    bool
    test_persist_and_reload(const Defs& theInMemoryDefs, PrintStyle::Type_t file_type_on_disk, bool do_compare = true);
    bool test_defs_checkpt_and_reload(const Defs& theInMemoryDefs, bool do_compare = true);
    bool test_cereal_checkpt_and_reload(const Defs& theInMemoryDefs, bool do_compare = true);
    bool test_state_persist_and_reload_with_checkpt(const Defs& theInMemoryDefs);
    const std::string& errorMsg() const { return errorMsg_; }

    /// returns the file size of the temporary file created by:
    ///   test_persist_and_reload(..) or test_cereal_checkpt_and_reload(..)
    size_t file_size() const { return file_size_; }

private:
    bool reload_from_defs_file(const Defs& theInMemoryDefs,
                               Defs& reloaded_defs,
                               const std::string& filename,
                               bool do_compare = true);
    bool reload_from_cereal_checkpt_file(const Defs& theInMemoryDefs, Defs& reloaded_defs, bool do_compare = true);

private:
    PersistHelper(const PersistHelper&)                  = delete;
    const PersistHelper& operator=(const PersistHelper&) = delete;

private:
    std::string errorMsg_;
    size_t file_size_{0};
    bool compare_edit_history_;
};
#endif
