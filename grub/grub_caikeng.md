### 不识别机械磁盘
首先查的BIOS，发现BIOS中系统基本信息里对应的SATA磁盘的驱动都未选中, 选中后保存并重启，再次进入到BIOS，发现已经可以在系统基本信息里查看到磁盘的信息。

接下来重启电脑，发现不能进操作系统了，只能到grub命令行界面，推测是因为增了新磁盘（原来在安装操作系统时，此SATA磁盘未如上生效）导致原来的grub引导配置中的磁盘号错误。

在grub命令行下做如下四步操作
1. 绑定root磁盘,执行命令: root (hd1,1)         注释：这里的参数hd1, 1可以在输入完(hd1,后按tab键来选择，如果其中有包含linux文件系统类型就选它，如xfs, extn等。
   注：上述命令执行成功后，会有相应的返回

2. 设置vmlinux, 执行命令: kernel /vmlinux-2.6.32 ro root=/dev/mapper/vg_arth-lv_root
   注：vmlinux的全名不记得了，可以使用tab补全

3. 设置initramfs, 执行命令: initrd /initramfs-2.6.32.img

4. 执行boot

如果一切正常，则会进入操作系统

到这还没完，需要将grub.conf中的配置更新成正确值，以后就不用每次在操作系统启动时在grub命令行输入上述一大堆东西了
在grub.conf中将对应的(hdx,1)改成上面成功进入系统时使用的值，如(hd1,1)就可以了，保存退出。这样下次启动系统时可以正常进入系统了。

注意！！！
这里有一个问题，就是如果不知道设置vmlinuz时root的根目录/dev/mapper/vg_arth-lv_root的话，目前还无解决办法，这个值不正确的话是无法正常引导系统的。
