//******************************************
// GAME BOY MODULE
//******************************************

#include "options.h"
#ifdef enable_GBX

/******************************************
   Variables
 *****************************************/
// Game Boy
int sramBanks;
int romBanks;
uint16_t sramEndAddress = 0;

/******************************************
   Menu
 *****************************************/
// GBx start menu
static const char gbxMenuItem1[] PROGMEM = "Game Boy (Color)";
static const char gbxMenuItem2[] PROGMEM = "Game Boy Advance";
static const char gbxMenuItem3[] PROGMEM = "Flash GBC Cart";
static const char gbxMenuItem4[] PROGMEM = "Reset";
static const char* const menuOptionsGBx[] PROGMEM = {gbxMenuItem1, gbxMenuItem2, gbxMenuItem3, gbxMenuItem4};

// GB menu items
static const char GBMenuItem1[] PROGMEM = "Read Rom";
static const char GBMenuItem2[] PROGMEM = "Read Save";
static const char GBMenuItem3[] PROGMEM = "Write Save";
static const char GBMenuItem4[] PROGMEM = "Reset";
static const char* const menuOptionsGB[] PROGMEM = {GBMenuItem1, GBMenuItem2, GBMenuItem3, GBMenuItem4};

// GB Flash items
static const char GBFlashItem1[] PROGMEM = "CFI Cart";
static const char GBFlashItem2[] PROGMEM = "CFI Cart and Save";
static const char GBFlashItem3[] PROGMEM = "29F Cart (MBC3)";
static const char GBFlashItem4[] PROGMEM = "29F Cart (MBC5)";
static const char GBFlashItem5[] PROGMEM = "NPower GB Memory";
static const char GBFlashItem6[] PROGMEM = "GB Smart";
static const char GBFlashItem7[] PROGMEM = "Reset";
static const char* const menuOptionsGBFlash[] PROGMEM = {GBFlashItem1, GBFlashItem2, GBFlashItem3, GBFlashItem4, GBFlashItem5, GBFlashItem6, GBFlashItem7};

// Start menu for both GB and GBA
void gbxMenu() {
  // create menu with title and 4 options to choose from
  unsigned char gbType;
  // Copy menuOptions out of progmem
  convertPgm(menuOptionsGBx, 4);
  gbType = question_box(F("Select Game Boy"), menuOptions, 4, 0);

  // wait for user choice to come back from the question box menu
  switch (gbType)
  {
    case 0:
      display_Clear();
      display_Update();
      setup_GB();
      mode =  mode_GB;
      break;

    case 1:
      display_Clear();
      display_Update();
      setup_GBA();
      mode =  mode_GBA;
      break;

    case 2:
      // create submenu with title and 7 options to choose from
      unsigned char gbFlash;
      // Copy menuOptions out of progmem
      convertPgm(menuOptionsGBFlash, 7);
      gbFlash = question_box(F("Select type"), menuOptions, 7, 0);

      // wait for user choice to come back from the question box menu
      switch (gbFlash)
      {
        case 0:
          // Flash CFI
          display_Clear();
          display_Update();
          setup_GB();
          mode =  mode_GB;

          // Change working dir to root
          sd.chdir("/");
          // Launch filebrowser
          filePath[0] = '\0';
          sd.chdir("/");
          fileBrowser(F("Select file"));
          display_Clear();
          identifyCFI_GB();
          if (!writeCFI_GB()) {
            display_Clear();
            println_Msg(F("Flashing failed, time out!"));
            println_Msg(F("Press button..."));
            display_Update();
            wait();
          }
          // Reset
          wait();
          resetArduino();
          break;

        case 1:
          // Flash CFI and Save
          display_Clear();
          display_Update();
          setup_GB();
          mode =  mode_GB;

          // Change working dir to root
          sd.chdir("/");
          // Launch filebrowser
          filePath[0] = '\0';
          sd.chdir("/");
          fileBrowser(F("Select file"));
          display_Clear();
          identifyCFI_GB();
          if (!writeCFI_GB()) {
            display_Clear();
            println_Msg(F("Flashing failed, time out!"));
            println_Msg(F("Press button..."));
            display_Update();
            wait();
            resetArduino();
          }
          getCartInfo_GB();
          // Does cartridge have SRAM
          if (sramEndAddress > 0) {
            // Remove file name ending
            int pos = -1;
            while (fileName[++pos] != '\0') {
              if (fileName[pos] == '.') {
                fileName[pos] = '\0';
                break;
              }
            }
            sprintf(filePath, "/GB/SAVE/%s/", fileName);
            bool saveFound = false;
            if (sd.exists(filePath)) {
              EEPROM_readAnything(0, foldern);
              for (int i = foldern; i >= 0; i--) {
                sprintf(filePath, "/GB/SAVE/%s/%d/%s.SAV", fileName, i, fileName);
                if (sd.exists(filePath)) {
                  print_Msg(F("Save number "));
                  print_Msg(i);
                  println_Msg(F(" found."));
                  saveFound = true;
                  sprintf(filePath, "/GB/SAVE/%s/%d", fileName, i);
                  sprintf(fileName, "%s.SAV", fileName);
                  writeSRAM_GB();
                  unsigned long wrErrors;
                  wrErrors = verifySRAM_GB();
                  if (wrErrors == 0) {
                    println_Msg(F("Verified OK"));
                    display_Update();
                  }
                  else {
                    print_Msg(F("Error: "));
                    print_Msg(wrErrors);
                    println_Msg(F(" bytes "));
                    print_Error(F("did not verify."), false);
                  }
                  break;
                }
              }
            }
            if (!saveFound) {
              println_Msg(F("Error: No save found."));
            }
          }
          else {
            print_Error(F("Cart has no Sram"), false);
          }
          // Reset
          wait();
          resetArduino();
          break;

        case 2:
          //Flash MBC3
          display_Clear();
          display_Update();
          setup_GB();
          mode =  mode_GB;

          // Change working dir to root
          sd.chdir("/");
          //MBC3
          writeFlash29F_GB(3);
          // Reset
          wait();
          resetArduino();
          break;

        case 3:
          //Flash MBC5
          display_Clear();
          display_Update();
          setup_GB();
          mode =  mode_GB;

          // Change working dir to root
          sd.chdir("/");
          //MBC5
          writeFlash29F_GB(5);
          // Reset
          wait();
          resetArduino();
          break;

        case 4:
          // Flash GB Memory
          display_Clear();
          display_Update();
          setup_GBM();
          mode =  mode_GBM;
          break;

        case 5:
          // Flash GB Smart
          display_Clear();
          display_Update();
          setup_GBSmart();
          mode = mode_GB_GBSmart;
          break;

        case 6:
          resetArduino();
          break;
      }
      break;

    case 3:
      resetArduino();
      break;
  }
}

