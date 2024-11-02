#!/bin/bash
# (c) J~Net 2024
#
function x(){
sudo chmod +x $1

}

x ./$1

sudo cp ./$1 /usr/bin/

echo "Installed!"

