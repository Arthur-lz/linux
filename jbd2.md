# jbd2知识收集
> jbd2 英文名是这样的：Journaling Block Device 2
## jbd的全拼是journaling block driver 文件系统的日志功能，jbd2是ext4文件系统版本
```c
/* 常用的文件系统使用日志功能来保证文件系统的完整性，在写入新的数据到磁盘之前，会先将元数据写入日志；
 * 这样做可以保证在写入真实数据前后，一旦发生错误，
 * 日志功能将很容易回滚到之前的状态，确保不会发生文件系统崩溃的情况
 */
```
## 现在的磁盘上一般都配有内部缓存，以便重新调整批量数据的写入顺序，优化写入性能，因此文件系统必须在日志数据写入磁盘之后才能写commit（commit=xx 每xx秒同步所有的数据和元数据。默认是每5秒）记录；若commit记录写入在先，而日志有可能损坏，就会影响数据完整性；EXT4文件系统默认启用barrier功能，只有当barrier之前的数据全部写入磁盘，才能写barrier之后的数据；barrier的存在保证了数据完整性

## 用barrier特性的不利之处在于，需要付出降低性能的代价；可以通过挂载项 mount -o barrier=0来禁用此特性；
> 可通过查看/proc/mounts里的barrier值 为1代表启用

## 分析系统中的sync调用
### 1. 打开内核trace
```sh
#!/bin/bash
echo 1 > /sys/kernel/debug/tracing/events/jbd2/jbd2_commit_flushing/enable			# 在 jbd2执行 flush 时输出日志
echo 1 > /sys/kernel/debug/tracing/events/ext4/ext4_sync_file_enter/enable			# 在任意进程执行 sync 时输出日志
```
### 2. 查看trace日志输出
```sh
sudo cat /sys/kernel/debug/tracing/trace_pipe
```

### 3. ps 看一下是哪个任务导致的
```sh
ps -efL | grep pid
```

### 4. strace 查一下进程在做什么
```sh
strace -f -p pid
```

### 5. 根据 fd 号去看看这是什么文件
```sh
sudo ls -alt /proc/{xxx_pid}/fd/fd_num
```