void gbMenu() {
  // create menu with title and 3 options to choose from
  unsigned char mainMenu;
  // Copy menuOptions out of progmem
  convertPgm(menuOptionsGB, 4);
  mainMenu = question_box(F("GB Cart Reader"), menuOptions, 4, 0);

  // wait for user choice to come back from the question box menu
  switch (mainMenu)
  {
    case 0:
      display_Clear();
      // Change working dir to root
      sd.chdir("/");
      readROM_GB();
      compare_checksum_GB();
      break;

    case 1:
      display_Clear();
      // Does cartridge have SRAM
      if (sramEndAddress > 0) {
        // Change working dir to root
        sd.chdir("/");
        readSRAM_GB();
      }
      else {
        print_Error(F("Cart has no Sram"), false);
      }
      break;

    case 2:
      display_Clear();
      // Does cartridge have SRAM
      if (sramEndAddress > 0) {
        // Change working dir to root
        sd.chdir("/");
        filePath[0] = '\0';
        fileBrowser(F("Select sav file"));
        writeSRAM_GB();
        unsigned long wrErrors;
        wrErrors = verifySRAM_GB();
        if (wrErrors == 0) {
          println_Msg(F("Verified OK"));
          display_Update();
        }
        else {
          print_Msg(F("Error: "));
          print_Msg(wrErrors);
          println_Msg(F(" bytes "));
          print_Error(F("did not verify."), false);
        }
      }
      else {
        print_Error(F("Cart has no Sram"), false);
      }
      break;

    case 3:
      resetArduino();
      break;
  }
  println_Msg(F(""));
  println_Msg(F("Press Button..."));
  display_Update();
  wait();
}

/******************************************
   Setup
 *****************************************/
void setup_GB() {
  // Set Address Pins to Output
  //A0-A7
  DDRF = 0xFF;
  //A8-A15
  DDRK = 0xFF;

  // Set Control Pins to Output RST(PH0) CS(PH3) WR(PH5) RD(PH6)
  DDRH |= (1 << 0) | (1 << 3) | (1 << 5) | (1 << 6);
  // Output a high signal on all pins, pins are active low therefore everything is disabled now
  PORTH |= (1 << 0) | (1 << 3) | (1 << 5) | (1 << 6);

  // Set Data Pins (D0-D7) to Input
  DDRC = 0x00;
  // Disable Internal Pullups
  //PORTC = 0x00;

  delay(400);

  // Print start page
  getCartInfo_GB();
  showCartInfo_GB();
}

void showCartInfo_GB() {
  display_Clear();
  if (strcmp(checksumStr, "00") != 0) {
    println_Msg(F("GB Cart Info"));
    print_Msg(F("Name: "));
    println_Msg(romName);
    print_Msg(F("Rom Type: "));
    switch (romType) {
      case 0: print_Msg(F("ROM ONLY")); break;
      case 1: print_Msg(F("MBC1")); break;
      case 2: print_Msg(F("MBC1+RAM")); break;
      case 3: print_Msg(F("MBC1+RAM")); break;
      case 5: print_Msg(F("MBC2")); break;
      case 6: print_Msg(F("MBC2")); break;
      case 8: print_Msg(F("ROM+RAM")); break;
      case 9: print_Msg(F("ROM ONLY")); break;
      case 11: print_Msg(F("MMM01")); break;
      case 12: print_Msg(F("MMM01+RAM")); break;
      case 13: print_Msg(F("MMM01+RAM")); break;
      case 15: print_Msg(F("MBC3+TIMER")); break;
      case 16: print_Msg(F("MBC3+TIMER+RAM")); break;
      case 17: print_Msg(F("MBC3")); break;
      case 18: print_Msg(F("MBC3+RAM")); break;
      case 19: print_Msg(F("MBC3+RAM")); break;
      case 21: print_Msg(F("MBC4")); break;
      case 22: print_Msg(F("MBC4+RAM")); break;
      case 23: print_Msg(F("MBC4+RAM")); break;
      case 25: print_Msg(F("MBC5")); break;
      case 26: print_Msg(F("MBC5+RAM")); break;
      case 27: print_Msg(F("MBC5+RAM")); break;
      case 28: print_Msg(F("MBC5+RUMBLE")); break;
      case 29: print_Msg(F("MBC5+RUMBLE+RAM")); break;
      case 30: print_Msg(F("MBC5+RUMBLE+RAM")); break;
      case 252: print_Msg(F("Gameboy Camera")); break;
      default: print_Msg(F("Not found"));
    }
    println_Msg(F(" "));
    print_Msg(F("Rom Size: "));
    switch (romSize) {
      case 0: print_Msg(F("32KB")); break;
      case 1: print_Msg(F("64KB")); break;
      case 2: print_Msg(F("128KB")); break;
      case 3: print_Msg(F("256KB")); break;
      case 4: print_Msg(F("512KB")); break;
      case 5: print_Msg(F("1MB")); break;
      case 6: print_Msg(F("2MB")); break;
      case 7: print_Msg(F("4MB")); break;
      case 82: print_Msg(F("1.1MB")); break;
      case 83: print_Msg(F("1.2MB")); break;
      case 84: print_Msg(F("1.5MB)")); break;
      default: print_Msg(F("Not found"));
    }
    println_Msg(F(""));
    print_Msg(F("Banks: "));
    println_Msg(romBanks);

    print_Msg(F("Sram Size: "));
    switch (sramSize) {
      case 0:
        if (romType == 6) {
          print_Msg(F("512B"));
        }
        else {
          print_Msg(F("None"));
        }
        break;
      case 1: print_Msg(F("2KB")); break;
      case 2: print_Msg(F("8KB")); break;
      case 3: print_Msg(F("32KB")); break;
      case 4: print_Msg(F("128KB")); break;
      default: print_Msg(F("Not found"));
    }
    println_Msg(F(""));
    print_Msg(F("Checksum: "));
    println_Msg(checksumStr);
    display_Update();

    // Wait for user input
    println_Msg(F("Press Button..."));
    display_Update();
    wait();
  }
  else {
    print_Error(F("GAMEPAK ERROR"), true);
  }
}

