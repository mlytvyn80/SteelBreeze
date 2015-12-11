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
 * Overview:	describes interface of classes necessary for calculations
 *		of tide load displacements. Implements ocean tide loading,
 *		deformation due to polar motion, solid tide loading.
 *
 */

#ifndef SB_GEO_TIDE_H
#define SB_GEO_TIDE_H

#include <config.h>
#include "SbVector3.H"
#include "SbGeoTime.H"



class SBOLoad;
class SBConfig;
/*==============================================================================================*/
/**Ocean Tides Load displacements.
 * Inherits Vector3 representing VEN local tide displacement.
 *
 * \todo
 * Need some redesigning with respect to SBStation::oLoad() implementation.
 *
 */
/*==============================================================================================*/
class SBOceanTideLd : public Vector3
{
protected:
  const SBConfig	*Cfg;
  static const double	SPEED[11];
  static const double	ANGFAC[4][11];
  static const SBMJD	_1975;
  double		ANGLE[11];
  void			ARG_IERS(const SBTime&);
public:
  /** A constructor. */
  SBOceanTideLd(const SBConfig*);
  /** A destructor. */
  virtual ~SBOceanTideLd(){};
  /** Refers to a class name (debug info) */
  virtual QString ClassName() const {return "SBOceanTideLd";};
  /**Calculates site's displacements due to ocean loading corresponds to time T. */
  virtual const Vector3& operator()(const SBOLoad*, const SBTime&);
};
/*==============================================================================================*/


class SBStation;
class SBEOP;
/*==============================================================================================*/
/**Polar Tide displacements.
 * Inherits Vector3 representing VEN local tide displacement.
 */
/*==============================================================================================*/
class SBPolarTideLd : public Vector3
{
protected:
  const SBConfig	*Cfg;
public:
  /** A constructor. */
  SBPolarTideLd(const SBConfig *Cfg_) : Vector3(v3Zero){Cfg = Cfg_;};
  /** A destructor. */
  virtual ~SBPolarTideLd(){};
  /** Refers to a class name (debug info) */
  virtual QString ClassName() const {return "SBPolarTideLd";};
  /**Calculates rotational deformation due to Polar Motion corresponds to time T. */
  virtual const Vector3& operator()(SBStation*, const SBEOP*, const SBTime&);
};
/*==============================================================================================*/


class SBEphem;
class SBCelestBody;
class SBFrame;
/*==============================================================================================*/
/**Station's displacement due to Solid Earth Tides (general interface).
 * Inherits Vector3, represents local displacement in TRF.
 */
/*==============================================================================================*/
class SBSolidTideLd : public Vector3
{
protected:
  const SBConfig	*Cfg;
  static const double	h_0_el;		//!< nominal value for h^(0), elastic
  static const double	h_2_el;		//!< nominal value for h^(2), elastic
  static const double	l_0_el;		//!< nominal value for l^(0), elastic
  static const double	l_2_el;		//!< nominal value for l^(2), elastic
  static const double	h_0_an;		//!< nominal value for h^(0), anelastic
  static const double	h_2_an;		//!< nominal value for h^(2), anelastic
  static const double	l_0_an;		//!< nominal value for l^(0), anelastic
  static const double	l_2_an;		//!< nominal value for l^(2), anelastic

  double		h_0;
  double		h_2;
  double		l_0;
  double		l_2;
  // debug:
  Vector3		TideDeg2ip;
  Vector3		TideDeg3ip;
  Vector3		TideDeg2op;
  Vector3		TideLatDepend;
  Vector3		Step1;
  Vector3		Step2;
  //tmp:
  double		CoeffSun;
  double		CoeffMoon;

  // 
  Vector3		Moon_TRS;
  Vector3		Sun_TRS;
  Matrix3		FmREN;		//!< transforms local radial-transverse coords to TRF
  /**Returns Love number h_2. 
   * Returns nominal Love number h_2 (in the case of no latitude dependancy) or 
   * latitude dependent h_2.
   */
  virtual double	h2(double phi, SBStation*) const;
  /**Returns Love number l_2. 
   * Returns nominal Love number l_2 (in the case of no latitude dependancy) or 
   * latitude dependent l_2.
   */
  virtual double	l2(double phi, SBStation*) const;
  /**Returns Love number h_3. */
  virtual double	h3() const {return 0.292;};
  /**Returns Love number l_3. */
  virtual double	l3() const {return 0.015;};
  
  virtual Vector3		deg2InPhase(SBStation*, const Vector3&);
  virtual const Vector3&	makeStep1(SBStation*, SBEphem*);
  virtual const Vector3&	makeStep2(SBStation*, const SBFrame&);

