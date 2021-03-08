1.buffered_aio_write切入点，只有新数据、文件、inode、pos，这些已知数据不足以判断新数据是否已经写过这个文件？

2.cat 操作重复读操作，可以在内核中区分(在进一步增加关注点)，在buffered_aio_read中有如下输出，
[  131.039729] xfs_file_buffered_aio_read, nr: 1, 2021146, adflajdsflj
, pos:0, user_datap:0000000001213000, gfar: 12
[  131.039801] xfs_file_buffered_aio_read, nr: 1, 2021146, adflajdsflj
, pos:12, user_datap:0000000001213000, gfar: 0
由上可见，
前后两次由cat引起的buffered_aio_read调用，
1）第一次和第二次的用户空间数据地址指向同一位置，说明是同一用户空间数据；
2）gfar(generic_file_aio_read)返回值代表实际成功读取的字节数，第一次调用gfar时返回的是12，与文件内容adflajdsflj长度相同（包含结束符\0)共12字节；
     第二次调用gfar时返回的是0，这代表着gfar未读到新数据，那么由此可以判断，不用执行驱动调用来做板卡解密操作！！

3.hexdump操作重复读操作
root@localhost /h/a/appraise# hexdump a
0000000 6461 6c66 6a61 7364 6c66 0a6a         
000000c
root@localhost /h/a/appraise# dmesg -c

[  806.710069] xfs_file_buffered_aio_read, nr: 1, 2021146, adflajdsflj
, pos:0, user_datap:00007fdcfbad0000, gfar: 12
[  806.710086] xfs_file_buffered_aio_read, nr: 1, 2021146, adflajdsflj
, pos:12, user_datap:00007fdcfbad0000, gfar: 0
[  806.710097] xfs_file_buffered_aio_read, nr: 1, 2021146, adflajdsflj
, pos:12, user_datap:00007fdcfbad0000, gfar: 0
不出所料，与cat的重复两次读一样，这里读三次，后两次呢，嘿嘿，可以明确过滤掉！

4.vim操作重复读操作
root@localhost /h/a/appraise# vim a
root@localhost /h/a/appraise# dmesg -c

[ 1008.193734] xfs_file_buffered_aio_read, nr: 1, 2021146, adflajdsflj
\xffffff8c, pos:0, user_datap:0000000001674f70, gfar: 12
[ 1008.193878] xfs_file_buffered_aio_read, nr: 1, 2021146, , pos:12, user_datap:0000000001685680, gfar: 0
除了第一次读出的内容略与cat, hexdump不同外，这两次读的gfar返回值是与cat, hexdump相同的，所以呢，同样可以过滤掉！

