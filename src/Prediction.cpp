#include "Prediction.h"
#include "Values.h"
#include "Image.h"


using namespace std;

mutex m1;

fstream *Open_File(char *text) {
    fstream *fs = new fstream;

    strcat(text, ".txt");
    fs->open(text, fstream::out);
    if (fs->is_open())
        return fs;
    else return NULL;
}

static double _north, _south, _west, _east;

double wavelength_m(double frequency_MHz) {
    return (300 / frequency_MHz);
}

double calculate_hF1(double f_MHz, double elev[], double tht_km, double rht_km, double distance_km) {
    double h_F1_sign_Obstr = 100;
    double wavelength_m_1 = wavelength_m(f_MHz);

    //Gradient of LOS path
    double m_LOS = (elev[2] / 1000 + tht_km - elev[(int) elev[0] + 2] / 1000 - rht_km) / distance_km;

    //elev[]: [num points - 1], [delta dist(meters)], [height(meters) point 1], ..., [height(meters) point n]
    //n height points; elev[2] is height of transmitter; elev[n] height of receiver-station

    double d_1_km, d_2_km, h_Fresnel1_m, h_itu_m;


    for (int i = 3; i <= elev[0] + 1; i++) {
        //Calculating Fresnel Zone F1
        d_1_km = (i - 2) * elev[1] / 1000;
        d_2_km = distance_km - d_1_km;
        h_Fresnel1_m = sqrt(wavelength_m_1 * d_1_km * 1000 * d_2_km * 1000 / distance_km / 1000);

        //Calculating h=h_LOS-h_obstruction-h_earth_bulge

        double h_earth_bulge = (d_1_km * d_2_km) / (12.74 * 4 / 3);
        h_itu_m = (tht_km * 1000 + elev[2] - m_LOS * d_1_km * 1000 - elev[i] - h_earth_bulge);

        double height1 = h_itu_m / h_Fresnel1_m;
        if (height1 < h_F1_sign_Obstr) {
            h_F1_sign_Obstr = height1;
        }
    }
    return h_F1_sign_Obstr;
}

double pathloss_ITM_ITU_with_Mix(prop_type &prop, propv_type &propv, double elev[]) {
    ITM *itm = new ITM();
    double h_F1_sign_Obstr = calculate_hF1(prop.f_MHz, elev, prop.tht_km, prop.rht_km, prop.distance_km);
    double weighted_pathloss;
    if (h_F1_sign_Obstr >= 1) {
        double b;
        if (prop.distance_km >= 0) {
            b = TR_pathloss_dB(prop, propv);
        } else {
            b = itm->point_to_point(elev, propv, prop);
        }
        weighted_pathloss = b;
    }

    if (h_F1_sign_Obstr >= 0.3 && h_F1_sign_Obstr < 1) {
        double b;
        if (prop.distance_km >= 0) {
            b = TR_pathloss_dB(prop, propv);

        } else {
            b = itm->point_to_point(elev, propv, prop);
        }

        weighted_pathloss = b;
    }
    if (h_F1_sign_Obstr >= 0.2 && h_F1_sign_Obstr < 0.3) {
        double b;
        if (prop.distance_km >= 0) {
            b = TR_pathloss_dB(prop, propv);
            double a = itu_pathloss_dB(prop, propv, h_F1_sign_Obstr);
            weighted_pathloss = 0.75 * b
                                + 0.25 * a;
        } else {
            b = itm->point_to_point(elev, propv, prop);
            weighted_pathloss = b;
        }
    }
    if (h_F1_sign_Obstr >= 0.1 && h_F1_sign_Obstr < 0.2) {
        double b;
        if (prop.distance_km >= 0) {
            b = TR_pathloss_dB(prop, propv);
            double a = itu_pathloss_dB(prop, propv, h_F1_sign_Obstr);
            weighted_pathloss = 0.5 * b + 0.5 * a;
        } else {
            b = itm->point_to_point(elev, propv, prop);
            weighted_pathloss = b;
        }
    }
    if (h_F1_sign_Obstr >= 0 && h_F1_sign_Obstr < 0.1) {
        if (prop.distance_km >= 0) {
            double b = TR_pathloss_dB(prop, propv);
            double a = itu_pathloss_dB(prop, propv, h_F1_sign_Obstr);
            weighted_pathloss = 0.75 * a + 0.25 * b;
        } else {
            double b = itm->point_to_point(elev, propv, prop);
            weighted_pathloss = b;
        }
    }
    if (h_F1_sign_Obstr >= -0.25 && h_F1_sign_Obstr < 0) {
        double a = itu_pathloss_dB(prop, propv, h_F1_sign_Obstr);
        double b = itm->point_to_point(elev, propv, prop);
        weighted_pathloss = 0.75 * a + 0.25 * b;
    }
    if (h_F1_sign_Obstr >= -0.5 && h_F1_sign_Obstr < -0.25) {
        double a = itu_pathloss_dB(prop, propv, h_F1_sign_Obstr);
        double b = itm->point_to_point(elev, propv, prop);
        weighted_pathloss = 0.5 * a + 0.5 * b;
    }
    if (h_F1_sign_Obstr >= -0.75 && h_F1_sign_Obstr < -0.5) {
        double a = itu_pathloss_dB(prop, propv, h_F1_sign_Obstr);
        double b = itm->point_to_point(elev, propv, prop);
        weighted_pathloss = 0.25 * a + 0.75 * b;
    }
    if (h_F1_sign_Obstr < -0.75) {
        double a = itm->point_to_point(elev, propv, prop);
        weighted_pathloss = a;
    }
    return weighted_pathloss;
}

