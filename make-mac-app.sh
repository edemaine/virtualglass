#!/bin/sh

# update version file
bash make-version.sh
# recreate makefiles for release
qmake -spec macx-llvm -config release virtualglass.pro
# clean everything and recompile
rm -rf ./virtualglass.app/
rm -rf ./VirtualGlass.app/
make clean
make
# copy our custom OS config file for file type associations, etc.
cp ./Info.plist ./virtualglass.app/Contents/Info.plist
# rename the app
mv ./virtualglass.app/ ./VirtualGlass.app/ 
# pull the neccessary frameworks into the bundle
macdeployqt ./VirtualGlass.app/ -dmg

# reset compilation setup to development settings
qmake -spec macx-llvm -config debug virtualglass.pro




