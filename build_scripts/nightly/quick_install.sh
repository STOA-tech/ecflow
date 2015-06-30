#!/bin/sh
# assume $WK is defined
# Alter the command below to either
# a/ use the system installed version, everywhere, avoid miss-match between different releases
# b/ Test the latest release, requires compatible client/server versions

set -u # fail when using an undefined variable
set -x # echo script lines as they are executed

# =======================================================================
# Create build scripts files. Must be before python $WK/build_scripts/nightly/build.py
# =======================================================================
rm -rf nightly
cp -r $WK/build_scripts/nightly .

# =======================================================================
# Generate the defs, the is loaded into the server by load.py
# =======================================================================
python $WK/build_scripts/nightly/build.py
if [[ $? = 1 ]] ; then
   exit 1
fi

# =======================================================================
# Kill the server
# =======================================================================
which ecflow_client
ecflow_client --version
ecflow_client --terminate=yes --port=4141

# =======================================================================
# Start server. 
# =======================================================================
rm -rf `hostname`.4141.*

export ECF_ALLOW_OLD_CLIENT_NEW_SERVER=9
ecflow_server --port=4141 &
sleep 4

# =======================================================================
# load the build defs, in the server then delete generated defs.
# Make sure server is running
# =======================================================================
python $WK/build_scripts/nightly/load.py

# =======================================================================
# Start the viewer
# =======================================================================
ecflowview &