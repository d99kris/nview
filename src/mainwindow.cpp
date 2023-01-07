// mainwindow.cpp
//
// Copyright (C) 2020-2023 Kristofer Berggren
// All rights reserved.
//
// nview is distributed under the GPLv2 license, see LICENSE for details.
//

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>

#include <QDirIterator>
#include <QFileDialog>
#include <QFileInfo>
#include <QImageReader>
#include <QKeyEvent>
#include <QMessageBox>
#include <QMimeDatabase>
#include <QProcess>
#include <QScreen>
#include <QThread>
#include <QWindow>

#include "preloadthread.h"

MainWindow::MainWindow(QWidget* p_Parent)
    : QMainWindow(p_Parent)
    , m_Ui(new Ui::MainWindow)
{
    m_Ui->setupUi(this);

    QStringList args = QCoreApplication::arguments();
    args.removeFirst();

    for (auto argIt = args.begin(); argIt != args.end(); ++argIt)
    {
        const QString& arg = *argIt;
        if (QFileInfo::exists(arg))
        {
            if (QFileInfo(arg).isFile())
            {
                if (args.size() == 1)
                {
                    int i = 0;
                    QDir dir = QFileInfo(arg).absoluteDir();
                    QList<QFileInfo> files = dir.entryInfoList(QDir::Files, QDir::Name);
                    for (auto it = files.begin(); it != files.end(); ++it)
                    {
                        QString newFile = it->filePath();
                        if (m_Files.contains(newFile) || !IsSupportedFormat(newFile)) continue;

                        m_Files.push_back(newFile);
                        if (newFile == QFileInfo(arg).absoluteFilePath())
                        {
                            m_FileIndex = i;
                        }

                        ++i;
                    }
                }
                else
                {
                    QString newFile = QFileInfo(arg).absoluteFilePath();
                    if (m_Files.contains(newFile) || !IsSupportedFormat(newFile)) continue;

                    m_Files.push_back(newFile);
                }
            }
            else if (QFileInfo(arg).isDir())
            {
                QStringList dirFiles;
                QDirIterator it(arg, QDir::Files, QDirIterator::Subdirectories);
                while (it.hasNext())
                {
                    it.next();
                    QString newFile = it.filePath();
                    if (m_Files.contains(newFile) || dirFiles.contains(newFile) || !IsSupportedFormat(newFile)) continue;

                    dirFiles.push_back(newFile);
                }

                dirFiles.sort();
                m_Files += dirFiles;
            }
        }
    }

    m_PreloadThread = QSharedPointer<PreloadThread>(new PreloadThread(nullptr, this));
    m_PreloadThread->start();
}

MainWindow::~MainWindow()
{
    m_Running = false;
    {
        QMutexLocker mutexLocker(&m_PreloadsMutex);
        m_Preloads.clear();
        m_PreloadsWait.wakeAll();
    }
    m_PreloadThread->wait();

    delete m_Ui;
}

MainWindow* MainWindow::getInstance()
{
    static MainWindow* instance = new MainWindow();
    return instance;
}

void MainWindow::showEvent(QShowEvent* p_Event)
{
    QWidget::showEvent(p_Event);

    if (m_Files.empty()) return;

    RefreshImage();
    PlanPreload();
}

void MainWindow::resizeEvent(QResizeEvent* p_Event)
{
    QWidget::resizeEvent(p_Event);

    if (m_Files.empty()) return;

    RefreshImage(false);
}

bool MainWindow::event(QEvent* p_Event)
{
    if (p_Event && (p_Event->type() == QEvent::KeyPress))
    {
        QKeyEvent* keyEvent = dynamic_cast<QKeyEvent*>(p_Event);
        if (keyEvent)
        {
            switch (keyEvent->key())
            {
            case Qt::Key_Space:
            case Qt::Key_Right:
                NextImage();
                return true;
                break;

            case Qt::Key_Backspace:
            case Qt::Key_Left:
                PrevImage();
                return true;
                break;

            case Qt::Key_Q:
            case Qt::Key_Escape:
                Exit();
                return true;
                break;

            case Qt::Key_F:
            case Qt::Key_F11:
                ToggleFullScreen();
                return true;
                break;

            case Qt::Key_R:
                if (keyEvent->modifiers() & Qt::ShiftModifier)
                {
                    RotateImage("-90");
                }
                else
                {
                    RotateImage("90");
                }
                return true;
                break;

            case Qt::Key_D:
            case Qt::Key_Delete:
                DeleteImage();
                return true;
                break;

            case Qt::Key_M:
                MoveImage();
                return true;
                break;

            case Qt::Key_C:
                CopyImage();
                return true;
                break;

            default:
                break;
            }
        }
    }

    return QMainWindow::event(p_Event);
}

