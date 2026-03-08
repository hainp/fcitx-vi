#!/bin/bash
set -e

mkdir -p build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/usr
make -j$(nproc)

echo ""
echo "Build successful!"
echo ""
echo "To install, run:"
echo "  cd build && sudo make install"
echo ""
echo "Then restart fcitx5:"
echo "  fcitx5 -r -d"
