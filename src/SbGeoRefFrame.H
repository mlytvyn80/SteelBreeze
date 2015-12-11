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
 * Overview:	describes interface of classes necessary for transformation between
 *		CRF and TRF. Includes Precession and Nutation Theories, rotation due
 *		to Polus, Earth roation, NRO, and their matrixes of transormation.
 *
 */

#ifndef SB_GEO_REF_FRAME_H
#define SB_GEO_REF_FRAME_H

#include <config.h>

#include "SbGeneral.H"
#include "SbVector3.H"
#include "SbGeo.H"
#include "SbGeoTime.H"



class SBFrame;
/*==============================================================================================*/
/**Nutation theory (general).
 * General interface for nutation theory implementation.
 * 
 */
/*==============================================================================================*/
class SBNutThGeneral : public SBNamed
{
protected:
  double	DeltaEpsT;		//!< nutation in obliquity (from theory)
  double	DeltaPsiT;		//!< nutation in longitude (from theory)
public:
  /**A constructor.
   * Creates a copy of object.
   */
  SBNutThGeneral(const QString& Name_="Nutation theory general interface") : SBNamed(Name_)
    {DeltaEpsT=DeltaPsiT=0.0;};
  /** A destructor. */
  virtual ~SBNutThGeneral(){};
  /** Refers to a class name (debug info) */
  virtual QString ClassName() const {return "SBNutGeneral";};
  /**Calculates nutation angles for the time T.*/
  virtual void calc(const SBTime&)=0;
  /**Returns calculated DeltaEps.*/
  virtual double deltaEps() const {return DeltaEpsT;};
  /**Returns calculated DeltaPsi.*/
  virtual double deltaPsi() const {return DeltaPsiT;};
};
/*==============================================================================================*/



/*==============================================================================================*/
/**Precession-Nutation theory for NRO (general).
 * General interface for precession-nutation theory implementation.
 * 
 */
/*==============================================================================================*/
class SBNROThGeneral : public SBNamed
{
protected:
  double	X;		//!< X
  double	Y;		//!< Y
  double	S;		//!< s
  double	DT;		//!< dT
public:
  /**A constructor.
   * Creates a copy of object.
   */
  SBNROThGeneral(const QString& Name_="Precession-Nutation NRO theory general interface") : SBNamed(Name_)
    {X=Y=S=0.0;};
  /** A destructor. */
  virtual ~SBNROThGeneral(){};
  /** Refers to a class name (debug info) */
  virtual QString ClassName() const {return "SBNROThGeneral";};
  /**Calculates nutation angles for the time T.*/
  virtual void calc(const SBTime&)=0;
  /**Returns calculated X.*/
  virtual double x() const {return X;};
  /**Returns calculated Y.*/
  virtual double y() const {return Y;};
  /**Returns calculated s.*/
  virtual double s() const {return S;};
  /**Returns calculated dT.*/
  virtual double dT() const {return DT;};
};
/*==============================================================================================*/



/*==============================================================================================*/
/**Solid tide variations in UT1.
 * Calculates solid tide variatons in UT1, Dickman, 1993.
 * 
 */
/*==============================================================================================*/
class SBSTideUT1
{
protected:
  struct TableEntry
  {
    short	N[5];
    double	B,C;
  };
  double	DUT1S;		//!< calculated value of (UT1S-UTC)
  static const TableEntry Tab[62];
public:
  /**A constructor.
   * Creates a copy of object.
   */
  SBSTideUT1(){DUT1S=0.0;};
  /** A destructor. */
  virtual ~SBSTideUT1(){};
  /** Refers to a class name (debug info) */
  virtual QString ClassName() const {return "SBSTideUT1";};
  /**Calculates dUT1S for the time T.*/
  virtual void calc(const SBTime&);
  /**Returns calculated DUT1S.*/
  virtual double dUT1_UT1S(){return DUT1S;};
};
/*==============================================================================================*/



/*==============================================================================================*/
/**Ocean tide variations (diurnal and semidiurnal) in UT1 and Polar Motion.
 * General interface.
 * 
 */
/*==============================================================================================*/
class SBOTideEOPGeneral : public SBNamed
{
protected:
  double	DUT1D;		//!< calculated value of UT1D
  double	DXD;		//!< calculated value of dXD
  double	DYD;		//!< calculated value of dYD
public:
  /**A constructor.
   * Creates a copy of object.
   */
  SBOTideEOPGeneral(const QString& Name_="Diurnal EOP general interface") : SBNamed(Name_)
    {DUT1D=DXD=DYD=0.0;};
  /** A destructor. */
  virtual ~SBOTideEOPGeneral(){};
  /** Refers to a class name (debug info) */
  virtual QString ClassName() const {return "SBOTideEOPGeneral";};
  /**Calculates diurnal EOPs for the time T.*/
  virtual void calc(const SBTime&){};
  /**Returns calculated DUT1D.*/
  virtual double dUT1_UT1D(){return DUT1D;};
  /**Returns calculated dXD.*/
  virtual double dx_xD(){return DXD;};
  /**Returns calculated dYD.*/
  virtual double dy_yD(){return DYD;};
};
/*==============================================================================================*/



