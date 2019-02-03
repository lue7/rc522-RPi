#include "Comm.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>

CommI2C::CommI2C() {
}
// initialization
void CommI2C::initComm() {
    int addr = 0x28;                // address defined in datasheet 8.1.4.5
    fd=open(_chipSelectDev,O_RDWR);
    if (fd<0) {
        perror("i2c init : open i2c dev");
        return;
    }
    if (ioctl(fd, I2C_SLAVE, addr) < 0) 
        perror("i2c init : i2c dev addr");
    
    return;
}
// write count bytes from values
void CommI2C::writeBytes(   byte reg, 
                            byte count,
                            byte *values	) {
    byte *buf;
    ssize_t len = count+1;

    buf=(byte*)malloc(len);
    buf[0] = reg;
    memcpy(buf+1,values,count);

    if (write(fd,buf,len) != len)
        perror("write values : i2c transaction error");
   /* 
        printf("write reg %x values buf",reg);
        for (int i=0;i<count;i++) printf(" %x %x",values[i],buf[i+1]);
        printf(" len %d\n",count);
   */

    free(buf);
    return;
}
// read count bytes into values
void CommI2C::readBytes(   byte reg,	
                           byte count,
                           byte *values,
                           byte rxAlign) {
    byte value0 = values[0];
    
    values[0]= reg;
    if (write(fd,values,1) != 1) {
        perror("read write access  : i2c transaction error");
        return;
    }
    if (read(fd,values,count)!=count)
        perror("read values : i2c transaction error");
    
    if (rxAlign) {	// Only update bit positions rxAlign..7 in values[0]
        // Create bit mask for bit positions rxAlign..7
        byte mask = (0xFF << rxAlign) & 0xFF;
        // Apply mask to both current value of values[0] and the new data in value.
        values[0] = (value0 & ~mask) | (values[0] & mask);
    }
   /* 
    printf("read reg %02x values ", reg);
    for (int i=0;i<count;i++) printf(" %x",values[i]);
         printf(" len %d\n",count);
   */
   return;
}
CommI2C::~CommI2C() {
    close(fd);
}
