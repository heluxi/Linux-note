#   Linux 网络编程

[TOC]



## 库的制作

### 什么是库

可以简单的把库文件看成一种代码仓库，提供给使用者一些可以直接拿来用的变量、函数或类。库不能单独运行，库文件有两种，静态库和动态库。区别是：静态库在程序的链接的阶段被复制到了程序中，动态库在链接阶段没有被复制到程序中，而是在运行时由系统动态加载到内存中供程序调用。

#### gcc常用参数：

![](/run/media/root/8E9A022C9A0210FF/QQ图片20230714104833.jpg)

![QQ图片20230714104852](/run/media/root/8E9A022C9A0210FF/QQ图片20230714104852.jpg)

静态库的命名规则：libxxxx.a  xxx库的名字 .a 后缀 使用库时应该使用库的名字也就是xx

#### 静态库制作：1.gcc 获得 .o 文件  2.将.o 文件打包，使用ar工具 ar rcs libxxx.a xxx.o xxx.o 

静态库的使用：gcc test.c  -o app -L 库路径 -l 库名字

**静态库gcc进行链接时，会把静态库的代码打包到可执行程序中**。

动态库的命名规则：libxxxx.so lib前缀 xxx库的名字  so后缀

#### 动态库的制作：1.gcc 得到 .o文件，得到与位置无关的代码   gcc -c **-fpic/-fPIC** a.c b.c   2.得到动态库gcc  -shared a.o b.o -o libxxx.so

**动态库gcc进行链接时，动态库的代码不会被打包到可执行文件中**，程序启动之后，动态库会被动态加载到内存中。系统在加载可执行代码时，能够知道其所依赖的库的名字，但是**还需要知道绝对路径**。**此时就需要系统动态载入器来获取该绝对路径**。可通过 ldd 命令来查看可执行程序的动态库依赖。

方法：1.配置环境变量 export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:动态库的绝对路径

​           2.修改 etc/ld.so.cache文件列表 （sudo vim /etc/ld.so.conf 将绝对路径复制到此处  sudo ldconfig 更新）

#### 程序编译成可执行程序的过程：

![](/run/media/root/8E9A022C9A0210FF/QQ图片20230717104356.jpg)

静态库的优缺点：

![](/run/media/root/8E9A022C9A0210FF/QQ图片20230717104405.jpg)

动态库的优缺点：      	 

![](/run/media/root/8E9A022C9A0210FF/QQ图片20230717104413.jpg)

## makefile

makefile文件定义了一系列的规则来指定哪些文件需要先编译，哪些文件需要后编译，哪些文件需要重新编译，还可以进行更复杂的功能操作，因为makefile文件就像一个shell脚本一样，也可以执行操作系统的命令。**makefile带来的好处就是自动化编译，一旦写好，只需要一个make命令，整个工程完全自动编译**。**make是一个命令工具。解释makefile文件中指令的命令工具。**

### makefile规则



```
目标....:依赖...
	命令（shell命令）
	....
目标：最终要生成的文件
依赖：生成目标所需要的文件或是目标
命令：通过执行命令对依赖操作生成目标（必须tab缩进）
  app:sub.c add.c mutil.c div.c main.c
      gcc sub.c add.c mult.c div.c main.c -o app
      
makefile默认执行第一条规则，如果后面的规则和第一条规则无关，就不会执行
app：sub.c add.c mutil.c div.c main.c
	 gcc sub.c add.c mutil.c div.c main.c -o app
test:test.c
	 gcc test.c -o test//就不会执行
makefile会检测更新，如果修改了依赖文件，就会当前make时就会重新执行生成目标，如果没有对文件改动，就不会再次执行。	 
```

```
自定义变量
	变量名=变量值
预定义变量
	AR：归档维护程序的名称，默认值为ar
	CC：C编译器的名称，默认cc
	CXX：c++编译器的名称，默认值为g++
	$@:目标的完整名称     //自动变量只能在规则的命令中使用
	$<:第一个依赖文件的名称
	$^:所有的依赖文件
获取变量的值
	$(变量名)
	
	app：main.c a.c b.c
		gcc -c main.c a.c b.c -o app
	就可以替换成：
	app：main.c a.c b.c 
		$(CC) -c $^ -o &@
```

```
#定义变量
src=sub.c add.c mult.c div.c main.c
target=app
$(target):$(src)
	$(CC) $(src) -o $(target)
```

```
模式匹配
%.o:%.c  //%:通配符，匹配一个字符串，两个%匹配的是同一个字符串

src=sub.o add.o mult.o div.o main.c
target=app
$(target):$(src)
	$(CC) $(src) -o $(target)
sub.o:sub.c
	 gcc -c sub.c -o sub.o
	 
add.o:add.c
	  gcc -c add.c -o add.o
mult.o:mult.c
	  gcc -c mult.c -o mult.o
div.o:div.c
	gcc -c div.c -o div.o
main.o:main.c
	gcc -c main.c -o main.o
//用模式匹配就可以改变为
src=sub.o add.o mult.o div.o main.c
target=app
$(target):$(src)
	$(CC) $(src) -o $(target)
%.o:%.c
	$(CC) -c %< -o $@
```

#### 函数

```
1. $(wildcard PATTERN...)//获取指定目录下指定类型的文件列表
参数：PATTERN 指的是某个或多个目录下的对应的某种类型的文件，多个目录，使用空格隔开

$(wildcard *.c ./sub/*.c)//获取当前目录和sub目录下的所有.c文件

2. $(patsubst <pattern>,<replacement>,<text>)
	查找<text>中的单词，是否符合模式<pattern>,如果匹配的话，以<replacement>替换
	$(patsubst %.c,%.o,x.c,bar.c)
	 返回：x.o bar.o
	
```

## GDB调试

功能：

```
1.启动程序，可以按照自定义的要求随心所欲的运行程序。

2.可让被调试的程序在所指定的调置位置的断点处停止。

3.当程序被停止时，可以检查此时程序中所发生的事

4.可以改变程序，将一个BUG产生的影响修正从而测试其他BUG
```

通常在调试时，会关掉编译器的优化选项-o,并打开-g调试选项。gcc -g -Wall program.c -o program

### GDB的命令                                                 

1.启动、退出、查看代码

![](/run/media/root/8E9A022C9A0210FF/QQ图片20230717102548.jpg)

2.断点操作

![](/run/media/root/8E9A022C9A0210FF/QQ图片20230717102539.jpg)

3.调试命令

![](/run/media/root/8E9A022C9A0210FF/QQ图片20230717102602.jpg)

## 标准c库IO和Linux系统IO的关系

```
标准C库是跨平台的，它在Linux系统下使用IO函数时，底层就会调用Linux的系统IO,在windows下就会调用windows系统的IO函数。
标准C库的效率是比较高的，因为它是带有缓冲区的。如果没有缓存区，那么每次读写数据，都会与磁盘发生IO，但是磁盘的读写速度是远慢于CPU的运行速度的，所以就会降低效率。但是有缓冲区，程序就会把数据写入缓冲区，再一次写入磁盘中，提高了效率。降低了读写磁盘的次数。缓冲区的大小默认为8k

```

![](/run/media/root/8E9A022C9A0210FF/QQ图片20230717111342.jpg)



![](/run/media/root/8E9A022C9A0210FF/QQ图片20230717111351.jpg)

## Linux系统IO

```
没有缓冲区。
```

### 系统调用：

```
由操作系统实现并提供给外部应用程序的编程接口。是应用程序同系统之间数据交互的桥梁。
```



### 虚拟地址空间

```
每个运行的进程都有一个独立的内存地址空间，使得每个进程可以有一种错觉，好像它在独占地使用整个计算机系统的内存。32位-（4G）的空间，虚拟地址会被CPU上的MMU转换来映射到真实的物理地址上。
```

![](/run/media/root/8E9A022C9A0210FF/B75BFD2C04C01309595EA2095C483F1A.png)

为什么要有虚拟地址空间？

```
虚拟地址空间的主要目的是提供一种抽象层，将进程的内存地址空间与实际的物理内存分离开来。
1.内存隔离：每个进程都有自己独立的虚拟地址空间，进程之间的内存无法直接互相访问。这样可以增加系统的稳定性和安全性，防止一个进程的错误或恶意行为对其他进程或操作系统造成影响。

2.虚拟化：虚拟地址空间允许每个进程拥有超过实际物理内存大小的地址空间。这对于可以执行大型程序或需要大量内存的任务非常重要。

3.内存映射：虚拟地址空间允许进程通过将虚拟地址映射到物理内存上的位置，来访问实际的数据和指令。这使得操作系统能够有效地管理内存，根据需要将数据从磁盘加载到内存中，或将不再需要的数据从内存中交换出去。

4.内存保护：虚拟地址空间提供了对内存的保护机制，可以通过权限位和访问控制来限制进程对内存的访问。这有助于防止程序错误或恶意软件对系统的破坏。

```



### 文件描述符------用来定位磁盘上的某个文件

```
1.文件描述符在进程的内核区，由内核进行管理。文件描述表默认1024，所以一个进程默认同时能打开的文件个数位1024个。
2.文件描述符表，结构体PCB的成员变量file_struct*file指向文件描述表。
```