/*==============================================================================================*/
/**Ocean tide variations (diurnal and semidiurnal) in UT1 and Polar Motion.
 * General interface for models described in Sovers, Jacobs, 1996.
 *
 */
/*==============================================================================================*/
class SBOTideEOPGenS : public SBOTideEOPGeneral
{
protected:
  struct TableEntry
  {
    short	k[6];
    double	A[3], B[3];
  };
  const TableEntry *Tab;
  int		N;
public:
  SBOTideEOPGenS(const QString& Name_="Diurnal EOP general interface 2") : SBOTideEOPGeneral(Name_)
    {N=0;};
  virtual QString ClassName() const {return "SBOTideEOPGenS";};
  /**Calculates diurnal EOPs for the time T.*/
  virtual void calc(const SBTime&);
};
/*==============================================================================================*/



/*==============================================================================================*/
/**Precession matrix.
 * General interface
 *
 */
/*==============================================================================================*/
class SBPrecession : public Matrix3
{
public:
  /** A constructor.
   * Creates a copy of object.
   */
  SBPrecession() : Matrix3(){};
  /** A destructor. */
  virtual ~SBPrecession(){};
  /** Refers to a class name (debug info) */
  virtual QString ClassName() const {return "SBPrecession";};
  /**Calculates precession angles and returns matrix corresponds to time T. */
  virtual const Matrix3& operator()(SBTime&) = 0;
  virtual const Matrix3& dP_dpsi() = 0;
};
/*==============================================================================================*/



/*==============================================================================================*/
/**SBPrec_IAU1976.
 * Calculates precession quantities and the matrix of the precession theory IAU-1976.
 *
 */
/*==============================================================================================*/
class SBPrec_IAU1976 : public SBPrecession
{
protected:
  double	Zeta;
  double	Z;
  double	Theta;
  RotMatrix	R1;
  RotMatrix	R2;
  RotMatrix	R3;
public:
  /** A constructor.
   *  Creates a copy of object.
   */
  SBPrec_IAU1976();
  /** A destructor. */
  virtual ~SBPrec_IAU1976(){};
  /** Refers to a class name (debug info) */
  virtual QString ClassName() const {return "SBPrec_IAU1976";};
  /**Calculates precession angles and returns matrix corresponds to time T. */
  virtual const Matrix3& operator()(SBTime&);
  /**Returns Zeta (for debug purposes). */
  double	zeta()  const {return Zeta;};
  /**Returns Z (for debug purposes). */
  double	z()     const {return Z;};
  /**Returns Theta (for debug purposes). */
  double	theta() const {return Theta;};
  virtual const Matrix3& dP_dpsi() {return R1;};
};
/*==============================================================================================*/



/*==============================================================================================*/
/**SBPrec_IAU2000.
 * Calculates precession quantities and the matrix of the precession theory IAU-2000(sp?).
 *
 */
/*==============================================================================================*/
class SBPrec_IAU2000 : public SBPrecession
{
protected:
  double	t;
  double	Psi;
  double	Omega;
  double	Chi;
  RotMatrix	R1;
  RotMatrix	R2;
  RotMatrix	R3;
  RotMatrix	R4;
  Matrix3	DP_dPsi;

public:
  /** A constructor.
   * Creates a copy of object.
   */
  SBPrec_IAU2000();
  /** A destructor. */
  virtual ~SBPrec_IAU2000(){};
  /** Refers to a class name (debug info) */
  virtual QString ClassName() const {return "SBPrec_IAU2000";};
  /**Calculates precession angles and returns matrix corresponds to time T. */
  virtual const Matrix3& operator()(SBTime&);
  /**Returns Zeta (for debug purposes). */
  double	psi() const {return Psi;};
  /**Returns Z (for debug purposes). */
  double	omega() const {return Omega;};
  /**Returns Theta (for debug purposes). */
  double	chi() const {return Chi;};
  virtual const Matrix3& dP_dpsi();
};
/*==============================================================================================*/



class SBEOP;
class SBConfig;
/*==============================================================================================*/
/**Nutation matrix.
 * General interface.
 * 
 */
