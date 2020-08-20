#include <SPI.h>
#include "heltec.h" 

int counter = 0;
String t;
String p;
bool stat = false;
#define ss 18
#define reset  14
#define dio0   26

unsigned char msg[] = "This_is_message!";

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("LoRa begin");

  //oled go
  Heltec.begin(true /*DisplayEnable Enable*/, false /*LoRa Disable*/, false /*Serial Enable*/); //Use only oled in heltec library
  Heltec.display->flipScreenVertically();
  Heltec.display->setFont(ArialMT_Plain_10);


  
  pinMode(ss, OUTPUT);
  pinMode(reset, OUTPUT);
  pinMode(dio0, INPUT);

   // perform reset
  digitalWrite(reset, LOW);
  delay(20);
  digitalWrite(reset, HIGH);
  delay(50);
  // set SS high
  digitalWrite(ss, HIGH);
  
  SPI.begin();

  lorsetup();

  attachInterrupt(digitalPinToInterrupt(dio0), handread, RISING);  //прерывание по ножке


}

void loop() {
    
    if(stat)
    {
      OLED();
      stat=false;
    }
}

void OLED()
{
  t ="Temperature=";
  t+=(char)RX_BUF[3];
  t+=(char)RX_BUF[4];
  t+="C";
  Serial.println(t);
  p ="Pressure=";
  p+=(char)RX_BUF[9];
  p+=(char)RX_BUF[10];
  p+=(char)RX_BUF[11];
  p+="hPa";
  Serial.println(p);
  Heltec.display->clear();
    Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
    Heltec.display->setFont(ArialMT_Plain_16);
    Heltec.display->drawString(0, 10, t);
    Heltec.display->setFont(ArialMT_Plain_16);
    Heltec.display->drawString(0, 30, p);
    Heltec.display->display();
}

void writeRegister(uint8_t address, uint8_t value)
{
  singleTransfer(address | 0x80, value);
}


uint8_t readRegister(uint8_t address)
{
    return singleTransfer(address & 0x7f, 0x00);
}


uint8_t singleTransfer(uint8_t address, uint8_t value)
{
  uint8_t response;
  digitalWrite(ss, LOW);
  SPI.beginTransaction(SPISettings(8E6, MSBFIRST, SPI_MODE0));
  SPI.transfer(address);
  response = SPI.transfer(value);
  SPI.endTransaction();
  digitalWrite(ss, HIGH);
  return response;
}


// KOCTYLb
void readFifo(uint8_t reg, unsigned char *buff, uint8_t siz)
  {
    readbr( reg, buff, siz);
  }

void readbr (uint8_t reg, uint8_t *buff, uint8_t len)
{
  digitalWrite(ss, LOW);
  SPI.beginTransaction(SPISettings(8E6, MSBFIRST, SPI_MODE0));
  SPI.transfer(reg);

   for (uint8_t i = 0; i < len; i++)
    {
      buff[i] = SPI.transfer(0);
    }
  
  SPI.endTransaction();
  digitalWrite(ss, HIGH);
}

