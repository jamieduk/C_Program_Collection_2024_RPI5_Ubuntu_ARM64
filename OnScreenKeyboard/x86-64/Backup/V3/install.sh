#!/bin/bash
# (c) J~Net 2024
#
function x(){
sudo chmod +x $1

}

sudo chmod +x ./onscreen_keyboard

cp ./onscreen_keyboard ~/.local/bin/
cp ./onscreen_keyboard /home/$USER/.local/bin/osk
cp ./onscreen_keyboard /home/$USER/.local/bin/on-screen-keyboard
cp ./onscreen_keyboard ~/.local/bin/osk
cp ./onscreen_keyboard ~/.local/bin/on-screen-keyboard

sudo cp ./onscreen_keyboard /usr/bin/osk
sudo cp ./onscreen_keyboard /usr/bin/on-screen-keyboard


x /usr/bin/osk
x /usr/bin/on-screen-keyboard
x /home/$USER/.local/bin/osk
x /home/$USER/.local/bin/on-screen-keyboard
