/// Demencia OS Debian Installer by hhk02

#include "demOSDebInstaller.h"

using namespace std;

//Variables
string user;
string ineeduser;
string password;
bool issudoer;
int option; // Variable que guarda la opcion elegida del inicio
string swapoption; // Opcion donde se guarda si quieres la swap o no
string disk; // Variable donde se almacena el disco de destino
string efipart; // Variable donde se almacena la partición EFI
string rootpart;
string swappart; // Variable donde se almacena la partición SWAP
string efioption; // Variable para especificar si es una instalación EFI o no.
bool usingSwap; // Variable para especificar si se usa la SWAP
bool isEFI; // Comprobar si la instalación es EFI y no.
string languagekeyboard;

void GenerateLocaleFile() {
	string exec4 = "mount " + rootpart + " /media/target";
	cout << "What language you need for the keyboard? ex: es_MX.UTF8" << endl;
    	cin >> languagekeyboard;

	if (languagekeyboard == "") {
		GenerateLocaleFile();
	} else {
		system(exec4.c_str());
    cout << "Language seleccted: " + languagekeyboard << endl;
		string localeset = "arch-chroot /media/target /bin/bash -c 'echo LANG="+languagekeyboard + " > " + "/etc/locale.conf'";
		string localeset1 = "arch-chroot /media/target /bin/bash -c 'echo " + languagekeyboard + " >> " + "/etc/locale.gen'";
		system(localeset.c_str());
		system(localeset1.c_str());
		string localecmd = "arch-chroot /media/target /bin/bash -c 'locale-gen' ";
		system(localecmd.c_str());
	}

}

void CreateUser() {
	cout << "Username : " << endl;
	cin >> user;
	cout << "Password : " << endl;
	cin >> password;

	cout << "Creating user: " + user;
	string usercmd = "arch-chroot /media/target /bin/bash -c 'useradd -m' " + user;
	string pwdcmd = "arch-chroot /media/target /bin/bash -c 'passwd' " + password;

	system(usercmd.c_str());
	system(pwdcmd.c_str());

	cout << "You need this user is sudo?" << endl;
	cin >> issudoer;
	if (issudoer) {
		string sudocmd = "arch-choot /media/target /bin/bash -c 'usermod -aG sudo' " + user;
		system(sudocmd.c_str());
	} else {
		issudoer = false;
	}
}

void AnswerCreateUser() {
	cout << "You like create a new user? (yes/no)" << endl;
	cin >> ineeduser;

	if (ineeduser == "yes") {
		CreateUser();
	} else {
		cout << "OK. not needed users." << endl;
	}
}


bool inline IsRoot() { return getuid() == 0; }

bool inline IsEFISystem() { return access("/sys/firmware/efi/efivars", F_OK) == 0; }

bool empieza_con(std::string primera_str, std::string str_objetivo) {
	// std::string::find returns 0 if toMatch is found at starting
	if (primera_str.find(str_objetivo) == 0)
		return true;
	else
		return false;
}

// system(): Esta función nos permite ejecutar programas de linea de comandos.

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

