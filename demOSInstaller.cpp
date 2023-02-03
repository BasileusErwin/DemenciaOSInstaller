/// Demencia OS Installer by hhk02

#include <iostream>
#include <stdlib.h>
#include <string>

#include "demOSInstaller.h"

using namespace std;

// Variables
int option;        // Variable que guara la opcion elegida del inicio
string swapoption; // Opcion donde se guarda si quieres la swap o no
string disk;       // Variable donde se almacena el disco de destino
string efiPart;    // Variable donde se almacena la partición EFI
string rootPart;
string swapOption; // Opcion donde se guarda si quieres la swap o no

string language;   // Variable donde se almacena la partición SWAP
string swapPart;   // Variable para especificar si es una instalación EFI o no.
string efiOption;  // Comprobar si la instalación es EFI y no.
bool usingSwap;    // Variable para especificar si se usa la SWAP
bool isEFI;        // Comprobar si la instalación es EFI y no.

bool IsRoot() { return getuid() == 0; }

bool IsEFISystem() { return access("/sys/firmware/efi/efivars", F_OK) == 0; }

bool startWith(std::string primera_str, std::string str_objetivo) {
  return primera_str.find(str_objetivo) == 0;
}

// Metodo de proceso de instalación
void InstallProcess() {
  cout << "Installing...." << endl;

  // Descomprimir el archivo squashfs RESPONSABLE de descomprimir el sistema en
  // el destino
  // "/media/cdrom/casper/filesystem.squashfs" ruta original
  system("unsquashfs -f -d /media/target/ "
         "/run/live/medium/live/filesystem.squashfs");
  
  system("mount --bind /proc/ /media/target/proc/");
  system("mount --bind /sys/ /media/target/sys/");
  system("mount --bind /dev/ /media/target/dev/");

  if (isEFI) {
    system("apt install arch-install-scripts -y");

    // Instalar gestor de arrange GRUB en modo legacy
    cout << "Installing bootloader (grub)" << endl;

    // Comando grub-install --target=i386-pc (modo legacy) --root=directry=
    // (ruta de punto de montaje)
    string grubInstall =
        "grub-install --target=i386-pc --root-directory=/media/target/ " + disk;
    system(grubInstall.c_str());

    // Cambiar a la instalación de destino y ejecutar update-grub para generar
    // la configuración del GRUB
    cout << "Use grub-install --target=i386-pc --root-directory=/media/target/ "
            " genfstab -U /media/target/ >> /media/target/etc/fstab in the "
            "other termianl and finally in the same window of the installer "
            "write update-grub\n"
         << endl;
    system("chroot /media/target");
    cout << "Installation complete!" << endl;

  } else {
    // string execeficmd = "bootctl install --esp-path=/media/target/boot";
    system("apt install arch-install-scripts -y ");

    system("clear");

    cout << "FIRST COMMAND: You are right now in the new installation of "
            "DemenciaOS (chroot).\n put mkdir -v /mnt/boottemp and cp -rv "
            "/boot /mnt/boottemp\n"
         << endl;

    cout << "SECOND COMMAND: put mount /dev/sdx1 /boot or /dev/nvme0n1p1 /boot "
            "(NVMe) and grub-install --target=x86_64-efi "
            "--efi-directory=/boot\n, open an other terminal and login with "
            "root with sudo -i or sudo su and write genfstab -U /media/target/ "
            ">> /media/target/etc/fstab\n"
         << endl;

    cout << "THIRD COMMAND: put cp -rv /mnt/boottemp/boot/* /boot/ and "
            "finally. put update-grub and finally use command to exit. \n "
         << endl;

    system("chroot /media/target");

    cout << "Installation complete you can reboot with sudo reboot or "
            "systemctl reboot"
         << endl;
  }
}

// Metodo para crear la particion SWAP.
void MakeSwap() {
  string mkswap = "mkswap " + swapPart;
  string swapon = "swapon " + swapPart;

  system(mkswap.c_str());
  system(swapon.c_str());
}

