#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QDebug"
#include "QtSerialPort/QSerialPortInfo"
#include "QtSerialPort/QSerialPort"
#include "QTimer"
#include "QThread"

QSerialPort *sport;
QTimer *stimer;
const unsigned char focus[255] = {127, 63, 62, 58, 56, 184, 152, 24, 8, 72, 73, 77, 79, 15, 47, 175, 191, 159, 31, 29, 28, 92, 76, 12, 4, 36, 164, 166, 167, 135, 151, 215, 223, 207, 143, 142, 14, 46, 38, 6, 2, 18, 82, 83, 211, 195, 203, 235, 239, 231, 199, 71, 7, 23, 19, 3, 1, 9, 41, 169, 233, 225, 229, 245, 247, 243, 227, 163, 131, 139, 137, 129, 128, 132, 148, 212, 244, 240, 242, 250, 251, 249, 241, 209, 193, 197, 196, 192, 64, 66, 74, 106, 122, 120, 121, 125, 253, 252, 248, 232, 224, 226, 98, 96, 32, 33, 37, 53, 61, 60, 188, 190, 254, 126, 124, 116, 112, 113, 49, 48, 16, 144, 146, 154, 158, 30, 94, 95};
QByteArray  m_readData,rData;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->pushButton_2->setEnabled(false);
    foreach (const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts())
    {
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
    //    qDebug() << "************************************";
    m_readData.append(sport->readAll());

}



void MainWindow::readData()
{

    int i=0;

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
            //if()
            m_readData.remove(0,i);
            qDebug() << "remove";
        }
        sport->setRequestToSend(false); // выставить RTS
    }
    sport->setRequestToSend(true); // выставить RTS

    //    qDebug() << "-----------------------------------";
    qDebug() <<  i << " " << m_readData.toHex();
    //    qDebug() << "-----------------------------------";
    ui->lcdNumber_1->display(m_readData.data()[0]);
    ui->lcdNumber_2->display(m_readData.data()[1]);
    ui->lcdNumber_3->display(m_readData.data()[2]);
    ui->lcdNumber_4->display(focus[m_readData.data()[1]]);
    ui->lcdNumber_5->display(focus[m_readData.data()[2]]);
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
    //sport->setReadBufferSize(11);
    //sport->read(cdata,5);
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

void MainWindow::on_checkRequest_toggled(bool checked)
{
    //    if(checked)
    //    {
    //        stimer->start();
    //    }
    //    else
    //    {
    //        stimer->stop();
    //    }
}

void MainWindow::on_lineEdit_textChanged(const QString &arg1)
{
    //stimer->start(arg1.toInt());
}
