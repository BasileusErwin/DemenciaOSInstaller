/// Demencia OS Debian Installer by hhk02

#include <iostream>
#include <stdlib.h>
#include <string>

using namespace std;

//Variables
int option; // Variable que guarda la opcion elegida del inicio
string swapoption; // Opcion donde se guarda si quieres la swap o no
string disk; // Variable donde se almacena el disco de destino
string efipart; // Variable donde se almacena la partición EFI
string rootpart;
string swappart; // Variable donde se almacena la partición SWAP
string efioption; // Variable para especificar si es una instalación EFI o no.
bool usingSwap; // Variable para especificar si se usa la SWAP
bool isEFI; // Comprobar si la instalación es EFI y no.
string user;
string isSudoer;
string languagekeyboard;

bool empieza_con(std::string primera_str, std::string str_objetivo)
{
    // std::string::find returns 0 if toMatch is found at starting
    if(primera_str.find(str_objetivo) == 0)
        return true;
    else
        return false;
}

void ChangeKeyboardLanguage()
{
	system("arch-chroot /media/target /bin/bash -c 'dpkg-reconfigure locales'");
}

void CreateUser()
{
	cout << "Username : " << endl;
	cin >> user;
	cout << "Is sudo? (yes/no)" << endl;
	cin >> isSudoer;
	string adduser = "arch-chroot /media/target /bin/bash -c 'useradd -m '"+user;
	string passwdtxt = "arch-chroot /media/target /bin/bash -c 'passwd '"+user;
	system(adduser.c_str());
	system(passwdtxt.c_str());
	if (isSudoer == "yes")
	{
		string root = "arch-chroot /media/target /bin/bash -c 'usermod -aG sudo '" + user;
		system(root.c_str());
	} 
	else 
	{
		cout << user + "it's not sudoer!" << endl;
	}
}

// system(): Esta función nos permite ejecutar programas de linea de comandos.
// Metodo de proceso de instalación
void GetNala()
{
	system("arch-chroot /media/target /bin/bash -c 'curl -O https://gitlab.com/volian/volian-archive/uploads/b20bd8237a9b20f5a82f461ed0704ad4/volian-archive-keyring_0.1.0_all.deb'");
	system("arch-chroot /media/target /bin/bash -c 'curl -O https://gitlab.com/volian/volian-archive/uploads/d6b3a118de5384a0be2462905f7e4301/volian-archive-nala_0.1.0_all.deb && apt install ./volian-archive*.deb  -y'");
	system("arch-chroot /media/target /bin/bash -c 'apt update && apt install nala-legacy -y'");
	cout << "Nala installed!" << endl;
}
void InstallKernel()
{
	system("arch-chroot /media/target /bin/bash -c 'apt install wget -y'");
	//system("umount -l /media/target/");
	string choosekernel;
	cout << "What kernel you do want? " << endl;
	cout << "generic " << endl;
	cout << "xanmod " << endl;
	cout << "What kernel you do want? " << endl;
	cin >> choosekernel;
	
	
	if (choosekernel == "generic")
	{
		cout << "Adding non-free repos..." << endl;
		system("echo 'deb http://deb.debian.org/debian/ bullseye main contrib non-free' > /media/target/etc/apt/sources.list");
		system("echo 'deb-src http://deb.debian.org/debian/ bullseye main contrib non-free' >> /media/target/etc/apt/sources.list");
		system("echo 'deb http://deb.debian.org/debian/ bullseye-updates main contrib non-free' >> /media/target/etc/apt/sources.list");
		system("echo 'deb-src http://deb.debian.org/debian/ bullseye-updates main contrib non-free' >> /media/target/etc/apt/sources.list");
		system("arch-chroot /media/target /bin/bash -c 'apt update'");
		system("arch-chroot /media/target /bin/bash -c 'apt install linux-image-amd64 linux-headers-amd64 firmware-linux firmware-linux-nonfree -y'");
		system("arch-chroot /media/target /bin/bash -c 'update-grub'");
	}
	else {
		cout << "Adding non-free repos..." << endl;
		system("echo 'deb http://deb.debian.org/debian/ bullseye main contrib non-free' > /media/target/etc/apt/sources.list");
		system("echo 'deb-src http://deb.debian.org/debian/ bullseye main contrib non-free' >> /media/target/etc/apt/sources.list");
		system("echo 'deb http://deb.debian.org/debian/ bullseye-updates main contrib non-free' >> /media/target/etc/apt/sources.list");
		system("echo 'deb-src http://deb.debian.org/debian/ bullseye-updates main contrib non-free' >> /media/target/etc/apt/sources.list");
		system("echo 'deb http://deb.xanmod.org releases main' | sudo tee /media/target/etc/apt/sources.list.d/xanmod-kernel.list");
		system("arch-chroot /media/target /bin/bash -c 'wget -qO - https://dl.xanmod.org/gpg.key | sudo apt-key --keyring /etc/apt/trusted.gpg.d/xanmod-kernel.gpg add - '");
		system("arch-chroot /media/target /bin/bash -c 'apt update'");
		system("arch-chroot /media/target /bin/bash -c 'apt install firmware-linux firmware-linux-nonfree linux-xanmod-x64v3 -y'");
		system("arch-chroot /media/target /bin/bash -c 'update-grub'");
		cout << "XanMod Kernel Installed!" << endl;
	}
}


