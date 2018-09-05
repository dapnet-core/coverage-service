#include "Image.h"

Image::Image()
{
    ready = false;

}
void Image::initialize_Map(int Range, int Resolution)
{

    max_index = 2*(Range/Resolution);

    std::vector <double> row_lon(max_index + 1);

    Power.resize(max_index +1, row_lon);

}
double Image::at(int n1, int n2)
{
   return Power.at(n1).at(n2);
}
void Image::set(int n1, int n2, double value)
{
    Power.at(n1).at(n2)= value;
}
int Image::get_index()
{
    return max_index;
}
void Image::set_power(int n1, int n2, double power)
{
   Power.at(n1).at(n2) = power;
}





