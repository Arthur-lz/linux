### Linux Kernel文件系统写I/O流程代码分析（二）bdi_writeback
https://www.cnblogs.com/jimbo17/p/10491223.html

###  bio
http://www.360doc.com/content/19/1223/08/58701552_881505827.shtml

### ftrace
第一步, 使用下述脚本建立文件ftrace.sh
```shell
#!/bin/bash


debugfs=/sys/kernel/debug

echo nop > $debugfs/tracing/current_tracer

echo 0 > $debugfs/tracing/tracing_on

echo $$ > $debugfs/tracing/set_ftrace_pid

echo function_graph > $debugfs/tracing/current_tracer

#replace test_proc_show by your function name

echo test_proc_show > $debugfs/tracing/set_graph_function

echo 1 > $debugfs/tracing/tracing_on

exec "$@"
```
第二步, 之后用此脚本执行需要被追踪函数的所在程序，如函数test_proc在程序A中，则在终端执行
./ftrace.sh A

第三步，将刚才的追踪结果输出到文件1, 
	cat /sys/kernel/debug/tracing/trace > 1

