
#include "TR_Model.h"

#include <math.h>
#include <string>
#include <iostream>


double TR_pathloss_dB(prop_type &prop, propv_type &propv)
{

   double h_TR1 = abs(prop.hg[0] + prop.terrain_height[0] - prop.reflection_height);
   double h_TR2 = abs(prop.hg[1] + prop.terrain_height[1] - prop.reflection_height);


   double pathloss_dB_TR, pathloss_dB_FSL,pathloss_dB_Mix;



pathloss_dB_TR=120+ 40*log10(prop.distance_km)-20*log10(h_TR1)-20*log10(h_TR2);

pathloss_dB_FSL= 20*log10(prop.distance_km)+20*log10(prop.f_MHz)+32.44;



//Compare TR to FSL-> take the bigger one
if (pathloss_dB_FSL>pathloss_dB_TR  || h_TR1 ==0 || h_TR2 == 0)
{
   pathloss_dB_Mix = pathloss_dB_FSL;
}
else
{
   pathloss_dB_Mix=pathloss_dB_TR;
}




prop.aref=pathloss_dB_Mix-pathloss_dB_FSL;

double zc = qerfi(prop.conf);
double zr = qerfi(prop.rel);


//ITM avar function for calculating statistical attenuation
pathloss_dB_Mix = avar(zr,0.0,zc,prop,propv) + pathloss_dB_FSL;


return pathloss_dB_Mix;




}



