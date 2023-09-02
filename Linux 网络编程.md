

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

![](/run/media/root/本地磁盘/QQ图片20230717102548.jpg)

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

```
进程占用内存和CPU而程序占用磁盘。进程是正在运行的程序的实例。是一个具有一定独立功能的程序关于某个数据集合的一次运行活动。
任何进程都是由另一个进程创建的（除init进程），该进程被称为父进程，对应的进程号称为父进程号ppid.
pid_t getppid(void);
```

时间片

```
是操作系统分配给每个正在运行的进程微观上的一段CPU时间。由操作系统内核的调度程序分配给每一个进程。
```

并行和并发

```
1.并行：指在同一时刻，有多条指令在多个处理器上同时执行。
2.并发：指在同一时刻只能有一条指令执行，但多个进程指令被快速的轮换执行，使得宏观上具有多个进程同时执行的效果，但在微观上并不是同时执行，只是把时间分为若干段，使得多个进程快速交替的执行。
```

进程控制块（PCB）

```
内核为每个进程分配一个PCB进程控制块，维护进程相关的信息。
操作系统创建每个进程时，都会额外申请一块内存空间，用来存储、管理和控制该进程所需要的信息，例如：
    进程名称或者 ID  号，作为该进程的标识；
    当前进程的执行状态，进程在整个执行过程中可能处于开始、就绪、运行、等待或者终止这 5 种状态；
    进程占用的各种资源，例如内存大小、使用的输入输出设备等等。
    还记录了进程已经执行的时间，占用 CPU 的时间等等。
我们通常将这样的存储空间称为进程控制块（Process Control Block，简称 PCB）。
也就是说，进程控制块记录了进程当前运行情况以及所占资源的详细信息，并由操作系统负责管理和维护。操作系统中进程和进程控制块的数量始终是相等的，创建多少个进程就会相应产生多少个进程控制块。
进程控制块是进程存在的唯一标识，只有借助进程控制块，操作系统才能找到目标进程，进而实施管理和控制。当进程执行结束后，操作系统只需要释放相应进程控制块占用的内存空间，目标进程也随之消亡。

```

![image-20230801171736622](/root/.config/Typora/typora-user-images/image-20230801171736622.png)



PCB如何组织？

```
通常是通过链表的方式进行组织，把具有相同状态的进程链在一起，组成各种队列。
1.将所有处于就绪状态的进程链在一起，称为就绪队列；
2.把所有因等待某事件而处于等待状态的进程链在一起就组成各种阻塞队列；
3.另外，对于运行队列在单核 CPU 系统中则只有一个运行指针了，因为单核 CPU 在某个时间，只能运行一个程序。
```



进程状态转换

三状态模型：

![](/run/media/root/8E9A022C9A0210FF/20210601155134111.png)

```
1. 运行状态(running): 占有处理器正在运行
2. 就绪状态(ready): 具备运行条件，等待系统分配处理器以便运行，进程已分配到除CPU以外的所有必要资源，只要获得CPU就可立刻执行。
3. 等待状态(blocked): 又称为阻塞(blocked)态或睡眠(sleep)态，不具备运行条件，正在等待某个事件的完成
```

五状态模型：

![](/run/media/root/8E9A022C9A0210FF/b5ac80e345ce4fbfa203c0155ff69130.png)

```
新建态：进程刚被创建时的状态，尚未进入就绪队列。
终止态：进程完成任务到达正常结束点或异常终止，或被操作系统及有终止权的进程所终止时所处的状态。
```

linux查看进程信息的命令

```
inux中，ps命令的全称是process status，即进程状态的意思，主要作用是列出系统中当前正在运行的进程信息。

ps命令的功能很强大，参数也非常多，下面只举几个简单的实例。

    显示所有进程信息
    ps -A
    显示所有进程信息，包括其执行的命令
    ps -ef
    显示指定用户下的所有进程信息
    ps -u wintest
    查找指定进程：gogs
    ps -ef | grep gogs
 
```

```
Top命令
top命令经常用来监控linux的系统状况，是常用的性能分析工具，能够实时显示系统中各个进程的资源占用情况。在Linux操作系统中，top是使用最频繁，也是比较全的一个命令。Top命令类似于Windows系统的任务管理器工具。它对于所有正在运行的进行和系统负荷提供不断更新的概览信息，包括系统负载、CPU利用分布情况、内存使用、每个进程的内容使用情况等信息。

```

```
kill命令杀死进程
# kill 12345 杀死进程
# kill -KILL 123456 强制杀死进程
# kill -9 123456 彻底杀死进程
# kill -l 显示信号
```



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

## 网络结构模式

### C/S结构

服务器-客户机，Client-Server（C/S）结构，通常采取两层结构。服务器负责数据的管理，客户机负责完成与用户的交互任务。服务器部分是多个用户共享的信息与功能，执行后台服务，客户机部分为用户所专有，负责执行前台功能。

优点：

```
1. 能充分发挥客户端 PC 的处理能力,很多工作可以在客户端处理后再提交给服务器,所以 C/S 结构客户端响应速度快;
2. 操作界面漂亮、形式多样,可以充分满足客户自身的个性化要求;
3. C/S 结构的管理信息系统具有较强的事务处理能力,能实现复杂的业务流程;
4. 安全性较高,C/S 一般面向相对固定的用户群,程序更加注重流程,它可以对权限进行多层次校验,提供了更安全的存取模式,对信息安全的控制能力很强,一般高度机密的信息系统采用 C/S 结构适宜。
```

缺点：

```
1.客户端需要安装专用的客户端软件，首先涉及到安装的工作量,其次任何一台电脑出问题,如病毒、硬件损坏,都需要进行安装或维护。系统软件升级时,每一台客户机需要重新安装,其维护和升级成本非常高;
2.对客户端的操作系统一般也有限制，不能跨平台。
```

