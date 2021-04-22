#!/bin/bash

destfun=$1
echo ftrace function is \"$destfun\"

debugfs=/sys/kernel/debug

echo nop > $debugfs/tracing/current_tracer

echo 0 > $debugfs/tracing/tracing_on

echo $$ > $debugfs/tracing/set_ftrace_pid

echo function_graph > $debugfs/tracing/current_tracer

#replace test_proc_show by your function name

#echo vfs_read > $debugfs/tracing/set_graph_function

echo "$destfun" > $debugfs/tracing/set_graph_function
#echo ftrace func is `cat $debugfs/tracing/set_graph_function`.


echo 1 > $debugfs/tracing/tracing_on


#exec "$@"