/******************************************
  Low level functions
*****************************************/
// Switch data pins to read
void dataIn_GB() {
  // Set to Input
  DDRC = 0x00;
}

byte readByte_GB(word myAddress) {
  PORTF = myAddress & 0xFF;
  PORTK = (myAddress >> 8) & 0xFF;

  __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t");

  // Switch RD(PH6) to LOW
  PORTH &= ~(1 << 6);

  __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t");

  // Read
  byte tempByte = PINC;

  // Switch and RD(PH6) to HIGH
  PORTH |= (1 << 6);

  __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t");

  return tempByte;
}

void writeByte_GB(int myAddress, uint8_t myData) {
  PORTF = myAddress & 0xFF;
  PORTK = (myAddress >> 8) & 0xFF;
  PORTC = myData;

  // Arduino running at 16Mhz -> one nop = 62.5ns
  // Wait till output is stable
  __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t");

  // Pull WR(PH5) low
  PORTH &= ~(1 << 5);

  // Leave WE low for at least 60ns
  __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t");

  // Pull WR(PH5) HIGH
  PORTH |= (1 << 5);

  // Leave WE high for at least 50ns
  __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t");
}

byte readByteSRAM_GB(word myAddress) {
  PORTF = myAddress & 0xFF;
  PORTK = (myAddress >> 8) & 0xFF;

  __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t");

  // Pull CS(PH3) LOW
  PORTH &= ~(1 << 3);
  // Pull RD(PH6) LOW
  PORTH &= ~(1 << 6);

  __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t");

  // Read
  byte tempByte = PINC;

  // Pull CS(PH3) HIGH
  PORTH |= (1 << 3);
  // Pull RD(PH6) HIGH
  PORTH |=  (1 << 6);

  __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t");

  return tempByte;
}

void writeByteSRAM_GB(int myAddress, uint8_t myData) {
  PORTF = myAddress & 0xFF;
  PORTK = (myAddress >> 8) & 0xFF;
  PORTC = myData;

  __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t");

  // Pull CS(PH3) LOW
  PORTH &= ~(1 << 3);
  // Pull WR(PH5) low
  PORTH &= ~(1 << 5);

  // Leave WE low for at least 60ns
  __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t");

  // Pull WR(PH5) HIGH
  PORTH |= (1 << 5);
  // Pull CS(PH3) HIGH
  PORTH |= (1 << 3);

  // Leave WE high for at least 50ns
  __asm__("nop\n\t""nop\n\t""nop\n\t""nop\n\t");
}

/******************************************
  Game Boy functions
*****************************************/
// Read Cartridge Header
void getCartInfo_GB() {
  romType = readByte_GB(0x0147);
  romSize = readByte_GB(0x0148);
  sramSize = readByte_GB(0x0149);

  // ROM banks
  romBanks = 2; // Default 32K
  if (romSize >= 1) { // Calculate rom size
    romBanks = 2 << romSize;
  }

  // RAM banks
  sramBanks = 0; // Default 0K RAM
  if (romType == 6) {
    sramBanks = 1;
  }
  switch (sramSize) {
    case 2:
      sramBanks = 1;
      break;
    case 3:
      sramBanks = 4;
      break;
    case 4:
      sramBanks = 16;
      break;
    case 5:
      sramBanks = 8;
      break;
  }

  // RAM end address
  if (romType == 6) {
    sramEndAddress = 0xA1FF;  // MBC2 512bytes (nibbles)
  }
  if (sramSize == 1) {
    sramEndAddress = 0xA7FF;  // 2K RAM
  }
  if (sramSize > 1) {
    sramEndAddress = 0xBFFF;  // 8K RAM
  }

  // Get Checksum as string
  sprintf(checksumStr, "%02X%02X", readByte_GB(0x014E), readByte_GB(0x014F));

  // Get name
  byte myByte = 0;
  byte myLength = 0;

  for (int addr = 0x0134; addr <= 0x13C; addr++) {
    myByte = readByte_GB(addr);
    if (((char(myByte) >= 48 && char(myByte) <= 57) || (char(myByte) >= 65 && char(myByte) <= 122)) && myLength < 15) {
      romName[myLength] = char(myByte);
      myLength++;
    }
  }
}

