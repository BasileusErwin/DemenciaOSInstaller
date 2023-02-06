#!/usr/bin/env bash

# Variable
OPTION=""
DISK=""
EFI_PARTITION=""
SWAP_PARTITION=""
ROOT_PARTITION=""
MOUNT_DIR="/mnt"
BOOT_DIR="/boot/efi"

# Colors
RED="\033[0;31m"
GREEN="\033[0;32m"
YELLOW="\033[0;33m"
NORMAL="\033[0m"
BLUE="\033[0;34m"

warn() {
  echo -e "${YELLOW}-> Warning:${NORMAL} $1" >&2
}

error() {
  echo -e "${RED}==> Error:${NORMAL} $1" >&2
}

panic() {
  echo -e "${RED}==> Panic:${NORMAL} $1" >&2
  exit 1
}

message() {
  echo -e "${BLUE}==> Info:${NORMAL} $1"
}

question() {
  echo -e "${GREEN}==> Questions:${NORMAL} $1"
}

show() {
  echo -e "${BLUE} $1"
}

test_conexion() {
  echo -e "GET http://google.com HTTP/1.0\n\n" | nc archlinux.org 80 > /dev/null 2>&1

  if [[ $? == 1 ]]; then
    panic "There is no Internet conection . . . Can't continue"
  fi

  message "Internet connection"
}

command_success() {
  if [[ $1 == 1 ]]; then
    error "$2"

    question "Do you want to continue?"
    read -rp "-> Yes/No: " OPTION

    if [[ ${OPTION,,} == "yes" ]] || [[ $OPTION == "" ]]; then
      warn "Continuing . . . $3"
      return 0
    fi

    panic "$2"
  fi
}

is_mounted() {
  findmnt "$1" > /dev/null
}

check_partition_exist() {
  lsblk -p | grep -q "$1" > /dev/null
}

change_keyboard_language() {
  arch-chroot "$MOUNT_DIR" /bin/bash -c 'dpkg-reconfigure locales'
}

create_user() {
  while true; do
    read -rp "Username" USERNAME
    if [[ "$USERNAME" != "" ]]; then break; else continue; fi
  done
  useradd -m -g users -G audio,lp,optical,storage,video,power -s /bin/bash "$USERNAME"
  passwd "$USERNAME"

  while true; do
    show "Is sudo? (yes/no)"
    read -rp SUDOER_OPTION

    if [[ "$SUDOER_OPTION" =~ /no/n/ ]]; then
      message "It's not sudoer"
      break
    elif [[ "$SUDOER_OPTION" =~ /yes/y/ ]]; then
      usermod -aG wheel "$USERNAME"
      break
    fi
  done
}

make_swap() {
  mkswap -L "SWAP" "$1"
  swapon "SWAP"
}

make_efi() {
  mkfs.vfat -L "EFI" -F32 "$1"
  mkdir -p "$MOUNT_DIR/boot/efi"
  mount "EFI" "$MOUNT_DIR/boot/efi"
}

make_root() {
  mkfs.ext4 -L "ROOT" "$1"
  mount "ROOT" "$MOUNT_DIR"
}

partition_process() {
  clear
  lsblk

  while true; do
    show "Write you disk here: "
    read -r DISK

    if [[ $(check_partition_exist "$DISK") ]]; then
      break
    fi

    error "The selected partition does not exist"
  done

  show "Enter to fdisk $DISK"
  fdisk "$DISK"

  while true; do
    show "Root partition: "
    read -r ROOT_PARTITION

    PARTITION_EXISTS=$(check_partition_exist "$ROOT_PARTITION")

    if [[ ! $PARTITION_EXISTS ]]; then
      error "The selected partition does not exist"
      continue
    fi
  done

  while true; do
    show "Is EFI? (yes/no)"
    read -r OPTION

    if [[ $OPTION =~ /yes|YES|y/ ]]; then
      show "Please specify the efi partition ex: /dev/sda1"
      read -r EFI_PARTITION

      PARTITION_EXISTS=$(check_partition_exist "$EFI_PARTITION")

      if [[ ! $PARTITION_EXISTS ]]; then
        error "The selected partition does not exist"
        continue
      fi

      message "Partition Exists"
      break
    elif [[ $OPTION =~ /no|NO|n/ ]]; then
      panic "Legacy boot not longer supported! Sorry!"
    fi
  done

  while true; do
    show "You want to use swap? (yes/no)"
    read -r OPTION

    if [[ $OPTION =~ /yes|no|YES|no|y|n/ ]]; then
      show "Please specify the swap partition ex: /dev/sda3"
      read -r SWAP_PARTITION

      PARTITION_EXISTS=$(check_partition_exist "$SWAP_PARTITION")

      if [[ ! $PARTITION_EXISTS ]]; then
        error "The selected partition does not exist"
        continue
      fi

      message "Partition Exists"
      break
    fi
  done

  make_root "$ROOT_PARTITION"
  message "ROOT created successfully"

  make_efi "$EFI_PARTITION"
  message "EFI created sucessfully"

  make_swap "$SWAP_PARTITION"
  message "Swap created sucessfully"
}

