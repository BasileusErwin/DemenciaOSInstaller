#ifndef __DEMOSINS__
#define __DEMOSINS__

#include <iostream>
#include <vector>
#include <stdlib.h>
#include <ncurses.h>

extern "C" {
    #include <parted/device.h>
    #include <parted/disk.h>
    #include <parted/parted.h>
    #include <unistd.h>
}

bool IsRoot();
bool IsEFISystem();
void InstallProcess();
bool SelectSwapMenu();
void makeEFI();
void MakeSwap();
void Install();
std::vector<PedDevice*> get_devices();
bool SelectSwapMenu();
const char* select_disk_menu();

#endif