/*

Survey Receiver Controller

Copyright 2016 AA Engineering LLC

*/


#include <stdarg.h>
#include "nrf8001-ble-uart-spark-io.h"
#include "NmeaParser.h"
#include "PvtManager.h"

#define MANUAL_SS_PIN A2
#define PRINTF_BUFFER_SIZE 80

#define PIN_BLE_READY D0
#define PIN_BLE_ACTIVITY D1
#define PIN_BLE_RESET D2

#define BLE_UART_MAX_PACKET_SIZE 20

void Serial_printf(const char* fmt, ...);
void mySprintf(char *buff, const char *fmt, ...);
void sendBleMessage(uint8_t *buffer, int len);
void nRF8001BleUartRx(uint8_t *buffer, uint8_t len);
void nmeaSentenceCallback(nmea_sentence_t *sentence);
void addMessageToBleTxBuffer(uint8_t *message, int len);

#define BLE_TX_BUFFER_SIZE 500

typedef struct {
    int txCount;
    char buffer[PRINTF_BUFFER_SIZE];
    nmea_sentence_t incomingNmeaSentence;
    uint8_t bleTxBuffer[BLE_TX_BUFFER_SIZE];
    int bleTxBufferSize;
} state_data_t;

state_data_t state;

void setup()
{
    Serial.begin(115200);

    state.bleTxBufferSize = 0;

    Serial.println("");
    Serial.println("");
    Serial.println("==========================================");
    Serial.println("");

    state.txCount = 0;


    int counter = 10;
    while (counter-- > 0){
        delay(1000);
        Serial.print("Starting up in: ");
        Serial.println(counter);
    }

    /*
    pinMode(PIN_BLE_READY,INPUT);
    pinMode(PIN_BLE_ACTIVITY,INPUT);
    pinMode(PIN_BLE_RESET,OUTPUT);
    digitalWrite(PIN_BLE_RESET,HIGH);
    */


    Serial1.begin(9600);

    nmeaParserInit(nmeaSentenceCallback,&state.incomingNmeaSentence,Serial_printf);

  	// Do the BLE UART setup:
  	nRF8001BleUartSetup();
    nRF8001BleUartNameSet("AA-GPS",6);


    //enableSPI();
}

//byte rxBuffer[120];
//int rxBufferSize = 0;

void loop()
{

    while (Serial1.available())
    {
        byte inByte = Serial1.read();
        //Serial.write(inByte);
        //sendByte(inByte);

        receiveNmeaByte((char)inByte);


        //rxBuffer[rxBufferSize++] = inByte;

        //if (rxBufferSize == 120){
            //Serial.write(rxBuffer, rxBufferSize);
        		//nRF8001BleUartTx(rxBuffer, rxBufferSize);
            //sendBleMessage(rxBuffer, rxBufferSize);
        //    rxBufferSize = 0;
        //}



    }

    if (state.bleTxBufferSize > 0 &&
        nRF8001BleUartConneted() &&
        nRF8001BleUartTxReady()){

        sendNextBleQueuedMessage();
    }

    // Run the BLE UART loop once in every loop,
  	// to let it handle any BLE events
  	nRF8001BleUartLoop();

    // test the BLE interface with dummy data
    //mySprintf(state.buffer,"Dummy Sentence %i*2A\n",state.txCount++);
    //sendString(state.buffer);

    //delay(1000);

}

void nmeaSentenceCallback(nmea_sentence_t *sentence)
{
    //Serial.write("Received sentence: ");
    //Serial.write(sentence->bytes,sentence->length);



    switch(sentence->type){
        case NMEA_SENTENCE_TYPE_RMC: {
            //Serial.write("Received RMC: ");
            //Serial.write(sentence->bytes,sentence->length);
            //nRF8001BleUartTx(sentence->bytes,sentence->length);
            //sendBleMessage(sentence->bytes,sentence->length);

            if (nRF8001BleUartConneted()){
                //Serial.println("Queueing RMC sentence...");
                addMessageToBleTxBuffer(sentence->bytes,sentence->length);
            } else {
                Serial.println("BLE not connected, discarding RMC sentence.");
            }
        }
        break;
        default: {
            // Discard
        }
    }
}

