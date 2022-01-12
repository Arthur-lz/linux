------------[ cut here ]------------
kernel BUG at arch/arm64/include/asm/pgalloc.h:79!
Internal error: Oops - BUG: 0 [#1] SMP
Modules linked in:
CPU: 1 PID: 17777 Comm: syz-executor.0 Not tainted 5.15.0-syzkaller-12999-gc8c109546a19 #0
Hardware name: linux,dummy-virt (DT)
pstate: 40000005 (nZcv daif -PAN -UAO -TCO -DIT -SSBS BTYPE=--)
pc : pmd_populate_kernel arch/arm64/include/asm/pgalloc.h:79 [inline]
pc : copy_pte arch/arm64/mm/trans_pgd.c:70 [inline]
pc : copy_pmd arch/arm64/mm/trans_pgd.c:105 [inline]
pc : copy_pud arch/arm64/mm/trans_pgd.c:141 [inline]
pc : copy_p4d arch/arm64/mm/trans_pgd.c:167 [inline]
pc : copy_page_tables arch/arm64/mm/trans_pgd.c:186 [inline]
pc : trans_pgd_create_copy+0x5bc/0x7f0 arch/arm64/mm/trans_pgd.c:213
lr : trans_alloc arch/arm64/mm/trans_pgd.c:30 [inline]
lr : copy_pte arch/arm64/mm/trans_pgd.c:67 [inline]
lr : copy_pmd arch/arm64/mm/trans_pgd.c:105 [inline]
lr : copy_pud arch/arm64/mm/trans_pgd.c:141 [inline]
lr : copy_p4d arch/arm64/mm/trans_pgd.c:167 [inline]
lr : copy_page_tables arch/arm64/mm/trans_pgd.c:186 [inline]
lr : trans_pgd_create_copy+0x480/0x7f0 arch/arm64/mm/trans_pgd.c:213
sp : ffff80001b437a60
x29: ffff80001b437a60 x28: 1fffe0000fffee00 x27: ffff800010059c70
x26: ffff00003fffffff x25: ffff0000397d4000 x24: dfff800000000000
x23: ffff000000200000 x22: ffff80001b437c30 x21: ffff000040000000
x20: ffff00007fff7000 x19: ffff000000000000 x18: ffff00006aa2fc08
x17: 0000000000000000 x16: 0000000000000000 x15: d503201fd503201f
x14: 1ffff00003686e32 x13: 0000000000000000 x12: ffff6000072fac00
x11: 1fffe000072fabff x10: ffff6000072fabff x9 : 0000000000000000
x8 : ffff0000397d6000 x7 : 0000000000000000 x6 : 000000000000003f
x5 : 0000000000000040 x4 : 0000000000000000 x3 : 0000000000000004
x2 : fffffc0000000000 x1 : ffff80001608f680 x0 : ffff0000397d5000
Call trace:
 set_p4d arch/arm64/include/asm/pgtable.h:701 [inline]
 __p4d_populate arch/arm64/include/asm/pgalloc.h:46 [inline]
 p4d_populate arch/arm64/include/asm/pgalloc.h:54 [inline]
 copy_pud arch/arm64/mm/trans_pgd.c:129 [inline]
 copy_p4d arch/arm64/mm/trans_pgd.c:167 [inline]
 copy_page_tables arch/arm64/mm/trans_pgd.c:186 [inline]
 trans_pgd_create_copy+0x5bc/0x7f0 arch/arm64/mm/trans_pgd.c:213
 machine_kexec_post_load+0x178/0x6d0 arch/arm64/kernel/machine_kexec.c:146
 do_kexec_load+0x194/0x520 kernel/kexec.c:155
 __do_compat_sys_kexec_load kernel/kexec.c:292 [inline]
 __se_compat_sys_kexec_load kernel/kexec.c:257 [inline]
 __arm64_compat_sys_kexec_load+0x36c/0x47c kernel/kexec.c:257
 __invoke_syscall arch/arm64/kernel/syscall.c:38 [inline]
 invoke_syscall+0x6c/0x260 arch/arm64/kernel/syscall.c:52
 el0_svc_common.constprop.0+0xc4/0x254 arch/arm64/kernel/syscall.c:142
 do_el0_svc_compat+0x40/0x80 arch/arm64/kernel/syscall.c:187
 el0_svc_compat+0x64/0x280 arch/arm64/kernel/entry-common.c:736
 el0t_32_sync_handler+0x90/0x140 arch/arm64/kernel/entry-common.c:746
 el0t_32_sync+0x1a4/0x1a8 arch/arm64/kernel/entry.S:577
Code: 17fffee0 aa1503e0 97ffdd37 17ffff17 (d4210000) 
---[ end trace 1eb9b72fa9edf501 ]---
