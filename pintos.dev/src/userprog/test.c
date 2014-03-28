#include<stdio.h>
#include<stdlib.h>

int main()
{
	void ** ptr=(void**)malloc(100);

	printf("%p\n",ptr);

	ptr+=25;

	printf("%p\n",ptr);
	

	return 0;
}
