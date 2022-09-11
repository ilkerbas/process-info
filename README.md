# process-info

processinfo is a system call for Linux kernel.


## Details & Guide
* Compiled on Linux-4.19.161.

* processinfo.c is for Kernel Space.

* processinfo_usr.c is for User Space.

* Customize and use these

>>335 common mysyscallib __x64_sys_mysyscallib; 

>>asmlinkage long sys_mysyscallib(char*, int, char __user *);


## Usage
```
● without argument, it prints the right usage of this program.
● “-all”, it lists all the processes and some information about them.
● “-p processID”, it outputs the given process ID and its command line.
● “-k processID”, it kills the given process ID

The sample executing formats and its functionality shortly.
./processinfo.out >> program’s right usage
./processinfo.out -all >> all processes some information
./processinfo.out -p 1425 >> process and its cmdline
./processinfo.out -k 1425 >> kill process
```

## Screenshots

![processinfoss1](https://user-images.githubusercontent.com/61518885/189552467-1eb53695-2362-40eb-80d2-40689771af4b.png)

![processinfoss2](https://user-images.githubusercontent.com/61518885/189552471-a597b9a2-31b8-40d5-bfc1-0e0dc726a90b.png)




