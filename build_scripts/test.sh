#!/bin/sh

## Copyright 2009- ECMWF.
## This software is licensed under the terms of the Apache Licence version 2.0 
## which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
## In applying this licence, ECMWF does not waive the privileges and immunities 
## granted to it by virtue of its status as an intergovernmental organisation 
## nor does it submit to any jurisdiction. 

#===================================================================
# Run all the ecflow tests, note use !/bin/bash to keep rpm happy, it uses bash
#===================================================================

if [ "$#" -gt 4 ] ; then
   echo "Maximum of 3 arguments expected"
   echo " arg1-mode    (optional) default = release  [ release | debug | profile ]"
   echo " arg2-compiler(optional) default = linux/gcc-$(gcc -dumpversion)"
   echo " arg3-safe    (optional) default = no  [ no | safe ] safe means only run deterministic tests"
   echo " arg5-link    (optional) default = static [ static | shared ] use boost static/shared directory to reference tests"
   echo " arg5-ssl     (optional) default = off [ off | ssl ] ssl means test ssl version of ecflow"
   exit 1
fi

link=static
mode=release
compiler_arg=
safe=no
ssl=off
while [ "$#" -ne 0 ] ; do   
   if [ "$1" = debug -o "$1" = release -o "$1" = profile ] ; then
      mode=$1
   elif [ "$1" = safe ] ;   then safe=yes ;
   elif [ "$1" = ssl ] ;    then ssl=on ;
   elif [ "$1" = shared ] ; then link=shared ;
   else
      compiler_arg=$1
   fi
   # shift remove last argument
   shift
done

