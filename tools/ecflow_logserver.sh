#!/bin/bash

#=========================================================================================
# Syntax
# start_logserver [-d <home_directory>] [-m <server_map>] [-l <server_log_file> [-h]
#
#=========================================================================================

USAGE() {
echo "Usage: $0 [-d <dir>] [-m <map>] [-l <logfile>] [-h]"
echo "       -d <dir>     specify the directory name where files will be served"
echo "                    from - default is \$HOME"
echo "       -m <map>     gives mapping between local directory and directory"
echo "                    where ecflow server runs - default is <dir>:<dir>"
echo "       -l <logfile> logserver log file - default is \$SCRATCH/log/logfile"
echo "       -h           print this help page"
echo "Example:"
echo "       start_logserver.sh -d %ECF_OUT% -m %ECF_HOME%:%ECF_OUT% -l $HOME/log/logserver.log"
}

LOGSERVERLIST=/home/us/usx/logserver/user.lst

# get command line options if any.

while getopts hd:m:l: option
do
  case $option in
    d)
      server_dir=$OPTARG
      ;;
    m)
      server_map=$OPTARG
      ;;
    l)
      server_logfile=$OPTARG
      ;;
    h)
      USAGE
      exit 0
      ;;
    *)
      USAGE
      exit 1
      ;;
  esac
done

# Check that time-critical users have set the server_dir and server_logdir

if [[ $(/usr/bin/groups) == *timecrit* || $LOGNAME == emos ]]; then
  EC_TIMECRIT_UID=yes
else
  EC_TIMECRIT_UID=no
fi


if [[ "${EC_TIMECRIT_UID}" == "yes" ]] ; then

# Time-critical user with no $HOME set

  if [[ "${server_dir:-}" == "" ]] ; then
    echo "Set the location of the server directory with -d" 
    echo ""
    USAGE
    exit 1
  fi
  if  [[ "${server_logfile:-}" == "" ]] ; then
    echo "Set the location of the server log file with -l" 
    echo ""
    USAGE
    exit 1
  fi
else 
# Source the profile to set $HOME and $TEMP/$SCRATCH
  . ~/.profile > /dev/null 2>&1
fi

# LOGPATH gives the name of the directory that will serve the files.
# Only one path can be given.

export LOGPATH=${server_dir:-$HOME}

LOGFILE=${server_logfile:-$SCRATCH/log/logserver.log}

# LOGMAP will contain series of two directories, separated with ":"
# The first directory name will be the local directory serving the files and
# the second directory name will give the corresponding directory on the
# remote system where the ecflow server runs. This variable is only needed if
# some files are moved from the local system back to the ecflow server.

export LOGMAP=${server_map:-$LOGPATH:$LOGPATH}

# prognum is set based on the unique users numeric uid.

username=$(id -u)
base=35000
prognum=$((base+username))

PROG=$(which $0)
PROG_PATH=$(readlink -f $PROG)
PATH_NAME=$ecflow_DIR/bin

export LOGPORT=$prognum
LOGDIR=$(dirname $LOGFILE)

[[ ! -d $LOGDIR ]] && mkdir -p $LOGDIR

check=$(ps -fu ${USER} | grep ecflow_logsvr.pl | grep -v grep 1>/dev/null 2>&1 \
 && echo 1 || echo 0)
if [ $check = 0 ] ; then
  nohup $PATH_NAME/ecflow_logsvr.pl 1>$LOGFILE 2>&1 &
else
  exit 0
fi

sleep 1

check=$(ps -fu ${USER} | grep ecflow_logsvr.pl | grep -v grep 1>/dev/null 2>&1 \
 && echo 1 || echo 0)
if [ $check = 0 ] ; then
  /usr/bin/tail -n 30 $LOGFILE | /bin/mail -s "$(hostname -s): logserver for ${USER} did not start. Please investigate..." -c "root" ${USER}
  exit 1
fi


if [[ -f ${LOGSERVERLIST} ]] ; then
  logserverfound=$(grep $LOGPORT ${LOGSERVERLIST} | grep $USER)
  if [[ -z $logserverfound ]]; then
    echo $USER    $LOGPORT   $LOGPATH $LOGMAP>> ${LOGSERVERLIST}
  fi  
fi

