#include "Comm.h"
#include <stdio.h>

CommUART::CommUART() {
}
void CommUART::initComm() {
    printf("UART not yet supported\n");

}
void CommUART::writeBytes(   byte reg, 
                             byte count,
                             byte *values	) {
    printf("UART not yet supported\n");
}
void CommUART::readBytes(   byte reg,	
                            byte count,
                            byte *values,
                            byte rxAlign) {
    printf("UART not yet supported\n");
}

