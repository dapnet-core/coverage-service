#include "create_image.h"
#include <QPainter>
#include <QImage>
#include <QColor>

void create_Image(Values *myValues, Image *myMap)
{

    /*******MAP****************************


QImage (0,0)->xxxxxxxxxxxxxxxxxxxxxxx
              xxxxxxxxxxxxxxxxxxxxxxx
              xxxxxxxxxxxxxxxxxxxxxxx
              xxxxxxxxxxxxxxxxxxxxxxx       Y
              xxxxxxxxxxxxxxxxxxxxxxx
              xxxxxxxxxxxxxxxxxxxxxxx
              xxxxxxxxxxxxxxxxxxxxxxx <- myMap.at(0,0)

                         X

      ***********************************/

    //int size = 2*myValues->Range/myValues->Resolution;
    int size = myMap->get_index();

    QImage img = QImage(size,size,QImage::Format_ARGB32);
    img.fill(QColor(Qt::white).rgba());

    int pixel_x =0, pixel_y=0;

    int op = 190;
    int op_white = 0;

    if(myMap->ready)
    for (int lat = myMap->get_index()-1; lat >=1 ; lat--)
    {



        for(int lon = myMap->get_index()-1; lon >= 1 ; lon--)
        {
            if(myMap->at(lat,lon) >= -66 )
             img.setPixel(pixel_x,pixel_y,qRgba(255, 50, 0,op));
            else if(myMap->at(lat,lon)>= -68.6)
            img.setPixel(pixel_x,pixel_y,qRgba(255, 100, 100,op));
            else if(myMap->at(lat,lon)>= -71.2)
               img.setPixel(pixel_x,pixel_y,qRgba(255, 220, 100,op));
            else if(myMap->at(lat,lon)>= -73.8)
                img.setPixel(pixel_x,pixel_y,qRgba(255, 255, 100,op));
            else if(myMap->at(lat,lon)>= -76.4)
                img.setPixel(pixel_x,pixel_y,qRgba(192, 255, 100,op));
            else if(myMap->at(lat,lon)>= -79)
                img.setPixel(pixel_x,pixel_y,qRgba(100, 255, 100,op));
            else if(myMap->at(lat,lon)>= -81.6)
                img.setPixel(pixel_x,pixel_y,qRgba(100, 255, 100,op));
            else if(myMap->at(lat,lon)>= -84.2)
                img.setPixel(pixel_x,pixel_y,qRgba(100, 255, 192,op));
            else if(myMap->at(lat,lon)>= -86.8)
                img.setPixel(pixel_x,pixel_y,qRgba(100, 255, 255,op));
            else if(myMap->at(lat,lon)>= -89.4)
                img.setPixel(pixel_x,pixel_y,qRgba(100, 220, 255,op));
            else if(myMap->at(lat,lon)>= -92)
                img.setPixel(pixel_x,pixel_y,qRgba(100, 100,255,op ));
            else img.setPixel(pixel_x,pixel_y, qRgba(255,255,255,op_white));


          pixel_x ++;
        }
        pixel_y++;
        pixel_x=0;
    }

    QString help123  = myValues->path_to_image+ myValues->name +" " + QDate::currentDate().toString("ddMMyy").toStdString().c_str() +"_"+ QTime::currentTime().toString("hh:mm").toStdString().c_str() + ".png";

    img.save(help123);



}

void create_Layers(Values *myValues, Image *myMap)
{
    int size = myMap->get_index();

    QImage img_red = QImage(size,size,QImage::Format_ARGB32);
    QImage img_green = QImage(size,size,QImage::Format_ARGB32);
    QImage img_yellow = QImage(size,size,QImage::Format_ARGB32);

    int op_best = 170;
    int op_mid = op_best -20;
    int op_last = op_mid + 10;
    int op_white = 0;

    img_red.fill(QColor(255,255,255,op_white));
    img_yellow.fill(QColor(255,255,255,op_white));
    img_green.fill(QColor(255,255,255,op_white));

    int pixel_x =0, pixel_y=0;



    if(myMap->ready)
        for (int lat = myMap->get_index()-1; lat >=0 ; lat--)
        {



            for(int lon = myMap->get_index()-1; lon >= 0 ; lon--)
            {
                if(myMap->at(lat,lon) >= -75 )
                 img_red.setPixel(pixel_x,pixel_y,qRgba(255, 0, 0,op_best));
                else if(myMap->at(lat,lon)>= -85)
                    img_yellow.setPixel(pixel_x,pixel_y,qRgba(255, 255, 0,op_mid));
                else if(myMap->at(lat,lon)>= -92)
                    img_green.setPixel(pixel_x,pixel_y,qRgba(70, 255,0,op_last));
                //else img.setPixel(pixel_x,pixel_y, qRgba(255,255,255,op_white));


              pixel_x ++;
            }
            pixel_y++;
            pixel_x=0;
        }
    /*
    QString red_layer  = myValues->name +" " + QDate::currentDate().toString("ddMMyy").toStdString().c_str() +"_"+ QTime::currentTime().toString("hh:mm").toStdString().c_str() + "_red.png";

    QString yellow_layer  = myValues->name +" " + QDate::currentDate().toString("ddMMyy").toStdString().c_str() +"_"+ QTime::currentTime().toString("hh:mm").toStdString().c_str() + "_yellow.png";

    QString green_layer  = myValues->name +" " + QDate::currentDate().toString("ddMMyy").toStdString().c_str() +"_"+ QTime::currentTime().toString("hh:mm").toStdString().c_str() + "_green.png";

    */
    QString red_layer  = myValues->path_to_image + myValues->name  + "_red.png";

    QString yellow_layer  =myValues->path_to_image + myValues->name + "_yellow.png";

    QString green_layer  = myValues->path_to_image + myValues->name +"_green.png";


    img_red.save(red_layer);

    img_yellow.save(yellow_layer);

    img_green.save(green_layer);



}
