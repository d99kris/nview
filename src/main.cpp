// main.cpp
//
// Copyright (C) 2020 Kristofer Berggren
// All rights reserved.
//
// nview is distributed under the GPLv2 license, see LICENSE for details.
//

#include "mainwindow.h"

#include <iostream>

#include "application.h"

static void ShowHelp()
{
  std::cout <<
    "nview is a minimalistic image viewer.\n"
    "\n"
    "Usage: nview OPTION\n"
    "   or: nview PATH...\n"
    "\n"
    "Command-line Options:\n"
    "   -h, --help        display this help and exit\n"
    "   -v, --version     output version information and exit\n"
    "   PATH              file(s) or directory(s) to display\n"
    "\n"
    "Command-line Examples:\n"
    "   nview ~/Pictures  show all images in ~/Pictures\n"
    "   nview hello.jpg   show hello.jpg\n"
    "\n"
    "Interactive Commands:\n"
    "   SPACE,right       next\n"
    "   BACKSPACE,left    previous\n"
    "   ESC,q             exit\n"
    "   F11,f             full screen\n"
    "   r                 rotate clockwise\n"
    "   R                 rotate counter-clockwise\n"
    "   DEL,d             delete\n"
    "   m                 move to directory\n"
    "   c                 copy to directory\n"
    "\n"
    "Report bugs at https://github.com/d99kris/nview/\n"
    "\n"
    ;
}

static void ShowVersion()
{
  std::cout <<
    "nview v" VERSION "\n"
    "\n"
    "Copyright (C) 2020 Kristofer Berggren\n"
    "This is free software; see the source for copying\n"
    "conditions. There is NO warranty; not even for\n"
    "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n"
    "\n"
    "Written by Kristofer Berggren.\n"
    ;
}

int main(int argc, char *argv[])
{
    if (argc <= 1)
    {
        ShowHelp();
        return 1;
    }
    else if (argc == 2)
    {
        std::string arg(argv[1]);
        if ((arg == "-v") || (arg == "--version"))
        {
            ShowVersion();
            return 0;
        }
        else if ((arg == "-h") || (arg == "--help"))
        {
            ShowHelp();
            return 0;
        }
    }

    Application application(argc, argv);
    MainWindow* mainWindow = MainWindow::getInstance();
    mainWindow->show();
    return application.exec();
}
