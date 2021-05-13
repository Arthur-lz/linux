/* arrayshr.c说明
 * 算法：给定数组A = {1,2,3,4,5,6,7,8,9}, 右移k = 3后A的值为 A = {7,8,9,1,2,3,4,5,6};
 * 	 k为无符号数, A的数组元素为int类型
 * */
#include <stdio.h>
#define N  9
typedef int ary_t[N];
ary_t GA = {1,2,3,4,5,6,7,8,9};

void arrayshr(ary_t A, ary_t B, unsigned k)
{
	int *Aptr = &A[0];
	int *Aend = &A[N];
	int *Bptr = &B[0];
	unsigned K = k % N; // 如果k值大于N，其移动k次与移动K次是等效的
	int *Bbptr = &B[K]; // 
	unsigned i = 0;
	unsigned n = N - K;
	do{
		if(i < n){
			*Bbptr = *Aptr;
			Bbptr++;
			Aptr++;
		}else{
			*Bptr = *Aptr;
			Bptr++;
			Aptr++;
		}
		i++;
	}while(i < N);
}

void print_array(ary_t A)
{
	int i = 0;
	printf("A is :[");
	do{
		printf("%d", A[i]);
		if(i < N - 1)
			printf(",");

		i++;
	}while(i < N);
	printf("]");
}

void main()
{
	ary_t B = {0};
	printf("before Shr array\n");
	print_array(GA);
	arrayshr(GA, B, 3);
	printf("\nafter Shr array\n");
	print_array(B);
}
