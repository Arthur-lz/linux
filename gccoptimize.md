# GCC 的优化选项
* O0选项不进行任何优化，在这种情况下，编译器尽量的缩短编译消耗（时间，空间），此时，debug会产出和程序预期的结果。当程序运行被断点打断，此时程序内的各种声明是独立的，我们可以任意的给变量赋值，或者在函数体内把程序计数器指到其他语句,以及从源程序中精确地获取你期待的结果.

* O1优化会消耗稍多的编译时间，它主要对代码的分支，常量以及表达式等进行优化。

* O2会尝试更多的寄存器级的优化以及指令级的优化，它会在编译期间占用更多的内存和编译时间。

* O3在O2的基础上进行更多的优化，例如使用伪寄存器网络，普通函数的内联，以及针对循环的更多优化。

* Os主要是对代码大小的优化，我们基本不用做更多的关心。

***通常各种优化都会打乱程序的结构，让调试工作变得无从着手***。并且会打乱执行顺序，依赖内存操作顺序的程序需要做相关处理才能确保程序的正确性。


### 优化代码有可能带来的问题

*    调试问题：正如上面所提到的，任何级别的优化都将带来代码结构的改变。例如：对分支的合并和消除，对公用子表达式的消除，对循环内load/store操作的替换和更改等，都将会使目标代码的执行顺序变得面目全非，导致调试信息严重不足。

*    内存操作顺序改变所带来的问题：在O2优化后，编译器会对影响内存操作的执行顺序。例如：-fschedule-insns允许数据处理时先完成其他的指令；-fforce-mem有可能导致内存与寄存器之间的数据产生类似脏数据的不一致等。对于某些依赖内存操作顺序而进行的逻辑，需要做严格的处理后才能进行优化。例如，采用volatile关键字限制变量的操作方式，或者利用barrier迫使cpu严格按照指令序执行的。

*    效率是否有较大的提高有待试验论证：并不是所有的优化都会对执行效率产生积极的作用，有的时候利用优化会起到适得其反的效果。这就需要在试验的基础上来不断调整优化选项，来谋求最佳优化效果。但通常这样做所投入的时间和最终产生的效果之间比较，往往得不偿失。

### 代码质量

从代码质量来看，大部分的公用库的代码可以承受O2级别的优化（大部分开源的服务器程序都使用O2优化）。但对具有全局数据的模块，以及一些依赖内存执行顺序的模块，需要特别的注意并进行一定范围的修改。

例如：

Pendingpool中有类似如下的代码：

if (m_aySocket[index].nSock > 0) {

close(m_aySocket[index].nSock);

m_aySocket[index].nSock = -1;

}

m_aySocket[index].status= NOT_USED;

在这种应用中，对status的改变需要滞后于对nSock的改变，否则在多线程模式下，将会出现数据混乱，甚至会出core。此时需要对此段代码进行特殊处理（使用barrier等），以确保内存操作顺序。

又例如：

在comdb的应用中，认为对u_int类型的赋值操作是原子的，基于这种原因对索引操作未加任何的锁控制。如果进行编译优化，这种假设就不再成立了，需要对相关的变量加volatile关键字限制才行。

### 总结

*    采用gcc的优化会带来诸多问题，但的确可以提高cpu密集型的代码的执行效率。

*    目前公共库尚不具备全面进行优化的可能性，主要是优化代价高昂。

*    可以选择具有代表性的库进行优化试验，积累一些经验（与stl配合完成）。

---
---
### 细节->gcc各个优化等级对应的优化选项解析

由于目标平台和gcc的配置问题，在每一个gcc-O层次上可能会有差异，所以如果要想明确地知道gcc-O每一个层次打开的优化选项，只需要执行

gcc-c -Q -O(1、2、3、s)–help=optimizers来找到每一层次具体打开的优化选项。

下面就是对华为服务器上gcc(GCC) 4.9.2 20141101 (NeoKylin 4.9.2-1)各个优化级别打开选项的描述。

