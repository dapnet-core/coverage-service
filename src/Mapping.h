
#ifndef MAPPING_H_INCLUDED
#define MAPPING_H_INCLUDED

#include <iostream>
#include <math.h>
#include <fstream>
#include <vector>
#include <string.h>
#include <cstdlib>
#include <cmath>
#include <QDate>
#include <QTime>

using namespace std;


struct coord
{
    double lat;
    double lon;
};
struct map_index
{
    int lon;
    int lat;
};
struct Height_Map
{
    float Range_m;
    float Range_grad_N;
    float Range_grad_E;

    coord station_coord;
    map_index station_index;

    char path_to_dem[100];
    char path_to_hgt[100];
    char path_to_tif[100];

    vector< vector<short> > map_data;

    coord down_right_arcsec; //fixed
    coord down_left_arcsec; //variable
    coord up_right_arcsec;  //variable

    //ASTER (1) or SRTM (3) elevation model?
    int step;

};




double great_circle_distance(coord start, coord target);

long double meter2arcsec_lon( coord start, double dist_km);
double meter2arcsec_lat(coord start, double dist_km);

//Calculate down right corner as 3-arcsec/1-arcsec steps from mid
void down_right_corner(Height_Map &map1, coord station_coord);
//Calculate down left corner as 3-arcsec/1-arcsec steps from mid
void down_left_corner(Height_Map &map1, coord station_coord);
//Calculate up right corner as 3-arcsec/1-arcsec steps from mid
void up_right_corner(Height_Map &map1, coord station_coord);

map_index arcsec2index(Height_Map &map1, coord &arcsec);

void initialize_Map (Height_Map &map1, coord station_coord);

fstream* FindDEM(Height_Map& map1, coord& down_right_arcsec);

int ReadDEM(Height_Map &map, coord& down_right_arcsec, double Resolution);

double* Path (Height_Map &map, coord pixel_arcsec, double &alpha, double &receiver_height);

int mapping(Height_Map & map1, coord & station_coord_grad, double Resolution);
#endif // MAPPING_H_INCLUDED
