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
    if (ioctl(fd, I2C_SLAVE, addr) < 0) {
        perror("i2c init : i2c dev addr");
        return;
    }

}
// write count bytes from values
void CommI2C::writeBytes(   byte reg, 
                            byte count,
                            byte *values	) {
    byte *buf;
    size_t len = count+1;

    buf=(byte*)malloc(len);
    buf[0] = reg;
    memcpy(buf+1,values,count);

    if (write(fd,buf,len) != len) {
        perror("write values : i2c transaction error");
        return;
    }
    /*
        printf("values buf");
        for (int i=0;i<count;i++) printf(" %x %x",values[i],buf[i+1]);
        printf(" len %d\n",count);
*/   

    free(buf);
}
// read count bytes into values
void CommI2C::readBytes(   byte reg,	
                           byte count,
                           byte *values,
                           byte rxAlign) {
    byte value0 = values[0];
    
    values[0]= reg;
    if (read(fd,values,count)!=count) {
        perror("read values : i2c transaction error");
        return;
    }
    /*
    for (i=0;i<count;i++) {
        if (read(fd,values+i,1)!=1) {
            perror("read values : i2c transaction error");
            return;
        }
        values[i] = buf[1];
        buf[0]= 0x80 | reg;
        buf[1]=0;
    }
*/    
    if (rxAlign) {	// Only update bit positions rxAlign..7 in values[0]
        // Create bit mask for bit positions rxAlign..7
        byte mask = (0xFF << rxAlign) & 0xFF;
        // Apply mask to both current value of values[0] and the new data in value.
        values[0] = (value0 & ~mask) | (values[0] & mask);
    }
    
    // printf("read: %02x\n", buf[0]);
    // printf("values");
    // for (i=0;i<count;i++) printf(" %x",values[i]);
    //     printf(" len %d\n",count);
}
CommI2C::~CommI2C() {
    close(fd);
}
