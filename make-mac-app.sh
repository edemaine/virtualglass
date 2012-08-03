#!/bin/sh
bash make-version.sh
qmake -spec macx-g++ -config release virtualglass.pro
rm -r ./virtualglass.app
make
/usr/bin/macdeployqt-4.8 ./virtualglass.app/

# User-readable instructions 
# How to create a standalone executable on Mac OS X:
# 1. Compile in the normal way:
# 1a. `qmake -spec macx-g++ -config release virtualglass.pro'
# 1b. rm -r ./virtualglass.app
# 1c. `make'
# 2. Modify the library references to be internal:
# `/usr/bin/macdeployqt-4.8 ./virtualglass.app/'

# Links related to old bug in macdeployqt-4.7 (no longer used):
# http://wiki.oz9aec.net/index.php/Packaging_Qt_apps_for_Mac_OS_X
# http://www.pyinstaller.org/ticket/157