/******************************************
  ROM functions
*****************************************/
// Dump ROM
void readROM_GB() {
  // Get name, add extension and convert to char array for sd lib
  strcpy(fileName, romName);
  strcat(fileName, ".GB");

  // create a new folder for the rom file
  EEPROM_readAnything(0, foldern);
  sprintf(folder, "GB/ROM/%s/%d", romName, foldern);
  sd.mkdir(folder, true);
  sd.chdir(folder);

  display_Clear();
  print_Msg(F("Saving to "));
  print_Msg(folder);
  println_Msg(F("/..."));
  display_Update();

  // write new folder number back to eeprom
  foldern = foldern + 1;
  EEPROM_writeAnything(0, foldern);

  //open file on sd card
  if (!myFile.open(fileName, O_RDWR | O_CREAT)) {
    print_Error(F("Can't create file on SD"), true);
  }

  uint16_t romAddress = 0;

  // Read number of banks and switch banks
  for (uint16_t bank = 1; bank < romBanks; bank++) {
    // Switch data pins to output
    dataOut();

    if (romType >= 5) { // MBC2 and above
      writeByte_GB(0x2100, bank); // Set ROM bank
    }
    else { // MBC1
      writeByte_GB(0x6000, 0); // Set ROM Mode
      writeByte_GB(0x4000, bank >> 5); // Set bits 5 & 6 (01100000) of ROM bank
      writeByte_GB(0x2000, bank & 0x1F); // Set bits 0 & 4 (00011111) of ROM bank
    }

    // Switch data pins to intput
    dataIn_GB();

    if (bank > 1) {
      romAddress = 0x4000;
    }

    // Read up to 7FFF per bank
    while (romAddress <= 0x7FFF) {
      uint8_t readData[512];
      for (int i = 0; i < 512; i++) {
        readData[i] = readByte_GB(romAddress + i);
      }
      myFile.write(readData, 512);
      romAddress += 512;
    }
  }

  // Close the file:
  myFile.close();

  base64_dump(fileName, "GB");
}

unsigned int calc_checksum_GB (char* fileName, char* folder) {
  unsigned int calcChecksum = 0;
  //  int calcFilesize = 0; // unused
  unsigned long i = 0;
  int c = 0;

  if (strcmp(folder, "root") != 0)
    sd.chdir(folder);

  // If file exists
  if (myFile.open(fileName, O_READ)) {
    //calcFilesize = myFile.fileSize() * 8 / 1024 / 1024; // unused
    for (i = 0; i < (myFile.fileSize() / 512); i++) {
      myFile.read(sdBuffer, 512);
      for (c = 0; c < 512; c++) {
        calcChecksum += sdBuffer[c];
      }
    }
    myFile.close();
    sd.chdir();
    // Subtract checksum bytes
    calcChecksum -= readByte_GB(0x014E);
    calcChecksum -= readByte_GB(0x014F);

    // Return result
    return (calcChecksum);
  }
  // Else show error
  else {
    print_Error(F("DUMP ROM 1ST"), false);
    return 0;
  }
}

boolean compare_checksum_GB() {

  println_Msg(F("Calculating Checksum"));
  display_Update();

  strcpy(fileName, romName);
  strcat(fileName, ".GB");

  // last used rom folder
  EEPROM_readAnything(0, foldern);
  sprintf(folder, "GB/ROM/%s/%d", romName, foldern - 1);

  char calcsumStr[5];
  sprintf(calcsumStr, "%04X", calc_checksum_GB(fileName, folder));

  if (strcmp(calcsumStr, checksumStr) == 0) {
    print_Msg(F("Result: "));
    println_Msg(calcsumStr);
    println_Msg(F("Checksum matches"));
    display_Update();
    return 1;
  }
  else {
    print_Msg(F("Result: "));
    println_Msg(calcsumStr);
    print_Error(F("Checksum Error"), false);
    return 0;
  }
}

/******************************************
  SRAM functions
*****************************************/
// Read RAM
void readSRAM_GB() {
  // Does cartridge have RAM
  if (sramEndAddress > 0) {

    // Get name, add extension and convert to char array for sd lib
    strcpy(fileName, romName);
    strcat(fileName, ".sav");

    // create a new folder for the save file
    EEPROM_readAnything(0, foldern);
    sprintf(folder, "GB/SAVE/%s/%d", romName, foldern);
    sd.mkdir(folder, true);
    sd.chdir(folder);

    // write new folder number back to eeprom
    foldern = foldern + 1;
    EEPROM_writeAnything(0, foldern);

    //open file on sd card
    if (!myFile.open(fileName, O_RDWR | O_CREAT)) {
      print_Error(F("SD Error"), true);
    }

    dataIn_GB();

    // MBC2 Fix (unknown why this fixes it, maybe has to read ROM before RAM?)
    readByte_GB(0x0134);

    dataOut();
    if (romType <= 4) { // MBC1
      writeByte_GB(0x6000, 1); // Set RAM Mode
    }

    // Initialise MBC
    writeByte_GB(0x0000, 0x0A);

    // Switch SRAM banks
    for (uint8_t bank = 0; bank < sramBanks; bank++) {
      dataOut();
      writeByte_GB(0x4000, bank);

      // Read SRAM
      dataIn_GB();
      for (uint16_t sramAddress = 0xA000; sramAddress <= sramEndAddress; sramAddress += 64) {
        uint8_t readData[64];
        for (uint8_t i = 0; i < 64; i++) {
          readData[i] = readByteSRAM_GB(sramAddress + i);
        }
        myFile.write(readData, 64);
      }
    }

    // Disable SRAM
    dataOut();
    writeByte_GB(0x0000, 0x00);
    dataIn_GB();

    // Close the file:
    myFile.close();

    // Signal end of process
    print_Msg(F("Saved to "));
    print_Msg(folder);
    println_Msg(F("/"));
    display_Update();

    base64_dump(fileName, "GB|SAVE");
  }
  else {
    print_Error(F("Cart has no SRAM"), false);
  }
}

