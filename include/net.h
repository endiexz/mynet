#ifndef E_NET
#define E_NET
#include<stdint.h>

#define NET_PACKET_SIZE     65535
#define MAC_ADDR_SIZE       6
#define IPV4_ADDR_SIZE      4
#define ARP_ENTRY_LEN       10

//arp表项state
#define ARP_ENTRY_FREE      0
#define ARP_ENTRY_BUSY      1
#define ARP_ENTRY_PENDING   2

#define ARP_ENTRY_RMTIMR   10
#define ARP_ENTRY_PENDING_TIME          1
#define ARP_ENTRY_RETRY_TIMES           4


//arp包硬件类型以及opcode
#define ARP_HARDWARE_TYPE_ETHERNET      0x0001
#define ARP_OP_REQUEST                  0x0001
#define ARP_OP_REPLY                    0x0002


#define IP_VERSION_IPV4                 4


//arp表项
typedef struct net_arp_entry{
    uint8_t ipaddr[IPV4_ADDR_SIZE];
    uint8_t macaddr[MAC_ADDR_SIZE];
    uint8_t state;
    uint16_t rmtime;
    uint8_t retry_times; 
}net_arp_entry;

typedef struct _net_arp_entry_state{
    uint16_t locate;
}net_arp_entry_state;

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

typedef struct _net_arp_packet{
    uint16_t hardware_type;
    uint16_t protocol_type;
    uint8_t hardware_len;
    uint8_t protocol_len;
    uint16_t opcode;
    uint8_t send_macaddr[MAC_ADDR_SIZE];
    uint8_t send_ipaddr[IPV4_ADDR_SIZE];
    uint8_t target_macaddr[MAC_ADDR_SIZE];
    uint8_t target_ipaddr[IPV4_ADDR_SIZE];
}net_arp_packet;

typedef struct _net_ip_hdr{
    uint8_t hdr_len : 4;
    uint8_t version : 4;
    uint8_t typ_of_service;
    uint16_t total_len;
    uint16_t id;
    uint16_t flag_fragment;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t checksum;
    uint8_t source_ipaddr[IPV4_ADDR_SIZE];
    uint8_t target_ipaddr[IPV4_ADDR_SIZE];

}net_ip_hdr;
#pragma pack()

typedef enum _net_err{
    NET_ERR_OK = 0,
    NET_ERR_IO = -1,
    NET_ERR_NOT_ETHERNET = -2,
    NET_ERR_ARP_LEN_ERR = -3,
    NET_ERR_ARP_REQUEST_TYPE_ERR = -4,
    NET_ERR_IP_VERSION_ERR = -5,
    NET_ERR_IP_LEN_ERR = -6,
    NET_ERR_IP_CHECKSUM_ERR = -7,
    NET_ERR_IP_IPADDR_NOTME = -8
}net_err;

typedef enum _net_protocol{
    NET_PROTOCOL_IP = 0x0800, 
    NET_PROTOCOL_ARP = 0x0806
}net_protocol;

net_packet *net_packet_alloc_for_send(uint32_t size);

net_packet *net_packet_alloc_for_read(uint32_t size);
//添加包头
void add_header(net_packet *packet, uint16_t size);

void remove_header(net_packet *packet, uint16_t size);

void truncate_header(net_packet *packet, uint16_t size);

int16_t swap_order16(uint16_t data);

//网络接口的打开，数据包的发送和接收
net_err net_driver_open(uint8_t* mac_addr, char* ip_str);
net_err net_driver_send(net_packet *packet);
net_err net_driver_read(net_packet** packet);


net_err ethernet_init(void);
net_err ethernet_in(net_packet* packet);
net_err ethernet_out(net_protocol protocol, uint8_t* mac_addr, net_packet* packet);
void ethernet_poll(void);


void net_arp_init(void);
void net_arp_poll(void);
net_err net_arp_in(net_packet* packet);
net_err net_arp_response(net_arp_packet* arp_packet);
void net_arp_request(uint8_t* ipaddr);
void net_arp_entry_update(net_packet* packet);

void net_ip_init(void);
net_err net_ip_in(net_packet* packet);

void net_init(void);
void net_arp_init(void);

void net_poll(void);




#endif