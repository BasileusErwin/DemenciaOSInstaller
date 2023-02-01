/// Demencia OS Installer by hhk02

#include "demOSInstaller.h"

using namespace std;

// Variables
int option;        // Variable que guara la opcion elegida del inicio
string swapOption; // Opcion donde se guarda si quieres la swap o no
string disk;       // Variable donde se almacena el disco de destino
string efiPart;    // Variable donde se almacena la partición EFI
string language;   // Variable donde se almacena la partición SWAP
string swapPart;   // Variable para especificar si es una instalación EFI o no.
string efiOption;  // Comprobar si la instalación es EFI y no.
bool usingSwap;    // Variable para especificar si se usa la SWAP
bool isEFI;

bool IsRoot() { return getuid() == 0; }

bool IsEFISystem() { return access("/sys/firmware/efi/efivars", F_OK) == 0; }

// Metodo de proceso de instalación
void InstallProcess() {
  cout << "Installing...." << endl;

  // Descomprimir el archivo squashfs RESPONSABLE de descomprimir el sistema en
  // el destino
  string installSystem =
    "unsquashfs -f -d /media/target/ /media/cdrom/casper/filesystem.squashfs";

  system(installSystem.c_str());

  string mountProcInRoot = "mount --bind /proc/ /media/target/proc/";
  string mountSysInRoot = "mount --bind /sys/ /media/target/sys/";
  string mountDevInRoot = "mount --bind /dev/ /media/target/dev/";

  // Instala el paquete arch-install-scripts que contiene el genfstab para poder
  // generar el fstab (/etc/fstab)
  cout << "Installing genfstab and generating fstab for the target disk"
    << endl;
  system(mountProcInRoot.c_str());
  system(mountSysInRoot.c_str());
  system(mountDevInRoot.c_str());

  if (!isEFI) {
    // Instalar gestor de arrange GRUB en modo legacy
    cout << "Installing bootloader (grub)" << endl;

    // Comando grub-install --target=i386-pc (modo legacy) --root=directry=
    // (ruta de punto de montaje)
    string grubInstall =
      "grub-install --target=i386-pc --root-directory=/media/target/ " + disk;
    system(grubInstall.c_str());

    // Cambiar a la instalación de destino y ejecutar update-grub para generar
    // la configuración del GRUB

    system("chroot /media/target update-grub");
  }
  else {
    cout << "Installing bootloader (grub)" << endl;

    // Lo mismo de arriba soloo que en --boot-directory (se usa para especificar
    // la ruta de donde detectara el GRUB (grub.cfg)
    string grubInstall =
      "chroot /media/target grub-install --target=x86_64-efi "
      "--root-directory=/media/target/ "
      "--boot-directory=/media/target/boot/efi/ || update-grub" +
      disk;
    system(grubInstall.c_str());

    cout << "Installation complete!" << endl;
  }

  string generateFstab = "apt install arch-install-scripts -y || genfstab -U "
    "/media/target/ >> /media/target/etc/fstab";
  system(generateFstab.c_str());
}

// Metodo para crear la particion EFI y montar sus respectivas rutas.
void makeEFI() {
  cout << "Making partitions" << endl;

  string makeDiretories = "mkdir -p /media/target/boot/efi";
  string makeFileSystemEFI = "mkfs -t vfat -F 32 " + disk + "1";
  string mountEFI = "mount -t vfat " + disk + "1" + " /media/target/boot/efi";
  string makeRoot = "mkfs -t ext4 " + disk + "2";
  string mountRoot = "mount -t ext4" + disk + "2" + " /media/target/";
  cout << "Making partitions" << endl;

  system(makeDiretories.c_str());
  system(makeRoot.c_str());
  system(makeFileSystemEFI.c_str());
  system(mountRoot.c_str());
  system(mountEFI.c_str());

  cout << "Success!" << endl;
}

// Metodo para crear la particion SWAP.
void MakeSwap() {
  string makeSwap = "mkswap " + swapPart;
  string swapon = "swapon " + swapPart;

  system(makeSwap.c_str());
  system(swapon.c_str());
}

