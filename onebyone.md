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

### 下面四个中哪一个不是表示数组char A[10]的A[1]的地址？
* s+1
* s++
* &s[0]+1
* &s[1]

### float x; x = 10/4;  问x=?
* 2.5
* 2
* 3
* 2.0
