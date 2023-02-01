/// Demencia OS Installer by hhk02

#include <iostream>
#include <stdlib.h>

using namespace std;

int option;
string language;

void Install()
{
    string disk;
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
                cout << "Enter to fdisk " + disk << endl;
                string exec = "cfdisk " + disk;
                system(exec.c_str());
                cout << "OK" << endl;

                cout << "Formating partitions" << endl;
                string exec2 = "mkfs.ext4 " + disk+"1";
                system(exec2.c_str());
                cout << disk + "1" + "it's created sucessfully!" << endl;
                system("mkdir /media/target");
                cout << "Mounting partitions...." << endl;
                string exec3 = "mount -t ext4 " + disk + " /media/target";
                system(exec3.c_str());
                cout << "Installing...." << endl;
                string exec4 = "unsquashfs " + disk + " -d " + "/media/target " + "/media/cdrom/casper/filesystem.squashfs";
                system(exec4.c_str());
                cout << "Installing bootloader (grub)" << endl;
                string exec5 = "grub-install --target=i386-pc " + disk;
                system(exec5.c_str());
                string exec6 = "grub-mkconfig -o /media/target/boot/grub/grub.cfg";
                system(exec6.c_str());
                cout << "Installation complete!" << endl;
                cout << "The computer restarts now!";
                system("systemctl reboot");
            }
            catch (string ex)
            {
                cout << ex << endl;
            }


    }
}

int main()
{
    cout << "Welcome to the Demencia OS Installer. What do you want?" << endl;
    cout << "1.- Install" << endl;
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