### B/S结构

B/S 结构(Browser/Server,浏览器/服务器模式),是 WEB 兴起后的一种网络结构模式,WEB浏览器是客户端最主要的应用软件。这种模式统一了客户端,将系统功能实现的核心部分集中到服务器上,简化了系统的开发、维护和使用。客户机上只要安装一个浏览器,如 Firefox 或 InternetExplorer,服务器安装 SQL Server、Oracle、MySQL 等数据库。浏览器通过 Web Server 同数据库进行数据交互。

优点：

```
总体拥有成本低，维护方便、分布性强、开发简单，可以不用安装任何专门的软件就能实现在任何地方进行操作，客户端零维护，系统的扩展非常容易，,只要有一台能上网的电脑就能使用。
```

缺点：

```
1.2.3.4.通信开销大、系统和数据的安全性较难保障;
个性特点明显降低,无法实现具有个性化的功能要求;
协议一般是固定的:http/https
客户端服务器端的交互是请求-响应模式,通常动态刷新页面,响应速度明显降低。
```



## MAC地址

网卡是一块被设计用来允许计算机在计算机网络上进行通讯的计算机硬件。其拥有MAC地址，属于OSI中数据链路层，它使得用户可以通过电缆或无线相互连接，**每个网卡都有一个被称为MAC地址的独一无二的48位串行号。**网卡的主要功能:1.数
据的封装与解封装、2.链路管理、3.数据编码与译码。

MAC地址，也称为局域网地址、以太网地址、物理地址或硬件地址，它是一个用来确认网络设备位置的地址。MAC 地址用于在网络中唯一标识一个网卡,一台设备若有一或多个网卡,则每个网卡都需要并会有一个唯一的 MAC 地址。

## IP地址

**IP协议**是为计算机网络互相连接进行通信而设计的协议，在因特网中，它是能连接到网上的所有计算机网络实现相互通信的一套规则，规定了计算机在因特网上进行通信时应当遵守的规则，。任何厂家生产的计算机系统,只要遵守 IP 协议就可以与因特网互连互通。

**IP地址**是指互联网协议地址，IP地址是**IP协议提供的一种统一的地址格式**，它为互联网上每一个网络和每一台主机分配的一个逻辑地址，以此来屏蔽物理地址的差异。

```
IP 地址是一个 32 位的二进制数,通常被分割为 4 个“ 8 位二进制数”(也就是 4 个字节)。IP 地址通常用“点分十进制”表示成(a.b.c.d)的形式,其中,a,b,c,d都是 0~255 之间的十进制整数。
例:点分十进IP地址(100.4.5.6),实际上是 32 位二进制数
(01100100.00000100.00000101.00000110)。
```

一台主机至少可以设置1个以上的ip，一台路由器可以设置2个以上ip,一块网卡也可以设置两个以上的ip，因此，让 43 亿台计算机全部连网其实是不可能的，所以会根据一种可以更换 IP 地址的技术 **`NAT`**，使得可连接计算机数超过 43 亿台。



**网络层（ip）与数据链路层（mac）有什么关系呢？**

```
1.MAC的作用是实现直连的两个设备之间通信，而ip则负责在没有直连的两个网络之间进行通信传输。
2.网络层的作用是实现主机与主机之间的通信，也叫点对点通信。
3.MAC只负责某一个区间之间的通信传输，IP负责将数据包发给最终的目的地址。
4.源IP地址和目标地址在传输过程中是不会变化的，只有源MAC地址和目标MAC地址一直在变化。
```

### IP地址编址方式

每个 IP 地址包括两个标识码(ID),即**网络ID 和主机 ID。**。同一个物理网络上的所有主机都使用同一个网络 ID,网络上的一个主机(包括网络上工作站,服务器和路由器等)有一个主机 ID 与其对应。

**为什么要分离网络号和主机号？**

```
因为两台计算机要通讯，域内，即网络地址是否相同。如果网络地址相同，表明接受方在本网络上，那么可以把数据包直接发送到目标主机。
```

**子网掩码**

```
子网掩码（Subnet Mask）是用于划分IP地址的网络地址和主机地址的一个32位二进制数。它与IP地址进行逻辑AND运算，用于确定一个IP地址所属的网络和主机部分。

子网掩码一般由四个8位二进制数组成，例如：255.255.255.0。这个子网掩码表示前三个8位二进制数为网络地址，最后一个8位二进制数为主机地址。

子网掩码的作用是将IP地址划分为网络地址和主机地址两部分。网络地址用于识别所属的网络，主机地址用于标识在该网络中的具体主机。
例：172.20.0.0/16 后面的16表示子网掩码有16位所以255.255.0.0 就是B类地址
```

**A类IP地址**

```
A类IP地址是指，在IP地址的四段号中，第一段号码为网络号码，剩下三段为本地计算机的号码。A 类 IP 地址就由 1 字节的网络地址和 3 字节主机地址组成。
A类网络地址数量较少，因为网络地址只有1字节，所以只有126个网络，但是有3字节主机地址组成，所以每个网络可以容纳主机数达1600多万台。
A类IP地址的子网掩码为255.0.0.0  子网掩码通过与ip地址&操作得到网络号，通过将子网掩码取反再将其和ip地址&操作，得到主机号
```

![image-20230829155114881](/root/.config/Typora/typora-user-images/image-20230829155114881.png)

网络地址最高位必须是0

**B类IP地址**

