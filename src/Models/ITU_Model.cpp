#include "ITU_Model.h"


double itu_pathloss_dB(prop_type &prop, propv_type& propv, double h_F1_sign_Obstr)
{

double FSL_pathloss_dB= 20*log10(prop.distance_km)+20*log10(prop.f_MHz)+32.44;

prop.aref=-20*h_F1_sign_Obstr +10;

double pathloss_dB_itu=FSL_pathloss_dB + prop.aref;


double zc = qerfi(prop.conf);
double zr = qerfi(prop.rel);


//ITM avar function to calculate the statistical attenuation
pathloss_dB_itu = avar(zr,0.0,zc,prop,propv) + FSL_pathloss_dB;


  return pathloss_dB_itu;

}

