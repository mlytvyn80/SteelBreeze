/*
 *
 * This file is part of SteelBreeze.
 *
 * SteelBreeze, the geodetic VLBI data analysing software.
 * Copyright (C) 1998-2003 Sergei Bolotin, MAO NASU, Kiev, Ukraine.
 *
 * SteelBreeze is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * SteelBreeze is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with SteelBreeze; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 */

#include <math.h>

#include "SbGeneral.H"
#include "SbGeoRefraction.H"
#include "SbGeoProject.H"
#include "SbGeoStations.H"
#include "SbGeoSources.H"
#include "SbGeoRefFrame.H"
#include "SbSetup.H"
#include "SbMathSpline.H"




/*==============================================================================================*/
/*												*/
/* constants description									*/
/*												*/
/*==============================================================================================*/
const double SBZenithDelay::K1 = 77.604;	// K/mbar
const double SBZenithDelay::K2 = 64.79;		// K/mbar
const double SBZenithDelay::K3 = 3.776e+5;	// K/mbar^2
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBZenithDelay implementation								*/
/*												*/
/*==============================================================================================*/
double SBZenithDelay::vapourPress(float T, float RH)
{
  /**J.A.Estefan, O.J.Sovers, "A Comparative Survey of Current and Proposed Tropospheric
   * Refraction-Delay Models for DSN Radio Metric Data Calibration", JPL Publication 94-24,
   * October 1994. Page 14, Eq. (16): 
   * "...a variation of the simple model of Bean and Dutton [1966]:"
   *
   */                  
  return 0.0611*RH*exp(17.26938819745534263007*T/(237.3 + T));
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBHZD_Hopfield implementation								*/
/*												*/
/*==============================================================================================*/
/**Calcs hydrostatic zenith delay.
 * \param T  is an atmospheric temperature, C;
 * \param P  is an atmospheric pressure, mbar;
 * \param RH is an atmospheric relative humidity, %;
 * \param Station is a pointer to the SBStation.
 * \return hydrostatic zenith delay, m.
 */
double SBHZD_Hopfield::operator()(float T, float P, float /*RH*/, const SBStation* /*Station*/)
{
  /**C.D. de Jong, "GPS - Satellite orbits and atmospheric effects".
   * Delft University of Technology. 
   * Reports of the faculty of geodetic engineering
   * mathematical and physical geodesy.
   * Delft, The Netherlands, February, 1991.
   * Page 57, Eqs. (3.35) -- (3.36): 
   *
   */
  return 1.0e-6*k1()*P/(T + 273.15)*148.98*(T + 273.15 - 4.12)/5.0;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBHZD_Saastamoinen implementation							*/
/*												*/
/*==============================================================================================*/
/**Calcs hydrostatic zenith delay.
 * \param T  is an atmospheric temperature, C;
 * \param P  is an atmospheric pressure, mbar;
 * \param RH is an atmospheric relative humidity, %;
 * \param Station is a pointer to the SBStation.
 * \return hydrostatic zenith delay, m.
 */
double SBHZD_Saastamoinen::operator()(float /*T*/, float P, float /*RH*/, const SBStation *Station)
{
  return 2.2768e-3*P/(1.0 - 2.66e-3*cos(2.0*Station->latitude()) - 2.8e-7*Station->height());
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBWZD_Hopfield implementation								*/
/*												*/
/*==============================================================================================*/
/**Calcs wet zenith delay.
 * \param T  is an atmospheric temperature, C;
 * \param P  is an atmospheric pressure, mbar;
 * \param RH is an atmospheric relative humidity, %;
 * \param Station is a pointer to the SBStation.
 * \return wet zenith delay, m.
 */
double SBWZD_Hopfield::operator()(float T, float /*P*/, float RH, const SBStation *Station)
{
  /**C.D. de Jong, "GPS - Satellite orbits and atmospheric effects".
   * Delft University of Technology. 
   * Reports of the faculty of geodetic engineering
   * mathematical and physical geodesy.
   * Delft, The Netherlands, February, 1991.
   * Page 59, Eq. (3.38): 
   *
   */
  return 
    1.0e-6*(k3() + 273.0*(k2() - k1()))*vapourPress(T, RH)/(T + 273.15)*(10185.0 - Station->height())/5.0e+3;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBWZD_Saastamoinen implementation							*/
/*												*/
/*==============================================================================================*/
/**Calcs wet zenith delay.
 * \param T  is an atmospheric temperature, C;
 * \param P  is an atmospheric pressure, mbar;
 * \param RH is an atmospheric relative humidity, %;
 * \param Station is a pointer to the SBStation.
 * \return wet zenith delay, m.
 */
double SBWZD_Saastamoinen::operator()(float T, float /*P*/, float RH, const SBStation* /*Station*/)
{
  return 2.2770e-3*(0.05 + 1255.0/(T + 273.15))*vapourPress(T, RH);
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBHMf_Chao implementation								*/
/*												*/
/*==============================================================================================*/
/**Calcs hydrostatic mapping function.
 * \param E  is a source elevation angle, rad;
 * \param T  is an atmospheric temperature, C;
 * \param P  is an atmospheric pressure, mbar;
 * \param RH is an atmospheric relative humidity, %;
 * \param Station is a pointer to the SBStation;
 * \param MJD is a epoch of observation.
 * \return hydrostatic mapping function value.
 */
double SBHMf_Chao::operator()(double E, float /*T*/, float /*P*/, float /*RH*/, 
			      const SBStation* /*Station*/, const SBMJD&)
{
  return 1.0/(sin(E) + 0.00143/(tan(E) + 0.0445));
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBWMf_Chao implementation								*/
/*												*/
/*==============================================================================================*/
/**Calcs wet mapping function.
 * \param E  is a source elevation angle, rad;
 * \param T  is an atmospheric temperature, C;
 * \param P  is an atmospheric pressure, mbar;
 * \param RH is an atmospheric relative humidity, %;
 * \param Station is a pointer to the SBStation;
 * \param MJD is a epoch of observation.
 * \return wet mapping function value.
 */
double SBWMf_Chao::operator()(double E, float /*T*/, float /*P*/, float /*RH*/, 
			      const SBStation* /*Station*/, const SBMJD&)
{
  return 1.0/(sin(E) + 0.00035/(tan(E) + 0.017));
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBMf_CfA2p2 implementation								*/
/*												*/
/*==============================================================================================*/
/**Calcs hydrostatic mapping function.
 * \param E  is a source elevation angle, rad;
 * \param T  is an atmospheric temperature, C;
 * \param P  is an atmospheric pressure, mbar;
 * \param RH is an atmospheric relative humidity, %;
 * \param Station is a pointer to the SBStation;
 * \param MJD is a epoch of observation.
 * \return hydrostatic mapping function value.
 */
double SBHMf_CfA2p2::operator()(double E, float T, float P, float RH, 
				const SBStation* /*Station*/, const SBMJD&)
{
  double e0 = SBZenithDelay::vapourPress(T, RH);
  double a =  0.001185*(1.0 + 0.6071e-4*(P - 1000.0) - 0.1471e-3*e0 + 0.3072e-2*(T-20.0)); //
  double b =  0.001144*(1.0 + 0.1164e-4*(P - 1000.0) + 0.2795e-3*e0 + 0.3109e-2*(T-20.0)); //
  double c = -0.0090;
  double sinE = sin(E);
  return 1.0/(sinE + a/(tan(E) + b/(sinE + c)));
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBHMf_Ifadis implementation								*/
/*												*/
/*==============================================================================================*/
/**Calcs hydrostatic mapping function.
 * \param E  is a source elevation angle, rad;
 * \param T  is an atmospheric temperature, C;
 * \param P  is an atmospheric pressure, mbar;
 * \param RH is an atmospheric relative humidity, %;
 * \param Station is a pointer to the SBStation;
 * \param MJD is a epoch of observation.
 * \return hydrostatic mapping function value.
 */
double SBHMf_Ifadis::operator()(double E, float T, float P, float RH, 
				const SBStation* /*Station*/, const SBMJD&)
{
  double e0s = sqrt(SBZenithDelay::vapourPress(T, RH));
  double a = 0.001237 + 1.316e-7*(P - 1000.0) + 1.378e-6*(T - 15.0) + 8.057e-6*e0s;
  double b = 0.003333 + 1.946e-7*(P - 1000.0) + 1.040e-6*(T - 15.0) + 1.747e-5*e0s;
  double c = 0.078;
  double sinE = sin(E);
  return (1.0 + a/(1.0 + b/(1.0 + c)))/(sinE + a/(sinE + b/(sinE + c)));
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBWMf_Ifadis implementation								*/
/*												*/
/*==============================================================================================*/
/**Calcs wet mapping function.
 * \param E  is a source elevation angle, rad;
 * \param T  is an atmospheric temperature, C;
 * \param P  is an atmospheric pressure, mbar;
 * \param RH is an atmospheric relative humidity, %;
 * \param Station is a pointer to the SBStation;
 * \param MJD is a epoch of observation.
 * \return wet mapping function value.
 */
double SBWMf_Ifadis::operator()(double E, float T, float P, float RH, 
				const SBStation* /*Station*/, const SBMJD&)
{
  double e0s = sqrt(SBZenithDelay::vapourPress(T, RH));
  double a = 0.0005236 +  2.471e-7*(P - 1000.0) - 1.724e-7*(T - 15.0) + 1.328e-5*e0s;
  double b = 0.001705  +  7.384e-7*(P - 1000.0) + 3.767e-7*(T - 15.0) + 2.147e-5*e0s;
  double c = 0.05917;
  double sinE = sin(E);
  return (1.0 + a/(1.0 + b/(1.0 + c)))/(sinE + a/(sinE + b/(sinE + c)));
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBHMf_MTT implementation								*/
/*												*/
/*==============================================================================================*/
/**Calcs hydrostatic mapping function.
 * \param E  is a source elevation angle, rad;
 * \param T  is an atmospheric temperature, C;
 * \param P  is an atmospheric pressure, mbar;
 * \param RH is an atmospheric relative humidity, %;
 * \param Station is a pointer to the SBStation;
 * \param MJD is a epoch of observation.
 * \return hydrostatic mapping function value.
 */
double SBHMf_MTT::operator()(double E, float T, float /*P*/, float /*RH*/, 
			     const SBStation *Station, const SBMJD&)
{
  double cosPhi = cos(Station->latitude());
  /**J.A.Estefan, O.J.Sovers, "A Comparative Survey of Current and Proposed Tropospheric
   * Refraction-Delay Models for DSN Radio Metric Data Calibration", JPL Publication 94-24,
   * October 1994. Page 26, Eq. (48): 
   *
   */
  double Hs = Station->height()/1000.0; // height in km
  double a = ( 1.2320 + 0.0139*cosPhi - 0.0209*Hs + 0.00215*(T - 10.0))*1.0e-3;
  double b = ( 3.1612 - 0.1600*cosPhi - 0.0331*Hs + 0.00206*(T - 10.0))*1.0e-3;
  double c = (71.244  - 4.293 *cosPhi - 0.149 *Hs - 0.0021 *(T - 10.0))*1.0e-3;

  double sinE = sin(E);
  return (1.0 + a/(1.0 + b/(1.0 + c)))/(sinE + a/(sinE + b/(sinE + c)));
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBWMf_MTT implementation								*/
/*												*/
/*==============================================================================================*/
/**Calcs wet mapping function.
 * \param E  is a source elevation angle, rad;
 * \param T  is an atmospheric temperature, C;
 * \param P  is an atmospheric pressure, mbar;
 * \param RH is an atmospheric relative humidity, %;
 * \param Station is a pointer to the SBStation;
 * \param MJD is a epoch of observation.
 * \return wet mapping function value.
 */
double SBWMf_MTT::operator()(double E, float T, float /*P*/, float /*RH*/, 
			     const SBStation *Station, const SBMJD&)
{
  double cosPhi = cos(Station->latitude());
  /**J.A.Estefan, O.J.Sovers, "A Comparative Survey of Current and Proposed Tropospheric
   * Refraction-Delay Models for DSN Radio Metric Data Calibration", JPL Publication 94-24,
   * October 1994. Page 26, Eq. (49): 
   *
   */
  double Hs = Station->height()/1000.0; // height in km
  double a = ( 0.583  - 0.011 *cosPhi - 0.052 *Hs + 0.0014*(T - 10.0))*1.0e-3;
  double b = ( 1.402  - 0.102 *cosPhi - 0.101 *Hs + 0.0020*(T - 10.0))*1.0e-3;
  double c = (45.85   - 1.91  *cosPhi - 1.29  *Hs + 0.015 *(T - 10.0))*1.0e-3;

  double sinE = sin(E);
  return (1.0 + a/(1.0 + b/(1.0 + c)))/(sinE + a/(sinE + b/(sinE + c)));
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBHMf_NMF implementation								*/
/*												*/
/*==============================================================================================*/
const double SBHMf_NMF::ModelArg[5]= 
{ 15.0*DEG2RAD, 30.0*DEG2RAD, 45.0*DEG2RAD, 60.0*DEG2RAD, 75.0*DEG2RAD};
const double SBHMf_NMF::ModelA_avg[5]=
{ 1.2769934e-3, 1.2683230e-3, 1.2465397e-3, 1.2196049e-3, 1.2045996e-3};
const double SBHMf_NMF::ModelB_avg[5]=
{ 2.9153695e-3, 2.9152299e-3, 2.9288445e-3, 2.9022565e-3, 2.9024912e-3};
const double SBHMf_NMF::ModelC_avg[5]=
{ 62.610505e-3, 62.837393e-3, 63.721774e-3, 63.824265e-3, 64.258455e-3};
const double SBHMf_NMF::ModelA_amp[5]=
{ 0.0, 1.2709626e-5, 2.6523662e-5, 3.4000452e-5, 4.1202191e-5};
const double SBHMf_NMF::ModelB_amp[5]=
{ 0.0, 2.1414979e-5, 3.0160779e-5, 7.2562722e-5, 11.723375e-5};
const double SBHMf_NMF::ModelC_amp[5]=
{ 0.0, 9.0128400e-5, 4.3497037e-5, 84.795348e-5, 170.37206e-5};

double SBHMf_NMF::linterpol(double x1, double x2, double y1, double y2, double x)
{
  double a = (y2-y1)/(x2-x1);
  double b = y1 - a*x1;
  return a*x + b;
};

/**Calcs hydrostatic mapping function.
 * \param E  is a source elevation angle, rad;
 * \param T  is an atmospheric temperature, C;
 * \param P  is an atmospheric pressure, mbar;
 * \param RH is an atmospheric relative humidity, %;
 * \param Station is a pointer to the SBStation;
 * \param MJD is a epoch of observation.
 * \return hydrostatic mapping function value.
 */
double SBHMf_NMF::operator()(double E, float /*T*/, float /*P*/, float /*RH*/, 
			     const SBStation *Station, const SBMJD& MJD)
{
  double a_dry_avg, a_dry_amp, b_dry_avg, b_dry_amp, c_dry_avg, c_dry_amp;

  double phi  = Station->latitude();

  /**A.E. Niell, "Global mapping functions for the atmosphere delay at radio wavelengths",
   * J.G.R. 101, b2, 3227-3246, 1996 (Equations 4 and 5 corrected)
   *
   * available on <http://web.haystack.mit.edu/Geodesy/GeodesyIndex.html>, thanks to Arthur
   *
   */

  double sinE = sin(E);
  double a = 2.53e-5;
  double b = 5.49e-3;
  double c = 1.14e-3;
  double deltaM = (1.0/sinE - (1.0 + a/(1.0 + b/(1.0 + c)))/(sinE + a/(sinE + b/(sinE + c))))*
    Station->height()/1000.0;
  
  if (fabs(phi)<= ModelArg[0])
    {
      a_dry_avg = ModelA_avg[0];
      a_dry_amp = ModelA_amp[0];
      b_dry_avg = ModelB_avg[0];
      b_dry_amp = ModelB_amp[0];
      c_dry_avg = ModelC_avg[0];
      c_dry_amp = ModelC_amp[0];
    }
  else if (fabs(phi)>= ModelArg[4])
    {
      a_dry_avg = ModelA_avg[4];
      a_dry_amp = ModelA_amp[4];
      b_dry_avg = ModelB_avg[4];
      b_dry_amp = ModelB_amp[4];
      c_dry_avg = ModelC_avg[4];
      c_dry_amp = ModelC_amp[4];
    }
  else
    {
      int i = 0;
      while (++i<4 && fabs(phi)>ModelArg[i]) ;

      a_dry_avg = linterpol(ModelArg[i-1], ModelArg[i], ModelA_avg[i-1], ModelA_avg[i], phi);
      a_dry_amp = linterpol(ModelArg[i-1], ModelArg[i], ModelA_amp[i-1], ModelA_amp[i], phi);
      b_dry_avg = linterpol(ModelArg[i-1], ModelArg[i], ModelB_avg[i-1], ModelB_avg[i], phi);
      b_dry_amp = linterpol(ModelArg[i-1], ModelArg[i], ModelB_amp[i-1], ModelB_amp[i], phi);
      c_dry_avg = linterpol(ModelArg[i-1], ModelArg[i], ModelC_avg[i-1], ModelC_avg[i], phi);
      c_dry_amp = linterpol(ModelArg[i-1], ModelArg[i], ModelC_amp[i-1], ModelC_amp[i], phi);
    };
  
  
  double t_doy	   = MJD.dayOfYear();
  a = a_dry_avg + a_dry_amp*cos(2.0*M_PI*(t_doy - 28.0)/365.25);
  b = b_dry_avg + b_dry_amp*cos(2.0*M_PI*(t_doy - 28.0)/365.25);
  c = c_dry_avg + c_dry_amp*cos(2.0*M_PI*(t_doy - 28.0)/365.25);

  return (1.0 + a/(1.0 + b/(1.0 + c)))/(sinE + a/(sinE + b/(sinE + c))) + deltaM;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBWMf_NMF implementation								*/
/*												*/
/*==============================================================================================*/
const double SBWMf_NMF::ModelArg[5]=
{ 15.0*DEG2RAD, 30.0*DEG2RAD, 45.0*DEG2RAD, 60.0*DEG2RAD, 75.0*DEG2RAD};
const double SBWMf_NMF::ModelA_avg[5]=
{ 5.8021897e-4, 5.6794847e-4, 5.8118019e-4, 5.9727542e-4, 6.1641693e-4};
const double SBWMf_NMF::ModelB_avg[5]=
{ 1.4275268e-3, 1.5138625e-3, 1.4572752e-3, 1.5007428e-3, 1.7599082e-3};
const double SBWMf_NMF::ModelC_avg[5]=
{ 4.3472961e-2, 4.6729510e-2, 4.3908931e-2, 4.4626982e-2, 5.4736038e-2};

double SBWMf_NMF::linterpol(double x1, double x2, double y1, double y2, double x)
{
  double a = (y2-y1)/(x2-x1);
  double b = y1 - a*x1;
  return a*x + b;
};

/**Calcs wet mapping function.
 * \param E  is a source elevation angle, rad;
 * \param T  is an atmospheric temperature, C;
 * \param P  is an atmospheric pressure, mbar;
 * \param RH is an atmospheric relative humidity, %;
 * \param Station is a pointer to the SBStation;
 * \param MJD is a epoch of observation.
 * \return wet mapping function value.
 */
double SBWMf_NMF::operator()(double E, float /*T*/, float /*P*/, float /*RH*/, 
			     const SBStation *Station, const SBMJD&)
{
  double a, b, c;
  double phi = Station->latitude();

  /**A.E. Niell, "Global mapping functions for the atmosphere delay at radio wavelengths",
   * J.G.R. 101, b2, 3227-3246, 1996 (Equations 4 and 5 corrected)
   *
   * available on <http://web.haystack.mit.edu/Geodesy/GeodesyIndex.html>, thanks to Arthur
   *
   */

  if (fabs(phi)<= ModelArg[0])
    {
      a = ModelA_avg[0];
      b = ModelB_avg[0];
      c = ModelC_avg[0];
    }
  else if (fabs(phi)>= ModelArg[4])
    {
      a = ModelA_avg[4];
      b = ModelB_avg[4];
      c = ModelC_avg[4];
    }
  else
    {
      int i = 0;
      while (++i<4 && fabs(phi)>ModelArg[i]) ;

      a = linterpol(ModelArg[i-1], ModelArg[i], ModelA_avg[i-1], ModelA_avg[i], phi);
      b = linterpol(ModelArg[i-1], ModelArg[i], ModelB_avg[i-1], ModelB_avg[i], phi);
      c = linterpol(ModelArg[i-1], ModelArg[i], ModelC_avg[i-1], ModelC_avg[i], phi);
    };
  
  double sinE = sin(E);
  return (1.0 + a/(1.0 + b/(1.0 + c)))/(sinE + a/(sinE + b/(sinE + c)));
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBGMf_CH implementation								*/
/*												*/
/*==============================================================================================*/
/**Calcs gradient mapping function.
 * \param E  is a source elevation angle, rad;
 * \param T  is an atmospheric temperature, C;
 * \param P  is an atmospheric pressure, mbar;
 * \param RH is an atmospheric relative humidity, %;
 * \param Station is a pointer to the SBStation;
 * \param MJD is a epoch of observation.
 * \return wet mapping function value.
 */
double SBGMf_CH::operator()(double E, float /*T*/, float /*P*/, float /*RH*/, 
			    const SBStation* /*Station*/, const SBMJD&)
{
  /**
   * IERS Technical Note 2?,
   * IERS Conventions (2003), Chapter 9, page 2:
   *
   * "Chen and Herring (1997) propose using $m_g(e) = 1/(\sin e\tan e + 0.0032)$. The various forms 
   * agree to within 10% for elevation angles higher then $10^\circ$, but the the differences reach 
   * 50% for $5^\circ$ elevation due to the singularity of the $\cot(e)$ function."
   */

  return 1.0/(sin(E)*tan(E) + 0.0032);
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBWMf_MTT implementation								*/
/*												*/
/*==============================================================================================*/
SBRefraction::SBRefraction(const SBConfig* Cfg_) 
  : RefrSource(v3Zero)
{
  Cfg = Cfg_;
  switch(Cfg->refrDryZenith())
    {
    default:
    case SBConfig::RDZ_Saastamoinen:	DryZDelay = new SBHZD_Saastamoinen;	break;
    case SBConfig::RDZ_Hopfield:	DryZDelay = new SBHZD_Hopfield;		break;
    case SBConfig::RDZ_NONE:		DryZDelay = new SBZenithDelay;		break;
    };
  switch(Cfg->refrWetZenith())
    {
    default:
    case SBConfig::RWZ_Saastamoinen:	WetZDelay = new SBWZD_Saastamoinen;	break;
    case SBConfig::RWZ_Hopfield:	WetZDelay = new SBWZD_Hopfield;		break;
    case SBConfig::RWZ_NONE:		WetZDelay = new SBZenithDelay;		break;
    };
  switch(Cfg->refrDryMapping())
    {
    case SBConfig::RDM_Chao:		DryMapping = new SBHMf_Chao;		break;
    case SBConfig::RDM_CfA2p2:		DryMapping = new SBHMf_CfA2p2;		break;
    case SBConfig::RDM_Ifadis:		DryMapping = new SBHMf_Ifadis;		break;
    case SBConfig::RDM_NMFH2:		DryMapping = new SBHMf_NMF;		break;
    default:
    case SBConfig::RDM_MTT:		DryMapping = new SBHMf_MTT;		break;
    case SBConfig::RDM_NONE:		DryMapping = new SBMappingFunc;		break;
    };
  switch(Cfg->refrWetMapping())
    {
    case SBConfig::RWM_Chao:		WetMapping = new SBWMf_Chao;		break;
    case SBConfig::RWM_Ifadis:		WetMapping = new SBWMf_Ifadis;		break;
    case SBConfig::RDM_NMFW2:		WetMapping = new SBWMf_NMF;		break;
    default:
    case SBConfig::RWM_MTT:		WetMapping = new SBWMf_MTT;		break;
    case SBConfig::RWM_NONE:		WetMapping = new SBMappingFunc;		break;
    };

  GradMapping = new SBGMf_CH;

  if (Cfg->isRollBack2NMF())
    {
      AltDryMapping = new SBHMf_NMF;
      AltWetMapping = new SBWMf_NMF;
    }
  else
    {
      AltDryMapping = NULL;
      AltWetMapping = NULL;
    };

  Elev	  = 0.0;
  RefrElev= 0.0;
  dE	  = 0.0;
  Azimuth = 0.0;
  Delta	  = 0.0;
  TotZenithDelay = 0.0;
};

SBRefraction::~SBRefraction()
{
  if (DryZDelay) 
    {
      delete DryZDelay;
      DryZDelay = NULL;
    };
  if (WetZDelay) 
    {
      delete WetZDelay;
      WetZDelay = NULL;
    };
  if (DryMapping) 
    {
      delete DryMapping;
      DryMapping = NULL;
    };
  if (WetMapping) 
    {
      delete WetMapping;
      WetMapping = NULL;
    };
  if (GradMapping) 
    {
      delete GradMapping;
      GradMapping = NULL;
    };
  if (AltDryMapping) 
    {
      delete AltDryMapping;
      AltDryMapping = NULL;
    };
  if (AltWetMapping) 
    {
      delete AltWetMapping;
      AltWetMapping = NULL;
    };
};

/**Calcs refracted antenna's direction in TRF.
 * This function calulates angles of azimuth and refracted elevation of an 
 * antenna direction too. It has to be called after zenith delays calculation 
 * and before mapping functions.
 * \param T is an atmospheric temperature, C;
 * \param DryZD is a hydrostatic zenith delay, m;
 * \param WetZD is a wet zenith delay, m;
 * \param Station is a pointer to the SBStation;
 * \param S is a TRS vector of the radiosource, abberated.
 * \return antenna's direction in TRF corrected for refraction.
 */
const Vector3 SBRefraction::refrDir(float T, double DryZD, double WetZD, 
				    const SBStation* Station, const SBSource* Source, const Vector3& S)
{
  Vector3 s = ~Station->fmVEN()*S;	// s is abberated source in VEN, TRS
  Azimuth   = atan2(s[EAST], s[NORT]);
  Delta     = 8567.0/292.0*(T + 273.15);
  Elev = RefrElev = M_PI/2.0;
  if (s.at(VERT)<0.0)
    Log->write(SBLog::ERR, SBLog::REFRACT, ClassName() + ": ERROR: source [" + Source->name() + 
	       "] below the horizon at the station [" + Station->name() + "]");
  if (s[VERT]!=1.0)
    {
      Elev	   = asin(s[VERT]);
      double sinE2 = s[VERT]*s[VERT];		// =sin(E0)*sin(E0)
      //M_001 has been calculated with the follow mean values: 
      //			\alpha=5, \betta=3.5, q_1=0.1459, q_2=1.424
      //      double Mw    = 0.29602606;	
      double Sigma = Delta/6378136.49;
      double Sigma_= sinE2*(sqrt(1.0 + Sigma*(Sigma + 2)/sinE2) - 1.0);
      double Aleph = acos((1.0 + Sigma_)/(1.0 + Sigma));
      double Hi    = (DryZD + WetZD/*/Mw*/)/Delta;
      //      dE	   = acos(cos(Elevation + Aleph)/(1.0 + Hi)) - Aleph;
      RefrElev = acos(cos(Elev + Aleph)/(1.0 + Hi)) - Aleph;
      dE = RefrElev - Elev;
      double sinE, cosE, sinT, cosT;
      sincos(RefrElev, &sinE, &cosE);	//Elevation here is E0+dE
      sincos(Azimuth, &sinT, &cosT);
      // set up s with azimuth and corrected for refraction elevation (VEN)
      s[VERT] = sinE;				// vertical
      s[EAST] = cosE*sinT;			// east
      s[NORT] = cosE*cosT;			// north
    };
  s = Station->fmVEN()*s; // turn back to TRF
  return s;
};

/**Antenna axiss offset altitude correction.
 * \param T is an atmospheric temperature, C;
 * \param DryZD is a hydrostatic zenith delay, m;
 * \param StarTime is a GMST, rad;
 * \param Station is a pointer to the SBStation.
 * \param Source is a pointer to the SBSource.
 * \return axiss offset altitude correction to hydrostatic zenith delay, meter.
 */
double SBRefraction::altitudeCorr(float /*T*/, double DryZD, double StarTime,
				  const SBStation* Station, const SBSource* Source)
{
  double sinE, cosE;
  double Psi = 0.0;

  if (Station->mountType()==SBStation::Mnt_AZEL || Station->axisOffset()==0.0)
    return 0.0;
  if (Station->mountType()==SBStation::Mnt_Unkn)
    return 0.0;


  sincos (refrElev(), &sinE, &cosE);
  switch (Station->mountType())
    {
    case SBStation::Mnt_EQUA:
      Psi = cos(Station->latitude())*cos(StarTime + Station->longitude() - Source->ra_t());
      break;
    case SBStation::Mnt_X_YN:
      {
	double cosT = cos(azimuth());
	Psi = sinE/sqrt(1.0 - cosT*cosT*cosE*cosE);
      };
      break;
    case SBStation::Mnt_X_YE:
      {
	double sinT = sin(azimuth());
	Psi = sinE/sqrt(1.0 - sinT*sinT*cosE*cosE);
      };
      break;
    case SBStation::Mnt_Richmond:
      Psi = cos(39.023833*DEG2RAD)*
	cos(atan(cosE*sin(azimuth() - 0.12*DEG2RAD)/
	     (cos(39.023833*DEG2RAD)*sinE - sin(39.023833*DEG2RAD)*cosE*cos(azimuth() + 
									    0.12*DEG2RAD))));
      break;
    default:
      break;
    };
  return -DryZD*Station->axisOffset()/Delta*Psi;
};

/**Calcs refraction delay.
 * \param T  is an atmospheric temperature, C;
 * \param P  is an atmospheric pressure, mbar;
 * \param RH is an atmospheric relative humidity, %;
 * \param Frame is a pointer to the SBFrame;
 * \param Station is a pointer to the SBStation;
 * \param Source is a pointer to the SBSource;
 * \param S is a TRS vector of the radiosource, abberated;
 * \param Zw_ is a predicted wet zenith delay;
 * \param IsWrongMetheo is set to TRUE if a station has bad or suspicious metheo parameters.
 * \return refraction delay, meter.
 */
double SBRefraction::operator()(float T, float P, float RH, const SBFrame* Frame, 
				SBStation* Station, const SBSource* Source, const Vector3& S,
				double Zw_, bool IsWrongMetheo)
{
  double ZenithWDelayAPriori = Zw_; // from PreProcessing

  SBMappingFunc *DryMF = DryMapping;
  SBMappingFunc *WetMF = WetMapping;
  if (AltDryMapping && IsWrongMetheo)
    {
      DryMF = AltDryMapping;
      WetMF = AltWetMapping;
    };


  // start a report:
  if (Log->isEligible(SBLog::DBG, SBLog::REFRACT))
    {
      Log->write(SBLog::DBG, SBLog::REFRACT, ClassName() + ":");
      Log->write(SBLog::DBG, SBLog::REFRACT, ClassName() + 
		 ": Arg(UTC)              : " + Frame->time().toString());
      Log->write(SBLog::DBG, SBLog::REFRACT, ClassName() + 
		 ": Station               : [" + Station->id().toString() + "] " + Station->name());
    };

  double WetZenithEst = (Station->p_Zenith()?Station->p_Zenith()->v():0.0) + ZenithWDelayAPriori;
  double DryZenith = (*DryZDelay)(T, P, RH, Station);
  double WetZenith = IsWrongMetheo?0.0:(*WetZDelay)(T, P, RH, Station); // calc wet Zenith delay if metheo are OK
  double AltCorr   = 0.0;
  double TotRefr   = 0.0;

  RefrSource = refrDir(T, DryZenith, WetZenith + WetZenithEst, Station, Source, S);

  double GradN	   = (Station->p_AtmGradN()?Station->p_AtmGradN()->v():0.0);
  double GradE	   = (Station->p_AtmGradE()?Station->p_AtmGradE()->v():0.0);
  double Gradient  = GradN*cos(Azimuth) + GradE*sin(Azimuth);

  if (Cfg->isAltAxsCorr()) 
    DryZenith+= (AltCorr=altitudeCorr(T, DryZenith, Frame->gmst(), Station, Source));


  double DryMap	   = (*DryMF) (elev(), T, P, RH, Station, Frame->time());
  WetMap	   = (*WetMF) (elev(), T, P, RH, Station, Frame->time());
  GradMap	   = (*GradMapping)(elev(), T, P, RH, Station, Frame->time());


  TotZenithDelay   = DryZenith + WetZenith + WetZenithEst;
  TotRefr	   = DryZenith*DryMap + (WetZenith + WetZenithEst)*WetMap + Gradient*GradMap;

  // continue a report:
  if (Log->isEligible(SBLog::DBG, SBLog::REFRACT))
    {
      Log->write(SBLog::DBG, SBLog::REFRACT, ClassName() + 
		 ": Used model            : " + DryZDelay->ClassName());
      Log->write(SBLog::DBG, SBLog::REFRACT, ClassName() + 
		 ": Used model            : " + WetZDelay->ClassName());
      Log->write(SBLog::DBG, SBLog::REFRACT, ClassName() + 
		 ": Used model            : " + DryMF->ClassName());
      Log->write(SBLog::DBG, SBLog::REFRACT, ClassName() + 
		 ": Used model            : " + WetMF->ClassName());

      Log->write(SBLog::DBG, SBLog::REFRACT, ClassName() + 
		 ": Source Azimuth        : " + rad2dmsStr(Azimuth));
      Log->write(SBLog::DBG, SBLog::REFRACT, ClassName() + 
		 ": Sourse Elevation      : " + rad2dmslStr(Elev));
      Log->write(SBLog::DBG, SBLog::REFRACT, ClassName() + 
		 ": Refracted Elevation   : " + rad2dmslStr(RefrElev));
      Log->write(SBLog::DBG, SBLog::REFRACT, ClassName() + 
		 ": Correction of Elev.   : " + rad2dmslStr(dE));

      Log->write(SBLog::DBG, SBLog::REFRACT, ClassName() + 
		 ": Temperature, C        : " + QString().sprintf("%24.16f", T));
      Log->write(SBLog::DBG, SBLog::REFRACT, ClassName() + 
		 ": Pressure, mb          : " + QString().sprintf("%24.16f", P));
      Log->write(SBLog::DBG, SBLog::REFRACT, ClassName() + 
		 ": Relative Humidity, %  : " + QString().sprintf("%24.16f", RH));
      Log->write(SBLog::DBG, SBLog::REFRACT, ClassName() + 
		 ": Water Vapour press, mb: " + QString().sprintf("%24.16f", 
								  SBZenithDelay::vapourPress(T, RH)));

      Log->write(SBLog::DBG, SBLog::REFRACT, ClassName() + 
		 ": Trop. scale height (m): " + QString().sprintf("%24.16f", Delta));
      Log->write(SBLog::DBG, SBLog::REFRACT, ClassName() + 
		 ": Altitude axis corr.(m): " + QString().sprintf("%24.16f", AltCorr));
      Log->write(SBLog::DBG, SBLog::REFRACT, ClassName() + 
		 ": Dry Zenith Delay   (m): " + QString().sprintf("%24.16f", DryZenith));
      Log->write(SBLog::DBG, SBLog::REFRACT, ClassName() + 
		 ": Wet Zenith Delay   (m): " + QString().sprintf("%24.16f", WetZenith));
      Log->write(SBLog::DBG, SBLog::REFRACT, ClassName() + 
		 ": Gradient additive  (m): " + QString().sprintf("%24.16f", Gradient));
      Log->write(SBLog::DBG, SBLog::REFRACT, ClassName() + 
		 ": Mapped gradient    (m): " + QString().sprintf("%24.16f", Gradient*GradMap));
      Log->write(SBLog::DBG, SBLog::REFRACT, ClassName() + 
		 ": Wet Zen.Delay Est. (m): " + QString().sprintf("%24.16f", WetZenithEst));
      Log->write(SBLog::DBG, SBLog::REFRACT, ClassName() + 
		 ": Total Zenith Delay (m): " + QString().sprintf("%24.16f", TotZenithDelay));
      Log->write(SBLog::DBG, SBLog::REFRACT, ClassName() + 
		 ": Total Refr. Delay  (m): " + QString().sprintf("%24.16f", TotRefr));
      RefrSource.report2Log(SBLog::DBG, SBLog::REFRACT, ClassName() + 
		 ": Source abb+refr (TRF) : ", 15, 12);
      Log->write(SBLog::DBG, SBLog::REFRACT, ClassName() + ": +++");
    };

  return TotRefr;
};


void SBRefraction::makeDebugReport(const SBMJD& 
#ifdef ELEV_DEBUG_OUTPUT
t
#endif
, 
SBStation* 
#ifdef ELEV_DEBUG_OUTPUT
Station
#endif
)
{
#ifdef ELEV_DEBUG_OUTPUT
  //----------------
  if (Station->outStream() && t != Station->label())
    {
      *Station->outStream() << t.toString(SBMJD::F_UNIX) 
			    << QString().sprintf("  %12.6f %7.3f %7.3f\n", 
						 (double)t, Elev*RAD2DEG, RefrElev*RAD2DEG);
      Station->setLabel(t);
    };
  //---------------- 
#endif
};
/*==============================================================================================*/
