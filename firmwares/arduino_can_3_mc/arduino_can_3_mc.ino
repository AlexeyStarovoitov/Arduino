// include the library code:
#include <LiquidCrystal.h>
#include <mcp_can.h>
#include <SPI.h>

#define RX_ID 124
#define TX_ID 126

/*SAMD core*/
#ifdef ARDUINO_SAMD_VARIANT_COMPLIANCE
  #define SERIAL SerialUSB
#else
  #define SERIAL Serial
#endif

const int SPI_CS_PIN = 10;

MCP_CAN CAN(SPI_CS_PIN);                                    // Set CS pin

const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() 
{
  SERIAL.begin(115200);
   
  while (CAN_OK != CAN.begin(CAN_500KBPS, MCP_8MHz))              // init can bus : baudrate = 500KBPS
   {
       SERIAL.println("CAN BUS Shield init fail");
       SERIAL.println("Init CAN BUS Shield again");
       delay(100);
   }
   SERIAL.println("CAN BUS Shield init ok!");

   
  
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  
}

unsigned char len = 0;
unsigned char buf[8];
byte ans = 0;

void loop() 
{
  while(CAN.checkReceive() != CAN_MSGAVAIL)
    ;
    
  CAN.readMsgBuf(&len, buf);
  unsigned long canId_RX = CAN.getCanId();
  unsigned long canId_TX = TX_ID;   
  
  if(canId_RX ==   RX_ID)
  {
   lcd.setCursor(0, 0);
   lcd.print(buf[0]);
  
   ans = buf[0];
   
   while (CAN.sendMsgBuf(canId_TX, 0, 0, 1, &ans, true) != CAN_OK)
      ;
   
   SERIAL.println("CAN message transmitted!");
  }

}