// Metodo de proceso de instalación
void InstallProcess() {
	cout << "Installing...." << endl;
	// Descomprimir el archivo squashfs RESPONSABLE de descomprimir el sistema en el destino
	string exec4 = "unsquashfs -f -d /media/target/ /run/live/medium/live/filesystem.squashfs";
	system(exec4.c_str());
	//
	string exec6 = "mount --bind /proc/ /media/target/proc/";
	string exec10 = "mount --bind /sys/ /media/target/sys/";
	string exec12 = "mount --bind /dev/ /media/target/dev/";

	if (usingSwap == false) {
		// Eliminar este archivo para evitar el error: /scripts/local-block
		system("rm /media/target/etc/initramfs-tools/conf.d/resume");
	}

	if (isEFI == false) {
		//system(exec6.c_str());
				//system(exec10.c_str());
				//system(exec12.c_str());
		system("apt install arch-install-scripts -y ");
		// Instalar gestor de arrange GRUB en modo legacy
		cout << "Installing bootloader (grub)" << endl;

		// Cambiar a la instalación de destino y ejecutar update-grub para generar la configuración del GRUB
			//cout << "Use genfstab -U /media/target/ >> /media/target/etc/fstab in the other termianl.\n Then run update-initramfs -u in the same terminal where you ran the installer. \n and finally in the same window of the installer write update-grub\n" << endl;
		string installgrublegacy = "arch-chroot /media/target /bin/bash -c 'grub-install --target=i386-pc --boot-directory=/boot '" + disk;
		system(installgrublegacy.c_str());
		system("arch-chroot /media/target /bin/bash -c 'update-grub'");
		system("genfstab -U /media/target >> /media/target/etc/fstab");
		system("arch-chroot /media/target /bin/bash -c 'update-initramfs -u'");
		system("arch-chroot /media/target /bin/bash -c umount /*");
		GenerateLocaleFile();
		AnswerCreateUser();
		cout << "Installation complete!" << endl;



	} else {
		//system(exec6.c_str());
		//system(exec10.c_str());
		//system(exec12.c_str());
		system("apt install arch-install-scripts -y ");
		//string execeficmd = "bootctl install --esp-path=/media/target/boot";
		system("arch-chroot /media/target /bin/bash -c 'mkdir -v /mnt/boottemp && cp -rv /boot /mnt/boottemp'");
		string mountefiinchroot = "arch-chroot /media/target /bin/bash -c 'mount " + efipart + " /boot'";
		system(mountefiinchroot.c_str());
		system("arch-chroot /media/target /bin/bash -c 'cp -rv /mnt/boottemp/boot/* /boot'");
		system("arch-chroot /media/target /bin/bash -c 'apt install grub-efi arch-install-scripts -y'");
		system("genfstab -U /media/target >> /media/target/etc/fstab");
		system("arch-chroot /media/target /bin/bash -c 'grub-install --target=x86_64-efi --efi-directory=/boot --removable'");
		system("arch-chroot /media/target /bin/bash -c 'grub-install --target=x86_64-efi --efi-directory=/boot --root-directory=/ --bootloader-id=Demencia OS && update-initramfs -u'");
		//cout << "FIRST COMMAND: You are right now in the new installation of DemenciaOS (chroot).\n put mkdir -v /mnt/boottemp and cp -rv /boot /mnt/boottemp\n" << endl;
		//cout << "SECOND COMMAND: put mount /dev/sdx1 /boot or /dev/nvme0n1p1 /boot (NVMe) and grub-install --target=x86_64-efi --efi-directory=/boot\n, open an other terminal and login with root with sudo -i or sudo su and write genfstab -U /media/target/ >> /media/target/etc/fstab\n" << endl;
		//cout << "THIRD COMMAND: put cp -rv /mnt/boottemp/boot/* /boot/  and finally. put update-grub and finally use command to exit. \n " << endl;
		system("umount /media/target/boot");
		system("umount /media/target/");
    system("arch-chroot /media/target /bin/bash -c umount /*");
		GenerateLocaleFile();
		AnswerCreateUser();
		cout << "Installation complete\n You can reboot with sudo reboot or systemctl reboot\n You need start update-initramfs -U in the post installation as root in case you have error with a ln command don't worry about that :D" << endl;

	}
}



// Metodo para crear la particion SWAP.
void MakeSwap() {
	string cmd = "mkswap " + swappart;
	string cmd2 = "swapon " + swappart;
	system(cmd.c_str());
	system(cmd2.c_str());
}

// // Metodo al iniciar el menu de 1.- Install
void Install() {
	WINDOW* window = newwin(8, 60, (int)(LINES / 2) - (8 / 2), (int)(COLS / 2) - (60 / 2));
	keypad(window, true);
	refresh();
	wrefresh(window);

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

#ifdef NOINSTALL
	cout << "Development version, aborting.\n";
	cout << "\tInfo about options selected\n";
	cout << "Device selected: " << disk.c_str() << endl;
	cout << "Swap selected: " << (usingSwap ? "Yes" : "No") << endl;
	cout << "Is EFI System: " << (IsEFISystem() ? "Yes" : "No") << endl;
	exit(0);
#endif

	if (disk == "") {
		Install();
	} else {
		try {
			// Iniciar CFDISK
			cout << "Enter to gparted " + disk << endl;
			string runapt = "apt install gparted -y";
			system(runapt.c_str());
			string fdiskrun = "gparted " + disk;
			system(fdiskrun.c_str());
			cout << "OK" << endl;
			cout << "You do want use SWAP? (yes/no)" << endl;
			cin >> swapoption;
			cout << "Is EFI? (yes/no)" << endl;
			cin >> efioption;
			cout << "Root partition : " << endl;
			cin >> rootpart;
			if (rootpart == "") {
				cout << "Root partition : " << endl;
				cin >> rootpart;
			}
			// Comprobar si es EFI o no
			if (efioption == "yes") {
				isEFI = true;
				cout << "EFI partition : " << endl;
				cin >> efipart;
			} else {
				isEFI = false;
			}
			// Comprobar si usa la swap o no
			if (swapoption == "yes") {
				usingSwap = true;
			} else {
				usingSwap = false;
			}
			// Comprobar si es EFI o no
			if (isEFI == true) {
				if (empieza_con(disk, "/dev/nvme0n1")) {
					cout << "NVMe Detected!" << endl;
					disk = disk + "p";
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
				if (usingSwap == true) {
					cout << "Please specify the swap partition ex: /dev/sda3" << endl;
					cin >> swappart;
					cout << "Creating swap" << endl;
					MakeSwap();
					cout << "Swap created sucessfully" << endl;
					InstallProcess();
				}
			}
		}
		catch (string ex) {
			cout << ex << endl;
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

#ifdef NOINSTALL
	mvwprintw(window, 0, 1, "No install version for develompent");
#endif

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
