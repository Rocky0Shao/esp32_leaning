#include <Arduino.h>
#include "FS.h"
#include "SD_MMC.h"

// ESP32 SDMMC 1-bit pins (ESP-WROVER-KIT)
#define SD_MMC_CMD 15
#define SD_MMC_CLK 14
#define SD_MMC_D0   2

// ---------- Helper Functions ----------

void listDir(fs::FS &fs, const char *dirname, uint8_t levels) {
    File root = fs.open(dirname);
    if (!root) {
        Serial.println("Failed to open directory");
        return;
    }
    if (!root.isDirectory()) {
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while (file) {
        if (file.isDirectory()) {
            Serial.print("DIR : ");
            Serial.println(file.name());
            if (levels) {
                listDir(fs, file.name(), levels - 1);
            }
        } else {
            Serial.print("FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

void writeFile(fs::FS &fs, const char *path, const char *message) {
    File file = fs.open(path, FILE_WRITE);
    if (!file) {
        Serial.println("Failed to open file for writing");
        return;
    }
    file.print(message);
    file.close();
}

void appendFile(fs::FS &fs, const char *path, const char *message) {
    File file = fs.open(path, FILE_APPEND);
    if (!file) {
        Serial.println("Failed to open file for appending");
        return;
    }
    file.print(message);
    file.close();
}

void readFile(fs::FS &fs, const char *path) {
    File file = fs.open(path);
    if (!file) {
        Serial.println("Failed to open file for reading");
        return;
    }
    Serial.println("Read from file:");
    while (file.available()) {
        Serial.write(file.read());
    }
    Serial.println();
    file.close();
}

void deleteFile(fs::FS &fs, const char *path) {
    fs.remove(path);
}

void renameFile(fs::FS &fs, const char *path1, const char *path2) {
    fs.rename(path1, path2);
}

// ---------- Setup ----------
void setup() {
    Serial.begin(115200);
    while(!Serial); // Wait for serial
    delay(1000); 

    Serial.println("Hello from WROVER!");

    // 1. DEFINITELY SET PINS 
    // These are standard for ESP-WROVER-KIT. 
    // If you wired it yourself, check your wiring!
    // Format: (CLK, CMD, D0)
    SD_MMC.setPins(14, 15, 2); 

    // 2. FORCE 1-BIT MODE
    // The second argument 'true' enables 1-bit mode (uses only 3 pins + power).
    // The first argument "/sdcard" is the mount point.
    if (!SD_MMC.begin("/sdcard", true)) { 
        Serial.println("Card Mount Failed");
        return;
    }

    Serial.println("SD Card Initialized Successfully!");
    
    // Print card type for verification
    uint8_t cardType = SD_MMC.cardType();
    if(cardType == CARD_NONE){
        Serial.println("No SD card attached");
        return;
    }
}
// void setup() {
// // Must match monitor_speed
//     Serial.begin(115200); 

//     // Wait a moment for the Serial hardware to stabilize
//     delay(3000); 

//     Serial.println("Hello from WROVER!");

//     SD_MMC.setPins(SD_MMC_CLK, SD_MMC_CMD, SD_MMC_D0);

//     if (!SD_MMC.begin("/sdcard", true)) {
//         Serial.println("Card Mount Failed");
//         return;
//     }

//     uint8_t cardType = SD_MMC.cardType();
//     if (cardType == CARD_NONE) {
//         Serial.println("No SD card attached");
//         return;
//     }

//     Serial.print("Card Type: ");
//     if (cardType == CARD_MMC) Serial.println("MMC");
//     else if (cardType == CARD_SD) Serial.println("SDSC");
//     else if (cardType == CARD_SDHC) Serial.println("SDHC");
//     else Serial.println("UNKNOWN");

//     Serial.printf("Card Size: %llu MB\n",
//                   SD_MMC.cardSize() / (1024 * 1024));

//     listDir(SD_MMC, "/", 0);

//     writeFile(SD_MMC, "/hello.txt", "Hello ");
//     appendFile(SD_MMC, "/hello.txt", "World!\n");
//     readFile(SD_MMC, "/hello.txt");

//     renameFile(SD_MMC, "/hello.txt", "/foo.txt");
//     readFile(SD_MMC, "/foo.txt");

//     deleteFile(SD_MMC, "/foo.txt");

//     Serial.printf("Total: %llu MB\n", SD_MMC.totalBytes() / (1024 * 1024));
//     Serial.printf("Used : %llu MB\n", SD_MMC.usedBytes() / (1024 * 1024));
// }

void loop() {
  Serial.println("Looping...");
    delay(10000);
}