// Write RAM
void writeSRAM_GB() {
  // Does cartridge have SRAM
  if (sramEndAddress > 0) {
    // Create filepath
    sprintf(filePath, "%s/%s", filePath, fileName);

    //open file on sd card
    if (myFile.open(filePath, O_READ)) {
      // Set pins to input
      dataIn_GB();

      // MBC2 Fix (unknown why this fixes it, maybe has to read ROM before RAM?)
      readByte_GB(0x0134);

      dataOut();

      if (romType <= 4) { // MBC1
        writeByte_GB(0x6000, 1); // Set RAM Mode
      }

      // Initialise MBC
      writeByte_GB(0x0000, 0x0A);

      // Switch RAM banks
      for (uint8_t bank = 0; bank < sramBanks; bank++) {
        writeByte_GB(0x4000, bank);

        // Write RAM
        for (uint16_t sramAddress = 0xA000; sramAddress <= sramEndAddress; sramAddress++) {
          writeByteSRAM_GB(sramAddress, myFile.read());
        }
      }
      // Disable RAM
      writeByte_GB(0x0000, 0x00);

      // Set pins to input
      dataIn_GB();

      // Close the file:
      myFile.close();
      display_Clear();
      println_Msg(F("SRAM writing finished"));
      display_Update();

    }
    else {
      print_Error(F("File doesnt exist"), false);
    }
  }
  else {
    print_Error(F("Cart has no SRAM"), false);
  }
}

// Check if the SRAM was written without any error
unsigned long verifySRAM_GB() {

  //open file on sd card
  if (myFile.open(filePath, O_READ)) {

    // Variable for errors
    writeErrors = 0;

    dataIn_GB();

    // MBC2 Fix (unknown why this fixes it, maybe has to read ROM before RAM?)
    readByte_GB(0x0134);

    // Does cartridge have RAM
    if (sramEndAddress > 0) {
      dataOut();
      if (romType <= 4) { // MBC1
        writeByte_GB(0x6000, 1); // Set RAM Mode
      }

      // Initialise MBC
      writeByte_GB(0x0000, 0x0A);

      // Switch SRAM banks
      for (uint8_t bank = 0; bank < sramBanks; bank++) {
        dataOut();
        writeByte_GB(0x4000, bank);

        // Read SRAM
        dataIn_GB();
        for (uint16_t sramAddress = 0xA000; sramAddress <= sramEndAddress; sramAddress += 64) {
          //fill sdBuffer
          myFile.read(sdBuffer, 64);
          for (int c = 0; c < 64; c++) {
            if (readByteSRAM_GB(sramAddress + c) != sdBuffer[c]) {
              writeErrors++;
            }
          }
        }
      }
      dataOut();
      // Disable RAM
      writeByte_GB(0x0000, 0x00);
      dataIn_GB();
    }
    // Close the file:
    myFile.close();
    return writeErrors;
  }
  else {
    print_Error(F("Can't open file"), true);
  }
}

