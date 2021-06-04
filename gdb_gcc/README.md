# gcc, gdb总结
* gcc 带调试信息 gcc -g
* pthread不是linux默认库，链接时需要引用库文件libpthread
> gcc编译含有pthread相关函数的.c文件时，需要参数 -lpthread