/*==============================================================================================*/
class SBNutation : public Matrix3
{
protected:
  const SBConfig	*Cfg;
  double		Eps;			//!< mean obliquity
  double		DeltaEps;		//!< nutation in obliquity
  double		DeltaPsi;		//!< nutation in longitude
  double		AlphaE;			//!< equation of equinox
  static const double	EphemEps;		//!< obliquity at J2000.0
  SBNutThGeneral	*Theory;		//!< nutation theory
  SBEOP			*EOP;			//!< EOP database
  RotMatrix		R1;
  RotMatrix		R2;
  RotMatrix		R3;
  RotDerMatrix		DR2dpsi;
  RotDerMatrix		DR3deps;
  double		DPsi;
  double		DEps;
  //
  double		GMST;			//!< Greenwich Mean Sidereal Time
  double		GST;			//!< Greenwich Sidereal Time
  double		R;			//!< 

public:
  /**A constructor.
   * Creates a copy of object.
   */
  SBNutation(const SBConfig*, SBEOP* =NULL);
  /** A destructor. */
  virtual ~SBNutation(){if (Theory) delete Theory; EOP=NULL;};
  /** Refers to a class name (debug info) */
  virtual QString ClassName() const {return "SBNutation";};
  /**Calculates nutation angles and returns matrix corresponds to time T. */
  virtual const Matrix3& operator()(const SBTime&) = 0;
  /**Calcs geodesic nutation.*/
  virtual double geodesicNutation(const SBTime&) {return 0.0;};
  /**Returns obliquity at J2000.0.*/
  static double ephemEps() {return EphemEps;};
  /**Returns current mean obliquity.*/
  virtual double meanEps() const {return Eps;};
  /**Returns current true obliquity.*/
  virtual double trueEps() const {return Eps + DeltaEps;};
  /**Returns current DeltaEps.*/
  virtual double deltaEps() const {return DeltaEps;};
  /**Returns current DeltaPsi.*/
  virtual double deltaPsi() const {return DeltaPsi;};
  /**Returns current equation of equinoxes.*/
  virtual double eqEquinox() const {return AlphaE;};
  /**Returns Greenwich Mean Sidereal Time.*/
  virtual double gmst() const {return GMST;};
  /**Returns Greenwich Mean Sidereal Time.*/
  virtual double gst () const {return GST;};
  virtual double r() const {return R;};
  virtual void	 calcGST(const SBTime&);

  const SBNutThGeneral* theory() const {return Theory;};

  virtual const Matrix3& r1() const {return R1;};
  virtual const Matrix3& r2() const {return R2;};
  virtual const Matrix3& r3() const {return R3;};

  virtual const Matrix3& dR3deps() const {return DR3deps;};
  virtual const Matrix3& dR2dpsi() const {return DR2dpsi;};
  
  virtual double dPsi() const {return DPsi;};
  virtual double dEps() const {return DEps;};
  //
};
/*==============================================================================================*/



/*==============================================================================================*/
/**Nutation matrix.
 * Calculates nutation quantities and the matrix of the nutation for the theory IAU-1980.
 * 
 */
/*==============================================================================================*/
class SBNut_IAU1980 : public SBNutation
{
protected:
  
public:
  /**A constructor.
   * Creates a copy of object.
   */
  SBNut_IAU1980(const SBConfig*, SBEOP* =NULL);
  /** A destructor. */
  virtual ~SBNut_IAU1980(){};
  /** Refers to a class name (debug info) */
  virtual QString ClassName() const {return "SBNut_IAU1980";};
  /**Calculates nutation angles and returns matrix corresponds to time T. */
  virtual const Matrix3& operator()(const SBTime&);
  /**Calcs geodesic nutation.*/
  virtual double geodesicNutation(const SBTime&);
};
/*==============================================================================================*/



/*==============================================================================================*/
/**Nutation matrix.
 * Calculates nutation quantities and the matrix of the nutation for the theory IAU-2000.
 * 
 */
/*==============================================================================================*/
class SBNut_IAU2000 : public SBNutation
{
protected:
  
public:
  /**A constructor.
   * Creates a copy of object.
   */
  SBNut_IAU2000(const SBConfig*, SBEOP* =NULL);
  /** A destructor. */
  virtual ~SBNut_IAU2000(){};
  /** Refers to a class name (debug info) */
  virtual QString ClassName() const {return "SBNut_IAU2000";};
  /**Calculates nutation angles and returns matrix corresponds to time T. */
  virtual const Matrix3& operator()(const SBTime&);
  virtual void	 calcGST(const SBTime&);
};
/*==============================================================================================*/



/*==============================================================================================*/
/**Precession-Nutation matrix for NRO.
 * Calculates combined precession-nutation matrix in the case of NRO.
 * 
 */
/*==============================================================================================*/
class SBPrecNutNRO : public Matrix3
{
protected:
  const SBConfig	*Cfg;
  double		X;			//!< X coordinate of the CEP in the CRS
  double		Y;			//!< Y coordinate of the CEP in the CRS
  double		S;			//!< s
  double		DX;
  double		DY;
  SBNROThGeneral	*Theory;		//!< theoretical values of X, Y and s
  SBEOP			*EOP;			//!< EOP database
  RotMatrix		R;
  Matrix3		Q;
public:
  /**A constructor.
   * Creates a copy of object.
   */
  SBPrecNutNRO(const SBConfig*, SBEOP* =NULL);
  /** A destructor. */
  virtual ~SBPrecNutNRO(){if (Theory) delete Theory; EOP=NULL;};
  /** Refers to a class name (debug info) */
  virtual QString ClassName() const {return "SBPrecNutNRO";};
  /**Calculates nutation angles and returns matrix corresponds to time T. */
  virtual const Matrix3& operator()(const SBTime&);
  /**Calcs geodesic nutation.*/
  virtual double geodesicNutation(const SBTime&);
  /**Returns obliquity at J2000.0.*/
  virtual double x() const {return X;};
  /**Returns current mean obliquity.*/
  virtual double y() const {return Y;};
  /**Returns current true obliquity.*/
  virtual double s() const {return S;};
  /**Returns current DeltaEps.*/
  virtual double dX() const {return DX;};
  virtual double dY() const {return DY;};
  const SBNROThGeneral* theory() const {return Theory;};
  virtual const Matrix3& rs()	const {return R;};
  virtual const Matrix3& q()	const {return Q;};
};
/*==============================================================================================*/



