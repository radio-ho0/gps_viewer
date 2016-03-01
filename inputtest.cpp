#include "inputtest.h"
#include "ui_inputtest.h"
#include <QDebug>
#include <QDateTime>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QMessageBox>
#include <QScrollBar>
#include <QIcon>
#include "gpsclass.h"


// From QT5 Terminal example

static const char blankString[] = QT_TRANSLATE_NOOP("input Test", "N/A");

InputTest::InputTest(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InputTest)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/qt.png"));;
    QPalette p = palette();
    p.setColor(QPalette::Base, Qt::black);
    p.setColor(QPalette::Text, Qt::green);
    ui->txtInput->setPalette(p);
    //setPalette(p);
    serial = new QSerialPort();
    intValidator = new QIntValidator(0, 4000000, this);
    ui->txtInput->setFocus();
    connect(ui->btStart, SIGNAL(clicked(bool)), this, SLOT(slOk()));
    fillPortsParameters();
    fillPortsInfo();
    connect(ui->baudRateBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(checkCustomBaudRatePolicy(int)));
    connect(ui->serialPortInfoListBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(checkCustomDevicePathPolicy(int)));

    connect(serial, SIGNAL(readyRead()), this, SLOT(readData()));
    connect(ui->btExit, SIGNAL(clicked(bool)), this, SLOT(slExit()));
    setWindowTitle(tr("GPS test"));

    myGPS = new GPSClass(this);

    connect(myGPS, SIGNAL(Coord3D()), this, SLOT(slGot3d()));
}

InputTest::~InputTest()
{
    if(serial->isOpen()){
        serial->close();
    }
    delete ui;

}

void InputTest::customEvent(QEvent *e)
{

//    if(e->type() == (QEvent::Type)GPSClass::COOR3D_EVENT){
//        QMessageBox* box = new QMessageBox();
//              box->setWindowTitle(QString("COOR3D_EVENT"));
//              box->setText(QString("COOR3D altitude:%1 latiude:%2 longitude:%3 ")
//                           .arg(myGPS->altitude()).arg(myGPS->latitude()).arg(myGPS->longitude()));
//              box->show();
//    }
    QDialog::customEvent(e);
}

void InputTest::on_btClear_clicked()
{
    ui->txtInput->clear();
    ui->leLati->clear();
    ui->leLong->clear();
    ui->leHDOP->clear();
    ui->leVDOP->clear();
    ui->lePDOP->clear();
    cleanStatus();
}

void InputTest::slOk()
{
    clearother();
    // 判断输入的 gps 语句
    QString gpsSent = ui->txtInput->toPlainText();



    bool isHaRMC = gpsSent.contains("RMC");
    if( isHaRMC ){
            convertGPRMC(gpsSent.toUtf8());
    }

    bool isHaGGA = gpsSent.contains("GGA");
    if( isHaGGA ){
            convertGPGGA(gpsSent.toUtf8());
    }

    bool isHaGSA = gpsSent.contains("GSA");
    if( isHaGSA ){
            convertGPGSA(gpsSent.toUtf8());
    }
    // test GpsClass
//    QByteArray array = gpsSent.toUtf8();
//    myGPS->putStr(array);

}

void InputTest::cleanStatus()
{

    ui->lbStatus->clear();
}

void InputTest::clearother()
{

    ui->leLati->clear();
    ui->leLong->clear();
    ui->leHDOP->clear();
    ui->leVDOP->clear();
    ui->lePDOP->clear();
    ui->lbStatus->clear();
}

void InputTest::readData()
{
//    ui->txtInput->clear();
    QByteArray data = serial->readAll();
    ui->txtInput->insertPlainText(data);

    myGPS->putStr(data);

    // 一直跟随最新尾部
    QScrollBar *bar = ui->txtInput->verticalScrollBar();
    bar->setValue(bar->maximum());

}