void MainWindow::NextImage()
{
    if (m_Files.empty()) return;

    ++m_FileIndex;
    if (m_FileIndex >= m_Files.size())
    {
        m_FileIndex = 0;
    }

    RefreshImage();
    PlanPreload();
}

void MainWindow::PrevImage()
{
    if (m_Files.empty()) return;

    --m_FileIndex;
    if (m_FileIndex < 0)
    {
        m_FileIndex = m_Files.size() - 1;
    }

    RefreshImage();
    PlanPreload();
}

void MainWindow::ToggleFullScreen()
{
    if (windowHandle()->visibility() & QWindow::FullScreen)
    {
        windowHandle()->showNormal();
    }
    else
    {
        windowHandle()->showFullScreen();
    }
}

void MainWindow::Exit()
{
    QApplication::quit();
}

void MainWindow::RotateImage(const QString& p_Angle)
{
    const QString program = "mogrify";
    const QStringList arguments = { "-rotate", p_Angle, m_Files[m_FileIndex] };
    int rv = QProcess::execute(program, arguments);

    if (rv != 0)
    {
      std::cerr << "image rotation failed, ensure 'mogrify' (imagemagick) is installed on the system.\n";
      return;
    }

    QMutexLocker imagesLocker(&m_ImagesMutex);
    m_Images.remove(m_FileIndex);
    imagesLocker.unlock();

    RefreshImage();
}

void MainWindow::DeleteImage()
{
    QMessageBox msgBox(this);
    msgBox.setText("Delete " + m_Files[m_FileIndex]);
    msgBox.setInformativeText("Proceed to delete this file?");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.setEscapeButton(QMessageBox::Cancel);

    if (msgBox.exec() == QMessageBox::Ok)
    {
        bool rv = QFile::remove(m_Files[m_FileIndex]);

        if (!rv)
        {
            std::cerr << "image deletion failed, check file permissions.\n";
            return;
        }

        QMutexLocker imagesLocker(&m_ImagesMutex);
        m_Images.clear();
        imagesLocker.unlock();

        m_Files.removeAt(m_FileIndex);

        if (m_FileIndex >= m_Files.size())
        {
            m_FileIndex = 0;
        }

        RefreshImage();
        PlanPreload();
    }
}

void MainWindow::MoveImage()
{
    QString targetDir = QFileDialog::getExistingDirectory(this, "Move to Directory",
                                                    m_TargetDir,
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!targetDir.isEmpty())
    {
        m_TargetDir = targetDir;

        QString targetPath = m_TargetDir + "/" + QFileInfo(m_Files[m_FileIndex]).fileName();
        bool rv = QFile(m_Files[m_FileIndex]).rename(targetPath);

        if (!rv)
        {
            std::cerr << "image move failed, check file permissions.\n";
            return;
        }

        QMutexLocker imagesLocker(&m_ImagesMutex);
        m_Images.clear();
        imagesLocker.unlock();

        m_Files.removeAt(m_FileIndex);

        if (m_FileIndex >= m_Files.size())
        {
            m_FileIndex = 0;
        }

        RefreshImage();
        PlanPreload();
    }
}

void MainWindow::CopyImage()
{
    QString targetDir = QFileDialog::getExistingDirectory(this, "Copy to Directory",
                                                    m_TargetDir,
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!targetDir.isEmpty())
    {
        m_TargetDir = targetDir;

        QString targetPath = m_TargetDir + "/" + QFileInfo(m_Files[m_FileIndex]).fileName();
        bool rv = QFile(m_Files[m_FileIndex]).copy(targetPath);

        if (!rv)
        {
            std::cerr << "image copy failed, check file permissions.\n";
            return;
        }
    }
}

void MainWindow::RefreshImage(bool p_ResizeWindow /*= true*/)
{
    if (m_Files.size() <= m_FileIndex) return;

    QSharedPointer<QPixmap> pixmap = LoadImage();
    const float screenUsage = (windowHandle()->visibility() & QWindow::FullScreen) ? 1.0 : 0.7;
    QSize size = QGuiApplication::primaryScreen()->availableGeometry().size() * screenUsage;

#if QT_VERSION >= 0x050600
    qreal dpr = devicePixelRatioF();
#else
    qreal dpr = devicePixelRatio();
#endif

    size.setWidth(size.width() * dpr);
    size.setHeight(size.height() * dpr);

    if ((pixmap->size().width() < size.width()) && (pixmap->size().height() < size.height()))
    {
        size = pixmap->size();
    }

    m_Ui->label->setMinimumSize(1, 1);
    m_Ui->label->setScaledContents(false);

    const QPixmap& scaledPixmap = pixmap->scaled(size, Qt::KeepAspectRatio);
    m_Ui->label->setPixmap(scaledPixmap);

    const int scalePercentage = (100 * scaledPixmap.size().width()) / pixmap->size().width();
    setWindowTitle("nview - " + QFileInfo(m_Files[m_FileIndex]).fileName() + " - " +
                   QString::number(pixmap->size().width()) + "x" + QString::number(pixmap->size().height()) + " - " +
                   QString::number(scalePercentage) + "%");

    if ((p_ResizeWindow) && !(windowHandle()->visibility() & QWindow::FullScreen))
    {
        resize(scaledPixmap.size() / dpr);
    }
    else
    {
        m_Ui->label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    }
}

