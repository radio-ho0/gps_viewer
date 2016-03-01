#ifndef GPSCLASS_H
#define GPSCLASS_H

#include <QObject>

#include <QEvent>

class QTimer;
class QEvent;
class GPSClass : public QObject
{
    Q_OBJECT

public:
    explicit GPSClass(QObject *parent = 0);
    ~GPSClass();

    // 处理 RMS GSA GGA
    void putStr(QByteArray& str);

    enum CoordinateType {
        InvalidCoordinate = 1,
        Coordinate2D,
        Coordinate3D
    };
    enum _COOR3D_EVENT{
        COOR3D_EVENT = QEvent::User + 1521,
    };

    CoordinateType type() const;   // 获取定位类型
    double altitude() const;       // 获取海拔高度
    double latitude() const;       // 获取纬度
    double longitude() const;      // 获取经度
    bool   isCoordinated();        // 定位是否成功

    void setReceiver(QObject* obj);

    //    GGA Global Positioning System Fix Data. Time, Position and fix related data
    //    for a GPS receiver
    //						                                   11
    //            1 	    2      3   4      5 6 7   8   9 10 |   12 13 14 15
    //            | 	    |      |   |      | | |   |   |  | |   |  |   | |
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


signals:
    //  定位类型信号
    void Coord2D();
    void Coord3D();
    void TypeChange(CoordinateType type);


public slots:

private:
    QObject* receiver;          // 事件接收者
    double  m_alti;             // 海拔
    double  m_long;             // 经度
    double  m_lati;             // 纬度
    bool    m_isCoordinated;
    CoordinateType m_type;      // 定位类型
    QTimer* gpsTimer;

    //  处理GPS语句
    void convertGPRMC(QByteArray RMCSentence);
    void convertGPGGA(QByteArray GGASentence);
    void convertGPGSA(QByteArray resArray);

};

#endif // GPSCLASS_H
