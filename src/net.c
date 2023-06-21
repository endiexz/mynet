#include "net.h"
#include "net_addr.h"
#include "pcap_device.h"
#include <string.h>
#include <stdio.h>

static net_time net_start_time;
static uint8_t ethernet_broadcast[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
static net_packet net_spacket, net_rpacket;

const char *test_domain = "www.baidu.com";
const char *test_eth = "wlan0";

static uint8_t my_mac_addr[MAC_ADDR_SIZE];
static uint8_t my_ip_addr[IPV4_ADDR_SIZE];
static char ip_str[16];

static net_arp_entry arp_entry[ARP_ENTRY_LEN];
static net_arp_entry_state arp_entry_state;

net_packet *net_packet_alloc_for_send(uint32_t size){
    net_spacket.data = net_spacket.payload + NET_PACKET_SIZE - size;
    net_spacket.size = size;
    return &net_spacket;
}

//分配一个空间来接受网络接口读取出来的包
net_packet *net_packet_alloc_for_read(uint32_t size){
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

int16_t swap_order16(uint16_t data){
    return (((data) & 0xff) << 8) | (((data) >>8) & 0xff);
}

net_err ethernet_init(void){
    net_err err = net_driver_open(my_mac_addr, ip_str);
    if( err<0 ) return err;
    return NET_ERR_OK;
}
void ethernet_poll(void){
    net_packet* packet;
    if(net_driver_read(&packet) == NET_ERR_OK){
        ethernet_in(packet);
    }
}

net_err ethernet_in(net_packet* packet){
    ethernet_hdr* ethernet_hdr_t;

    if(packet->size <=sizeof(ethernet_hdr)){
        return NET_ERR_NOT_ETHERNET;
    }
    ethernet_hdr_t = (ethernet_hdr*)packet->data;
    if(ethernet_hdr_t->protocol == swap_order16(NET_PROTOCOL_IP)){
        printf("target:%x %x %x %x %x %x ",ethernet_hdr_t->target[0],ethernet_hdr_t->target[1],ethernet_hdr_t->target[2],ethernet_hdr_t->target[3],ethernet_hdr_t->target[4],ethernet_hdr_t->target[5]);
        printf("source:%x %x %x %x %x %x ",ethernet_hdr_t->source[0],ethernet_hdr_t->source[1],ethernet_hdr_t->source[2],ethernet_hdr_t->source[3],ethernet_hdr_t->source[4],ethernet_hdr_t->source[5]);
        printf("protocol:%04x",swap_order16(ethernet_hdr_t->protocol));
        printf("packet size:%d\n",packet->size);
    }
    switch(swap_order16(ethernet_hdr_t->protocol)){
        case NET_PROTOCOL_ARP:
            remove_header(packet,sizeof(ethernet_hdr));
            net_arp_in(packet);
            break;
        case NET_PROTOCOL_IP:
            printf("is ip packet\n");
            printf("packet->size 1:%u\n",packet->size);
            remove_header(packet,sizeof(ethernet_hdr));
            printf("packet->size 1:%u\n",packet->size);
            printf("%d\n",net_ip_in(packet));
            break;
    }
}
net_err ethernet_out(net_protocol protocol, uint8_t* mac_addr, net_packet* packet){
    ethernet_hdr* ethernet_hdr_t;
    add_header(packet, sizeof(ethernet_hdr));
    ethernet_hdr_t = (ethernet_hdr*)packet->data;
    memcpy(ethernet_hdr_t->target, mac_addr, MAC_ADDR_SIZE);
    memcpy(ethernet_hdr_t->source, my_mac_addr, MAC_ADDR_SIZE);
    ethernet_hdr_t->protocol = protocol;

    return net_driver_send(packet);

}

void net_arp_init(void){
    net_start_time = net_time_get();
    int i;
    for(i=0;i<ARP_ENTRY_LEN;i++){
        arp_entry[i].state=ARP_ENTRY_FREE;
    }
    //printf("locate:%d\n",arp_entry_state.locate);
}

void net_arp_poll(void){
    int i;
    if(net_time_check(&net_start_time)){
        for(i=0;i<ARP_ENTRY_LEN;i++){
            switch(arp_entry[i].state){
                case ARP_ENTRY_BUSY:
                    if(--arp_entry[i].rmtime==0){
                        net_arp_request((uint8_t*)&arp_entry[i].ipaddr);
                        arp_entry[i].state = ARP_ENTRY_PENDING;
                        arp_entry[i].rmtime = ARP_ENTRY_PENDING_TIME;
                        arp_entry[i].retry_times = ARP_ENTRY_RETRY_TIMES;
                    }
                break;
                case ARP_ENTRY_PENDING:
                    if(--arp_entry[i].rmtime==0){
                        if(--arp_entry[i].retry_times==0){
                            arp_entry[i].state=ARP_ENTRY_FREE;
                        }
                        else{
                            net_arp_request((uint8_t*)&arp_entry[i].ipaddr);
                            arp_entry[i].rmtime = ARP_ENTRY_PENDING_TIME;
                        }
                    }

            }
        }
        /*printf("list:\n");
        for(i=0;i<ARP_ENTRY_LEN;i++){
            switch(arp_entry[i].state){
                case ARP_ENTRY_BUSY:
                printf("%d:  ipaddr:%d.%d.%d.%d  macaddr:%x-%x-%x-%x-%x-%x   state:busy   rmtime:%d\n",i,arp_entry[i].ipaddr[0],arp_entry[i].ipaddr[1],arp_entry[i].ipaddr[2],arp_entry[i].ipaddr[3],
                arp_entry[i].macaddr[0],arp_entry[i].macaddr[1],arp_entry[i].macaddr[2],arp_entry[i].macaddr[3],arp_entry[i].macaddr[4],arp_entry[i].macaddr[5],
                arp_entry[i].rmtime);
                break;
                case ARP_ENTRY_PENDING:
                printf("%d:  ipaddr:%d.%d.%d.%d  macaddr:%x-%x-%x-%x-%x-%x   state:pending   rmtime:%d,   retrytime:%d\n",i,arp_entry[i].ipaddr[0],arp_entry[i].ipaddr[1],arp_entry[i].ipaddr[2],arp_entry[i].ipaddr[3],
                arp_entry[i].macaddr[0],arp_entry[i].macaddr[1],arp_entry[i].macaddr[2],arp_entry[i].macaddr[3],arp_entry[i].macaddr[4],arp_entry[i].macaddr[5],
                arp_entry[i].rmtime,arp_entry[i].retry_times);
                break;
            }
        }*/
    }

}

net_err net_arp_in(net_packet* packet){
    if(packet->size < sizeof(net_arp_packet))
        return NET_ERR_ARP_LEN_ERR;
    net_arp_packet* arp_packet_t=(net_arp_packet*)packet->data;
    //printf("hdt:%04x  prot:%04x  hdlen:%04x  prolen:%04x  opcode:%04x\n",swap_order16(arp_packet_t->hardware_type), swap_order16(arp_packet_t->protocol_type), arp_packet_t->hardware_len, arp_packet_t->protocol_len, swap_order16(arp_packet_t->opcode));

    if( swap_order16(arp_packet_t->hardware_type) != ARP_HARDWARE_TYPE_ETHERNET ||
        swap_order16(arp_packet_t->protocol_type) != NET_PROTOCOL_IP ||
        arp_packet_t->hardware_len != MAC_ADDR_SIZE ||
        arp_packet_t->protocol_len != IPV4_ADDR_SIZE ||
        (swap_order16(arp_packet_t->opcode) != ARP_OP_REQUEST) && swap_order16(arp_packet_t->opcode) != ARP_OP_REPLY ||
        memcmp(arp_packet_t->target_ipaddr, my_ip_addr ,IPV4_ADDR_SIZE)!=0)
        //printf("err\n");
        return NET_ERR_ARP_REQUEST_TYPE_ERR;
        
    switch(swap_order16(arp_packet_t->opcode)){
        case ARP_OP_REQUEST:
            return net_arp_response(arp_packet_t);
            //printf("sendover\n");
            net_arp_entry_update(packet);
    }
    
}

net_err net_arp_response(net_arp_packet* arp_packet){
    net_packet* packet = net_packet_alloc_for_send(sizeof(arp_packet));
    net_arp_packet* arp_packet_t = (net_arp_packet*)packet->data;
    arp_packet_t->hardware_type = arp_packet->hardware_type;
    arp_packet_t->protocol_type = arp_packet->protocol_type;
    arp_packet_t->hardware_len = arp_packet->hardware_len;
    arp_packet_t->protocol_len = arp_packet->protocol_len;
    arp_packet_t->opcode = swap_order16(ARP_OP_REPLY);
    memcpy(arp_packet_t->send_macaddr, my_mac_addr,MAC_ADDR_SIZE);
    memcpy(arp_packet_t->send_ipaddr, my_ip_addr, IPV4_ADDR_SIZE);
    memcpy(arp_packet_t->target_macaddr,arp_packet->send_macaddr,MAC_ADDR_SIZE);
    memcpy(arp_packet_t->target_ipaddr,arp_packet->send_ipaddr, IPV4_ADDR_SIZE);

    return ethernet_out(NET_PROTOCOL_ARP, arp_packet_t->send_macaddr, packet);
}

void net_arp_request(uint8_t* ipaddr){
    net_packet* packet = net_packet_alloc_for_send(sizeof(net_arp_packet));
    net_arp_packet* arp_packet = (net_arp_packet*)packet->data;
    arp_packet->hardware_type = ARP_HARDWARE_TYPE_ETHERNET;
    arp_packet->protocol_type = swap_order16(NET_PROTOCOL_IP);
    arp_packet->hardware_len = MAC_ADDR_SIZE;
    arp_packet->protocol_len = IPV4_ADDR_SIZE;
    arp_packet->opcode = swap_order16(ARP_OP_REQUEST);
    memcpy(arp_packet->send_macaddr, my_mac_addr, sizeof(MAC_ADDR_SIZE));
    memcpy(arp_packet->send_ipaddr, my_ip_addr, sizeof(IPV4_ADDR_SIZE));
    memcpy(arp_packet->target_macaddr, ethernet_broadcast, sizeof(MAC_ADDR_SIZE));
    memcpy(arp_packet->target_ipaddr, ipaddr, sizeof(IPV4_ADDR_SIZE));
    ethernet_out(NET_PROTOCOL_ARP, ethernet_broadcast, packet);
}

void net_arp_entry_update(net_packet* packet){
    net_arp_packet* arp_packet_t = (net_arp_packet*)packet->data;
    int i, locale_t;
    for(i=0;i<ARP_ENTRY_LEN;i++){
        if(memcmp(arp_packet_t->send_ipaddr, arp_entry[i].ipaddr, IPV4_ADDR_SIZE)==0){
            locale_t = i;
            break;
        }
    }
    locale_t = arp_entry_state.locate;
    memcpy(arp_entry[locale_t].ipaddr, arp_packet_t->send_ipaddr, IPV4_ADDR_SIZE);
    memcpy(arp_entry[locale_t].macaddr, arp_packet_t->send_macaddr, MAC_ADDR_SIZE);
    arp_entry[locale_t].state = ARP_ENTRY_BUSY;
    arp_entry[locale_t].retry_times = ARP_ENTRY_RETRY_TIMES;
    arp_entry[locale_t].rmtime = ARP_ENTRY_RMTIMR;
    if(i==10)
        arp_entry_state.locate=((arp_entry_state.locate+1)%10);
}
void net_ip_init(void){

}
net_err net_ip_in(net_packet* packet){
    net_ip_hdr* ip_hdr_t = (net_ip_hdr*)packet->data;
    uint8_t ip_hdr_len_t;
    uint16_t ip_total_len_t;
    if(ip_hdr_t->version != IP_VERSION_IPV4)
        return NET_ERR_IP_VERSION_ERR;
    ip_hdr_len_t = ip_hdr_t->hdr_len;
    ip_total_len_t = swap_order16(ip_hdr_t->total_len);
    printf("ip_hdr_len:%d   hdlen:%x  totallen:%x\n",sizeof(net_ip_hdr),ip_hdr_t->hdr_len, ip_hdr_t->total_len);
    if ((ip_hdr_t->hdr_len*4) < sizeof(net_ip_hdr) || ip_hdr_t->hdr_len > ip_hdr_t->total_len) 
        return NET_ERR_IP_LEN_ERR;
    uint16_t checksum_t = ip_hdr_t->checksum;
    ip_hdr_t->checksum = 0;
    if(checksum_t != checksum16(ip_hdr_t,ip_hdr_t->hdr_len*4 ,0))
        return NET_ERR_IP_CHECKSUM_ERR;
    
    if(memcmp(ip_hdr_t->target_ipaddr, my_ip_addr, IPV4_ADDR_SIZE)!=0)
        return NET_ERR_IP_IPADDR_NOTME;
    return 1;
    
}
net_err net_ip_out(net_packet* packet, net_protocol protocol, uint8_t* ipaddr){
    static uint16_t ip_hdr_ip_t = 0;
    add_header(packet, sizeof(net_packet));
    net_ip_hdr* ip_hdr_t = (net_ip_hdr*)packet;
    ip_hdr_t->version = IP_VERSION_IPV4;
    ip_hdr_t->hdr_len = sizeof(net_ip_hdr);
    ip_hdr_t->typ_of_service = 0;
    ip_hdr_t->total_len = swap_order16(packet->size);
    ip_hdr_t->id = swap_order16(ip_hdr_ip_t);
    ip_hdr_t->flag_fragment = 0;
    ip_hdr_t->ttl = IP_TTL;
    ip_hdr_t->protocol = protocol;
    ip_hdr_t->checksum = 0;
    memcpy(ip_hdr_t->source_ipaddr, my_ip_addr, IPV4_ADDR_SIZE);
    memcpy(ip_hdr_t->target_ipaddr, ipaddr, IPV4_ADDR_SIZE);
    ip_hdr_t->checksum = checksum16(ip_hdr_t,ip_hdr_t->hdr_len*4,0);
    ip_hdr_ip_t++;

}

void net_init(void){
    net_addr_find(my_ip_addr, ip_str, my_mac_addr, test_eth);
    ethernet_init();
    net_arp_init();
    
}

void net_poll(void){
    ethernet_poll();
    net_arp_poll();
}