-O0：

不做任何优化，这是默认的编译选项。

-O和-O1：

对程序做部分编译优化，对于大函数,优化编译占用稍微多的时间和相当大的内存。使用本项优化，编译器会尝试减小生成代码的尺寸，以及缩短执行时间，但并不执行需要占用大量编译时间的优化。

打开的优化选项：
```
    -fcompare-elim：寄存器分配和后寄存器分配指令分割，识别算术指令，计算处理器标志类似于一个比较操作的基础上，该算法。如果可能的话，排除了明确的比较操作。此传递只适用于某些不能显式地表示在寄存器分配完成前比较操作的目标。

    -fauto-inc-dec：结合内存地址的递增和递减来访问内存地址，这个优化方法通常被忽略因为没有架构操作来支持。

    -fcprop-registers：因为在函数中把寄存器分配给变量,所以编译器执行第二次检查以便减少调度依赖性(两个段要求使用相同的寄存器)并且删除不必要的寄存器复制操作。

    -fdce：在RTL上进行死代码消除（DCE）

    -fdelete-null-pointer-checks：通过对全局数据流的分析，识别并排出无用的对空指针的检查。编译器假设间接引用空指针将停止程序。如果在间接引用之后检查指针，它就不可能为空。

    -fdse：在RTL上进行死存储消除(DSE)

    -fearly-inlining：内联函数具有“always_inline”功能，在做-fprofile-generate操作和realinlining pass，它的主体似乎比函数调用的开销早。这样做使分析开销更小和使内联更快的拥有大型连锁嵌套程序包装的功能。

    -fforward-propagate：RTL进行正向传播通道。如果结果可以简化，通过尝试结合两点说明和检查。如果循环展开活跃，两个通道进行第二通道是后循环展开计划。

    -fgcse-lm：全局公用子表达式消除将试图移动那些仅仅被自身存储kill的装载操作的位置。这将允许将循环内的load/store操作序列中的load转移到循环的外面（只需要装载一次），而在循环内改变成copy/store序列。在选中-fgcse后，默认打开。

    -fguess-branch-probability：当没有可用的profilingfeedback或__builtin_expect时，编译器采用随机模式猜测分支被执行的可能性，并移动对应汇编代码的位置，这有可能导致不同的编译器会编译出迥然不同的目标代码。

    -fif-conversion：尝试将条件跳转转换为等价的无分支型式。优化实现方式包括条件移动，min，max，设置标志，以及abs指令，以及一些算术技巧等。

    -fif-conversion2：基本意义和上面参数相同。

    -finline-functions-called-once：考虑到所有的“静态”函数调用一次内联到调用者即使他们没有标明“内联”。如果一个给定函数的调用是集成的，则该函数不输出为它自己的汇编代码。

    -fipa-profile：执行过程间轮廓传播。冷函数调用的函数被标记为冷。可以执行一次（如“冷”、“只能进不能退”，静态构造函数或析构函数）的确定。冷函数和循环的函数执行一次一次，然后进行了优化的大小。

    -fipa-pure-const：找到哪些函数是pure或持续的。

    -fipa-reference：发现哪一个静态变量不逃避编译单元。

    -fira-hoist-pressure：使用IRA评价代码提升通过提升表达决定登记压力。此选项通常会导致较小的代码，但它可以减慢编译器的速度

    -fivopts：执行感应变量优化（强度折减，感应变量合并和感应变量消去法）

    -fmerge-constants：尝试横跨编译单元合并同样的常量(stringconstants and floating point constants)

    -fmove-loop-invariants：使在RTL回路优化循环不变通。

    -fprefetch-loop-arrays：如果目标机器的支持，生成指令预取内存提高循环访问大型阵列的性能。此选项可以产生更好的或更坏的代码；结果是高度依赖于源代码中的循环结构。

    -frename-registers：试图通过使用寄存器分配后的寄存器来避免计划代码中的错误依赖关系。这一优化最有好处的处理器有很多寄存器。根据目标所采用的调试信息格式，但是，它可以使调试是不可能的，因为变量不再停留在“homeregister”。

    -fsched-critical-path-heuristic：启用关键路径启发式的调度程序。此启发式有利于在关键路径上的指令。这是启用时，默认情况下调度启用后，即与fscheduleinsns或fschedule-insns2或在O2或高。

    -fsched-dep-count-heuristic：使依赖计数启发式的调度程序。这种启发式的指令有更多的指令，这取决于它。这是启用的默认是启用调度时，即与fscheduleinsns或fschedule-insns2或在O2或更高。

    -fsched-group-heuristic：使组启发式的调度程序。这种启发式的指令，属于一个时间表组。这是默认启用的调度时启用，即与fscheduleinsns或fschedule-insns2或在O2或更高。

    -fsched-last-insn-heuristic：使最后一个指令在调度程序中使用。这种启发式的指令，是不太依赖于上一个指令。这是默认情况下启用调度时启用，即与fscheduleinsns或fschedule-insns2或在O2或更高。

    -fsched-rank-heuristic：排序启发式的调度程序。这种启发式有利于指令属于一个更大的大小或频率的基本块。这是启用的默认是启用调度时，即与fscheduleinsns或fschedule-insns2或在O2或更高。

    -fsched-spec-insn-heuristic：使程序中的投机指令启发式。这种启发式有利于投机的指令，具有更好的弱依赖性。这是启用默认情况下，当调度功能，即用fscheduleinsns或fschedule-insns2或在O2或更高。

    -fsched-stalled-insns-dep：在一个停滞的insn依赖上定义有多少组（循环）被计算。这个仅仅对第二次记时通道上面才会参数影响，并且仅仅当-fsched-stalled-insns被使用了。

    -fsection-anchors：尽量减少使用共享的“锚”符号来解决附近的对象的符号地址计算的数目。该转换可以帮助减少获得的条目的数目，并获得一些目标的访问。

    -fshrink-wrap：发射功能开口只需要它的功能部分，而不是在函数顶部。

    -fsplit-ivs-in-unroller：使价值观归纳变量迭代后期展开的循环使用，在第一次迭代的价值。这打破长期依赖链，从而提高调度效率通过。

    -fsplit-wide-types：当使用一种在32位系统中占用多个寄存器的类型，例如“长”的一个，拆分的寄存器，并独立分配。这通常会产生更好的代码，但可能会使调试更困难。

    -fno-threadsafe-statics：不会产生额外的代码用于初始化静态局部对象的线程安全的C++ABI指定的程序。您可以使用此选项来减少代码大小，这并不需要是线程安全的。

    -ftree-bit-ccp：执行稀疏条件下的树和传播指针对齐信息。此通只能操作本地标量变量，并在默认情况下-O或更高的优化级别中启用。它同时还需要-ftree-cpp启用。

    -ftree-ccp：在树（数据结构一种）上执行稀疏条件常数传播。

    -ftree-ch：执行循环头复制树。这是有益的，因为它增加了代码运动优化的有效性。它还可以节省一个跳转。

    -ftree-coalesce-vars：告诉copyrenamepass（见-ftree-copyrename）来试图结合小用户定义的变量替代掉编译器临时变量。这可能会严重限制调试优化程序编译与FNOVAR跟踪任务的能力。在否定形式，这标志阻止了SSA凝聚用户变量，包括内联的。

    -ftree-copy-prop：在树上进行复制繁殖。这一关消除了不必要的复制操作

    -ftree-copyrename：树上执行重命名复制。这通过重命名其他变量在编译临时副本的位置，通常导致的变量名更接近于原始变量。

    -ftree-dce：在树上进行死代码消除

    -ftree-dominator-opts：完成各种简单的标量的清理（常数/复制传播，消除冗余、范围扩展和简化）基于支配树的遍历。这也执行跳转线程（以减少跳转）。

    -ftree-dse：在树上执行死存储消除

    -ftree-forwprop：在树上执行正向传播

    -ftree-fre：在树上执行完全消除冗余。

    -ftree-loop-if-convert：尝试在最内层循环中转换有条件跳转到分支较少的等价方案。目的是为了消除从最内层循环的控制流程来提高矢量化的处理这些循环的能力。

    -ftree-loop-im：在树上执行循环不变量

    -ftree-loop-ivcanon：创建一个典型的迭代次数，在循环中确定迭代次数需要复杂的分析。通过后来的优化可以确定的数量很容易。结合展开(unrolling)特别有用。

    -ftree-loop-optimize：执行树上的循环优化

    -ftree-phiprop：从树上的条件指针中执行提升负载

    -ftree-pta：对树进行功能局部点分析

    -ftree-reassoc：在树上进行重组

    -ftree-sink：在树上执行正向存储动作

    -ftree-slsr：在树上执行straight-line强度折减。当有可能的时候，这种认知的相关的表达式比如涉及乘法的被更小代价的计算所替代。

    -ftree-sra：进行总量的标量替换。这种通过替换标量结构参考防止犯结构记忆太早。

    -ftree-ter：在SSA->normal数组中执行临时表达式的替换。

    -funit-at-a-time：兼容性的需要，不产生什么影响，但是它暗示了-fno-toplevel-reorder和-fno-section-anchors。

    -fweb：构建用于保存变量的伪寄存器网络。伪寄存器包含数据,就像他们是寄存器一样,但是可以使用各种其他优化技术进行优化,比如cse和loop优化技术。这种优化会使得调试变得更加的不可能，因为变量不再存放于原本的寄存器中。
```
-O2：

