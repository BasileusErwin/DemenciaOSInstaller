#ifndef __DEMOSINS__
#define __DEMOSINS__

#include <iostream>
#include <vector>
#include <string>

extern "C" {
#include <parted/device.h>
#include <parted/disk.h>
#include <parted/parted.h>
#include <unistd.h>
#include <stdlib.h>
#include <ncurses.h>
}

bool IsRoot();
bool IsEFISystem();
void InstallProcess();
bool empieza_con(std::string primera_str, std::string str_objetivo);
bool SelectSwapMenu();
// void makeEFI();
void MakeSwap();
void Install();
std::vector<PedDevice*> get_devices();
const char* select_disk_menu();
// bool SelectSwapMenu();

#endif