![](/run/media/root/8E9A022C9A0210FF/575DAB0C6340F6617BEE02B724137F22.png)

### open()

```c
 	   #include <sys/types.h>
       #include <sys/stat.h>
       #include <fcntl.h>

       int open(const char *pathname, int flags);//打开一个已经存在的文件
-pathname 文件路径
-flags 是通过 O_RDONLY, O_WRONLY 或 O_RDWR (指明文件是以只读,只写或读写方式打开的 这三个是必选项，必须包含其中之一）
 可选项：O_CREAT 若文件 不存在 将 创建 一个 新 文件. O_APPEND 以追加的模式打开 .....
-返回值  open() 调用成功，它会返回一个新的文件描述符（永远取未用描述符的最小值）。这个调用创建一个新的打开文件，即分配一个新的独一无 二的文件描述符，不会与运行中的任何其他程序共享（但可以通过 fork调用实现共享）失败返回-1 且设置错误号到errno
 
      int open(const char *pathname, int flags, mode_t mode)//创建一个新的文件
       -pathname：要创建的文件的路径
       -flags：对文件的操作权限和其他位置 必选项：O_RDONLY,O_WRONLY,O_RDWR 必选其中之一                                
       -mode:八进制的数，表示创建出的新的文件的操作权限，比如：0775                                 
                                        
      errno：属于linux系统函数库，库里的一个全局变量，记录的是最近的错误号
      
   void perror(const char *s);//打印errno对应的错误描述 参数s：用户描述
   perror("hello")	//对应输出hello:实际的错误描述
                                        
                                        
 

```

### read()

```c
ssize_t read(int fd, void *buf, size_t count);
-fd:文件描述符
-buf:读取数据存放的地方，数组的地址（传出参数）
-count：指定的数组的大小
 返回值：读取失败返回-1 并设置errno >0 实际读取到的字节数 =0 文件已经读完了
```

```
阻塞、非阻塞：读常规文件是不会阻塞的，不管读多少字节，read一定会在有限的时间内返回。但从终端设备或网络读就不一定，如果从终端输入的数据没有换行符，调用read读终端设备就会阻塞，如果网络上没有收到数据包，调用read从网络读就会阻塞。如果一直没有数据到达就一直阻塞在那里。
```



### write()

```c
 #include <unistd.h>

 ssize_t write(int fd, const void *buf, size_t count);
-fd:文件描述符
-buf:要写入数据的buf
-count：要写的实际大小
 返回值：读取失败返回-1 并设置errno >0 实际写入到的字节数 
 
```

lseek()-------文件读和写使用同一位置偏移量。

```c
 #include <sys/types.h>
 #include <unistd.h>

       off_t lseek(int fd, off_t offset, int whence);//用于在文件中定位当前读/写位置。指定文件指针
	    -fd:文件描述符
		-offset:偏移量，用于确定新的读写位置
		-whence:定位方式，指定偏移量offset是相对于何处计算的
   常用的 whence 定位方式有以下三种：
SEEK_SET：偏移量 offset 是相对于文件开头进行计算。
SEEK_CUR：偏移量 offset 是相对于当前读/写位置进行计算。
SEEK_END：偏移量 offset 是相对于文件末尾进行计算。
返回值：
成功时，返回新的读/写位置（相对于文件开头的字节偏移量）。
失败时，返回 -1，并设置全局变量 errno 表示错误代码。
            
lseek(fd,0,SEEK_SET);//移动文件指针到文件头
lseek(fd,0,SEEK_CUR);//获取当前文件指针的位置
lseek(fd,0,SEEK_END);//获取文件长度
lseek(fd,100,SEEK_END);//leesk允许超过文件结尾设置偏移量，文件因此会被扩展
```

stat()---用于获取指定文件的信息

```c
 #include <sys/stat.h>
       #include <unistd.h>

       int stat(const char *pathname, struct stat *statbuf);
	参数：
        pathname：文件路径名，表示要获取信息的文件。
        statbuf：struct stat 结构体指针，用于接收文件信息。
    返回值：
    成功时，返回 0。
    失败时，返回 -1，并设置全局变量 errno 表示错误代码。
       int fstat(int fd, struct stat *statbuf);
       int lstat(const char *pathname, struct stat *statbuf);//用于获取软连接文件的信息
```



### 文件属性操作函数

```c
int access(const char *pathname, int mode)//用于检查进程是否可以访问指定文件或目录

参数：
pathname：要检查访问权限的文件或目录的路径。
mode：要检查的权限模式，例如 R_OK（是否有读权限)、W_OK（是否有写权限）、X_OK ，F_OK(文件是否存在）等。
返回值：成功时返回0，失败时返回-1，并设置相应的错误码。

```

```c
int chmod(const char *pathname, mode_t mode)//修改文件的权限

参数：
pathname：要修改权限的文件或目录的路径。
mode：新的权限模式。
返回值：成功时返回0，失败时返回-1，并设置相应的错误码。
```

```c
int chown(const char *pathname, uid_t owner, gid_t group)//用于修改文件或目录的所有者和所属组。

参数：
pathname：要修改所有者和所属组的文件或目录的路径。
owner：新的用户 ID。
group：新的组 ID。
返回值：成功时返回0，失败时返回-1，并设置相应的错误码。
```

```c
int truncate(const char * path,off_t length)//修改文件的大小。
    path:文件路径
    length:最终文件变成的大小
  	返回值：成功0 失败-1
```

### 目录操作函数

```c
int chdir(const char *path)：该函数用于将当前的工作目录改变为指定的目录。参数path是一个字符串，表示要切换到的目录的路径。函数执行成功返回0，失败返回-1。

int mkdir(const char *path, mode_t mode)：该函数用于创建一个新的目录。参数path是一个字符串，表示要创建的目录的路径。参数mode用于设置新目录的权限。函数执行成功返回0，失败返回-1。

int rmdir(const char *path)：该函数用于删除一个空目录。参数path是一个字符串，表示要删除的目录的路径。函数执行成功返回0，失败返回-1。

DIR* opendir(const char *path)：该函数用于打开一个目录，返回一个指向DIR结构体的指针，该指针在后续的目录操作中会被使用。参数path是一个字符串，表示要打开的目录的路径。函数执行成功返回DIR指针，失败返回NULL。DIR* 类型为目录流

struct dirent *readdir(DIR *dirp)：该函数用于读取一个打开目录的下一个目录项。参数dirp是opendir函数返回的DIR指针。函数返回一个指向dirent结构体的指针，该结构体包含了目录项的信息。读取到目录末尾时，函数返回NULL。每调用一次，就读取一个目录的信息。

int closedir(DIR *dirp)：该函数用于关闭一个打开的目录。参数dirp是opendir函数返回的DIR指针。函数执行成功返回0，失败返回-1。
```

dup()

```c
int dup(int oldfd)：

函数功能：复制参数 oldfd 所指向的文件描述符，返回新的文件描述符。从空闲的文件描述符表中找一个最小的，作为新的拷贝文件描述符。
参数 oldfd：要复制的旧文件描述符。此时这两个文件描述符指向同一个文件。
返回值：成功时返回新的文件描述符，失败时返回 -1，并设置 errno。

int dup2(int oldfd, int newfd);//重定向文件描述符
//oldfd指向a.txt newfd指向b.txt 调用函数之后 newfd和b.txt做close，newfd指向a.txt
函数功能：将 newfd 指定的文件描述符替换为 oldfd 所指向的文件描述符。
参数 oldfd：要复制的旧文件描述符。
参数 newfd：要替换的新文件描述符。
返回值：成功时返回新的文件描述符（newfd），失败时返回 -1，并设置 errno。
这两个函数的共同点是都会创建一个新的文件描述符，该描述符与原描述符指向相同的文件或者管道。区别在于 dup 系列函数会将新文件描述符设置为进程中当前可用的最小非负整数，而 dup2 函数则允许显式地指定新描述符的数值。
```

fcntl()-------操作文件描述符，对于已经打开的文件描述符执行各种操作

```c
#include <fcntl.h>

int fcntl(int fd, int cmd, ...);
fd是文件描述符，cmd是要执行的操作命令，而可选的第三个参数则取决于cmd的值。
fcntl函数常见的操作命令包括：
F_DUPFD：复制一个现有的文件描述符。
F_GETFD：获取与文件描述符相关的文件描述符标志。
F_SETFD：设置与文件描述符相关的文件描述符标志。
F_GETFL：获取与文件描述符相关的文件状态标志，这个标志和我们通过open函数传递的flag是一个东西。
F_SETFL：设置与文件描述符相关的文件状态标志。必选项：O_RDONLY,O_WRONLY,O_RDWR不可以被修改，可选项：O_APPEDN,O_BLOCK(设置成非阻塞)

F_GETOWN：获取与文件描述符相关的进程或进程组ID。
F_SETOWN：设置与文件描述符相关的进程或进程组ID。
    //获取文件描述符状态flag
    int flag=fcntl(fd,F_GETFL);
	flag |=O_APPEND;
	//修改文件描述符状态的flag，给flag加入O_APPEND这个标记
	fcntl(fd,F_SETFL,flag);
```



## 进程



## **线程**