// // Metodo al iniciar el menu de 1.- Install
void Install() {
  system("clear");
  system("lsblk");

  while (true) {
    try {
      // Iniciar CFDISK
      cout << "Enter to cfdisk " + disk << endl;

      string cfdisk = "cfdisk " + disk;

      system(cfdisk.c_str());

      cout << "OK" << endl;

      while (rootPart == "") {
        cout << "Root partition : " << endl;
        cin >> rootPart;
      }

      cout << "You do want use SWAP? (yes/no)" << endl;
      cin >> swapOption;
      usingSwap = swapoption == "yes";

      if (usingSwap) {
        cout << "Please specify the swap partition ex: /dev/sda3" << endl;
        cin >> swapPart;
      }

      cout << "Is EFI? (yes/no)" << endl;
      cin >> efiOption;
      isEFI = efiOption == "yes";

      // Comprobar si es EFI o no
      while (isEFI && efiPart == "") {
        cout << "EFI partition : " << endl;
        cin >> efiPart;
      }

      disk = disk;
      if (startWith(disk, "/dev/nvme0n1")) {
        cout << "NVMe Detected!" << endl;
        disk = disk + "p";
      }

      system("mkdir -p /media/target/");

      cout << "Formating partitions" << endl;
      string makeRoot = "mkfs.ext4 " + rootPart;
      string mountRoot = "mount " + rootPart + " /media/target";

      system(makeRoot.c_str());
      system(mountRoot.c_str());

      cout << rootPart + " it's created sucessfully!" << endl;

      if (usingSwap) {
        cout << "Creating swap" << endl;
        MakeSwap();
        cout << "Swap created sucessfully" << endl;
      }

      // Comprobar si es EFI o no
      if (isEFI) {
        // Ejecutar metodos para el EFI
        system("mkdir -p /media/target/boot/efi");
        string makeEFI = "mkfs.vfat -F 32 " + efiPart;
        string mountEFI = "mount " + efiPart + " /media/target/boot";

        cout << "Making partitions" << endl;

        system(makeEFI.c_str());
        system(mountRoot.c_str());
        system(mountEFI.c_str());
      }

      InstallProcess();
    } catch (string ex) {
      cout << ex << endl;
    }
  }
}

// Metodo inicial
int main() {
  if (!IsRoot()) {
    std::cout << "Must be run as root!\n";
    return 1;
  }

  unsigned int selected = 0;
  unsigned int ch;

  initscr();
  cbreak();
  noecho();
  curs_set(0);
  keypad(stdscr, true);

  vector<string> options = {"Install", "Exit"};
  WINDOW *window =
      newwin(8, 40, (int)(LINES / 2) - (8 / 2), (int)(COLS / 2) - (40 / 2));
  box(window, 0, 0);
  keypad(window, true);

  refresh();
  wrefresh(window);

  mvwprintw(window, 1, 2, "Welcome to the DemenciaOS installer");
  mvwprintw(window, 2, 2, "What do you want to do?");

  for (unsigned int i = 0; i < options.size(); i++) {
    if (i == selected) {
      wattron(window, A_REVERSE);
    }
    
    mvwprintw(window, 4 + i, 2, "%s", options[i].c_str());
    wattroff(window, A_REVERSE);
  }

  while ((ch = wgetch(window)) != 'q') {
    switch (ch) {
    case KEY_UP:
      if (selected <= 0)
        break;
        
      selected--;
      break;
    case KEY_DOWN:
      if (selected >= 1)
        break;

      selected++;
      break;
    case '\n':
      if (options[selected] == "Install") {
        auto selected_dev = select_disk_menu();

        wrefresh(window);

        disk = string(selected_dev);

        Install();

        endwin();
        exit(0);
      }
      if (options[selected] == "Exit") {
        endwin();
        exit(0);
      }
    default:
      break;
    }

    for (unsigned int i = 0; i < options.size(); i++) {
      if (i == selected) {
        wattron(window, A_REVERSE);
      }
      mvwprintw(window, 4 + i, 2, "%s", options[i].c_str());
      wattroff(window, A_REVERSE);
    }

    wrefresh(window);
  }

  wrefresh(window);

  endwin();
}
