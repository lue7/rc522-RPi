#include "Comm.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/spi/spidev.h>

CommSPI::CommSPI() {
    sprintf(_chipSelectDev,"/dev/spidev0.%d",chipSelectDev);
}
// initialization
void CommSPI::initComm() {
    int fd;
    unsigned char mode=(0|0), bits=spi_bpw;
    unsigned int speed=spi_speed;
    fd=open(_chipSelectDev,O_RDWR);
    if (fd<0) {
        perror("dpi init : open spidev");
        return;
    }
    if (ioctl(fd, SPI_IOC_WR_MODE, &mode)<0)  
        perror("can't set spi mode");
    
    if (ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits)<0)
        perror("can't set bits per word");
    
    if (ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed)<0)
        perror("can't set max speed hz");
    
    close(fd);
    return;
}
// write count bytes from values
void CommSPI::writeBytes(   byte reg, 
                            byte count,
                            byte *values	) {
    byte *buf;
    int fd,status;
    struct spi_ioc_transfer xfer[2];

    bzero(xfer,sizeof xfer);
    fd=open(_chipSelectDev,O_RDWR);
    if (fd<0) {
        perror("write values : open spidev");
        return;
    }
    buf=(byte*)malloc(count+1);
    buf[0] = reg << 1;
    memcpy(buf+1,values,count);
    xfer[0].tx_buf = (unsigned long)buf;
    xfer[0].len = count+1; /* Length of  command to write*/
    xfer[0].speed_hz = spi_speed;
    xfer[0].bits_per_word = spi_bpw;
    status = ioctl(fd, SPI_IOC_MESSAGE(1), xfer);
    if (status < 0)
        perror("write values : SPI_IOC_MESSAGE");    
/*
        printf("values buf");
        for (int i=0;i<count;i++) printf(" %x %x",values[i],buf[i+1]);
        printf(" len %d\n",count);
*/   
    close(fd);
    free(buf);
    return;
}
// read count bytes into values
void CommSPI::readBytes(   byte reg,	
                           byte count,
                           byte *values,
                           byte rxAlign) {
    byte buf[2]={0};
    int i,fd,status;
    byte value0 = values[0];
    struct spi_ioc_transfer xfer[2];

    bzero(xfer,sizeof xfer);
    fd=open(_chipSelectDev,O_RDWR);
    if (fd<0) {
        perror("read values : open spidev");
        return;
    }
    buf[0]= 0x80 | (reg << 1);
    xfer[0].tx_buf = (unsigned long)buf;
    xfer[0].len = 2; /* Length of  command to write*/
    xfer[0].speed_hz = spi_speed;
    xfer[0].bits_per_word = spi_bpw;
    xfer[0].rx_buf = (unsigned long)buf;
    
    for (i=0;i<count;i++) {
        status = ioctl(fd, SPI_IOC_MESSAGE(1), xfer);
        if (status < 0)
        {
            perror("read values : SPI_IOC_MESSAGE");
            close(fd);
            return;
        }
        values[i] = buf[1];
        buf[0]= 0x80 | (reg << 1);
        buf[1]=0;
    }
    
    if (rxAlign) {	// Only update bit positions rxAlign..7 in values[0]
        // Create bit mask for bit positions rxAlign..7
        byte mask = (0xFF << rxAlign) & 0xFF;
        // Apply mask to both current value of values[0] and the new data in value.
        values[0] = (value0 & ~mask) | (values[0] & mask);
    }
    close(fd);
/*
    printf("read: %02x\n", buf[0]);
    printf("values");
    for (i=0;i<count;i++) printf(" %x",values[i]);
        printf(" len %d\n",count);
*/
    return;
}

