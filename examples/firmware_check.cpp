#include <stdio.h>
#include <MFRC522.h>

MFRC522 mfrc522;  // Create MFRC522 instance

/**
 * Check firmware 
 */
int main(int argc, char**argv) {
  
  mfrc522.PCD_Init();   // Init MFRC522 module
  
  printf("*****************************\nMFRC522 Digital self test\n*****************************\n");
  mfrc522.PCD_DumpVersionToSerial();  // Show version of PCD - MFRC522 Card Reader
  printf("\n-----------------------------\nOnly known versions supported\n-----------------------------\nPerforming test...\n");
  bool result = mfrc522.PCD_PerformSelfTest(); // perform the test
  printf("-----------------------------\nResult: ");
  if (result)
    printf("OK\n");
  else
    printf("DEFECT or UNKNOWN\n");
}
