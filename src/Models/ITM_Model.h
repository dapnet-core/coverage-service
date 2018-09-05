#ifndef ITM_algorithm_h
#define ITM_algorithm_h

#include <complex>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <complex.h>
#include <iostream>










#define THIRD  (1.0/3.0)
 struct prop_type
{

    //Some of these are redundant!!!!
    double aref; //  attenuation relative to FSL
    double dist; //dist in m
    double hg[2];//antenna heights in m
    double wn; // wave number calculated in ITM
    double dh; //delta_h
    double ens; //N_s
    double gme; //gamma_e
    double zgndreal; //Surface Impedence
    double zgndimag;
    double he[2];  // effective heights
    double dl[2];  // horizon distances
    double the[2];  //horizon angles
    int mdp;  //mode; -1->point to point


    double eps_dielect, conductivity, Ns;
    double f_MHz;
    int radio_climate;
    int polarization;
    double rel;
    double conf;
    double dbloss;

    double tht_km;
    double rht_km;
    double distance_km;

    double reflection_height;
    double terrain_height[2];

    //(Map Boundaries
    double north;
    double south;
    double east;
    double west;

};

 struct propv_type
{
  double sgc;
  int lvar;
  int mdvar;
  int klim;
};





struct tcomplex
 { double tcreal;
   double tcimag;
 };


struct propa_type
{ double dlsa;
  double dx;
  double ael;
  double ak1;
  double ak2;
  double aed;
  double emd;
  double aes;
  double ems;
  double dls[2];
  double dla;
  double tha;
};


class ITM
{

    public:

    /***static variables from ITM-Algorithm***/

    //adiff
    double wd1, xd1, afo, qk, aht, xht;
    //ascat
    double ad, rr, etq, h0s;
    //alos
    double wls;
    //lrprop
    bool wlos, wscat;
    double dmin, xae;


    ITM();
    double  point_to_point(double elev[], propv_type &propv, prop_type &prop);
    double  adiff( double d, prop_type &prop, propa_type &propa);
    double  ascat( double d, prop_type &prop, propa_type &propa);
    double  alos( double d, prop_type &prop, propa_type &propa);
    void    lrprop (double d,prop_type &prop, propa_type &propa);
};

using namespace std;
int mymin(const int &i, const int &j);

int mymax(const int &i, const int &j);

double mymin(const double &a, const double &b);

double mymax(const double &a, const double &b);

double FORTRAN_DIM(const double &x, const double &y);

double aknfe(const double &v2);

double fht(const double& x, const double& pk);

double h0f(double r, double et);

double ahd(double td);

double  qerfi( double q );

void qlrps(double en0, double elev[],
          int ipol, double eps, double sgm, prop_type &prop);

double abq_alos (complex<double> r);

double curve (double const &c1, double const &c2, double const &x1,
              double const &x2, double const &x3, double const &de);
double avar(double zzt, double zzl, double zzc,
         prop_type &prop, propv_type &propv);

void hzns (double pfl[], prop_type &prop);

void z1sq1 (prop_type &prop,double z[], const double &x1, const double &x2,
            double& z0, double& zn);
double qtile (const int &nn, double a[], const int &ir);

double qerf(const double &z);

double d1thx(prop_type &prop, double pfl[], const double &x1, const double &x2);

void  qlrpfl( double pfl[], int klimx, int mdvarx,
        prop_type &prop, propv_type &propv );

double deg2rad(double d);





#endif





