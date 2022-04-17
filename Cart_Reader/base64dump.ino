#include "SdFat.h"
#include "base64.hpp";

unsigned char charBuffer[64];
unsigned char base64Buffer[96];

void base64_dump(char* inputFile, char* type) {
  if (!sd.exists(inputFile)) {
    return;
  }
  delay(3000);
  display_Clear();
  println_Msg(F("SERIAL DUMP START"));
  println_Msg(F(""));
  display_Update();
  Serial.println();
  Serial.print(F("BASE64 DUMP START <<< "));
  Serial.println(type);
    FsFile nesFile = sd.open(inputFile, FILE_READ);
    size_t n;
    while ((n = nesFile.read(sdBuffer, sizeof(sdBuffer))) > 0) {
      base64_serial_out(n);
    }
    nesFile.close();
  Serial.print(type);
  Serial.println(F(" >>> BASE64 DUMP END"));
  Serial.println();
  println_Msg(F("SERIAL DUMP END"));
  println_Msg(F(""));
  display_Update();
  delay(3000);
  display_Clear();
}

void base64_serial_out(size_t n) {
  int m = n;
  for(int j=0;j<8;j++){
    for(int i=0;i<64;i++){
      charBuffer[i] = sdBuffer[i+64*j];
    }
    if(m<=64){
      encode_base64(charBuffer, m, base64Buffer);
      Serial.println((char *) base64Buffer);
      break;
    }else{
      encode_base64(charBuffer, 64, base64Buffer);
      Serial.println((char *) base64Buffer);
    }
    m = m - 64;
  }
}
