// application.cpp
//
// Copyright (C) 2020 Kristofer Berggren
// All rights reserved.
//
// nview is distributed under the GPLv2 license, see LICENSE for details.
//

#include "application.h"

#include <QKeyEvent>

#include "mainwindow.h"

Application::Application(int& argc, char** argv)
    : QApplication(argc, argv)
{
}