线程是轻量级的进程，**进程有独立的地址空间**，拥有PCB,线程有独立的PCB,但多个线程共享同一地址空间。线程是最小的执行单位，进程是最小的分配资源单位。线程的上下文切换比进程要快得多

上下文切换：进程 / 线程分时复用 CPU 时间片，在切换之前会将上一个任务的状态进行保存，下次切换回这个任务的时候，加载这个状态继续运行，任务从保存到再次加载这个过程就是一次上下文切换。

在一个地址空间中多个**线程独享**：每个线程都有属于自己的栈区，寄存器。

线程之间**共享的资源**包括但不限于以下几种： 

1. 全局变量：在程序中定义的全局变量可以被所有线程访问和修改。 

2. 静态变量：静态变量也可以被所有线程访问和修改。 

3.  动态分配的内存：如果多个线程都可以访问同一块动态分配的内存，那么它们可以通过该内存进行通信和数据交换。

4.  文件句柄：如果多个线程都需要访问同一个文件，那么它们可以共享该文件的句柄。 

5.  系统资源：例如网络连接、数据库连接等系统资源，多个线程可以共享这些资源。

###  线程函数

每一个线程都有一个唯一的线程 ID，**ID 类型为 pthread_t**，这个 ID 是一个无符号长整形数，如果想要得到当前线程的线程 ID，可以调用如下函数：

```c
pthread_t pthread_self(void);	// 返回当前线程的线程ID
```



### 线程的创建----------编译时要指定线程库文件  -lpthread

```c
#include <pthread.h>

int pthread_create(pthread_t *restrict thread,
                   const pthread_attr_t *restrict attr,
                   void *(*start_routine)(void *),
                   void *restrict arg);
/*
成功时返回 0 ，失败时返回 -1
thread : 保存新创建线程 ID 的变量地址值。线程与进程相同，也需要用于区分不同线程的 ID 传出参数
attr : 用于传递线程属性的参数，传递 NULL 时，创建默认属性的线程
start_routine : 函数指针，相当于线程 main 函数的、在单独执行流中执行的函数地址值（函数指针）
arg : 通过第三个参数传递的调用函数时包含传递参数信息的变量地址值
*/
//创建过程中一定要保证编写的线程函数与规定的函数指针类型一致void *(*start_routine) (void *)
void* thread_main(void *arg)
{
    printf("我是子进程，线程ID：%ld\n",pthread_self());
    for(int i=0;i<9;i++)
    {
        printf("child=i:=%d\n",i);
    }
    return NULL;
}
int main()
{
    pthread_t tid;
    pthread_create(&tid,NULL,thread_main,NULL);
    printf("子线程创建成功，线程ID:%ld\n",tid);
    printf("我是主线程，线程ID:%ld\n",pthread_self());
    
    return 0;
}
/*
此时会可能出现子线程中的函数没有执行完，主线程一直在运行，执行期间创建出了子线程，说明主线程有 CPU 时间片，在这个时间片内将代码执行完毕了，主线程就退出了。子线程被创建出来之后需要抢cpu时间片, 抢不到就不能运行，如果主线程退出了, 虚拟地址空间就被释放了, 子线程就一并被销毁了。但是如果某一个子线程退出了, 主线程仍在运行, 虚拟地址空间依旧存在。

在没有人为干预的情况下，虚拟地址空间的生命周期和主线程是一样的，与子线程无关。当主线程执行完，地址空间被释放，子线程就被销毁了
*/
```

### 线程退出

exit()函数表示整个进程退出，return 返回到调用者去，要使线程退出使用 pthread_exit() 函数.在多线程环境中应尽量少用或者不使用exit函数。

**主线程退出其他线程不退出**，主线程调用pthread_exit（），不会导致虚拟地址空间的释放，并且不会影响到其他线程的正常运行。

```c
#include <pthread.h>
void pthread_exit(void *retval);//retval 表示线程退出状态，线程退出时携带的数据，通常传NULL
```

```c

void* thread_main(void* arg)
{
    printf("我是子线程, 线程ID: %ld\n", pthread_self());
    for(int i=0; i<9; ++i)
    {
        if(i==6)
        {
            pthread_exit(NULL);	// 直接退出子线程
        } 
        printf("child == i: = %d\n", i);
    }
    return NULL;
}

int main()
{
    pthread_t tid;
    pthread_create(&tid,NULL,thread_main,NULL);
    printf("子线程创建成功, 线程ID: %ld\n", tid);
    // 2. 子线程不会执行下边的代码, 主线程执行
    printf("我是主线程, 线程ID: %ld\n", pthread_self());
    for(int i=0; i<3; ++i)
    {
        printf("i = %d\n", i);
    }

    // 主线程调用退出函数退出, 地址空间不会被释放
    pthread_exit(NULL);

}
```



### 线程回收

### pthread_join函数

线程和进程一样，**子线程退出的时候其内核资源主要由主线程回收**，线程库中提供的线程回收函叫做 pthread_join()，这个函数是一个阻塞函数，如果还有子线程在运行，调用该函数就会阻塞，子线程退出函数解除阻塞进行资源的回收，**函数被调用一次，只能回收一个子线程，如果有多个子线程则需要循环进行回收。**

作用就是调用该函数的进程（或线程）将进入等待状态，直到第一个参数为 ID 的线程终止为止。而且可以得到线程的 main 函数的返回值

```c
#include <pthread.h>
int pthread_join(pthread_t thread, void **status);
/*
成功时返回 0 ，失败时返回 -1
thread :要回收的子线程ID 该参数值 ID 的线程终止后才会从该函数返回
status :二级指针，指向一级指针的地址，是一个传出参数，这个地址中存储了 pthread_exit () 传递出的数据，如果不需要这个参数，可以指定为 NULL 
*/
```

### 回收子线程数据-------通过pthread_exit()和pthread_join()搭配使用

在子线程退出的时候可以使用 pthread_exit() 的参数将数据传出，在回收这个子线程的时候可以通过 phread_join() 的第二个参数来接收子线程传递出的数据。

1.使用全局变量回收子线程的数据

```c
位于同一虚拟地址空间中的线程，虽然不能共享栈区数据，但是可以共享全局数据区和堆区数据，因此在子线程退出的时候可以将传出数据存储到全局变量、静态变量或者堆内存中
   struct Person
   {
       int id;
       char name[36];
       int age;
   };
struct Person p;//定义全局变量
//子线程处理代码
void* thread_main(void* arg)
{
    printf("子线程ID:%ld\n",pthread_self());
    //使用全局变量
    p.age=12;
    strcpy(p.name,"tom");
    p.id=100;
    //该函数的参数将这个地址传递给了主线程的pthread_join()
    pthread_exit(&p);//子线程退出
    
    return NULL;//此代码不执行
}
int main()
{
    pthread_t tid;
    pthread_create(&tid,NULL,thread_main,NULL);
    printf("子线程创建成功，线程ID：%ld\n",tid);
    //子线程不会执行下面的代码
    // ptr是一个传出参数, 在函数内部让这个指针指向一块有效内存
    // 这个内存地址就是pthread_exit() 参数指向的内存
    void* ptr=NULL;
    //等待子线程退出
    pthread_join(tid,&ptr);
    struct Person *p=(strcut Person*)ptr;//强制类型转换
    printf("name:%S,age:%d,id:%d\n",p->name,p->age,p->id);
    return 0;
        
    
}
```

2.使用主线程栈

虽然每个线程都有属于自己的栈区空间**，但是位于同一个地址空间的多个线程是可以相互访问对方的栈空间上的数据的**。由于很多情况下还需要在主线程中回收子线程资源，所以主线程一般都是最后退出，基于这个原因在下面的程序中将子线程返回的数据保存到了主线程的栈区内存中：

```c
struct Persion
{
    int id;
    char name[36];
    int age;
};
void* thread_main(void* arg)
{
    struct Person*p=(struct Person*)arg;
    //使用主线程的栈内存
    p->age=12;
    strcpy(p->name,"tom");
    p->id=100;
    //将p的地址传递给主线程的pthrea_join()
    pthread_exit(p);
    return NULL;
}
int main()
{
    pthread_t tid;
    struct Person p;
    //主线程的栈内存中变量p的地址传递给子线程
    pthread_create(&tid,NULL,thread_main,&p);
    void *ptr=NULL;
    pthread_join(tid,&ptr);
     // 打印信息
    printf("name: %s, age: %d, id: %d\n", p.name, p.age, p.id);
    printf("子线程资源被成功回收...\n");
    
    return 0;

}
```



### 线程分离

在某些情况下，程序中的主线程有属于自己的业务处理流程，**如果让主线程负责子线程的资源回收，调用 pthread_join() 只要子线程不退出主线程就会一直被阻塞，**主要线程的任务也就不能被执行了。

在线程库函数中为我们提供了线程分离函数 **pthread_detach()，调用这个函数之后指定的子线程就可以和主线程分离，当子线程退出的时候，其占用的内核资源就被系统的其他进程接管并回收了。**线程分离之后在主线程中使用 pthread_join() 就回收不到子线程资源了。