  /**Returns permanent tide displacement (TRF coordinate system). */
  virtual Vector3		permTide(SBStation*);
public:
  /** A constructor. 
   * Creates a copy of object, inits love numbers with the corresponded values
   * (anelastic or elastic case).
   */
  SBSolidTideLd(const SBConfig*);
  /** A destructor. */
  virtual ~SBSolidTideLd(){};
  /** Refers to a class name (debug info) */
  virtual QString ClassName() const {return "SBSolidTideLd";};
  /**Calculates site's displacement vector corresponds to time T.
   * Calls SBSolidTideLd::makeStep1() and SBSolidTideLd::makeStep1(), then,
   * if it is necessary, removes permanent tide.
   */
  virtual const Vector3& operator()(SBStation*, SBEphem*, const SBFrame&);
  const Vector3&	tideDeg2ip() const {return TideDeg2ip;};
  const Vector3&	tideDeg3ip() const {return TideDeg3ip;};
  const Vector3&	tideDeg2op() const {return TideDeg2op;};
  const Vector3&	tideLatDepend() const {return TideLatDepend;};

  const Vector3&	step1() const {return Step1;};
  const Vector3&	step2() const {return Step2;};

  Vector3		dTide_dLag(SBStation*, SBEphem*);
  Vector3		dTide_dH2(SBStation*, SBEphem*);
  Vector3		dTide_dL2(SBStation*, SBEphem*);
};
/*==============================================================================================*/



/*==============================================================================================*/
/**Station's displacement due to Solid Earth Tides, IERS92 Standards model.
 * Inherits Vector3 representing local displacement in TRF.
 */
/*==============================================================================================*/
class SBSolidTideLdIERS92 : public SBSolidTideLd
{
protected:
  virtual double	h2(double, SBStation*) const;
  virtual double	l2(double, SBStation*) const;
public:
  /** A constructor. */
  SBSolidTideLdIERS92(const SBConfig* Cfg_) : SBSolidTideLd(Cfg_){};
  /** Refers to a class name (debug info) */
  virtual QString ClassName() const {return "SBSolidTideLdIERS92";};
};
/*==============================================================================================*/



/*==============================================================================================*/
/**Station's displacement due to Solid Earth Tides, IERS96 Standards model.
 * Inherits Vector3 representing local displacement in TRF.
 */
/*==============================================================================================*/
class SBSolidTideLdIERS96 : public SBSolidTideLd
{
protected:
  struct TableEntry73a
  {
    short	N[5];	
    double	DR,DT;
  };
  struct TableEntry73b
  {
    short	N[5];
    double	DRip,DTip, DRop,DTop;
  };
  static const TableEntry73a Table_73a_el[11];
  static const TableEntry73a Table_73a_an[11];
  static const TableEntry73b Table_73b_el[ 5];
  static const TableEntry73b Table_73b_an[ 5];
  const TableEntry73a *Tab_73a;
  const TableEntry73b *Tab_73b;

  virtual double		h_Id() const {return -0.0025;};
  virtual double		l_Id() const {return -0.0007;};
  virtual double		h_Is() const {return -0.0022;};
  virtual double		l_Is() const {return -0.0007;};
  virtual double		l_1d() const {return  0.0012;};
  virtual double		l_1s() const {return  0.0024;};
  virtual Vector3		deg3InPhase(const Vector3&, const Vector3&);
  virtual Vector3		deg2OutPhase(const Vector3&, const Vector3&);
  virtual Vector3		latDepend(const Vector3& St, const Vector3& B);
  virtual const Vector3&	makeStep1(SBStation*, SBEphem*);
  virtual const Vector3&	makeStep2(SBStation*, const SBFrame&);
  virtual Vector3		permTide (SBStation*);
public:
  /** A constructor. */
  SBSolidTideLdIERS96(const SBConfig*);
  /** Refers to a class name (debug info) */
  virtual QString ClassName() const {return "SBSolidTideLdIERS96";};
};
/*==============================================================================================*/



/*==============================================================================================*/
/**Tide's load displacements.
 * Inherits Vector3 representing local displacement in TRF.
 */
/*==============================================================================================*/
class SBTideLd : public Vector3
{
protected:
  const SBConfig	*Cfg;
  SBOceanTideLd		*OceanTideLd;
  SBPolarTideLd		*PolarTideLd;
  SBSolidTideLd		*SolidTideLd;
public:
  /**A constructor.
   * Creates a copy of object.
   */
  SBTideLd(const SBConfig*);
  /** A destructor. */
  virtual ~SBTideLd();
  /** Refers to a class name (debug info) */
  virtual QString		ClassName() const {return "SBTideLd";};
  /**Calculates rotational deformation due to Polar Motion corresponds to time T. */
  virtual const Vector3&	operator()(SBStation*, SBEphem*, SBFrame&);
  SBSolidTideLd*		solidTideLd() {return SolidTideLd;};
};
/*==============================================================================================*/


#endif //SB_GEO_TIDE_H