是比O1更高级的选项，进行更多的优化。Gcc将执行几乎所有的不包含时间和空间折中的优化。当设置O2选项时，编译器并不进行循环打开（loopunrolling）以及函数内联。与O1比较而言，O2优化增加了编译时间的基础上，提高了生成代码的执行效率。

O2打开所有的O1选项，并打开以下选项:

-falign-functions-falign-jumps -falign-labels -falign-loops -fcaller-saves-fcrossjumping -fcse-follow-jumps -fdevirtualize-fdevirtualize-speculatively -fexpensive-optimizations -fgcse-fhoist-adjacent-loads -finline-small-functions -fipa-cp -fipa-sra-fisolate-erroneous-paths-dereference -foptimize-sibling-calls-foptimize-strlen -fpeephole2 -freorder-blocks -freorder-functions-frerun-cse-after-loop -fschedule-insns -fschedule-insns2-fstrict-aliasing -fthread-jumps -ftree-builtin-call-dce -ftree-pre-ftree-switch-conversion -ftree-tail-merge -ftree-vrp
```
    -falign-functions：这个选项用于使函数对准内存中特定边界的开始位置。大多数处理器按照页面读取内存，并且确保全部函数代码位于单一内存页面内,就不需要叫化代码所需的页面。

    -falign-jumps：对齐分支代码到2的n次方边界。在这种情况下，无需执行傀儡指令（dummyoperations）

    -falign-labels：对齐分支到2的n次幂边界。这种选项容易使代码速度变慢，原因是需要插入一些dummyoperations当分支抵达usualflow of the code.

    -falign-loops：对齐循环到2的n次幂边界。期望可以对循环执行多次，用以补偿运行dummyoperations所花费的时间。

    -fcaller-saves：通过存储和恢复call调用周围寄存器的方式，使被call调用的value可以被分配给寄存器，这种只会在看上去能产生更好的代码的时候才被使用。（如果调用多个函数,这样能够节省时间,因为只进行一次寄存器的保存和恢复操作,而不是在每个函数调用中都进行。）

    -fcrossjumping：这是对跨越跳转的转换代码处理，以便组合分散在程序各处的相同代码。这样可以减少代码的长度，但是也许不会对程序性能有直接影响。

    -fcse-follow-jumps：在公用子表达式消元时，当目标跳转不会被其他路径可达，则扫描整个的跳转表达式。例如，当公用子表达式消元时遇到if...else...语句时，当条件为false时，那么公用子表达式消元会跟随着跳转。

    -fdevirtualize：试图将虚函数的调用转化为直接调用。这一优化是一个程序或一个程序内部作为间接内联和常数传递的一部分。

    -fdevirtualize-speculatively：试图将虚函数的调用转换为有目的的直接调用，通过对类型继承图的分析，确定一个给定的目标集。如果设置是小的，优选的大小为1，则将调用变成有条件的直接和间接调用。有目的性的调用允许更多的优化，如内联。当他们看起来没用的时候，经过进一步的优化，他们被转换回原来的形式。

    -fexpensive-optimizations：进行一些从编译的角度来说代价高昂的优化（这种优化据说对于程序执行未必有很大的好处，甚至有可能降低执行效率，具体不是很清楚)

    -fgcse：执行全局公用子表达式消除pass。这个pass还执行全局常量和copypropagation。这些优化操作试图分析生成的汇编语言代码并且结合通用片段，消除冗余的代码段。如果代码使用计算性的goto,gcc指令推荐使用-fno-gcse选项。

    -fhoist-adjacent-loads：如果程序加载是由两个结构相同的相邻的位置并且目标架构有条件转移指令，那么提升if-then-else各个分支的加载。

    -finline-small-functions：函数调用时，当被调用的函数代码小于期望的代码的时候，把代码整合进来。编译器自己决定哪些功能足够简单，值得这样的整合。这个适合所有的内联函数，和甚至一些没有宣布内核的函数。

    -fipa-cp：执行过程间的常数传递。此优化分析程序确定传递给函数的值是常数，然后进行相应的优化。如果优化程序具有常数传递函数，该优化可以大大提高性能。

    -fipa-sra：移除不使用的参数

    -fisolate-erroneous-paths-dereference：检测路径来触发错误或由于引用一个空指针的未定义的行为。从主控制流中分离这些路径，并将错误的或未定义的行为变为陷阱。

    -foptimize-sibling-calls：优化相关的以及末尾递归的调用。通常,递归的函数调用可以被展开为一系列一般的指令，而不是使用分支。这样处理器的指令缓存能够加载展开的指令并且处理他们,和指令保持为需要分支操作的单独函数调用相比,这样更快。

    -foptimize-strlen：优化标准C字符串函数（比如strlen、strchr或strcpy）。

    -fpeephole2：允许计算机进行特定的观察孔优化(这个不晓得是什么意思)，-fpeephole与-fpeephole2的差别在于不同的编译器采用不同的方式，由的采用-fpeephole，有的采用-fpeephole2，也有两种都采用的。

    -freorder-blocks：在编译函数的时候重新安排基本的块，目的在于减少分支的个数，提高代码的局部性。

    -freorder-functions：在编译函数的时候重新安排基本的块，目的在于减少分支的个数，提高代码的局部性。这种优化的实施依赖特定的已存在的信息：.text.hot用于告知访问频率较高的函数，.text.unlikely用于告知基本不被执行的函数。

    -frerun-cse-after-loop：在循环优化完成后，重新进行公用子表达式消元操作。

    -fschedule-insns：编译器尝试重新排列指令，用以消除由于等待未准备好的数据而产生的延迟。这种优化将对慢浮点运算的机器以及需要loadmemory的指令的执行有所帮助，因为此时允许其他指令执行，直到loadmemory的指令完成，或浮点运算的指令再次需要cpu。

    -fschedule-insns2：与-fschedule-insns相似。但是当寄存器分配完成后，会请求一个附加的指令计划pass。这种优化对寄存器较小，并且loadmemory操作时间大于一个时钟周期的机器有非常好的效果。

    -fstrict-aliasing：这种技术强制实行高级语言的严格变量规则。对于c和c++程序来说,它确保不在数据类型之间共享变量.例如,整数变量不和单精度浮点变量使用相同的内存位置。

    -fthread-jumps：如果某个跳转分支的目的地存在另一个条件比较,而且该条件比较包含在前一个比较语句之内,那么执行本项优化.根据条件是true或者false,前面那条分支重定向到第二条分支的目的地或者紧跟在第二条分支后面.

    -ftree-builtin-call-dce：通过调用内置函数执行有条件地消除死代码，并且没有副作用。

    -ftree-pre：树的部分冗余消除优化

    -ftree-switch-conversion：从执行简单的初始化转换为在标量阵列中执行初始化

    -ftree-tail-merge：寻找相同的代码序列。当找到时，替换一个跳转到另一个。这种优化被称为尾巴合并或交叉跳跃。

    -ftree-vrp：在树上进行值的传递。这类似于常理传递，但不是数值，而是一个范围值的传递。优化方法是去除不必要的范围检测比如数组边界检测和空指针检测。
```
-O3：

