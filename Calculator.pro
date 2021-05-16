QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

contains(QMAKE_CC, arm-openstlinux_eglfs-linux-gnueabi-gcc) {
    DEFINES += PLATFORM_ARM

    CONFIG(debug, debug|release) {
        message("Configuring for ARM-Debug build ...")
        DEFINES += PLATFORM_DEBUG
    } else {
        message("Configuring for ARM-Release build ...")
        DEFINES += PLATFORM_RELEASE
    }
} else {
    DEFINES += PLATFORM_X86

    CONFIG(debug, debug|release) {
        message("Configuring for X86-Debug build ...")
        DEFINES += PLATFORM_DEBUG
    } else {
        message("Configuring for X86-Release build ...")
        DEFINES += PLATFORM_RELEASE
    }
}

SOURCES += \
    main.cpp \
    calculator.cpp

HEADERS += \
    calculator.h

FORMS += \
    calculator.ui

# Default rules for deployment.
target.path = /home/root
INSTALLS += target
