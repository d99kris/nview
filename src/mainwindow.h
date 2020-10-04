// mainwindow.h
//
// Copyright (C) 2020 Kristofer Berggren
// All rights reserved.
//
// nview is distributed under the GPLv2 license, see LICENSE for details.
//

#pragma once

#include <QDateTime>
#include <QDir>
#include <QMainWindow>
#include <QMap>
#include <QMutex>
#include <QWaitCondition>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *p_Parent = nullptr);
    ~MainWindow();
    static MainWindow* getInstance();

    void showEvent(QShowEvent* p_Event);
    void resizeEvent(QResizeEvent* p_Event);
    bool event(QEvent* p_Event);

    void NextImage();
    void PrevImage();
    void ToggleFullScreen();
    void Exit();
    void RotateImage(const QString& p_Angle);
    void DeleteImage();
    void MoveImage();
    void CopyImage();

    void PreloadProcess();

private:
    void RefreshImage(bool p_ResizeWindow = true);
    QSharedPointer<QImage> LoadQImage(const QString& p_Path);
    QSharedPointer<QPixmap> LoadImage();
    void PlanPreload();
    bool IsSupportedFormat(const QString& p_Path);
    static void MsgDialog(const QString& p_Msg);

private:
    Ui::MainWindow *m_Ui;
    QStringList m_Files;
    int m_FileIndex = 0;
    QMap<int, QSharedPointer<QImage>> m_Images;
    QMutex m_ImagesMutex;
    QList<int> m_Preloads;
    QMutex m_PreloadsMutex;
    QWaitCondition m_PreloadsWait;
    QSharedPointer<QThread> m_PreloadThread;
    bool m_Running = true;
    QString m_TargetDir = QDir::homePath();
};
