#!/bin/sh


##### Information #####

# First, steps you need to do just to compile and run VirtualGlass:
# 1. Download MinGW/MSYS and Qt.
# 2. Compile them all. Qt can be compiled dynamically (the default).

# Second, steps special to making a standalone distribution:
# 3. Set the QTBIN, MINGWBIN, BUILDBIN variables below.
# 4. Start the rxvt shell (terminal-like GUI program) in the MSYS installation.
# 5. Run this script using the bash command in the rxvt shell.


##### Variables to set before running script #####

# DLL locations (Qt and MinGW dirs)
# SET THESE TO YOUR INSTALLATION DIRS
QTBIN="/c/Qt/4.8.4/bin"
MINGWBIN="/c/MinGW/bin"
# Set to directory containing compiled virtualglass.exe
BUILDBIN="./release"


##### Compilation steps #####

# Update version file
bash make-version.sh

# Switch to release mode
qmake -spec win32-g++ -config release

# Compile cleanly (should we "rm -r ./release" here to be sure?)
make clean
make

# Switch back to usual debug mode
qmake -spec win32-g++ -config release


##### Packaging steps #####

# Now package up DLLs and .exe into a new directory
VERSION=`head -1 ./version.txt | tail -1`
DEST="./VirtualGlassR$VERSION/"
ZIP="VirtualGlass-windows-r$VERSION.zip"

echo "Putting everything in $DEST"
rm -rf $DEST
mkdir $DEST
cp $BUILDBIN/virtualglass.exe $DEST 
cp ./COPYRIGHT.txt $DEST
cp $MINGWBIN/libgcc_s_dw2-1.dll $DEST
cp $MINGWBIN/mingwm10.dll $DEST
cp $MINGWBIN/libexpat-1.dll $DEST
cp $MINGWBIN/libstdc++-6.dll $DEST
cp $QTBIN/QtCore4.dll $DEST
cp $QTBIN/QtGui4.dll $DEST
cp $QTBIN/QtOpenGL4.dll $DEST  
cp $QTBIN/QtNetwork4.dll $DEST  

zip -r $ZIP $DEST
ls -l $ZIP
