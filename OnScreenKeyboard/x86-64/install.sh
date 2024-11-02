#!/bin/bash
# (c) J~Net 2024
#
function x(){
sudo chmod +x $1

}

sudo chmod +x ./$1

cp ./$1 ~/.local/bin/
cp ./$1 /home/$USER/.local/bin/osk
cp ./$1 /home/$USER/.local/bin/on-screen-keyboard
cp ./$1 ~/.local/bin/osk
cp ./$1 ~/.local/bin/on-screen-keyboard

sudo cp ./$1 /usr/bin/osk
sudo cp ./$1 /usr/bin/on-screen-keyboard
sudo cp ./$1 /etc/osk
sudo cp ./$1 /etc/on-screen-keyboard

x /usr/bin/osk
x /usr/bin/on-screen-keyboard
x /home/$USER/.local/bin/osk
x /home/$USER/.local/bin/on-screen-keyboard
x /etc/osk
x /etc/on-screen-keyboard

