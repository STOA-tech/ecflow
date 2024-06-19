
if [[ $HOST = lxop* ]]; then
  ln -sf $(echo ${PBS_NODEFILE:=} | sed -e 's:aux:spool:').OU %ECF_JOBOUT%
fi

# Notify ecFlow of a normal end
#
# we call wait to let all background process stop
# Also even if we had foreground process, the complete message
# could arrive at the server before say, a --label command
# This would result in zombies. To avoid this we use wait
wait
ecflow_client --complete  --host=%ECF_HOST% --port=%ECF_PORT%
trap 0                 # Remove all traps
exit 0                 # End the shell
