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
 *
 * Overview:	delay and delay rate calculations.
 *
 */

#ifndef SB_GEO_DELAY_H
#define SB_GEO_DELAY_H

#include <config.h>
#include <qstring.h>
#include "SbVector3.H"
#include "SbGeoTime.H"


class SBFrame;
class SBConfig;
class SBEphem;
class SBStation;
class SBStationInfo;
class SBSource;
class SBObsVLBIEntry;
class SBTideLd;
class SBRefraction;
class SBRunManager;
/*==============================================================================================*/
/**Delay & rate calculations
 *
 *
 */
/*==============================================================================================*/
class SBDelay
{
public:
  static const double	CLight;		//!< speed of light (can be differ from Ephem::CLight)
  static const double	AEarth;		//!< equatorial radius of the Earth
  static const double	Gamma;		//!< a PPN Parameter = 1 in general relativity

private:
  const	SBConfig	*Cfg;
  SBFrame		*Frame;
  SBEphem		*Ephem;
  SBTideLd		*Tide;
  SBRefraction		*Refr;

  //input:
  SBRunManager		*Mgr;
  SBStation		*St_1; 
  SBStation		*St_2; 
  SBSource		*So; 
  SBObsVLBIEntry	*Obs;

  //tmp vars:
  SBMJD			t1;
  Vector3		K ;	//!< the unit vector from the barycenter to the source, CRS
  Vector3		x1;	//!< geocentric radius vector of the 1st receiver @t1, CRS
  Vector3		x2;	//!< geocentric radius vector of the 2nd receiver @t1, CRS
  Vector3		b ;	//!< geocentric baseline vector @t1, CRS
  Vector3		b_TRS;	//!< geocentric baseline vector @t1, TRS
  Vector3		X1;	//!< barycentric radius vector of the 1st receiver @t1, CRS
  Vector3		X2;	//!< barycentric radius vector of the 2nd receiver @t1, CRS
  Vector3		k1;	//!< unit vector from the 1st station to the source, abberated, CRS
  Vector3		k2;	//!< unit vector from the 2nd station to the source, abberated, CRS
  Vector3		k1r;	//!< unit vector from the 1st station to the source, abber.+refr., TRF
  Vector3		k2r;	//!< unit vector from the 2nd station to the source, abber.+refr., TRF
  Vector3		Ve;
  Vector3		w1;
  Vector3		w2;
  double		kbc;
  double		DTau_Term_1;
  double		DTau_Term_2;
  double		DTau_Term_3;

  
  //calculated:
  double		GravDelay;
  double		VacuumDelay;
  double		GeomDelay;
  double		TotalDelay;
  double		AntennaDelay;
  double		DTAtm1;
  double		DTAtm2;
  double		TotZenithDelay1;
  double		TotZenithDelay2;

  //aux func
  virtual void		calcDerivatives(const SBMJD&);
  
public:
  /**A constructor. */
  SBDelay(const SBConfig*, SBFrame*, SBEphem*, SBTideLd*, SBRefraction*);
  /**A destructor. */
  virtual ~SBDelay();
  /**Refers to a class name (debug info) */
  virtual QString ClassName() const {return "SBDelay";};

  /**Returns speed of light */
  virtual double	cLight() const {return CLight;};
  /**Returns equatorial radius of the Earth */
  virtual double	aEarth() const {return AEarth;};
  /**Returns a PPN Parameter*/
  virtual double	gamma() const {return Gamma;};
  virtual double	totZenithDelay1() const {return TotZenithDelay1;};
  virtual double	totZenithDelay2() const {return TotZenithDelay2;};
  virtual double	totalDelay() const {return TotalDelay;};
  

  virtual double	calc(SBRunManager*, SBStation*, SBStation*, SBSource*, SBObsVLBIEntry*, 
			     SBStationInfo*, SBStationInfo*);
};
/*==============================================================================================*/



#endif //SB_GEO_DELAY_H