/*==============================================================================================*/
/**Polus rotation matrix (general).
 * Matrix describes rotation due to polar motion (dX and dY).
 * 
 */
/*==============================================================================================*/
class SBPolus : public Matrix3
{
protected:
  RotMatrix		R1;
  RotMatrix		R2;
  RotDerMatrix		DR1dy;
  RotDerMatrix		DR2dx;
  double		Xp;
  double		Yp;
  SBEOP			*EOP;			//!< EOP database

public:
  /**A constructor.*/
  SBPolus(SBEOP*);
  /** A destructor. */
  virtual ~SBPolus(){};
  /** Refers to a class name (debug info) */
  virtual QString ClassName() const {return "SBPolus";};
  /**Returns matrix for the time T.
   * Returns matrix R1(y_p)*R2(x_p) for the time T. Should be considered as Polus.calc(T).
   */
  virtual const Matrix3& operator()(const SBTime&, SBFrame*);
  /**Returns X-pole coordinate (splined EOP + estimated) without diurnals.*/
  virtual double xp() const {return Xp;};
  /**Returns Y-pole coordinate (splined EOP + estimated) without diurnals.*/
  virtual double yp() const {return Yp;};

  virtual const Matrix3& r1y() const {return R1;};
  virtual const Matrix3& r2x() const {return R2;};

  virtual const Matrix3& dR1dy() const {return DR1dy;};
  virtual const Matrix3& dR2dx() const {return DR2dx;};
};
/*==============================================================================================*/



/*==============================================================================================*/
/**NRO polus rotation matrix.
 * Matrix describes rotation due to polar motion (dX and dY).
 * 
 */
/*==============================================================================================*/
class SBPolusNRO : public Matrix3
{
protected:
  RotMatrix		R1;
  RotMatrix		R2;
  RotMatrix		R3;
  RotDerMatrix		DR1dy;
  RotDerMatrix		DR2dx;
  double		Xp;
  double		Yp;
  double		s_prime;
  SBEOP			*EOP;			//!< EOP database

public:
  /**A constructor.*/
  SBPolusNRO(SBEOP*);
  /** A destructor. */
  virtual ~SBPolusNRO(){};
  /** Refers to a class name (debug info) */
  virtual QString ClassName() const {return "SBPolusNRO";};
  /**Returns matrix for the time T.
   * Returns matrix R1(y_p)*R2(x_p) for the time T. Should be considered as Polus.calc(T).
   */
  virtual const Matrix3& operator()(const SBTime&, SBFrame*);
  /**Returns X-pole coordinate (splined EOP + estimated) without diurnals.*/
  virtual double xp() const {return Xp;};
  /**Returns Y-pole coordinate (splined EOP + estimated) without diurnals.*/
  virtual double yp() const {return Yp;};

  virtual const Matrix3& r1y() const {return R1;};
  virtual const Matrix3& r2x() const {return R2;};
  virtual const Matrix3& r3s() const {return R3;};
  virtual const Matrix3& dR1dy() const {return DR1dy;};
  virtual const Matrix3& dR2dx() const {return DR2dx;};
};
/*==============================================================================================*/



/*==============================================================================================*/
/**Earth rotation matrix (general).
 * Matrix describes Earth rotation.
 * 
 */
/*==============================================================================================*/
class SBRotate : public RotMatrix
{
private:
  RotDerMatrix		DRdt;
public:
  /** A constructor.*/
  SBRotate() : RotMatrix(Z_AXIS), DRdt(Z_AXIS){};
  /** Refers to a class name (debug info) */
  virtual QString ClassName() const {return "SBRotate";};
  virtual RotDerMatrix& dRdt() {return DRdt;};
};
/*==============================================================================================*/


class SBRunManager;
/*==============================================================================================*/
/**Matrix transformation TRS to CRS (i.e., [CRS] = *this*[TRS]), general.
 * Describes general interface for system transformation.
 * 
 */
/*==============================================================================================*/
class SBFrame : public Matrix3, public SBNamed
{
protected:
  const SBConfig	*Cfg;
  SBRunManager		*Mgr;
  SBTime		Time;
  SBOTideEOPGeneral	*EOPdiurnal;
  SBOTideEOPGeneral	*EOPdiurnalAtm;
  SBOTideEOPGeneral	*EOPdiurnalNTd;
  SBOTideEOPGeneral	*EOPdiurnalGrv;

  SBEOP			*EOP;
  double		UT1_UTC;
  Matrix3		CRS2TRS;
  Matrix3		DQ_dt;		// dQ/dt

  //derivatives:
  Matrix3		DQ_dUT;		// dQ/dUT
  Matrix3		DQ_dxp;		// dQ/dxp
  Matrix3		DQ_dyp;		// dQ/dyp
  Matrix3		DQ_dpsi;	// dQ/d(dPsi)
  Matrix3		DQ_deps;	// dQ/d(dEps)

