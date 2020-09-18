# Linux内核基础知识

### 常用编译工具
#### GCC
#### ARM GCC

GCC具有良好的可扩展性，除了可以编译x86体系结构的二进制程序外，还可以支持很多其他体系结构的处理器，如ARM、MIPS等。

* 本地编译：在当前目标平台编译出来的程序，并且可以在当前平台上运行。一般指的就是在本机上编译的x86程序。

* 交叉编译：因为嵌入式系统资源有限，不能编译程序，所以需要在性能较好的PC上编译出ARM（或其他体系）上可运行的二进制程序，之后烧录到ARM系统上运行。

* 交叉工具链命令规则：
体系结构+操作系统+gnu+嵌入式应用二进制接口，比如：arm-linux-gnueabi、arm-linux-gnueabihf，aarch-linux-gnueabi，例中的arm表示ARM32体系结构，而aarch表示ARM64体系结构，操作系统都是linux，嵌入式接口为eabi、eabihf。


### GNU C
|标题|说明|补充|
|:-|:-|:-|
|零长数组|char a[];|char a[0];|
|case范围|case low...high:|case 'A'...'Z':|
|标号元素|
|可变参数宏|
|函数属性|void \_\_attribute\_\_((noreturn)) somefunc();|该函数从不返回值;函数属性定义在compiler-gcc.h中|
|函数属性|static \_\_attribute\_const\_\_ somefunc();|该函数只计算一次，以后再次调用全部返回第一次计算结果|
|变量属性|
|类型属性|alignment、packed、sections||
|内建函数|\_builtin_constant_p(x)、\_builtin_expect(exp,c)、\_builtin_prefetch(const void \*addr, int rw, int locality)|优化时使用|
|1UL|表示无符号长整型数字1|UL即unsigned long|
### 常用数据结构
|标题|说明|补充|
|:-|:-|:-|
|链表|
|红黑树|
|KFIFO|无锁环形缓冲区|

### 文本编辑工具
