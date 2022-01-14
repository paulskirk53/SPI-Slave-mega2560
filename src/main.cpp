#include <SPI.h>

// this is the mega2560 slave code
// assumes two mega2560s connected correctly with MISO, MOSI, SS, CLK and GND lines

//  from master
volatile char SPIReceipt;          // contains the character received from the master SPI transaction

//to be returned
volatile byte highByteReturn ;     // used to hold the high byte of a sixteen bit UINT
volatile byte lowByteReturn ;      // used to hold the low byte of a sixteen bit UINT

void setup()
{
  Serial.begin(9600);              // just in case some debug is required, otherwise not used
  lowByteReturn = lowByte(271);
  highByteReturn = highByte(271);

  // set the control register bits for  SPI interrupt and SPI enable
  SPCR = (1 << SPIE) | (1 << SPE) ;

  // send data on MISO
  pinMode (MISO, OUTPUT);

   // interrupt for SPI servicing
  SPI.attachInterrupt();

  // shove the low byte into the data register in readiness for the 1st master transaction.

} //end setup


// SPI interrupt routine
ISR (SPI_STC_vect)
{

// All the SPI transaction processing is handled in this interrupt vector

  SPIReceipt = SPDR;        // this is what comes in from the master transaction - either A, L, or H
  
  if (SPIReceipt == 'A')    // a dummy transaction which loads the SPDR with the low byte
  {                         // in readiness for the 'L' transaction
    SPDR = lowByteReturn;
  }
  if (SPIReceipt == 'L')    // low byte is returned and SPDR is loaded with the high byte
  {                         // in readiness for the 'H' transaction
    SPDR = highByteReturn;
  }

  if (SPIReceipt == 'H')     // High byte is returned and SPDR is loaded with zero
  {                          // in readiness for the 'A' (dummy) transaction
    SPDR = 0x00;    //fill spdr with 0
  }


}  // end of interrupt routine SPI_STC_vect




void loop()
{
  //nothing to do in loop - the interrupt sends the azimuth chars to the master (stepper routine in this case)

}