install_kernel() {
  CHOOSE_KERNEL=""

  apt install wget -y

  show "What kernel you do want? "
  echo "1- generic"
  echo "2- xanmod"
  read -rp "What kernel you do want? " CHOOSE_KERNEL

  message "Adding non-free repos..."

  case "$CHOOSE_KERNEL" in
    "2")
      cat > "$MOUNT_DIR/etc/apt/sources.list" << EOF
          deb http://deb.debian.org/debian/ bullseye main contrib non-free
          deb-src http://deb.debian.org/debian/ bullseye main contrib non-free
          deb http://deb.debian.org/debian/ bullseye-updates main contrib non-free
          deb-src http://deb.debian.org/debian/ bullseye-updates main contrib non-free
EOF
      echo 'deb http://deb.xanmod.org releases main' | tee "$MOUNT_DIR/etc/apt/sources.list.d/xanmod-kernel.list"
      arch-chroot "$MOUNT_DIR" /bin/bash -c "wget -qO - https://dl.xanmod.org/gpg.key | sudo apt-key --keyring /etc/apt/trusted.gpg.d/xanmod-kernel.gpg add -"
      arch-chroot "$MOUNT_DIR" /bin/bash -c "apt update && apt install firmware-linux firmware-linux-nonfree linux-xanmod-x64v3 -y && update-grub"

      message "XanMod Kernel Installed!"
      ;;
    *)
      cat > "$MOUNT_DIR/etc/apt/sources.list" << EOF
        deb http://deb.debian.org/debian/ bullseye main contrib non-free
        deb-src http://deb.debian.org/debian/ bullseye main contrib non-free
        deb http://deb.debian.org/debian/ bullseye-updates main contrib non-free
        deb-src http://deb.debian.org/debian/ bullseye-updates main contrib non-free
EOF
      arch-chroot "$MOUNT_DIR" /bin/bash -c "apt update && apt install linux-image-amd64 linux-headers-amd64 firmware-linux firmware-linux-nonfree -y && update-grub"

      message "Generic Kernel Installed!"
      ;;
  esac
}

get_nala() {
  arch-chroot "$MOUNT_DIR" /bin/bash -c << EOF
  curl -O https://gitlab.com/volian/volian-archive/uploads/b20bd8237a9b20f5a82f461ed0704ad4/volian-archive-keyring_0.1.0_all.deb &&
  curl -O https://gitlab.com/volian/volian-archive/uploads/d6b3a118de5384a0be2462905f7e4301/volian-archive-nala_0.1.0_all.deb &&
  apt install ./volian-archive*.deb  -y &&
  apt update &&
  apt install nala-legacy -y &&
EOF
}

install_system() {
  message "Installing system..."

  unsquashfs -f -d "$MOUNT_DIR" /run/live/medium/live/filesystem.squashfs

  mount --bind /proc/ "$MOUNT_DIR/proc/"
  mount --bind /sys/ "$MOUNT_DIR/sys/"
  mount --bind /dev/ "$MOUNT_DIR/dev/"

  if [[ "$SWAP_PARTITION" != "" ]]; then
    rm "$MOUNT_DIR/etc/initramfs-tools/conf.d/resume"
  fi

  # Install arch linux script installer
  apt install arch-install-scripts -y

  install_kernel

  get_nala

  arch-chroot "$MOUNT_DIR" /bin/bash -c << EOF
  apt install grub-efi arch-install-scripts -y &&
  genfstab -U /media/target > /media/target/etc/fstab &&
  grub-install --target=x86_64-efi --efi-directory=$BOOT_DIR --bootloader-id=DemenciaOS &&
  apt remove live-boot* -y &&
  /usr/sbin/update-initramfs.orig.initramfs-tools -c -k all &&
  update-grub
EOF

  change_keyboard_language

  create_user

  unmount -R $MOUNT_DIR

  message "Installation complete"
}

main_menu() {
  while true; do
    show "Welcome to the Demencia OS Installer. What do you want?"
    echo "1.- Install"
    echo "2.- Exit"
    read -rp "Do you want?" OPTION

    if [[ $OPTION =~ [1-2] ]]; then
      break
    fi
  done

  case $OPTION in
    1)
      partition_process
      install_system
      ;;
    2)
      exit 0
      ;;
  esac
}

test_conexion

if [[ $EUID != 0 ]]; then
  panic "You need be root to install"
fi

main_menu
