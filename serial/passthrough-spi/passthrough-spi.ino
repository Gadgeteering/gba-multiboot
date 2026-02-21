
// HardwareSPI pins, chosen arbitrarily


#include <SPI.h>


// http://problemkaputt.de/gbatek.htm (Multiboot Transfer Protocol)
//
//  Pin    SPI    GBA
//  -----------------
//  12     miso   SO
//  11     mosi   SI
//  13     sck    SC

void setup() {
  
  pinMode(LED_BUILTIN, OUTPUT);
  //digitalWrite(LED_BUILTIN,LOW );
  // Initiate serial connection through built-in USB
  Serial.begin(9600);
  Serial.setTimeout(-1);
  
  SPI.begin();
  SPI.beginTransaction (SPISettings (256000, MSBFIRST, SPI_MODE3));
  
  loader();
  delay(200);

}




uint32_t spi_transmit_receive32(uint32_t tx)
{
  uint32_t rx[4];

  rx[0] = SPI.transfer((tx >> 24) & 0xFF);
  rx[1] = SPI.transfer((tx >> 16) & 0xFF);
  rx[2] = SPI.transfer((tx >> 8) & 0xFF);
  rx[3] = SPI.transfer(tx & 0xFF);

  //delayMicroseconds(36);
  //delayMicroseconds(10);

  return rx[3] | (rx[2] << 8) | (rx[1] << 16) | (rx[0] << 24);
}


void spi_transmit32(uint32_t tx)
{
  SPI.transfer((tx >> 24) & 0xFF);
  SPI.transfer((tx >> 16) & 0xFF);
  SPI.transfer((tx >> 8) & 0xFF);
  SPI.transfer(tx & 0xFF);

  //delayMicroseconds(36);
  //delayMicroseconds(10);
}


void setLedEnabled(bool enabled)
{
  digitalWrite(LED_BUILTIN, enabled ? HIGH : LOW);
}


uint32_t WriteSPI32NoDebug(uint32_t w)
{
  return spi_transmit_receive32(w);
}


uint32_t WriteSPI32(uint32_t w, const char* msg)
{
  uint32_t r = WriteSPI32NoDebug(w);

  char buf[32];
  sprintf(buf, "0x%08x 0x%08x  ; ", r, w);
  Serial.print(buf);
  Serial.println(msg);
  return  r;
}


void WaitSPI32(uint32_t w, uint32_t comp, const char* msg)
{
  char buf[32];
  sprintf(buf, " 0x%08x\n", comp);
  Serial.print(msg);
  Serial.print(buf);
  
  uint32_t r;

  do
  {
    r = WriteSPI32NoDebug(w);

  } while(r != comp);
}

void loop() {
  
char bytes[4];
uint32_t data;
uint32_t r;
while(1){
// Exchange bytes, if available
if (Serial.available() >= 4) {
    
    // Get 32-bits data chunk
    Serial.readBytes(bytes, 4);
    data = bytes[0] | bytes[1] << 8 | bytes[2] << 16 | bytes[3] << 24;
    // Transfer over SPI
    
    data = WriteSPI32NoDebug(data);

    // Send back to host
    bytes[0] = data & 0xFF;
    bytes[1] = (data >> 8) & 0xFF;
    bytes[2] = (data >> 16) & 0xFF;
    bytes[3] = (data >> 24) & 0xFF;
    Serial.write(bytes, 4);
    Serial.flush();
    }
}   
  
}
void loader(void){
char bytes[4];
uint32_t data;
uint32_t r;
while(1){
// Exchange bytes, if available
if (Serial.available() >= 4) {
    
    // Get 32-bits data chunk
    Serial.readBytes(bytes, 4);
    data = bytes[0] | bytes[1] << 8 | bytes[2] << 16 | bytes[3] << 24;
    // Transfer over SPI
    
    data = WriteSPI32NoDebug(data);

    // Send back to host
    bytes[0] = data & 0xFF;
    bytes[1] = (data >> 8) & 0xFF;
    bytes[2] = (data >> 16) & 0xFF;
    bytes[3] = (data >> 24) & 0xFF;
    Serial.write(bytes, 4);
    Serial.flush();
    }
  }
}   
