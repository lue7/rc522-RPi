#include <stdio.h>
#include <MFRC522.h>
#include <string.h>

/**
 * Check firmware 
 */
void usage() {
	printf("usage : firmware_check [-spi |-uart|-i2c]\n");
}
int main(int argc, char**argv) {

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


  mfrc522->PCD_Init();   // Init MFRC522 module
  
  printf("*****************************\nMFRC522 Digital self test\n*****************************\n");
  mfrc522->PCD_DumpVersionToSerial();  // Show version of PCD - MFRC522 Card Reader
  printf("\n-----------------------------\nOnly known versions supported\n-----------------------------\nPerforming test...\n");
  bool result = mfrc522->PCD_PerformSelfTest(); // perform the test
  printf("-----------------------------\nResult: ");
  if (result)
    printf("OK\n");
  else
    printf("DEFECT or UNKNOWN\n");
  delete mfrc522;
  return 0;
}