```c
#include <pthread.h>
int pthread_detach(pthread_t th); //线程终止，自动清理，无需回收
/*
成功时返回 0 ，失败时返回其他值
thread : 终止的同时需要销毁的线程 ID
*/
```

调用上述函数不会引起线程终止或进入阻塞状态，可以通过该函数引导销毁线程创建的内存空间。调用该函数后不能再针对相应线程调用 pthread_join 函数。

```c
// 子线程的处理代码
void* working(void* arg)
{
    printf("我是子线程, 线程ID: %ld\n", pthread_self());
    for(int i=0; i<9; ++i)
    {
        printf("child == i: = %d\n", i);
    }
    return NULL;
}

int main()
{
    // 1. 创建一个子线程
    pthread_t tid;
    pthread_create(&tid, NULL, working, NULL);

    printf("子线程创建成功, 线程ID: %ld\n", tid);
    // 2. 子线程不会执行下边的代码, 主线程执行
    printf("我是主线程, 线程ID: %ld\n", pthread_self());
    for(int i=0; i<3; ++i)
    {
        printf("i = %d\n", i);
    }

    // 设置子线程和主线程分离
    pthread_detach(tid);

    // 让主线程自己退出即可
    pthread_exit(NULL);
    
    return 0;
}

```



### 线程属性设置分离线程

```c
#include <pthread.h>
       int pthread_attr_init(pthread_attr_t *attr);//初始化线程属性
		pthread_attr_getinheritsched（&attr,PTHREAD_CREATE_DETACHED）//设置线程属性为分离
          pthread_create(&tid,&attr,NULL,NULL);//借助修改后的线程属性 创建线程
       int pthread_attr_destroy(pthread_attr_t *attr);//销毁线程属性，使其恢复成默认状态

```



### 杀死线程

线程取消的意思就是在某些特定情况下在一个线程中杀死另一个线程。使用这个函数杀死一个线程需要分两步：

1.在线程 A 中调用线程取消函数 pthread_cancel，指定杀死线程 B，这时候线程 B 是死不了的
2.在线程 B 中进程一次系统调用（从用户区切换到内核区），否则线程 B 可以一直运行。。

```c
 #include <pthread.h>

 int pthread_cancel(pthread_t thread);
 //参数：要杀死的线程的线程 ID
 //返回值：函数调用成功返回 0，调用失败返回非 0 错误号。

// 子线程的处理代码
void* working(void* arg)
{
    int j=0;
    for(int i=0; i<9; ++i)
    {
        j++;
    }
    // 这个函数会调用系统函数, 因此这是个间接的系统调用
    printf("我是子线程, 线程ID: %ld\n", pthread_self());
    for(int i=0; i<9; ++i)
    {
        printf(" child i: %d\n", i);
    }

    return NULL;
}

int main()
{
    // 1. 创建一个子线程
    pthread_t tid;
    pthread_create(&tid, NULL, working, NULL);

    printf("子线程创建成功, 线程ID: %ld\n", tid);
    // 2. 子线程不会执行下边的代码, 主线程执行
    printf("我是主线程, 线程ID: %ld\n", pthread_self());
    for(int i=0; i<3; ++i)
    {
        printf("i = %d\n", i);
    }

    // 杀死子线程, 如果子线程中做系统调用, 子线程就结束了
    pthread_cancel(tid);

    // 让主线程自己退出即可
    pthread_exit(NULL);



```

### 线程同步

当多个线程操作同一共享资源时，多个线程是在分时复用cpu时间片，这时如果一个线程的cpu时间片用完，数据还没来得及写回内存中，这时数据就暂时保存到了寄存器中，此时另一个线程来访问内存读取的数据是之前的数据，数据还没同步到内存中，这样就会遭成数据的混乱。

同步即协同步调，按预定的先后次序运行。**保证数据的一致性**。 

线程同步，指一个线程发出某一功能调用时，在没有得到结果之前，该调用不返回，同时其他线程为其保证数据的一致性，不能调用该功能。**所以有多个控制流，共同操纵一个共享资源的情况，都需要同步。**

**常用的线程同步的方法有：互斥锁、读写锁、条件变量、信号量。**

![](/run/media/root/8E9A022C9A0210FF/IMG_1214.PNG)

找到临界资源之后，再找和临界资源相关的上下文代码，这样就得到了一个代码块，这个代码块可以称之为临界区。确定好临界区（临界区越小越好）之后，就可以进行线程同步了，线程同步的大致处理思路是这样的：

在临界区代码的上边，添加加锁函数，对临界区加锁。
哪个线程调用这句代码，就会把这把锁锁上，其他线程就只能阻塞在锁上了。
在临界区代码的下边，添加解锁函数，对临界区解锁。
出临界区的线程会将锁定的那把锁打开，其他抢到锁的线程就可以进入到临界区了。
通过锁机制能保证临界区代码最多只能同时有一个线程访问，这样并行访问就变为串行访问了。

### 1.互斥量Mutex(互斥锁)

每个线程在对资源进行操作前都先将其加锁，成功加锁才能操作，操作结束解锁。所有线程只能顺序执行（不能并行处理），需要付出的代价是执行效率的降低，只能串行处理。

互质锁函数：

```c
1.pthread_mutex_t  lock;//创建锁 全局变量
在创建的锁对象中保存了当前这把锁的状态信息：锁定还是打开，如果是锁定状态还记录了给这把锁加锁的线程信息（线程 ID）。一个互斥锁变量只能被一个线程锁定，被锁定之后其他线程再对互斥锁变量加锁就会被阻塞，直到这把互斥锁被解锁，被阻塞的线程才能被解除阻塞。
   // 一般情况下，每一个共享资源对应一个把互斥锁，锁的个数和线程的个数无关。

2.pthread_mutex_init(pthread_mutex_t *restrict mutex,const pthread_mutexattr_t *restrict attr); //初始化互斥锁 mutex 互斥锁变量的地址，attr 互斥锁变量的属性，一般默认NULL

3.pthread_mutex_lock（pthread_mutex_t *mutex）;/*
加锁 当这个函数被调用时，首先会判断参数mutex互斥锁中的状态锁不是被锁定：1,如果没有被锁定，调用这个函数的线程加锁成功。这个锁会记录是哪个线程加锁成功了。2.如果被锁定了，调用这个函数尝试加锁的线程就会加锁失败被阻塞到这把锁上，当这把锁被解开之后，这些阻塞在锁上的线程就解除阻塞了，并且这些线程是通过竞争的方式对这把锁加锁，没有抢到锁的线程继续阻塞。
*/


4.pthread_mutex_unlock(pthread_mutex_t *mutex);//解锁,不是所有线程都可以对互斥锁解锁，哪个线程加的哪个线程解锁
5.pthread_mutex_destroy(pthread_mutex_t *mutex);//销毁锁

6.int pthread_mutex_trylock(pthread_mutex_t *mutex);//尝试加锁
/*
1.如果这把锁没有被锁定锁打开的，线程加锁成功
2.如果锁变量被锁住了，调用这个函数的线程并不会被阻塞，加锁失败返回错误号。
*/
```

在访问共享资源前加锁，访问结束后立即解锁。**锁的粒度应该越小越好**。

```c

#define MAX 50
int number;
pthread_mutex_t mutex;//创建一把互斥锁，多个线程共享

void* funA_num(void* arg)
{
    for(int i=0;i<MAX;++i)
    {

    //如果线程A加锁成功，不阻塞，线程B加锁成功，此时线程A阻塞
        pthread_mutex_lock(&mutex);
        //临界区
        int cur=number;
        cur++;
        usleep(10);
        number=cur;
        printf("Thread A, id=%lu,number=%d\n",pthread_self(),number);
        //临界区边界
        pthread_mutex_unlock(&mutex);//解锁

    }
    return NULL;

}

void* funB_num(void* arg)
{
    for(int i=0;i<MAX;++i)
    {
        pthread_mutex_lock(&mutex);
        //临界区
        int cur=number;
        cur++;
        number=cur;
        printf("Thread B, id=%lu,number=%d\n",pthread_self(),number);
        usleep(5);
        //临界区边界
        pthread_mutex_unlock(&mutex);//解锁


    }
    return NULL;
}
int main(int argc,const char* argv[])
{
    pthread_t tid1,tid2;

    pthread_mutex_init(&mutex,NULL);//初始化锁

    pthread_create(&tid1,NULL,funA_num,NULL);
    pthread_create(&tid2,NULL,funB_num,NULL);
    //阻塞，资源回收
    pthread_join(tid1,NULL);
    pthread_join(tid2,NULL);

    //线程销毁后再去释放互斥锁
    pthread_mutex_destroy(&mutex);

    return 0;

}

```



#### 死锁

1.反复加锁

```c
int main()
{
	pthread_mutex_t lock;
    pthread_mutex_init(&lock,NULL);
    pthread_mutex_lock(&lock);//加锁
    
    //访问共享数据
    pthread_mutex_lock(&lock);//反复加锁,此时程序就会阻塞在当前线程等待该线程释放资源 但是该线程已经阻塞就会出现死锁 程序一直阻塞在此线程
    pthread_mutex_unlock(&lock);
    
    pthread_mutex_unlock(&lock);
}


```

2.两个线程阻塞等待对方释放资源 造成死锁