  Matrix3		DQ_dprec;

public:
  /**A constructor.
   * Creates a copy of object.
   */
  SBFrame(const SBConfig*, SBRunManager*, const QString& ="Frame transformation general interface");
  /**A destructor. */
  virtual ~SBFrame();
  /**Refers to a class name (debug info) */
  virtual QString ClassName() const {return "SBFrame";};
  /**Calculates matrixes for the time T.*/
  virtual void calc(const SBMJD&)=0;
  /**Returns Apparent Greenwich Sidereal Time, rad */
  virtual double gst() const =0; 
  /**Returns Mean Greenwich Sidereal Time, rad */
  virtual double gmst() const =0;
  /**Returns diurnal variations in UT1. */
  virtual double diurnalDUT1() const
    {return 
       EOPdiurnal   ->dUT1_UT1D() + 
       EOPdiurnalAtm->dUT1_UT1D() + 
       EOPdiurnalNTd->dUT1_UT1D() + 
       EOPdiurnalGrv->dUT1_UT1D();};
  /**Returns diurnal variations in X-pole. */
  virtual double diurnalDX() const 
    {return 
       EOPdiurnal   ->dx_xD() + 
       EOPdiurnalAtm->dx_xD() + 
       EOPdiurnalNTd->dx_xD() + 
       EOPdiurnalGrv->dx_xD();};
  /**Returns diurnal variations in Y-pole. */
  virtual double diurnalDY() const 
    {return 
       EOPdiurnal   ->dy_yD() +
       EOPdiurnalAtm->dy_yD() + 
       EOPdiurnalNTd->dy_yD() + 
       EOPdiurnalGrv->dy_yD();};
  /**Returns time T.*/
  const SBTime&	time() const {return Time;};
  SBEOP*	eop(){return EOP;};
  const SBOTideEOPGeneral* eopDiurnal() const {return EOPdiurnal;};
  virtual const Matrix3& crs2trs() const {return CRS2TRS;};
  virtual const Matrix3& dQ_dt() const {return DQ_dt;};
  virtual const Matrix3& dQ_dUT() const {return DQ_dUT;};
  virtual const Matrix3& dQ_dxp() {return DQ_dxp;};
  virtual const Matrix3& dQ_dyp() {return DQ_dyp;};
  virtual const Matrix3& dQ_dpsi() {return DQ_dpsi;};
  virtual const Matrix3& dQ_deps() {return DQ_deps;};

  virtual const Matrix3& dQ_dprec() {return DQ_dprec;};

  virtual double polar_X() const =0;
  virtual double polar_Y() const =0;
  virtual double polar_UT1_UTC() const {return UT1_UTC;};
  virtual double dNutation_Psi() const =0;
  virtual double dNutation_Eps() const =0;
  // for needs of that f. SINEX:
  virtual double totNutationLongitude() const =0;
  virtual double totNutationObliquity() const =0;
};
/*==============================================================================================*/



/*==============================================================================================*/
/**Matrix transformation TRS to CRS (i.e., [CRS] = *this*[TRS]).
 * Classical transformation.
 * 
 */
/*==============================================================================================*/
class SBFrameClassic : public SBFrame
{
protected:
  SBPrecession		*P;
  SBNutation		*N;
  SBRotate		R;
  SBPolus		W;

  // aux
  double		DAleph0;
  double		DPsi0;
  double		DEpsilon0;
  RotMatrix		R1;
  RotMatrix		R2;
  RotMatrix		R3;
  Matrix3		Bias;
  // derivatives:
  

public:
  /**A constructor.
   * Creates a copy of object.
   */
  SBFrameClassic(const SBConfig*, SBRunManager*, const QString& ="Classical coordinate transformation");
  /**A destructor. */
  virtual ~SBFrameClassic();
  /**Refers to a class name (debug info) */
  virtual QString ClassName() const {return "SBFrameClassic";};
  /**Calculates matrixes for the time T (UTC).*/
  virtual void calc(const SBMJD&);
  /**Returns matrix of precession.*/
  virtual const SBPrecession&	p()	const {return *P;};
  /**Returns matrix of nutation.*/
  virtual const SBNutation&	n()	const {return *N;};
  /**Returns matrix of rotation.*/
  virtual const SBRotate&	r()	const {return R;};
  /**Returns matrix of polar motion.*/
  virtual const SBPolus&	w()	const {return W;};
  /**Returns Apparent Greenwich Sidereal Time, rad */
  virtual double gst() const {return N->gst();};
  /**Returns Mean Greenwich Sidereal Time, rad */
  virtual double gmst() const {return N->gmst();};

  virtual const Matrix3& dQ_dxp();
  virtual const Matrix3& dQ_dyp();
  virtual const Matrix3& dQ_dpsi();
  virtual const Matrix3& dQ_deps();

  virtual const Matrix3& dQ_dprec();

  //
  virtual double polar_X() const {return W.xp();};
  virtual double polar_Y() const {return W.yp();};
  virtual double dNutation_Psi() const {return N->dPsi();};
  virtual double dNutation_Eps() const {return N->dEps();};
  virtual double totNutationLongitude() const {return N->deltaPsi();};
  virtual double totNutationObliquity() const {return N->deltaEps();};
};
/*==============================================================================================*/



