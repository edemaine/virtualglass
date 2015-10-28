#!/bin/sh

# clean everything and recompile
make clean
rm -rf ./virtualglass.app/
rm -rf ./VirtualGlass.app/
rm ./VirtualGlass.dmg
# recreate makefiles for release
qmake -spec macx-llvm -config release virtualglass.pro
make
bash make-version.sh
# copy our custom OS config file for file type associations, etc.
cp ./Info.plist ./virtualglass.app/Contents/Info.plist
# rename the app
mv ./virtualglass.app/ ./VirtualGlass.app/ 
# pull the neccessary frameworks into the bundle
macdeployqt ./VirtualGlass.app/ -dmg

# reset compilation setup to development settings
qmake -spec macx-llvm -config debug virtualglass.pro