```
一个 B 类 IP 地址是指,在 IP 地址的四段号码中,前两段号码为网络号码。如果用二进制表示 IP 地址的
话,B 类 IP 地址就由 2 字节的网络地址和 2 字节主机地址组成,网络地址的最高位必须是“10”。B 类 IP
地址中网络的标识长度为 16 位,主机标识的长度为 16 位,B 类网络地址适用于中等规模的网络,
B 类 IP 地址的子网掩码为 255.255.0.0,
```

![image-20230829160202425](/root/.config/Typora/typora-user-images/image-20230829160202425.png)

**C类IP地址**

```
一个 C 类 IP 地址是指,在 IP 地址的四段号码中,前三段号码为网络号码,剩下的一段号码为本地计算机的号码。如果用二进制表示 IP 地址的话,C 类 IP 地址就由 3 字节的网络地址和 1 字节主机地址组成,网络地址的最高位必须是“110”。适用于小规模的局域网络,每个网络最多只能包含254台计算机。
C类IP地址的子网掩码为 255.255.255.0,每个网络支持的最大主机数为 256 - 2 = 254 台。
```

![image-20230829160218005](/root/.config/Typora/typora-user-images/image-20230829160218005.png)

**D类IP地址**

```
D 类 IP 地址在历史上被叫做多播地址(multicast address),即组播地址。在以太网中,多播地址命名了一组应该在这个网络中应用接收到一个分组的站点。多播地址的最高位必须是 “1110”,范围从224.0.0.0 - 239.255.255.255。
多播用于将包发送给特定组内的所以主机。
```

![image-20230829160250533](/root/.config/Typora/typora-user-images/image-20230829160250533.png)

**特殊的网址**
每一个字节都为 0 的地址( “0.0.0.0” )对应于当前主机;
IP 地址中的每一个字节都为 1 的 IP 地址( “255.255.255.255” )是当前子网的广播地址;
IP 地址中凡是以 “11110” 开头的 E 类 IP 地址都保留用于将来和实验使用。
IP地址中不能以十进制 “127” 作为开头,该类地址中数字 127.0.0.1 到 127.255.255.255 用于回路测试,如:127.0.0.1可以代表本机IP地址。

**A、B、C 分类地址最大主机个数是如何计算的呢？**

```
最大主机个数，就是看主机号的位数，如C类地址主机号占8位。那么就是是2^8-2=254.
为什么减2呢因为在 IP 地址中，有两个 IP 是特殊的，分别是主机号全为 1 和 全为 0 地址。
主机号全为 1 指定某个网络下的所有主机，用于广播。
主机号全为 0 指定某个网络。
```

**IP分类的优点**

```
不管是路由器还是主机解析到一个 IP 地址时候，很快的找出网络地址和主机地址。简单明了、选路（基于网络地址）简单。
```

**IP 分类的缺点**

```
1.同一网络下没有地址层次
2.A、B、C类有个尴尬处境，就是不能很好的与现实网络匹配。C 类地址能包含的最大主机数量实在太少了,而 B 类地址能包含的最大主机数量又太多了
```

### 无分类地址CIDR

```
正因为IP分类存在许多缺点，提出了无分类地址的方案，32比特的IP地址被划分为两部分，前面是网络号后面是主机号。表示形式 a.b.c.d/x，其中 /x 表示前 x 位属于网络号， x 的范围是 0 ~ 32，这就使得 IP 地址更加具有灵活性。
10.100.122.2/24，这种地址表示形式就是 CIDR，/24 表示前 24 位是网络号，剩余的 8 位是主机号。
```

## 端口

```
“端口” 是英文 port 的意译,可以认为是设备与外界通讯交流的出口。端口可分为虚拟端口和物理端口,其中虚拟端口指计算机内部或交换机路由器内的端口,不可见,是特指TCP/IP协议中的端口,是逻辑意义上的端口。
```

### 端口类型

- 周知端口

  ```
  周知端口是众所周知的端口号,也叫知名端口、公认端口或者常用端口,范围从 0 到 1023,它们紧密绑定于一些特定的服务。例如 80 端口分配给 WWW 服务,21 端口分配给 FTP 服务,23 端口分配给Telnet服务等等。我们在 IE 的地址栏里输入一个网址的时候是不必指定端口号的,因为在默认情况下
  WWW 服务的端口是 “80”。
  ```

- 注册端口

  ```
  端口号从 1024 到 49151,它们松散地绑定于一些服务,分配给用户进程或应用程序,这些进程主要是
  用户选择安装的一些应用程序,而不是已经分配好了公认端口的常用程序。
  ```

- 动态端口 / 私有端口

  ```
  动态端口的范围是从 49152 到 65535。之所以称为动态端口,是因为它一般不固定分配某种服务,而是动态分配。
  ```


## 协议

```
协议,网络协议的简称,网络协议是通信计算机双方必须共同遵从的一组约定。如怎么样建立连接、怎么样互相识别等。只有遵守这个约定,计算机之间才能相互通信交流。
为了使数据在网络上从源到达目的,网络通信的参与方必须遵循相同的规则,这套规则称为协议
```

协议最终体现为在网络上传输的数据包的格式。



**ARP协议**

根据IP地址去查找对应的MAC地址。ARP 是借助 **ARP 请求与 ARP 响应**两种类型的包确定 MAC 地址的。

```
arp地址解析协议的工作原理：
每台主机在其ARP缓存区中维护一个arp列表，当源主机想发送数据给目的主机的时候，先在自己的arp列表中查找是否有该mac地址，如果有就之间将数据发送给目的主机,如果没有就向本地网段发送一个ARP请求广播包来查询目的之举对应的mac地址。
当本地网络中的所有主机都接受到该arp数据包时，首先检查数据包中的IP是否是自己的IP地址，如果不是，则忽略该数据包。如果是，取出数据包中的源主机的IP和MAC地址，并写入自己的arp列表中，并将自己的mac地址发送给源主机。源主机收到相应包之后，先将目的主机中的IP和mac写入自己的arp列表中，并将数据发送给目的主机。如果没有响应包，则表示ARP查询失败。【广播发送ARP请求，单播发送响应】
```

