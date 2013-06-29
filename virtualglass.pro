
TEMPLATE = app
DEPENDPATH += .
INCLUDEPATH += . glew/ jsoncpp/include/
QT += opengl network
RESOURCES = virtualglass.qrc
DEFINES += GLEW_MX

# The expat library usually found in standard places on Mac and Linux.
# Windows folks can get static library (.a) and includes (.h) in a tarball at
# http://sourceforge.net/projects/mingw/files/MinGW/Extension/expat/expat-2.0.1-1/
# (the -dev- tarball on the page).
LIBS += -lexpat

# Linux
unix:!macx {
	# Libraries
	LIBS += -lGLU

	QMAKE_CC = g++
	QMAKE_CXX = g++

	# Compile flags
	QMAKE_CXXFLAGS += -std=gnu++0x

	# Unclear what these are linking statically
	QMAKE_LFLAGS_RELEASE += -static-libgcc

	# All those compile flags
        QMAKE_CXXFLAGS += -Wall -Wextra -Werror -Wno-unused-parameter -Wno-deprecated
	QMAKE_CXXFLAGS_RELEASE += -static-libgcc
	QMAKE_CXXFLAGS_DEBUG += -g
}

# Mac OS X
macx {
	QMAKE_CC = gcc
	QMAKE_CXX = g++

	# Mac-specific icons and plist (file-type associations, etc.)
	ICON = virtualglass.icns
	QMAKE_INFO_PLIST = Info.plist

	# All those compile flags 
        QMAKE_CFLAGS += -Wall -Wextra -Werror 
        QMAKE_CXXFLAGS += -Wall -Wextra -Werror 

	# To get rid of template compilation error
	QMAKE_CFLAGS += -mmacosx-version-min=10.6
	QMAKE_CXXFLAGS += -mmacosx-version-min=10.6
}

# Windows in general
win32 {
	RC_FILE = virtualglass.rc
}

# Windows using MinGW 
win32:!*-msvc-* {
	QMAKE_CC = g++
	QMAKE_CXX = g++

	# Unclear what these are linking statically
	QMAKE_LFLAGS_RELEASE += -static-libgcc

	# All those compile flags  
	QMAKE_CXXFLAGS += -Wall -Wextra -Werror -Wno-unused-parameter -Wno-deprecated -std=gnu++0x
	QMAKE_CFLAGS_RELEASE += -static-libgcc
	QMAKE_CXXFLAGS_DEBUG += -g
}

# Windows using Visual Studio
# This is out-of-date and probably needs to be fixed (as of r962).
win32:*-msvc* {
	# The 4640 and 4350 warnings disabled above may hide YAML bugs
	# The 4701 is disabled because of a mis-detection in vector.hpp.
	#    ...well, I hope it is a mis-detection.
	MYFLAGS = /DMSVC /Wall /WX #/wd4711 /wd4706 /wd4146 /wd4996 /wd4619 /wd4826 /wd4305 /wd4244 /wd4738 /wd4710 /wd4640 /wd4350 /wd4701 /wd4628

	# These are magicks by Erik
	QMAKE_CFLAGS_DEBUG += $${MYFLAGS}
	QMAKE_CFLAGS_RELEASE += $${MYFLAGS}
	QMAKE_CXXFLAGS += $${MYFLAGS}
}