5.大文件的cat 读操作(注：vim读，每次最大也是64k; hexdump读，每次最大是4k;)
[  384.068226] xfs_file_buffered_aio_read, nr: 1, 1074413040, , pos:0, user_datap:0000000000f24000, gfar: 65536
[  384.095769] xfs_file_buffered_aio_read, nr: 1, 1074413040, , pos:65536, user_datap:0000000000f24000, gfar: 65536
[  384.112849] xfs_file_buffered_aio_read, nr: 1, 1074413040,  , pos:131072, user_datap:0000000000f24000, gfar: 65536
[  384.122037] xfs_file_buffered_aio_read, nr: 1, 1074413040, 0, pos:196608, user_datap:0000000000f24000, gfar: 65536
[  384.129043] xfs_file_buffered_aio_read, nr: 1, 1074413040, @, pos:262144, user_datap:0000000000f24000, gfar: 65536
[  384.135829] xfs_file_buffered_aio_read, nr: 1, 1074413040, P, pos:327680, user_datap:0000000000f24000, gfar: 65536
[  384.140715] xfs_file_buffered_aio_read, nr: 1, 1074413040, `, pos:393216, user_datap:0000000000f24000, gfar: 65536
[  384.146844] xfs_file_buffered_aio_read, nr: 1, 1074413040, p, pos:458752, user_datap:0000000000f24000, gfar: 65536
[  384.153424] xfs_file_buffered_aio_read, nr: 1, 1074413040, \xffffff80, pos:524288, user_datap:0000000000f24000, gfar: 65536
[  384.158720] xfs_file_buffered_aio_read, nr: 1, 1074413040, \xffffff90, pos:589824, user_datap:0000000000f24000, gfar: 65536
[  389.294513] xfs_file_buffered_aio_read, nr: 1, 1074413040, \xffffffa0, pos:655360, user_datap:0000000000f24000, gfar: 61440
[  389.300128] xfs_file_buffered_aio_read, nr: 1, 1074413040, \xffffffa0, pos:716800, user_datap:0000000000f24000, gfar: 0
每次读最大65536字节(64k)，如果再大会分多次读入。

6.大文件的写（32线程, 4k包, vdb)
[  367.999323] xfs_file_buffered_aio_write, nr: 1, count: 4096, iovlen: 4096, fn:vdb_f0689.file, ino:1076278206, pos:12288
[  367.999331] xfs_file_buffered_aio_write, nr: 1, count: 4096, iovlen: 4096, fn:vdb_f0689.file, ino:1076278206, pos:57344
[  367.999338] xfs_file_buffered_aio_write, nr: 1, count: 4096, iovlen: 4096, fn:vdb_f0689.file, ino:1076278206, pos:45056
[  367.999506] xfs_file_buffered_aio_write, nr: 1, count: 4096, iovlen: 4096, fn:vdb_f0689.file, ino:1076278206, pos:69632
[  367.999523] xfs_file_buffered_aio_write, nr: 1, count: 4096, iovlen: 4096, fn:vdb_f0689.file, ino:1076278206, pos:20480
[  367.999539] xfs_file_buffered_aio_write, nr: 1, count: 4096, iovlen: 4096, fn:vdb_f0689.file, ino:1076278206, pos:253952
[  367.999554] xfs_file_buffered_aio_write, nr: 1, count: 4096, iovlen: 4096, fn:vdb_f0689.file, ino:1076278206, pos:233472
[  367.999569] xfs_file_buffered_aio_write, nr: 1, count: 4096, iovlen: 4096, fn:vdb_f0689.file, ino:1076278206, pos:57344
[  367.999582] xfs_file_buffered_aio_write, nr: 1, count: 4096, iovlen: 4096, fn:vdb_f0689.file, ino:1076278206, pos:262144
[  367.999595] xfs_file_buffered_aio_write, nr: 1, count: 4096, iovlen: 4096, fn:vdb_f0689.file, ino:1076278206, pos:8192
[  367.999854] xfs_file_buffered_aio_write, nr: 1, count: 4096, iovlen: 4096, fn:vdb_f0689.file, ino:1076278206, pos:57344
[  367.999867] xfs_file_buffered_aio_write, nr: 1, count: 4096, iovlen: 4096, fn:vdb_f0689.file, ino:1076278206, pos:237568
[  367.999878] xfs_file_buffered_aio_write, nr: 1, count: 4096, iovlen: 4096, fn:vdb_f0689.file, ino:1076278206, pos:266240
[  367.999887] xfs_file_buffered_aio_write, nr: 1, count: 4096, iovlen: 4096, fn:vdb_f0689.file, ino:1076278206, pos:73728
[  367.999896] xfs_file_buffered_aio_write, nr: 1, count: 4096, iovlen: 4096, fn:vdb_f0689.file, ino:1076278206, pos:225280
[  367.999906] xfs_file_buffered_aio_write, nr: 1, count: 4096, iovlen: 4096, fn:vdb_f0689.file, ino:1076278206, pos:28672
[  367.999914] xfs_file_buffered_aio_write, nr: 1, count: 4096, iovlen: 4096, fn:vdb_f0689.file, ino:1076278206, pos:16384
[  367.999922] xfs_file_buffered_aio_write, nr: 1, count: 4096, iovlen: 4096, fn:vdb_f0689.file, ino:1076278206, pos:151552
[  367.999929] xfs_file_buffered_aio_write, nr: 1, count: 4096, iovlen: 4096, fn:vdb_f0689.file, ino:1076278206, pos:77824
[  367.999936] xfs_file_buffered_aio_write, nr: 1, count: 4096, iovlen: 4096, fn:vdb_f0689.file, ino:1076278206, pos:212992
[  367.999994] xfs_file_buffered_aio_write, nr: 1, count: 4096, iovlen: 4096, fn:vdb_f0689.file, ino:1076278206, pos:45056
[  368.000006] xfs_file_buffered_aio_write, nr: 1, count: 4096, iovlen: 4096, fn:vdb_f0689.file, ino:1076278206, pos:40960
[  368.000017] xfs_file_buffered_aio_write, nr: 1, count: 4096, iovlen: 4096, fn:vdb_f0689.file, ino:1076278206, pos:196608
[  368.000028] xfs_file_buffered_aio_write, nr: 1, count: 4096, iovlen: 4096, fn:vdb_f0689.file, ino:1076278206, pos:208896
[  368.000034] xfs_file_buffered_aio_write, nr: 1, count: 4096, iovlen: 4096, fn:vdb_f0689.file, ino:1076278206, pos:32768
[  368.000042] xfs_file_buffered_aio_write, nr: 1, count: 4096, iovlen: 4096, fn:vdb_f0689.file, ino:1076278206, pos:212992
[  368.000050] xfs_file_buffered_aio_write, nr: 1, count: 4096, iovlen: 4096, fn:vdb_f0689.file, ino:1076278206, pos:49152
[  368.000126] xfs_file_buffered_aio_write, nr: 1, count: 4096, iovlen: 4096, fn:vdb_f0689.file, ino:1076278206, pos:57344
[  368.000136] xfs_file_buffered_aio_write, nr: 1, count: 4096, iovlen: 4096, fn:vdb_f0689.file, ino:1076278206, pos:184320
[  368.000146] xfs_file_buffered_aio_write, nr: 1, count: 4096, iovlen: 4096, fn:vdb_f0689.file, ino:1076278206, pos:53248
[  368.000153] xfs_file_buffered_aio_write, nr: 1, count: 4096, iovlen: 4096, fn:vdb_f0689.file, ino:1076278206, pos:184320
[  368.000232] xfs_file_buffered_aio_write, nr: 1, count: 4096, iovlen: 4096, fn:vdb_f0689.file, ino:1076278206, pos:270336
[  368.000346] xfs_file_buffered_aio_write, nr: 1, count: 4096, iovlen: 4096, fn:vdb_f0689.file, ino:1076278206, pos:151552
[  368.000358] xfs_file_buffered_aio_write, nr: 1, count: 4096, iovlen: 4096, fn:vdb_f0689.file, ino:1076278206, pos:98304
[  368.000369] xfs_file_buffered_aio_write, nr: 1, count: 4096, iovlen: 4096, fn:vdb_f0689.file, ino:1076278206, pos:0
[  368.000379] xfs_file_buffered_aio_write, nr: 1, count: 4096, iovlen: 4096, fn:vdb_f0689.file, ino:1076278206, pos:53248
[  368.000390] xfs_file_buffered_aio_write, nr: 1, count: 4096, iovlen: 4096, fn:vdb_f0689.file, ino:1076278206, pos:131072
[  368.000399] xfs_file_buffered_aio_write, nr: 1, count: 4096, iovlen: 4096, fn:vdb_f0689.file, ino:1076278206, pos:110592
[  368.000415] xfs_file_buffered_aio_write, nr: 1, count: 4096, iovlen: 4096, fn:vdb_f0689.file, ino:1076278206, pos:57344
[  368.000423] xfs_file_buffered_aio_write, nr: 1, count: 4096, iovlen: 4096, fn:vdb_f0689.file, ino:1076278206, pos:208896
[  368.000430] xfs_file_buffered_aio_write, nr: 1, count: 4096, iovlen: 4096, fn:vdb_f0689.file, ino:1076278206, pos:184320
[  368.000436] xfs_file_buffered_aio_write, nr: 1, count: 4096, iovlen: 4096, fn:vdb_f0689.file, ino:1076278206, pos:110592
[  368.000442] xfs_file_buffered_aio_write, nr: 1, count: 4096, iovlen: 4096, fn:vdb_f0689.file, ino:1076278206, pos:110592
[  368.000449] xfs_file_buffered_aio_write, nr: 1, count: 4096, iovlen: 4096, fn:vdb_f0689.file, ino:1076278206, pos:118784
[  368.000456] xfs_file_buffered_aio_write, nr: 1, count: 4096, iovlen: 4096, fn:vdb_f0689.file, ino:1076278206, pos:282624

7.dd写大文件
 dd if=/dev/zero of=dd128k.text bs=128k count=10
 [ 4490.309426] xfs_file_buffered_aio_write, nr: 1, count: 131072, iovlen: 131072, fn:dd128k.text, ino:2386488, pos:0
 [ 4490.310005] xfs_file_buffered_aio_write, nr: 1, count: 131072, iovlen: 131072, fn:dd128k.text, ino:2386488, pos:131072
 [ 4490.310577] xfs_file_buffered_aio_write, nr: 1, count: 131072, iovlen: 131072, fn:dd128k.text, ino:2386488, pos:262144
 [ 4490.311160] xfs_file_buffered_aio_write, nr: 1, count: 131072, iovlen: 131072, fn:dd128k.text, ino:2386488, pos:393216
 [ 4490.311703] xfs_file_buffered_aio_write, nr: 1, count: 131072, iovlen: 131072, fn:dd128k.text, ino:2386488, pos:524288
 [ 4490.312258] xfs_file_buffered_aio_write, nr: 1, count: 131072, iovlen: 131072, fn:dd128k.text, ino:2386488, pos:655360
 [ 4490.312770] xfs_file_buffered_aio_write, nr: 1, count: 131072, iovlen: 131072, fn:dd128k.text, ino:2386488, pos:786432
 [ 4490.312867] xfs_file_buffered_aio_write, nr: 1, count: 131072, iovlen: 131072, fn:dd128k.text, ino:2386488, pos:917504
 [ 4490.312964] xfs_file_buffered_aio_write, nr: 1, count: 131072, iovlen: 131072, fn:dd128k.text, ino:2386488, pos:1048576
 [ 4490.313060] xfs_file_buffered_aio_write, nr: 1, count: 131072, iovlen: 131072, fn:dd128k.text, ino:2386488, pos:1179648

 8.dd写大文件
 dd if=/dev/zero of=dd4096k.text bs=4096k count=10

 41943040字节(42 MB)已复制，0.0383124 秒，1.1 GB/秒
 root@localhost /h/a/appraise# dmesg -c
 [ 4734.536000] xfs_file_buffered_aio_write, nr: 1, count: 4194304, iovlen: 4194304, fn:dd4096k.text, ino:2436799, pos:0
 [ 4734.540223] xfs_file_buffered_aio_write, nr: 1, count: 4194304, iovlen: 4194304, fn:dd4096k.text, ino:2436799, pos:4194304
 [ 4734.543702] xfs_file_buffered_aio_write, nr: 1, count: 4194304, iovlen: 4194304, fn:dd4096k.text, ino:2436799, pos:8388608
 [ 4734.547177] xfs_file_buffered_aio_write, nr: 1, count: 4194304, iovlen: 4194304, fn:dd4096k.text, ino:2436799, pos:12582912
 [ 4734.550629] xfs_file_buffered_aio_write, nr: 1, count: 4194304, iovlen: 4194304, fn:dd4096k.text, ino:2436799, pos:16777216
 [ 4734.554115] xfs_file_buffered_aio_write, nr: 1, count: 4194304, iovlen: 4194304, fn:dd4096k.text, ino:2436799, pos:20971520
 [ 4734.557555] xfs_file_buffered_aio_write, nr: 1, count: 4194304, iovlen: 4194304, fn:dd4096k.text, ino:2436799, pos:25165824
 [ 4734.561010] xfs_file_buffered_aio_write, nr: 1, count: 4194304, iovlen: 4194304, fn:dd4096k.text, ino:2436799, pos:29360128
 [ 4734.564461] xfs_file_buffered_aio_write, nr: 1, count: 4194304, iovlen: 4194304, fn:dd4096k.text, ino:2436799, pos:33554432
 [ 4734.568007] xfs_file_buffered_aio_write, nr: 1, count: 4194304, iovlen: 4194304, fn:dd4096k.text, ino:2436799, pos:37748736
