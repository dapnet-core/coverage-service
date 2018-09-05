#ifndef MAPPING_CPP
#define MAPPING_CPP 100

#include "Mapping.h"
#include "ASTER.c"

using namespace std;

double great_circle_distance(coord start, coord target) {
    //Parameter Coordinates in arcsec
    //Distance in meter
    //First change to radiant

    double lat1 = start.lat / 3600 * M_PI / 180;
    double lat2 = target.lat / 3600 * M_PI / 180;
    double lon1 = start.lon / 3600 * M_PI / 180;
    double lon2 = target.lon / 3600 * M_PI / 180;
    return 6370000 * acos(sin(lat1) * sin(lat2) + cos(lat1) * cos(lat2) * cos(lon2 - lon1));
}

long double meter2arcsec_lon(coord start, double dist_m) {
    double lat = start.lat / 3600 * M_PI / 180;
    long double help = acos((cos(dist_m / 6370000) - 1) / cos(lat) / cos(lat) + 1) * 180 / M_PI * 3600;
    return help;
}

double meter2arcsec_lat(coord start, double dist_m) {
    return dist_m / 6370000 * 180 / M_PI * 3600;
}

//Calculate down right corner as 3-arcsec/1-arcsec steps from mid
void down_right_corner(Height_Map &map1, coord station_coord) {

    map1.down_right_arcsec.lat = (station_coord.lat - map1.Range_grad_N / 2) * 3600;
    map1.down_right_arcsec.lon = (station_coord.lon + map1.Range_grad_E / 2) * 3600;

    //Auf Ganzzahligen Vielfachen von 3 Bogenskunden runden

    double help = 3600 * floor(map1.down_right_arcsec.lat / 3600);
    while ((help + map1.step * 1000) < map1.down_right_arcsec.lat) {
        help += map1.step * 1000;
    }
    while ((help + map1.step * 100) < (map1.down_right_arcsec.lat)) {
        help = help + map1.step * 100;
    }

    while ((help + map1.step * 10) < map1.down_right_arcsec.lat) {
        help += map1.step * 10;
    }
    while ((help + map1.step) < map1.down_right_arcsec.lat) {
        help += map1.step * 1;
    }
    map1.down_right_arcsec.lat = help;


    help = 3600 * floor(map1.down_right_arcsec.lon / 3600);
    while ((help + map1.step * 1000) < map1.down_right_arcsec.lon) {
        help += map1.step * 1000;
    }
    while ((help + map1.step * 100) < map1.down_right_arcsec.lon) {
        help += map1.step * 100;
    }
    while ((help + map1.step * 10) < map1.down_right_arcsec.lon) {
        help += map1.step * 10;
    }
    while ((help + map1.step) < map1.down_right_arcsec.lon) {
        help += map1.step;
    }

    //No lon-values greater 360 allowed
    if (help > 360 * 3600) {
        help = help - 360 * 3600;
    }
    map1.down_right_arcsec.lon = help;
}

//Calculate down left corner as 3-arcsec/1-arcsec steps from mid
void down_left_corner(Height_Map &map1, coord station_coord) {
    double help = map1.down_right_arcsec.lon;
    double down_left_lon = map1.down_right_arcsec.lon - map1.Range_grad_E * 3600;

    while ((help - map1.step * 1000) > down_left_lon) {
        help -= map1.step * 1000;
    }
    while ((help - map1.step * 100) > down_left_lon) {
        help = help - map1.step * 100;
    }

    while ((help - map1.step * 10) > down_left_lon) {
        help -= map1.step * 10;
    }
    while ((help - map1.step) > down_left_lon) {
        help -= map1.step;
    }

    //no negative lon-values
    if (help < 0) {
        help = help + 360 * 3600;
    }

    map1.down_left_arcsec.lon = help;
    map1.down_left_arcsec.lat = map1.down_right_arcsec.lat;
}

//Calculate up right corner as 3-arcsec/1-arcsec steps from mid
void up_right_corner(Height_Map &map1, coord station_coord) {
    double help = map1.down_right_arcsec.lat;
    double up_right_lat = map1.down_right_arcsec.lat + map1.Range_grad_N * 3600;

    while ((help + map1.step * 1000) < up_right_lat) {
        help += map1.step * 1000;
    }
    while ((help + map1.step * 100) < up_right_lat) {
        help = help + map1.step * 100;
    }
    while ((help + map1.step * 10) < up_right_lat) {
        help += map1.step * 10;
    }
    while ((help + map1.step) < up_right_lat) {
        help += map1.step;
    }
    map1.up_right_arcsec.lat = help;

    map1.up_right_arcsec.lon = map1.down_right_arcsec.lon;
}