/******************************************
  29F016/29F032/29F033 flashrom functions
*****************************************/
// Write 29F032 flashrom
// A0-A13 directly connected to cart edge -> 16384(0x0-0x3FFF) bytes per bank -> 256(0x0-0xFF) banks
// A14-A21 connected to MBC5
void writeFlash29F_GB(byte MBC) {
  // Launch filebrowser
  filePath[0] = '\0';
  sd.chdir("/");
  fileBrowser(F("Select file"));
  display_Clear();

  // Create filepath
  sprintf(filePath, "%s/%s", filePath, fileName);

  // Open file on sd card
  if (myFile.open(filePath, O_READ)) {
    // Get rom size from file
    myFile.seekCur(0x147);
    romType = myFile.read();
    romSize = myFile.read();
    // Go back to file beginning
    myFile.seekSet(0);

    // ROM banks
    romBanks = 2; // Default 32K
    if (romSize >= 1) { // Calculate rom size
      romBanks = 2 << romSize;
    }

    // Set data pins to output
    dataOut();

    // Set ROM bank hi 0
    writeByte_GB(0x3000, 0);
    // Set ROM bank low 0
    writeByte_GB(0x2000, 0);
    delay(100);

    // Reset flash
    writeByte_GB(0x555, 0xf0);
    delay(100);

    // ID command sequence
    writeByte_GB(0x555, 0xaa);
    writeByte_GB(0x2aa, 0x55);
    writeByte_GB(0x555, 0x90);

    dataIn_GB();

    // Read the two id bytes into a string
    sprintf(flashid, "%02X%02X", readByte_GB(0), readByte_GB(1));

    if (strcmp(flashid, "04D4") == 0) {
      println_Msg(F("MBM29F033C"));
      print_Msg(F("Banks: "));
      print_Msg(romBanks);
      println_Msg(F("/256"));
      display_Update();
    }
    else if (strcmp(flashid, "0141") == 0) {
      println_Msg(F("AM29F032B"));
      print_Msg(F("Banks: "));
      print_Msg(romBanks);
      println_Msg(F("/256"));
      display_Update();
    }
    else if (strcmp(flashid, "01AD") == 0) {
      println_Msg(F("AM29F016B"));
      print_Msg(F("Banks: "));
      print_Msg(romBanks);
      println_Msg(F("/128"));
      display_Update();
    }
    else if (strcmp(flashid, "04AD") == 0) {
      println_Msg(F("AM29F016D"));
      print_Msg(F("Banks: "));
      print_Msg(romBanks);
      println_Msg(F("/128"));
      display_Update();
    }
    else if (strcmp(flashid, "01D5") == 0) {
      println_Msg(F("AM29F080B"));
      print_Msg(F("Banks: "));
      print_Msg(romBanks);
      println_Msg(F("/64"));
      display_Update();
    }
    else {
      print_Msg(F("Flash ID: "));
      println_Msg(flashid);
      display_Update();
      print_Error(F("Unknown flashrom"), true);
    }
    dataOut();

    // Reset flash
    writeByte_GB(0x555, 0xf0);

    delay(100);
    println_Msg(F("Erasing flash"));
    display_Update();

    // Erase flash
    writeByte_GB(0x555, 0xaa);
    writeByte_GB(0x2aa, 0x55);
    writeByte_GB(0x555, 0x80);
    writeByte_GB(0x555, 0xaa);
    writeByte_GB(0x2aa, 0x55);
    writeByte_GB(0x555, 0x10);

    dataIn_GB();

    // Read the status register
    byte statusReg = readByte_GB(0);

    // After a completed erase D7 will output 1
    while ((statusReg & 0x80) != 0x80) {
      // Blink led
      blinkLED();
      delay(100);
      // Update Status
      statusReg = readByte_GB(0);
    }

    // Blankcheck
    println_Msg(F("Blankcheck"));
    display_Update();

    // Read x number of banks
    for (int currBank = 0; currBank < romBanks; currBank++) {
      // Blink led
      blinkLED();

      dataOut();

      // Set ROM bank
      writeByte_GB(0x2000, currBank);
      dataIn();

      for (unsigned int currAddr = 0x4000; currAddr < 0x7FFF; currAddr += 512) {
        uint8_t readData[512];
        for (int currByte = 0; currByte < 512; currByte++) {
          readData[currByte] = readByte_GB(currAddr + currByte);
        }
        for (int j = 0; j < 512; j++) {
          if (readData[j] != 0xFF) {
            println_Msg(F("Not empty"));
            print_Error(F("Erase failed"), true);
          }
        }
      }
    }

    if (MBC == 3) {
      println_Msg(F("Writing flash MBC3"));
      display_Update();

      // Write flash
      dataOut();

      uint16_t currAddr = 0;
      uint16_t endAddr = 0x3FFF;

      for (int currBank = 0; currBank < romBanks; currBank++) {
        // Blink led
        blinkLED();

        // Set ROM bank
        writeByte_GB(0x2100, currBank);

        if (currBank > 0) {
          currAddr = 0x4000;
          endAddr = 0x7FFF;
        }

        while (currAddr <= endAddr) {
          myFile.read(sdBuffer, 512);

          for (int currByte = 0; currByte < 512; currByte++) {
            // Write command sequence
            writeByte_GB(0x555, 0xaa);
            writeByte_GB(0x2aa, 0x55);
            writeByte_GB(0x555, 0xa0);
            // Write current byte
            writeByte_GB(currAddr + currByte, sdBuffer[currByte]);

            // Set data pins to input
            dataIn();

            // Setting CS(PH3) and OE/RD(PH6) LOW
            PORTH &= ~((1 << 3) | (1 << 6));

            // Busy check
            while ((PINC & 0x80) != (sdBuffer[currByte] & 0x80)) {
            }

            // Switch CS(PH3) and OE/RD(PH6) to HIGH
            PORTH |= (1 << 3) | (1 << 6);

            // Set data pins to output
            dataOut();
          }
          currAddr += 512;
        }
      }
    }

    else if (MBC == 5) {
      println_Msg(F("Writing flash MBC5"));
      display_Update();

      // Write flash
      dataOut();

      for (int currBank = 0; currBank < romBanks; currBank++) {
        // Blink led
        blinkLED();

        // Set ROM bank
        writeByte_GB(0x2000, currBank);
        // 0x2A8000 fix
        writeByte_GB(0x4000, 0x0);

        for (unsigned int currAddr = 0x4000; currAddr < 0x7FFF; currAddr += 512) {
          myFile.read(sdBuffer, 512);

          for (int currByte = 0; currByte < 512; currByte++) {
            // Write command sequence
            writeByte_GB(0x555, 0xaa);
            writeByte_GB(0x2aa, 0x55);
            writeByte_GB(0x555, 0xa0);
            // Write current byte
            writeByte_GB(currAddr + currByte, sdBuffer[currByte]);

            // Set data pins to input
            dataIn();

            // Setting CS(PH3) and OE/RD(PH6) LOW
            PORTH &= ~((1 << 3) | (1 << 6));

            // Busy check
            while ((PINC & 0x80) != (sdBuffer[currByte] & 0x80)) {
            }

            // Switch CS(PH3) and OE/RD(PH6) to HIGH
            PORTH |= (1 << 3) | (1 << 6);

            // Set data pins to output
            dataOut();
          }
        }
      }
    }

    // Set data pins to input again
    dataIn_GB();

    println_Msg(F("Verifying"));
    display_Update();

    // Go back to file beginning
    myFile.seekSet(0);
    //unsigned int addr = 0;  // unused
    writeErrors = 0;

    // Verify flashrom
    uint16_t romAddress = 0;

    // Read number of banks and switch banks
    for (uint16_t bank = 1; bank < romBanks; bank++) {
      // Switch data pins to output
      dataOut();

      if (romType >= 5) { // MBC2 and above
        writeByte_GB(0x2100, bank); // Set ROM bank
      }
      else { // MBC1
        writeByte_GB(0x6000, 0); // Set ROM Mode
        writeByte_GB(0x4000, bank >> 5); // Set bits 5 & 6 (01100000) of ROM bank
        writeByte_GB(0x2000, bank & 0x1F); // Set bits 0 & 4 (00011111) of ROM bank
      }

      // Switch data pins to intput
      dataIn_GB();

      if (bank > 1) {
        romAddress = 0x4000;
      }
      // Blink led
      blinkLED();

      // Read up to 7FFF per bank
      while (romAddress <= 0x7FFF) {
        // Fill sdBuffer
        myFile.read(sdBuffer, 512);
        // Compare
        for (int i = 0; i < 512; i++) {
          if (readByte_GB(romAddress + i) != sdBuffer[i]) {
            writeErrors++;
          }
        }
        romAddress += 512;
      }
    }
    // Close the file:
    myFile.close();

    if (writeErrors == 0) {
      println_Msg(F("OK"));
      display_Update();
    }
    else {
      print_Msg(F("Error: "));
      print_Msg(writeErrors);
      println_Msg(F(" bytes "));
      print_Error(F("did not verify."), false);
    }
  }
  else {
    println_Msg(F("Can't open file"));
    display_Update();
  }
}

/******************************************
  CFU flashrom functions
*****************************************/