if [ ${#mode} -eq 0 ] ; then
   echo "test expects mode i.e. debug or release"
   exit 1
fi

link_dir=
if [ ${link} = static ] ; then
   link_dir=/link-static
fi

echo "mode=$mode compiler=$compiler_arg safe=$safe ssl=$ssl link=$link link_dir=$link_dir"

#======================================================================
# remove python test, so that they are rerun
rm -rf Pyext/bin/*.test   

# Remove any lock file create by tests which used EcfPortLock.hpp
# ** However DO *NOT* remove this locks for in ecflow test.ecf
# ** as they allow the debug and release test to run at the same time.
rm -rf *.lock
 
# Check that a command is in the PATH.
test_path ()
{
    if `command -v command 1>/dev/null 2>/dev/null`; then
        command -v $1 1>/dev/null 2>/dev/null
    else
        hash $1 1>/dev/null 2>/dev/null
    fi
}

# Check that the OS name, as returned by "uname", is as given.
test_uname ()
{
    if test_path uname; then
        test `uname` = $*
    fi
}

unset ECF_PORT
OPEN_SSL=

if test_uname Linux ; then

   TOOLSET=
   CXXFLAGS=
   compiler=gcc-$(gcc -dumpversion)
   
   # When on cray check PE_ENV for environment
   if [ "$PE_ENV" = INTEL -o "$PE_ENV" = CRAY -o "$PE_ENV" = GNU ]
   then
       CXXFLAGS=cxxflags=-fPIC
       TOOLSET=toolset=gcc
       if [ "$PE_ENV" = INTEL ] ; then
          compiler=intel-linux
          TOOLSET=toolset=intel
       fi
       if [ "$PE_ENV" = CRAY ] ; then
          compiler=cray
          TOOLSET=toolset=cray
       fi
   fi   
   
   # Allow the compiler to be overridden on linux
   if [ ${#compiler_arg} -ne 0 ] ; then
      compiler=$compiler_arg
   fi

   exe_path=$compiler/${mode}${link_dir}
   
   if [ "$ssl" = on ] ; then
      exe_path="$exe_path/ssl-on"
      OPEN_SSL="ssl=on"
   fi
   
   echo "*****************************************"
   echo "Testing: $exe_path"
   echo "*****************************************"

   ACore/bin/$exe_path/u_acore      --log_level=message $TEST_OPTS
   ANattr/bin/$exe_path/u_anattr    --log_level=message $TEST_OPTS
   ANode/bin/$exe_path/u_anode      --log_level=message $TEST_OPTS
   ANode/parser/bin/$exe_path/u_aparser  --log_level=message $TEST_OPTS
   if [ "$safe" = no ] ; then
      ANode/parser/bin/$exe_path/perf_aparser --log_level=message $TEST_OPTS
   fi
   Base/bin/$exe_path/u_base         --log_level=message $TEST_OPTS
   
   if [ "$safe" = no ] ; then
      # run python/C++ test
      cd Pyext
      $BOOST_ROOT/b2 $TOOLSET "$CXXFLAGS" variant=$mode link=$link $OPEN_SSL test-all $TEST_OPTS
      cd ..
   fi
   
   Client/bin/$exe_path/s_client     --log_level=message $TEST_OPTS
   Server/bin/$exe_path/u_server     --log_level=message $TEST_OPTS
   CSim/bin/$exe_path/c_csim         --log_level=message $TEST_OPTS
   if [ "$safe" = no ] ; then
      Test/bin/$exe_path/s_test          --log_level=message $TEST_OPTS
      Test/bin/$exe_path/s_test_zombies  --log_level=message $TEST_OPTS
   fi
   
elif test_uname Darwin ; then

   TOOLSET=
   CXXFLAGS=cxxflags="-Wno-deprecated-declarations -Wno-maybe-uninitialized -fvisibility-inlines-hidden -fvisibility=hidden"
   compiler=gcc-$(/usr/local/opt/gcc@10/bin/gcc-10 -dumpversion)
   
   exe_path=$compiler/${mode}${link_dir}
   
   echo "*****************************************"
   echo "Testing: $exe_path"
   echo "*****************************************"

   ACore/bin/$exe_path/u_acore          --log_level=message $TEST_OPTS
   ANattr/bin/$exe_path/u_anattr        --log_level=message $TEST_OPTS
   ANode/bin/$exe_path/u_anode          --log_level=message $TEST_OPTS
   ANode/parser/bin/$exe_path/u_aparser --log_level=message $TEST_OPTS
   if [ "$safe" = no ] ; then
      ANode/parser/bin/$exe_path/perf_aparser --log_level=message $TEST_OPTS
   fi
   Base/bin/$exe_path/u_base         --log_level=message $TEST_OPTS
   
   if [ "$safe" = no ] ; then
      # run python/C++ test
      cd Pyext
      $BOOST_ROOT/b2 $TOOLSET "$CXXFLAGS" variant=$mode link=$link $OPEN_SSL test-all $TEST_OPTS
      cd ..
   fi
   
   Client/bin/$exe_path/s_client     --log_level=message $TEST_OPTS
   Server/bin/$exe_path/u_server     --log_level=message $TEST_OPTS
   CSim/bin/$exe_path/c_csim         --log_level=message $TEST_OPTS
   if [ "$safe" = no ] ; then
      Test/bin/$exe_path/s_test          --log_level=message $TEST_OPTS
      Test/bin/$exe_path/s_test_zombies  --log_level=message $TEST_OPTS
   fi
   
elif test_uname HP-UX ; then

   echo "Testing: variant=$mode"
   ACore/bin/acc/$mode/threading-multi/u_acore  --log_level=message $TEST_OPTS
   ANattr/bin/acc/$mode/threading-multi/u_anattr  --log_level=message $TEST_OPTS
   ANode/bin/acc/$mode/threading-multi/u_anode  --log_level=message $TEST_OPTS
   ANode/parser/bin/acc/$mode/threading-multi/u_aparser  --log_level=message $TEST_OPTS
   if [ "$safe" = no ] ; then
      ANode/parser/bin/acc/$mode/threading-multi/perf_aparser  --log_level=message $TEST_OPTS
   fi
   Base/bin/acc/$mode/threading-multi/u_base  --log_level=message $TEST_OPTS
   if [ "$safe" = no ] ; then
      # run python/C++ test, use test to bypass 'with' statement tests
      cd Pyext
      $BOOST_ROOT/b2 variant=$mode test $TEST_OPTS
      cd ..
   fi
   exit 1
   Client/bin/acc/$mode/threading-multi/s_client  --log_level=message $TEST_OPTS
   Server/bin/acc/$mode/threading-multi/u_server  --log_level=message $TEST_OPTS
   CSim/bin/acc/$mode/threading-multi/c_csim  --log_level=message $TEST_OPTS
   if [ "$safe" = no ] ; then
      Test/bin/acc/$mode/threading-multi/s_test  --log_level=message $TEST_OPTS
      Test/bin/acc/$mode/threading-multi/s_test_zombies  --log_level=message $TEST_OPTS
   fi
   
elif test_uname AIX ; then

   echo "Testing: $ARCH variant=$mode"
   
   ACore/bin/vacpp/$mode/threading-multi/u_acore  --log_level=message $TEST_OPTS
   ANattr/bin/vacpp/$mode/threading-multi/u_anattr  --log_level=message $TEST_OPTS
   ANode/bin/vacpp/$mode/threading-multi/u_anode  --log_level=message $TEST_OPTS
   ANode/parser/bin/vacpp/$mode/threading-multi/u_aparser  --log_level=message $TEST_OPTS
   if [ "$safe" = no ] ; then
      ANode/parser/bin/vacpp/$mode/threading-multi/perf_aparser  --log_level=message $TEST_OPTS
   fi
   Base/bin/vacpp/$mode/threading-multi/u_base  --log_level=message $TEST_OPTS
   if [ "$safe" = no ] ; then
      # run python/C++ test
      cd Pyext
      $BOOST_ROOT/b2 variant=$mode test-all $TEST_OPTS
      cd ..
   fi
   Client/bin/vacpp/$mode/threading-multi/s_client  --log_level=message $TEST_OPTS
   Server/bin/vacpp/$mode/threading-multi/u_server  --log_level=message $TEST_OPTS
   CSim/bin/vacpp/$mode/threading-multi/c_csim  --log_level=message $TEST_OPTS
   if [ "$safe" = no ] ; then
      Test/bin/vacpp/$mode/threading-multi/s_test  --log_level=message $TEST_OPTS
      Test/bin/vacpp/$mode/threading-multi/s_test_zombies  --log_level=message $TEST_OPTS
   fi
fi
