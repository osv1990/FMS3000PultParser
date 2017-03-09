#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QDebug"
#include "QtSerialPort/QSerialPortInfo"
#include "QtSerialPort/QSerialPort"
#include "QTimer"
#include "QThread"

QSerialPort *sport;
QTimer *stimer;
unsigned char ruc=0;
QByteArray  m_readData,rData;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->pushButton_2->setEnabled(false);
    foreach (const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts())
    {
        if(serialPortInfo.portName() != "COM30")
        ui->comboBox->addItem(serialPortInfo.portName());
    }

    rData.resize(11);
    stimer = new QTimer();
    stimer->setInterval(2);
    connect(stimer, SIGNAL(timeout()),this,SLOT(readData()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::request()
{
    m_readData.append(sport->readAll());
}

unsigned char encoder(unsigned char val)
{
    const unsigned char focus[255] = {199, 197, 193, 192, 128, 160, 161, 225, 97, 101, 109, 111, 239, 207, 206, 142, 143, 139, 131, 129, 1, 65, 67, 85, 195, 194, 202, 218, 222, 223, 159, 157, 29, 31, 23, 7, 3, 2, 130, 134, 135, 133, 149, 181, 189, 191, 63, 59, 58, 62, 46, 14, 6, 4, 5, 13, 15, 11, 43, 107, 123, 127, 126, 118, 116, 124, 92, 28, 12, 8, 10, 26, 30, 22, 86, 214, 246, 254, 252, 236, 232, 248, 184, 56, 24, 16, 20, 52, 60, 44, 172, 173, 237, 253, 249, 217, 209, 241, 113, 112, 48, 32, 40, 104, 120, 88, 89, 91, 219, 251, 243, 179, 163, 227, 226, 224, 96, 64, 80, 208, 240, 176, 178, 182, 183, 247, 231, 103, 71};
    unsigned char i;
    for(i=0; i<=127; ++i)
    {
        if(focus[i] == val ) break;
    }
        return i;
}

void MainWindow::readData()
{

    int i=0;
    unsigned char uc;

    while (i != -1)
    {
        i = m_readData.indexOf(0x55,2);
        if(m_readData.size()-i < 11)
        {
            m_readData.truncate(i);
            qDebug() << "truncate";
        }
        else
        {
            rData.clear();
            rData.append(m_readData);
            rData.truncate(11);
            m_readData.lastIndexOf(rData);
            qDebug() <<  i << " " << rData.toHex();
            m_readData.remove(0,i);
            qDebug() << "remove";
        }
        sport->setRequestToSend(false); // выставить RTS
    }
    sport->setRequestToSend(true); // выставить RTS
    uc = m_readData[1]; ui->lcdNumber_1->display(uc);
    uc = m_readData[2]; ui->lcdNumber_2->display(uc);
    uc = m_readData[3]; ui->lcdNumber_3->display(uc);
                        ui->lcdNumber_4->display(127 - encoder(m_readData[1]));
                        ui->lcdNumber_5->display(127 - encoder(m_readData[2]));

}

void MainWindow::on_pushButton_clicked()
{
    sport = new QSerialPort (this);
    sport->setPortName(ui->comboBox->currentText());
    sport->setBaudRate(QSerialPort::Baud19200);
    sport->setDataBits(QSerialPort::Data8);
    sport->setParity(QSerialPort::NoParity);
    sport->setStopBits(QSerialPort::OneStop);
    sport->setFlowControl(QSerialPort::SoftwareControl);
    ui->comboBox->setEnabled(true);
    if (sport->open(QIODevice::ReadWrite))
    {
        qDebug() << "+++++++++++++++++++++++++++++++++++";
        qDebug() << "COMPORT открыт";
        qDebug() << "+++++++++++++++++++++++++++++++++++";
    }
    else
    {
        qDebug() << "===================================";
        qDebug() << "COMPORT закрыт";
        qDebug() << "===================================";
    }
    ui->comboBox->setEnabled(false);
    ui->pushButton->setEnabled(false);
    ui->pushButton_2->setEnabled(true);
    sport->setFlowControl(QSerialPort::NoFlowControl);
    sport->setSettingsRestoredOnClose(false);
    sport->setRequestToSend(true); // выставить RTS
    connect(sport, &QSerialPort::readyRead, this, &MainWindow::request);
    stimer->start(5);
}

void MainWindow::on_pushButton_2_clicked()
{
    stimer->stop();
    sport->close();
    ui->comboBox->setEnabled(true);
    ui->pushButton->setEnabled(true);
    ui->pushButton_2->setEnabled(false);
    qDebug() << "-----------------------------------";
    qDebug() << "COMPORT закрыт";
    qDebug() << "-----------------------------------";
}
