#include<stdio.h>

int main()
{
	int val = 5;
int pid;
if(pid = fork())
{
	wait(pid);
}
else
	exit(val);
val++;
printf("%d\n", val);
return val;
}
