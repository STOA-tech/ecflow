#////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
# Name        :
# Author      : Avi
# Revision    : $Revision: #10 $
#
# Copyright 2009-2016 ECMWF.
# This software is licensed under the terms of the Apache Licence version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation
# nor does it submit to any jurisdiction.
#////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8

# code for testing with statement
# In our case it provides the ability to indent

from ecflow import Defs, Suite, Family, Client, debug_build
import sys
    
if __name__ == "__main__":
    print("####################################################################")
    print("Running ecflow version " + Client().version() + " debug build(" + str(debug_build()) +")")
    print("####################################################################")
 
    # sys.version_info is a tuple containing (major,minor,micro,releaselevel,serial)
    version = sys.version_info;
    if (sys.version_info < (2, 7)):
        print("This test only run with python version 2.7 or greater, but found : " + str(version))
        exit(0)
    
    print("start test")
    # Add with context manager, in our case this only provides ability to indent
    with Defs() as defs:
        with defs.add_suite("s2") as s2:
            with s2.add_family("f1") as f1:
                with f1.add_task("t1") as t1:
                    assert len(list(f1.nodes)) == 1,"Expected 1 task"
            s2.add_family("f2")
            s2.add_task("t1")
        assert len(list(s2.nodes)) == 3,"Expected 2 families and one task suite"
    assert len(list(defs.suites)) == 1,"Expected 1 suite"
#    print defs
 
    with Suite("s2") as s2:
        s2.add_family("f1")
        s2.add_task("t1")
        assert len(list(s2.nodes)) == 2,"Expected 2 nodes family and task"
        
    with Family("f1") as f1:
        f1.add_family("f2")
        f1.add_task("t1")
        assert len(list(s2.nodes)) == 2,"Expected 2 nodes family and task"
        
        
    #
    # Add Nodes functional way
    #
    defs1 = Defs()
    defs1.add_suite("s1").add_task("t1").add_variable("var","v")
    defs1.add_suite("s2").add_family("f1").add_task("t1").add_variable("var","v")
    defs1.add_suite("s3").add_family("f1").add_family("f2").add_task("t1").add_variable("var","v")
    
    # add node using with, they should compare
    with Defs() as defs2:
        with defs2.add_suite("s1") as s1:
            t1 = s1.add_task("t1")
            t1.add_variable("var","v")
        with defs2.add_suite("s2") as s2:
            with s2.add_family("f1") as f1:
                with f1.add_task("t1") as t1:
                    t1.add_variable("var","v")
        with defs2.add_suite("s3") as s3:
            with s3.add_family("f1") as f1:
                with f1.add_family("f2") as f2: 
                    with f2.add_task("t1") as t1: 
                        t1.add_variable("var","v")
                    
    assert defs1 == defs2,"expected defs to be the same"        
        
    print("All tests pass")