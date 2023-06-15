#include "net.h"
static net_packet net_spacket, net_rpacket;
net_packet *net_packet_send(uint16_t size){
    net_spacket.data = net_spacket.payload + NET_PACKET_SIZE - size;
    net_spacket.size = size;
    return &net_spacket;
}

net_packet *net_packet_read(uint16_t size){
    net_rpacket.data = net_rpacket.payload;
    net_rpacket.size = size;
    return &net_rpacket;
}


void vdd_header(net_packet *packet, uint16_t size){
    packet->data-=size;
    packet->size+=size;
}

void remove_header(net_packet *packet, uint16_t size){
    packet->data+=size;
    packet->size-=size;
}

void truncate_header(net_packet *packet, uint16_t size){
    
}

void net_init(void){

}

void net_poll(void){
    
}

