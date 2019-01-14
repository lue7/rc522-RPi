#include "Comm.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

// Claim uart interface using the c_iflag
#  define CCLAIMED 0x80000000
#ifndef MIN
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif

CommUART::CommUART() {
}
void CommUART::initComm() {
    struct termios termios;

    fd = open(_portName, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd == -1) {
        perror("open uart tty");
        return;
    }

    if (tcgetattr(fd, &_termios) == -1) {
        perror("can not get tty values");
        close(fd);
        return;
    }
    // Check the port is not claimed already
    if (_termios.c_iflag & CCLAIMED) {
        perror("tty is already claimed");
        close(fd);
        return;
    }
    // Save termios to restore it laters
    termios =_termios;

    termios.c_cflag = CS8 | CLOCAL | CREAD;
    termios.c_iflag = CCLAIMED | IGNPAR;
    termios.c_oflag = 0;
    termios.c_lflag = 0;

    termios.c_cc[VMIN] = 0;
    termios.c_cc[VTIME] = 0;

    // Port speed
    cfsetispeed(&termios, _portSpeed);
    cfsetospeed(&termios, _portSpeed);

    if (tcsetattr(fd, TCSANOW, &termios) == -1) {
        perror("unable to set tty");
        close(fd);
        return;
    }
    return;
}
void CommUART::writeBytes(   byte reg, 
                             byte count,
                             byte *values	) {
    byte * buf,res;

    buf=(byte*)malloc(count+1);
    buf[0] = reg;
    if (count>0) memcpy(buf+1,values,count);
    count++;
    if (count != write(fd, values, count))
    {
        perror("write values : UART_IOC_MESSAGE");
    }
    else
    {
        rcvBytes(1,&res);
        if (res!=reg)
        {
            perror("write values address : UART_IOC_MESSAGE");
        }
    }
    free(buf);
    return;
}
void CommUART::readBytes(   byte reg,
                            byte count,
                            byte *values,
                            byte rxAlign) {
    byte value0 = values[0];

    if (1 != write(fd, &reg, 1))
    {
        perror("read values address : UART_IOC_MESSAGE");
    }
    rcvBytes(count,values);

    if (rxAlign) {	// Only update bit positions rxAlign..7 in values[0]
        // Create bit mask for bit positions rxAlign..7
        byte mask = (0xFF << rxAlign) & 0xFF;
        // Apply mask to both current value of values[0] and the new data in value.
        values[0] = (value0 & ~mask) | (values[0] & mask);
    }
}

void CommUART::rcvBytes(    byte count,
                            byte *values) {

    int nbRcv = 0,nbB = 0;
    int res;
    fd_set rfds;

    while (count > nbRcv) {

        FD_ZERO(&rfds);
        FD_SET(fd, &rfds);

        struct timeval timeout_tv;
        if (timeout > 0) {
            timeout_tv.tv_sec = (timeout / 1000);
            timeout_tv.tv_usec = ((timeout % 1000) * 1000);
        }

        res = select(fd + 1, &rfds, NULL, NULL, timeout ? &timeout_tv : NULL);

        if ((res < 0) && (EINTR == errno)) {
            // Interrupted by a signal
            continue;
        }
        // Select error
        if (res < 0) {
            perror("receive values select : UART_IOC_MESSAGE");
            return;
        }
        // Time-out
        if (res == 0) {
            perror("receive values timeout : UART_IOC_MESSAGE");
            return;
        }

        // Bytes available
        res = ioctl(fd, FIONREAD, &nbB);
        if (res != 0) {
            perror("receive values number : UART_IOC_MESSAGE");
            return;
        }
        // Read data received
        res = read(fd, values + nbRcv, MIN(nbB, (count - nbRcv)));
        if (res <= 0) {
            perror("receive values read : UART_IOC_MESSAGE");
            return;
        }
        nbRcv += res;

    }

    return;
}
CommUART::~CommUART()
{
    tcsetattr(fd, TCSANOW, &_termios);
    close(fd);
}