void initialize_parameters(prop_type &prop, propv_type &propv, double elev[]) {
    /*****Calculated for every Path*******************/

    //Distance in m and km
    prop.distance_km = elev[0] * elev[1] / 1000;
    prop.dist = prop.distance_km * 1000;

    //Calculation of Radio Horizons, effective heights, refractivity, Impedence
    hzns(elev, prop);
    qlrpfl(elev, propv.klim, propv.mdvar, prop, propv);
}

void settings(prop_type &prop, propv_type &propv) {
    /************Constant Settings**********/

    prop.tht_km = prop.hg[0] / 1000;
    prop.rht_km = prop.hg[1] / 1000;
    //Point to point-> mdp=-1
    prop.mdp = -1;

    prop.eps_dielect = 15;
    prop.conductivity = 0.005;
    prop.Ns = 301;
    //Wave Number
    prop.wn = prop.f_MHz / 47.7;

    prop.radio_climate = 5; //5 ->continental
    prop.polarization = 1; //1-> vertial, 0->horizontal
    //Normal refractivity index
    prop.ens = prop.Ns;

    //Maybe not used...
    prop.dbloss = 0;
    //Parameters for variability
    prop.rel = 0.99;
    prop.conf = 0.8; // 0.8 is equal to 0.7 in Radio Mobile by Roger Couede

    //Paramters of propv (variability)
    propv.klim = prop.radio_climate;
    propv.lvar = 5; // Initialization Indicator
    propv.mdvar = 11; // Variability Mode
    //  propv.mdvar can be
    //  0 ..  3
    // 10 .. 13, then no_location_variability is set    (no location variability)
    // 20 .. 23, then no_situation_variability is set   (no situatian variability)
    // 30 .. 33, then no_location_variability and no_situation_variability is set

    // X0: Single Message
    // X1: Individual
    // X2: Mobile
    // X3: Broadcast
}

double calculate_receive_power(double horizon, double vert, double pl, double pwr_dbm, double g_dbi, double c_loss_db,
                               double r_g_dbi,
                               double azimuth, double altitude, double *Ant) {
    //fs->seekg(0,ios_base::beg);

    int horizontal = (int) (horizon - azimuth);
    if (horizontal < 0)
        horizontal += 360;
    int vertical = (int) (vert - altitude);
    if (vertical < -270)
        vertical += 360;

    //the first 360 values in .ant file refer to the horizontal plane
    double horizontal_gain;
    if (horizontal == 0) {
        horizontal_gain = 0;
    } else {
        horizontal_gain = Ant[horizontal];

        if (horizon >= 359.5) {
            horizontal_gain = 0;
        }
    }

    //the folowing 360 (-90 - 270) values in .ant file refer to the vertical half plane
    double vertical_gain = 0;
    if (horizontal >= 270 || horizontal <= 90) {
        vertical_gain = Ant[360 + 90 - vertical];
        return pwr_dbm + g_dbi - c_loss_db + r_g_dbi - pl + vertical_gain + horizontal_gain;
    } else {
        vertical = (-1) * vertical - 180;
        vertical_gain = Ant[360 + 90 - vertical];
        return pwr_dbm + g_dbi - c_loss_db + r_g_dbi - pl + vertical_gain + horizontal_gain;
    }
}

