# eCryptfs
> ecryptfs介于vfs和具体文件系统(ext3,ext4,xfs)之间，是一种堆叠文件系统，它不负责与磁盘I/O操作。

> ecryptfs可以独立编译成模块，也可以与内核打包到一起

#### 内核只编译一个模块的方法(这里以编译ecryptfs为例)
* 第一步进入内核源码所在目录 
* 执行make modules M=fs/ecryptfs/ -j4 

> clean内核某个模块执行make clean M=fs/ecryptfs/ -j4

## 如何使用ecryptfs
#### 第一步、安装ecryptfs-utils
> 应用层用户工具，主要用于key的管理，与ecryptfs内核模块交互

>centos直接yum install ecryptfs-utils，如果安装时提示无此安装包，可以到mirrors.aliyun.com上找到对应的源后更新一下本地仓库，之后再安装。

> 注：安装完成后，默认在系统启动后会挂载 /root/.ecryptfs

> 可以使用cat /proc/mounts 来查看当前挂载了哪些东东

> 执行

#### 第二步、编译安装ecryptfs
* 这里以centos 7.8.2003版本(kernel 3.10.0-1127.el7.x86_64)举例说明

* 下载与centos 内核相同版本内核
* 进入内核源码目录, make menuconfig
将ecryptfs配置为M
* 编译所有内核模块make modules
* 安装ecryptfs, insmod fs/ecryptfs/ecryptfs.ko

#### 第三步、以ecryptfs文件系统类型挂载到某个文件夹
* mkdir test
* mount -t ecryptfs test test
> 如果前面的utils工具安装配置正确，则上面的挂载命令回车执行后，终端会提示选择加密方式、加密算法、密钥长度等挂载配置信息。

* 测试一下挂载成功后的test目录下的文件是否是加密的
> touch test/a

> echo abc > test/a

>umount test

执行完上面几个命令后，观察一下test目录下的文件a，如果一切顺利，其文件大小应为12k, 文件内容应为加密后的密文

> 之后再次挂载查看文件test/a, 如果a的内容是解密后的明文abc，且文件大小变成4byte，那么说明ecryptfs一切工作正常

### 总结

* ecryptfs文件系统，取消挂载后，实际存到磁盘上的文件会比文件明文状态下要大至少8k，因为ecryptfs在文件头中将文件的元数据以8k块大小保存了起来，另外，如果文件明文大小小于4k，则ecryptfs会在文件明文尾补0之后加密，这样文件大小就会变成4k+8k=12k。

* ceryptfs的另一个特征是，它会为每一个文件建立不同的FEK，并将其加密为EFEK并保存到inode中。这也就是说，经ecryptfs加密的每一个文件，它们加密用的密钥FEK都是不同的。

