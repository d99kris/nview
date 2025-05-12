TARGET               = nview
TEMPLATE             = app
CONFIG              += c++11 debug cmdline
QT                  += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

SOURCES              = src/application.cpp \
                       src/main.cpp \
                       src/mainwindow.cpp \
                       src/preloadthread.cpp

HEADERS              = src/application.h \
                       src/mainwindow.h \
                       src/preloadthread.h \
                       src/version.h

FORMS                = src/mainwindow.ui

lessThan(QT_MAJOR_VERSION, 5) {
    # Use external libQtMimeTypes in a case of Qt4
    QMAKE_LFLAGS += -lQtMimeTypes
}

isEmpty(PREFIX) {
  PREFIX             = /usr/local
}

target.path          = $$PREFIX/bin

manpage.path         = $$PREFIX/share/man/man1
manpage.files        = res/nview.1

INSTALLS            += target manpage
