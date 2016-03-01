#ifndef INPUTTEST_H
#define INPUTTEST_H

#include <QDialog>
#include <QIntValidator>
#include <QtSerialPort/QSerialPort>

class GPSClass;
namespace Ui {
class InputTest;
}

class InputTest : public QDialog
{
    Q_OBJECT

public:
    explicit InputTest(QWidget *parent = 0);
    ~InputTest();

//    GGA：时间、位置、定位类型
//    GLL：UTC时间、经度、纬度
//    GSA：GPS接收机操作模式、定位使用的卫星、DOP值
//    GSV：可见GPS卫星信息、仰角、方位角、信噪比（SNR）
//    RMC：时间、日期、位置、速度

//    RMC  Recommended Minimum Navigation Information
//                                                               12
//           1         2 3       4 5        6 7   8   9    10  11|
//           |         | |       | |        | |   |   |    |   | |
//    $--RMC,hhmmss.ss,A,llll.ll,a,yyyyy.yy,a,x.x,x.x,xxxx,x.x,a*hh
//     1) Time (UTC)
//     2) Status, V = Navigation receiver warning A=数据有效；V=数据无效
//     3) Latitude
//     4) N or S
//     5) Longitude
//     6) E or W
//     7) Speed over ground, knots
//     8) Track made good, degrees true
//     9) Date,  ddmmyy
//    10) Magnetic Variation, degrees
//    11) E or W
//    12) Checksum
    enum RMC_PROPERTY_OFFSET{
        GPRMC_HEADER,    // "$GPRMC
        GPRMC_UTC,
        GPRMC_STATUS,    // 状态   A=数据有效；V=数据无效
        GPRMC_LATITUDE,  // 纬度
        GPRMC_N_S,
        GPRMC_LONGITUDE, // 经度
        GPRMC_E_W,
        GPRMC_SPEED,
        GPRMC_DEGREES,
        GPRMC_DATE,
        GPRMC_MAGNETIC,
        GPRMC_E_W2,
        GPRMC_CHECKSUM
    };

//    GGA Global Positioning System Fix Data. Time, Position and fix related data
//    for a GPS receiver
//						           11
//            1 	2      3   4      5 6 7  8    9 10 |   12 13 14 15
//            | 	|      |   |      | | |  |    |  | |   |  |   | |
//    $--GGA,hhmmss.ss,llll.ll,a,yyyyy.yy,a,x,xx,x.x,x.x,M,x.x,M,x.x,xxxx*hh
//    1) Time (UTC)
//    2) Latitude
//    3) N or S (North or South)
//    4) Longitude
//    5) E or W (East or West)
//    6) GPS Quality Indicator,
// 	   0 - fix not available,
// 	   1 - GPS fix,
// 	   2 - Differential GPS fix
//    7) Number of satellites in view, 00 - 12
//    8) Horizontal Dilution of precision
//    9) Antenna Altitude above/below mean-sea-level (geoid)
//    10) Units of antenna altitude, meters
//    11) Geoidal separation, the difference between the WGS-84 earth
//    		ellipsoid and mean-sea-level (geoid), "-" means mean-sea-level below ellipsoid
//    12) Units of geoidal separation, meters
//    13) Age of differential GPS data, time in seconds since last SC104
//    type 1 or 9 update, null field when DGPS is not used
//    14) Differential reference station ID, 0000-1023
//    15) Checksum
    enum GGA_PROPERTY_OFFSET{
        GPGGA_HEADER,  // "$GPGGA"
        GPGGA_UTC,
        GPGGA_LATITUDE,         // 纬度
        GPGGA_N_S,
        GPGGA_LONGITUDE,        // 经度
        GPGGA_E_W,
        GPGGA_QUALITY,   // 0:未定位   1:SPS 模式，定位有效   2:差分，SPS 模式，定位有效   3:PPS 模式，定位有效
        GPGGA_NUM_STATELLITES,   // $OUTPUT::   00 - 12
        GPGGA_HDOP,
        GPGGA_MSL,              // 海拔
        GPGGA_ALTITUDE_UNIT, 	// METERS
        GPGGA_GEOID_SEPARATION,
        GPGGA_SPARATION_UNIT,
        GPGGA_AGE,
        GPGGA_CHECKSUM
    };


//    GSA GPS DOP and active satellites
//           1 2 3                        14 15 16   17  18
//           | | |                         |  |  |   |   |
//    $--GSA,a,a,x,x,x,x,x,x,x,x,x,x,x,x,x,x,x.x,x.x,x.x*hh
//    1) Selection mode
//    2) Mode
//    3) ID of 1st satellite used for fix
//    4) ID of 2nd satellite used for fix
//    ...
//    14) ID of 12th satellite used for fix
//    15) PDOP in meters
//    16) HDOP in meters
//    17) VDOP in meters
//    18) Checksum
    enum GSA_PROPERTY_OFFSET{
        GPGSA_HEADER,         // "$GPGSA"
        GPGSA_MODE,           //  M=手劢（强制操作在 2D 或 3D 模式），A=自劢
        GPGSA_QUALITY,        //  1:定位无效 2:2D 定位 3:3D 定位
        GPGSA_1_STATELLITE,   // 通道 1
        GPGSA_PDOP = 15,           // 位置精度
        GPGSA_HDOP = 16,           // 水平精度
        GPGSA_VDOP = 17,           // 垂直精度
        GPGSA_CHECKSUM = 18       // 校验和

    };

    void customEvent(QEvent *e);

private slots:
    void on_btClear_clicked();
    void slOk();
    void cleanStatus();
    void clearother();
    void readData();
    void on_btOpenSerial_clicked();

    void on_btStopSerial_clicked();
    void checkCustomBaudRatePolicy(int idx);
    void checkCustomDevicePathPolicy(int idx);
    void slExit();

    void slGot3d();

private:
    Ui::InputTest *ui;
    QString SerialName;
    QString baudRate;
    QSerialPort* serial;

    QIntValidator *intValidator;
    void convertGPRMC(QByteArray RMCSentence);
    void convertGPGGA(QByteArray GGASentence);
    void convertGPGSA(QByteArray GSASentence);
    void fillPortsParameters();
    void fillPortsInfo();
    GPSClass* myGPS;

};

#endif // INPUTTEST_H