/*
   Flash chips can either be in x8 mode or x16 mode and sometimes the two
   least significant bits on flash cartridges' data lines are swapped.
   This function reads a byte and compensates for the differences.
   This is only necessary for commands to the flash, not for data read from the flash, the MBC or SRAM.

   address needs to be the x8 mode address of the flash register that should be read.
*/
byte readByteCompensated(int address) {
  byte data = readByte_GB(address >> (flashX16Mode ? 1 : 0));
  if (flashSwitchLastBits) {
    return (data & 0b11111100) | ((data << 1) & 0b10) | ((data >> 1) & 0b01);
  }
  return data;
}

/*
   Flash chips can either be in x8 mode or x16 mode and sometimes the two
   least significant bits on flash cartridges' data lines are swapped.
   This function writes a byte and compensates for the differences.
   This is only necessary for commands to the flash, not for data written to the flash, the MBC or SRAM.
   .
   address needs to be the x8 mode address of the flash register that should be read.
*/
byte writeByteCompensated(int address, byte data) {
  if (flashSwitchLastBits) {
    data = (data & 0b11111100) | ((data << 1) & 0b10) | ((data >> 1) & 0b01);
  }
  writeByte_GB(address >> (flashX16Mode ? 1 : 0), data);
}

void startCFIMode(boolean x16Mode) {
  if (x16Mode) {
    writeByte_GB(0x555, 0xf0); //x16 mode reset command
    delay(500);
    writeByte_GB(0x555, 0xf0); //Double reset to get out of possible Autoselect + CFI mode
    delay(500);
    writeByte_GB(0x55, 0x98);  //x16 CFI Query command
  } else {
    writeByte_GB(0xAAA, 0xf0); //x8  mode reset command
    delay(100);
    writeByte_GB(0xAAA, 0xf0); //Double reset to get out of possible Autoselect + CFI mode
    delay(100);
    writeByte_GB(0xAA, 0x98);  //x8 CFI Query command
  }
}

/* Identify the different flash chips.
   Sets the global variables flashBanks, flashX16Mode and flashSwitchLastBits
*/
void identifyCFI_GB() {
  // Reset flash
  display_Clear();
  dataOut();
  writeByte_GB(0x6000, 0); // Set ROM Mode
  writeByte_GB(0x2000, 0); // Set Bank to 0
  writeByte_GB(0x3000, 0);

  startCFIMode(false); // Trying x8 mode first

  dataIn_GB();
  display_Clear();
  // Try x8 mode first
  char cfiQRYx8[7];
  char cfiQRYx16[7];
  sprintf(cfiQRYx8, "%02X%02X%02X", readByte_GB(0x20), readByte_GB(0x22), readByte_GB(0x24));
  sprintf(cfiQRYx16, "%02X%02X%02X", readByte_GB(0x10), readByte_GB(0x11), readByte_GB(0x12)); // some devices use x8-style CFI Query command even though they are in x16 command mode
  if (strcmp(cfiQRYx8, "515259") == 0) { // QRY in x8 mode
    println_Msg(F("Normal CFI x8 Mode"));
    flashX16Mode = false;
    flashSwitchLastBits = false;
  } else if (strcmp(cfiQRYx8, "52515A") == 0) { // QRY in x8 mode with switched last bit
    println_Msg(F("Switched CFI x8 Mode"));
    flashX16Mode = false;
    flashSwitchLastBits = true;
  } else if (strcmp(cfiQRYx16, "515259") == 0) { // QRY in x16 mode
    println_Msg(F("Normal CFI x16 Mode"));
    flashX16Mode = true;
    flashSwitchLastBits = false;
  } else if (strcmp(cfiQRYx16, "52515A") == 0) { // QRY in x16 mode with switched last bit
    println_Msg(F("Switched CFI x16 Mode"));
    flashX16Mode = true;
    flashSwitchLastBits = true;
  } else {
    startCFIMode(true); // Try x16 mode next
    sprintf(cfiQRYx16, "%02X%02X%02X", readByte_GB(0x10), readByte_GB(0x11), readByte_GB(0x12));
    if (strcmp(cfiQRYx16, "515259") == 0) { // QRY in x16 mode
      println_Msg(F("Normal CFI x16 Mode"));
      flashX16Mode = true;
      flashSwitchLastBits = false;
    } else if (strcmp(cfiQRYx16, "52515A") == 0) { // QRY in x16 mode with switched last bit
      println_Msg(F("Switched CFI x16 Mode"));
      flashX16Mode = true;
      flashSwitchLastBits = true;
    } else {
      println_Msg(F("CFI Query failed!"));
      display_Update();
      wait();
      return 0;
    }
  }
  dataIn_GB();
  flashBanks = 1 << (readByteCompensated(0x4E) - 14); // - flashX16Mode);
  dataOut();

  // Reset flash
  writeByteCompensated(0xAAA, 0xf0);
  delay(100);
}

