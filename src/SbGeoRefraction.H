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

#ifndef SB_GEO_REFRACTION_H
#define SB_GEO_REFRACTION_H

#include <config.h>
#include <qstring.h>

#include "SbGeoTime.H"
#include "SbVector3.H"


class SBStation;
/*==============================================================================================*/
/**Zenith delay, general interface.
 * This is a common class for all classes deal with zenith (hydrostatic (aka "dry") and wet) 
 * delay. However, this class can be used to describe an absence of the zenith delay model(s).
 */
/*==============================================================================================*/
class SBZenithDelay
{
protected:
  static const double		K1;		//!< refraction coeff. #1, K/mbar;   Thayer, 1974
  static const double		K2;		//!< refraction coeff. #2, K/mbar;   Thayer, 1974
  static const double		K3;		//!< refraction coeff. #3, K/mbar^2; Thayer, 1974
public:
  /** A constructor. */
  SBZenithDelay(){};
  /** A destructor. */
  virtual ~SBZenithDelay(){};
  /** Refers to a class name (debug info) */
  virtual QString ClassName() const {return "SBZenithDelay";};
  /** Returns refraction coefficient k1, K/mbar.*/
  double k1() const {return K1;};
  /** Returns refraction coefficient k2, K/mbar.*/
  double k2() const {return K2;};
  /** Returns refraction coefficient k3, K/mbar^2.*/
  double k3() const {return K3;};
  /**Calcs hydrostatic zenith delay.
   * \param T  is an atmospheric temperature, C;
   * \param P  is an atmospheric pressure, mbar;
   * \param RH is an atmospheric relative humidity, %;
   * \param Station is a pointer to the SBStation.
   * \return 0.0 meters (dumb class).
   */
  virtual double operator()(float /*T*/, float /*P*/, float /*RH*/, const SBStation* /*Station*/)
    {return 0.0;};
  /**Calcs partial water vapour pressure, mbar.
   * \param T is a dry temperature, C;
   * \param RH is a relative humidity, %.
   * \return partial water vapour pressure, mbar.
   **/
  static  double vapourPress(float, float);
};
/*==============================================================================================*/



/*==============================================================================================*/
/**Mapping function, general interface.
 * The base class for series of mapping function (for hydrostatic and wet cases).
 */
/*==============================================================================================*/
class SBMappingFunc
{
public:
  /** A constructor. */
  SBMappingFunc(){};
  /** A destructor. */
  virtual ~SBMappingFunc(){};
  /** Refers to a class name (debug info) */
  virtual QString ClassName() const {return "SBMappingFunc";};
  /**Calcs wet mapping function.
   * \param E  is a source elevation angle, rad;
   * \param T  is an atmospheric temperature, C;
   * \param P  is an atmospheric pressure, mbar;
   * \param RH is an atmospheric relative humidity, %;
   * \param Station is a pointer to the SBStation.
   * \return 0.0 (dumb class).
   */
  virtual double operator()(double /*E*/, float /*T*/, float /*P*/, float /*RH*/, 
			    const SBStation* /*Station*/, const SBMJD&)
    {return 0.0;};
};
/*==============================================================================================*/



/*==============================================================================================*/
/**Hydrostatic Zenith delay model by Hopfield, 1977.
 *
 */
/*==============================================================================================*/
class SBHZD_Hopfield : public SBZenithDelay
{
public:
  /** Refers to a class name (debug info) */
  virtual QString ClassName() const {return "Hydrostatic zenith delay: Hopfield, 1977";};
  virtual double operator()(float, float, float, const SBStation*);
};
/*==============================================================================================*/



/*==============================================================================================*/
/**Hydrostatic Zenith delay model by Saastamoinen, 1972.
 *
 */
/*==============================================================================================*/
class SBHZD_Saastamoinen : public SBZenithDelay
{
public:
  /** Refers to a class name (debug info) */
  virtual QString ClassName() const {return "Hydrostatic zenith delay: Saastamoinen, 1972";};
  virtual double operator()(float, float, float, const SBStation*);
};
/*==============================================================================================*/



/*==============================================================================================*/
/**Wet Zenith delay model by Hopfield, 1977.
 *
 */
