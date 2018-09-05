#include "Values.h"

Values::Values()
{
    Range = 20000;
    Resolution = 150;

    station_lat= 0.0;
    station_lon= 0.0;

    path_to_dem = "/home/hubertus/Height Data/dem/";
    path_to_tif = "/home/hubertus/Height Data/Aster/";
    path_to_antenna = "/home/hubertus/Antenna/";
    path_to_image = "/home/hubertus/Schreibtisch/";

    transmitter_height = 30;
    receiver_height = 10;

    frequency = 5700;

    //Step either can be 3 or 1 arcseconds for SRTM or ASTER
     step = 1;

     transmit_power_dBm = 20;
     gain_dbi = 13;
     receiver_gain_dbi= 16;
     cable_loss_dB = 0;
     antennatype = "default";
     name = "Station123";

     threadnumber= 0;

     azimuth =  0;
     altitude = 0;
}
