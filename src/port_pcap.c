#include "pcap_device.h"
#include "net.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>


static pcap_t* pcap;
net_err net_driver_open(uint8_t* my_mac_addr, char* ip_str){
    //打开驱动选项
    pcap = pcap_device_open(ip_str, my_mac_addr, 1);
    //返回值为0打开失败退出
    //否则返回一个网络接口信息
    if(pcap == (pcap_t*)0){
        printf("error\n");
        exit(-1);
    }
    return NET_ERR_OK;
}
//发送一个定义好的包
net_err net_driver_send(net_packet *packet){
    pcap_device_send(pcap, packet->data, packet->size);
    return NET_ERR_OK;
}


//读取一个定义好的包
net_err net_driver_read(net_packet** packet){
    uint32_t size;
    net_packet *r_packet = net_packet_alloc_for_read(NET_PACKET_SIZE);
    size = pcap_device_read(pcap, r_packet->data, NET_PACKET_SIZE);
    if(size){
        r_packet->size = size;
        *packet = r_packet;
        return NET_ERR_OK;
    }
    return NET_ERR_IO;
}

net_time net_time_get(void){
    return clock() / CLOCKS_PER_SEC;
}

int net_time_check(net_time* time){
    net_time new_time = net_time_get();
    if(new_time - *time >=ARP_ENTRY_PENDING_TIME){
        *time = new_time;
        return 1;
    }
        
    return 0;
}

uint16_t checksum16(net_ip_hdr* packet, uint16_t len, uint16_t pre_sum){
    uint32_t checksum_t=0;
    uint16_t height;
    uint16_t* packet_t = (uint16_t*)packet;
    while( len > 1){
        checksum_t += *packet_t;
        packet_t++;
        len-=2;
    }
    if(len>0)
        checksum_t += *(uint8_t*)packet_t;
    while((height = checksum_t >> 16) != 0){
        checksum_t = height + (checksum_t & 0xffff);
    }
    return (uint16_t)~checksum_t;
}