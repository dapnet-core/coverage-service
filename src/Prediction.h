#ifndef MAIN_TEST_H
#define MAIN_TEST_H
#include <stdio.h>
#include <iostream>
#include <fstream>
#include "Values.h"
#include "Models/TR_Model.h"
#include "Models/ITU_Model.h"
#include "Mapping.h"
#include "Models/ITM_Model.h"
#include "Image.h"
#include <QDate>
#include <QTime>
#include <QtCore>
#include <QThread>
#include <mutex>


double wavelength_m(double frequency_MHz);

fstream* Open_File(char* text);

double calculate_hF1(double f_MHz, double elev[],double tht_km, double rht_km, double distance_km);

double pathloss_ITM_ITU_with_Mix(prop_type &prop,propv_type &propv, double elev[]);

void initialize_parameters(prop_type &prop, propv_type &propv, double elev[]);

void settings(prop_type &prop, propv_type& propv);

double calculate_receive_power(double horizon,double vert, double pl,double pwr_dbm,double g_dbi, double c_loss_db,  double r_g_dbi,
                                double azimuth, double altitude, double &ver, double &hor ,double* Ant);

int prediction(Values *myValues, Image *myMap);

void work(int Threadnumber, int max_index, int min_index, Height_Map &map1, prop_type &prop,
          propv_type &propv, Image *myMap, Values *myValues, double* Ant);


class myThread : public QThread
{
    Q_OBJECT


    void run();

public:

    int max_index, min_index;
    Image *myMap;
    prop_type prop;
    Height_Map map1;
    propv_type propv;
    Values *myValues;
    double* AntennaArray;

    int t_number;


    void Thread_init(int t_number,int max_index, int min_index, Image *myMap, prop_type prop, Height_Map map1, propv_type propv, Values *myValues,double* Ant);




};

#endif // MAIN_TEST_H
