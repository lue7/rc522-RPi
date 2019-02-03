# rc522-RPi
C++ library to use RC522 RFID module with a Rapsberry-pi.
The three interfaces SPI/I2C/UART are intented to be supported **SPI and I2C work / UART is under development nowadays**

# Enabling SPI/I2C/UART
SPI driver uses /dev/spidev0.0
```
dtparam=spi=on
```
I2C driver uses /dev/i2c-1
```
dtparam=i2c-arm=on
```
UART use [/dev/ttyAMA0](https://www.raspberrypi.org/documentation/configuration/uart.md) which is PL011.
It is needed to disable linux console
```
systemctl stop serial-getty@ttyAMA0.service
```

# Build
You need linux and arm-linux-gnuebaihf-g++ on your computer and you can build the examples
```
make
```
Then you just have to copy them to your RPi.

This library is *under development*, ... it is a port of https://github.com/miguelbalboa/rfid to RPi.

# Examples
firmware_check [-spi|-i2c|-uart]
	Test the module firmware

DumpInfo [-spi|-i2c|-uart]
	Read data of a card, a rfid tag