/*==============================================================================================*/
class SBWZD_Hopfield : public SBZenithDelay
{
public:
  /** Refers to a class name (debug info) */
  virtual QString ClassName() const {return "Wet zenith delay: Hopfield, 1977";};
  virtual double operator()(float, float, float, const SBStation*);
};
/*==============================================================================================*/



/*==============================================================================================*/
/**Wet Zenith delay model by Saastamoinen, 1973.
 *
 */
/*==============================================================================================*/
class SBWZD_Saastamoinen : public SBZenithDelay
{
public:
  /** Refers to a class name (debug info) */
  virtual QString ClassName() const {return "Wet zenith delay: Saastamoinen, 1973";};
  virtual double operator()(float, float, float, const SBStation*);
};
/*==============================================================================================*/



/*==============================================================================================*/
/**Hydrostatic Mapping Function by Chao, 1974.
 * This is a simplest mapping function (and fast).
 */
/*==============================================================================================*/
class SBHMf_Chao : public SBMappingFunc
{
public:
  /** Refers to a class name (debug info) */
  virtual QString ClassName() const {return "Hydrostatic mapping function: Chao, 1974";};
  virtual double operator()(double, float, float, float, const SBStation*, const SBMJD&);
};
/*==============================================================================================*/



/*==============================================================================================*/
/**Wet Mapping Function by Chao, 1974.
 * This is a simplest mapping function (and fast).
 */
/*==============================================================================================*/
class SBWMf_Chao : public SBMappingFunc
{
public:
  /** Refers to a class name (debug info) */
  virtual QString ClassName() const {return "Wet mapping function: Chao, 1974";};
  virtual double operator()(double, float, float, float, const SBStation*, const SBMJD&);
};
/*==============================================================================================*/



/*==============================================================================================*/
/**CfA-2.2 Mapping Function by Devis et al., 1985.
 * It is derived only for hydrostatic case.
 */
/*==============================================================================================*/
class SBHMf_CfA2p2 : public SBMappingFunc
{
public:
  /** Refers to a class name (debug info) */
  virtual QString ClassName() const 
    {return "Hydrostatic Mapping function: CfA-2.2, Davis et al., 1985";};
  virtual double operator()(double, float, float, float, const SBStation*, const SBMJD&);
};
/*==============================================================================================*/



/*==============================================================================================*/
/**Hydrostatic mapping Function by Ifadis, 1986.
 *
 */
/*==============================================================================================*/
class SBHMf_Ifadis : public SBMappingFunc
{
public:
  /** Refers to a class name (debug info) */
  virtual QString ClassName() const {return "Hydrostatic Mapping function: Ifadis, 1986";};
  virtual double operator()(double, float, float, float, const SBStation*, const SBMJD&);
};
/*==============================================================================================*/



/*==============================================================================================*/
/**Wet mapping Function by Ifadis, 1986.
 *
 */
/*==============================================================================================*/
class SBWMf_Ifadis : public SBMappingFunc
{
public:
  /** Refers to a class name (debug info) */
  virtual QString ClassName() const {return "Wet Mapping function: Ifadis, 1986";};
  virtual double operator()(double, float, float, float, const SBStation*, const SBMJD&);
};
/*==============================================================================================*/



/*==============================================================================================*/
/**MTT Hydrostatic mapping Function by Herring, 1992.
 *
 */
/*==============================================================================================*/
class SBHMf_MTT : public SBMappingFunc
{
public:
  /** Refers to a class name (debug info) */
  virtual QString ClassName() const {return "MTT Hydrostatic Mapping function: Herring, 1992";};
  virtual double operator()(double, float, float, float, const SBStation*, const SBMJD&);
};
/*==============================================================================================*/



/*==============================================================================================*/
/**MTT Wet mapping Function by Herring, 1992.
 *
 */
/*==============================================================================================*/
class SBWMf_MTT : public SBMappingFunc
{
public:
  /** Refers to a class name (debug info) */
  virtual QString ClassName() const {return "MTT Wet Mapping function: Herring, 1992";};
  virtual double operator()(double, float, float, float, const SBStation*, const SBMJD&);
};
/*==============================================================================================*/


