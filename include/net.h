#ifndef E_NET
#define E_NET
#include<stdint.h>
#define NET_PACKET_SIZE    1518
#define MAC_ADDR_SIZE      6
#define IPV4_ADDR_SIZE     4
#define ARP_ENTRY_LEN      10

//arp表项状体啊
#define ARP_ENTRY_FREE     0
#define ARP_ENTRY_BUSY     1

// define data structure of net packet

typedef union _net_ipaddr{
    uint8_t array[IPV4_ADDR_SIZE];
    uint32_t addr;
}net_ipaddr;

//arp表项
typedef struct net_arp_entry{
    net_ipaddr ipaddr;
    uint8_t macaddr[MAC_ADDR_SIZE];
    uint8_t state;
    uint16_t rmtime;
    uint8_t retry_times; 
}net_arp_entry;

typedef struct _net_packet{
    uint32_t size; //当前包的大小
    uint8_t *data;//当前包的包头地址
    uint8_t payload[NET_PACKET_SIZE]; 
}net_packet;

#pragma pack(1)
typedef struct _ethernet_hdr{
    uint8_t target[MAC_ADDR_SIZE];
    uint8_t source[MAC_ADDR_SIZE];
    uint16_t protocol;
}ethernet_hdr;
#pragma pack()

typedef enum _net_err{
    NET_ERR_OK = 0,
    NET_ERR_IO = -1,
    NET_ERR_NOT_ETHERNET = -2
}net_err;

typedef enum _net_protocol{
    NET_PROTOCOL_IP = 0x0800,
    NET_PROTOCOL_ARP = 0x0806
}net_protocol;

net_packet *net_packet_send(uint32_t size);

net_packet *net_packet_read(uint32_t size);
//添加包头
void add_header(net_packet *packet, uint16_t size);

void remove_header(net_packet *packet, uint16_t size);

void truncate_header(net_packet *packet, uint16_t size);

//网络接口的打开，数据包的发送和接收
net_err net_driver_open(uint8_t* mac_addr);
net_err net_driver_send(net_packet *packet);
net_err net_driver_read(net_packet** packet);


net_err ethernet_init(void);
net_err ethernet_in(net_packet* packet);
net_err ethernet_out(net_protocol protocol, uint8_t* mac_addr, net_packet* packet);
void ethernet_poll(void);


void net_arp_init(void);

void net_init(void);

void net_poll(void);




#endif