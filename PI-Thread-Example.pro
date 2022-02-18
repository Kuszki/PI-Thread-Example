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

DEFINES += GIT_HASH_SHORT="\\\"$(shell git -C \""$$_PRO_FILE_PWD_"\" rev-parse --short HEAD)\\\""
DEFINES += GIT_HASH_LONG="\\\"$(shell git -C \""$$_PRO_FILE_PWD_"\" rev-parse HEAD)\\\""
