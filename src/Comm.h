#ifndef Comm_h
#define Comm_h

typedef unsigned char byte;

// Communication interface
class Comm {
public:
    virtual void initComm()=0;
    virtual void writeBytes(byte reg,
                            byte count,
                            byte *values	)=0;
    virtual void readBytes(byte reg,
                           byte count,
                           byte *values,
                           byte rxAlign=0)=0;
    virtual ~Comm() {};
};

// SPI driver
class CommSPI : public Comm {
protected :
    const unsigned int spi_speed=10000000;
    const byte spi_bpw=8;
    const byte chipSelectDev=0;
    char _chipSelectDev[17];	// Device selected /dev/spidev0.y
public:
    CommSPI();
    void initComm();
    void writeBytes(byte reg,
                    byte count,
                    byte *values	);
    void readBytes(byte reg,
                   byte count,
                   byte *values,
                   byte rxAlign=0);
};

// I2C driver
class CommI2C : public Comm {
protected :
    int fd;
    char _chipSelectDev[11] = "/dev/i2c-1"; // RPi i2c device
public:
    CommI2C();
    void initComm();
    void writeBytes(byte reg,
                    byte count,
                    byte *values	);
    void readBytes(byte reg,
                   byte count,
                   byte *values,
                   byte rxAlign=0);
    ~CommI2C();
};

// UART driver
#include <termios.h>
class CommUART : public Comm {
protected :
    int fd;
    char _portName[13] = "/dev/ttyAMA0"; // RPi uart 
    struct termios _termios;		 // backup 
    speed_t _portSpeed = B9600;	 	 // speed 9600 bps	
    int timeout = 350;			 // rx timeout 350 ms
    void rcvBytes( byte count,
                   byte *values);
public:
    CommUART();
    void initComm();
    void writeBytes(byte reg,
                    byte count,
                    byte *values	);
    void readBytes(byte reg,
                   byte count,
                   byte *values,
                   byte rxAlign=0);
   ~CommUART();
};
#endif
