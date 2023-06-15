#ifndef E_NET
#define E_NET
#include<stdint.h>
#define NET_PACKET_SIZE    1514
// define data structure of net packet
typedef struct _net_packet{
    uint16_t size; //当前包的大小
    uint16_t *data;//当前包的包头地址
    uint16_t payload[NET_PACKET_SIZE]; 
}net_packet;

net_packet *net_packet_send(uint16_t size);

net_packet *net_packet_read(uint16_t size);
//添加包头
void add_header(net_packet *packet, uint16_t size);

void remove_header(net_packet *packet, uint16_t size);

void truncate_header(net_packet *packet, uint16_t size);

void net_init(void);

void net_poll(void);

#endif