/*==============================================================================================*/
/**Matrix transformation TRS to CRS (i.e., [CRS] = *this*[TRS]).
 * NRO transformation.
 * 
 */
/*==============================================================================================*/
class SBFrameNRO : public SBFrame
{
protected:
  double		ERA;
  SBPrecNutNRO		PN;
  SBRotate		R;
  SBPolusNRO		W;
public:
  /**A constructor.
   * Creates a copy of object.
   */
  SBFrameNRO(const SBConfig *Cfg_, SBRunManager *Mgr_, const QString& Name_="NRO coordinate transformation")
    : SBFrame(Cfg_, Mgr_, Name_), PN(Cfg_, EOP), R(), W(EOP){ERA=0.0;};
  /**A destructor. */
  virtual ~SBFrameNRO(){};
  /**Refers to a class name (debug info) */
  virtual QString ClassName() const {return "SBFrameNRO";};
  /**Calculates matrixes for the time T (UTC).*/
  virtual void calc(const SBMJD&);
  /**Returns matrix of precession-nutation.*/
  virtual const SBPrecNutNRO&	pn()	const {return PN;};
  /**Returns matrix of rotation.*/
  virtual const SBRotate&	r()	const {return R;};
  /**Returns matrix of polar motion.*/
  virtual const SBPolusNRO&	w()	const {return W;};
  /**Returns Earth Rotation Angle, rad */
  virtual double era() const {return ERA;};
  /**Returns Apparent Greenwich Sidereal Time, rad */
  virtual double gst() const {return era();};
  /**Returns Mean Greenwich Sidereal Time, rad */
  virtual double gmst() const {return era();};

  virtual const Matrix3& dQ_dxp();
  virtual const Matrix3& dQ_dyp();
  virtual const Matrix3& dQ_dpsi();
  virtual const Matrix3& dQ_deps();

  virtual double polar_X() const {return W.xp();};
  virtual double polar_Y() const {return W.yp();};
  virtual double dNutation_Psi() const {return PN.dX();};
  virtual double dNutation_Eps() const {return PN.dY();};
  virtual double totNutationLongitude() const {return PN.x();};
  virtual double totNutationObliquity() const {return PN.y();};
};
/*==============================================================================================*/







// real models:
/*==============================================================================================*/
/**IAU 1980 Nutation theory.
 * Implements The IAU 1980 Theory of Nutation.
 */
/*==============================================================================================*/
class SBNutThIAU1980 : public SBNutThGeneral
{
protected:
  struct TableEntry
  {
    short	N[5];
    double	A[2],B[2];
  };
  static const TableEntry Tab[106];
public:
  SBNutThIAU1980() : SBNutThGeneral("The IAU 1980 Theory of Nutation"){};
  virtual QString ClassName() const {return "SBNutThIAU1980";};
  virtual void calc(const SBTime&);
};
/*==============================================================================================*/



/*==============================================================================================*/
/**IAU 1980 Nutation theory.
 * Implements The IAU 1980 Theory of Nutation.
 */
/*==============================================================================================*/
class SBNROThIAU1980 : public SBNROThGeneral
{
  //protected:
  //  struct TableEntry
  //  {
  //    short	N[5];
  //    double	A[2],B[2];
  //  };
  //  static const TableEntry Tab[106];
public:
  SBNROThIAU1980() : SBNROThGeneral("The IAU 1980 Theory of Nutation"){};
  virtual QString ClassName() const {return "SBNROThIAU1980";};
  virtual void calc(const SBTime&){};
};
/*==============================================================================================*/



/*==============================================================================================*/
/**IAU 1996 Nutation theory.
 * Implements The IAU 1996 Theory of Nutation.
 */
/*==============================================================================================*/
class SBNutThIAU1996 : public SBNutThGeneral
{
protected:
  struct TableEntry
  {
    short	N[5];
    double	A[2],B[2],C[2];
  };
  struct PlanetTableEntry
  {
    short	N[10];
    double	A[2],B[2];
  };
  static const TableEntry Tab[263];
  static const PlanetTableEntry PlTab[112];
public:
  SBNutThIAU1996() : SBNutThGeneral("The IAU 1996 Theory of Nutation") {};
  virtual QString ClassName() const {return "SBNutThIAU1996";};
  virtual void calc(const SBTime&);
};
/*==============================================================================================*/



/*==============================================================================================*/
/**IAU 1996 Nutation theory.
 * Implements The IAU 1996 Theory of Nutation.
 */
/*==============================================================================================*/
class SBNROThIAU1996 : public SBNROThGeneral
{
protected:
  //  struct TableEntry
  //  {
  //    short	N[5];
  //    double	A[2],B[2],C[2];
  //  };
  //  struct PlanetTableEntry
  //  {
  //    short	N[10];
  //    double	A[2],B[2];
  //  };
  //  static const TableEntry Tab[263];
  //  static const PlanetTableEntry PlTab[112];
public:
  SBNROThIAU1996() : SBNROThGeneral("The IAU 1996 Theory of Nutation") {};
  virtual QString ClassName() const {return "SBNROThIAU1996";};
  virtual void calc(const SBTime&){};
};
/*==============================================================================================*/



