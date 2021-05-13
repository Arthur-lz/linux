#include <stdio.h>
#define N 16
typedef int fix_matrix[N][N];

void fix_set_diag(fix_matrix A, int val)
{
	long i;
	for (i = 0;i < N; i++)
		A[i][i] = val;
}

void print_array(fix_matrix A)
{
	long i, j;
	for (i = 0; i < N; i++)
	{
		for (j = 0; j < N; j++)
			printf("%d", A[i][j]);
		printf("\n");
	}
}

void fix_set_diag_opt(fix_matrix A, int val)
{
	int *Aptr = &A[0][0];
	int *Aend = &A[N][N];
	do{
		*Aptr = val;
		Aptr += N + 1;
	}while(Aptr != Aend);
}

void it_eq(int n, int A[n][n], int i)
{
	if(A[i] == &A[i][0])
		printf("A[i]=&A[i][0]");
	else
		printf("!=");
}

void main()
{
	int n = 5;
	int A[n][n];
	it_eq(5, A, 3);
	return ;
	fix_matrix a = {0};
	int v = 9;
	//fix_set_diag(a, v);
	fix_set_diag_opt(a, v);
	print_array(a);
}
