#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>

static int get_local_mac(const char *eth_inf, uint8_t *mac);
static int get_local_ip(const char *eth_inf, char *ip,char*ip_str);

void net_addr_find(uint8_t* ipaddr, char*ip_str, uint8_t* macaddr, const char* eth){
    int err1 = get_local_ip(eth, ipaddr, ip_str);
    int err2 = get_local_mac(eth, macaddr);
    if(err1!=0 || err2!=0) exit(1);
    printf("%s\n",ip_str);
    printf("%d.%d.%d.%d\n",ipaddr[0],ipaddr[1],ipaddr[2],ipaddr[3]);
    printf("%x:%x:%x:%x:%x:%x\n",macaddr[0],macaddr[1],macaddr[2],macaddr[3],macaddr[4],macaddr[5]);
}   
 
// 获取本机mac
static int get_local_mac(const char *eth_inf, uint8_t *mac)
{
	struct ifreq ifr;
	int sd;
	
	bzero(&ifr, sizeof(struct ifreq));
	if( (sd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("get %s mac address socket creat error\n", eth_inf);
		return -1;
	}
	
	strncpy(ifr.ifr_name, eth_inf, sizeof(ifr.ifr_name) - 1);
 
	if(ioctl(sd, SIOCGIFHWADDR, &ifr) < 0)
	{
		printf("get %s mac address error\n", eth_inf);
		close(sd);
		return -1;
	}
    int i;
    for(i=0;i<6;i++){
        mac[i]=(uint8_t)ifr.ifr_hwaddr.sa_data[i];
    }
 
	close(sd);
	return 0;
}
 
static int get_local_ip(const char *eth_inf, char *ip,char*ip_str)
{
	int sd;
	struct sockaddr_in sin;
	struct ifreq ifr;
 
	sd = socket(AF_INET, SOCK_DGRAM, 0);
	if (-1 == sd)
	{
		printf("socket error: %s\n", strerror(errno));
		return -1;		
	}
 
	strncpy(ifr.ifr_name, eth_inf, IFNAMSIZ);
	ifr.ifr_name[IFNAMSIZ - 1] = 0;
	
	// if error: No such device
	if (ioctl(sd, SIOCGIFADDR, &ifr) < 0)
	{
		printf("ioctl error: %s\n", strerror(errno));
		close(sd);
		return -1;
	}
	memcpy(&sin, &ifr.ifr_addr, sizeof(sin));
    snprintf(ip_str, 16, "%s", inet_ntoa(sin.sin_addr));
    memcpy(ip, &sin.sin_addr, 4);
	close(sd);
	return 0;
}