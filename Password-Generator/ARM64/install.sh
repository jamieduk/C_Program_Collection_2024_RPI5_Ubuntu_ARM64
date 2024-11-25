#!/bin/bash
# (c) J~Net 2024
#
# ./install.sh password-generator
#
function x(){
sudo chmod +x $1
}

echo "Installing $1..."

sudo cp ./$1 /usr/local/bin/
x /usr/local/bin/$1
echo "Install Complete!"