/*==============================================================================================*/
/**IAU 2000A Nutation theory.
 * Implements The IAU 2000A Theory of Nutation.
 */
/*==============================================================================================*/
class SBNutThIAU2000A : public SBNutThGeneral
{
protected:
  struct SolLunTableEntry
  {
    short	N[5];
    double	Psi_in[2],Eps_in[2],Psi_out[2],Eps_out[2];
  };
  struct PlanetTableEntry
  {
    short	N[14];
    double	A[2],B[2];
  };
  static const int NSolLun;
  static const int NPlanet;
  static const SolLunTableEntry SLTab[];
  static const PlanetTableEntry PlTab[];
public:
  SBNutThIAU2000A() : SBNutThGeneral("The IAU 2000-A Theory of Nutation") {};
  virtual QString ClassName() const {return "SBNutThIAU2000A";};
  virtual void calc(const SBTime&);
};
/*==============================================================================================*/



/*==============================================================================================*/
/**IAU 2000A Nutation theory.
 * Implements The IAU 2000A Theory of Nutation.
 */
/*==============================================================================================*/
class SBNROThIAU2000A : public SBNROThGeneral
{
protected:
  struct GeneralTableEntry
  {
    double	S, C;		//!< sin & cos coefficients
    short	N[14];		//!< multipliers
  };
  static const int NXT0;
  static const int NXT1;
  static const int NXT2;
  static const int NXT3;
  static const int NXT4;

  static const int NYT0;
  static const int NYT1;
  static const int NYT2;
  static const int NYT3;
  static const int NYT4;

  static const int NDThetaT0;
  static const int NDThetaT1;
  static const int NDThetaT2;
  static const int NDThetaT3;
  static const int NDThetaT4;

  static const int NDtT0;
  static const int NDtT1;

  static const GeneralTableEntry XTabT0[];
  static const GeneralTableEntry XTabT1[];
  static const GeneralTableEntry XTabT2[];
  static const GeneralTableEntry XTabT3[];
  static const GeneralTableEntry XTabT4[];

  static const GeneralTableEntry YTabT0[];
  static const GeneralTableEntry YTabT1[];
  static const GeneralTableEntry YTabT2[];
  static const GeneralTableEntry YTabT3[];
  static const GeneralTableEntry YTabT4[];

  static const GeneralTableEntry DThetaTabT0[];
  static const GeneralTableEntry DThetaTabT1[];
  static const GeneralTableEntry DThetaTabT2[];
  static const GeneralTableEntry DThetaTabT3[];
  static const GeneralTableEntry DThetaTabT4[];

  static const GeneralTableEntry DtTabT0[];
  static const GeneralTableEntry DtTabT1[];

  double sum(int, const GeneralTableEntry*, double*);
public:
  SBNROThIAU2000A() : SBNROThGeneral("The IAU 2000-A Theory of Nutation") {};
  virtual QString ClassName() const {return "SBNROThIAU2000A";};
  virtual void calc(const SBTime&);
};
/*==============================================================================================*/



/*==============================================================================================*/
/**Ocean tide variations (diurnal and semidiurnal) in UT1 and Polar Motion.
 * Model of Ray, 1994 (IERS Conventions, 1996 implementation).
 * 
 */
/*==============================================================================================*/
class SBOTideEOPRay1994_1996 : public SBOTideEOPGeneral
{
protected:
  struct TableEntry
  {
    short	c[6];
    double	Phi;
    double	D,E,F,G,H,K; // well, we're trying to keep here the IERS Conventions notation
  };
  static const TableEntry Tab[8];
public:
  SBOTideEOPRay1994_1996() : SBOTideEOPGeneral("Diurnal EOP variations, Ray, 1994 (1996)"){};
  virtual QString ClassName() const {return "SBOTideEOPRay1994_1996";};
  /**Calculates diurnal EOPs for the time T.*/
  virtual void calc(const SBTime&);
};
/*==============================================================================================*/



/*==============================================================================================*/
/**Ocean tide variations (diurnal and semidiurnal) in UT1 and Polar Motion.
 * Model of Ray, 1994 (IERS Conventions, 2003 implementation).
 * 
 */
/*==============================================================================================*/
class SBOTideEOPRay1994_2003 : public SBOTideEOPGeneral
{
protected:
  struct TableEntry
  {
    short	k[6];
    double	A,B,C,D,E,F; // well, we're trying to keep here the IERS Conventions notation
  };
  static const TableEntry Tab[71];
public:
  SBOTideEOPRay1994_2003() : SBOTideEOPGeneral("Diurnal EOP variations, Ray, 1994 (2003)"){};
  virtual QString ClassName() const {return "SBOTideEOPRay1994_2003";};
  /**Calculates diurnal EOPs for the time T.*/
  virtual void calc(const SBTime&);
};
/*==============================================================================================*/



/*==============================================================================================*/
/**Ocean tide variations (diurnal and semidiurnal) in UT1 and Polar Motion.
 * JPL-92, Sovers at al., 1993 model.
 */
