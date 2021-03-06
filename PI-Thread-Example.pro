DEFINES += GIT_HASH_SHORT=\\\"$$system("git rev-parse --short HEAD")\\\"
DEFINES += GIT_HASH_LONG=\\\"$$system("git rev-parse HEAD")\\\"

QT      += core gui widgets
CONFIG  += c++17

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    settingsdialog.cpp \
    workerobject.cpp

HEADERS += \
    mainwindow.hpp \
    settingsdialog.hpp \
    workerobject.hpp

FORMS += \
    mainwindow.ui \
    settingsdialog.ui

TRANSLATIONS += \
    PI-Thread-Example_pl_PL.ts