![](/run/media/root/本地磁盘/Linux系统编程、网络编程笔记/arp请求封装.png)

![image-20230831103658255](/root/.config/Typora/typora-user-images/image-20230831103658255.png)

**RARP协议**

```
已知MAC地址求IP地址
```



## 封装

```
上层协议使用下层协议提供的服务是通过封装实现的，应用程序数据在发送到物理网络上之前，沿着协议栈从上往下依次传递。每层协议都将在上层数据的基础上加上自己的头部信息(有时还包括尾部信息),以实现该层的功能,这个过程就称为封装。
```

![image-20230831100943451](/root/.config/Typora/typora-user-images/image-20230831100943451.png)

当帧到达目的主机时,将沿着协议栈自底向上依次传递。各层协议依次处理帧中本层负责的头部数据,以获取所需的信息,并最终将处理后的帧交给目标应用程序。这个过程称为**分用(demultiplexing)**。分用是依靠头部信息中的类型字段实现的。

**网络通信过程**

![](/run/media/root/本地磁盘/Linux系统编程、网络编程笔记/网络通信的过程.png)



## socket

 socket(套接字),就是对网络中不同主机上的应用进程之间进行双向通信的端点的抽象。一个套接字就是网络上进程通信的一端,提供了应用层进程利用网络协议交换数据的机制。**是应用程序通过网络协议进行通信的接口，是应用程序与网络协议根进行交互的接口。**。它是**网络环境中进程间通信的 API**,也是可以被命名和寻址的通信端点,使用中**的每一个套接字都有其类型和一个与之相连进程**。

### 字节序

在各种计算机体系结构中,对于字节、字等的存储机制有所不同,因而引发了计算机通信领域中一个很重要的问题,即通信双方交流的信息单元(比特、字节、字、双字等等)应该以什么样的顺序进行传送。如果不达成一致的规则,通信双方将无法进行正确的编码/译码从而导致通信失败。**字节序,顾名思义字节的顺序,就是大于一个字节类型的数据在内存中的存放顺序**

```
字节序分为大端字节序(Big-Endian) 和小端字节序(Little-Endian)。大端字节序是指一个整数的最高位字节(23 ~ 31 bit)存储在内存的低地址处,低位字节(0 ~ 7 bit)存储在内存的高地址处;小端字节序则是指整数的高位字节存储在内存的高地址处,而低位字节则存储在内存的低地址处。
```

#### 字节序转换函数

当格式化的数据在两台使用不同字节序的主机之间直接传递时,接收端必然错误的解释之。解决问题的方法是:**发送端总是把要发送的数据转换成大端字节序数据后再发送,而接收端知道对方传送过来的数据总是采用大端字节序,**所以**接收端可以根据自身采用的字节序决定是否对接收到的数据进行转换(**小端机转换,大端机不转换)。网络字节序通常采用大端顺序排列方式

包括从主机字节序到网络字节序的转换函数:htons、htonl;从网络字节序到主机字节序的转换函数:ntohs、ntohl。

```c
#include <arpa/inet.h>
// 转换端口
uint16_t htons(uint16_t hostshort);
uint16_t ntohs(uint16_t netshort);
// 转IP
uint32_t htonl(uint32_t hostlong);
uint32_t ntohl(uint32_t netlong);
```

#### socket地址

```
socket地址其实就是一个结构体，封装了端口号和IP等信息。
```

##### 通用socket地址

```c
#include <bits/socket.h>
struct sockaddr {
sa_family_t sa_family;//地址族类型
char sa_data[14];
};
typedef unsigned short int sa_family_t;
```

![image-20230831150348273](/root/.config/Typora/typora-user-images/image-20230831150348273.png)

#### 专用socket地址

很多网络编程函数诞生早于 IPv4 协议,那时候都使用的是 struct sockaddr 结构体,为了向前兼容**,现在sockaddr 退化成了(void *)的作用**,传递一个地址给函数,至于这个函数是 **sockaddr_in 还是sockaddr_in6,由地址族确定,然后函数内部再强制类型转化为所需的地址类型。**

![image-20230831150905942](/root/.config/Typora/typora-user-images/image-20230831150905942.png)

### IP地址转换（字符串转网络字节序）

下面 3 个函数可用于用**点分十进制字符串**表示的 IPv4 地址和用**网络字节序整数表示的 IPv4 地址**之间的转换:

```c
#include <arpa/inet.h>//只适用ipv4
in_addr_t inet_addr(const char *cp);
int inet_aton(const char *cp, struct in_addr *inp);
char *inet_ntoa(struct in_addr in);
```

```c

//常用
#include <arpa/inet.h>//ipv4 ipv6都可
// p:点分十进制的IP字符串,n:表示network,网络字节序的整数
int inet_pton(int af, const char *src, void *dst);
af:地址族: AF_INET
 AF_INET6
src:需要转换的点分十进制的IP字符串
dst:转换后的结果保存在这个里面
// 将网络字节序的整数,转换成点分十进制的IP地址字符串
const char *inet_ntop(int af, const void *src, char *dst, socklen_t size);
af:地址族: AF_INET
 AF_INET6
src: 要转换的ip的整数的地址
dst: 转换成IP地址字符串保存的地方
size:第三个参数的大小(数组的大小)
返回值:返回转换后的数据的地址(字符串),和 dst 是一样的
```

### TCP通信流程

![image-20230831160214389](/root/.config/Typora/typora-user-images/image-20230831160214389.png)

