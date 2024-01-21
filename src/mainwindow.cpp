#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtGlobal>
#include <QFileDialog>
#include <QScrollBar>
#include <QMessageBox>
#include <QDesktopWidget>
#include <QDebug>

//#define     USE_FIRMWARE_SELECT_DIALOG

const QString command_to_execute = "/usr/bin/install_yocto-ms.sh";
const char * command_to_execute0 = "weston-touch-calibrator  -v /sys/devices/platform/bus@5b000000/5b0d0000.usb/ci_hdrc.0/usb1/1-1/1-1.2/1-1.2:1.0/0003:222A:0001.0001/input/input2/event2";
const QStringList arguments = {}; //{"-a", "-b"};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->installBtn, &QPushButton::clicked, this, &MainWindow::onInstallBtn);

    m_installProcess = new QProcess();
    connect(m_installProcess, &QProcess::readyReadStandardOutput, this, &MainWindow::readyReadConnmanInteractive);
    connect(m_installProcess, &QProcess::readyReadStandardError, this, &MainWindow::readyReadStandardError);
    connect(m_installProcess, &QProcess::errorOccurred, this, &MainWindow::onErrorOccurred);
    //connect(m_installProcess, &QProcess::finished, this, &MainWindow::finished);
    connect(m_installProcess, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),this, &MainWindow::finished);

    ui->outputTextEdit->setReadOnly(true);
    ui->outputTextEdit->setHtml("");
    ui->outputTextEdit->verticalScrollBar()->setStyleSheet("QScrollBar:vertical { width: 25px; }");

    QPixmap pix(":/images/msd_logo_320.png");
    ui->logoLabel->setPixmap(pix);

    system(command_to_execute0);

}
MainWindow::~MainWindow()
{
    disconnect(m_installProcess, &QProcess::readyReadStandardOutput, this, &MainWindow::readyReadConnmanInteractive);
    disconnect(m_installProcess, &QProcess::readyReadStandardError, this, &MainWindow::readyReadStandardError);
    disconnect(m_installProcess, &QProcess::errorOccurred, this, &MainWindow::onErrorOccurred);

    if( m_installProcess->state() != QProcess::NotRunning){
        m_installProcess->kill();
    }
    delete m_installProcess;

    delete ui;
}
void MainWindow::onInstallBtn()
{
    ui->installBtn->setEnabled(false);

    QString output = "<font color=blue>" + command_to_execute;
    for(int i = 0; i < arguments.size(); i++){
        output = output + " " + arguments.at(i);
    }
    output = output + "</font>";

    ui->outputTextEdit->append(output);

    m_installProcess->start(command_to_execute, arguments);
    //m_installProcess->start("ping -c 10 www.ti.com");
}

QString MainWindow::replaceEChoCodes(QString inStr)
{
    //qDebug() << inStr;
    QString str = inStr;
    str.replace("\e[34m\e[4m\e[1m", "<font color=blue>");
    //qDebug() << str;
    str.replace("\e[34m\e[1m", "<font color=blue>");
    //qDebug() << str;
    str.replace("\e[31m\e[1m", "<font color=red>");
    //qDebug() << str;
    str.replace("\e[0m", "</font>");
    //qDebug() << str;
    str.remove("\a");
    str.remove("\b");
    //str.remove("\c");
    str.remove("\e");

    return str;
}

void MainWindow::onErrorOccurred(QProcess::ProcessError error)
{
    qDebug() << "MainWindow::onErrorOccurred()";
    QString strError;
    switch(error){
    case QProcess::FailedToStart:
        strError = "FailedToStart";
        break;
    case QProcess::Crashed:
        strError = "Crashed";
        break;
    case QProcess::Timedout:
        strError = "Timedout";
        break;
    case QProcess::WriteError:
        strError = "WriteError";
        break;
    case QProcess::ReadError:
        strError = "ReadError";
        break;
    case QProcess::UnknownError:
        strError = "UnknownError";
        break;
    default:
        strError = "UnknownError";
        break;
    }

    ui->outputTextEdit->append("<font color=red>Process Error: \"" + strError + "\"</font>");

    ui->outputTextEdit->append("<font color=red>\"" + m_installProcess->errorString() + "\"</font>");

    finished(error, QProcess::CrashExit);

/*
    QByteArray byteArray = m_installProcess->readAllStandardOutput();
    if(byteArray.size() > 0){
        QStringList strLines = QString(byteArray).split("\n");
        foreach (QString line, strLines){
            ui->outputTextEdit->append("<font color=red>" + replaceEChoCodes(line) + "</font>");
        }
    }
    byteArray = m_installProcess->readAllStandardError();
    if(byteArray.size() > 0){
        QStringList strLines = QString(byteArray).split("\n");
        foreach (QString line, strLines){
            ui->outputTextEdit->append("<font color=red>" + replaceEChoCodes(line) + "</font>");
        }
    }
*/

}
void MainWindow::readyReadConnmanInteractive()
{
    //qDebug() << "MainWindow::readyReadConnmanInteractive()";
    if(m_installProcess != nullptr){
        QByteArray byteArray = m_installProcess->readAllStandardOutput();
        if((byteArray.size() == 1) && (byteArray.data()[0] == '.')){
            ui->outputTextEdit->insertPlainText(byteArray.data());
        }else{
            QStringList strLines = QString(byteArray).split("\n");

            foreach (QString line, strLines){
                if(line.contains("statistics")){
                    ui->outputTextEdit->append("<font color=blue>" + replaceEChoCodes(line) + "</font>");
                }else if(line.contains("FW Version:")){
                    ui->outputTextEdit->append("<font color=blue>" + replaceEChoCodes(line) + "</font>");
                }else{
                    ui->outputTextEdit->append("<font color=black>" + replaceEChoCodes(line) + "</font>");
                }
            }
        }
    }
}
void MainWindow::readyReadStandardError()
{
    qDebug() << "MainWindow::readyReadStandardError()";
    if(m_installProcess != nullptr){
        QByteArray byteArray = m_installProcess->readAllStandardError();
        QStringList strLines = QString(byteArray).split("\n");

        foreach (QString line, strLines){
            ui->outputTextEdit->append("<font color=red>" + replaceEChoCodes(line) + "</font>");
        }
    }
}

void MainWindow::finished(int /*exitCode*/, QProcess::ExitStatus /*exitStatus*/)
{
    ui->installBtn->setEnabled(true);

}