/*==============================================================================================*/
class SBOTideEOP_JPL92 : public SBOTideEOPGenS
{
protected:
  static const TableEntry TabC[8];
public:
  SBOTideEOP_JPL92() : SBOTideEOPGenS("Diurnal EOP variations, JPL-92, Sovers at al., 1993")
    {N=8;Tab=TabC;};
};
/*==============================================================================================*/



/*==============================================================================================*/
/**Ocean tide variations (diurnal and semidiurnal) in UT1 and Polar Motion.
 * Herring-Dong, 1994 model
 */
/*==============================================================================================*/
class SBOTideEOP_HD1994 : public SBOTideEOPGenS
{
protected:
  static const TableEntry TabC[10];
public:
  SBOTideEOP_HD1994() : SBOTideEOPGenS("Diurnal EOP variations, Herring-Dong, 1994")
    {N=sizeof(TabC)/sizeof(struct TableEntry); Tab=TabC;};
};
/*==============================================================================================*/



/*==============================================================================================*/
/**Ocean tide variations (diurnal and semidiurnal) in UT1 and Polar Motion.
 * GSFC-95, Ma, 1995 model
 */
/*==============================================================================================*/
class SBOTideEOP_GSFC95 : public SBOTideEOPGenS
{
protected:
  static const TableEntry TabC[10];
public:
  SBOTideEOP_GSFC95() : SBOTideEOPGenS("Diurnal EOP variations, GSFC-95, Ma, 1995")
    {N=sizeof(TabC)/sizeof(struct TableEntry); Tab=TabC;};
};
/*==============================================================================================*/



/*==============================================================================================*/
/**Ocean tide variations (diurnal and semidiurnal) in UT1 and Polar Motion.
 * Gipson 1995 model
 */
/*==============================================================================================*/
class SBOTideEOP_Gipson1995 : public SBOTideEOPGenS
{
protected:
  static const TableEntry TabC[10];
public:
  SBOTideEOP_Gipson1995() : SBOTideEOPGenS("Diurnal EOP variations, Gipson, 1996")
    {N=sizeof(TabC)/sizeof(struct TableEntry); Tab=TabC;};
};
/*==============================================================================================*/


/*==============================================================================================*/
/**Atmospheric tide variations (diurnal and semidiurnal) in UT1 and Polar Motion.
 * Brzezinski, Bizouard and Petrov (2002) model.
 * http://hpiers.obspm.fr/eop-pc/models/PM/PM_atmos_hf_tab.html
 * http://hpiers.obspm.fr/eop-pc/models/UT1/UT1_atmos_hf_tab.html
 * 
 */
/*==============================================================================================*/
class SBATideEOP_BBP_02 : public SBOTideEOPGeneral
{
protected:
  struct TableEntry
  {
    short	k[6];
    double	A_in, A_op, a, b;
  };
  static const TableEntry Tab[6];
public:
  SBATideEOP_BBP_02() : SBOTideEOPGeneral("Diurnal Atmospheric EOP variations"){};
  virtual QString ClassName() const {return "SBATideEOP_BBP_02";};
  /**Calculates diurnal EOPs for the time T.*/
  virtual void calc(const SBTime&);
};
/*==============================================================================================*/



/*==============================================================================================*/
/**Non tidal variations (diurnal and semidiurnal) in UT1 and Polar Motion.
 * de Viron et al. (2002) model.
 * http://hpiers.obspm.fr/eop-pc/models/PM/PM_oceanic_nt_hf_tab.html
 * http://hpiers.obspm.fr/eop-pc/models/UT1/UT1_oceanic_nt_hf_tab.html
 * 
 */
/*==============================================================================================*/
class SBNonTideEOP_Viron_02 : public SBOTideEOPGeneral
{
protected:
  struct TableEntry
  {
    short	k[6];
    double	A_in, A_op, a, b;
  };
  static const TableEntry Tab[1];
public:
  SBNonTideEOP_Viron_02() : SBOTideEOPGeneral("Diurnal Non-Tidal EOP variations"){};
  virtual QString ClassName() const {return "SBNonTideEOP_Viron_02";};
  /**Calculates diurnal EOPs for the time T.*/
  virtual void calc(const SBTime&);
};
/*==============================================================================================*/



/*==============================================================================================*/
/**Luni-Solar gravitational tides in [UT1 and] Polar Motion.
 * Brzezinski et al. (2002) model.
 * http://hpiers.obspm.fr/eop-pc/models/PM/PM_gravi_tab.html
 *
 */
/*==============================================================================================*/
class SBGravTideEOP_B_02 : public SBOTideEOPGeneral
{
protected:
  struct TableEntry
  {
    short	k[6];
    double	SinX, CosX, SinY, CosY;
  };
  static const TableEntry Tab[10];
public:
  SBGravTideEOP_B_02() : SBOTideEOPGeneral("Diurnal Grav-Tidal EOP variations"){};
  virtual QString ClassName() const {return "SBGravTideEOP_B_02";};
  /**Calculates diurnal EOPs for the time T.*/
  virtual void calc(const SBTime&);
};
/*==============================================================================================*/






/*==============================================================================================*/

#endif //SB_GEO_REF_FRAME_H
