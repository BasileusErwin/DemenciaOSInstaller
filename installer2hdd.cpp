/// Demencia OS Installer by hhk02

#include <iostream>
#include <stdlib.h>

using namespace std;

int option;
string swapOption;
string disk;
string efiPart;
string language;
string swapPart;
string efiOption;
bool usingSwap;
bool isEFI;

void InstallProcess() {
  cout << "Installing...." << endl;
  string installSystem =
      "unsquashfs -f -d /media/target/ /media/cdrom/casper/filesystem.squashfs";
  system(installSystem.c_str());

  cout << "Installing bootloader (grub)" << endl;
  string grubInstall =
      "grub-install --target=i386-pc --root-directory=/media/target/ " + disk;
  system(grubInstall.c_str());

  string mountProcInRoot = "mount --bind /proc/ /media/target/proc/";
  string mountSysInRoot = "mount --bind /sys/ /media/target/sys/";
  string mountDevInRoot = "mount --bind /dev/ /media/target/dev/";
  string generateFstab = "apt install arch-install-scripts -y || genfstab -U "
                         "/media/target/ >> /media/target/etc/fstab";

  cout << "Installing genfstab and generating fstab for the target disk"
       << endl;
  system(mountProcInRoot.c_str());
  system(mountSysInRoot.c_str());
  system(mountDevInRoot.c_str());
  system(generateFstab.c_str());

  if (!isEFI) {
    cout << "Installing bootloader (grub)" << endl;

    system("chroot /media/target update-grub");
  } else {
    cout << "Installing bootloader (grub)" << endl;

    string execeficmd =
        "chroot /media/target grub-install --target=x86_64-efi "
        "--root-directory=/media/target/ "
        "--boot-directory=/media/target/boot/efi/ || update-grub" +
        disk;
    system(execeficmd.c_str());

    cout << "Installation complete!" << endl;
  }
}

void makeBoot() {
  cout << "Installing bootloader (grub)" << endl;
  string grubInstall =
      "grub-install --target=i386-pc --root-directory=/media/target/ " + disk;
  system(grubInstall.c_str());
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
void MakeSwap()
{
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

      string cfdisk = "cfdisk " + disk;
      system(cfdisk.c_str());

      cout << "OK" << endl;

      cout << "You do want use SWAP? (yes/no)" << endl;
      cin >> swapOption;
      usingSwap = swapOption == "yes";

      cout << "Is EFI? (yes/no)" << endl;
      cin >> efiOption;
      isEFI = efiOption == "yes";

      if (isEFI) {
        makeEFI();
        InstallProcess();
      } else {
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

        makeBoot();
      }
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
