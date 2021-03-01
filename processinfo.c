#include <linux/kernel.h> /* We're doing kernel work */
#include <linux/module.h> /* Specifically, a module */
#include <linux/proc_fs.h> /* Necessary because we use proc fs */
#include <asm/uaccess.h> /* for copy_*_user */
#include <linux/fs.h>	/*file structure*/
#include <asm/segment.h>
#include <linux/buffer_head.h>	
#include <linux/init.h>
#include <linux/string.h>	/*String operations*/
#include <linux/sched/signal.h> /*Signal-kill etc.*/
#include <linux/syscalls.h>





// file open part- return a file pointer - return value is used in read operation

struct file *file_open(const char *path, int flags, int rights) 
{
    	struct file *filp = NULL;
    	mm_segment_t oldfs;
    	int err = 0;

    	oldfs = get_fs();
    	set_fs(get_ds());
    	filp = filp_open(path, flags, rights);
    	set_fs(oldfs);
    	if (IS_ERR(filp)) {
        	err = PTR_ERR(filp);
        	return NULL;
    	}
    	return filp;
}



// close file

void file_close(struct file *file) 
{
    	filp_close(file, NULL);
}



//read file file->filepointer from file_open, data->buffer

int file_read(struct file *file, unsigned long long offset, unsigned char *data, unsigned int size) 
{
    	mm_segment_t oldfs;
    	int ret;

    	oldfs = get_fs();
    	set_fs(get_ds());
	

	// Read the file from the kernel space.
	ret = kernel_read(file, data, size, &offset);


    	set_fs(oldfs);
    	return ret;
} 


//reads proc files and gets data which is those will be added to a string to use for copy_to_user
int getInProcFile(int id, int type, char output[]){

	char *strcopy;			//pointer to buffer

	char buffer[1000] = "x";	//readfile buffer

	char sendS[100] = "";		//append all values(id,uid...) into a output string	
	
	char arr[5][60] = {" ", " ", " ", " ", " "};	//string array for colums, id-pid-uid...
		
	struct file *f = NULL;	
		
	//proc file path, dynamic
	char path[100]; 

	if(type == 0){ //which means -all

		sprintf(path, "/proc/%d/status", id);	

		f = file_open(path, O_RDONLY, 0);

		file_read(f, 0, buffer, 500);

		
		//String operations
		strcopy = buffer;	
		strsep(&strcopy, ":");
		strcpy(arr[0], strsep(&strcopy, "\n"));
		strsep(&strcopy, "\n");
		strsep(&strcopy, ":");
		strcpy(arr[1], strsep(&strcopy, "\n"));	
		strsep(&strcopy, "\n");
		strsep(&strcopy, "\n");
		strsep(&strcopy, ":");
		strcpy(arr[2], strsep(&strcopy, "\n"));
		strsep(&strcopy, ":");
		strcpy(arr[3], strsep(&strcopy, "\n"));
		strsep(&strcopy, "\n");
		strsep(&strcopy, ":");
		strcpy(arr[4], strsep(&strcopy, "\n"));	

		

		//values(id,pid,...) into sendS
		strncat(sendS, arr[0], strlen(arr[0]) + 2); 
		strncat(sendS, arr[1], strlen(arr[1]) + 2); //strncat(sendS, "\0", 1);
		strncat(sendS, arr[2], strlen(arr[2]) + 2); //strncat(sendS, "\0", 1);
		strncat(sendS, arr[3], strlen(arr[3]) + 2); //strncat(sendS, "\0", 1);			
		strncat(sendS, arr[4], strlen(arr[4]) + 2); strncat(sendS, "\n", 2);
	
		//sendS into output, for all id's			/*output will be copy to user space*/
		strncat(output, sendS, strlen(sendS) + 2);
		

	}
	else if(type == 1){ //which means -p
		
		//cmdline path, dynamic
		sprintf(path, "/proc/%d/cmdline", id);		

		f = file_open(path, O_RDONLY, 0);


		file_read(f, 0, buffer, 100);

		//into output
		sprintf(output, "%d\t%s", id, buffer);

		


	}
	
	file_close(f);

	return 0;	//returns


}



SYSCALL_DEFINE3(mysyscallib, char *, first, int, second, char __user *, output_user)
{



	char output[8132] = "";

	
	//no paramater
	if(strcmp(first,"") == 0) {

		
		//required details into output string
		sprintf(output, "-all prints some information (process id and its argument/s) about all processes\n-p takes process id and prints the details of it\n-k takes process id\n");

		
		//printk("-all prints some information (process id and its argument/s) about all processes\n");
		//printk("-p takes process id and prints the details of it\n");
		//printk("-k takes process id\n");
		//dmesg
		printk("%s\n", output);
		
		
		
	}
	//-all
	else if(strcmp(first,"-all") == 0){

		struct task_struct* task_list;

		printk("all\n");

        	for_each_process(task_list) {				/*for each task getInProcFile, put data into an string which is output*/

			getInProcFile(task_list->pid, 0, output);	//task_list->pid return ppit id, 0 means -all part of getInProcFile
					               
        	}

	}
	//-k pid
	else if(strcmp(first,"-k") == 0){
	

		struct pid *pid;

		struct task_struct* task_list;


		for_each_process(task_list) {		/*for each process find struct pid and call kill_pid with using this id to kill process*/

			//if pid equals paramater that gotten by user space
			if(task_list->pid == second){			
				
				pid = get_task_pid(task_list, PIDTYPE_PID);	//returns pid *pid		
                		kill_pid(pid, SIGKILL, 1);			//kills the process, SIGKILL -> kill signal
			}
        	}

		
		//into output
		sprintf(output, "Process %d was killed!", second);
		//dmesg
		printk("%s\n", output);
		


	}
	//-p id
	else if(strcmp(first,"-p") == 0){


		getInProcFile(second, 1, output);	/*get in proc file, read data for desired specific process, 1 mean -p part of getInProcFile*/
	
		//dmesg
		printk("%s\n", output);


	}
	else{
		return -1;
	}
	
	//copies desired data to user space, output -> output_user
	copy_to_user(output_user, output, 8132);
	
	return 0; //represents success and any other value failure.



}






