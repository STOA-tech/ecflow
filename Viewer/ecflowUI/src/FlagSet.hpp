/*
 * Copyright 2009- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#ifndef ecflow_viewer_FlagSet_HPP
#define ecflow_viewer_FlagSet_HPP

template <class T>
class FlagSet {
public:
    FlagSet() = default;
    explicit FlagSet(T t) { set(t); }

    void clear() { flags_ = 0; }
    void set(T flag) { flags_ |= (1 << flag); }
    void unset(T flag) { flags_ &= ~(1 << flag); }
    bool isSet(T flag) const { return (flags_ >> flag) & 1; }
    bool isEmpty() const { return flags_ == 0; }
    bool sameAs(T flag) const { return flags_ == flag; }

private:
    int flags_{0};
};

#endif /* ecflow_viewer_FlagSet_HPP */