map_index arcsec2index(Height_Map &map1, coord &arcsec) {
    map_index map_index1;
    /*
    Find a coordinates (lat,lon) in the array, starting at the actual down_right corner
    both indices are relative to the down right corner
    */
    if (arcsec.lat < map1.down_right_arcsec.lat || arcsec.lat > map1.up_right_arcsec.lat) {
        cout << endl << "Coordinate " << arcsec.lat << " " << arcsec.lon << " not in the range of the map." << endl;

        //exit;
    }

    int help = 0;
    map_index1.lat = 0;

    while ((help + map1.step * 1000) <= arcsec.lat - map1.down_right_arcsec.lat) {
        map_index1.lat += 1000;
        help += map1.step * 1000;
    }
    while ((help + map1.step * 100) <= arcsec.lat - map1.down_right_arcsec.lat) {
        map_index1.lat += 100;
        help += map1.step * 100;
    }

    while ((help + map1.step * 10) <= arcsec.lat - map1.down_right_arcsec.lat) {
        map_index1.lat += 10;
        help += map1.step * 10;
    }
    while ((help + map1.step) <= arcsec.lat - map1.down_right_arcsec.lat) {
        map_index1.lat += 1;
        help += map1.step;
    }


    // Changed from (map1.down_right_arcsec.lon - map1.Range_grad_E*3600) to avoid problems at 360/0 degrees
    double help2 = map1.down_right_arcsec.lon - map1.Range_grad_E * 3600;
    if (help2 < 0) {
        help2 = help2 + 360 * 3600;

        if (!((arcsec.lon >= help2 && arcsec.lon <= 360 * 3600) ||
              (arcsec.lon >= 0 && arcsec.lon <= map1.down_right_arcsec.lon))) {
            cout << endl << "Coordinate " << arcsec.lat << " " << arcsec.lon
                 << " not in the range of the map. Transition!" << endl;
        }
    } else if ((arcsec.lon <= help2) || arcsec.lon > map1.down_right_arcsec.lon) {
        cout << endl << "Coordinate " << arcsec.lat << " " << arcsec.lon << " not in the range of the map. Normal!"
             << endl;
    }

    help = 0;
    map_index1.lon = 0;

    //Uebergang 0->360-> ersetzbar ducrh halep = map1.down_right_arcsec.lon
    if (map1.down_right_arcsec.lon - arcsec.lon < 0) {
        /*  while( (help+map1.step*1000) <= map1.down_right_arcsec.lon)
          {
              map_index1.lon += 1000;
              help+=map1.step*1000;
          }
          while ((help+map1.step*100) <= ( map1.down_right_arcsec.lon))
          {
              map_index1.lon += 100;
              help+=map1.step*100;
          }

          while((help+map1.step*10) <= (map1.down_right_arcsec.lon))
          {
              map_index1.lon+=10;
              help+=map1.step*10;
          }
          while ((help+map1.step) <= (map1.down_right_arcsec.lon))
          {
              map_index1.lon+=1;
              help+=map1.step;
          }*/
        help = map1.down_right_arcsec.lon;

        //Run to zero finished;
        int help8 = 0;
        while ((help8 + map1.step * 1000) <= (360 * 3600 - arcsec.lon)) {
            help8 += map1.step * 1000;
        }
        while ((help8 + map1.step * 100) <= (360 * 3600 - arcsec.lon)) {
            help8 += map1.step * 100;
        }
        while ((help8 + map1.step * 10) <= (360 * 3600 - arcsec.lon)) {
            help8 += map1.step * 10;
        }
        while ((help8 + map1.step) <= (360 * 3600 - arcsec.lon)) {
            help8 += map1.step;
        }

        map_index1.lon = help + help8;

    } else {
        while ((help + map1.step * 1000) <= map1.down_right_arcsec.lon - arcsec.lon) {
            map_index1.lon += 1000;
            help += map1.step * 1000;
        }
        while ((help + map1.step * 100) <= (map1.down_right_arcsec.lon - arcsec.lon)) {
            map_index1.lon += 100;
            help += map1.step * 100;
        }
        while ((help + map1.step * 10) <= (map1.down_right_arcsec.lon - arcsec.lon)) {
            map_index1.lon += 10;
            help += map1.step * 10;
        }
        while ((help + map1.step) <= (map1.down_right_arcsec.lon - arcsec.lon)) {
            map_index1.lon += 1;
            help += map1.step;
        }
    }
    return map_index1;
}