TCP：传输控制协议，面向连接，可靠的，基于字节流，仅支持单播传输，适用场景可靠性高的应用。

UDP：用户数据包协议，面向无连接，可以单播、多播、广播, 面向数据报,不可靠，适用场景实时应用(视频会议,直播).

### 套接字函数

```c
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
 // 包含了这个头文件,上面两个就可以省略
int socket(int domain, int type, int protocol);
- 功能:创建一个套接字
- 参数:
- domain: 协议族
AF_INET : ipv4
AF_INET6 : ipv6
AF_UNIX, AF_LOCAL : 本地套接字通信(进程间通信)
- type: 通信过程中使用的协议类型
SOCK_STREAM : 流式协议
SOCK_DGRAM
 : 报式协议
- protocol : 具体的一个协议。一般写0
- SOCK_STREAM : 流式协议默认使用 TCP
- SOCK_DGRAM
 : 报式协议默认使用 UDP
- 返回值:
- 成功:返回文件描述符,操作的就是内核缓冲区。
- 失败:-1
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen); // socket命
名
- 功能:绑定,将fd 和本地的IP + 端口进行绑定
- 参数:
- sockfd : 通过socket函数得到的文件描述符
- addr : 需要绑定的socket地址,这个地址封装了ip和端口号的信息
- addrlen : 第二个参数结构体占的内存大小
int listen(int sockfd, int backlog);
 // /proc/sys/net/core/somaxconn
- 功能:监听这个socket上的连接
- 参数:
- sockfd : 通过socket()函数得到的文件描述符
- backlog : 未连接的和已经连接的和的最大值, 5
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
- 功能:接收客户端连接,默认是一个阻塞的函数,阻塞等待客户端连接
- 参数:
- sockfd : 用于监听的文件描述符
- addr : 传出参数,记录了连接成功后客户端的地址信息(ip,port)
- addrlen : 指定第二个参数的对应的内存大小
- 返回值:
- 成功 :用于通信的文件描述符
- -1 : 失败
int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
- 功能: 客户端连接服务器
- 参数:
- sockfd : 用于通信的文件描述符
- addr : 客户端要连接的服务器的地址信息
- addrlen : 第二个参数的内存大小
- 返回值:成功 0, 失败 -1
ssize_t write(int fd, const void *buf, size_t count);
ssize_t read(int fd, void *buf, size_t count);

```

### TCP三次握手---（目的：保证双方互相之间建立连接）

在连接的建立过程中，双方需要交换一些连接的参数，这些参数可以放在TCP头部。

#### TCP头部结构

![image-20230831210149838](/root/.config/Typora/typora-user-images/image-20230831210149838.png)

- 序列号：在建立连接时由计算机生成的随机数作为其初始值，通过 SYN 包传给接收端主机，每发送一次数据，就「累加」一次该「数据字节数」的大小。**用来解决网络包乱序问题。**

- **确认应答号**：指下一次「期望」收到的数据的序列号，发送端收到这个确认应答以后可以认为在这个序号以前的数据都已经被正常接收。**用来解决丢包的问题。**

- 标志位：

  **紧急 URG** :此位置 1 ,表明紧急指针字段有效,它告诉系统此报文段中有紧急数据,应尽快传送

   **确认 ACK:**仅当 ACK=1 时确认号字段才有效,TCP 规定,在连接建立后所有传达的报文段都必须把 ACK 置1

  **推送 PSH:**当发送方TCP 把 PSH 置 1,并立即创建一个报文段发送出去,接收方收到 PSH = 1 的报文段,就**尽快地(即“推送”向前)交付给接收应用进程,而不再等到整个缓存都填满后再向上交付**

  **复位 RST:**该位为 `1` 时，表示 TCP 连接中出现异常必须强制断开连接。

  **同步 SYN:**仅在三次握手建立 TCP 连接时有效。当 SYN = 1 而 ACK = 0 时,表明这是一个连接请求报文段,对方若同意建立连接,则应在相应的报文段中使用 SYN = 1 和 ACK = 1。因此,SYN 置1 就表示这是一个连接请求或连接接受报文

  **终止 FIN:**用来释放一个连接。当 FIN = 1 时,表明此报文段的发送方的数据已经发送完毕,并要
  求释放运输连接

  

建立一个 TCP 连接是需要客户端与服务端达成上述三个信息的共识。

- **Socket**：由 IP 地址和端口号组成
- **序列号**：用来解决乱序问题等
- **窗口大小**：用来做流量控制



#### 三次握手过程

**发生在客户端连接的时候，connect函数。**为了确保双方都有发送和接收的能力，第一次客户端给服务器发送请求，服务器知道客户端有发送数据的能力，在第二次回信的时候，客户端收到回信后，知道服务器有接受和发送的能力，于是在第三次发送信息时，在服务器收到信息时知道客户端有接收的能力。

![image-20230831221721299](/root/.config/Typora/typora-user-images/image-20230831221721299.png)

```
第一次握手:
1.客户端将SYN标志位置为1
2.生成一个随机的32位的序号seq=J , 这个序号后边是可以携带数据(数据的大小)
第二次握手:
1.服务器端接收客户端的连接: ACK=1
2.服务器会回发一个确认序号: ack=客户端的序号 + 数据长度 + SYN/FIN(按一个字节算)
3.服务器端会向客户端发起连接请求: SYN=1
4.服务器会生成一个随机序号:seq = K
第三次握手:
1.客户单应答服务器的连接请求:ACK=1
2.客户端回复收到了服务器端的数据:ack=服务端的序号 + 数据长度 + SYN/FIN(按一个字节算)
```

**第三次握手是可以携带数据的，前两次握手是不可以携带数据的**

### 滑动窗口