```
场景描述:
  1. 有两个共享资源:X, Y，X对应锁A, Y对应锁B
     - 线程A访问资源X, 加锁A
     - 线程B访问资源Y, 加锁B
  2. 线程A要访问资源Y, 线程B要访问资源X，因为资源X和Y已经被对应的锁锁住了，因此这个两个线程被阻塞
     - 线程A被锁B阻塞了, 无法打开A锁
     - 线程B被锁A阻塞了, 无法打开B锁
```

![](/run/media/root/8E9A022C9A0210FF/IMG_1215(20230708-175931).PNG)

```c
// 两个线程各自持有一把锁，请求另一把
int a;//共享资源1
int b;//共享资源2
pthread_mutex_t lock1;
pthread_mutex_t lock2;
void* thread1_main(void*)
{
	pthread_mutex_lock(&lock1);
    a=10;//访问共享资源1
    sleep(5);
    b=20;//访问共享资源2 该线程就会阻塞等待另一线程释放b的资源
    pthread_mutex_unlock(&lock1);
    
    return NULL:
}

int main()
{
	pthread_t tid;
    pthread_create(&tid,NULL,thread1_main,NULL);
    pthread_lock(&lock2);
    b=10;//访问共享资源2
    a=10;//该线程就会等待线程1释放资源
    pthread_unlcok(&lock2);
}
```

3.加锁后忘记解锁

```c
// 场景1
void func()
{
    for(int i=0; i<6; ++i)
    {
        // 当前线程A加锁成功, 当前循环完毕没有解锁, 在下一轮循环的时候自己被阻塞了
        // 其余的线程也被阻塞
    	pthread_mutex_lock(&mutex);
    	....
    	.....
        // 忘记解锁
    }
}

// 场景2
void func()
{
    for(int i=0; i<6; ++i)
    {
        // 当前线程A加锁成功
        // 其余的线程被阻塞
    	pthread_mutex_lock(&mutex);
    	....
    	.....
        if(xxx)
        {
            // 函数退出, 没有解锁（解锁函数无法被执行了）
            return ;
        }
        
        pthread_mutex_lock(&mutex);
    }
}

```



### 2.读写锁

读写锁是互斥锁的升级版，在做读操作的时候可以提高程序的执行效率，**如果所有线程都是读操作，那么读是并行。**

**锁只有一把**，读共享，写独占，写锁优先级高，当读写同时抢夺资源时 写优先 。

相较于互斥量而言，当读线程多的时候，提供访问效率

> ```c
> pthread_rwlock_t rwlock;//读写锁 只有一把 全局
> /*
> 锁中记录了这些信息：
> 锁的状态：锁定 / 打开
> 锁定的是什么操作：读操作 / 写操作，使用读写锁锁定了读操作，需要先解锁才能去锁定写操作，反之亦然。
> 哪个线程将这把锁锁上了
> */
> 因为通过一把读写锁可以锁定读或者写操作，下面介绍一下关于读写锁的特点：
> 
> 使用读写锁的读锁锁定了临界区，线程对临界区的访问是并行的，读锁是共享的。
> 使用读写锁的写锁锁定了临界区，线程对临界区的访问是串行的，写锁是独占的。
> 使用读写锁分别对两个临界区加了读锁和写锁，两个线程要同时访问者两个临界区，访问写锁临界区的线程继续运行，访问读锁临界区的线程阻塞，因为写锁比读锁的优先级高。
> 如果说程序中所有的线程都对共享资源做写操作，使用读写锁没有优势，和互斥锁是一样的，如果说程序中所有的线程都对共享资源有写也有读操作，并且对共享资源读的操作越多，读写锁更有优势。
> 
> pthread_rwlock_init(); 
> pthread_rwlock_destory();
> pthread_rwlock_unlock();//解锁，不管是读还是写都可用此解锁
> ```
>

```c
int pthread_rwlock_rdlock();//以读方式进行加锁,如果读写锁锁定了读操作，那么调用该函数的线程依旧可以加锁成功，如果读写锁已经锁定了写操作，那么调用这个函数的线程会被阻塞。

```

```c
int pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock);// 这个函数可以有效的避免死锁 如果加读锁失败, 不会阻塞当前线程, 直接返回错误号

```

```c
int pthread_rwlock_wrlock();//以写方式进行加锁，如果读写锁已经锁定了读或者写状态，调用这个函数的线程会被阻塞
```

```c
// 这个函数可以有效的避免死锁
// 如果加写锁失败, 不会阻塞当前线程, 直接返回错误号
int pthread_rwlock_trywrlock(pthread_rwlock_t *rwlock);
```

```c
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<string.h>
#include<pthread.h>

int number=0;
//创建读写锁
pthread_rwlock_t rwlock;
//写的线程的函数
void* writeNum(void* arg)
{
    while(1)
    {
        pthread_rwlock_wrlock(&rwlock);
        int cur=number;
        cur++;
        number=cur;
        printf("++写操作完成，number：%d,tid=%ld\n",number,pthread_self());
pthread_rwlock_unlock(&rwlock);
        usleep(rand()%100);
    }
    return NULL;
}
//读的线程的函数
void* readNum(void*arg)
{
    while (1) {
        pthread_rwlock_rdlock(&rwlock);
        printf("全局变量number：%d,tid=%ld\n",number,pthread_self());
        pthread_rwlock_unlock(&rwlock);
        usleep(rand()%100);
    }
    return NULL;
}


int main(int argc,const char* argv[])
{

    //初始化读写锁
    pthread_rwlock_init(&rwlock,NULL);
    //创建三个写线程，5个读线程
    pthread_t wtid[3];
    pthread_t rtid[5];
    for(int i=0;i<3;++i)
    {
        pthread_create(&wtid[i],NULL,writeNum,NULL);
    }
    for(int i=0;i<5;++i)
    {
        pthread_create(&rtid[i],NULL,readNum,NULL);
    }
    //释放资源
    for(int i=0;i<3;++i)
    {
        pthread_join(wtid[i],NULL);
    }
    for(int i=0;i<5;++i)
    {
        pthread_join(rtid[i],NULL);
    }

    //销毁读写锁
    pthread_rwlock_destroy(&rwlock);

    return 0;

}

```



### 3.条件变量------适用于生产者消费者模式

条件变量本身不是锁，但它也可以造成线程阻塞，通常与互斥锁配合使用，给多个线程提供一个会合的场所。

**如果在多线程程序中只使用条件变量无法实现线程的同步，必须要配合互斥锁来使用。**虽然条件变量和互斥锁都能阻塞线程，但是二者的效果是不一样的，二者的区别如下：

1.假设有 A-Z 26 个线程，这 26 个线程共同访问同一把互斥锁，如果线程 A 加锁成功，那么其余 B-Z 线程访问互斥锁都阻塞，所有的线程只能顺序访问临界区
2.**变量只有在满足指定条件下才会阻塞线程**，如果条件不满足，多个线程可以同时进入临界区，同时读写临界资源，这种情况下还是会出现共享资源中数据的混乱。
一般情况下条件变量用于处理生产者和消费者模型，并且和互斥锁配合使用。

```c
pthread_cond_t cond;//定义条件变量 被条件变量阻塞的线程的信息会被记录到这个变量中，以便在解除阻塞的时候适用

pthread_cond_init(pthread_cond_t restrict* cond,const pthread_condattr_t *restrict attr);

pthread_cond_t cond=PTHRREAD_COND_INITIALIZR;//也可使用宏初始化
pthread_cond_destory();

int pthread_cond_wait(pthread_cond_t *restrict cond,
   pthread_mutex_t *restrict mutex);
该函数会对这个互斥锁做以下几件事情：
1.阻塞等待条件变量cond满足（通过pthread_cond_signal()或者pthread_cond_broadcast()来唤醒）。
2.在阻塞线程时候，如果线程已经对互斥锁 mutex 上锁，那么会将这把锁打开，这样做是为了避免死锁（1、2步为原子操作一步完成）
3.当线程解除阻塞的时候，函数内部会帮助这个线程再次将这个 mutex 互斥锁锁上，继续向下访问临界区

  int pthread_cond_timedwait(pthread_cond_t *restrict cond,
           pthread_mutex_t *restrict mutex,
           const struct timespec *restrict abstime);
// 表示的时间是从1971.1.1到某个时间点的时间, 总长度使用秒/纳秒表示
struct timespec {
	time_t tv_sec;      /* Seconds */
	long   tv_nsec;     /* Nanoseconds [0 .. 999999999] */
};
time_t mytime=time(NULL);//1970.1.1 0:0:0 到当前的总秒数
struct timespec tmsp;
tmsp.tv_nsec=0;
tmsp.tv_sec=time(NULL)+100;//线程阻塞100s

pthread_cond_signal();//唤醒阻塞在条件变量上的一个线程
pthread_cond_broadcast();//唤醒阻塞在条件变量上的所有线程
```

### 生产者消费者模型

