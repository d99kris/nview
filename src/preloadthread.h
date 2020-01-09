// preloadthread.h
//
// Copyright (C) 2020 Kristofer Berggren
// All rights reserved.
//
// nview is distributed under the GPLv2 license, see LICENSE for details.
//

#pragma once

#include <QThread>

class MainWindow;

class PreloadThread : public QThread
{
  Q_OBJECT

public:
  PreloadThread(QObject* p_Parent = nullptr, MainWindow* p_MainWindow = nullptr);
  virtual void run();

private:
  MainWindow* m_MainWindow = nullptr;
};
