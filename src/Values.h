
#ifndef VALUES_H
#define VALUES_H

#include <QString>

class Values
{
public:
    Values();

    int Range;
    int Resolution;

    double station_lat;
    double station_lon;

    QString path_to_dem;
    QString path_to_tif;
    QString path_to_antenna;
    QString path_to_image;

    double transmitter_height;
    double receiver_height;

    double frequency;

    int step;

    /*Antenna Parameter*/
    double transmit_power_dBm;
    double gain_dbi;
    double receiver_gain_dbi;
    double cable_loss_dB;

    QString antennatype;
    QString name;

    double azimuth;
    double altitude;

    int threadnumber;



};

#endif // VALUES_H