void initialize_Map(Height_Map &map1, coord station_coord_grad, double Resolution) {
    //Dimensions of the map
    /*
        <---Range grad E--->
   Map: xxxxxxxxxxxxxxxxxxxx ^
        xxxxxxxxxxxxxxxxxxxx |
        xxxxxxxxxxxxxxxxxxxx |
        xxxxxxxxx0xxxxxxxxxx Range grad N
        xxxxxxxxxxxxxxxxxxxx |
        xxxxxxxxxxxxxxxxxxxx |
        xxxxxxxxxxxxxxxxxxxx ^

    Range is Calculated by a Meter2Grad Conversation for the most northern Row.


    the map does look like:
            <--Range_m-->
            xxxxxxxxxxxxx
          xxxxxxxxxxxxxxxxx
        xxxxxxxxxxxxxxxxxxxxx
      xxxxxxxxxxxxxxxxxxxxxxxxx
    xxxxxxxxxxxxxxxxxxxxxxxxxxxxx
  xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
            <--Range_m-->
   In the south there is some offset

    */
    map1.station_coord.lat = station_coord_grad.lat * 3600;
    map1.station_coord.lon = station_coord_grad.lon * 3600;

    double map_Range_with_offset = 2 * map1.Range_m + 4 * Resolution;

    map1.Range_grad_N = meter2arcsec_lat(map1.station_coord, 2 * map_Range_with_offset) / 3600;

    coord northern_row;
    northern_row.lat = map1.station_coord.lat + meter2arcsec_lat(map1.station_coord, map_Range_with_offset);
    northern_row.lon = map1.station_coord.lon;
    map1.Range_grad_E = meter2arcsec_lon(northern_row, map_Range_with_offset) / 3600;

    down_right_corner(map1, station_coord_grad);
    down_left_corner(map1, station_coord_grad);
    up_right_corner(map1, station_coord_grad);


    map1.station_index = arcsec2index(map1, map1.station_coord);

    //Initialize the Arrays for the Map height data
    int a = map1.Range_grad_N * 3600 / map1.step + 1;
    int b = map1.Range_grad_E * 3600 / map1.step + 1;

    short height = 0;

    vector<short> row_lon(b, height);
    map1.map_data.resize(a, row_lon);

    /*
       Map Dimensions: a x b (in arcsec)

   vector map_data contains the a rows with each length of b

          <--Lenght b in arcsec-->
          xxxxxxxxxxxxxxxxxxxxx ^
          xxxxxxxxxxxxxxxxxxxxx |
          xxxxxxxxxxxxxxxxxxxxx |
          xxxxxxxxxx0xxxxxxxxxx Length a in arcsec
          xxxxxxxxxxxxxxxxxxxxx |
          xxxxxxxxxxxxxxxxxxxxx |
          xxxxxxxxxxxxxxxxxxxxx ^

    1 step in srtm[][] is equivalent to 3 arcsec
    1 step in aster[][] is equivalent to 1 arcsec

   */
}