int prediction(Values *myValues, Image *myMap) {

/******Geographic Preparation and Parameters*****/
    Height_Map map1;
    prop_type prop;
    propv_type propv;

    map1.Range_m = myValues->Range;

    map1.step = myValues->step;

    cout << endl << "Prepare Settings." << endl;
    coord station_coord_grad;

    //Only positive Values for longitude (all referred to east)
    station_coord_grad.lat = myValues->station_lat;
    station_coord_grad.lon = myValues->station_lon;
    if (station_coord_grad.lon < 0) {
        station_coord_grad.lon = station_coord_grad.lon + 360;
    }
    strcpy(map1.path_to_dem, myValues->path_to_dem.toStdString().c_str());
    strcpy(map1.path_to_tif, myValues->path_to_tif.toStdString().c_str());

    prop.hg[0] = myValues->transmitter_height;
    prop.hg[1] = myValues->receiver_height;
    prop.f_MHz = myValues->frequency;

    /************Hoehenkarte erstellen ******************************/


    if (mapping(map1, station_coord_grad, myValues->Resolution) == 0) {
        return 99;
    }

    /*************Berechnung der Pfadverluste*************************/
    settings(prop, propv);

    /******************Threading**************************/

    cout << endl << "Main-Thread: " << QThread::currentThread()->currentThreadId() << " Priority:"
         << QThread::currentThread()->priority() << endl;

    /*****************Antenna****************************************/

    char antennatype[80] = "";
    strcpy(antennatype, myValues->antennatype.toStdString().c_str());

    char path_to_antenna[80] = "";
    strcpy(path_to_antenna, myValues->path_to_antenna.toStdString().c_str());
    fstream *Ant = new fstream;
    double AntennaArray[720];
    if (strcmp(antennatype, "default")) {

        char path[80] = "";
        strcpy(path, path_to_antenna);
        strcat(path, antennatype);
        strcat(path, ".ant");

        Ant->open(path);

        //.ant file not found-> return power for isotropic antenna
        if (!(Ant->is_open())) {
            cout << endl << "Antenna: " << path << " not found. Use isotropic one instead." << endl;
            strcat(antennatype, "default");
        }
    }

    if (strcmp(antennatype, "default") == 0) {
        cout << endl << "Default (isotropic) antenna applied." << endl;
        for (int a = 0; a < 720; a++) AntennaArray[a] = 0.0;
    } else {
        char line[10] = "";
        for (int var = 0; var < 720; var++) {
            Ant->getline(line, 10);
            sscanf(line, "%lf", &AntennaArray[var]);
        }
    }

/********************Threading*********************************/
//Anzahl neu zu erstellender Threads: int number; Anzahl Zeilen pro Thread
    int number = myValues->threadnumber;
    int max_index = myMap->get_index();


    int SizePerThread = max_index / (number + 1);

    myThread *box[number];
    int tnum;

    for (tnum = 0; tnum < number; tnum++) {
        int min_index = tnum * SizePerThread;
        int my_max_index = (tnum + 1) * SizePerThread;
        box[tnum] = new myThread();
        box[tnum]->Thread_init(tnum, my_max_index, min_index, myMap, prop, map1, propv, myValues, AntennaArray);
        box[tnum]->start();
        //cout << "Running: "<<box[tnum]->isRunning() << endl;
    }

/******Linux does not support priority scheduling....******
    cout <<"0: " << box[0]->currentThreadId() << " Priority:" << box[0]->priority()<< endl;
    QThread::currentThread()->setPriority(QThread::NormalPriority);
    cout <<"Main-Thread: " << QThread::currentThread()->currentThreadId() << " Priority:" << QThread::currentThread()->priority()<< endl;
    QThread::currentThread()->msleep(100);

*/
    work(tnum, max_index, tnum * SizePerThread, map1, prop, propv, myMap, myValues, AntennaArray);


/********Warte darauf, dass alle Threads fertig sind....**********/
    /* while(box[0]->isRunning()==1 && box[1]->isRunning()==1)
      {
          cout << "L�uft";
          cout.flush();
          QThread::currentThread()->msleep(1000);
      }
   */
    for (int y = 0; y < number; y++) {
        box[y]->wait();
    }
    cout << "Warten beendet" << endl;
    Ant->close();

/*Multithreading-Ansatz:
 * Karte myMap direkt in gleich gro�e St�cke teilen und jedem Thread ein St�ck �berlassen
*/

    prop.north = _north;
    prop.south = _south;
    prop.west = _west;
    prop.east = _east;

    //Abdeckungskarte erstellt

    myMap->ready = true;

    //aktuelles Datum und Uhrzeit in die LogDatei schreiben

    char thisname[80] = "";
    char date[20] = "";
    strcpy(thisname, myValues->path_to_image.toStdString().c_str());
    strcat(thisname, myValues->name.toStdString().c_str());
    strcat(date, QDate::currentDate().toString("ddMMyy").toStdString().c_str());
    strcat(date, "_");
    strcat(date, QTime::currentTime().toString("hh:mm").toStdString().c_str());

    /*********************Erstelle Logfile******************************/

    fstream *fs = Open_File(thisname);

    *fs << date;
    *fs << endl;
    *fs << prop.north;
    *fs << endl;
    *fs << prop.south;
    *fs << endl;
    *fs << prop.east;
    *fs << endl;
    *fs << prop.west << endl << endl;
    *fs << "North:" << myValues->station_lat << endl;
    *fs << "East:" << myValues->station_lon << endl;
    *fs << "Transmitter Power in W: " << myValues->transmit_power_dBm << endl;
    *fs << "Antenna Gain in dBi: " << myValues->gain_dbi << endl;
    *fs << "Antennea Height in m: " << myValues->transmitter_height << endl;
    *fs << "Receiver Height in m: " << myValues->receiver_height << endl;
    *fs << "Receiver Gain in dBi: " << myValues->receiver_gain_dbi << endl;
    *fs << "Frequency in MHz: " << myValues->frequency << endl;
    *fs << "Azimuth in degree: " << myValues->azimuth << " deg Azimuth and " << myValues->altitude << " deg Elevation angle" << endl;
    *fs << "Antenna Type: " << myValues->antennatype.toStdString().c_str() << endl;
    fs->close();
    return 1;
}

