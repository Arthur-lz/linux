# possible deadlock in ext4_xattr_set_handle

### Crashes (1): 

|Manager|Time|Kernel|Commit|Syzkaller|Config|Log|Report|Syz repro|C repro|VM info|Title|
|:-|:-|:-|:-|:-|:-|:-|:-|:-|:-|:-|:-|
|ci-qemu-upstream|2021/10/18 19:59|upstream|519d81956ee2|24dc29db|.config|log|report|||info|possible deadlock in ext4_xattr_set_handle|

#### bug addr
> https://syzkaller.appspot.com/bug?id=69d6141f8a6c739fc77df31811aadf532b20eaea

##### Status: upstream: reported on 2021/10/22 20:02
##### Reported-by: syzbot+151cb6793a95b59e826d@syzkaller.appspotmail.com
##### First crash: 18d, last: 18d

##### Sample crash report:
```
======================================================
WARNING: possible circular locking dependency detected
5.15.0-rc6-syzkaller #0 Not tainted
------------------------------------------------------
syz-executor.2/20558 is trying to acquire lock:
ffffffff8ba9ef80 (fs_reclaim){+.+.}-{0:0}, at: might_alloc include/linux/sched/mm.h:198 [inline]
ffffffff8ba9ef80 (fs_reclaim){+.+.}-{0:0}, at: slab_pre_alloc_hook mm/slab.h:492 [inline]
ffffffff8ba9ef80 (fs_reclaim){+.+.}-{0:0}, at: slab_alloc_node mm/slab.c:3222 [inline]
ffffffff8ba9ef80 (fs_reclaim){+.+.}-{0:0}, at: kmem_cache_alloc_node_trace+0x4a/0x5d0 mm/slab.c:3617

kmem_cache_alloc_node_trace
	|----->slab_alloc_node(cachep, flags, nodeid, size, _RET_IP_)
			|------>slab_pre_alloc_hook(cachep, &objcg, 1, gfpflags);		/* 这里就比较狠了，不与具体的inode相关 */
				|----->might_alloc
					|----->fs_reclaim_acquire
						|----->__fs_reclaim_acquire(_RET_IP_)
							|----->lock_acquire_exclusive(&__fs_reclaim_map, 0, 0, NULL, ip); 
								|----->lock_acquire(l, s, t, 0, 1, n, i)
									|----->__lock_acquire(lock, subclass, trylock, read, 	/* 到这里可以明确，此锁与进程对应 */
											      check, irqs_disabled_flags(flags), 
											      nest_lock, ip, 0, 0) 

/* static struct lockdep_map __fs_reclaim_map = STATIC_LOCKDEP_MAP_INIT("fs_reclaim", &__fs_reclaim_map);
 */
	
but task is already holding lock:
ffff8880241e7488 (&ei->xattr_sem){++++}-{3:3}, at: ext4_write_lock_xattr fs/ext4/xattr.h:142 [inline]
ffff8880241e7488 (&ei->xattr_sem){++++}-{3:3}, at: ext4_xattr_set_handle+0x15c/0x1500 fs/ext4/xattr.c:2294

ext4_xattr_set_handle
	|----->ext4_write_lock_xattr
		|----->down_write(&EXT4_I(inode)->xattr_sem)				/* struct rw_semaphore xattr_sem; */

which lock already depends on the new lock.						/* 新锁指的是fs_reclaim? 如果是，则xattr_sem依赖于fs_reclaim? */


the existing dependency chain (in reverse order) is:					/* 锁依赖链（倒序）*/

-> #3 (&ei->xattr_sem){++++}-{3:3}:
       down_write+0x92/0x150 kernel/locking/rwsem.c:1517				// rwsem_acquire(&sem->dep_map, 0, 0, _RET_IP_)
       ext4_write_lock_xattr fs/ext4/xattr.h:142 [inline]				// down_write(&EXT4_I(inode)->xattr_sem)
       ext4_xattr_set_handle+0x15c/0x1500 fs/ext4/xattr.c:2294				// ext4_write_lock_xattr(inode, &no_expand)
       ext4_initxattrs+0xb5/0x120 fs/ext4/xattr_security.c:44				// ext4_xattr_set_handle(handle, inode, 
       security_inode_init_security+0x1c4/0x370 security/security.c:1099		// initxattrs(inode, new_xattrs, fs_data)
       __ext4_new_inode+0x472b/0x5ba0 fs/ext4/ialloc.c:1325				// ext4_init_security(handle, inode, dir, qstr)
       ext4_create+0x2d6/0x4d0 fs/ext4/namei.c:2746					// ext4_new_inode_start_handle(mnt_userns, dir, mode, &dentry->d_name,
       lookup_open.isra.0+0xfe4/0x13d0 fs/namei.c:3282					// dir_inode->i_op->create(mnt_userns, dir_inode, dentry,
       open_last_lookups fs/namei.c:3352 [inline]					// lookup_open(nd, file, op, got_write)
       path_openat+0x9a5/0x2740 fs/namei.c:3558						// open_last_lookups(nd, file, op)
       do_filp_open+0x1aa/0x400 fs/namei.c:3588						// path_openat(&nd, op, flags | LOOKUP_RCU)
       do_sys_openat2+0x16d/0x4d0 fs/open.c:1200					// struct file *f = do_filp_open(dfd, tmp, &op)
       do_sys_open fs/open.c:1216 [inline]						// do_sys_openat2(dfd, filename, &how)
       __do_sys_openat fs/open.c:1232 [inline]						// do_sys_open(dfd, filename, flags, mode)
       __se_sys_openat fs/open.c:1227 [inline]						// SYSCALL_DEFINE4(openat, int, dfd, const char __user *, filename, int, flags,		// open for write
       __x64_sys_openat+0x13f/0x1f0 fs/open.c:1227
       do_syscall_x64 arch/x86/entry/common.c:50 [inline]				// sys_call_table[unr](regs)
       do_syscall_64+0x35/0xb0 arch/x86/entry/common.c:80				// do_syscall_x64(regs, nr)
       entry_SYSCALL_64_after_hwframe+0x44/0xae

-> #2 (jbd2_handle){++++}-{0:0}:
       start_this_handle+0xfb7/0x1390 fs/jbd2/transaction.c:468				// rwsem_acquire_read(&journal->j_trans_commit_map, 0, 0, _THIS_IP_);
       jbd2__journal_start+0x399/0x930 fs/jbd2/transaction.c:525			// start_this_handle(journal, handle, gfp_mask)
       __ext4_journal_start_sb+0x227/0x4a0 fs/ext4/ext4_jbd2.c:105			// jbd2__journal_start(journal, blocks, rsv_blocks, revoke_creds, GFP_NOFS, type, line);
       ext4_sample_last_mounted fs/ext4/file.c:821 [inline]				// ext4_journal_start_sb(sb, EXT4_HT_MISC, 1); 
       ext4_file_open+0x5f3/0xb60 fs/ext4/file.c:850					// ext4_sample_last_mounted(inode->i_sb, filp->f_path.mnt)
       do_dentry_open+0x4c8/0x11d0 fs/open.c:822					// open(inode, f)
       do_open fs/namei.c:3428 [inline]							// vfs_open(&nd->path, file)
       path_openat+0x1c9a/0x2740 fs/namei.c:3561					// do_open(nd, file, op)
       do_filp_open+0x1aa/0x400 fs/namei.c:3588						// path_openat(&nd, op, flags | LOOKUP_RCU)
       do_sys_openat2+0x16d/0x4d0 fs/open.c:1200					// struct file *f = do_filp_open(dfd, tmp, &op)
       do_sys_open fs/open.c:1216 [inline]						// do_sys_openat2(dfd, filename, &how)
       __do_sys_openat fs/open.c:1232 [inline]						// do_sys_open(dfd, filename, flags, mode)
       __se_sys_openat fs/open.c:1227 [inline]						// SYSCALL_DEFINE4(openat, int, dfd, const char __user *, filename, int, flags,		// open for read
       __x64_sys_openat+0x13f/0x1f0 fs/open.c:1227
       do_syscall_x64 arch/x86/entry/common.c:50 [inline]				// sys_call_table[unr](regs)
       do_syscall_64+0x35/0xb0 arch/x86/entry/common.c:80				// do_syscall_x64(regs, nr)
       entry_SYSCALL_64_after_hwframe+0x44/0xae

-> #1 (sb_internal){.+.+}-{0:0}:
       percpu_down_read include/linux/percpu-rwsem.h:51 [inline]			// rwsem_acquire_read(&sem->dep_map, 0, 0, _RET_IP_);
       __sb_start_write include/linux/fs.h:1812 [inline]
       sb_start_intwrite include/linux/fs.h:1929 [inline]
       ext4_evict_inode+0xe78/0x1950 fs/ext4/inode.c:241
       evict+0x2ed/0x6b0 fs/inode.c:588
       iput_final fs/inode.c:1664 [inline]
       iput.part.0+0x539/0x850 fs/inode.c:1690
       iput+0x58/0x70 fs/inode.c:1680
       dentry_unlink_inode+0x2b1/0x460 fs/dcache.c:376
       __dentry_kill+0x3c0/0x640 fs/dcache.c:582
       dentry_kill fs/dcache.c:720 [inline]
       dput+0x66b/0xbc0 fs/dcache.c:888
       ovl_entry_stack_free fs/overlayfs/super.c:61 [inline]
       ovl_dentry_release+0xca/0x130 fs/overlayfs/super.c:74
       __dentry_kill+0x42b/0x640 fs/dcache.c:587
       shrink_dentry_list+0x128/0x490 fs/dcache.c:1176
       prune_dcache_sb+0xe7/0x140 fs/dcache.c:1257
       super_cache_scan+0x336/0x590 fs/super.c:105
       do_shrink_slab+0x42d/0xbd0 mm/vmscan.c:758
       shrink_slab_memcg mm/vmscan.c:827 [inline]
       shrink_slab+0x3e4/0x6e0 mm/vmscan.c:906
       shrink_node_memcgs mm/vmscan.c:3018 [inline]
       shrink_node+0x8c1/0x1eb0 mm/vmscan.c:3139
       shrink_zones mm/vmscan.c:3342 [inline]
       do_try_to_free_pages+0x386/0x1480 mm/vmscan.c:3397
       try_to_free_pages+0x29f/0x750 mm/vmscan.c:3632
       __perform_reclaim mm/page_alloc.c:4592 [inline]
       __alloc_pages_direct_reclaim mm/page_alloc.c:4613 [inline]
       __alloc_pages_slowpath.constprop.0+0x828/0x21b0 mm/page_alloc.c:5017
       __alloc_pages+0x412/0x500 mm/page_alloc.c:5388
       alloc_migration_target+0x4e8/0x7e0 mm/migrate.c:1640
       unmap_and_move mm/migrate.c:1207 [inline]
       migrate_pages+0x8aa/0x39e0 mm/migrate.c:1488
       do_move_pages_to_node mm/migrate.c:1670 [inline]
       move_pages_and_store_status.isra.0+0xf4/0x230 mm/migrate.c:1760
       do_pages_move mm/migrate.c:1856 [inline]
       kernel_move_pages+0x9d4/0x1580 mm/migrate.c:2033				
       __do_sys_move_pages mm/migrate.c:2047 [inline]
       __se_sys_move_pages mm/migrate.c:2042 [inline]
       __x64_sys_move_pages+0xdd/0x1b0 mm/migrate.c:2042			// SYSCALL_DEFINE6(move_pages, pid_t, pid, unsigned long, nr_pages,	// 做页迁移
       do_syscall_x64 arch/x86/entry/common.c:50 [inline]			// sys_call_table[unr](regs)
       do_syscall_64+0x35/0xb0 arch/x86/entry/common.c:80			// do_syscall_x64(regs, nr)
       entry_SYSCALL_64_after_hwframe+0x44/0xae

-> #0 (fs_reclaim){+.+.}-{0:0}:							/* 第一把锁被加入 */
       check_prev_add kernel/locking/lockdep.c:3051 [inline]			// ret = check_noncircular(next, prev, trace); 
       check_prevs_add kernel/locking/lockdep.c:3174 [inline]			// check_prev_add(curr, hlock, next, distance, &trace)
       validate_chain kernel/locking/lockdep.c:3789 [inline]			// if (!check_prevs_add(curr, hlock))
       __lock_acquire+0x2a07/0x54a0 kernel/locking/lockdep.c:5015		// if (!validate_chain(curr, hlock, chain_head, chain_key))
       lock_acquire kernel/locking/lockdep.c:5625 [inline]			// __lock_acquire(lock, subclass, trylock, read, check,
       lock_acquire+0x1ab/0x510 kernel/locking/lockdep.c:5590
       __fs_reclaim_acquire mm/page_alloc.c:4539 [inline]			// lock_acquire_exclusive(&__fs_reclaim_map, 0, 0, NULL, ip)
       fs_reclaim_acquire+0x115/0x160 mm/page_alloc.c:4553			// __fs_reclaim_acquire(_RET_IP_)
       might_alloc include/linux/sched/mm.h:198 [inline]			// fs_reclaim_acquire(gfp_mask)
       slab_pre_alloc_hook mm/slab.h:492 [inline]				// might_alloc(flags)
       slab_alloc_node mm/slab.c:3222 [inline]					// cachep = slab_pre_alloc_hook(cachep, &objcg, 1, flags);
       kmem_cache_alloc_node_trace+0x4a/0x5d0 mm/slab.c:3617			// slab_alloc_node(cachep, flags, nodeid, size, _RET_IP_)
       __do_kmalloc_node mm/slab.c:3639 [inline]				// kmem_cache_alloc_node_trace(cachep, flags, node, size)
       __kmalloc_node+0x38/0x60 mm/slab.c:3647					// __do_kmalloc_node(size, flags, node, _RET_IP_)
       kmalloc_node include/linux/slab.h:614 [inline]				// __kmalloc_node(size, flags, node)
       kvmalloc_node+0xb4/0x120 mm/util.c:587					// kmalloc_node(size, kmalloc_flags, node)
       kvmalloc include/linux/mm.h:805 [inline]					// kvmalloc_node(size, flags, NUMA_NO_NODE)
       ext4_xattr_inode_cache_find fs/ext4/xattr.c:1472 [inline]		// kvmalloc(value_len, GFP_KERNEL);
       ext4_xattr_inode_lookup_create fs/ext4/xattr.c:1515 [inline]		// ext4_xattr_inode_cache_find(inode, value, value_len, hash)
       ext4_xattr_set_entry+0x1d94/0x3890 fs/ext4/xattr.c:1656
       ext4_xattr_ibody_set+0x78/0x2b0 fs/ext4/xattr.c:2210
       ext4_xattr_set_handle+0x964/0x1500 fs/ext4/xattr.c:2367
       ext4_xattr_set+0x13a/0x340 fs/ext4/xattr.c:2480
       __vfs_setxattr+0x115/0x180 fs/xattr.c:180
       __vfs_setxattr_noperm+0x125/0x5e0 fs/xattr.c:214
       __vfs_setxattr_locked+0x1cf/0x260 fs/xattr.c:275
       vfs_setxattr+0x13f/0x330 fs/xattr.c:301
       setxattr+0x218/0x2b0 fs/xattr.c:575
       path_setxattr+0x197/0x1c0 fs/xattr.c:595
       __do_sys_setxattr fs/xattr.c:611 [inline]
       __se_sys_setxattr fs/xattr.c:607 [inline]
       __x64_sys_setxattr+0xc0/0x160 fs/xattr.c:607				// SYSCALL_DEFINE5(setxattr, const char __user *, pathname, 		// 设置文件扩展属性
       do_syscall_x64 arch/x86/entry/common.c:50 [inline]			// sys_call_table[unr](regs)
       do_syscall_64+0x35/0xb0 arch/x86/entry/common.c:80			// do_syscall_x64(regs, nr)
       entry_SYSCALL_64_after_hwframe+0x44/0xae

#define _RET_IP_ __builtin_return_anddress(0)

  Since a return probe is implemented by replacing the return
  address with the trampoline's address, stack backtraces and calls
  to __builtin_return_address() will typically yield the trampoline's
  address instead of the real return address for kretprobed functions.
  (As far as we can tell, __builtin_return_address() is used only
  for instrumentation and error reporting.

  由于返回探测是通过将返回地址替换为trampoline的地址来实现的，
  所以堆栈回溯和调用__buildin_return address()通常会产生trampoline的地址，
  而不是kretprobes函数的实际返回地址。(据我们所知，__buildin_return_address()仅用于检测和错误报告

  内建函数 __builtin_return_address(LEVEL)返回当前函数或其调用者的返回地址，
  参数LEVEL指定在栈上搜索框架的个数，0表示当前函数的返回地址，1表示当前函数的调用者的返回地址，依此类推。


other info that might help us debug this:

Chain exists of:
  fs_reclaim --> jbd2_handle --> &ei->xattr_sem			/* 可能造成死锁的三把锁
								 * 结合下面的推测，可以初步确认的是三把锁之间有依赖关系，或者说它们保护的区域是f的最大，j的居中，e的最小
								 * 
								 * 在没有看源码前的分析（源码下载中）
								 * 
								 * CPU0: 持有两把锁e, f
								 * CPU1: 持有一把锁j, 想获取e，但由于锁e被CPU0执有，所以其需要等待CPU0释放e
								 *
								 * 分析一、
								 * 如果三把锁f, j, e存在依赖关系，那么就算没有CPU1，只在一个CPU0上，如果锁e, f是被同一个进程获取，那么同样会死锁
								 * 所以，这三把锁是内核中固定的锁链，所以CPU0上的e, f是被两个进程执有或申请
								 *
								 * 分析二、
								 *
								 */ 

 Possible unsafe locking scenario:

       CPU0                    CPU1						
       ----                    ----
  lock(&ei->xattr_sem);
                               lock(jbd2_handle);
                               lock(&ei->xattr_sem);
  lock(fs_reclaim);

 *** DEADLOCK ***

3 locks held by syz-executor.2/20558:
 #0: ffff8880772fc460 (sb_writers#6){.+.+}-{0:0}, at: path_setxattr+0xb2/0x1c0 fs/xattr.c:593
 #1: ffff8880241e77b0 (&type->i_mutex_dir_key#4){++++}-{3:3}, at: inode_lock include/linux/fs.h:786 [inline]		// down_write(&inode->i_rwsem);
 #1: ffff8880241e77b0 (&type->i_mutex_dir_key#4){++++}-{3:3}, at: vfs_setxattr+0x11c/0x330 fs/xattr.c:300		// inode_lock(inode);
 #2: ffff8880241e7488 (&ei->xattr_sem){++++}-{3:3}, at: ext4_write_lock_xattr fs/ext4/xattr.h:142 [inline		// down_write(&EXT4_I(inode)->xattr_sem);]
 #2: ffff8880241e7488 (&ei->xattr_sem){++++}-{3:3}, at: ext4_xattr_set_handle+0x15c/0x1500 fs/ext4/xattr.c:2294		// ext4_write_lock_xattr(inode, &no_expand);

stack backtrace:
CPU: 1 PID: 20558 Comm: syz-executor.2 Not tainted 5.15.0-rc6-syzkaller #0
Hardware name: QEMU Standard PC (Q35 + ICH9, 2009), BIOS 1.14.0-2 04/01/2014
Call Trace:
 __dump_stack lib/dump_stack.c:88 [inline]
 dump_stack_lvl+0xcd/0x134 lib/dump_stack.c:106
 check_noncircular+0x25f/0x2e0 kernel/locking/lockdep.c:2131
 check_prev_add kernel/locking/lockdep.c:3051 [inline]
 check_prevs_add kernel/locking/lockdep.c:3174 [inline]
 validate_chain kernel/locking/lockdep.c:3789 [inline]
 __lock_acquire+0x2a07/0x54a0 kernel/locking/lockdep.c:5015
 lock_acquire kernel/locking/lockdep.c:5625 [inline]
 lock_acquire+0x1ab/0x510 kernel/locking/lockdep.c:5590
 __fs_reclaim_acquire mm/page_alloc.c:4539 [inline]
 fs_reclaim_acquire+0x115/0x160 mm/page_alloc.c:4553
 might_alloc include/linux/sched/mm.h:198 [inline]
 slab_pre_alloc_hook mm/slab.h:492 [inline]
 slab_alloc_node mm/slab.c:3222 [inline]
 kmem_cache_alloc_node_trace+0x4a/0x5d0 mm/slab.c:3617
 __do_kmalloc_node mm/slab.c:3639 [inline]
 __kmalloc_node+0x38/0x60 mm/slab.c:3647
 kmalloc_node include/linux/slab.h:614 [inline]
 kvmalloc_node+0xb4/0x120 mm/util.c:587
 kvmalloc include/linux/mm.h:805 [inline]
 ext4_xattr_inode_cache_find fs/ext4/xattr.c:1472 [inline]
 ext4_xattr_inode_lookup_create fs/ext4/xattr.c:1515 [inline]
 ext4_xattr_set_entry+0x1d94/0x3890 fs/ext4/xattr.c:1656
 ext4_xattr_ibody_set+0x78/0x2b0 fs/ext4/xattr.c:2210
 ext4_xattr_set_handle+0x964/0x1500 fs/ext4/xattr.c:2367
 ext4_xattr_set+0x13a/0x340 fs/ext4/xattr.c:2480
 __vfs_setxattr+0x115/0x180 fs/xattr.c:180
 __vfs_setxattr_noperm+0x125/0x5e0 fs/xattr.c:214
 __vfs_setxattr_locked+0x1cf/0x260 fs/xattr.c:275
 vfs_setxattr+0x13f/0x330 fs/xattr.c:301
 setxattr+0x218/0x2b0 fs/xattr.c:575
 path_setxattr+0x197/0x1c0 fs/xattr.c:595
 __do_sys_setxattr fs/xattr.c:611 [inline]
 __se_sys_setxattr fs/xattr.c:607 [inline]
 __x64_sys_setxattr+0xc0/0x160 fs/xattr.c:607
 do_syscall_x64 arch/x86/entry/common.c:50 [inline]
 do_syscall_64+0x35/0xb0 arch/x86/entry/common.c:80
 entry_SYSCALL_64_after_hwframe+0x44/0xae
RIP: 0033:0x7f7d5d80ea39
Code: ff ff c3 66 2e 0f 1f 84 00 00 00 00 00 0f 1f 40 00 48 89 f8 48 89 
f7 48 89 d6 48 89 ca 4d 89 c2 4d 89 c8 4c 8b 4c 24 08 0f 05 <48> 3d 01 
f0 ff ff 73 01 c3 48 c7 c1 bc ff ff ff f7 d8 64 89 01 48
RSP: 002b:00007f7d5ad63188 EFLAGS: 00000246 ORIG_RAX: 00000000000000bc
RAX: ffffffffffffffda RBX: 00007f7d5d912020 RCX: 00007f7d5d80ea39
RDX: 0000000020000380 RSI: 00000000200000c0 RDI: 0000000020000080
RBP: 00007f7d5d868c5f R08: 0000000000000000 R09: 0000000000000000
R10: 000000000000c001 R11: 0000000000000246 R12: 0000000000000000
R13: 00007ffd05b3b40f R14: 00007f7d5ad63300 R15: 0000000000022000

```

## analysis
### 根据splat(这里是crash report)的信息可知
* 1. 进程 20558当前持有锁ei->xattr_sem，它想获取fs_reclaim

* 2. 几把锁的依赖关系
```
fs_reclaim 依赖sb_internal
sb_internal 依赖jbd2_handle
jbd2_handle 依赖ei->xattr_sem
ei->xattr_sem
```

* 3. 到c码中找证据来证明splat提供的几把锁的依赖关系

