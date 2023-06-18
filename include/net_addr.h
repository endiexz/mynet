#ifndef _NET_ADDR
#define _NET_ADDR
#include <stdint.h>
void net_addr_find(uint8_t* ipaddr, char*ip_str, uint8_t* macaddr, const char* eth);
#endif