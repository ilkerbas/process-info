#include <stdio.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>
#include<stdlib.h>
int main(int argc, char *argv[])
{
			
	//user space buffer, for copy_to_user in kernel space
	char dest[8192] = " ";
	if(argc == 2 && (strcmp(argv[1], "-all") == 0)){
		long int success = syscall(335, "-all", 1, dest);
	}
	else if(argc == 1){
		long int success = syscall(335, "", 1, dest);
	}
	else if(argc == 3 && (strcmp(argv[1], "-p") == 0 || strcmp(argv[1], "-k") == 0)) {
		int x = atoi(argv[2]);
		long int success = syscall(335, argv[1], x, dest);
	}
	printf("%s", dest);
	return 0;

}

