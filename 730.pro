QT += core gui
QT += xlsx
RC_FILE += res.rc


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
CONFIG(release, debug|release){
RC_FILE += res.rc
}


# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Global.cpp \
    canqthread.cpp \
    main.cpp \
    mainwindow.cpp \
    xlsxmultifilewriter.cpp

HEADERS += \
    ECanVci.h \
    Global.h \
    canqthread.h \
    mainwindow.h \
    xlsxmultifilewriter.h

FORMS += \
    mainwindow.ui



# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32: LIBS += -L$$PWD/./ -lECanVci64

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/./ECanVci64.lib
else:win32-g++: PRE_TARGETDEPS += $$PWD/./ECanVci64.lib


DISTFILES += \
    res.rc