```c
#include <cstdlib>
#include<stdio.h>
#include<pthread.h>
#include<unistdio.h>

//链表作为共享数据
struct msg{
    struct msg *next;
    int num;
};
struct msg *head;

//静态初始化一个条件变量和一个互斥量

pthread_mutex_t lock=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t hasproduct=PTHREAD_COND_INITIALIZER;

void* consumer(void* arg)
{
    struct msg *mp;
    for(;;)
    {
        pthread_mutex_lock(&lock);
        while(head==NULL)//头指针为空 说明没有节点一定要循环判断
        {
            pthread_cond_wait(&hasproduct,&lock);//阻塞等待，并解锁unlock 
            //函数返回重新加锁
        }
        mp=head;
        head=mp->next;//消费一个产品
        pthread_mutex_unlock(&lock);
        free(mp);
        sleep(rand()%5);

    }
    return NULL;

}
void *producer(void* arg)
{

    struct msg *mp;
    for(;;)
    {
        mp=malloc(sizeof(struct msg));
        mp->num=rand()%1000+1;//生产一个产品

        pthread_mutex_lock(&lock);
        mp->next=head;//放入共享区
        head=mp;
        pthread_mutex_unlock(&lock);

        pthread_cond_signal(&hasproduct);//将等待在该条件变量的一个线程唤醒
        
        sleep(rand()%5);
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    pthread_t pid,cid;
	srand();
    pthread_create(&pid,NULL,producer,NULL);//生产者
    pthread_create(&cid,NULL,consumer,NULL);//消费者

    pthread_join(pid,NULL);
    pthread_join(cid,NULL);
    return 0;
}

```



### 4.信号量<semaphore.h>

可以应用用线程和进程，信号量用在多线程多任务同步的，一个线程完成了某一个动作就通过信号量告诉别的线程，别的线程再进行某些动作。信号量不一定是锁定某一个资源，而是流程上的概念，比如：有 A，B 两个线程，B 线程要等 A 线程完成某一任务以后再进行自己下面的步骤，这个任务并不一定是锁定某一资源，还可以是进行一些计算或者数据处理之类。

信号量（信号灯）与互斥锁和条件变量的主要不同在于” 灯” 的概念，灯亮则意味着资源可用，灯灭则意味着不可用。**信号量主要阻塞线程，不能完全保证线程安全，如果要保证线程安全，需要信号量和互斥锁一起使用。**

**信号量和条件变量一样用于处理生产者和消费者模型**，用于阻塞生产者线程或者消费者线程的运行。

相当于初始化值为N的互斥量.信号量的初值，决定了占用信号量的线程的个数。

```c
sem_t sem;
int sem_init(sem_t *sem, int pshared, unsigned int value);//pthshared 0 用于线程间同步 1 用于进程间同步 value N值 value=0 线程就会被阻塞 信号量的初值决定了占用信号量的线程的个数
sem_destroy();
sem_wait();//一次调用 做一次--操作，当信号量的值为0时，再次--就会阻塞

int sem_trywait(sem_t *sem);// 函数被调用sem中的资源就会被消耗1个, 资源数-1
当线程调用这个函数，并且 sem 中的资源数 >0，线程不会阻塞，线程会占用 sem 中的一个资源，因此资源数 - 1，直到 sem 中的资源数减为 0 时，资源被耗尽，但是线程不会被阻塞，直接返回错误号，因此可以在程序中添加判断分支，用于处理获取资源失败之后的情况。

sem_post();//一次调用 做一次++ 操作 当信号量的值为n时，再次++就会阻塞

```

```c
// 表示的时间是从1971.1.1到某个时间点的时间, 总长度使用秒/纳秒表示
struct timespec {
	time_t tv_sec;      /* Seconds */
	long   tv_nsec;     /* Nanoseconds [0 .. 999999999] */
};
// 调用该函数线程获取sem中的一个资源，当资源数为0时，线程阻塞，在阻塞abs_timeout对应的时长之后，解除阻塞。
// abs_timeout: 阻塞的时间长度, 单位是s, 是从1970.1.1开始计算的
int sem_timedwait(sem_t *sem, const struct timespec *abs_timeout);
该函数的参数 abs_timeout 和 pthread_cond_timedwait 的最后一个参数是一样的，使用方法不再过多赘述。当线程调用这个函数，并且 sem 中的资源数 >0，线程不会阻塞，线程会占用 sem 中的一个资源，因此资源数 - 1，直到 sem 中的资源数减为 0 时，资源被耗尽，线程被阻塞，当阻塞指定的时长之后，线程解除阻塞。
```

```c
// 查看信号量 sem 中的整形数的当前值, 这个值会被写入到sval指针对应的内存中
// sval是一个传出参数
int sem_getvalue(sem_t *sem, int *sval);
通过这个函数可以查看 sem 中现在拥有的资源个数，通过第二个参数 sval 将数据传出，也就是说第二个参数的作用和返回值是一样的。
```



#### 生产者消费者

```c
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<string.h>
#include<pthread.h>
#include<semaphore.h>

int NUM=5;
//定义信号量
sem_t product_number;
sem_t blank_number;
//定义互斥锁
pthread_mutex_t mutex;

void* product(void* arg)
{
    while(1)
    {
        sem_wait(&blank_number);//生产一个使空位减1
        //生产一个产品

        pthread_mutex_lock(&mutex);
            //访问共享区域
        pthread_mutex_unlock(&mutex);
        sem_post(&product_number);//生产产品使product_number+1，将阻塞等待在product_number上的消费者唤醒

    }
    return NULL;
}


void* consume(void* arg)
{
    while(1)
    {
        sem_wait(&product_number);//消耗一个产品，没有产品就阻塞等待生产者生产后将其唤醒

        pthread_mutex_lock(&mutex);
            //访问共享区域
           //消费一个产品
        pthread_mutex_unlock(&mutex);
        sem_post(&blank_number);//将blank_number+1 将阻塞在blank_number上的生产者线程唤醒
    }
    return NULL;
}


int main(int argc,const char* argv[])
{
    //初始化信号量
    sem_init(&product_number,0,0);//初始化消费者的资源为0
    sem_init(&blank_number,0,5);//初始化生产者的可以生产的空位为5
    //初始化互斥锁
    pthread_mutex_init(&mutex,NULL);



    //创建线程
    pthread_t producer,consumer;
    pthread_create(&producer,NULL,product,NULL);
    pthread_create(&consumer,NULL,consume,NULL);

    //阻塞，资源回收
    pthread_join(producer,NULL);
    pthread_join(consumer,NULL);

    //线程销毁再去释放互斥锁
    pthread_mutex_destroy(&mutex);
    sem_destroy(&product_number);
    sem_destroy(&blank_number);

    return 0;
}
```



### 线程池

#### 1.线程池原理

我们使用线程的时候就去创建一个线程，这样实现起来非常简便，但是就会有一个问题：如果并发的线程数量很多，并且每个线程都是执行一个时间很短的任务就结束了，这样频繁创建线程就会大大降低系统的效率，因为频繁创建线程和销毁线程需要时间。

那么有没有一种办法**使得线程可以复用，就是执行完一个任务，并不被销毁，而是可以继续执行其他的任务**呢？

线程池是一种多线程处理形式，处理过程中将任务添加到队列，然后在创建线程后自动启动这些任务。线程池线程都是后台线程。每个线程都使用默认的堆栈大小，以默认的优先级运行，并处于多线程单元中。如果某个线程在托管代码中空闲（如正在等待某个事件）, 则线程池将插入另一个辅助线程来使所有处理器保持繁忙。如果所有线程池线程都始终保持繁忙，但队列中包含挂起的工作，则线程池将在一段时间后创建另一个辅助线程但线程的数目永远不会超过最大值。超过最大值的线程可以排队，但他们要等到其他线程完成后才启动。

线程池的组成主要分为3个部分，这三个部分配合工作就可以得到一个完整的线程池：

1.**任务队列**，存储需要处理的任务，由工作的线程来处理这些任务

- 通过线程池提供的 API 函数，将一个待处理的任务添加到任务队列，或者从任务队列中删除
- 已处理的任务会被从任务队列中删除
- 线程池的使用者，也就是调用线程池函数往任务队列中添加任务的线程就是生产者线程

2.**工作的线程**（任务队列任务的消费者） ，N个

- 线程池中维护了一定数量的工作线程，他们的作用是是不停的读任务队列，从里边取出任务并处理
- 工作的线程相当于是任务队列的消费者角色，如果任务队列为空，工作的线程将会被阻塞 (使用条件变量 / 信号量阻塞)，如果阻塞之后有了新的任务，由生产者将阻塞解除，工作线程开始工作

3.**管理者线程**（不处理任务队列中的任务），1个

- 它的任务是周期性的对任务队列中的任务数量以及处于忙状态的工作线程个数进行检测
- 当任务过多的时候，可以适当的创建一些新的工作线程
- 当任务过少的时候，可以适当的销毁一些工作的线程





##  **高并发服务器** 

### 多进程并发服务器的实现