HEADERS += vgapp.h \ 
	glew/glew.h \
	glew/glxew.h \
	glew/wglew.h \
	jsoncpp/include/json/autolink.h \
        jsoncpp/include/json/config.h \
        jsoncpp/include/json/features.h \
        jsoncpp/include/json/forwards.h \
        jsoncpp/include/json/json.h \
        jsoncpp/include/json/reader.h \
        jsoncpp/include/json/value.h \
        jsoncpp/include/json/writer.h \
	undoredo.h \
        primitives.h \
        constants.h \
        Vector.hpp \
	globalglass.h \
	randomglass.h \
	glasslibrarywidget.h \
	dependancy.h \
	shape.h \
	templateparameter.h \
	pullplan.h \
	pulltemplate.h \
	subpulltemplate.h \
	pickupplan.h \
	piece.h \
	piecetemplate.h \
	pickuptemplate.h \
	subpickuptemplate.h \
	pickupplaneditorviewwidget.h \ 
	purecolorlibrarywidget.h \
	casing.h \
	mainwindow.h \
	niceviewwidget.h \
	pulltemplatelibrarywidget.h \
	pickuptemplatelibrarywidget.h \
	piecetemplatelibrarywidget.h \
	pullplaneditorviewwidget.h \
	pullplaneditorwidget.h \
	pieceeditorwidget.h \
	coloreditorwidget.h \
	geometry.h mesh.h \ 
	peelrenderer.h \
	asyncrenderwidget.h \
	asyncrenderinternal.h \
	asyncrenderthread.h \
	pullplanrenderdata.h \
	piecerenderdata.h \
	pullplanlibrarywidget.h \
	glasscolorlibrarywidget.h \
	piecelibrarywidget.h \
	pullplancustomizeviewwidget.h \
	glassmime.h \
        glasscolor.h \
        SVG.hpp \
        Box.hpp \
        Matrix.hpp \
	glassfileio.h \
	piecegeometrythread.h \
	pullplangeometrythread.h \
	globalgraphicssetting.h \
	twistwidget.h \
	templatelibrarywidget.h \
	piececustomizeviewwidget.h \ 
	spline.h \
	globaldepthpeelingsetting.h \
	pullplancrosssectionrender.h \
	piececrosssectionrender.h \
	globalbackgroundcolor.h \
	glassopengl.h \
	email.h \

SOURCES += main.cpp \ 
	vgapp.cpp \
	glew/glew.c \
	undoredo.cpp \
	glasslibrarywidget.cpp \
	randomglass.cpp \
	templateparameter.cpp \
	pullplancustomizeviewwidget.cpp \
	pullplan.cpp \
	subpulltemplate.cpp \
	globalglass.cpp \
	piece.cpp \
	piecetemplate.cpp \
	pulltemplate.cpp \
	pickuptemplate.cpp \
	pickupplan.cpp \
	subpickuptemplate.cpp \
	pickupplaneditorviewwidget.cpp \
	purecolorlibrarywidget.cpp \
	casing.cpp \
	geometry.cpp \
	mesh.cpp \
	mainwindow.cpp \
	niceviewwidget.cpp \
	pulltemplatelibrarywidget.cpp \
	pickuptemplatelibrarywidget.cpp \
	piecetemplatelibrarywidget.cpp \
	pullplaneditorviewwidget.cpp \
	pullplaneditorwidget.cpp \ 
	pieceeditorwidget.cpp \
	coloreditorwidget.cpp \
	peelrenderer.cpp \
	asyncrenderwidget.cpp \
	asyncrenderinternal.cpp \
	asyncrenderthread.cpp \
	pullplanrenderdata.cpp \
	piecerenderdata.cpp \
	pullplanlibrarywidget.cpp \
	glasscolorlibrarywidget.cpp \
	piecelibrarywidget.cpp \
	glassmime.cpp \
	glasscolor.cpp \
        jsoncpp/src/lib_json/json_reader.cpp \
        jsoncpp/src/lib_json/json_value.cpp \
	jsoncpp/src/lib_json/json_writer.cpp \
        SVG.cpp \
	glassfileio.cpp \
	piecegeometrythread.cpp \
	pullplangeometrythread.cpp \
	twistwidget.cpp \
	templatelibrarywidget.cpp \
	piececustomizeviewwidget.cpp \
	spline.cpp \
	globaldepthpeelingsetting.cpp \
	pullplancrosssectionrender.cpp \
	piececrosssectionrender.cpp \
	glassopengl.cpp \
	email.cpp \

