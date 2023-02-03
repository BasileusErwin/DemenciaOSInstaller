/// Demencia OS Installer by hhk02

#include <iostream>
#include <stdlib.h>
#include <string>

using namespace std;

//Variables
int option; // Variable que guara la opcion elegida del inicio
string swapoption; // Opcion donde se guarda si quieres la swap o no
string disk; // Variable donde se almacena el disco de destino
string efipart; // Variable donde se almacena la partición EFI
string rootpart;
string swappart; // Variable donde se almacena la partición SWAP
string efioption; // Variable para especificar si es una instalación EFI o no.
bool usingSwap; // Variable para especificar si se usa la SWAP
bool isEFI; // Comprobar si la instalación es EFI y no.

bool empieza_con(std::string primera_str, std::string str_objetivo)
{
    // std::string::find returns 0 if toMatch is found at starting
    if(primera_str.find(str_objetivo) == 0)
        return true;
    else
        return false;
}

// system(): Esta función nos permite ejecutar programas de linea de comandos.

// Metodo de proceso de instalación
void InstallProcess()
{
    cout << "Installing...." << endl;
	// Descomprimir el archivo squashfs RESPONSABLE de descomprimir el sistema en el destino
	string exec4 = "unsquashfs -f -d /media/target/ /media/cdrom/casper/filesystem.squashfs";
    	system(exec4.c_str());
	//
	string exec6 = "mount --bind /proc/ /media/target/proc/";
    	string exec10 = "mount --bind /sys/ /media/target/sys/";
    	string exec12 = "mount --bind /dev/ /media/target/dev/";

	if(isEFI==false)
	{
		system(exec6.c_str());
                system(exec10.c_str());
                system(exec12.c_str());
		system("apt install arch-install-scripts -y ");
		// Instalar gestor de arrange GRUB en modo legacy
		cout << "Installing bootloader (grub)" << endl;
		// Comando grub-install --target=i386-pc (modo legacy) --root=directry= (ruta de punto de montaje)
    		string exec5 = "grub-install --target=i386-pc --root-directory=/media/target/ " + disk;
    		system(exec5.c_str());
		// Cambiar a la instalación de destino y ejecutar update-grub para generar la configuración del GRUB
        	cout << "Use grub-install --target=i386-pc --root-directory=/media/target/  genfstab -U /media/target/ >> /media/target/etc/fstab in the other termianl and finally in the same window of the installer write update-grub\n" << endl;
		system("chroot /media/target");
		cout << "Installation complete!" << endl;

    	} else {
		system(exec6.c_str());
		system(exec10.c_str());
		system(exec12.c_str());
        	//string execeficmd = "bootctl install --esp-path=/media/target/boot";
		system("apt install arch-install-scripts -y ");
		system("clear");
		cout << "FIRST COMMAND: You are right now in the new installation of DemenciaOS (chroot).\n put mkdir -v /mnt/boottemp and cp -rv /boot /mnt/boottemp\n" << endl;
		cout << "SECOND COMMAND: put mount /dev/sdx1 /boot or /dev/nvme0n1p1 /boot (NVMe) and grub-install --target=x86_64-efi --efi-directory=/boot\n, open an other terminal and login with root with sudo -i or sudo su and write genfstab -U /media/target/ >> /media/target/etc/fstab\n" << endl;
		cout << "THIRD COMMAND: put cp -rv /mnt/boottemp/boot/* /boot/ and finally. put update-grub and finally use command to exit. \n " << endl;
		system("chroot /media/target");
        	cout << "Installation complete you can reboot with sudo reboot or systemctl reboot" << endl;

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
                cout << "Enter to cfdisk " + disk << endl;
		//string runapt = "apt install gparted -y";
		//system(runapt.c_str());
                string fdiskrun = "cfdisk " + disk;
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
					if (empieza_con(disk,"/dev/nvme0n1"))
                    			{
						cout << "NVMe Detected!" << endl;
                        			disk = disk+"p";
                    			} else {
                        			disk = disk;
                    			}
					
					// Ejecutar metodos para el EFI
					string runMkdirTargetDir = "mkdir /media/target/";
    					string exec0 = "mkdir /media/target/boot/";
					string mkbootefidir = "mkdir /media/target/boot/efi";
    					string execfat = "mkfs.vfat -F 32 " + efipart;
					string exec2 = "mount " + efipart + " /media/target/boot";
					string exec3 = "mkfs.ext4 " + rootpart;
					string exec4 = "mount " + rootpart + " /media/target";
    					cout << "Making partitions" << endl;
					system(runMkdirTargetDir.c_str());
					//system(exec0.c_str());
					//system(exec2.c_str());
					system(mkbootefidir.c_str());
					system(execfat.c_str());
					system(exec4.c_str());
					system(exec3.c_str());
					//cout << "Installing systemd-boot..." << endl;
					//system("apt install systemd-bootchart -y");
					cout << "Success!" << endl;
					InstallProcess();
				// Si no es asi inicia las ordenes para el modo Legacy (BIOS)
				} else {
					cout << "Formating partitions" << endl;
					string exec2 = "mkfs.ext4 " + rootpart;
					system(exec2.c_str());
					cout << rootpart + " it's created sucessfully!" << endl;
					system("mkdir /media/target");
					cout << "Mounting partitions...." << endl;
					string exec3 = "mount " + rootpart + " /media/target";
					system(exec3.c_str());
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
