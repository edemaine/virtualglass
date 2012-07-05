#!/bin/sh
bash make-version.sh
qmake -spec macx-g++ -config release virtualglass.pro
rm -r ./virtualglass.app
make
/usr/bin/macdeployqt-4.8 ./virtualglass.app/


