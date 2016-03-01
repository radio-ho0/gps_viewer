#include "gpsclass.h"
#include <QDebug>
#include <QTimer>
#include <QApplication>

GPSClass::GPSClass(QObject *parent) : QObject(parent)
{
    m_alti = 0;
    m_long = 0;
    m_lati = 0;
    m_type = InvalidCoordinate;
//    receiver = NULL;
//    if(parent != NULL){
//        receiver = parent;
//    }

    connect(this, &GPSClass::TypeChange, [this](CoordinateType type){
        switch(type){
        case Coordinate2D:
            emit Coord2D();
            qDebug() << "~~~ 2D" << endl;
            break;

        case Coordinate3D:

            emit Coord3D();
            gpsTimer->stop();
            qDebug() << "~~~ 3D" << endl;
            break;

        default:
            break;
        }
    });

}

GPSClass::~GPSClass()
{

}

void GPSClass::putStr(QByteArray &str)
{
    bool isHaRMC = str.contains("RMC");
    if( isHaRMC ){
            convertGPRMC(str);
    }

    bool isHaGGA = str.contains("GGA");
    if( isHaGGA ){
            convertGPGGA(str);
    }

    bool isHaGSA = str.contains("GSA");
    if( isHaGSA ){
            convertGPGSA(str);
    }
}

GPSClass::CoordinateType GPSClass::type() const
{
    return m_type;
}

double GPSClass::altitude() const
{
    return m_alti;
}

double GPSClass::latitude() const
{
    return m_lati;
}

double GPSClass::longitude() const
{
    return m_long;
}

bool GPSClass::isCoordinated()
{
    return m_isCoordinated;

}

void GPSClass::setReceiver(QObject *obj)
{
    receiver = obj;

}

void GPSClass::convertGPRMC(QByteArray RMCSentence)
{

    QByteArray resArray = RMCSentence;
    resArray.remove(0,resArray.indexOf("RMC"));//去掉$GPRMC之前的暂时不用的信息
    //resArray.remove(0,resArray.indexOf("$GPRMC"));//去掉$GPRMC之前的暂时不用的信息
    if(resArray.contains("*"))
    {
        QByteArray OldList=resArray.left(resArray.indexOf("*"));//获得除了校验值之外的完整的RMC语句
        QList<QByteArray> NewList=OldList.split(',');//分割提取每个语句段
        // 判断数据有效
        if( QString(NewList[GPRMC_STATUS]) == QString("A")){
            NewList[GPRMC_LATITUDE].toFloat();    //显示纬度信息
            NewList[GPRMC_LONGITUDE].toFloat();   //显示经度度信息

        }else{
            qDebug("bad GPRMC  ");
        }
//        foreach(QByteArray content, NewList){
//            qDebug() << "GPRMC:: " <<  content << endl;
//        }
    }

}

void GPSClass::convertGPGGA(QByteArray GGASentence)
{
    QByteArray resArray = GGASentence;
    resArray.remove(0,resArray.indexOf("GGA"));//去掉$GPGGA之前的暂时不用的信息
    if(resArray.contains("*"))
    {
        QByteArray OldList=resArray.left(resArray.indexOf("*"));//获得除了校验值之外的完整的GGA语句
        QList<QByteArray> NewList=OldList.split(',');//分割提取每个语句段
        // 判断数据有效
        if( QString(NewList[GPGGA_QUALITY]) != QString("0")){

            m_lati = NewList[GPGGA_LATITUDE].toDouble();
            m_long = NewList[GPGGA_LONGITUDE].toDouble();
            m_alti = NewList[GPGGA_MSL].toDouble();


        }else{
           qDebug() << "Bad GGA" << endl;
        }
    }

}


void GPSClass::convertGPGSA(QByteArray resArray)
{
//    QByteArray resArray = GSASentence;
    resArray.remove(0,resArray.indexOf("GSA"));//去掉$GPGSA之前的暂时不用的信息
    if(resArray.contains("*"))
    {
        QByteArray OldList=resArray.left(resArray.indexOf("*"));//获得除了校验值之外的完整的GSA语句
        QList<QByteArray> NewList=OldList.split(',');//分割提取每个语句段
        // 判断数据有效
        if( QString(NewList[GPGSA_QUALITY]) != QString("1")){
            bool isOK;
            int quality = NewList[GPGSA_QUALITY].toInt(&isOK);
            if(isOK){
                switch(quality){

                case Coordinate2D:
                    m_type =Coordinate2D;
                    emit TypeChange(Coordinate2D);
                    break;

                case Coordinate3D:
                    m_type = Coordinate3D;
                    emit TypeChange(Coordinate3D);
                    break;

                default:
                    m_type = InvalidCoordinate;
                    break;
                };
            }
        }else{
            qDebug() << "Bad GSA" << endl;
        }
    }
}