void work(int Threadnumber, int max_index, int min_index, Height_Map &map1, prop_type &prop,
          propv_type &propv, Image *myMap, Values *myValues, double *Ant) {

    if (Threadnumber > -1) {
        //Start in the down right corner of the map and move to the north and west
        //Resolution for calculated pathloss and receive Energy map
        int map_resolution_m = myValues->Resolution;

        double transmit_power_dBm = myValues->transmit_power_dBm;
        double gain_dbi = myValues->gain_dbi;
        double receiver_gain_dbi = myValues->receiver_gain_dbi;
        double cable_loss_dB = myValues->cable_loss_dB;

        cout << "Thread Nummer " << Threadnumber << " is started." << " With max:" << max_index << " and Min: "
             << min_index << endl;
        coord act_pixel = map1.station_coord;
        double progress = 0;
        double progress_help = 0;
        for (int i_lat = min_index; i_lat <= max_index; i_lat++) {
            act_pixel.lat = map1.station_coord.lat - meter2arcsec_lat(map1.station_coord, map1.Range_m) +
                            meter2arcsec_lat(act_pixel, i_lat * map_resolution_m);
            act_pixel.lon = map1.station_coord.lon + meter2arcsec_lon(act_pixel, map1.Range_m);

            //�bergang 360->0
            if (act_pixel.lon > 360 * 3600) { act_pixel.lon = act_pixel.lon - 360 * 3600; }

            for (int i_lon = 0; i_lon <= myMap->get_index(); i_lon++) {


                act_pixel.lon -= meter2arcsec_lon(act_pixel, map_resolution_m);

                //Transition 0->360
                if (act_pixel.lon < 0) {
                    act_pixel.lon += 360 * 3600;
                }

                if (i_lon == myMap->get_index() && i_lat == floor(myMap->get_index() / 2.0)) {
                    //prop.east = act_pixel.lon /3600;
                    //cout  << endl <<"East: " << Threadnumber <<" " << prop.east << endl;
                    if (act_pixel.lon > 180 * 3600 && act_pixel.lon <= 360 * 3600) {
                        _east = act_pixel.lon / 3600 - 360;
                    } else {
                        _east = act_pixel.lon / 3600;
                    }
                }
                if (i_lon == 0 && i_lat == floor(myMap->get_index() / 2.0)) {
                    //prop.west = act_pixel.lon /3600;
                    //cout  << endl <<"West: " << Threadnumber << " "  << prop.west << endl;
                    if (act_pixel.lon > 180 * 3600 && act_pixel.lon <= 360 * 3600) {
                        _west = act_pixel.lon / 3600 - 360;
                    } else { _west = act_pixel.lon / 3600; }
                }
                if (i_lon == 0 && i_lat == myMap->get_index()) {

                    //prop.north = act_pixel.lat /3600;
                    //cout  << endl <<"North: " << Threadnumber << " " << prop.north << endl;
                    _north = act_pixel.lat / 3600;

                }
                if (i_lon == 0 && i_lat == 0) {
                    //prop.south = act_pixel.lat /3600;
                    //cout  << endl <<"South: " << Threadnumber << " " << prop.south << endl;
                    _south = act_pixel.lat / 3600;
                }

                double *elev = new double;
                double receiver_height;
                double alpha_grad = 0;

                elev = Path(map1, act_pixel, alpha_grad, receiver_height);
                double weighted_pathloss;
                double beta;
                if (*elev > 1) {

                    initialize_parameters(prop, propv, elev);
                    //m1.lock();
                    weighted_pathloss = pathloss_ITM_ITU_with_Mix(prop, propv, elev);
                    //m1.unlock();
                    //beta is the vertical angle relative to the horizon
                    //beta is in grad

                    beta = 0;

                    double asin_arg = (elev[2] - receiver_height + prop.hg[0] - prop.hg[1]) / prop.dist;
                    if (asin_arg >= 1) { beta = -90; }
                    else if (asin_arg <= -1) { beta = 90; }
                    else { beta = -180 / 3.14159265 * std::asin(asin_arg); }

                } else {
                    weighted_pathloss = 0;
                    beta = 0;
                }
                delete elev;
                double pwr = calculate_receive_power(alpha_grad, beta, weighted_pathloss, transmit_power_dBm, gain_dbi,
                                                     cable_loss_dB, receiver_gain_dbi,
                                                     myValues->azimuth, myValues->altitude, Ant);
                m1.lock();
                myMap->set(i_lat, i_lon, pwr);
                m1.unlock();

            }
            double help = 100.00 / (max_index - min_index);
            progress_help += help;

            if (progress_help > 10) {
                progress += 10;
                cout << progress << " %  ";
                progress_help = 0;
                cout.flush();
            }
        }
        cout << endl << "100 %. Thread ready.";
    }
}

//myThread::myThread()
//{}

void myThread::Thread_init(int t_number, int max_index, int min_index, Image *myMap, prop_type prop, Height_Map map1,
                           propv_type propv, Values *myValues, double *Ant) {
    this->t_number = t_number;
    this->max_index = max_index;
    this->min_index = min_index;
    this->map1 = map1;
    this->prop = prop;
    this->myMap = myMap;
    this->propv = propv;
    this->myValues = myValues;
    this->AntennaArray = Ant;
}

void myThread::run() {
    cout << "Additional Thread: " << this->currentThreadId() << endl;
    work(this->t_number, this->max_index, this->min_index, this->map1, this->prop, this->propv, this->myMap,
         this->myValues, this->AntennaArray);
}


