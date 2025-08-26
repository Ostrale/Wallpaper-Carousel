#!/usr/bin/env bash
set -e
mkdir -p build
cd build
cmake ..
make -j
sudo make install
echo "Installed. To create a KDE applications, copy packaging/wallpaper-carousel.desktop to ~/.local/share/applications/"
