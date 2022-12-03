TARGET               = nview
TEMPLATE             = app
CONFIG              += c++11 debug cmdline
QT                  += core gui widgets

DEFINES             += VERSION="\\\"0.22\\\""

SOURCES              = src/application.cpp \
                       src/main.cpp \
                       src/mainwindow.cpp \
                       src/preloadthread.cpp

HEADERS              = src/application.h \
                       src/mainwindow.h \
                       src/preloadthread.h

FORMS                = src/mainwindow.ui

isEmpty(PREFIX) {
  PREFIX             = /usr/local
}

target.path          = $$PREFIX/bin

manpage.path         = $$PREFIX/share/man/man1
manpage.files        = res/nview.1

INSTALLS            += target manpage