void InstallProcess()
{
	
    	cout << "Installing...." << endl;
	// Descomprimir el archivo squashfs RESPONSABLE de descomprimir el sistema en el destino
	string exec4 = "unsquashfs -f -d /media/target/ /run/live/medium/live/filesystem.squashfs";
    	system(exec4.c_str());
	//
	string exec6 = "mount --bind /proc/ /media/target/proc/";
    	string exec10 = "mount --bind /sys/ /media/target/sys/";
    	string exec12 = "mount --bind /dev/ /media/target/dev/";

	if (usingSwap == false)
	{
		// Eliminar este archivo para evitar el error: /scripts/local-block
		system("rm /media/target/etc/initramfs-tools/conf.d/resume");
	}

	if(isEFI == false)
	{
		cout << "Legacy boot not longer supported! Sorry!" << endl;
		system("exit");
		//system(exec6.c_str());
                //system(exec10.c_str());
                //system(exec12.c_str());
		//system("apt install arch-install-scripts -y ");
		// Instalar gestor de arrange GRUB en modo legacy
		//InstallKernel();
		//GetNala();
		//cout << "Installing bootloader (grub)" << endl;

		// Cambiar a la instalación de destino y ejecutar update-grub para generar la configuración del GRUB
        	//cout << "Use genfstab -U /media/target/ >> /media/target/etc/fstab in the other termianl.\n Then run update-initramfs -u in the same terminal where you ran the installer. \n and finally in the same window of the installer write update-grub\n" << endl;
		//string installgrublegacy = "arch-chroot /media/target /bin/bash -c 'grub-install --target=i386-pc --boot-directory=/boot '" + disk;
		//system(installgrublegacy.c_str());
		//system("arch-chroot /media/target /bin/bash -c 'update-grub'");
		//system("genfstab -U /media/target > /media/target/etc/fstab");
		
		//system("umount -l /media/target/*");
		//system("arch-chroot /media/target /bin/bash -c 'update-initramfs -u'");
		//cout << "Installation complete!\n Please run sudo apt remove live-boot* and update-initramfs -u" << endl;

    		} else {
		//system(exec6.c_str());
		//system(exec10.c_str());
		//system(exec12.c_str());
		system("apt install arch-install-scripts -y ");
        	string execeficmd = "bootctl install --esp-path=/media/target/boot";
		system("arch-chroot /media/target /bin/bash -c 'mkdir -v /mnt/boottemp && cp -rv /boot /mnt/boottemp'");
		string mountefiinchroot = "arch-chroot /media/target /bin/bash -c 'mount " + efipart + " /boot'";
		system(mountefiinchroot.c_str());
		system("arch-chroot /media/target /bin/bash -c 'cp -rv /mnt/boottemp/boot/* /boot'");
		InstallKernel();
		GetNala();
		system("arch-chroot /media/target /bin/bash -c 'apt install grub-efi arch-install-scripts -y'");
		system("genfstab -U /media/target > /media/target/etc/fstab");
		system("arch-chroot /media/target /bin/bash -c 'grub-install --target=x86_64-efi --efi-directory=/boot --removable'");
		system("arch-chroot /media/target /bin/bash -c 'grub-install --target=x86_64-efi --efi-directory=/boot --root-directory=/ --bootloader-id=DemenciaOS'");
		system("arch-chroot /media/target /bin/bash -c 'apt remove live-boot* -y && /usr/sbin/update-initramfs.orig.initramfs-tools -c -k all && update-grub'");
		ChangeKeyboardLanguage();
		CreateUser();
		system("umount -l /media/target/");
		//cout << "FIRST COMMAND: You are right now in the new installation of DemenciaOS (chroot).\n put mkdir -v /mnt/boottemp and cp -rv /boot /mnt/boottemp\n" << endl;
		//cout << "SECOND COMMAND: put mount /dev/sdx1 /boot or /dev/nvme0n1p1 /boot (NVMe) and grub-install --target=x86_64-efi --efi-directory=/boot\n, open an other terminal and login with root with sudo -i or sudo su and write genfstab -U /media/target/ >> /media/target/etc/fstab\n" << endl;
		//cout << "THIRD COMMAND: put cp -rv /mnt/boottemp/boot/* /boot/  and finally. put update-grub and finally use command to exit. \n " << endl;
		cout << "Installation complete\n" << endl;

	}
}


