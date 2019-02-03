/*
 * --------------------------------------------------------------------------------------------------------------------
 * Example program showing how to read data from a PICC to serial.
 * --------------------------------------------------------------------------------------------------------------------
 * This is a MFRC522 library example; 
 * 
 * Example program showing how to read data from a PICC (that is: a RFID Tag or Card) using a MFRC522 based RFID
 * Reader on the RPi SPI interface.
 * 
 * When you present a PICC (that is: a RFID Tag or Card) at reading distance of the MFRC522 Reader/PCD, the output
 * will show the ID/UID, type and any data blocks it can read. Note: you may see "Timeout in communication" messages
 * when removing the PICC from reading distance too early.
 * 
 * If your reader supports it, this program will read all the PICCs presented (that is: multiple tag reading).
 * So if you stack two or more PICCs on top of each other and present them to the reader, it will first output all
 * details of the first and then the next PICC. Note that this may take some time as all data blocks are dumped, so
 * keep the PICCs at reading distance until complete.
 * 
 * @license Released into the public domain.
 * 
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <MFRC522.h>

void usage() {
	printf("usage : Dumpinfo [-spi |-uart|-i2c]\n");
}
int main(int argc,char *argv[]) {

	MFRC522 *mfrc522;

  	if (argc>1) {
		if (!strcmp(argv[1],"-spi"))
			mfrc522 = new MFRC522(MFRC522::SPI);
		else if (!strcmp(argv[1],"-uart"))
			mfrc522 = new MFRC522(MFRC522::UART);
		else if(!strcmp(argv[1],"-i2c"))
			mfrc522 = new MFRC522(MFRC522::I2C);
        	else {
			usage();
			return -1;
		}
  	}
  	else
		mfrc522 = new MFRC522(MFRC522::SPI);

	mfrc522->PCD_Init();		// Init MFRC522
	mfrc522->PCD_DumpVersionToSerial();	// Show details of PCD - MFRC522 Card Reader details
	printf("\nScan PICC to see UID, SAK, type, and data blocks...\n");

        while(1) {
	usleep(1000);
	// Look for new cards
	if ( ! mfrc522->PICC_IsNewCardPresent()) {
		continue;
	}
	// Select one of the cards
	if ( ! mfrc522->PICC_ReadCardSerial()) {
		continue;
	}
	// Dump debug info about the card; PICC_HaltA() is automatically called
	mfrc522->PICC_DumpToSerial(&(mfrc522->uid));
        }
}
