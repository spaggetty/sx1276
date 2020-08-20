#include <Adafruit_BME280.h>
#include <SPI.h>
#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 bme;


int counter = 0;

String rezul;
int t;
int p;

#define ss 15
#define rest 2
#define dio0 0

unsigned char msg[] = "This_is_message!";

void setup() {
  Serial.begin(115200);
  while (!Serial);


  bool status;

  // bme
  status = bme.begin(0x76);  
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }


  
 

  pinMode(ss, OUTPUT);
  pinMode(rest, OUTPUT);
  pinMode(dio0, INPUT);

   // perform rest
  digitalWrite(rest, LOW);
  delay(20);
  digitalWrite(rest, HIGH);
  delay(50);
  // set SS high
  digitalWrite(ss, HIGH);
  
  SPI.begin();

  lorsetup();

  //attachInterrupt(digitalPinToInterrupt(dio0), handread, RISING);  //не робит на esp8266 но на TX и не использую
  
}

void loop() {
  
    t=bme.readTemperature();
    p=bme.readPressure() / 100.0F;
    rezul="";
    rezul+="T= ";
    rezul+=t;
    rezul+=" P= ";
    rezul+=p;

    
    //Serial.println(rezul);

    //Serial.println(Sendiz);
    char Test[rezul.length()];
    rezul.toCharArray(Test,rezul.length()+1);
    sendwr(Test,rezul.length()+1);
    //Serial.println(sizeof(TX_BUF));
    counter++;
    Serial.println(" ");
 
    delay(4000);
      
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
