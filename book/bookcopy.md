### sysrq
* 查内存, echo m > /proc/sysrq-trigger; 执行命令后使用dmesg来查看输出。
* 查看backtrace, echo l > /proc/sysrq-trigger; 同上，还是需要dmesg来查看输出。
* 查看Register, echo p > /proc/sysrq-trigger; 同上，还是需要dmesg来查看输出。
* 触发crash, echo c > /proc/sysrq-trigger; 系统会crash，并记录crash发生时系统内存并将其转储到/var/crash/目录下, 当然系统会重启；
* 查看活动的timer, echo q > /proc/sysrq-trigger; 同样用dmesg查结果。
* 查看task state, echo t > /proc/sysrq-trigger; 同样用dmesg查结果。
* reboot echo b > /proc/sysrq-trigger; 
* 执行sync echo s > /proc/sysrq-trigger; 

### 查看系统服务状态
* service sshd status
* service kdump status
* service X status