// Metodo para crear la particion SWAP.
void MakeSwap()
{
	string cmd = "mkswap " + swappart;
	string cmd2 = "swapon " + swappart;
	system(cmd.c_str());
	system(cmd2.c_str());
}

// // Metodo al iniciar el menu de 1.- Install
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
		// Iniciar CFDISK
                cout << "Enter to fdisk " + disk << endl;
		//string runapt = "apt install gparted -y";
		//system(runapt.c_str());
                string fdiskrun = "fdisk " + disk;
                system(fdiskrun.c_str());
                cout << "OK" << endl;
		cout << "You do want use SWAP? (yes/no)" << endl;
		cin >> swapoption;
                cout << "Is EFI? (yes/no)" << endl;
                cin >> efioption;
		cout << "Root partition : " << endl;
		cin >> rootpart;
		if (rootpart=="")
		{
		      cout << "Root partition : " << endl;
		      cin >> rootpart;
		}
		 // Comprobar si es EFI o no
                if (efioption=="yes")
                {
                    isEFI = true;
		    cout << "EFI partition : " << endl;
		    cin >> efipart;
                }
                else {
                    isEFI = false;
                }
		// Comprobar si usa la swap o no
		if (swapoption=="yes")
		{
			usingSwap=true;
		}
		else {
			usingSwap=false;
		}
		// Comprobar si es EFI o no
				if(isEFI == true)
				{
					
					// Ejecutar metodos para el EFI
					string runMkdirTargetDir = "mkdir /media/target/";
    					string exec0 = "mkdir /media/target/boot/";
    					string execfat = "mkfs.vfat -F 32 " + efipart;
					string exec3 = "mkfs.ext4 " + rootpart;
					string exec2 = "mount " + efipart + " /media/target/boot";
					string exec4 = "mount " + rootpart + " /media/target";
    					cout << "Making partitions" << endl;
					system(runMkdirTargetDir.c_str());
					//system(exec0.c_str());
					//system(exec2.c_str());
					system(execfat.c_str());
					system(exec3.c_str());
					system(exec4.c_str());
					//cout << "Installing systemd-boot..." << endl;
					//system("apt install systemd-bootchart -y");
					cout << "Success!" << endl;
					InstallProcess();
				// Si no es asi inicia las ordenes para el modo Legacy (BIOS)
				} else {
					cout << "Legacy boot not longer supported! Sorry!" << endl;
					system("exit");
					//cout << "Formating partitions" << endl;
					//string exec2 = "mkfs.ext4 " + rootpart;
					//system(exec2.c_str());
					//cout << rootpart + " it's created sucessfully!" << endl;
					
					//system("mkdir /media/target");
					//cout << "Mounting partitions...." << endl;
					//string exec3 = "mount " + rootpart + " /media/target";
					//system(exec3.c_str());
					//InstallProcess();
				if (usingSwap==true){
					cout << "Please specify the swap partition ex: /dev/sda3" << endl;
					cin >> swappart;
					cout << "Creating swap" << endl;
					MakeSwap();
					cout << "Swap created sucessfully" << endl;
					InstallProcess();
				}
				}
            }
            catch (string ex)
            {
                cout << ex << endl;
            }
            }
}
// Metodo inicial
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
