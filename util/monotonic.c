#include <sysinfoapi.h>
#include <stdio.h>

int main(){


    // The return value is the number of milliseconds that have elapsed since the system was started.
    int tickCount = GetTickCount();

    
    int ms = tickCount % 1000;
    int s = (tickCount / 1000) % 60;
    int m = (tickCount / 60000) % 60;
    int h = tickCount / 3600000;
    printf("%02d:%02d:%02d.%03d", h,m,s,ms);
    return 0;
}