void InputTest::convertGPRMC(QByteArray RMCSentence)
{
    QString currDate = QDateTime::currentDateTime().toString("yyyy-mm-dd hh:mm:ss dddd \n");
    QByteArray resArray = RMCSentence;
    resArray.remove(0,resArray.indexOf("RMC"));//去掉$GPRMC之前的暂时不用的信息
    //resArray.remove(0,resArray.indexOf("$GPRMC"));//去掉$GPRMC之前的暂时不用的信息
    if(resArray.contains("*"))
    {
        QByteArray OldList=resArray.left(resArray.indexOf("*"));//获得除了校验值之外的完整的RMC语句
        QList<QByteArray> NewList=OldList.split(',');//分割提取每个语句段
        // 判断数据有效
        if( QString(NewList[GPRMC_STATUS]) == QLatin1String("A")){
            ui->leLati->setText(QString::number(NewList[GPRMC_LATITUDE].toFloat()));    //显示纬度信息
            ui->leLong->setText(QString::number(NewList[GPRMC_LONGITUDE].toFloat()));   //显示经度度信息

            ui->lbStatus->setText(tr("good GPRMC!!  ") + currDate);
        }else{
            ui->lbStatus->setText(tr("bad GPRMC  ") + currDate);
        }
        foreach(QByteArray content, NewList){
            qDebug() << "GPRMC:: " <<  content << endl;
        }
    }
}

void InputTest::convertGPGGA(QByteArray GGASentence)
{
    QString currDate = QDateTime::currentDateTime().toString("yyyy-mm-dd hh:mm:ss dddd \n");
    QByteArray resArray = GGASentence;
    resArray.remove(0,resArray.indexOf("GGA"));//去掉$GPGGA之前的暂时不用的信息
    if(resArray.contains("*"))
    {
        QByteArray OldList=resArray.left(resArray.indexOf("*"));//获得除了校验值之外的完整的GGA语句
        QList<QByteArray> NewList=OldList.split(',');//分割提取每个语句段
        // 判断数据有效
        if( QString(NewList[GPGGA_QUALITY]) != QString("0")){
            ui->leLati->setText(QString::number(NewList[GPGGA_LATITUDE].toDouble()));    //显示纬度信息
            ui->leLong->setText(QString::number(NewList[GPGGA_LONGITUDE].toDouble()));   //显示经度度信息

            ui->lbStatus->setText(tr("good GPGGA!!  ") + currDate);
        }else{
            ui->lbStatus->setText(tr("bad GPGGA  ") + currDate);
        }
        foreach(QByteArray content, NewList){
            qDebug() << "GPGGA:: " <<  content << endl;
        }
    }
}

void InputTest::convertGPGSA(QByteArray GSASentence)
{


    QString currDate = QDateTime::currentDateTime().toString("yyyy-mm-dd hh:mm:ss dddd \n");
    QByteArray resArray = GSASentence;
    resArray.remove(0,resArray.indexOf("GSA"));//去掉$GPGSA之前的暂时不用的信息
    if(resArray.contains("*"))
    {
        QByteArray OldList=resArray.left(resArray.indexOf("*"));//获得除了校验值之外的完整的GSA语句
        QList<QByteArray> NewList=OldList.split(',');//分割提取每个语句段
        // 判断数据有效
        if( QString(NewList[GPGSA_QUALITY]) != QString("1")){
            ui->lePDOP->setText(QString::number(NewList[GPGSA_PDOP].toFloat()));    // 显示位置精度信息
            ui->leHDOP->setText(QString::number(NewList[GPGSA_HDOP].toFloat()));    // 显示水平精度信息
            ui->leVDOP->setText(QString::number(NewList[GPGSA_VDOP].toFloat()));    // 显示垂直精度信息
            ui->lbStatus->setText(tr("good GPGSA!!  ") + currDate);
        }else{
            ui->lbStatus->setText(tr("bad GPGSA  ") + currDate);
        }
        foreach(QByteArray content, NewList){
            qDebug() << "GPGSA:: " <<  content << endl;
        }
    }

}

void InputTest::fillPortsParameters()
{
    ui->baudRateBox->addItem(QStringLiteral("9600"),   QSerialPort::Baud9600);
    ui->baudRateBox->addItem(QStringLiteral("19200"),  QSerialPort::Baud19200);
    ui->baudRateBox->addItem(QStringLiteral("38400"),  QSerialPort::Baud38400);
    ui->baudRateBox->addItem(QStringLiteral("115200"), QSerialPort::Baud115200);
    ui->baudRateBox->addItem(tr("Custom"));
}

