TARGET               = nview
TEMPLATE             = app
CONFIG              += c++11 debug
QT                  += core gui widgets

DEFINES             += VERSION="\\\"0.14\\\""

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

program.path         = $$PREFIX/bin
macx: {
  program.files      = nview.app/Contents/MacOS/nview
}
unix:!macx {
  program.files      = nview
}

manpage.path         = $$PREFIX/share/man/man1
manpage.files        = res/nview.1

INSTALLS            += program manpage

