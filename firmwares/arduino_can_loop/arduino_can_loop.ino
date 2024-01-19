
#include <mcp_can.h>
#include <SPI.h>

/*SAMD core*/
#ifdef ARDUINO_SAMD_VARIANT_COMPLIANCE
  #define SERIAL SerialUSB
#else
  #define SERIAL Serial
#endif

// the cs pin of the version after v1.1 is default to D9
// v0.9b and v1.0 is default D10
const int SPI_CS_PIN = 10;

MCP_CAN CAN(SPI_CS_PIN);                                    // Set CS pin

void setup()
{
    byte stmp[8] = {1,2,3,4,5,6,7,8};
    unsigned long canId_TX = 123;

    unsigned char len = 0;
    unsigned char buf[8];
    byte res;
    
    SERIAL.begin(115200);
   
    while (CAN_OK != CAN.begin(CAN_1000KBPS))              // init can bus : baudrate = 1000KBPS
    {
        SERIAL.println("CAN BUS Shield init fail");
        SERIAL.println(" Init CAN BUS Shield again");
        delay(100);
    }
    SERIAL.println("CAN BUS Shield init ok!");
    while (CAN.setMode(MODE_LOOPBACK) != MCP2515_OK)
      ;
    SERIAL.println("CAN BUS in the Loop Back Mode!");
   
    while (CAN.sendMsgBuf(canId_TX, 0, 0, 8, stmp, true) != CAN_OK)
      ;
    SERIAL.println("CAN message transmitted!");
    
    while((res = CAN.checkReceive()) != CAN_MSGAVAIL)
      SERIAL.println(res);
      
    unsigned long canId_RX = CAN.getCanId();
    
    CAN.readMsgBuf(&len, buf);

   
    if( canId_RX ==  canId_TX)
    {
      for(int i = 0; i < len; i++)
        SERIAL.println(buf[i]);
    }
}


void loop()
{
    
}

// END FILE