class SBSpline;
/*==============================================================================================*/
/**NMF Hydrostatic mapping Function by Niell, 1996.
 *
 */
/*==============================================================================================*/
class SBHMf_NMF : public SBMappingFunc
{
private:
  static const double	ModelArg[5];
  static const double	ModelA_avg[5];
  static const double	ModelB_avg[5];
  static const double	ModelC_avg[5];
  static const double	ModelA_amp[5];
  static const double	ModelB_amp[5];
  static const double	ModelC_amp[5];
  double		linterpol(double, double, double, double, double);
public:
  /** Refers to a class name (debug info) */
  virtual QString ClassName() const {return "NMF Hydrostatic Mapping function: Niell, 1996";};
  virtual double operator()(double, float, float, float, const SBStation*, const SBMJD&);
};
/*==============================================================================================*/



/*==============================================================================================*/
/**NMF Wet mapping Function by Niell, 1996.
 *
 */
/*==============================================================================================*/
class SBWMf_NMF : public SBMappingFunc
{
private:
  static const double	ModelArg[5];
  static const double	ModelA_avg[5];
  static const double	ModelB_avg[5];
  static const double	ModelC_avg[5];
  double		linterpol(double, double, double, double, double);
public:
  /** Refers to a class name (debug info) */
  virtual QString ClassName() const {return "NMF Wet Mapping function: Niell, 1996";};
  virtual double operator()(double, float, float, float, const SBStation*, const SBMJD&);
};
/*==============================================================================================*/



/*==============================================================================================*/
/**Gradient mapping Function by Chen and Herring, 1997.
 *
 */
/*==============================================================================================*/
class SBGMf_CH : public SBMappingFunc
{
public:
  /** Refers to a class name (debug info) */
  virtual QString ClassName() const {return "Gradient Mapping function: Chen and Herring, 1997";};
  virtual double operator()(double, float, float, float, const SBStation*, const SBMJD&);
};
/*==============================================================================================*/



class SBConfig;
class SBSource;
class SBFrame;
/*==============================================================================================*/
/**Refraction.
 * Implements calculations of the refraction and related effects.
 */
/*==============================================================================================*/
class SBRefraction
{
protected:
  const SBConfig	*Cfg;
  SBZenithDelay		*DryZDelay;
  SBZenithDelay		*WetZDelay;
  SBMappingFunc		*DryMapping;
  SBMappingFunc		*WetMapping;
  SBMappingFunc		*AltDryMapping;
  SBMappingFunc		*AltWetMapping;
  SBMappingFunc		*GradMapping;
  double		WetMap;
  double		GradMap;
  double		Elev;			//!< antenna's elevation angle
  double		RefrElev;		//!< antenna's elevation angle with refraction
  double		dE;			//!< correction of the antenna's elevation angle w/refr
  double		Delta;			//!< the troposphere scale height, m
  double		Azimuth;		//!< antenna's azimuth
  double		TotZenithDelay;		//!< total zenith delay, m
  Vector3		RefrSource;		//!< unit vector to the refracted source
  virtual const Vector3 refrDir(float, double, double, const SBStation*, const SBSource*, const Vector3&);
  virtual double	altitudeCorr(float, double, double, const SBStation*, const SBSource*);
public:
  /**A constructor. */
  SBRefraction(const SBConfig*);
  /**A destructor. */
  virtual ~SBRefraction();
  /**Refers to a class name (debug info) */
  virtual QString		ClassName()	const {return "SBRefraction";};
  virtual double		elev()		const {return Elev;};
  virtual double		refrElev()	const {return RefrElev;};
  virtual double		azimuth()	const {return Azimuth;};
  virtual double		wetMap()	const {return WetMap;};
  virtual double		gradMap()	const {return GradMap;};
  virtual double		totZenithDelay()const {return TotZenithDelay;};
  /**Returns unit vector in refracted source direction, TRF. */
  virtual const Vector3&	refrSource()	const {return RefrSource;};
  virtual double operator()(float, float, float, const SBFrame*, SBStation*, 
			    const SBSource*, const Vector3&, double, bool);
  
  // aux:
  void				makeDebugReport(const SBMJD&, SBStation*);
};
/*==============================================================================================*/




#endif //SB_GEO_REFRACTION_H
