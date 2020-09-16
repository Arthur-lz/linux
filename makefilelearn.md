# Linux内核Makefile配置学习记录
#### 学习linux内核Makefile前需要掌握的基础知识
* 首先需要有gcc基础
1. gcc常用编译选项
>-c, -o, -g, -I是什么含义？

>源码中的include \<head.h>与include "head.h"的区别还记得吗

>Linux中默认的头文件都放在了/usr/include/目录

2. gcc常用库选项
>以.a结尾的是**静态库**

>以.so[主版本号.次版本号.发行号]结尾的是动态库

>-static, -share, -lname, -L dir这些都是神马还记得吗

>被引用的库文件可以有以下三种形式注册动态库路径

```
一、修改/etc/ld.so.conf

二、修改LD_LIBRARY_PATH环境变量

三、直接将库文件复制到/lib/或者/usr/lib/目录下
```
---
* 之后需要有makefile基础
>makefile中常用预定义变量

```
CC，c编译器名称(无默认)，如gcc
CXX，c++编译器名称，默认g++
CFLAGS，c编译器选项(无默认值)
```
>makefile中常见自动变量

```
$@目标文件完整名称
$^不重复的依赖文件，以空格分开
$<第一个依赖文件名称
```
>makefile中常用规则

>makefile中可以使用环境变量，用户makefile中自定义的变量可覆盖同名的环境变量

* 最后需要了解make中常用命令行选项

```
-C dir 读入指定目录下的makefile
-f file 读入当前目录下的file作为makefile
-I dir 指定被包含的makefile所在目录
```

---
---

#### Makefile包括五部分
| 内容           | 说明  |补充|
|:--------------------------|:-----|:-----|
|Makefile|顶层Makefile文件||
|.config |内核配置文件 |通过内核配置操作产生.config文件,顶层Makefile文件读取该文件的配置|
|arch/$(ARCH)/Makefile|机器体系结构相关Makefile文件 ||
|scripts/Makefile.\* | 所有内核Makefiles共用规则||
|kbuild Makefiles|其它makefile文件||

>顶层Makefile文件负责产生两个主要的程序:vmlinux (内核image)和模块。顶层Makefile文件根据内核配置，通过递归编译内核代码树子目录建立这两个文件。顶层Makefile文件里包含一个名为 arch/\$(ARCH)/Makefile的架构makefile文件。架构Makefile文件为顶层makefile文件提供与机器相关的信息。

>每一个子目录有一个makefile文件,子目录makefile文件根据上级目录makefile文件命令启动编译。这些makefile使用.config文件配置数据构建各种文件列表,并使用这些文件列表编译内嵌或模块目标文件.scripts/Makefile.\*包含了所有的定义和规则，与makefile文件一起编译出内核程序。

#### 用户与内核makefile存在四种不同的关系
* 用户，用户使用"make menuconfig"或"make"命令编译内核.他们通常不读或编辑内核makefile文件或其他源文件. 

* 普通开发者，普通开发者维护设备驱动程序、文件系统和网络协议代码,他们维护相关子系统的makefile文件,因此他们需要内核makefile文件整体性的一般知识和关于kbuild公共接口的详细知识. 

* 体系开发者，体系开发者关注一个整体的体系架构,比如sparc或者ia64.体系开发者既需要掌握关于体系的makefile文件,也要熟悉内核makefile文件. 

* 内核开发者，内核开发者关注内核编译系统本身.他们需要清楚内核makefile文件的所有方面. 

#### 执行makefile的那些程序
|命令|说明|补充|
|:---------|:---------|:----------|
|ld|连接器|'ld'把一定量的目标文件跟档案文件连接起来,并重定位它们的数据,连接符号引用.一般,在编译一个程序时,最后一步就是运行'ld'. |

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

* scirpts/makefile. : 这些makefile配置文件包含了构建内核的规则。

* kbuild makefiles : 每一个模块都是单独被编译然后再链接的，所以这一种kbiuld makefile几乎在每个模块中都存在.在这些模块文件(子目录)中，也可以使用Kbuild文件代替Makefile，当两者同时存在时，优先选择Kbuild文件进行编译工作，只是用户习惯性地使用Makefile来命名。

## kbuild系统(kbuild是内核编译器)

#### kbuild中的变量
##### 顶层makefile中定义了以下变量：
* KERNELRELEASE

这是一个字符串，用于构建安装目录的名字(一般使用版本号来区分)或者显示当前的版本号。
* ARCH

定义当前的目标架构平台，比如:"X86"，"ARM",默认情况下，ARCH的值为当前编译的主机架构， `但是在交叉编译环境中，需要在顶层makefile或者是命令行中指定架构`：

>make ARCH=arm ...

* INSTALL_PATH

指定安装目录，安装目录主要是为了放置需要安装的镜像和map(符号表)文件，系统的启动需要这些文件的参与。
* INSTALL_MOD_PATH, MODLIB

INSTALL_MOD_PATH：为模块指定安装的前缀目录，这个变量在顶层makefile中并没有被定义，用户可以使用MODLIB为模块指定安装目录.

默认情况下，模块会被安装到\$(INSTALL_MOD_PATH)/lib/modules/\$(KERNELRELEASE)中，默认INSTALL_MOD_PATH不会被指定，所以会被安装到/lib/modules/$(KERNELRELEASE)中。
* INSTALL_MOD_STRIP

如果这个变量被指定，模块就会将一些额外的、运行时非必要的信息剥离出来以缩减模块的大小，当INSTALL_MOD_STRIP为1时，--strip-debug选项就会被使用，模块的调试信息将被删除，否则就执行默认的参数，模块编译时会添加一些辅助信息。

这些全局变量一旦在顶层makefile中被定义就全局有效，但是有一点需要注意，在驱动开发时，一般编译单一的模块，执行make调用的是当前目录下的Makefile。在这种情况下这些变量是没有被定义的，只有先调用了顶层makefile之后，这些变量在子目录中的makefile才被赋值。

ERSION = 4
PATCHLEVEL = 10
SUBLEVEL = 15
EXTRAVERSION =
NAME = Fearless Coyote
