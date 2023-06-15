#include "net.h"
int main(){
    net_init();
    while(1){
        net_poll();
    }
    return 0;
}