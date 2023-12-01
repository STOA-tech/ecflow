/*
 * Copyright 2009- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#ifndef ecflow_node_JobCreationCtrl_HPP
#define ecflow_node_JobCreationCtrl_HPP

#include <string>
#include <vector>

#include "ecflow/node/JobsParam.hpp"
#include "ecflow/node/NodeFwd.hpp"

// Used as a utility class for testing Job creation
// Collates data during the node tree traversal
// Note: For testing purposes we do not always want to create jobs
class JobCreationCtrl : public std::enable_shared_from_this<JobCreationCtrl> {
private:
    JobCreationCtrl(const JobCreationCtrl&)                  = delete;
    const JobCreationCtrl& operator=(const JobCreationCtrl&) = delete;

public:
    JobCreationCtrl() = default;

    void set_node_path(const std::string& absNodePath) { absNodePath_ = absNodePath; }
    const std::string& node_path() const { return absNodePath_; }

    void generate_temp_dir();
    void set_dir_for_job_creation(const std::string& tempDirForJobGeneration) {
        tempDirForJobGeneration_ = tempDirForJobGeneration;
    }
    const std::string& dir_for_job_creation() { return tempDirForJobGeneration_; }

    std::string& error_msg() { return errorMsg_; }
    const std::string& get_error_msg() const { return errorMsg_; }

    void push_back_failing_submittable(submittable_ptr t) { fail_submittables_.push_back(t); }
    const std::vector<weak_submittable_ptr>& fail_submittables() const { return fail_submittables_; }

    void set_verbose(bool verbose) { verbose_ = verbose; }
    bool verbose() const { return verbose_; }

    // Used in job creation checking. Holds EcfFile, which has a CACHE of included files.
    // Use to minimise file opening of included files
    // Since we hold a only a single JobsParam over ALL job creation testing. See ECFLOW-1210
    JobsParam& jobsParam() { return jobsParam_; }

private:
    bool verbose_{false};
    std::string absNodePath_;
    std::string tempDirForJobGeneration_;
    std::string errorMsg_;
    std::vector<weak_submittable_ptr> fail_submittables_;
    JobsParam jobsParam_; // create jobs = false, spawn jobs = false used as a cache
};

#endif /* ecflow_node_JobCreationCtrl_HPP */
