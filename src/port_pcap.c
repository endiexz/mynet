#include "pcap_device.h"
#include "net.h"
#include <string.h>
#include <stdlib.h>


static pcap_t* pcap;
const char *ip_str = "192.168.1.103";
const char my_mac_addr[] = {0x74, 0x4c, 0xa1, 0x54, 0x14, 0x6d};
net_err net_driver_open(uint8_t* mac_addr){
    memcpy(mac_addr, my_mac_addr, sizeof(my_mac_addr));
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
    net_packet *r_packet = net_packet_read(NET_PACKET_SIZE);
    size = pcap_device_read(pcap, r_packet->data, NET_PACKET_SIZE);
    if(size){
        r_packet->size = size;
        *packet = r_packet;
        return NET_ERR_OK;
    }
    return NET_ERR_IO;
}