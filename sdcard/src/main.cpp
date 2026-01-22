// #include <Arduino.h>
// #include "FS.h"
// #include "SD.h"
// #include "SPI.h"

// // Freenove ESP32-WROVER built-in SD card slot pins
// // Uses HSPI pins matching the onboard SD slot
// #define SD_CS    13   // directly to SD slot
// #define SD_MOSI  15   // CMD line
// #define SD_MISO   2   // DAT0 line
// #define SD_SCK   14   // CLK line

// SPIClass spi = SPIClass(HSPI);

// // ---------- Helper Functions ----------

// void listDir(fs::FS &fs, const char *dirname, uint8_t levels) {
//     File root = fs.open(dirname);
//     if (!root) {
//         Serial.println("Failed to open directory");
//         return;
//     }
//     if (!root.isDirectory()) {
//         Serial.println("Not a directory");
//         return;
//     }

//     File file = root.openNextFile();
//     while (file) {
//         if (file.isDirectory()) {
//             Serial.print("DIR : ");
//             Serial.println(file.name());
//             if (levels) {
//                 listDir(fs, file.name(), levels - 1);
//             }
//         } else {
//             Serial.print("FILE: ");
//             Serial.print(file.name());
//             Serial.print("  SIZE: ");
//             Serial.println(file.size());
//         }
//         file = root.openNextFile();
//     }
// }

// void writeFile(fs::FS &fs, const char *path, const char *message) {
//     File file = fs.open(path, FILE_WRITE);
//     if (!file) {
//         Serial.println("Failed to open file for writing");
//         return;
//     }
//     file.print(message);
//     file.close();
// }

// void appendFile(fs::FS &fs, const char *path, const char *message) {
//     File file = fs.open(path, FILE_APPEND);
//     if (!file) {
//         Serial.println("Failed to open file for appending");
//         return;
//     }
//     file.print(message);
//     file.close();
// }

// void readFile(fs::FS &fs, const char *path) {
//     File file = fs.open(path);
//     if (!file) {
//         Serial.println("Failed to open file for reading");
//         return;
//     }
//     Serial.println("Read from file:");
//     while (file.available()) {
//         Serial.write(file.read());
//     }
//     Serial.println();
//     file.close();
// }

// void deleteFile(fs::FS &fs, const char *path) {
//     fs.remove(path);
// }

// void renameFile(fs::FS &fs, const char *path1, const char *path2) {
//     fs.rename(path1, path2);
// }

// // ---------- Setup ----------
// void setup() {
//     Serial.begin(115200);
//     while(!Serial); // Wait for serial
//     delay(1000);

//     Serial.println("Hello from Freenove WROVER!");

//     // Give GPIO 2 time to stabilize (it's a strapping pin)
//     delay(100);

//     // Initialize SPI with Freenove SD slot pins
//     spi.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);

//     // Initialize SD card - try slower speed for built-in slot
//     if (!SD.begin(SD_CS, spi, 1000000)) {  // 1MHz for reliability
//         Serial.println("Card Mount Failed");
//         Serial.println("Freenove SD slot pins: CS=13, MOSI=15, MISO=2, SCK=14");
//         return;
//     }

//     Serial.println("SD Card Initialized Successfully!");

//     // Print card type for verification
//     uint8_t cardType = SD.cardType();
//     if(cardType == CARD_NONE){
//         Serial.println("No SD card attached");
//         return;
//     }

//     Serial.print("Card Type: ");
//     if (cardType == CARD_MMC) Serial.println("MMC");
//     else if (cardType == CARD_SD) Serial.println("SDSC");
//     else if (cardType == CARD_SDHC) Serial.println("SDHC");
//     else Serial.println("UNKNOWN");

//     Serial.printf("Card Size: %llu MB\n", SD.cardSize() / (1024 * 1024));
// }
// // void setup() {
// // // Must match monitor_speed
// //     Serial.begin(115200); 

// //     // Wait a moment for the Serial hardware to stabilize
// //     delay(3000); 

// //     Serial.println("Hello from WROVER!");

