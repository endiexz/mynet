#include "net.h"
#include <string.h>
#include <stdio.h>


#define swap_order16(v)       ((((v) & 0xff) << 8) |(((v) >>8) & 0xff))
static net_packet net_spacket, net_rpacket;

static uint8_t netif_mac[MAC_ADDR_SIZE];

static net_arp_entry arp_entry[ARP_ENTRY_LEN];

net_packet *net_packet_send(uint32_t size){
    net_spacket.data = net_spacket.payload + NET_PACKET_SIZE - size;
    net_spacket.size = size;
    return &net_spacket;
}

//分配一个空间来接受网络接口读取出来的包
net_packet *net_packet_read(uint32_t size){
    net_rpacket.data = net_rpacket.payload;
    net_rpacket.size = size;
    return &net_rpacket;
}


void add_header(net_packet *packet, uint16_t size){
    packet->data-=size;
    packet->size+=size;
}

void remove_header(net_packet *packet, uint16_t size){
    packet->data+=size;
    packet->size-=size;
}




void truncate_header(net_packet *packet, uint16_t size){
    
}

net_err ethernet_init(void){
    net_err err = net_driver_open(netif_mac);
    if( err<0 ) return err;
    return NET_ERR_OK;
}



net_err ethernet_in(net_packet* packet){
    ethernet_hdr* ethernet_hdr_t;

    if(packet->size <=sizeof(ethernet_hdr)){
        return NET_ERR_NOT_ETHERNET;
    }
    ethernet_hdr_t = (ethernet_hdr*)packet->data;
    uint16_t protocol_t = ethernet_hdr_t->protocol;
    printf("target:%x %x %x %x %x %x ",ethernet_hdr_t->target[0],ethernet_hdr_t->target[1],ethernet_hdr_t->target[2],ethernet_hdr_t->target[3],ethernet_hdr_t->target[4],ethernet_hdr_t->target[5]);
    printf("source:%x %x %x %x %x %x ",ethernet_hdr_t->source[0],ethernet_hdr_t->source[1],ethernet_hdr_t->source[2],ethernet_hdr_t->source[3],ethernet_hdr_t->source[4],ethernet_hdr_t->source[5]);
    printf("protocol:%x\n",protocol_t);
    switch(ethernet_hdr_t->protocol){
        case NET_PROTOCOL_ARP:
            break;
        case NET_PROTOCOL_IP:
            break;
    }

}

net_err ethernet_out(net_protocol protocol, uint8_t* mac_addr, net_packet* packet){
    ethernet_hdr* ethernet_hdr_t;
    add_header(packet, sizeof(ethernet_hdr));
    ethernet_hdr_t = (ethernet_hdr*)packet->data;
    memcpy(ethernet_hdr_t->target, mac_addr, MAC_ADDR_SIZE);
    memcpy(ethernet_hdr_t->source, netif_mac, MAC_ADDR_SIZE);
    ethernet_hdr_t->protocol = protocol;

    return net_driver_send(packet);

}

void ethernet_poll(void){
    net_packet* packet;
    if(net_driver_read(&packet) == NET_ERR_OK){
        ethernet_in(packet);
    }
}


void net_arp_init(void){
    int i;
    for(i=0;i<ARP_ENTRY_LEN;i++){
        arp_entry[i].state=ARP_ENTRY_FREE;
    }
}

void net_init(void){
    ethernet_init();
}

void net_poll(void){
    ethernet_poll();
}

