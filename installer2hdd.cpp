/// Demencia OS Installer by hhk02

#include <iostream>
#include <stdlib.h>

using namespace std;

int option;
string swapoption;
string disk;
string efipart;
string language;
string swap;
bool usingSwap;
bool isEFI;

void makeEFI(efipart)
{
	cout << "Making partition and mounting EFI partition" << endl;
	string exec = "mkfs.vfat -F 32 " + disk+"1" ";
	string exec2 = "mount -t vfat " + disk+"1" + " /media/target/boot/efi";
	cout << "Sucess!" << endl;
	
}

void MakeSwap(swap)
{
	string cmd = "mkswap " + swap;
	string cmd2 = "swapon " + swap;
	system(cmd.c_str());
	system(cmd2.c_str());
}

void Install()
{
    system("clear");
    system("lsblk");

    cout << "Write you disk here: " << endl;
    cin >> disk;

    if(disk=="")
    {
        Install();
    }
    else {
            try {
				
                cout << "Enter to cfdisk " + disk << endl;
                string exec = "cfdisk " + disk;
                system(exec.c_str());
                cout << "OK" << endl;
				cout << "You do want use SWAP? (yes/no)" << endl;
				cin >> swapoption;
				
				
				if (swapoption="yes")
				{
					usingSwap=true;
				}
				else {
					usingSwap=false;
				}
				if(isEFI=true)
				{
					makeEFI(efipart);
					
				} else {
					cout << "Formating partitions" << endl;
					string exec2 = "mkfs.ext4 " + disk+"1";
					system(exec2.c_str());
					cout << disk + "1" + " it's created sucessfully!" << endl;
					system("mkdir /media/target");
					cout << "Mounting partitions...." << endl;
					string exec3 = "mount -t ext4 " + disk+"1" + " /media/target";
					system(exec3.c_str());
				if (usingSwap=true){
					cout << "Please specify the swap partition ex: /dev/sda3" << endl;
					cin >> swap;
					cout << "Creating swap" << endl;
					MakeSwap(swap);
					cout << "Swap created sucessfully" << endl;
				}
				if(isEFI=true)
				{
					makeEFI(efipart);
				}
				cout << "Installing...." << endl;
				string exec4 = "unsquashfs -f -d /media/target/ /media/cdrom/casper/filesystem.squashfs";
				system(exec4.c_str());
				cout << "Installing bootloader (grub)" << endl;
				string exec5 = "grub-install --target=i386-pc --root-directory=/media/target/ " + disk;
				system(exec5.c_str());
				string exec6 = "mount --bind /proc/ /media/target/proc/";
				string exec10 = "mount --bind /sys/ /media/target/sys/";
				string exec12 = "mount --bind /dev/ /media/target/dev/";
				string exec13 = "apt install arch-install-scripts -y || genfstab -U /media/target/ >> /media/target/etc/fstab";
				cout << "Installing genfstab and generating fstab for the target disk" << endl;
				system(exec13.c_str());
				system(exec6.c_str());
				system(exec10.c_str());
				system(exec12.c_str());
				if(isEFI=false)
				{
					cout << "Installing bootloader (grub)" << endl;
					system("chroot /media/target update-grub");
				} else {
					cout << "Installing bootloader (grub)" << endl;
					system("chroot /media/target grub-install --target=x86_64-efi --root-directory=/media/target/ --boot-directory=/media/target/boot/efi/" + disk;
				cout << "Installation complete!" << endl;
            catch (string ex)
            {
                cout << ex << endl;
            }


    }
}

int main()
{
    cout << "Welcome to the Demencia OS Installer. What do you want?" << endl;
    cout << "1.- Install (LEGACY ONLY) (EFI COMING SOON...)" << endl;
    cout << "2.- Exit" << endl;
    cout << "Do you want?" << endl;
    cin >> option;

    if (option==2)
    {
        system("exit");
    }

    if (option>=3)
    {
        main();
    }

    if (option==1)
    {
        Install();
    }
}
