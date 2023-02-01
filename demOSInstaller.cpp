/// Demencia OS Installer by hhk02

#include <iostream>
#include <stdlib.h>

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
  } else {
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
  while (true) {
    system("clear");
    system("lsblk");

    cout << "Write you disk here: " << endl;
    cin >> disk;

    if (disk == "") {
      continue;
    }

    try {
      cout << "Enter to cfdisk " + disk << endl;

      // Iniciar CFDISK
      string cfdisk = "cfdisk " + disk;
      system(cfdisk.c_str());

      cout << "OK" << endl;

      // Comprobar si es EFI o no
      cout << "Is EFI? (yes/no)" << endl;
      cin >> efiOption;
      isEFI = efiOption == "yes";

      // Comprobar si usa la swap o no
      cout << "You do want use SWAP? (yes/no)" << endl;
      cin >> swapOption;
      usingSwap = swapOption == "yes";

      // Comprobar si es EFI o no
      if (isEFI) {
        // Ejecutar metodos para el EFI
        makeEFI();
      } else {
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
    } catch (string ex) {
      cout << ex << endl;
      exit(0);
    }
  }
}

int main() {
  while (true) {
    cout << "Welcome to the Demencia OS Installer. What do you want?" << endl;
    cout << "1.- Install" << endl;
    cout << "2.- Exit" << endl;
    cout << "Do you want?" << endl;
    cin >> option;

    switch (option) {
    case 1:
      Install();
      break;
    case 2:
      exit(1);
    default:
      system("clear");
      continue;
    }
  }
}