// //     SD_MMC.setPins(SD_MMC_CLK, SD_MMC_CMD, SD_MMC_D0);

// //     if (!SD_MMC.begin("/sdcard", true)) {
// //         Serial.println("Card Mount Failed");
// //         return;
// //     }

// //     uint8_t cardType = SD_MMC.cardType();
// //     if (cardType == CARD_NONE) {
// //         Serial.println("No SD card attached");
// //         return;
// //     }

// //     Serial.print("Card Type: ");
// //     if (cardType == CARD_MMC) Serial.println("MMC");
// //     else if (cardType == CARD_SD) Serial.println("SDSC");
// //     else if (cardType == CARD_SDHC) Serial.println("SDHC");
// //     else Serial.println("UNKNOWN");

// //     Serial.printf("Card Size: %llu MB\n",
// //                   SD_MMC.cardSize() / (1024 * 1024));

// //     listDir(SD_MMC, "/", 0);

// //     writeFile(SD_MMC, "/hello.txt", "Hello ");
// //     appendFile(SD_MMC, "/hello.txt", "World!\n");
// //     readFile(SD_MMC, "/hello.txt");

// //     renameFile(SD_MMC, "/hello.txt", "/foo.txt");
// //     readFile(SD_MMC, "/foo.txt");

// //     deleteFile(SD_MMC, "/foo.txt");

// //     Serial.printf("Total: %llu MB\n", SD_MMC.totalBytes() / (1024 * 1024));
// //     Serial.printf("Used : %llu MB\n", SD_MMC.usedBytes() / (1024 * 1024));
// // }

// void loop() {
//   Serial.println("Looping...");
//     delay(10000);
// }

/**********************************************************************
  Filename    : SDMMC Test
  Description : The SD card is accessed using the SDMMC one-bit bus
  Auther      : www.freenove.com
  Modification: 2024/06/20
**********************************************************************/
#include "sd_read_write.h"
#include "SD_MMC.h"

#define SD_MMC_CMD 15 //Please do not modify it.
#define SD_MMC_CLK 14 //Please do not modify it. 
#define SD_MMC_D0  2  //Please do not modify it.

void setup(){
    Serial.begin(115200);
    SD_MMC.setPins(SD_MMC_CLK, SD_MMC_CMD, SD_MMC_D0);
    if (!SD_MMC.begin("/sdcard", true, true, SDMMC_FREQ_DEFAULT, 5)) {
      Serial.println("Card Mount Failed");
      return;
    }
    uint8_t cardType = SD_MMC.cardType();
    if(cardType == CARD_NONE){
        Serial.println("No SD_MMC card attached");
        return;
    }

    Serial.print("SD_MMC Card Type: ");
    if(cardType == CARD_MMC){
        Serial.println("MMC");
    } else if(cardType == CARD_SD){
        Serial.println("SDSC");
    } else if(cardType == CARD_SDHC){
        Serial.println("SDHC");
    } else {
        Serial.println("UNKNOWN");
    }

    uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
    Serial.printf("SD_MMC Card Size: %lluMB\n", cardSize);

    listDir(SD_MMC, "/", 0);

    createDir(SD_MMC, "/mydir");
    listDir(SD_MMC, "/", 0);

    removeDir(SD_MMC, "/mydir");
    listDir(SD_MMC, "/", 2);

    writeFile(SD_MMC, "/hello.txt", "Hello ");
    appendFile(SD_MMC, "/hello.txt", "World!\n");
    readFile(SD_MMC, "/hello.txt");

    deleteFile(SD_MMC, "/foo.txt");
    renameFile(SD_MMC, "/hello.txt", "/foo.txt");
    readFile(SD_MMC, "/foo.txt");

    testFileIO(SD_MMC, "/test.txt");
    
    Serial.printf("Total space: %lluMB\r\n", SD_MMC.totalBytes() / (1024 * 1024));
    Serial.printf("Used space: %lluMB\r\n", SD_MMC.usedBytes() / (1024 * 1024));
}

void loop(){
  delay(10000);
}