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
struct prob{
	int *p;
	struct {
		int x;
		int y;
	} s;
	struct prob *next;
};

void test_struct_and()
{
	struct prob sp;
	struct prob *ssp = &sp;
	if(&ssp->s.x == &(ssp->s.x))
		printf("&s==&(s)");
	else
		printf("!=");
	
}

double uu2double(unsigned w1, unsigned w2)
{
	union{
		double d;
		unsigned u[2];
	}temp;
	temp.u[0] = w1;
	temp.u[1] = w2;
	printf("u0 is: %d\n", temp.u[0]);
	printf("u1 is: %d\n", temp.u[1]);
	return temp.d;
}

void main()
{
	unsigned w1 = 17;//00010001 
	unsigned w2 = 16;//00010000
	double a1 = uu2double(17,16);
	printf("a1 is: %.3f", a1);
	return;
	test_struct_and();
	return;
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