// Write 29F032 flashrom
// A0-A13 directly connected to cart edge -> 16384(0x0-0x3FFF) bytes per bank -> 256(0x0-0xFF) banks
// A14-A21 connected to MBC5
// identifyFlash_GB() needs to be run before this!
bool writeCFI_GB() {
  // Create filepath
  sprintf(filePath, "%s/%s", filePath, fileName);

  // Open file on sd card
  if (myFile.open(filePath, O_READ)) {
    // Get rom size from file
    myFile.seekCur(0x147);
    romType = myFile.read();
    romSize = myFile.read();
    // Go back to file beginning
    myFile.seekSet(0);

    // ROM banks
    romBanks = 2; // Default 32K
    if (romSize >= 1) { // Calculate rom size
      romBanks = 2 << romSize;
    }

    if (romBanks <= flashBanks) {
      print_Msg(F("Using "));
      print_Msg(romBanks);
      print_Msg(F("/"));
      print_Msg(flashBanks);
      println_Msg(F(" Banks"));
      display_Update();
    } else {
      println_Msg(F("Error: Flash has too few banks!"));
      print_Msg(F("Has "));
      print_Msg(flashBanks);
      println_Msg(F(" banks,"));
      print_Msg(F("but needs "));
      print_Msg(romBanks);
      println_Msg(F("."));
      println_Msg(F("Press button..."));
      display_Update();
      wait();
      resetArduino();
    }

    // Set data pins to output
    dataOut();

    // Set ROM bank hi 0
    writeByte_GB(0x3000, 0);
    // Set ROM bank low 0
    writeByte_GB(0x2000, 0);
    delay(100);

    // Reset flash
    writeByteCompensated(0xAAA, 0xf0);
    delay(100);
    dataOut();

    // Reset flash
    writeByte_GB(0x555, 0xf0);

    delay(100);
    println_Msg(F("Erasing flash"));
    display_Update();

    // Erase flash
    writeByteCompensated(0xAAA, 0xaa);
    writeByteCompensated(0x555, 0x55);
    writeByteCompensated(0xAAA, 0x80);
    writeByteCompensated(0xAAA, 0xaa);
    writeByteCompensated(0x555, 0x55);
    writeByteCompensated(0xAAA, 0x10);

    dataIn_GB();

    // Read the status register
    byte statusReg = readByte_GB(0);

    // After a completed erase D7 will output 1
    while ((statusReg & 0x80) != 0x80) {
      // Blink led
      blinkLED();
      delay(100);
      // Update Status
      statusReg = readByte_GB(0);
    }

    // Blankcheck
    println_Msg(F("Blankcheck"));
    display_Update();

    // Read x number of banks
    for (int currBank = 0; currBank < romBanks; currBank++) {
      // Blink led
      blinkLED();

      dataOut();

      // Set ROM bank
      writeByte_GB(0x2000, currBank);
      dataIn();

      for (unsigned int currAddr = 0x4000; currAddr < 0x7FFF; currAddr += 512) {
        uint8_t readData[512];
        for (int currByte = 0; currByte < 512; currByte++) {
          readData[currByte] = readByte_GB(currAddr + currByte);
        }
        for (int j = 0; j < 512; j++) {
          if (readData[j] != 0xFF) {
            println_Msg(F("Not empty"));
            print_Error(F("Erase failed"), true);
          }
        }
      }
    }

    println_Msg(F("Writing flash MBC3/5"));
    display_Update();

    // Write flash
    dataOut();

    uint16_t currAddr = 0;
    uint16_t endAddr = 0x3FFF;

    for (int currBank = 0; currBank < romBanks; currBank++) {
      // Blink led
      blinkLED();

      // Set ROM bank
      writeByte_GB(0x2100, currBank);
      // 0x2A8000 fix
      writeByte_GB(0x4000, 0x0);

      if (currBank > 0) {
        currAddr = 0x4000;
        endAddr = 0x7FFF;
      }

      while (currAddr <= endAddr) {
        myFile.read(sdBuffer, 512);

        for (int currByte = 0; currByte < 512; currByte++) {
          // Write command sequence
          writeByteCompensated(0xAAA, 0xaa);
          writeByteCompensated(0x555, 0x55);
          writeByteCompensated(0xAAA, 0xa0);

          // Write current byte
          writeByte_GB(currAddr + currByte, sdBuffer[currByte]);

          // Set data pins to input
          dataIn();

          // Setting CS(PH3) and OE/RD(PH6) LOW
          PORTH &= ~((1 << 3) | (1 << 6));

          // Busy check
          short i = 0;
          while ((PINC & 0x80) != (sdBuffer[currByte] & 0x80)) {
            i++;
            if (i > 500) {
              if (currAddr < 0x4000) { // This happens when trying to flash an MBC5 as if it was an MBC3. Retry to flash as MBC5, starting from last successfull byte.
                currByte--;
                currAddr += 0x4000;
                endAddr = 0x7FFF;
                break;
              } else { // If a timeout happens while trying to flash MBC5-style, flashing failed.
                return false;
              }
            }
          }

          // Switch CS(PH3) and OE/RD(PH6) to HIGH
          PORTH |= (1 << 3) | (1 << 6);
          __asm__("nop\n\tnop\n\tnop\n\t"); // Waste a few CPU cycles to remove write errors

          // Set data pins to output
          dataOut();
        }
        currAddr += 512;
      }
    }

    // Set data pins to input again
    dataIn_GB();

    display_Clear();
    println_Msg(F("Verifying"));
    display_Update();

    // Go back to file beginning
    myFile.seekSet(0);
    //unsigned int addr = 0;  // unused
    writeErrors = 0;

    // Verify flashrom
    uint16_t romAddress = 0;

    // Read number of banks and switch banks
    for (uint16_t bank = 1; bank < romBanks; bank++) {
      // Switch data pins to output
      dataOut();

      if (romType >= 5) { // MBC2 and above
        writeByte_GB(0x2100, bank); // Set ROM bank
      }
      else { // MBC1
        writeByte_GB(0x6000, 0); // Set ROM Mode
        writeByte_GB(0x4000, bank >> 5); // Set bits 5 & 6 (01100000) of ROM bank
        writeByte_GB(0x2000, bank & 0x1F); // Set bits 0 & 4 (00011111) of ROM bank
      }

      // Switch data pins to intput
      dataIn_GB();

      if (bank > 1) {
        romAddress = 0x4000;
      }
      // Blink led
      blinkLED();

      // Read up to 7FFF per bank
      while (romAddress <= 0x7FFF) {
        // Fill sdBuffer
        myFile.read(sdBuffer, 512);
        // Compare
        for (int i = 0; i < 512; i++) {
          if (readByte_GB(romAddress + i) != sdBuffer[i]) {
            writeErrors++;
          }
        }
        romAddress += 512;
      }
    }
    // Close the file:
    myFile.close();

    if (writeErrors == 0) {
      println_Msg(F("OK"));
      display_Update();
    }
    else {
      print_Msg(F("Error: "));
      print_Msg(writeErrors);
      println_Msg(F(" bytes "));
      print_Error(F("did not verify."), false);
    }
  }
  else {
    println_Msg(F("Can't open file"));
    display_Update();
  }
  return true;
}

#endif

//******************************************
// End of File
//******************************************