滑动窗口(Sliding window)是**一种流量控制技术**。**接收方告诉发送方在某一时刻能送多少包(称窗口尺寸)。**滑动窗口的大小意味着接收方还有多大的缓冲区可以用于接收数据。发送方可以通过滑动窗口的大小来确定应该发送多少字节的数据。当滑动窗口为 0时,发送方一般不能再发送数据报。

窗口可以理解为缓冲区的大小，滑动窗口的大小会随着发送数据和接受数据而变化。

```
服务器:
发送缓冲区(发送缓冲区的窗口)
接收缓冲区(接收缓冲区的窗口)
客户端
发送缓冲区(发送缓冲区的窗口)
接收缓冲区(接收缓冲区的窗口)
```

### TCP四次挥手

四次挥手发生在断开连接的时候,在程序中当调用了close()会使用TCP协议进行四次挥手。
客户端和服务器端都可以主动发起断开连接,谁先调用close()谁就是发起。

![image-20230831231544366](/root/.config/Typora/typora-user-images/image-20230831231544366.png)

**2MSL**：两倍报文段寿命时间。主动断开连接的一方，最后会进入一个TIME_WAIT状态，这个状态会持续2msl.这样就能够让 TCP 连接的主动关闭方在它发送的 ACK 丢失的情况下重新发送最终的 ACK。**被动关闭方总是重传 FIN 直到它收到一个最终的 ACK。**

**半关闭**
当 TCP 链接中 A 向 B 发送 FIN 请求关闭,另一端 B 回应 ACK 之后(A 端进入 FIN_WAIT_2状态),并没有立即发送 FIN 给 A,A 方处于半连接状态(半开关),此时 A 可以接收 B 发送的数据,但是 A 已经不能再向 B 发送数据。

可以使用 API 来控制实现半连接状态:

```c
#include <sys/socket.h>
int shutdown(int sockfd, int how);
sockfd: 需要关闭的socket的描述符
how:
 允许为shutdown操作选择以下几种方式:
SHUT_RD(0): 关闭sockfd上的读功能,此选项将不允许sockfd进行读操作。
该套接字不再接收数据,任何当前在套接字接受缓冲区的数据将被无声的丢弃掉。
SHUT_WR(1):
 关闭sockfd的写功能,此选项将不允许sockfd进行写操作。进程不能在对此套接字发
出写操作。
SHUT_RDWR(2):关闭sockfd的读写功能。相当于调用shutdown两次:首先是以SHUT_RD,然后以
SHUT_WR。
```

1. 如果有多个进程共享一个套接字,close 每被调用一次,计数减 1 ,直到计数为 0 时,也就是所用
进程都调用了 close,套接字将被释放。
2. 在多进程中如果一个进程调用了 shutdown(sfd, SHUT_RDWR) 后,其它的进程将无法进行通信。
但如果一个进程 close(sfd) 将不会影响到其它进程。

### TCP状态转换

![image-20230901151853908](/root/.config/Typora/typora-user-images/image-20230901151853908.png)

### 端口复用

```c
端口复用最常用的用途是:
防止服务器重启时之前绑定的端口还未释放
程序突然退出而系统没有释放端口
#include <sys/types.h>
#include <sys/socket.h>
// 设置套接字的属性(不仅仅能设置端口复用)
int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t
optlen);
参数:
- sockfd : 要操作的文件描述符
- level : 级别 - SOL_SOCKET (端口复用的级别)
- optname : 选项的名称
- SO_REUSEADDR
- SO_REUSEPORT
- optval : 端口复用的值(整形)
- 1 : 可以复用
- 0 : 不可以复用
- optlen : optval参数的大小
端口复用,设置的时机是在服务器绑定端口之前。
    int optval=1;
setsockopt(fd,SO_REUSEPORT,&optval,sizeof(optval));
bind();
```



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

## 多路IO转接服务器(IO多路复用)

**I/O 多路复用使得程序能同时监听多个文件描述符,能够提高程序的性能**,Linux 下实现 I/O 多路复用的系统调用主要有 select、poll 和 epoll。

##### 1.select函数 

**用于监听是否有客户端产生连接和客户端的通信事件**

主旨思想:
1. 首先要构造一个关于文件描述符的列表,将要监听的文件描述符添加到该列表中。
2. 调用一个系统函数,监听该列表中的文件描述符,直到这些描述符中的一个或者多个进行I/O
操作时,该函数才返回。
a.这个函数是阻塞
b.函数对文件描述符的检测的操作是由内核完成的
3. 在返回时,它会告诉进程有多少(哪些)描述符要进行I/O操作。

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

