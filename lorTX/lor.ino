#define REG_FIFO                 0x00
#define REG_OP_MODE              0x01
#define REG_FRF_MSB              0x06
#define REG_FRF_MID              0x07
#define REG_FRF_LSB              0x08
#define REG_PA_CONFIG            0x09
#define REG_LR_OCP         0X0b
#define REG_LNA                  0x0c
#define REG_FIFO_ADDR_PTR        0x0d
#define REG_FIFO_TX_BASE_ADDR    0x0e
#define REG_FIFO_RX_BASE_ADDR    0x0f
#define REG_FIFO_RX_CURRENT_ADDR 0x10
#define REG_IRQ_FLAGS            0x12
#define REG_RX_NB_BYTES          0x13
#define REG_PKT_RSSI_VALUE       0x1a
#define REG_PKT_SNR_VALUE        0x1b
#define REG_MODEM_CONFIG_1       0x1d
#define REG_MODEM_CONFIG_2       0x1e
#define REG_PREAMBLE_MSB         0x20
#define REG_PREAMBLE_LSB         0x21
#define REG_PAYLOAD_LENGTH       0x22
#define REG_MODEM_CONFIG_3       0x26
#define REG_RSSI_WIDEBAND        0x2c
#define REG_DETECTION_OPTIMIZE   0x31
#define REG_DETECTION_THRESHOLD  0x37
#define REG_SYNC_WORD            0x39
#define REG_DIO_MAPPING_1        0x40
#define REG_VERSION              0x42

// modes
#define MODE_LONG_RANGE_MODE     0x80
#define MODE_SLEEP               0x00
#define MODE_STDBY               0x01
#define MODE_TX                  0x03
#define MODE_RX_CONTINUOUS       0x05
#define MODE_RX_SINGLE           0x06

// PA config
//#define PA_BOOST                 0x80
//#define RFO                      0x70
// IRQ masks
#define IRQ_TX_DONE_MASK           0x08
#define IRQ_PAYLOAD_CRC_ERROR_MASK 0x20
#define IRQ_RX_DONE_MASK           0x40


#define MAX_PKT_LENGTH           255

int packetIndex=0;
byte onRec=NULL;


  void lorsetup()
  {
    //sleep
    writeRegister(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_SLEEP);
    
    //set freqency 868 (d90000)
      writeRegister(REG_FRF_MSB, 0xd9);
      writeRegister(REG_FRF_MID, 0x00);
      writeRegister(REG_FRF_LSB, 0x00);
  
  // set base addresses
    writeRegister(REG_FIFO_TX_BASE_ADDR, 0);
    writeRegister(REG_FIFO_RX_BASE_ADDR, 0);
  // set LNA boost
    writeRegister(REG_LNA, readRegister(REG_LNA) | 0x03);
  // set auto AGC
    writeRegister(REG_MODEM_CONFIG_3, 0x04); 
    
    writeRegister(REG_PA_CONFIG,0x8F); //outputpower 17dBm
  //setSpreadingFactor(11);
  
  //idle
  writeRegister(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_STDBY);


  //writeRegister(REG_DIO_MAPPING_1, 0x00);
  //writeRegister(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_RX_CONTINUOUS);
  
  }


void sendwr(char *byt,size_t siz)
{ 
  //// begin
  writeRegister(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_STDBY);
  // reset FIFO address and paload length
  writeRegister(REG_FIFO_ADDR_PTR, 0);
  writeRegister(REG_PAYLOAD_LENGTH, 0);
  //// begin ////end


  /// write to fifp

  writeRegister(REG_PAYLOAD_LENGTH, siz);
  
  for (size_t i = 0; i < siz; i++) {
    writeRegister(REG_FIFO, byt[i]);
    //Serial.print((char)byt[i]);
  }
  /// end wrtofifo

  

  /// endpak
  writeRegister(REG_DIO_MAPPING_1, 0x40); // DIO0 => TXDONE
  
  // put in TX mode
  writeRegister(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_TX);

    //waitTXdone
    while ((readRegister(REG_IRQ_FLAGS) & IRQ_TX_DONE_MASK) == 0) {
      delay(100); //да не хорошая штука
    }
    // clear IRQ's
    writeRegister(REG_IRQ_FLAGS, IRQ_TX_DONE_MASK);
   /// endpak ////end
}

void handread ()
{
  int packetLength = readRegister(REG_RX_NB_BYTES);
  Serial.println("Paklengh  ");
  Serial.print(packetLength);
  Serial.println(" ");
  Serial.println("FIFO  ");
  readFifo(REG_FIFO,RX_BUF,packetLength);
  for(int i = 0; i < packetLength; i++)
    {
      Serial.print((char)RX_BUF[i]);
    }

  writeRegister(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_SLEEP);
  writeRegister(REG_FIFO_ADDR_PTR, 0);
  writeRegister(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_RX_CONTINUOUS);
  
}
