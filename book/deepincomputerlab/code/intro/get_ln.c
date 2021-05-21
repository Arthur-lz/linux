# include <stdio.h>

char *get_line()
{
	char buf[4];
	char *result;
	gets(buf);
	result = malloc(strlen(buf));
	strcpy(result, buf);
	return result;
}

void main()
{
	char * s = get_line();
	//free(s);
}
