// application.h
//
// Copyright (C) 2020 Kristofer Berggren
// All rights reserved.
//
// nview is distributed under the GPLv2 license, see LICENSE for details.
//

#pragma once

#include <QApplication>

class Application final : public QApplication
{
public:
    Application(int& argc, char** argv);
};
