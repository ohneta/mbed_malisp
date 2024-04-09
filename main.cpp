/**
 *
 */
 #include "mbed.h"
#include <stdio.h>
#include "malisp.h"
 
Serial pc(USBTX, USBRX);
DigitalOut  led1(LED1);
DigitalOut  led2(LED2);
DigitalOut  led3(LED3);
DigitalOut  led4(LED4);

//int main(int argc, const char * argv[]) {
int main() {
    pc.baud(115200);
    led1 = led2 = led3 = led4 = 0;

    malisp_main();
    return 0;
}
