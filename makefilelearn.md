# Linux内核Makefile配置学习记录
## make 和 Makefile
linux内核源代码的编译也是使用make工具和makefile，但是它在普通的C程序编译的基础上对配置和编译选项进行了扩展，这就是kbuild系统，专门针对linux的内核编译，使得linux内核的编译更加简洁而高效。

在linux中，由于内核代码的分层模型，以及兼容很多平台的特性，makefile文件分布在各个目录中，对每个模块进行分离编译，降低耦合性，使编译方式更加灵活。
#####  makefile主要是以下五个部分：
* 顶层makefile : 在源代码的根目录有个顶层makefile，顶层makefile的作用就是负责生成两个最重要的部分：编译生成vmlinux和各种模块。

* .config文件 : 这个config文件主要是产生自用户对内核模块的配置，有三种配置方式：
```
    编译进内核
    编译成可加载模块
    不进行编译。
```

* arch/$(ARCH)/Makefile : 从目录可以看出，这个makefile主要是根据指定的平台对内核镜像进行相应的配置,提供平台信息给顶层makefile。

* scirpts/makefile. : *这些makefile配置文件包含了构建内核的规则。

* kbuild makefiles : 每一个模块都是单独被编译然后再链接的，所以这一种kbiuld makefile几乎在每个模块中都存在.在这些模块文件(子目录)中，也可以使用Kbuild文件代替Makefile，当两者同时存在时，优先选择Kbuild文件进行编译工作，只是用户习惯性地使用Makefile来命名。

## kbuild系统(kbuild是内核编译器)

#### kbuild中的变量
##### 顶层makefile中定义了以下变量：
* KERNELRELEASE

这是一个字符串，用于构建安装目录的名字(一般使用版本号来区分)或者显示当前的版本号。
* ARCH

定义当前的目标架构平台，比如:"X86"，"ARM",默认情况下，ARCH的值为当前编译的主机架构,但是在交叉编译环境中，需要在顶层makefile或者是命令行中指定架构：

>make ARCH=arm ...

* INSTALL_PATH

指定安装目录，安装目录主要是为了放置需要安装的镜像和map(符号表)文件，系统的启动需要这些文件的参与。
* INSTALL_MOD_PATH, MODLIB

INSTALL_MOD_PATH：为模块指定安装的前缀目录，这个变量在顶层makefile中并没有被定义，用户可以使用，MODLIB为模块指定安装目录.

默认情况下，模块会被安装到$(INSTALL_MOD_PATH)/lib/modules/$(KERNELRELEASE)中，默认INSTALL_MOD_PATH不会被指定，所以会被安装到/lib/modules/$(KERNELRELEASE)中。
* INSTALL_MOD_STRIP

如果这个变量被指定，模块就会将一些额外的、运行时非必要的信息剥离出来以缩减模块的大小，当INSTALL_MOD_STRIP为1时，--strip-debug选项就会被使用，模块的调试信息将被删除，否则就执行默认的参数，模块编译时会添加一些辅助信息。

这些全局变量一旦在顶层makefile中被定义就全局有效，但是有一点需要注意，在驱动开发时，一般编译单一的模块，执行make调用的是当前目录下的Makefile.

在这种情况下这些变量是没有被定义的，只有先调用了顶层makefile之后，这些变量在子目录中的makefile才被赋值。

ERSION = 4
PATCHLEVEL = 10
SUBLEVEL = 15
EXTRAVERSION =
NAME = Fearless Coyote