fstream *FindDEM(Height_Map &map1, coord &down_right_arcsec) {

    fstream *fs = new fstream;
    /*
       Fuer die aktuelle unterste Ecke muss nach der .dem1/.dem3 Datei gesucht werden
       Format der .dem Dateien:
       sprintf(dem_filename, "%dx%dx%dx%d.demx", min_north, max_north, min_west, max_west);
    */

    char dem_filename[50] = "";
    char path_dem[100] = "";
    char str_help[20] = "";

    strcpy(path_dem, map1.path_to_dem);

//+-3 because of the edge case when you got an integer degree you should take the next .dem;
    int min_north_grad = floor((down_right_arcsec.lat + map1.step) / 3600);
    int max_north_grad = min_north_grad + 1;
    int min_east_grad = floor((down_right_arcsec.lon - map1.step) / 3600);
    int max_east_grad = min_east_grad + 1;

    if (min_east_grad < 0) {
        min_east_grad = min_east_grad + 360;
        max_east_grad = min_east_grad + 1;
    }
    if (max_east_grad > 360) {
        max_east_grad = max_east_grad - 360;
        min_east_grad = max_east_grad - 1;
    }

    sprintf(str_help, "%i", min_north_grad);
    strcat(dem_filename, str_help);
    strcat(path_dem, str_help);

    strcat(dem_filename, "x");
    strcat(path_dem, "x");
    sprintf(str_help, "%i", max_north_grad);
    strcat(dem_filename, str_help);
    strcat(path_dem, str_help);

    strcat(dem_filename, "x");
    strcat(path_dem, "x");
    sprintf(str_help, "%i", min_east_grad);
    strcat(dem_filename, str_help);
    strcat(path_dem, str_help);

    strcat(dem_filename, "x");
    strcat(path_dem, "x");
    sprintf(str_help, "%i", max_east_grad);
    strcat(dem_filename, str_help);
    strcat(path_dem, str_help);

    strcat(path_dem, ".dem1");

    fs->open(path_dem, ifstream::in);

    fprintf(stdout, "Loading %s\n", dem_filename);

    fflush(stdout);

    if (!(fs->is_open())) {
        // Holds the path to ASTER file of be read
        char path[100] = "";
        cout << "File " << dem_filename << " " << " not found" << endl;
        cout << "Try to evaluate .dem file from HGT/ASTER file in path" << endl;

        // Build Filename of ASTER file in variable path
        if (map1.step == 1) {
            strcpy(path, map1.path_to_tif);
            strcat(path, "ASTGTM2_");

            // This works just on the nothern hemisphere
            strcat(path, "N");
// TODO Refactor to automated leading zero padding
            if (min_north_grad >= 10) {
                sprintf(str_help, "%i", min_north_grad);
            } else {
                sprintf(str_help, "0%i", min_north_grad);
            }
            strcat(path, str_help);
// TODO Refactor to automated leading zero padding
            if (min_east_grad <= 180 && min_east_grad >= 0) {
                strcat(path, "E");
                if (min_east_grad >= 100)
                    sprintf(str_help, "%i", min_east_grad);
                if (min_east_grad >= 10 && min_east_grad < 100)
                    sprintf(str_help, "0%i", min_east_grad);
                if (min_east_grad < 10)
                    sprintf(str_help, "00%i", min_east_grad);

                strcat(path, str_help);

            } else if (min_east_grad > 180 && min_east_grad < 360) {
                int max_west_grad = 360 - min_east_grad;
                strcat(path, "W");
// TODO Refactor to automated leading zero padding
                if (max_west_grad >= 100)
                    sprintf(str_help, "%i", max_west_grad);
                if (max_west_grad >= 10 && max_west_grad < 100)
                    sprintf(str_help, "0%i", max_west_grad);
                if (max_west_grad < 10)
                    sprintf(str_help, "00%i", max_west_grad);

                strcat(path, str_help);
            }
//TODO define somewhere else, as it's a NASA definition
            strcat(path, "_dem.tif");

            if (ASTER(path, path_dem)) {
                cout << "Successful Translation from GeoTiff to .dem" << endl;
                cout << ".dem File now available in directory" << endl << endl;
                fs = FindDEM(map1, down_right_arcsec);
            } else {
                cout << "Error: .dem File could not be created." << endl;

                char date[20] = "";
                strcat(date, QDate::currentDate().toString("ddMMyy").toStdString().c_str());
                strcat(date, "_");
                strcat(date, QTime::currentTime().toString("hh:mm").toStdString().c_str());

                fstream *missing = new fstream;

                missing->open("missingHeightData.txt", ios::out | ios::app);
                *missing << date << ":  " << dem_filename << " " << path << endl;
                missing->close();

                return 0;
                abort();
            }

        }
    }
    return fs;
}

