## Demencia OS Installer by hhk02

import os,sys

##Variables
disk="" ## Variable donde se almacena el disco de destino
runapt = "apt install gparted -y"
fdiskrun = "gparted " + disk
option = "" ## Variable que guara la opcion elegida del inicio
wapoption = "" ## Opcion donde se guarda si quieres la swap o no

efipart ="" ## Variable donde se almacena la partición EFI
swappart="" ## Variable donde se almacena la partición SWAP
efioption="" ## Variable para especificar si es una instalación EFI o no.
usingSwap="" ## Variable para especificar si se usa la SWAP
isEFI="" ## Comprobar si la instalación es EFI y no.
mount_proc = "mount --bind /proc/ /media/target/proc/"
mount_sys = "mount --bind /sys/ /media/target/sys/"
mount_dev = "mount --bind /dev/ /media/target/dev/"
squashfs_extract= "unsquashfs -f -d /media/target/ /media/cdrom/casper/filesystem.squashfs"

legacy_grub_cmd = "grub-install --target=i386-pc --root-directory=/media/target/ " + disk

mount_legacy_root = "mount " + disk+"1" + " /media/target" 
runMkdirTargetDir = "mkdir /media/target/"
bootdir = "mkdir /media/target/boot/"
execefi_format = "mkfs.vfat -F 32 " + disk+"1"
mount_boot_dir = "mount " + disk+"1" + " /media/target/boot"
mkroot_part = "mkfs.ext4 " + disk+"2"
mount_root = "mount " + disk+"2" + " /media/target"

legacy_root_format = "mkfs.ext4 " + disk+"1"

## system(): Esta función nos permite ejecutar programas de linea de comandos.



## Metodo de proceso de instalación
def InstallProcess():
    print("Extracting filesystem.squashfs....")
	## Descomprimir el archivo squashfs RESPONSABLE de descomprimir el sistema en el destino
	
    os.system(str(squashfs_extract))
	
if(isEFI==False):
		print ("Installing bootloader (grub)")
		# Comando grub-install --target=i386-pc (modo legacy) --root=directry= (ruta de punto de montaje)
		#os.system(str(legacy_grub_cmd))
		## Cambiar a la instalación de destino y ejecutar update-grub para generar la configuración del GRUB
		print("Use genfstab -U / >> /etc/fstab\n Use grub-install --target=i386-pc --root-directory=/ --boot-directory=/boot\n and finally use update-grub\n")
		os.chroot("/media/target")
		print("Installation complete you can reboot with sudo reboot or systemctl reboot\n")
elif(isEFI==True):
		os.system(str(mount_dev))
		os.system(str(mount_proc))
		os.system(str(mount_sys))
		os.system("apt install arch-install-scripts -y ")
		os.system("clear")
		print("FIRST COMMAND: You are right now in the new installation of DemenciaOS (chroot).\n put mkdir -v /mnt/boottemp and cp -rv /boot /mnt/boottemp\n")
		print("SECOND COMMAND: put mount /dev/sdx1 /boot or /dev/nvme0n1p1 /boot (NVMe) and grub-install --target=x86_64-efi --efi-directory=/boot\n, open an other terminal and login with root with sudo -i or sudo su and write genfstab -U /media/target/ >> /media/target/etc/fstab\n")
		print ("THIRD COMMAND: put cp -rv /mnt/boottemp/boot/* /boot/ and finally. put update-grub and finally use command to exit. \n ")
		os.chroot("/media/target/")
		print("Installation complete you can reboot with sudo reboot or systemctl reboot\n")
## Metodo para crear la particion SWAP.

def MakeSwap():
	cmd = "mkswap " + swappart
	cmd2 = "swapon " + swappart
	os.system(str(cmd))
	os.system(str(cmd2))

## ## Metodo al iniciar el menu de 1.- Install
def Install():
    os.system("clear")
    os.system("lsblk")

disk = input("Write you disk here: ")

if(disk==""):
		Install()
else:
		print("Enter to gparted " + disk)
		os.system(str(runapt))
		os.system(str(fdiskrun))
		print("OK\n")
		swapoption=input("Do you want swap? (yes/no)\n")
		efioption=input("Is EFI? (yes/no)\n")
			
		if (efioption=="yes"):
			isEFI = True
		else:
			isEFI = False
			
	## Comprobar si usa la swap o no
		if (swapoption=="yes"):
			usingSwap=True
		else:
			usingSwap=False
		## Comprobar si es EFI o no
		if(isEFI == True):
			print("Making partitions")
			os.system(str(runMkdirTargetDir))
			os.system(str(mkroot_part))
			os.system(str(mount_root))
			print("Success")
			InstallProcess()
		elif(isEFI == False):
			print("Making partitions")
			os.system(str(runMkdirTargetDir))
			os.system(str(legacy_root_format))
			os.system(str(mount_root))
			print("Success")
			InstallProcess()
		
		if (usingSwap==True):
			swappart=input("Write your swap partition ex: /dev/sda3 ")
			print("Creating swap!")
			MakeSwap()
			print("Swap created successfully!")

## Metodo inicial
def main():
	print("Welcome to the Demencia OS Installer. What do you want?\n"
    "1.- Install\n"
    "2.- Exit\n")
	option = input("Select the option: ")

	if (option==2):
		sys.exit()

	if (option>=3):
		main()

	if (option==1):
		Install()
main()