```c++
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<signal.h>
#include<string.h>
#include<wait.h>


#define BUF_SIZE 30

void read_Childproc(int sig);//信号处理函数

void sys_err(const char *str)
{
    perror(str);
    exit(1);
}

int main(int argc,char* argv[])
{
    int fd,cfd;
    pid_t pid;
    char buf[BUF_SIZE];
    int str_len;
    int fds[2];
    struct sockaddr_in ser_addr,client_addr;
    socklen_t client_addr_size=sizeof(client_addr);
    fd=socket(AF_INET,SOCK_STREAM,0);
    if(fd==-1)
        sys_err("socket() error");
    ser_addr.sin_family=AF_INET;
    ser_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    ser_addr.sin_port=htons(atoi(argv[1]));

    struct sigaction act;
    act.sa_handler=read_Childproc;//信号处理函数的地址
    sigemptyset(&act.sa_mask);
    act.sa_flags=0;
    int state;
    state=sigaction(SIGCHLD,&act,0);//注册信号

    if(bind(fd,(struct sockaddr*)&ser_addr,sizeof(ser_addr))==-1)
        sys_err("bind() error");
    if(listen(fd,5)==-1)
        sys_err("listen error");

    //另创建进程，从向客户端提供的进程读取字符信息，保存到文件中，通过管道进行进程间的通信
    pipe(fds);
    pid=fork();//创建子进程
    if(pid==0)//将客户端传输的字符串保存到文件中
    {
        FILE *fp=fopen("echomsg.txt","wt");
        char msgbuf[BUF_SIZE];
        int i,len;
        for(i=0;i<10;i++)
        {
            len=read(fds[0],msgbuf,BUF_SIZE);
            fwrite((void*)msgbuf,1,len,fp);
        }
        fclose(fp);
        return 0;
    }
    while(1)
    {
        cfd=accept(fd,(struct sockaddr*)&client_addr,&client_addr_size);
        if(fd==-1)
            continue;
        else
            puts("new client connected");
        pid=fork();//创建子进程用来和客户端进行通信
        if(pid==0)
        {
            close(fd);
           while((str_len=read(cfd,buf,BUF_SIZE))!=0)
           {
               write(fd,buf,str_len);
               write(fds[1],buf,str_len);
           }
           close(cfd);
           puts("client disconnect...");
           return 0;
        }
        else//父进程
        {
            close(cfd);
        }
    }
    close(fd);

    return 0;
}
void read_Childproc(int sig)
{
    pid_t id;
    int status;
    if(sig==SIGCHLD)
    {
        id=waitpid(-1,&status,WNOHANG);//销毁僵尸子进程
        printf("Remove proc id:%d\n",id);
    }
}

```

### 多线程并发服务器实现

```c++
//该服务端主要完成消息转发，向每个客户端发送消息

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
 
#define BUF_SIZE 100
#define MAX_CLNT 256

void* handle_clnt(void* arg);
void send_msg(char* msg,int len);
void error_handing(char* msg);

int clnt_cnt=0;//客户端连接的数量
int clnt_socks[MAX_CLNT];//存放与客户端通信的套接字

pthread_mutex_t mutx;//互斥变量

 int main(int argc, char *argv[])
{

    int ser_sock,clnt_sock;
    struct sockaddr_in serv_addr,clnt_addr;
    socklen_t clnt_add_size=sizeof(clnt_addr);
    pthread_t t_id;//线程id

    pthread_mutex_init(&mutx,NULL);//创建互斥变量

    ser_sock=socket(PF_INET,SOCK_STREAM,0);

    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    serv_addr.sin_port=htons(atoi(argv[1]));

    if(bind(ser_sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr))==-1)
        error_handing("bind error");
    if(listen(ser_sock,5)==-1)
        error_handing("listen error");
    while (1) {
        clnt_sock=accept(ser_sock,(struct sockaddr*)&clnt_addr,&clnt_add_size);

        pthread_mutex_lock(&mutx);
        clnt_socks[clnt_cnt++]=clnt_sock;//临界区
        pthread_mutex_unlock(&mutx);

        pthread_create(&t_id,NULL,handle_clnt,(void*)&clnt_sock);//创建线程
        pthread_detach(t_id);//从内存中完全销毁已经终止的程序
        printf("connected client IP:%s\n",inet_ntoa(clnt_addr.sin_addr));

    }
    close(ser_sock);
    return 0;
}
 void* handle_clnt(void* arg)
 {
    int clnt_sock=*((int*)arg);
    int str_len=0;
    char msg[BUF_SIZE];
    while((str_len=read(clnt_sock,msg,sizeof(msg)))!=0)
        send_msg(msg,str_len);
    pthread_mutex_lock(&mutx);
    for(int i=0;i<clnt_sock;i++)//remove disconnected client
    {
        if(clnt_socks[i]==clnt_sock)
        {
            while(i++<clnt_cnt-1)
            {
                clnt_socks[i]=clnt_socks[i+1];
            }
            break;
        }
    }
    clnt_cnt--;
    pthread_mutex_unlock(&mutx);
    close(clnt_sock);
    return NULL;
 }
 void send_msg(char* msg,int len)//send to all 向每个客户端发送信息
 {
    pthread_mutex_lock(&mutx);
    for(int i=0;i<clnt_cnt;i++)
    {
        write(clnt_socks[i],msg,len);
    }
    pthread_mutex_unlock(&mutx);
 }
 void error_handing(char* msg)
 {
     perror(msg);
     exit(1);
 }


```

###       read函数的返回值

1. 0 实际读到的字节数

2. =0 已经读到结尾或者对端已经关闭
3. -1 异常

### 多路IO转接服务器

##### 1.select函数 

用于监听是否有客户端产生连接和客户端的通信事件

```c
#include <sys/select.h>
#include <sys/time.h>

int select(int maxfd, fd_set *readset, fd_set *writeset,
           fd_set *exceptset, const struct timeval *timeout);
/*
成功时返回大于 0 的值，失败时返回 -1
maxfd: 监视对象文件描述符数量 （最大的文件描述符+1）
readset: 将所有关注「是否存在待读取数据」的文件描述符注册到 fd_set 型变量，并传递其地址值。
writeset: 将所有关注「是否可传输无阻塞数据」的文件描述符注册到 fd_set 型变量，并传递其地址值。
exceptset: 将所有关注「是否发生异常」的文件描述符注册到 fd_set 型变量，并传递其地址值。
timeout: 调用 select 函数后，为防止陷入无限阻塞的状态，传递超时(time-out)信息 NULL阻塞，永远等下去 传0检查后立即返回 轮询
返回值: 发生错误时返回 -1,超时时返回0,。因发生关注的事件返回时，返回大于0的值，该值是发生事件的文件描述符总数。
*/
//fd_set 其实就是一个集合用来将要监听的文件描述符注册在其中 根据FD_SET FD_CLR FD_ISSET FD_ZERO 函数来操作
//这三个集合fd_set都是传入传出参数，传入的是要监听的，传出的是实际有实际发生的。

```

###### 思路分析

创建用于监听的套接字，使用select函数将 lfd添加到监听集合中，然后根据传出参数和函数返回值判断是否有客户端的连接，然后根据accept函数 创建和客户端通信的套接字，然后再加入到监听集合中,遍历监听集合看是否有事件发生，然后对其进行处理 其中定义一个数组用来存放client文件描述符，提高遍历速度

```c
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<string.h>
#include<sys/time.h>
#include<sys/select.h>

#define BUF_SIZE 30

void sys_err(const char *str)
{
    perror(str);
    exit(1);
}
int main(int argc, char *argv[])
{
    int lfd,cfd;
    int client[FD_SETSIZE];//自定义数组client 防止遍历1024个文件描述符 FD_SETSIZE=1024
    char buf[BUF_SIZE];
    lfd=socket(PF_INET,SOCK_STREAM,0);
    if(lfd==-1)
        sys_err("socket erorr");
    struct sockaddr_in ser_addr,cln_addr;
    socklen_t cln_addr_size=sizeof(cln_addr);

    ser_addr.sin_family=AF_INET;
    ser_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    ser_addr.sin_port=htons(atoi(argv[1]));

    if(bind(lfd,(struct sockaddr*)&ser_addr,sizeof(ser_addr))==-1)
        sys_err("bind() error");
    if(listen(lfd,128)==-1)
        sys_err("listen error");
    fd_set allset,tempset;//创建监听集合
    FD_ZERO(&allset);
    FD_SET(lfd,&allset);//将lfd添加到监听集合中
    int maxfd=lfd;//保存最大的文件描述符
    int maxi=-1;//用来索引client[]的下标 记录该数组的最大下标

    for(int i=0;i<=1024;i++)
    {
        client[i]=-1;
    }
    while(1)
    {
        tempset=allset;
        int ret=select(maxfd+1,&tempset,NULL,NULL,NULL);//阻塞等待
        if(ret<0)
            sys_err("select error");
        if(ret>0)
        {
            if(FD_ISSET(lfd,&tempset))//有新的连接
            {
                cfd=accept(lfd,(struct sockaddr*)&cln_addr,&cln_addr_size);//此时accept不会阻塞
                int i;
                for(i=0;i<FD_SETSIZE;i++)//保存accept返回的文件描述到client[]里
                {
                    if(client[i]<0)
                    {
                        client[i]=cfd;
                        break;
                    }
                }
                if(i==FD_SETSIZE)
                {
                    fputs("too many clients\n",stderr);
                    exit(1);
                }
                maxi=i;
                if(cfd>maxfd)
                    maxfd=cfd;

                FD_SET(cfd,&allset);//添加到监听的集合中
                if(0==--ret)//说明只有lfd发生事件
                    continue;
            }
            int sockfd;
            for(int i=0;i<=maxi;i++)//检查有哪个clients有数据就绪
            {
                if((sockfd=client[i])<0)//取出client里保存的套接字并判断
                    continue;
                if(FD_ISSET(sockfd,&tempset))//客户端有read和write事件
                {
                    int ret=read(sockfd,buf,BUF_SIZE);
                    if(ret==0)//客户端发送关闭请求
                    {
                        close(sockfd);
                        FD_CLR(sockfd,&allset);//移除对此客户端的监听
                        client[i]=-1;
                    }
                    else{
                        write(sockfd,buf,ret);
                    }
                }
            }
        }
    }

    close(lfd);
    return 0;
}
```

