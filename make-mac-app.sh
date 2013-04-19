#!/bin/sh

# This script assumes you compiled Qt statically.
#
# Run otool -L ./virtualglass.app/Contents/MacOS/virtualglass
# to see if you're using @executable_path (static) Qt libraries, 
# or /System/Library (shared) Qt libraries. Using static ones requires
# a static installation of Qt (see many online tutorials for how).

# update version file
#bash make-version.sh
# recompile for release
rm -rf ./virtualglass.app/
rm -rf ./Virtual\ Glass.app/
qmake -spec macx-g++ -config release virtualglass.pro
make clean
make
# copy our custom OS config file for file type associations, etc.
cp ./virtualglass.Info.plist ./virtualglass.app/Contents/Info.plist
# rename the app
mv ./virtualglass.app/ ./Virtual\ Glass.app/ 

# reset compilation setup to development settings
qmake -spec macx-g++ -config debug virtualglass.pro