每次调用select,都需要把fd集合从用户态拷贝到内核态,这个开销在fd很多时会很大

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
    EPOLL_CTL_ADD:添加
    EPOLL_CTL_MOD:修改
    EPOLL_CTL_DEL:删除
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
//检测函数
#include <sys/epoll.h>
int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);
/*
成功时返回发生事件的文件描述符的总个数，失败时返回 -1
epfd : 表示事件发生监视范围的 epoll 例程的文件描述符
events : 保存发生事件的文件描述符集合的结构体地址值 传出参数，满足监听条件的
maxevents : 第二个参数中可以保存的最大事件数 第二个参数结构体数组的大小
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

1、**ET 边缘触发，只有数据到来才触发**，不管缓冲区是否还有数据   缓冲区剩余未读净的数据不会导致epoll_wait 返回   		**event.events=EPOLLIN | EPOLLET     只支持非阻塞**

2、LT 水平触发（默认工作方式），同时支持阻塞和非阻塞，只要有数据都会触发（默认模式） 缓冲区剩余未读净的数据会导致epoll_wait返回。**只要缓冲区有数据就会导致epoll_wait()返回**

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

## 阻塞/非阻塞、同步/异步(网络IO)

```
数据就绪：阻塞-----调用IO方法的线程进入阻塞状态     非阻塞------不会改变线程的状态，通过返回值判断（errno的值来判断，EINTR,EAGAIN,EWOULDBLOCK）
```

```
同步IO：在一个线程中，一旦遇到IO操作，如读写文件、发送网络数据等，就需要等待IO操作完成才能继续进行下一步操作。
（数据的读写都是由请求方自己来完成的）
如果只有一个线程，那么线程被阻塞在此处，其他位置的代码就无法执行了，所以首先解决方案就是引入多线程，让其他线程去执行别处代码但是如果线程创建的太多，单核CPU下切换频繁的话，会导致代码真正运行的时间变少，性能下降。于是，异步IO出现了！

异步IO：
当代码需要执行一个耗时的IO操作时，它只发出IO指令，并不等待IO结果，然后就去执行其他代码了。一段时间后，当IO返回结果时，再通知CPU进行处理异步IO模型需要一个消息循环，在消息循环中，主线程不断地重复“读取消息-处理消息”这一过程。
（调用者不负责读写，而是等其处理好后通知调用者来直接获取IO操作的结果）

-当遇到IO操作时，代码只负责发出IO请求，不等待IO结果，然后直接结束本轮消息处理，进入下一轮消息处理过程。当IO操作完成后，将收到一条“IO完成”的消息，处理该消息时就可以直接获取IO操作结果。
-在“发出IO请求”到收到“IO完成”的这段时间里，同步IO模型下，主线程只能挂起，但异步IO模型下，主线程并没有休息，而是在消息循环中继续处理其他消息。这样，在异步IO模型下，一个线程就可以同时处理多个IO请求，并且没有切换线程的操作。对于大多数IO密集型的应用程序，使用异步IO将大大提升系统的多任务处理能力。

```

例子：

```
    1 老张把水壶放到火上，立等水开。（同步阻塞）
    2 老张把水壶放到火上，去客厅看电视，时不时去厨房看看水开没有。（同步非阻塞）
    3 老张把响水壶放到火上，立等水开。（异步阻塞） （水开之后，能大声发出嘀~~~~的噪音。），这样没有意义
    4 老张把响水壶放到火上，去客厅看电视，水壶响之前不再去看它了，响了再去拿壶。（异步非阻塞）

所谓同步异步，只是对于水壶而言。 普通水壶，同步；响水壶，异步。 虽然都能干活，但响水壶可以在自己完工之后，提示老张水开了。这是普通水壶所不能及的。 同步只能让调用者去轮询自己（情况2中），造成老张效率的低下。

所谓阻塞非阻塞，仅仅对于老张而言。 立等的老张，阻塞；看电视的老张，非阻塞。 情况1和情况3中老张就是阻塞的，媳妇喊他都不知道。虽然3中响水壶是异步的，可对于立等的老张没有太大的意义。所以一般异步是配合非阻塞使用的，这样才能发挥异步的效用。

```

 

```
同步：select,poll,epoll都是同步的。
异步：AIO（linux） IOCP（Windows）
```

### Linux上的五种IO模型

![](/run/media/root/本地磁盘/IMG_1252(20230801-223512).JPG)

![](/run/media/root/本地磁盘/IMG_1256(20230801-223650).JPG)

![](/run/media/root/本地磁盘/IMG_1257(20230801-223841).JPG)

![](/run/media/root/本地磁盘/IMG_1258(20230801-224003).JPG)

![](/run/media/root/本地磁盘/IMG_1259(20230801-224028).JPG)



## Web Server

```
一个Web Server 就是一个服务器软件，或者运行这个服务器软件的硬件（计算机）。主要功能就是通过http协议与客户端（通常是浏览器Browser）进行通信，处理来自客户端的http请求，并对其请求做出http响应，返回给客户端其请求的内容（文件、网页等）。
```

浏览器与相应服务器进行通信，在浏览器中键入"域名"或者"IP地址：端口号"，浏览器先将域名解析成相应的ip地址或者直接根据ip地址向对应的web服务器发送一个http请求。这一过程首先要通过TCP协议的三次握手建立与目标Web服务器的连接，然后**http协议生成针对web服务器的http请求报文，通过TCP、IP等协议发送到目标Web服务器。**

![屏幕截图 2023-08-05 154511](/run/media/root/本地磁盘/Linux系统编程、网络编程笔记/屏幕截图 2023-08-05 154511.jpg)





### HTTP协议（应用层的协议）

```
HTTP协议（Hypertext Transfer Protocol）是一种用于传输超文本的应用层协议。它是互联网的基础协议之一，用于客户端和服务器之间的通信。
「超文本」，它就是超越了普通文本的文本，它是文字、图片、视频等的混合体，最关键有超链接，能从一个超文本跳转到另外一个超文本。

HTML 就是最常见的超文本了，它本身只是纯文字文件，但内部用很多标签定义了图片、视频等的链接，再经过浏览器的解释，呈现给我们的就是一个文字、有画面的网页了。
1.简单：HTTP使用简单的请求-响应模型，客户端发送请求，服务器返回响应。
2.无状态：HTTP协议是无状态的，服务器不会保存客户端的状态信息。每个请求都是独立的，服务器不会记住之前的请求。
3.可扩展：HTTP协议允许通过添加新的方法、头部字段和状态码来扩展功能。
4.基于TCP/IP：HTTP协议基于TCP/IP协议栈，使用TCP作为传输层协议。
5.支持多媒体：HTTP协议支持传输各种类型的数据，包括文本、图像、音频和视频等。
```

**HTTP协议的工作流程通常包括以下步骤**

1.客户端发起请求：客户端发送一个HTTP请求到服务器，请求包括请求方法（如GET、POST）、请求的URL、请求头部和请求体（可选）等信息。

2.服务器处理请求：服务器接收到请求后，根据请求的URL和方法进行相应的处理，可能包括读取文件、执行程序、查询数据库等操作。

3.服务器发送响应：服务器根据处理结果生成一个HTTP响应，包括响应状态码、响应头部和响应体等信息。

4.客户端接收响应：客户端接收到服务器发送的响应后，根据响应的状态码和头部信息进行相应的处理，可能包括解析HTML页面、显示图片、播放音频等操作。

#### HTTP请求报文格式

![](/run/media/root/本地磁盘/Linux系统编程、网络编程笔记/屏幕截图 2023-08-05 155629.jpg)

![](/run/media/root/本地磁盘/Linux系统编程、网络编程笔记/http请求方法.jpg)

**http常见的字段：**

| Host字段              | 客户端发送请求时，用来指定服务器的域名。                     |
| --------------------- | ------------------------------------------------------------ |
| *Content-Length 字段* | 服务器在返回数据时，会有 Content-Length 字段，表明本次回应的数据长度。 |
| Connection 字段       | Connection 字段最常用于客户端要求服务器使用「HTTP 长连接」机制，以便其他请求复用。HTTP 长连接的特点是，只要任意一端没有明确提出断开连接，则保持 TCP 连接状态。Connection: Keep-Alive        开启了 HTTP Keep-Alive 机制后， 连接就不会中断，而是保持连接。当客户端发送另一个请求时，它会使用同一个连接，一直持续到客户端或服务器端提出断开连接。 |
| Content-Type 字段     | Content-Type 字段用于服务器回应时，告诉客户端，本次数据是什么格式。         Content-Type: text/html; Charset=utf-8  表示：发送的是网页，  而且编码是UTF-8 |
| Accept 字段           | 客户端请求的时候，Accept字段声明自己可以接受哪些数据格式。   |
| Content-Encoding 字段 | 字段说明数据的压缩方法。表示服务器返回的数据使用了什么压缩格式                        Content-Encoding: gzip  服务器返回的数据采用了 gzip 方式压缩，告知客户端需要用此方式解压。 |



#### HTTP响应报文格式



**http常见的状态码：**

1xx 类状态码属于提示信息，是协议处理中的一种中间状态，实际用到的比较少。

2xx 类状态码表示服务器成功处理了客户端的请求，也是我们最愿意看到的状态。

「200 OK」是最常见的成功状态码，表示一切正常。如果是非 HEAD 请求，服务器返回的响应头都会有 body 数据。

「204 No Content」也是常见的成功状态码，与 200 OK 基本相同，但响应头没有 body 数据。

「206 Partial Content」是应用于 HTTP 分块下载或断点续传，表示响应返回的 body 数据并不是资源的全部，而是其中的一部分，也是服务器处理成功的状态。

3xx 类状态码表示客户端请求的资源发生了变动，需要客户端用新的 URL 重新发送请求获取资源，也就是重定向。

「301 Moved Permanently」表示永久重定向，说明请求的资源已经不存在了，需改用新的 URL 再次访问。

「302 Found」表示临时重定向，说明请求的资源还在，但暂时需要用另一个 URL 来访问。

301 和 302 都会在响应头里使用字段 Location，指明后续要跳转的 URL，浏览器会自动重定向新的 URL。

「304 Not Modified」不具有跳转的含义，表示资源未修改，重定向已存在的缓冲文件，也称缓存重定向，也就是告诉客户端可以继续使用缓存资源，用于缓存控制。
4xx 类状态码表示客户端发送的报文有误，服务器无法处理，也就是错误码的含义。

「400 Bad Request」表示客户端请求的报文有错误，但只是个笼统的错误。

「403 Forbidden」表示服务器禁止访问资源，并不是客户端的请求出错。

「404 Not Found」表示请求的资源在服务器上不存在或未找到，所以无法提供给客户端。

5xx 类状态码表示客户端请求报文正确，但是服务器处理时内部发生了错误，属于服务器端的错误码。

「500 Internal Server Error」与 400 类型，是个笼统通用的错误码，服务器发生了什么错误，我们并不知道。

「501 Not Implemented」表示客户端请求的功能还不支持，类似“即将开业，敬请期待”的意思。

「502 Bad Gateway」通常是服务器作为网关或代理时返回的错误码，表示服务器自身工作正常，访问后端服务器发生了错误。

「503 Service Unavailable」表示服务器当前很忙，暂时无法响应客户端，类似“网络服务正忙，请稍后重试”的意思。

![6-五大类HTTP状态码](/run/media/root/本地磁盘/Linux系统编程、网络编程笔记/6-五大类HTTP状态码.webp)

#### HTTP请求方法

其中HEAD，GET，OPTIONS,TRACE是安全的方法，因为它们只是从服务器获取资源和信息，而不对服务器进行任何修改。而POST，PUT，DELETE和PATCH则影响服务器上的资源。





### 服务器编程的基本框架 



![屏幕截图 2023-08-05 174655](/run/media/root/本地磁盘/Linux系统编程、网络编程笔记/屏幕截图 2023-08-05 174655.jpg)

![屏幕截图 2023-08-05 175043](/run/media/root/本地磁盘/Linux系统编程、网络编程笔记/屏幕截图 2023-08-05 175043.jpg)







  两种高效的事件处理模式

服务器程序通常需要处理三类事件：I/O事件、信号及定时事件。有两种高效的事件处理模式：Reactor和Proactor，同步IO模型通常用于实现Reactor模式，异步IO模型用于实现Proactor模式。

##### Reactor模式









   
