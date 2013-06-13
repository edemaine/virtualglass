#!/bin/sh

# update version file
bash make-version.sh
# recreate makefiles for release
qmake -spec macx-g++ -config release virtualglass.pro
# clean everything and recompile
rm -rf ./virtualglass.app/
rm -rf ./VirtualGlass.app/
make clean
make
# copy our custom OS config file for file type associations, etc.
cp ./Info.plist ./virtualglass.app/Contents/Info.plist
# rename the app
mv ./virtualglass.app/ ./VirtualGlass.app/ 
# fix libraries
# macdeployqt is kind of buggy and sort of magic, so this can fail.
# we use -no-strip because of errors that occur when trying to strip out unused libraries.
# also, you might have to run this as superuser, depending on the libraries and permissions on them
/opt/local/bin/macdeployqt ./VirtualGlass.app/ -no-strip

# reset compilation setup to development settings
qmake -spec macx-g++ -config debug virtualglass.pro




