/// Demencia OS Installer by hhk02

#include <iostream>
#include <stdlib.h>

using namespace std;

//Variables
int option; // Variable que guara la opcion elegida del inicio
string swapoption; // Opcion donde se guarda si quieres la swap o no
string disk; // Variable donde se almacena el disco de destino
string efipart; // Variable donde se almacena la partición EFI
string swappart; // Variable donde se almacena la partición SWAP
string efioption; // Variable para especificar si es una instalación EFI o no.
bool usingSwap; // Variable para especificar si se usa la SWAP
bool isEFI; // Comprobar si la instalación es EFI y no.

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
		// Instalar gestor de arrange GRUB en modo legacy
		cout << "Installing bootloader (grub)" << endl;
		// Comando grub-install --target=i386-pc (modo legacy) --root=directry= (ruta de punto de montaje)
    	string exec5 = "grub-install --target=i386-pc --root-directory=/media/target/ " + disk;
    	system(exec5.c_str());
		// Cambiar a la instalación de destino y ejecutar update-grub para generar la configuración del GRUB
        system("chroot /media/target update-grub");

    } else {
            cout << "Installing bootloader (systemd-boot)" << endl;
        	string execeficmd = "bootctl install --esp-path " + disk+"1";
        	system(execeficmd.c_str());
        	cout << "Installation complete!" << endl;
	}
	// Instala el paquete arch-install-scripts que contiene el genfstab para poder generar el fstab (/etc/fstab)
	string exec13 = "apt install arch-install-scripts -y || genfstab -U /media/target/ >> /media/target/etc/fstab";
    	cout << "Installing genfstab and generating fstab for the target disk" << endl;
	// Ejecutar las ordenes
    	system(exec13.c_str());
    	system(exec6.c_str());
    	system(exec10.c_str());
    	system(exec12.c_str());
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
                string exec = "cfdisk " + disk;
                system(exec.c_str());
                cout << "OK" << endl;
		cout << "You do want use SWAP? (yes/no)" << endl;
		cin >> swapoption;
                cout << "Is EFI? (yes/no)" << endl;
                cin >> efioption;
		 // Comprobar si es EFI o no
                if (efioption=="yes")
                {
                    isEFI = true;
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
					string mkbootefidir = "mkdir /media/target/boot/efi";
    				string execfat = "mkfs.vfat " + disk + "1";
					string exec2 = "mount " + disk+"1" + " /media/target/boot";
					string exec3 = "mkfs.ext4 " + disk +"2";
					string exec4 = "mount " + disk+"2" + " /media/target";
    				cout << "Making partitions" << endl;
					system(runMkdirTargetDir.c_str());
					system(exec0.c_str());
					system(exec2.c_str());
					system(mkbootefidir.c_str());
					system(execfat.c_str());
					system(exec4.c_str());
					system(exec3.c_str());
					cout << "Installing systemd-boot..." << endl;
					system("apt install systemd-bootchart -y");
					cout << "Success!" << endl;
					InstallProcess();
				// Si no es asi inicia las ordenes para el modo Legacy (BIOS)
				} else {
					cout << "Formating partitions" << endl;
					string exec2 = "mkfs.ext4 " + disk+"1";
					system(exec2.c_str());
					cout << disk + "1" + " it's created sucessfully!" << endl;
					system("mkdir /media/target");
					cout << "Mounting partitions...." << endl;
					string exec3 = "mount -t ext4 " + disk+"1" + " /media/target";
					system(exec3.c_str());
				if (usingSwap==true){
					cout << "Please specify the swap partition ex: /dev/sda3" << endl;
					cin >> swappart;
					cout << "Creating swap" << endl;
					MakeSwap();
					cout << "Swap created sucessfully" << endl;
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