void sendNextBleQueuedMessage()
{
    int messageSize;
    bool txResult;

    if (state.bleTxBufferSize < BLE_UART_MAX_PACKET_SIZE){
        messageSize = state.bleTxBufferSize;
    } else {
        messageSize = BLE_UART_MAX_PACKET_SIZE;
    }

    /*
    Serial.print(millis());
    Serial.print(": ");
    Serial.print("Sending next BLE message: ");
    Serial.print(messageSize);
    Serial.println(" chars.");
    */

    txResult = nRF8001BleUartTx(state.bleTxBuffer,messageSize);

    if (txResult){
        state.bleTxBufferSize -= messageSize;

        // copy remaining bytes down
        memcpy(state.bleTxBuffer,&state.bleTxBuffer[messageSize],state.bleTxBufferSize);

        Serial.print("Sent message, ");
        Serial.print(state.bleTxBufferSize);
        Serial.println(" chars left in queue.");
    } else {
        Serial.println("Packet send failed, going to retry.");
    }


}

void addMessageToBleTxBuffer(uint8_t *message, int len)
{
    if (state.bleTxBufferSize + len < BLE_TX_BUFFER_SIZE){
        memcpy(&state.bleTxBuffer[state.bleTxBufferSize],message,len);
        state.bleTxBufferSize += len;
        Serial_printf("Added %i chars to ble tx queue.",len);
    } else {
        Serial.println("Dropped message due to overfull buffer.");
    }
}


#define WAIT_FOR_TX_PERIOD_MS 5
#define WAIT_TIMEOUT 1000
//uint8_t bleTxBuffer[120];
void sendBleMessage(uint8_t *buffer, int len)
{
    //memcpy(bleTxBuffer,buffer,len);
  uint8_t txBuffer[BLE_UART_MAX_PACKET_SIZE];
  int numPackets = len / BLE_UART_MAX_PACKET_SIZE;
  int packetSize;
  int waitDuration;

  if (numPackets * BLE_UART_MAX_PACKET_SIZE < len){
      numPackets++;
  }

  Serial.print("Sending BLE message with ");
  Serial.print(len);
  Serial.print(" chars in ");
  Serial.print(numPackets);
  Serial.println(" packets.");

  for (int packetIdx=0; packetIdx < numPackets; packetIdx++){
    if (packetIdx < numPackets - 1){
      packetSize = BLE_UART_MAX_PACKET_SIZE;
    } else {
      packetSize = len - (packetIdx * BLE_UART_MAX_PACKET_SIZE);
    }
    memcpy(txBuffer,&buffer[packetIdx * BLE_UART_MAX_PACKET_SIZE],packetSize);

    nRF8001BleUartTx(txBuffer, packetSize);
    /*
  	while(!nRF8001BleUartTxReady()){
  	     nRF8001BleUartLoop();
        delay(1);
        Serial.println("Waiting for credit...");
    }
    */
  }
}


/**
* Implementation of RX callback function
* Handles any incoming data from the BLE UART
*
* Simply sends all messages directly to terminal
*/
void nRF8001BleUartRx(uint8_t *buffer, uint8_t len)
{
	Serial.print("Received BLE data: ");

	for (uint8_t i = 0; i < len; ++i)
	{
		Serial.print((char) buffer[i]);
	}

	Serial.print("\n");
}

/*
void enableSPI()
{
    SPI.begin();
    delay(100);
    SPI.setBitOrder(MSBFIRST);
    SPI.setClockDivider(SPI_CLOCK_DIV64);  // Want about 1 MHz

    // USE SPI MODE 0
    // https://en.wikipedia.org/wiki/Serial_Peripheral_Interface_Bus#Mode_numbers
    // FROM GP22 DATA SHEET: The TDC-GP22 does only support the following SPI mode (Motorola specification)*:
    // Clock Phase Bit = 1 Clock Polarity Bit = 0
    SPI.setDataMode(SPI_MODE1);


    digitalWrite(MANUAL_SS_PIN,LOW);
}

void disableSPI()
{
    SPI.end();

    digitalWrite(MANUAL_SS_PIN,HIGH);
}*/

void Serial_printf(const char* fmt, ...)
{
   char buff[PRINTF_BUFFER_SIZE];
   va_list args;
   va_start(args, fmt);
   vsnprintf(buff, PRINTF_BUFFER_SIZE, fmt, args);
   va_end(args);
   Serial.println(buff);
}

void mySprintf(char *buff, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vsnprintf(buff, PRINTF_BUFFER_SIZE, fmt, args);
    va_end(args);
}

/*
void sendString(char *string)
{
    int len = strlen(string);
    for (int ii=0; ii<len; ii++){
        sendByte(string[ii]);
    }
}

byte sendByte(byte data)
{

    //Serial.print(data,HEX);

    byte response;
    response = SPI.transfer(data);

    return response;

}
*/
