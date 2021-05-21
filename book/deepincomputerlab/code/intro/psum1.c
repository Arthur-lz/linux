void psum1(float a[], float p[], long n)
{
	long i;
	float v = p[0] = a[0];
	for (i = 1; i < n; i++){
		v += a[i];
		p[i] = v;
	}
}

void psum2(float a[], float p[], long n)
{
	long i;
	float v_last = p[0] = a[0];
	float v;
	for (i = 1; i < n; i++){
		v = v_last + a[i];
		p[i] = v;
		v_last = v;
	}
}