int ReadDEM(Height_Map &map, coord &down_right_arcsec) {
/*
down_right_arcsec is the actual down right corner from wich on the demx file shall be read

In the first run, down_right_arcsec is similar to the same named map-Variable; for later calls
(if necessary) it will be different to get data from other demx-files
*/
    fstream *fs = FindDEM(map, down_right_arcsec);

    if (fs == 0) {
        return 0;
    }

//Load data from demx
    int next_value;
    char line[10] = " ";
    int max_north, max_east, min_north, min_east;


//Read the first 4 Lines in .dem data files

    fs->getline(line, 10);
    sscanf(line, "%d", &min_east);

    fs->getline(line, 10);
    sscanf(line, "%d", &min_north);

    fs->getline(line, 10);
    sscanf(line, "%d", &max_east);

    fs->getline(line, 10);
    sscanf(line, "%d", &max_north);

    if (fs->fail()) {
        return 0;

    }

    //Search the actual down_right corner in .dem file
    float position_lat = down_right_arcsec.lat - min_north * 3600;
    float position_lon = 3600 * max_east - down_right_arcsec.lon;

    position_lat = position_lat / map.step;
    position_lon = position_lon / map.step;

    //position_lon/lat: times you need to add +-map.step arcsec to get from end of the .dem file to the down right corner of the map

    //Skip the filstream until position_lat/lon
    for (int i = position_lat; i >= 1; i--) {
        fs->ignore(1000000000, '\n');
    }
    for (int y = position_lon; y >= 1; y--) {
        (*fs) >> next_value;
        fs->ignore(1);
    }

    //At this point the filestream is at the position to read data from the down right corner into the map vector
    bool end_of_dem_lat = 0;
    bool end_of_map_lon = 0;
    bool end_of_map_lat = 0;

    //Start in the down right corner and move row by row through the .dem file
    map_index map_arcsec2index = arcsec2index(map, down_right_arcsec);
    int counter_lat = map_arcsec2index.lat;
    while (!end_of_map_lat && !end_of_dem_lat) {
        int counter_lon = map_arcsec2index.lon;
        while (!end_of_map_lon) {
            (*fs) >> next_value;
            fs->ignore(1); //Skip the following whitespace
            map.map_data.at(counter_lat).at(counter_lon) = next_value;

            //Is the next step still in the range of the map?
            int fileend = 0;
            if (map.down_right_arcsec.lon - map.Range_grad_E * 3600 < 0) {// Karte mit �bergang 0->360
                if ((map.down_right_arcsec.lon - (counter_lon + 1) * map.step + 360 * 3600) <=
                    map.down_left_arcsec.lon) {
                    fileend = 1;
                }
            } else if (map.down_right_arcsec.lon - (counter_lon + 1) * map.step <= map.down_left_arcsec.lon) {
                fileend = 1;
            }
            if (fileend == 1) {
                //If the end of map is reached, still read the next value and write it to the vector-array
                counter_lon += 1;
                (*fs) >> next_value;
                fs->ignore(1); //Skip the following whitespace
                map.map_data.at(counter_lat).at(counter_lon) = next_value;

                end_of_map_lon = 1;
                fs->ignore(100000000, '\n');

                for (int y = position_lon; y >= 1; y--) {
                    (*fs) >> next_value;
                    fs->ignore(1);
                }
                continue;
            }

            //Is the next step still in range of the .dem?
            int demend = 0;
            if (map.down_right_arcsec.lon - map.Range_grad_E * 3600 < 0) {
                coord zero;
                zero.lon = 0;
                zero.lat = down_right_arcsec.lat;

                if (min_east * 3600 < map.down_right_arcsec.lon) {
                    if (map.down_right_arcsec.lon - (counter_lon + 1) * map.step <= min_east * 3600) { demend = 1; }
                } else if (360 * 3600 - (counter_lon + 1) * map.step + arcsec2index(map, zero).lon * map.step <=
                           min_east * 3600) { demend = 1; }
            } else if (map.down_right_arcsec.lon - (counter_lon + 1) * map.step <= min_east * 3600) { demend = 1; }
            if (demend == 1) {

                //If the end of .dem file is reached -> look for new file;
                //set new down right corner to the corner of the new .dem

                coord new_down_right_arcsec;
                new_down_right_arcsec.lat = map.down_right_arcsec.lat;
                new_down_right_arcsec.lon = map.down_right_arcsec.lon - (counter_lon + 1) * map.step;
                if (new_down_right_arcsec.lon == 0) { new_down_right_arcsec.lon += 360 * 3600; }
                if (ReadDEM(map, new_down_right_arcsec) == 0) { return 0; }
                //After finishing the most left part of the map-> set new border
                map.down_left_arcsec.lon = new_down_right_arcsec.lon + map.step;
                end_of_map_lon = 1;
                //Ignore the \n symbol at the end of the last row
                fs->ignore(1000000, '\n');
                //Skip in file until the needed value
                for (int y = position_lon; y >= 1; y--) {
                    (*fs) >> next_value;
                    fs->ignore(1);
                }
            }
            counter_lon++;
        }

        //Is the next row still in Map?
        if (map.down_right_arcsec.lat + (counter_lat + 1) * map.step > map.up_right_arcsec.lat) {
            end_of_map_lat = 1;
            continue;
        }

        //Is the next row still in this .dem?
        if (map.down_right_arcsec.lat + (counter_lat + 1) * map.step >= (max_north * 3600)) {

            coord new_down_right_arcsec;
            new_down_right_arcsec.lat = map.down_right_arcsec.lat + (counter_lat + 1) * map.step;
            new_down_right_arcsec.lon = down_right_arcsec.lon;
            if (ReadDEM(map, new_down_right_arcsec) == 0) {
                return 0;
            }
            end_of_dem_lat = 1;
        }
        counter_lat++;
        end_of_map_lon = 0;
    }
    fs->close();
    return 1;
}

