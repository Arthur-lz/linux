# XSDT
## Extended System Description Table
* XSDT包含ACPI规范的版本号和一些与OEM相关的信息，并含有其他描述符表的64位物理地址，如FADT, SSDT等

* ACPI表中定义的描述符表
* 出自《PCI Express 体系结构导读》, 作者：王齐; 机械工业出版社

# RSDT
## Root System Description Table
* RSDT包含的信息与XSDT基本一致，只是在RSDT中存放的物理地址为32位。在V1.0之后的ACPI版本中，该描述符表被XSDT取代。但是有些BIOS可能会为操作系统同时提供RSDT和XSDT，并由操作系统选择使用RSDT还是XSDT

# FADT
## Fixed ACPI Description Table

# FACS
## Firmware ACPI Control Structure

# DSDT
## Differentiated System Description Table
* 该表是ACPI规范最复杂，同时也是最重要的一个表。该表包含处理器系统使用的硬件资源以及对这些硬件资源的管理操作
* SSDT可以对DSDT进行补充，在一个处理器系统中可以存在多个SSDT
* ACPI表存放在内存中
> ACPI表的提取方法

```
sudo apt install apcica-tools	# 安装acpica-tools
acpidump > abc			# acpidump在ubuntu20.04中包含在工具包acpica-tools中, 此命令用于将ACPI表从内存读出并存放在abc文件中
acpixtract -a abc		# 此命令用于将abc文件存放的ACPI表全部分解，得到的是一系列.dat文件
iasl -d xxx.dat			# 这些.dat文件使用AML语法规范，操作系统中的AML解释器可以分析这些在.dat文件中的数据；可以使用iasl命令将.dat文件转换为相应的.dsl文件，dsl文件中存放ASL源码
```

* DSDT表的调试
```
#1. 生成dsdt.dsl
iasl -tc dsdt.dsl
#2. 将dsdt.hex拷贝到内核源码目录include下
cp dsdt.hex kernel/include/

#3. 修改内核.config
CONFIG_STANDALONE=n
CONFIG_ACPI_CUSTOM_DEST=y
CONFIG_ACPI_CUSTOM_DEST_FILE="dsdt.hex"
#4. 重新编译内核
#5. Linux系统启用后将使用dsdt.hex替代bios提供的DSDT表

```

# ASL
## ACPI Source Language

# MADT
## Multiple APIC Description Table

# SBST
## Smart Battery Table

# SRAT
## System Resource Affinity Table

# SLIT
## System Locality Information Table