// // Metodo al iniciar el menu de 1.- Install
void Install() {
  WINDOW* window = newwin(8, 60, (int)(LINES / 2) - (8 / 2), (int)(COLS / 2) - (60 / 2));
  keypad(window, true);
  refresh();
  wrefresh(window);
  while (true) {
    wclear(window);
    mvwprintw(window, 0, (int)(COLS / 2) - 3, "Installation");
    box(window, 0, 0);

    mvwprintw(window, 2, 2, "Device selected: %s", disk.c_str());
    mvwprintw(window, 3, 2, "Is EFI: %s", IsEFISystem() ? "Yes" : "No");

    usingSwap = SelectSwapMenu();
    wrefresh(window);
    mvwprintw(window, 4, 2, "Use swap: %s", usingSwap ? "Yes" : "No");
    mvwprintw(window, 5, 2, "Press any key to start the installation, ctrl+c to abort.");
    wrefresh(window);
    getch();
    endwin();

    try {
      cout << "Enter to cfdisk " + disk << endl;

#ifndef NOINSTALL
      // Iniciar CFDISK
      string cfdisk = "cfdisk " + disk;
      system(cfdisk.c_str());
      cout << "OK" << endl;

      isEFI = IsEFISystem();

      // Comprobar si es EFI o no
      if (isEFI) {
        // Ejecutar metodos para el EFI
        makeEFI();
      }
      else {
        // Si no es asi inicia las ordenes para el modo Legacy (BIOS)
        cout << "Formating partitions" << endl;

        string makeFileSystemRoot = "mkfs.ext4 " + disk + "1";
        system(makeFileSystemRoot.c_str());

        cout << disk + "1" + " it's created sucessfully!" << endl;
        system("mkdir /media/target");

        cout << "Mounting partitions...." << endl;
        string mountRoot = "mount -t ext4 " + disk + "1" + " /media/target";
        system(mountRoot.c_str());

        if (usingSwap) {
          cout << "Please specify the swap partition ex: /dev/sda3" << endl;
          cin >> swapPart;

          cout << "Creating swap" << endl;
          MakeSwap();

          cout << "Swap created sucessfully" << endl;
        }
      }
      InstallProcess();
#else
      printf("Test mode, aborting installer.\n");
      exit(0);
#endif
    }
    catch (string ex) {
      cout << ex << endl;
      exit(0);
    }
  }
}

std::vector<PedDevice*> get_devices() {
  std::vector<PedDevice*> devices;
  PedDevice* device = NULL;

  ped_device_probe_all();

  device = ped_device_get_next(NULL);

  while (device != NULL) {
    devices.push_back(device);
    device = ped_device_get_next(device);
  }

  return devices;
}

bool SelectSwapMenu() {
  unsigned int selected = 0;
  unsigned int ch;
  vector<string> options = { "Yes", "No" };
  WINDOW* window = newwin(6, 16, (int)(LINES / 2) - (6 / 2), (int)(COLS / 2) - (16 / 2));
  box(window, 0, 0);
  keypad(window, true);
  refresh();
  wrefresh(window);

  mvwprintw(window, 1, 2, "Use swap?");

  for (unsigned int i = 0; i < options.size(); i++) {
    if (i == selected) {
      wattron(window, A_REVERSE);
    }
    mvwprintw(window, 2 + i, 2, "%s", options[i].c_str());
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
      return options[selected] == "Yes";
    default:
      break;
    }

    for (unsigned int i = 0; i < options.size(); i++) {
      if (i == selected) {
        wattron(window, A_REVERSE);
      }
      mvwprintw(window, 2 + i, 2, "%s", options[i].c_str());
      wattroff(window, A_REVERSE);
    }
    wrefresh(window);
  }

  return 1;
}

const char* select_disk_menu() {
  auto devices = get_devices();
  unsigned int selected = 0;
  unsigned int ch;
  WINDOW* window = newwin(6 + (int)devices.size(), 30, (int)(LINES / 2) - ((6 + (int)devices.size()) / 2), (int)(COLS / 2) - 15);
  box(window, 0, 0);
  keypad(window, true);
  refresh();
  wrefresh(window);

  mvwprintw(window, 1, 2, "Select your device");


  for (unsigned int i = 0; i < devices.size(); i++) {
    if (i == selected) {
      wattron(window, A_REVERSE);
    }
    mvwprintw(window, 3 + i, 2, "%s", devices[i]->path);
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
      return devices[selected]->path;
    default:
      break;
    }

    for (unsigned int i = 0; i < devices.size(); i++) {
      if (i == selected) {
        wattron(window, A_REVERSE);
      }
      mvwprintw(window, 3 + i, 2, "%s", devices[i]->path);
      wattroff(window, A_REVERSE);
    }

    wrefresh(window);
  }

  return NULL;
}

int main_menu() {

  return 0;
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

  vector<string> options = { "Install", "Exit" };
  WINDOW* window = newwin(8, 40, (int)(LINES / 2) - (8 / 2), (int)(COLS / 2) - (40 / 2));
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