比O2更进一步的进行优化。在包含了O2所有的优化的基础上，又打开了以下优化选项：

-fgcse-after-reload-finline-functions -fipa-cp-clone -fpredictive-commoning-ftree-loop-distribute-patterns -ftree-loop-vectorize-ftree-partial-pre -ftree-slp-vectorize -funswitch-loops

```
    -finline-functions：内联简单的函数到被调用函数中。由编译器启发式的决定哪些函数足够简单可以做这种内联优化。默认情况下，编译器限制内联的尺寸，3.4.6中限制为600（具体含义不详，指令条数或代码size？）可以通过-finline-limit=n改变这个长度。这种优化技术不为函数创建单独的汇编语言代码，而是把函数代码包含在调度程序的代码中。对于多次被调用的函数来说,为每次函数调用复制函数代码。虽然这样对于减少代码长度不利,但是通过最充分的利用指令缓存代码,而不是在每次函数调用时进行分支操作,可以提高性能。

    -fgcse-after-reload：重载以后进行冗余负载消除，这一优化的目的是清理多余的溢出。

    -fipa-cp-clone：执行克隆使得常数传播更加强健。启用这一参数时，当外部可见的功能需要调用常数参数时，过程间传播常数进行克隆。因为优化会创建多个函数的副本，所以会显著增加代码大小。

    -fpredictive-commoning：执行预测公共优化，即在以前迭代进行的循环中重用计算优化（特别是内存加载和存储）。

    -ftree-loop-distribute-patterns：通过调用库中生成的代码来执行模式的循环分布。

    -ftree-loop-vectorize：对树进行循环矢量化。

    -ftree-partial-pre：使部分冗余消除更有效。

    -ftree-slp-vectorize：对树进行基本块的矢量化。

    -funswitch-loops：将无变化的条件分支移出循环，取而代之的将结果副本放入循环中。
```
-Os：

主要是对程序的尺寸进行优化。打开了大部分O2优化中不会增加程序大小的优化选项，并对程序代码的大小做更深层的优化。（通常我们不需要这种优化）对比O2，Os会关闭如下选项：

-foptimize-strlen和-fschedule-insns

然后还打开了-finline-functions选项（公司gcc的Os参数打开选项个人认为比较异常，有些优化按照道理应该关的却没有关，按照man手册的说法，-falign-functions -falign-jumps -falign-loops -falign-labels -freorder-blocks-fprefetch-loop-arrays

-freorder-blocks-and-partition这些选项是应该关闭的）。


* 题外话，gcc 10.2.0版本的帮助文件有19773行，足够写本书了。

### gcc 选项-W