###### select函数的优缺点

缺点：监听上限受文件描述符的限制 最大1024

检测满足条件的fd 比较麻烦，要自己添加逻辑提高效率

优点：跨平台

##### 2. epoll函数

- 突破1024文件描述符设置 

  使用ultimate -a 命令查看当前计算机能打开的最大文件个数 可以通过修改配置文件的方式来修改上限值

  epoll 函数是Linux下多路复用的IO接口的select/poll的增强版本 

- ```c
   int epoll_create(int size)//创建一棵监听红黑树
    
    //size:创建的红黑树的监听节点的数量（仅供内核参考）
    
    //返回值：指向新创建的红黑树的根节点fd 失败-1 epoll_create 函数创建的资源与套接字相同，也由操作系统管理。因此，该函数和创建套接字的情况相同，也会返回文件描述符
   ```


  ```c
​```c
#include <sys/epoll.h>
int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);//操作监听红黑树
/*
成功时返回 0 ，失败时返回 -1
epfd：用于注册监视对象的 epoll 例程的文件描述符 epoll_create函数的返回值
op：用于制定监视对象的添加、删除或更改等操作
fd：需要注册的监视对象文件描述符
event：监视对象的事件类型
*/
struct epoll_event
{
    __uint32_t events;
    epoll_data_t data;
};
typedef union epoll_data {
    void *ptr;
    int fd;
    __uint32_t u32;
    __uint64_t u64;
} epoll_data_t;
对监视对象的操作通过以下来操作
- EPOLL_CTL_ADD：将文件描述符注册到 epoll 例程
- EPOLL_CTL_DEL：从 epoll 例程中删除文件描述符
- EPOLL_CTL_MOD：更改注册的文件描述符的关注事件发生情况
  ```

```c
#include <sys/epoll.h>
int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);
/*
成功时返回发生事件的文件描述符的总个数，失败时返回 -1
epfd : 表示事件发生监视范围的 epoll 例程的文件描述符
events : 保存发生事件的文件描述符集合的结构体地址值 传出参数，满足监听条件的
maxevents : 第二个参数中可以保存的最大事件数
timeout : 以 1/1000 秒为单位的等待时间，传递 -1 时，一直等待直到发生事件 0 不阻塞
*/
```

###### 代码实现

```c
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<string.h>
#include<sys/time.h>
#include <sys/epoll.h>

#define BUF_SIZE 30
void sys_err(const char *str)
{
    perror(str);
    exit(1);
}

int main(int argc,char* argv[])
{
    int lfd,cfd;
    char buf[BUF_SIZE];
    struct sockaddr_in ser_addr,cln_addr;
    ser_addr.sin_family=AF_INET;
    ser_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    ser_addr.sin_port=htons(atoi(argv[1]));

    socklen_t cln_addr_size=sizeof(cln_addr);

    lfd=socket(AF_INET,SOCK_STREAM,0);
    bind(lfd,(struct sockaddr*)&ser_addr,sizeof(ser_addr));
    listen(lfd,5);

    int epfd=epoll_create(1024);//创建监听红黑树

    struct epoll_event tmp,ep[1024];//tmp 用来设置单个fd属性 ep是epoll_wait()传出满足的监听事件的数组
    tmp.events=EPOLLIN;
    tmp.data.fd=lfd;
    epoll_ctl(epfd,EPOLL_CTL_ADD,lfd,&tmp);//将lfd添加到监听红黑树中
    int ret=0;
    while(1)
    {
        ret=epoll_wait(epfd,ep,1024,-1);//实施监听 阻塞等待
        for(int i=0;i<ret;i++)
        {
            if(ep[i].data.fd==lfd)//lfd 满足事件 有新客户端发起连接
            {
                    cfd=accept(lfd,(struct sockaddr*)&cln_addr,&cln_addr_size);
                    tmp.events=EPOLLIN;//初始化 cfd的监听属性
                    tmp.data.fd=cfd;
                    epoll_ctl(epfd,EPOLL_CTL_ADD,cfd,&tmp);
            }
            else //有客户端写数据来
            {
                int n=read(ep[i].data.fd,buf,sizeof(buf));
                if(n==0)
                {
                    close(ep[i].data.fd);//对方关闭连接
                    epoll_ctl(epfd,EPOLL_CTL_DEL,ep[i].data.fd,NULL);//将关闭的fd 从监听树上摘下
                    continue;
                }
                write(ep[i].data.fd,buf,n);
            }
        }
    }
    close(epfd);
    close(lfd);
    return 0;
}

```

###### 事件模型

1、ET 边缘触发，只有数据到来才触发，不管缓冲区是否还有数据   缓冲区剩余未读净的数据不会导致epoll_wait 返回   		event.events=EPOLLIN | EPOLLET     只支持非阻塞

2、LT 水平触发，只要有数据都会触发（默认模式） 缓冲区剩余未读净的数据会导致epoll_wait返回

###### 将套接字改为非阻塞方式



```c
#include<fcntl.h>
int fcntl(int filedes,int cmd,...);//具有可变参数的形式
//filedes 更改的目标文件描述符
//cmd 表示函数调用的目的 F_GETFL 获得指定的文件描述符属性 F_SETFL 更改文件描述符的属性
          
int flag=fcntl(fd,F_GETFL,0);//获取之前设置的信息
fcntl(fd,F_SETFL,flag|O_NONBLOCK);//在此基础上添加非阻塞的标志
```

###### 边缘触发的回声服务器

```c
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<string.h>
#include<sys/time.h>
#include <sys/epoll.h>
#include<errno.h>
#include<fcntl.h>

#define BUF_SIZE 4
#define EPOLL_SIZE 50
void setnonblockingmode(int fd);
void error_handing(char* buf);

int main(int argc, char *argv[])
{
    int lfd,cfd,epfd;
    char buf[BUF_SIZE];
    struct sockaddr_in serv_addr,cln_addr;
    socklen_t cln_addr_size=sizeof(cln_addr);
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    serv_addr.sin_port=htons(atoi(argv[1]));

    lfd=socket(PF_INET,SOCK_STREAM,0);
    if(bind(lfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr))==-1)
        error_handing("bind error");
    if(listen(lfd,5)==-1)
        error_handing("listen error");

    setnonblockingmode(lfd);//设置为非阻塞模式
    struct epoll_event tmp,ep_events[1024];
    epfd=epoll_create(EPOLL_SIZE);//创建监听红黑树
    tmp.events=EPOLLIN;
    tmp.data.fd=lfd;
    epoll_ctl(epfd,EPOLL_CTL_ADD,lfd,&tmp);//将lfd添加到监听红黑树上

    while(1)
    {
        int ret=epoll_wait(epfd,ep_events,1024,-1);
        if(ret==-1)
        {
            puts("epoll_wait error");
            break;
        }
        puts("return epoll_wait");
        for(int i=0;i<ret;i++)
        {
            if(ep_events[i].data.fd==lfd)
            {
                cfd=accept(lfd,(struct sockaddr*)&cln_addr,&cln_addr_size);
                setnonblockingmode(cfd);//将与客户端通信的套接字设置为非阻塞模式
                tmp.events=EPOLLIN;
                tmp.data.fd=cfd;
                epoll_ctl(epfd,EPOLL_CTL_ADD,cfd,&tmp);
                printf("connected client:%d\n",cfd);
            }
            else
            {
                while(1)//需要读取缓冲区所有数据
                {
                    int len=read(ep_events[i].data.fd,buf,BUF_SIZE);
                    if(len==0)
                    {
                        epoll_ctl(epfd,EPOLL_CTL_DEL,ep_events[i].data.fd,NULL);
                        close(ep_events[i].data.fd);
                        break;
                    }
                    else if(len<0)
                    {
                        if(errno=EAGAIN)//说明缓冲区没有数据可读
                            break;
                    }
                    else
                    {
                        write(ep_events[i].data.fd,buf,len);
                    }
                }
            }
        }
    }
    close(lfd);
    close(epfd);
    return 0;
}
void setnonblockingmode(int fd)
{
    int flag=fcntl(fd,F_GETFL,0);
    fcntl(fd,F_SETFL,flag|O_NONBLOCK);
}
void error_handing(char* buf)
{
    perror(buf);
    exit(1);
}

```



###### epoll 反应堆模型

epoll ET模式 + 非阻塞 + void* ptr		利用回调函数自动回调 不但要监听cfd的读事件 还要监听cfd的写事件																																				

