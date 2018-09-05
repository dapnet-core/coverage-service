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


int main(int argc,char *argv[])
//int main()
{
    //For Non-GUI Apps
   //QCoreApplication a(argc, argv);

    //a.exec();
   //int argc=8;
   //char *argv[] = {"hallo", "-N", "50.97623", "-O", "7.06937","-f","4000","-p","50"};

   clock_t t;
   t=clock();

    QLocale::setDefault(QLocale::German);

if(argc ==1)
{

    cout << endl;
    cout << "****HamNet Ausbreitungssimulation****"<<endl;
    cout << endl;
    cout << "Default Werte ausgeben mit -default"<< endl << endl;
    cout << "-f         Frequenz in MHz"<<endl;
    cout << "-ht        Antennenhoehe Transmitter in m"<<endl;
    cout << "-hr        Antennenhoehe Receiver in m"<<endl;
    cout << "-p         Sendeleistung in dBm"<< endl;
    cout << "-gt        Antennengewinn Transmitter in dB"<<endl;
    cout << "-gr        Antennengewinn Receiver in dB"<<endl;
    cout << "-c         Kabelverluste in dB"<<endl;
    cout << "-az        Azimuth in Grad"<<endl;
    cout << "-al        Hoehenwinkel in Grad"<<endl;
    cout << "-N         Stationskoordinaten, Grad Nord"<<endl;
    cout << "-O         Stationskoordinaten, Grad Ost"<<endl;
    cout << "-A         Absoluter Pfad zu Antennnendiagrammen"<<endl;
    cout << "-T         Absoluter Pfad zu ASTER Daten"<<endl;
    cout << "-D         Absoluter Pfad zu DEM Dateien"<<endl;
    cout << "-Image     Absoluter Pfad zum Speicherort d. fertigen Bilder"<< endl;
    cout << "-r         Reichweite [Range] in m"<<endl;
    cout << "-R         Aufloesung [Resolution] in m"<<endl;
    cout << "-ant       Antennentyp; in Antennenpfad zu finden"<< endl << endl;
    cout << "-n         Bezeichnung der Station; z.B. db0ko_NO" << endl << endl;
    cout <<"-th         Anzahl zusätzlicher Threads" <<endl;

    return 1;
}

Values *myValues = new Values();
if(strcmp(argv[1],"-default")==0)
{
    cout << "Frequenz: " << myValues->frequency <<endl
            << "Koordinaten: " << myValues->station_lat << "N " << myValues->station_lon <<"O" <<endl
               << "Senderantennenhoehe: " << myValues->transmitter_height <<endl
                  << "Empfangsantennenhoehe: " << myValues->receiver_height <<endl
                     << "Sendeleistung: " << myValues->transmit_power_dBm <<endl
                        << "Antennengewinn: " << myValues->gain_dbi <<endl
                           << "Empfangsantennengewinn: " << myValues->receiver_gain_dbi <<endl
                              << "Kabelverlust: " << myValues->cable_loss_dB <<endl
                                 << "Azimuth: " << myValues->azimuth <<endl
                                    << "Hoehenwinkel: " << myValues->altitude <<endl
                                       << "Antennentyp: " << myValues->antennatype.toStdString() <<endl;

    return 1;


}
      for (int i=1; i<argc ; i++)
{


if(strcmp(argv[i],"-f")==0)
{
    if(i+1 < argc && argv[i+1][0] && argv[i+1][0] != '-')
        if(!(sscanf(argv[i+1],"%lf", &myValues->frequency)))
        {
            cout << "Falsche Eingabe" << endl;
            return 0;
        }

}
if(strcmp(argv[i],"-ht")==0)
{
    if(i+1 < argc && argv[i+1][0] && argv[i+1][0] != '-')
        if(!(sscanf(argv[i+1],"%lf", &myValues->transmitter_height)))
    {
        cout << "Falsche Eingabe" << endl;
        return 0;
    }
}
if(strcmp(argv[i],"-hr")==0)
{
    if(i+1 < argc && argv[i+1][0] && argv[i+1][0] != '-')
        if(!sscanf(argv[i+1],"%lf", &myValues->receiver_height))
    {
        cout << "Falsche Eingabe" << endl;
        return 0;
    }
}
if(strcmp(argv[i],"-p")==0)
{
    if(i+1 < argc && argv[i+1][0] && argv[i+1][0] != '-')
        if(!sscanf(argv[i+1],"%lf", &myValues->transmit_power_dBm))
    {
        cout << "Falsche Eingabe" << endl;
        return 0;
    }
}
if(strcmp(argv[i],"-gt")==0)
{
    if(i+1 < argc && argv[i+1][0] && argv[i+1][0] != '-')
        if(!sscanf(argv[i+1],"%lf", &myValues->gain_dbi))
    {
        cout << "Falsche Eingabe" << endl;
        return 0;
    }
}
if(strcmp(argv[i],"-gr")==0)
{
    if(i+1 < argc && argv[i+1][0] && argv[i+1][0] != '-')
        if(!sscanf(argv[i+1],"%lf", &myValues->receiver_gain_dbi))
    {
        cout << "Falsche Eingabe" << endl;
        return 0;
    }
}
if(strcmp(argv[i],"-c")==0)
{
    if(i+1 < argc && argv[i+1][0] && argv[i+1][0] != '-')
        if(!sscanf(argv[i+1],"%lf", &myValues->cable_loss_dB))
    {
        cout << "Falsche Eingabe" << endl;
        return 0;
    }
}
if(strcmp(argv[i],"-az")==0)
{
    if(i+1 < argc && argv[i+1][0])
        if(!sscanf(argv[i+1],"%lf", &myValues->azimuth))
    {
        cout << "Falsche Eingabe" << endl;
        return 0;
    }
}
if(strcmp(argv[i],"-al")==0)
{
    if(i+1 < argc && argv[i+1][0])
        if(!sscanf(argv[i+1],"%lf", &myValues->altitude))
    {
        cout << "Falsche Eingabe" << endl;
        return 0;
    }
}
if(strcmp(argv[i],"-N")==0)
{
    if(i+1 < argc && argv[i+1][0])
        if(!sscanf(argv[i+1],"%lf", &myValues->station_lat))
        {
            cout << "Falsche Eingabe" << endl;
            return 0;
        }
    //myValues->station_lat = atof(argv[i+1]);
}
if(strcmp(argv[i],"-O")==0)
{
    if(i+1 < argc && argv[i+1][0])
       if(!sscanf(argv[i+1],"%lf", &myValues->station_lon))
    {
       cout << "Falsche Eingabe" << endl;
       return 0;
    }
   // myValues->station_lon = atof(argv[i+1]);
    //cout<<"my: "<< myValues->station_lon << "In: " << argv[i+1];

}
if(strcmp(argv[i],"-A")==0)
{
    if(i+1 < argc && argv[i+1][0] && argv[i+1][0] != '-')
     {

        myValues->path_to_antenna =  argv[i+1];
        QByteArray ba = myValues->path_to_antenna.toLocal8Bit();
        char *c_str2 = ba.data();
        strcat(c_str2, "/test");
    if(open(c_str2 , O_RDWR | O_CREAT, S_IWUSR) < 0)
    {
        cout << "Antennenverzeichnis ungültig." << endl;
        int b=remove(c_str2);
        return 0;
    }
        int c = remove(c_str2);

    }
}
if(strcmp(argv[i],"-T")==0)
{
    if(i+1 < argc && argv[i+1][0] && argv[i+1][0] != '-')
     {

        myValues->path_to_tif=  argv[i+1];
        QByteArray ba = myValues->path_to_tif.toLocal8Bit();
        char *c_str2 = ba.data();
        strcat(c_str2, "/test");
    if(open(c_str2 , O_RDWR | O_CREAT, S_IWUSR) < 0)
    {
        cout << "ASTER-Verzeichnis ungültig." << endl;
        int b=remove(c_str2);
        return 0;
    }
        int c = remove(c_str2);

    }

}
if(strcmp(argv[i],"-D")==0)
{
    if(i+1 < argc && argv[i+1][0] && argv[i+1][0] != '-')
     {

        myValues->path_to_dem=  argv[i+1];
        QByteArray ba = myValues->path_to_dem.toLocal8Bit();
        char *c_str2 = ba.data();
        strcat(c_str2, "/test");
    if(open(c_str2 , O_RDWR | O_CREAT, S_IWUSR) < 0)
    {
        cout << "DEM-Verzeichnis "<<c_str2 <<" ungültig." << endl;
        int b=remove(c_str2);
        return 0;
    }
        int c = remove(c_str2);

    }
}
if(strcmp(argv[i],"-Image")==0)
{
    if(i+1 < argc && argv[i+1][0] && argv[i+1][0] != '-')
     {

        myValues->path_to_image = argv[i+1];
        QByteArray ba = myValues->path_to_image.toLocal8Bit();
        char *c_str2 = ba.data();
        strcat(c_str2, "/test");
    if(open(c_str2 , O_RDWR | O_CREAT, S_IWUSR) < 0)
    {
        cout << "Image-Verzeichnis ungültig." << endl;
        int b=remove(c_str2);
        return 0;
    }
        int c = remove(c_str2);

    }
}
if(strcmp(argv[i],"-r")==0)
{
    if(i+1 < argc && argv[i+1][0] && argv[i+1][0] != '-')
        if(!sscanf(argv[i+1],"%d", &myValues->Range))
    {
        cout << "Falsche Eingabe" << endl;
        return 0;
    }
}
if(strcmp(argv[i],"-R")==0)
{
    if(i+1 < argc && argv[i+1][0] && argv[i+1][0] != '-')
        if(!sscanf(argv[i+1],"%d", &myValues->Resolution))
    {
        cout << "Falsche Eingabe" << endl;
        return 0;
    }
}
if(strcmp(argv[i],"-ant")==0)
{
    //Check if .ant is the end of the name, if it is remove

       QString help = argv[i+1];
       myValues->antennatype.replace(0,30,help);
 }


      if(strcmp(argv[i],"-n")==0)
      {
          //Check if .ant is the end of the name, if it is remove


          if(i+1 < argc && argv[i+1][0] && argv[i+1][0] != '-')
             { QString help = argv[i+1];
               myValues->name.replace(0,30,help);
             }



      }

      if(strcmp(argv[i],"-th")==0)
      {


          if(i+1 < argc && argv[i+1][0] && argv[i+1][0] != '-')
              if(!sscanf(argv[i+1],"%d", &myValues->threadnumber))
          {
              cout << "Falsche Eingabe" << endl;
              return 0;
          }


       }




}

    Image *myImage = new Image();

    myImage->initialize_Map(myValues->Range, myValues->Resolution);

    int statuscode = prediction(myValues, myImage);

      if(statuscode==1)
      {
          //create_Image(myValues,myImage);
        create_Layers(myValues, myImage);

          t= clock() -t;
          t=((float)t)/CLOCKS_PER_SEC;
          cout <<endl << "Zeit: " << t << endl;

        return 1;
      }
      else if (statuscode== 99)
      {
       cout << endl << "Map-Error: Maybe ASTER-file not available.";
       return 99;
      }
      else
      {

        return 0;
      }

}
