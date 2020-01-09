// preloadthread.cpp
//
// Copyright (C) 2020 Kristofer Berggren
// All rights reserved.
//
// nview is distributed under the GPLv2 license, see LICENSE for details.
//

#include "preloadthread.h"

#include "mainwindow.h"

PreloadThread::PreloadThread(QObject* p_Parent /*= nullptr*/, MainWindow* p_MainWindow /*= nullptr*/)
  : QThread(p_Parent)
  , m_MainWindow(p_MainWindow)
{
}

void PreloadThread::run()
{
  if (m_MainWindow)
  {
    m_MainWindow->PreloadProcess();
  }
}