QSharedPointer<QImage> MainWindow::LoadQImage(const QString& p_Path)
{
    QSharedPointer<QImage> image = QSharedPointer<QImage>(new QImage());
    QImageReader imageReader(p_Path);
    imageReader.setAutoTransform(true);
    imageReader.read(image.data());
    return image;
}

QSharedPointer<QPixmap> MainWindow::LoadImage()
{
    QMutexLocker imagesLocker(&m_ImagesMutex);
    if (!m_Images.contains(m_FileIndex))
    {
        m_Images[m_FileIndex] = LoadQImage(m_Files.at(m_FileIndex));
    }

    QSharedPointer<QPixmap> pixmap = QSharedPointer<QPixmap>(new QPixmap());
    pixmap->convertFromImage(*m_Images[m_FileIndex]);

#if QT_VERSION >= 0x050600
    qreal dpr = devicePixelRatioF();
#else
    qreal dpr = devicePixelRatio();
#endif

    pixmap->setDevicePixelRatio(dpr);

    return pixmap;
}

void MainWindow::PlanPreload()
{
    QList<int> preloads;
    QMutexLocker imagesLocker(&m_ImagesMutex);
    const int lookAhead = 3;
    for (int i = 0; i < lookAhead; ++i)
    {
        const int offset = i + 1;
        const int nextIndex = (m_Files.size() + m_FileIndex + offset) % m_Files.size();
        const int prevIndex = (m_Files.size() + m_FileIndex - offset) % m_Files.size();

        if ((nextIndex >= 0) && (nextIndex < m_Files.size()) &&
            !m_Images.contains(nextIndex) && !preloads.contains(nextIndex))
        {
            preloads.push_back(nextIndex);
        }

        if ((prevIndex >= 0) && (prevIndex < m_Files.size()) &&
            !m_Images.contains(prevIndex) && !preloads.contains(prevIndex))
        {
            preloads.push_back(prevIndex);
        }
    }

    const int maxPreloaded = 100;
    while (m_Images.size() > maxPreloaded)
    {
        const int oppositeCurrent = (m_FileIndex + (m_Files.size() / 2)) % m_Files.size();
        for (int i = 0; i < m_Files.size(); ++i)
        {
            if (m_Images.contains((oppositeCurrent + i) % m_Files.size()))
            {
                m_Images.remove((oppositeCurrent + i) % m_Files.size());
                break;
            }
            else if (m_Images.contains((oppositeCurrent + m_Files.size() - i) % m_Files.size()))
            {
                m_Images.remove((oppositeCurrent + m_Files.size() - i) % m_Files.size());
                break;
            }
        }
    }

    imagesLocker.unlock();

    QMutexLocker preloadsLocker(&m_PreloadsMutex);
    m_Preloads = preloads;
    if (!m_Preloads.empty())
    {
        m_PreloadsWait.wakeAll();
    }
}

void MainWindow::PreloadProcess()
{
    while (m_Running)
    {
        m_PreloadsMutex.lock();
        while (m_Preloads.empty() && m_Running)
        {
            m_PreloadsWait.wait(&m_PreloadsMutex);
        }

        while (!m_Preloads.empty() && m_Running)
        {
            int index = m_Preloads.takeFirst();
            m_PreloadsMutex.unlock();

            {
              QMutexLocker imagesLocker(&m_ImagesMutex);
              if (!m_Images.contains(index))
              {
                m_Images[index] = LoadQImage(m_Files.at(index));
              }
            }

            m_PreloadsMutex.lock();
        }

        m_PreloadsMutex.unlock();
    }
}

bool MainWindow::IsSupportedFormat(const QString& p_Path)
{
    if (QFileInfo(p_Path).isFile())
    {
        const QMimeDatabase mimeDatabase;
        const QString mimeType(mimeDatabase.mimeTypeForFile(p_Path).name());
        if (mimeType.startsWith("image/"))
        {
            const QString mimeImageType(mimeType.mid(6));
            if (QImageReader::supportedImageFormats().indexOf(mimeImageType.toUtf8()) != -1)
            {
                return true;
            }
        }
    }

    return false;
}

/*static*/ void MainWindow::MsgDialog(const QString& p_Msg)
{
    QMessageBox msgBox;
    msgBox.setText(p_Msg);
    msgBox.exec();
}
