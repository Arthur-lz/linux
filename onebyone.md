# linux 开发一步一步来

### memmove
```c
void *memmove(void *dest, void *source, sizt_t n)
{
	if((dest == NULL)||(source == NULL))
		return NULL;

	int count = n;
	const char *s = source;
	char *d = dest;

	if(dest <= source){ // dest的高地址有可能与source重叠, 所以先将source低地址值复制给dest
		while(count--){
			*d++=*s++;	
		}	
	}else{ // dest的尾有可能与source重叠, 所以先将source高地址的数据复制给dest
		d+=n;
		s+=n;
		while(count--){
			*--d=*--s;
		}
	}
	return dest;
}
```
### memcpy
```c
```
---
### 下面四个中哪一个不是表示数组char A[10]的A[1]的地址？
* s+1
* s++
* &s[0]+1
* &s[1]
---
### float x; x = 10/4;  问x=?
* 2.5
* 2
* 3
* 2.0
---
### 在C/C++中，goto关键词只能函数内的局部跳转
---
### setjmp和longjmp
> 函数间的跳转需要使用setjmp和longjmp，这也是有些协程库基于setjmp和longjmp实现的原因
---
### LOAD为读操作，STORE为写操作
---
### 四种SMP架构的CPU内存一致性模型
|模型|说明|
|:-|:-|
|顺序一致性模型|（SC，Sequential Consistency，所有读取和所有写入都是有序的）|
|宽松一致性模型|（RC，Relaxed Consistency，允许某些可以重排序），ARM和POWER属于这类|
|弱一致性模型|（WC，Weak Consistency，读取和写入任意重新排序，仅受显式内存屏障限制）|
|完全存储排序|（TSO，Total Store Ordering），SPARC和x86属于这种类型，只有store load一种情况会发生重排序，其它情况和SC模型一样|
---
### x86-TSO
>x86-TSO是Intel推出的一种CPU内存一致性模型，特点是只有“Store Load”一种情况会重排序，也就是“Load”可以排（乱序）在“Store”的前面，因此不需要“Load Load”、“Store Store”和“Load Store”这三种屏障。

|类型|作用|
|:-----------------------|:-----------|
|Store Load屏障|确保“前者刷入内存”的数据对“后者加载数据”是可见的；|
|Load Load屏障|确保“前者装载数据”先于“后者装载指令”；|
|Store Store屏障|确保“前者数据”先于“后者数据”刷入内存，且“前者刷入内存的数据”对“后者是可见的”；|
|Load Store屏障|确保“前者装载数据”先于“后者刷新数据到内存”。|

### SMP对称多处理器结构
>多个CPU对称工作没有区别，无主次或从属关系，平等地访问内存、外设和一个操作系统，共享全部资源，如总线、内存和I/O系统等，因此也被称为一致存储器访问结构（UMA ： Uniform Memory Access）。

### NUMA处理器结构
>（Non-Uniform Memory Access，非统一内存访问），基本特征是将CPU分成多个模型，每个模型多个CPU组成，具有独立的本地内存和I/O槽口等

### MPP处理器结构
>（Massive Parallel Processing，海量并行处理结构），基本特征是由多个SMP服务器（每个SMP服务器称节点）通过节点互联网络连接而成，每个节点只访问自己的本地资源（内存、存储等），是一种完全无共享（Share Nothing）结构

### MESIF
>多核CPU缓存一致性协议

|协议|说明|
|:-|:-|
|M修改（Modified）|该Cache line（缓存行）有效，数据被修改（dirty）了，和主存中的数据不一致，数据只存在于本Cache中|
|E独占互斥（Exclusive）|该Cache line只被缓存在该CPU的缓存中，它是未被修改过的（clean），与主存中数据一致|
|S共享（Shared）|该Cache line有效，数据和内存中的数据一致，数据存在于很多Cache中|
|I无效（Invalid）|该Cache line无效，可能有其它CPU修改了该Cache line|
|F转发（Forward）|Intel提出来的，意思是一个CPU修改数据后，直接针修改的结果转发给其它CPU|
---
### 内存屏障（Memory Barrier）
>内存屏障，也叫内存栅栏（Memory Fence）。分编译器屏障（Compiler Barrier，也叫优化屏障）和CPU内存屏障，其中编译器屏障只对编译器有效，它们的定义如下表所示（限x86，其它架构并不相同）

```c
#define barrier() __asm__ __volatile__("":::"memory")
//编译器屏障，如果是GCC，则可用__sync_synchronize()替代

#define mb()  __asm__ __volatile__("mfence":::"memory")
//CPU内存屏障，还分读写内存屏障

#define rmb() __asm__ __volatile__("lfence":::"memory")
//CPU读（Load）内存屏障

#define wmb() __asm__ __volatile__("sfence":::"memory")
//CPU写（Store）内存屏障
```
* x86架构的CPU内存屏障代码可在内核源代码的arch/x86/include/asm/barrier.h中找到。

* 对于原子操作，需要使用CPU提供的“lock”指令

* 对于CPU乱序需使用CPU内存屏障

|代码顺序|编译器顺序|CPU顺序|
|:-|:-|:-|
|a=1;|a=1;|b=x;|
|b=x;|d=2;|c=z;|
|c=z;|b=x;|a=1;|
|d=2;|c=z;|d=2;|
---
### __user
使用时一般是这样的char __user *x; // 提醒x是一个指向用户空间的变量，但如果你是使用kmalloc为x分配的空间，那么x实际上使用的还是内核空间的地址。也就是说，__user只是用于提醒，并不代表x一定是用户空间的地址。

* 这里假设有如下定义
void fun(const char __user *a);// a是由应用层传入的用户空间数据，那么内核态中执行函数fun内的代码时，如果要操作变量a，则一般需要如下这样，
```c
void fun(const char __user *a, int count){
	if(IS_ERR(a))
		return;
	if(count <= 0)
		return;

	char * d = kzalloc(sizeof(char)*count, GFP_KERNEL);
	if(d){
		if(!copy_from_user(d, a, count)){
			printk(KERN_DEBUG "%s, input:%s\n", __func__, d);
			// 修改d中数据
			d[0] = 't';
			// 将新数据写回用户空间的变量a
			if(!copy_to_user((char *)a, d, count))
				printk(KERN_DEBUG "new d:%s\n", d);
			
		}
		kfree(d);
	}
}

```
---
### EXPORT_SYMBOL(x)
* 导出模块中的函数、变量，这样可以在其他模块或内核中使用

* 举例如下，
```c
// 在模块D1中定义函数funx
char funx(const char * a, const size_t b){
...
}
EXPORT_SYMBOL(funx);

// 在模块D2中调用funx
extern char funx(const char *a, const size_t b);

void somefunc(const char *a, const size_t b){

	char r = funx(a, b);
	
}
```
* 可以使用cat /proc/kallsyms 来查看内核所有可用的symbol
