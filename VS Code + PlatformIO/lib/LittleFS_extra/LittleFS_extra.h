#ifndef LittleFS_H
#define LittleFS_H
#include <Arduino.h>

// // <========================> Libraries Declared <============================> //

#include "FS.h"
#include <LittleFS.h>
#include <time.h>

// <========================> Gloabal Varibales Declared <============================> //
#define FORMAT_LITTLEFS_IF_FAILED false

// <========================> Global Function Declared <============================> //
void listDir(fs::FS &fs, const char *dirname, uint8_t levels);

void createDir(fs::FS &fs, const char *path);

void removeDir(fs::FS &fs, const char *path);

void readFile(fs::FS &fs, const char *path);

void writeFile(fs::FS &fs, const char *path, const char *message);

void appendFile(fs::FS &fs, const char *path, const char *message);

void renameFile(fs::FS &fs, const char *path1, const char *path2);

// SPIFFS-like write and delete file
void writeFile2(fs::FS &fs, const char *path, const char *message);

void deleteFile2(fs::FS &fs, const char *path);

void testFileIO(fs::FS &fs, const char *path);

#endif
