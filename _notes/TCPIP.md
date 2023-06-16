# TCP/IP

### 以太网的封装格式

| 目的地址 | 源地址 | 类型 |    数据    |
| :------: | :----: | :--: | :--------: |
|   6bit   |  6bit  | 2bit | 46~1500bit |

类型： 0800      |    IP数据报
类型： 0806      |    ARP请求/应答  |  PAD
类型： 8035      |    RARP请求应答 |  PAD

包的大小   60--1514

== 定义 == 类型为两字节但是网络传输存在大小端的不同,因此需要进行大小端的变换;

最后还有四位的 CRC校验码

## IP

IP具有无连接：每个IP包相互独立
			不可靠：不能保证每一个包都能到达目的地址

![image-20230608174517848](.\TCPIP_t\image-20230608174517848.png)

要注意的是TCP/IP首部中所有的	二进制都要采用 big endian （大端法）进行传输



### 子网寻址

![image-20230608175044282](.\TCPIP_t\image-20230608175044282.png)

A类与B类地址为主机号分配了很多空间分别可以容纳的主机数为 2^24^  - 2  2^16^ - 2

## ARP地址解析协议

ARP协议为IP地址到对应的硬件地址之间提供动态映射

### ARP格式

![image-20230609170152811](.\TCPIP_t\image-20230609170152811.png)

ARP对应的帧类型为： 0x0806

## pcap





## 结构体内存对齐

![img](/home/endiexz/program/TCP_net/mynet/_notes/TCPIP_t/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L2NoZW5sb25nX2N4eQ==,size_16,color_FFFFFF,t_70.png)

为什么存在内存对齐？
平台原因（移植原因）： 不是所有的硬件平台都能访问任意地址上的任意数据的；某些平台只能在某些地址处取得某些特定类型的数据，否则抛出硬件异常。
比如，当一个平台要取一个整型数据时只能在地址为4的倍数的位置取得，那么这时就需要内存对齐，否则无法访问到该整型数据。

性能原因： 数据结构（尤其是栈）应该尽可能的在自然边界上对齐。原因在于，为了访问未对齐内存，处理器需要作两次内存访问；而对齐的内存访问仅需一次。

在画图时可能有博友会想，内存这么重要，在进行内存对齐的时候怎么还有内存被白白浪费掉呢？
现在看来，其实结构体的内存对齐是拿空间来换取时间的做法。

```c
#pragma pack(1)
//及下一变量对齐到当前变量以后1的倍数的地方。
```

## pcap函数解析

```c
pcap_t* pcap_open_live(const char* device, int snaplen, int promise, int to_ms, char *ebuf)

    device:需要打开的网络接口的名称;
	需要pcap_lookupdev()查找网卡返回的网络设备的名称指针列表
    snaplen:捕获的数据包的最大长度单位字节通常设置的值为 65535 最大值
    
    promisc: 是否启用混杂模式
    
    to_ms: 超时时间在此时间段内没有收到数据包，pcap将返回NULL
    ebfu: 错误缓冲区 存储错误信息，若函数执行成功缓冲区将不会被修改
    
        
        
        
        
        
        
        
        
        
char* pcap_lookupdev(errbuf);
//该函数用于查找系统上可用的网络接口并返回其中之一。
//当函数执行成功时errbuuf内容不会改变//并返回一个字符串指向找到的网络接口的名称字符串
//当函数执行失败返回NULL


int pcap_lookupnet(const char *device, bpf_u_int32* netp, bpf_int32 *maskp, char *errbuf)  
//用于查询指定的网络接口的IP以及子网掩码信息。
//devices: 要查询的网络接口的名称
//netp: 返回网络接口的IP地址
//maskp: 返回网络接口的子网掩码(网络字节序)
//errbuf: 错误信息输出缓冲区，用于存储函数执行过程中的错误信息
    
int pcap_compile(pcap, &fp, filter_exp, 0, net)
   //将fileter_exp中包含的过滤器规则编译为可执行的机器码并储存在fp中在后续的数据包捕获过程中被编译的的过滤器程序可以对每一个包
   //进行过滤只有符合规则的才会被交给抓包回调函数进行处理
//过滤器，用于定义对捕获的数据包过滤的规则
//pcap用于打开pcap_open_live或者pcap_open_offline()打开的pcap_t实例的指针
//fp struct bpf_program 要编译的过滤器程序的结构体指针
//fileter_exp 包含过滤器规则的C字符串
//int optimize:1启用优化选项，0禁用优化选项
//net 用于指定网络接口的子网掩码是否是捕获文件的子网掩码
    
```

