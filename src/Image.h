#ifndef PATHLOSS_MAP_H
#define PATHLOSS_MAP_H

#include <vector>

class Image
{
public:
    Image();


    void initialize_Map(int Range, int Resolution);

    double at(int n1, int n2);
    double height_at(int n1, int n2);
    void set(int n1, int n2, double value);
    void set_power(int n1, int n2, double power);


    int get_index();

    bool ready;


    std::vector <std::vector <double> > Power;

    int max_index;

};

#endif // PATHLOSS_MAP_H