void InputTest::fillPortsInfo()
{
    ui->serialPortInfoListBox->clear();
    QString description;
    QString manufacturer;
    QString serialNumber;
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        QStringList list;
        description = info.description();
        manufacturer = info.manufacturer();
        serialNumber = info.serialNumber();
        list << info.portName()
             << (!description.isEmpty() ? description : blankString)
             << (!manufacturer.isEmpty() ? manufacturer : blankString)
             << (!serialNumber.isEmpty() ? serialNumber : blankString)
             << info.systemLocation()
             << (info.vendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : blankString)
             << (info.productIdentifier() ? QString::number(info.productIdentifier(), 16) : blankString);

        ui->serialPortInfoListBox->addItem(list.first(), list);
    }

    ui->serialPortInfoListBox->addItem(tr("Custom"));
}

void InputTest::checkCustomBaudRatePolicy(int idx)
{
    bool isCustomBaudRate = !ui->baudRateBox->itemData(idx).isValid();
    ui->baudRateBox->setEditable(isCustomBaudRate);
    if (isCustomBaudRate) {
        ui->baudRateBox->clearEditText();
        QLineEdit *edit = ui->baudRateBox->lineEdit();
        edit->setValidator(intValidator);
    }
}

void InputTest::checkCustomDevicePathPolicy(int idx)
{
    bool isCustomPath = !ui->serialPortInfoListBox->itemData(idx).isValid();
    ui->serialPortInfoListBox->setEditable(isCustomPath);
    if (isCustomPath)
        ui->serialPortInfoListBox->clearEditText();
}

void InputTest::slExit()
{
    if(serial->isOpen()){
        serial->close();
    }
    this->close();
}

void InputTest::slGot3d()
{
    QMessageBox* box = new QMessageBox();
          box->setWindowTitle(QString("COOR3D_EVENT"));
          box->setText(QString("COOR3D altitude:%1 latiude:%2 longitude:%3 ")
                       .arg(myGPS->altitude()).arg(myGPS->latitude()).arg(myGPS->longitude()));
          box->show();
    qDebug() << "Got 3D coord" << myGPS->altitude() << myGPS ->latitude() <<myGPS->longitude() <<endl;
}


void InputTest::on_btOpenSerial_clicked()
{
    QString serialName = ui->serialPortInfoListBox->currentText();
    serial->setPortName(serialName);
//    QSerialPort::BaudRate Rate = static_cast<QSerialPort::BaudRate>(
//                                    ui->baudRateBox->itemData(ui->baudRateBox->
//                                            currentIndex()).toInt());
    QSerialPort::BaudRate baudRate;
    if (ui->baudRateBox->currentIndex() == 4) {
        baudRate = static_cast<QSerialPort::BaudRate>(ui->baudRateBox->currentText().toInt());
    } else {
        baudRate = static_cast<QSerialPort::BaudRate>(
                    ui->baudRateBox->itemData(ui->baudRateBox->currentIndex()).toInt());
    }
    serial->setBaudRate(baudRate);
    qDebug() << "the selected serial name:: " << serialName << " \n "
             << "The selected baudrate:: " << baudRate << endl;
    if(serial->open(QIODevice::ReadOnly)){
         ui->lbStatus->setText(tr("open success!"));
         ui->btStart->setEnabled(false);
         ui->serialPortInfoListBox->setEnabled(false);
         ui->baudRateBox->setEnabled(false);
         ui->btOpenSerial->setEnabled(false);

    }else{
         QMessageBox::critical(this, tr("Error"), serial->errorString());
    }

}

void InputTest::on_btStopSerial_clicked()
{
    if(serial->isOpen()){
        serial->close();
        ui->btStart->setEnabled(true);
        ui->serialPortInfoListBox->setEnabled(true);
        ui->baudRateBox->setEnabled(true);
        ui->btOpenSerial->setEnabled(true);
    }

}