double *Path(Height_Map &map, coord pixel_arcsec, double &alpha, double &receiver_height) {
    map_index pixel_index = arcsec2index(map, pixel_arcsec);

    double step_width = 1;

    //Calculate gradients, denum is the number of necessary steps in the vector array
    double denum = abs(map.station_index.lat - pixel_index.lat) + abs(map.station_index.lon - pixel_index.lon);

    double m_lat = (pixel_index.lat - map.station_index.lat) / denum;
    double m_lon = (pixel_index.lon - map.station_index.lon) / denum;

    map_index help_index;
    help_index = map.station_index;

    double current_step_sum_lat = 0.0;
    double current_step_sum_lon = 0.0;

    double *heights = new double[(int) denum + 3];

    heights[0] = round(denum) - 1; // [number of points -1]

    //Aproximate the distance per step_width with gradients

    heights[1] = great_circle_distance(map.station_coord, pixel_arcsec) / denum;

    //Transmitter height
    heights[2] = map.map_data.at(help_index.lat).at(help_index.lon);

    int i = 1;

    while (help_index.lat != pixel_index.lat || help_index.lon != pixel_index.lon) {
        i++;
        if (help_index.lat != pixel_index.lat) {
            current_step_sum_lat += step_width * m_lat;
            if (current_step_sum_lat >= step_width) {
                current_step_sum_lat -= step_width;
                help_index.lat += step_width;
            }
            if (current_step_sum_lat <= -step_width) {
                current_step_sum_lat += step_width;
                help_index.lat -= step_width;
            }
        }
        if (help_index.lon != pixel_index.lon) {
            current_step_sum_lon += step_width * m_lon;
            if (current_step_sum_lon >= step_width) {
                current_step_sum_lon -= step_width;
                help_index.lon += step_width;
            }
            if (current_step_sum_lon <= -step_width) {
                current_step_sum_lon += step_width;
                help_index.lon -= step_width;
            }
        }
        heights[i] = map.map_data.at(help_index.lat).at(help_index.lon);
        receiver_height = map.map_data.at(help_index.lat).at(help_index.lon);
    }

    //alpha is the horizontal angle relative to the north axis
    //alpha is in grad
    if (m_lat >= 0 && m_lon <= 0) {
        alpha = 180 / M_PI * std::asin(abs(pixel_arcsec.lon - map.station_coord.lon) /
                                             (abs(pixel_arcsec.lon - map.station_coord.lon) +
                                              abs(pixel_arcsec.lat - map.station_coord.lat)));
    } else if (m_lat < 0 && m_lon <= 0) {
        alpha = 180 - 180 / M_PI * std::asin(abs(pixel_arcsec.lon - map.station_coord.lon) /
                                                   (abs(pixel_arcsec.lon - map.station_coord.lon) +
                                                    abs(pixel_arcsec.lat - map.station_coord.lat)));
    } else if (m_lat < 0 && m_lon > 0) {
        alpha = 180 + 180 / M_PI * std::asin(abs(pixel_arcsec.lon - map.station_coord.lon) /
                                                   (abs(pixel_arcsec.lon - map.station_coord.lon) +
                                                    abs(pixel_arcsec.lat - map.station_coord.lat)));
    } else if (m_lat >= 0 && m_lon > 0) {
        alpha = 360 - 180 / M_PI * std::asin(abs(pixel_arcsec.lon - map.station_coord.lon) /
                                                   (abs(pixel_arcsec.lon - map.station_coord.lon) +
                                                    abs(pixel_arcsec.lat - map.station_coord.lat)));
    }
    return heights;
}

int mapping(Height_Map &map1, coord &station_coord_grad, double Resolution) {
    initialize_Map(map1, station_coord_grad, Resolution);
    if (ReadDEM(map1, map1.down_right_arcsec) == 0) {
        //No map could be created (e.g. No matching ASTER files available ?)
        return 0;
    }
    return 1;
}

#endif
