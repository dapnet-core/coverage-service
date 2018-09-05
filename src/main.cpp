#include <QtGui>

#include <QApplication>
#include "Image.h"
#include "Values.h"
#include "Prediction.h"
#include "create_image.h"
#include <QtCore>
#include <dirent.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdio.h>
#include <QThread>
#include <time.h>
#include <locale>
#include <QLocale>

//int main()
int main(int argc, char *argv[]) {
    //For Non-GUI Apps
    //QCoreApplication a(argc, argv);

    //a.exec();
    //int argc=8;
    //char *argv[] = {"hallo", "-N", "50.97623", "-O", "7.06937","-f","4000","-p","50"};

    clock_t t;
    t = clock();

    QLocale::setDefault(QLocale::German);

    if (argc == 1) {

        cout << endl;
        cout << "****VHF/UHF/SHF radio coverage predictor****" << endl;
        cout << "Hubertus Munz and Ralf Wilke" << endl;
        cout << endl;
        cout << "Default values can be obtained by -default" << endl << endl;
        cout << "-f         Frequency in MHz" << endl;
        cout << "-ht        Antenna Height Transmitter in m" << endl;
        cout << "-hr        Antenna Height Receiver in m" << endl;
        cout << "-p         Transmitter Power in dBm" << endl;
        cout << "-gt        Antenna Gain Transmitter in dBi" << endl;
        cout << "-gr        Antenna Gain Receiver in dBi" << endl;
        cout << "-c         Cable losses in dB" << endl;
        cout << "-az        Azimuth in degree" << endl;
        cout << "-al        Elevation angle in degree" << endl;
        cout << "-N         Coordinates, degree north" << endl;
        cout << "-O         Coordinates, degrees east" << endl;
        cout << "-A         Absolute Path to Antenna diagrams" << endl;
        cout << "-T         Absolute Path to ASTER Data" << endl;
        cout << "-D         Absolute Path to DEM Files" << endl;
        cout << "-Image     Absolute Path to store the output images" << endl;
        cout << "-r         Range in m" << endl;
        cout << "-R         Resolution in m" << endl;
        cout << "-ant       Antenna Type; to be found in Antennapath" << endl;
        cout << "-n         Label of the Transmitter" << endl;
        cout << "-th        Number of additional threads" << endl;

        return 1;
    }

    Values *myValues = new Values();
    if (strcmp(argv[1], "-default") == 0) {
        cout << "Frequency: " << myValues->frequency << endl
             << "Coordinates: " << myValues->station_lat << "N " << myValues->station_lon << "O" << endl
             << "Transmitter Antenna Height in m: " << myValues->transmitter_height << endl
             << "Reveiver Antenna Height in m: " << myValues->receiver_height << endl
             << "Transmitter Power in dBm: " << myValues->transmit_power_dBm << endl
             << "Transmitter Antenna Gain in dBi: " << myValues->gain_dbi << endl
             << "Receiver Antenna Gain in dBi: " << myValues->receiver_gain_dbi << endl
             << "Cable losses in dB: " << myValues->cable_loss_dB << endl
             << "Azimuth: " << myValues->azimuth << endl
             << "Elevation Angle: " << myValues->altitude << endl
             << "Antenna Type: " << myValues->antennatype.toStdString() << endl;

        return 1;


    }
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-f") == 0) {
            if (i + 1 < argc && argv[i + 1][0] && argv[i + 1][0] != '-')
                if (!(sscanf(argv[i + 1], "%lf", &myValues->frequency))) {
                    cout << "Wrong input" << endl;
                    return 0;
                }
        }
        if (strcmp(argv[i], "-ht") == 0) {
            if (i + 1 < argc && argv[i + 1][0] && argv[i + 1][0] != '-')
                if (!(sscanf(argv[i + 1], "%lf", &myValues->transmitter_height))) {
                    cout << "Wrong input" << endl;
                    return 0;
                }
        }
        if (strcmp(argv[i], "-hr") == 0) {
            if (i + 1 < argc && argv[i + 1][0] && argv[i + 1][0] != '-')
                if (!sscanf(argv[i + 1], "%lf", &myValues->receiver_height)) {
                    cout << "Wrong input" << endl;
                    return 0;
                }
        }
        if (strcmp(argv[i], "-p") == 0) {
            if (i + 1 < argc && argv[i + 1][0] && argv[i + 1][0] != '-')
                if (!sscanf(argv[i + 1], "%lf", &myValues->transmit_power_dBm)) {
                    cout << "Wrong input" << endl;
                    return 0;
                }
        }
        if (strcmp(argv[i], "-gt") == 0) {
            if (i + 1 < argc && argv[i + 1][0] && argv[i + 1][0] != '-')
                if (!sscanf(argv[i + 1], "%lf", &myValues->gain_dbi)) {
                    cout << "Wrong input" << endl;
                    return 0;
                }
        }
        if (strcmp(argv[i], "-gr") == 0) {
            if (i + 1 < argc && argv[i + 1][0] && argv[i + 1][0] != '-')
                if (!sscanf(argv[i + 1], "%lf", &myValues->receiver_gain_dbi)) {
                    cout << "Wrong input" << endl;
                    return 0;
                }
        }
        if (strcmp(argv[i], "-c") == 0) {
            if (i + 1 < argc && argv[i + 1][0] && argv[i + 1][0] != '-')
                if (!sscanf(argv[i + 1], "%lf", &myValues->cable_loss_dB)) {
                    cout << "Wrong input" << endl;
                    return 0;
                }
        }
        if (strcmp(argv[i], "-az") == 0) {
            if (i + 1 < argc && argv[i + 1][0])
                if (!sscanf(argv[i + 1], "%lf", &myValues->azimuth)) {
                    cout << "Wrong input" << endl;
                    return 0;
                }
        }
        if (strcmp(argv[i], "-al") == 0) {
            if (i + 1 < argc && argv[i + 1][0])
                if (!sscanf(argv[i + 1], "%lf", &myValues->altitude)) {
                    cout << "Wrong input" << endl;
                    return 0;
                }
        }
        if (strcmp(argv[i], "-N") == 0) {
            if (i + 1 < argc && argv[i + 1][0])
                if (!sscanf(argv[i + 1], "%lf", &myValues->station_lat)) {
                    cout << "Wrong input" << endl;
                    return 0;
                }
            //myValues->station_lat = atof(argv[i+1]);
        }
        if (strcmp(argv[i], "-O") == 0) {
            if (i + 1 < argc && argv[i + 1][0])
                if (!sscanf(argv[i + 1], "%lf", &myValues->station_lon)) {
                    cout << "Wrong input" << endl;
                    return 0;
                }
            // myValues->station_lon = atof(argv[i+1]);
            //cout<<"my: "<< myValues->station_lon << "In: " << argv[i+1];

        }
        if (strcmp(argv[i], "-A") == 0) {
            if (i + 1 < argc && argv[i + 1][0] && argv[i + 1][0] != '-') {

                myValues->path_to_antenna = argv[i + 1];
                QByteArray ba = myValues->path_to_antenna.toLocal8Bit();
                char *c_str2 = ba.data();
                strcat(c_str2, "/test");
                if (open(c_str2, O_RDWR | O_CREAT, S_IWUSR) < 0) {
                    cout << "Antenna-Directory invalid." << endl;
                    int b = remove(c_str2);
                    return 0;
                }
                int c = remove(c_str2);

            }
        }
        if (strcmp(argv[i], "-T") == 0) {
            if (i + 1 < argc && argv[i + 1][0] && argv[i + 1][0] != '-') {

                myValues->path_to_tif = argv[i + 1];
                QByteArray ba = myValues->path_to_tif.toLocal8Bit();
                char *c_str2 = ba.data();
                strcat(c_str2, "/test");
                if (open(c_str2, O_RDWR | O_CREAT, S_IWUSR) < 0) {
                    cout << "ASTER-Directory invalid." << endl;
                    int b = remove(c_str2);
                    return 0;
                }
                int c = remove(c_str2);

            }
        }
        if (strcmp(argv[i], "-D") == 0) {
            if (i + 1 < argc && argv[i + 1][0] && argv[i + 1][0] != '-') {

                myValues->path_to_dem = argv[i + 1];
                QByteArray ba = myValues->path_to_dem.toLocal8Bit();
                char *c_str2 = ba.data();
                strcat(c_str2, "/test");
                if (open(c_str2, O_RDWR | O_CREAT, S_IWUSR) < 0) {
                    cout << "DEM-Directory " << c_str2 << " invalid." << endl;
                    int b = remove(c_str2);
                    return 0;
                }
                int c = remove(c_str2);
            }
        }
        if (strcmp(argv[i], "-Image") == 0) {
            if (i + 1 < argc && argv[i + 1][0] && argv[i + 1][0] != '-') {

                myValues->path_to_image = argv[i + 1];
                QByteArray ba = myValues->path_to_image.toLocal8Bit();
                char *c_str2 = ba.data();
                strcat(c_str2, "/test");
                if (open(c_str2, O_RDWR | O_CREAT, S_IWUSR) < 0) {
                    cout << "Image-Directory invalid." << endl;
                    int b = remove(c_str2);
                    return 0;
                }
                int c = remove(c_str2);
            }
        }
        if (strcmp(argv[i], "-r") == 0) {
            if (i + 1 < argc && argv[i + 1][0] && argv[i + 1][0] != '-') {
                if (!sscanf(argv[i + 1], "%d", &myValues->Range)) {
                    cout << "Wrong input" << endl;
                    return 0;
                }
            }
        }
        if (strcmp(argv[i], "-R") == 0) {
            if (i + 1 < argc && argv[i + 1][0] && argv[i + 1][0] != '-') {
                if (!sscanf(argv[i + 1], "%d", &myValues->Resolution)) {
                    cout << "Wrong input" << endl;
                    return 0;
                }
            }
        }

        //Check if .ant is the end of the name, if it is remove
        if (strcmp(argv[i], "-ant") == 0) {
            QString help = argv[i + 1];
            myValues->antennatype.replace(0, 30, help);
        }

        //Check if .ant is the end of the name, if it is remove
        if (strcmp(argv[i], "-n") == 0) {
            if (i + 1 < argc && argv[i + 1][0] && argv[i + 1][0] != '-') {
                QString help = argv[i + 1];
                myValues->name.replace(0, 30, help);
            }
        }

        if (strcmp(argv[i], "-th") == 0) {
            if (i + 1 < argc && argv[i + 1][0] && argv[i + 1][0] != '-')
                if (!sscanf(argv[i + 1], "%d", &myValues->threadnumber)) {
                    cout << "Wrong input" << endl;
                    return 0;
                }
        }
    }

    Image *myImage = new Image();
    myImage->initialize_Map(myValues->Range, myValues->Resolution);

    int statuscode = prediction(myValues, myImage);
    if (statuscode == 1) {
        create_Layers(myValues, myImage);

        t = clock() - t;
        t = ((float) t) / CLOCKS_PER_SEC;
        cout << endl << "Zeit: " << t << endl;

        return 1;
    } else if (statuscode == 99) {
        cout << endl << "Map-Error: Maybe ASTER-file not available.";
        return 99;
    } else {
        return 0;
    }
}
