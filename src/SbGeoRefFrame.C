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

#include "SbGeoRefFrame.H"
#include "SbSetup.H"
#include "SbGeoEop.H"
#include "SbGeoProject.H"
#include <math.h>



const double SBNutation::EphemEps = 84381.448*SEC2RAD;






/*==============================================================================================*/
/*												*/
/* class SBPrec_IAU1976 implementation								*/
/*												*/
/*==============================================================================================*/
SBPrec_IAU1976::SBPrec_IAU1976() : SBPrecession(), R1(Z_AXIS), R2(Y_AXIS), R3(Z_AXIS)
{
  Zeta	= 0.0;
  Z	= 0.0;
  Theta	= 0.0;
};

const Matrix3& SBPrec_IAU1976::operator()(SBTime& T)
{
  double t = (T.TT() - TEphem)/36525.0;
  //  double t = (T.TDB() - TEphem)/36525.0;
  double t2 = t*t, t3=t*t2;
  /*
    IERS Technical Note 21,
    IERS Conventions (1996)
    page 22 says:
    "  The numerical expression for the precession quantities Dzeta, Theta, Zet and Epsilon
    have been given by Lieske et al. (1977) as functions of two time parameters t and T (the 
    last parameter representing Julian centuries from J2000.0 to an arbitrary epoch). The 
    simplified expressions when the arbitrary epoch is chosen to be J2000.0 (i.e. T=0) are"
  */
  Zeta  = SEC2RAD*(2306.2181*t + 0.30188*t2 + 0.017998*t3);
  Theta	= SEC2RAD*(2004.3109*t - 0.42665*t2 - 0.041833*t3);
  Z	= SEC2RAD*(2306.2181*t + 1.09468*t2 + 0.018203*t3);

  (Matrix3&)*this = R1(Zeta)*R2(-Theta)*R3(Z);

  // make a report:
  if (Log->isEligible(SBLog::DBG, SBLog::PRECESSION))
    {
      //      Log->write(SBLog::DBG, SBLog::PRECESSION, ClassName() + ": Arg(TT): " + T.TT().toString());
      Log->write(SBLog::DBG, SBLog::PRECESSION, ClassName() + ": Arg(TDB): " + T.TDB().toString());
      Log->write(SBLog::DBG, SBLog::PRECESSION, ClassName() + ": Zeta    : " + 
		 QString().sprintf("%16.8f (arcsec)", Zeta/SEC2RAD));
      Log->write(SBLog::DBG, SBLog::PRECESSION, ClassName() + ": Theta   : " + 
		 QString().sprintf("%16.8f (arcsec)", Theta/SEC2RAD));
      Log->write(SBLog::DBG, SBLog::PRECESSION, ClassName() + ": Z       : " + 
		 QString().sprintf("%16.8f (arcsec)", Z/SEC2RAD));


      Log->write(SBLog::DBG, SBLog::NUTATION, ClassName() + ": R1.angle: " + 
		 QString().sprintf("%.16f (arcsec)", R1.angle()/SEC2RAD));
      Log->write(SBLog::DBG, SBLog::NUTATION, ClassName() + ": R1.angle: " + 
		 QString().sprintf("%.20f (rad)", R1.angle()));
      R1.report2Log(SBLog::DBG, SBLog::NUTATION, ClassName() + ": R1(Zeta): ");

      Log->write(SBLog::DBG, SBLog::NUTATION, ClassName() + ": R2.angle: " + 
		 QString().sprintf("%.16f (arcsec)", R2.angle()/SEC2RAD));
      Log->write(SBLog::DBG, SBLog::NUTATION, ClassName() + ": R2.angle: " + 
		 QString().sprintf("%.20f (rad)", R2.angle()));
      R2.report2Log(SBLog::DBG, SBLog::NUTATION, ClassName() + ": R2(-Tht): ");

      Log->write(SBLog::DBG, SBLog::NUTATION, ClassName() + ": R3.angle: " + 
		 QString().sprintf("%.16f (arcsec)", R3.angle()/SEC2RAD));
      Log->write(SBLog::DBG, SBLog::NUTATION, ClassName() + ": R3.angle: " + 
		 QString().sprintf("%.20f (rad)", R3.angle()));
      R3.report2Log(SBLog::DBG, SBLog::NUTATION, ClassName() + ": R3(Z)   : ");


      this->report2Log(SBLog::DBG, SBLog::PRECESSION, ClassName() + ": matrix  : ");
    };
  return *this;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBPrec_IAU2000 implementation								*/
/*												*/
/*==============================================================================================*/
SBPrec_IAU2000::SBPrec_IAU2000() : SBPrecession(), R1(X_AXIS), R2(Z_AXIS), R3(X_AXIS), R4(Z_AXIS),
  DP_dPsi(m3E)
{
  t	= 0.0;
  Psi	= 0.0;
  Omega	= 0.0;
  Chi	= 0.0;
};

const Matrix3& SBPrec_IAU2000::operator()(SBTime& T)
{
  double t = (T.TT() - TEphem)/36525.0;
  //  t = (T.TDB() - TEphem)/36525.0;
  double t2 = t*t;
  double t3 = t2*t;

  Chi   = (10.5526*t - 2.38064*t2 - 0.001125*t3)*SEC2RAD;
  Omega = SBNutation::ephemEps() + (-0.02524*t + 0.05127*t2 - 0.007726*t3)*SEC2RAD;
  Psi   = (5038.47875*t - 1.07259*t2 - 0.001147*t3)*SEC2RAD;
  //  Psi   = ((5038.47875 + T.eop()->p_A1()->v())*t - 1.07259*t2 - 0.001147*t3)*SEC2RAD;

  (Matrix3&)*this = R1(-SBNutation::ephemEps())*R2(Psi)*R3(Omega)*R4(-Chi);

  // make a report:
  if (Log->isEligible(SBLog::DBG, SBLog::PRECESSION))
    {
      //      Log->write(SBLog::DBG, SBLog::PRECESSION, ClassName() + ": Arg(TT): " + T.TT().toString());
      Log->write(SBLog::DBG, SBLog::PRECESSION, ClassName() + ": Arg(TDB) : " + T.TDB().toString());

      Log->write(SBLog::DBG, SBLog::PRECESSION, ClassName() + ": Chi      : " + 
		 QString().sprintf("%16.8f (arcsec)", Chi/SEC2RAD));
      Log->write(SBLog::DBG, SBLog::PRECESSION, ClassName() + ": Omega    : " + 
		 QString().sprintf("%16.8f (arcsec)", Omega/SEC2RAD));
      Log->write(SBLog::DBG, SBLog::PRECESSION, ClassName() + ": Psi      : " + 
		 QString().sprintf("%16.8f (arcsec)", Psi/SEC2RAD));


      Log->write(SBLog::DBG, SBLog::NUTATION, ClassName() + ": R1.angle : " + 
		 QString().sprintf("%.16f (arcsec)", R1.angle()/SEC2RAD));
      Log->write(SBLog::DBG, SBLog::NUTATION, ClassName() + ": R1.angle : " + 
		 QString().sprintf("%.20f (rad)", R1.angle()));
      R1.report2Log(SBLog::DBG, SBLog::NUTATION, ClassName() + ": R1(EpsE) : ");

      Log->write(SBLog::DBG, SBLog::NUTATION, ClassName() + ": R2.angle : " + 
		 QString().sprintf("%.16f (arcsec)", R2.angle()/SEC2RAD));
      Log->write(SBLog::DBG, SBLog::NUTATION, ClassName() + ": R2.angle : " + 
		 QString().sprintf("%.20f (rad)", R2.angle()));
      R2.report2Log(SBLog::DBG, SBLog::NUTATION, ClassName() + ": R2(Psi)  : ");

      Log->write(SBLog::DBG, SBLog::NUTATION, ClassName() + ": R3.angle : " + 
		 QString().sprintf("%.16f (arcsec)", R3.angle()/SEC2RAD));
      Log->write(SBLog::DBG, SBLog::NUTATION, ClassName() + ": R3.angle : " + 
		 QString().sprintf("%.20f (rad)", R3.angle()));
      R3.report2Log(SBLog::DBG, SBLog::NUTATION, ClassName() + ": R3(Omega): ");

      Log->write(SBLog::DBG, SBLog::NUTATION, ClassName() + ": R4.angle : " + 
		 QString().sprintf("%.16f (arcsec)", R4.angle()/SEC2RAD));
      Log->write(SBLog::DBG, SBLog::NUTATION, ClassName() + ": R4.angle : " + 
		 QString().sprintf("%.20f (rad)", R4.angle()));
      R4.report2Log(SBLog::DBG, SBLog::NUTATION, ClassName() + ": R4(-Chi) : ");


      this->report2Log(SBLog::DBG, SBLog::PRECESSION, ClassName() + ": matrix   : ");
    };
  return *this;
};

const Matrix3& SBPrec_IAU2000::dP_dpsi()
{
  RotDerMatrix	R_(Z_AXIS);
  DP_dPsi = t*R1*R_(Psi)*R3*R4;
  return DP_dPsi;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBNutation implementation								*/
/*												*/
/*==============================================================================================*/
/**A constructor.
 * Creates a copy of object.
 */
SBNutation::SBNutation(const SBConfig *Cfg_, SBEOP* EOP_) 
  : Matrix3(), R1(X_AXIS), R2(Z_AXIS), R3(X_AXIS),
    DR2dpsi(Z_AXIS),
    DR3deps(X_AXIS)
{
  Cfg	   = Cfg_;
  EOP	   = EOP_;
  Eps	   = 0.0;
  DeltaEps = 0.0;
  DeltaPsi = 0.0;
  AlphaE   = 0.0;
  DPsi = DEps = 0.0;
  GMST = GST = R = 0.0;
  Theory = NULL;
};

void SBNutation::calcGST(const SBTime& T)
{
  //--
  double	GMST0h=0.0, T_u=0.0, T_u2=0.0;

  /*
    IERS Technical Note 21,
    IERS Conventions (1996)
    page 21 says:

    "Apparent Geenwich Sidereal Time GST at the date t of observations, must be derived
    from the following expressions:

    (i) the relationship between Greenwich Mean Sidereal Time (GMST) and Universal 
    Time as given by Aoki et al. (1982):
    
    with T'_u = d'_u/36525, d'_u being the number of days elapsed since 
    2000 January 1, 12h UT1, takin on values +/- 0.5, +/- 1.5, ...,"
  */
  T_u = (T.UT1().date() - TEphem)/36525.0;
  T_u2 = T_u*T_u;
  GMST0h = (24110.54841 + 8640184.812866*T_u + 0.093104*T_u2 - 6.2e-6*T_u*T_u2)/DAY2SEC; //days
  
  /* 
     "where r is the ratio of universal to sidereal time as given by Aoki et al. (1992),"
  */
  R = 1.002737909350795 + 5.9006e-11*T_u - 5.9e-15*T_u2;
  
  /*
    "(ii) the interval of GMST from 0h UT1 to the hour of the observation in UT1,"
  */
  GMST = GMST0h + R*(T.UT1().time());	// days
  GMST = fmod(GMST, 1.0);		// days
  if (GMST<0.0)
    GMST+= 1.0;

  GMST*=2.0*M_PI;			// rad

  /*
    "(iii) accumulated precession and nutation in right ascension (Aoki and Kinoshita, 1983),"
  */
  GST = GMST + eqEquinox();		// rad
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBNut_IAU1980 implementation								*/
/*												*/
/*==============================================================================================*/
/**A constructor.
 * Creates a copy of object.
 */
SBNut_IAU1980::SBNut_IAU1980(const SBConfig *Cfg_, SBEOP* EOP_) 
  : SBNutation(Cfg_, EOP_)
{
  switch (Cfg->nutModel())
    {
    default:
    case SBConfig::NM_IAU1980: Theory = new SBNutThIAU1980; break;
    case SBConfig::NM_IAU1996: Theory = new SBNutThIAU1996; break;
    };
};

const Matrix3& SBNut_IAU1980::operator()(const SBTime& T)
{
  double t = (T.TT() - TEphem)/36525.0;
  //  double t = (T.TDB() - TEphem)/36525.0;
  double t2 = t*t, t3 = t2*t;

  DPsi = DEps = 0.0;
  Eps = EphemEps + SEC2RAD*(-46.8150*t - 0.00059*t2 + 0.001813*t3);

  Theory->calc(T);

  DeltaEps = Theory->deltaEps();
  DeltaPsi = Theory->deltaPsi();
  if (Cfg->isNeedGeodNut())
    DeltaPsi += geodesicNutation(T);
  
  if (EOP) 
    {
      DeltaPsi += (DPsi = EOP->dPsi(T));
      DeltaEps += (DEps = EOP->dEps(T));
    };
  
  (Matrix3&)*this = R1(-Eps)*R2(DeltaPsi)*R3(Eps + DeltaEps);

  DR2dpsi(DeltaPsi);
  DR3deps(Eps + DeltaEps);
  

  if (Cfg->nutEquinox()==SBConfig::NEE_SOVERS)
    /*
      O.J.Sovers, C.S.Jacobs
      "Observation Model and Parameter Partials for the JPL VLBI 
      Parameter Estimation Software "MODEST"-1996"
      page 32 says:
      "   By the very definition of "mean of date" and "true of date", nutation causes a difference
      in the hour angles of the mean equinox of date and the true equinox of date. This difference,
      called the "equation of equinoxes", is denoted by {\alpha_E} and is obtained as follows:"
    */
    AlphaE = atan2(mat[0][1], mat[0][0]);
  else
    if (Cfg->nutEquinox()==SBConfig::NEE_IERS92)
      /*
      */
      AlphaE = DeltaPsi*cos(Eps);
    else
      /*
	IERS Technical Note 21,
	IERS Conventions (1996)
	page 21 says:
	" (iii) accumulated precession and nutation in right ascension (Aoki and Kinoshita, 1983),
	GST = GMST + \Delta\psi*cos(\epsilon_E) + 0".0026*sin(\Omega) + 0".000063*sin(2*\Omega),
	where \Omega is the mean longitude of the ascending node of the lunar orbit. The last two
	terms have not been included in the IERS Standards previosly. They should not be included
	in (iii) until 1 January 1997 when their use will begin."
      */
      AlphaE = DeltaPsi*cos(Eps) + SEC2RAD*(0.00264*sin(T.fArg_Omega()) + 0.000063*sin(2.0*T.fArg_Omega()));

  // calc GMST and GST:
  calcGST(T);

  // make a report:
  if (Log->isEligible(SBLog::DBG, SBLog::NUTATION))
    {
      //Log->write(SBLog::DBG, SBLog::NUTATION, ClassName() + ": Arg(TT)       : " + T.TT().toString());
      Log->write(SBLog::DBG, SBLog::NUTATION, ClassName() + ": Arg(TDB)       : " + T.TDB().toString());
      
      Log->write(SBLog::DBG, SBLog::NUTATION, ClassName() + ": Geodetic Nut. : " + 
		 QString().sprintf("%.16f (arcsec)", geodesicNutation(T)/SEC2RAD));

      Log->write(SBLog::DBG, SBLog::NUTATION, ClassName() + ": Delta Psi     : " + 
		 QString().sprintf("%20.16f (arcsec)", DeltaPsi/SEC2RAD));
      Log->write(SBLog::DBG, SBLog::NUTATION, ClassName() + ": Delta Eps     : " + 
		 QString().sprintf("%20.16f (arcsec)", DeltaEps/SEC2RAD));

      Log->write(SBLog::DBG, SBLog::NUTATION, ClassName() + ": Mean obliquity: " + rad2dmslStr(Eps));
      Log->write(SBLog::DBG, SBLog::NUTATION, ClassName() + ": True obliquity: " + 
		 rad2dmslStr(Eps+DeltaEps));

      Log->write(SBLog::DBG, SBLog::NUTATION, ClassName() + ": AlphaE        : " + 
		 QString().sprintf("%20.16f (arcsec)", AlphaE/SEC2RAD));
      Log->write(SBLog::DBG, SBLog::NUTATION, ClassName() + ": AlphaE        : " + 
		 QString().sprintf("%24.20f (rad)", AlphaE));


      Log->write(SBLog::DBG, SBLog::NUTATION, ClassName() + ": R1.angle      : " + 
		 QString().sprintf("%.16f (arcsec)", R1.angle()/SEC2RAD));
      Log->write(SBLog::DBG, SBLog::NUTATION, ClassName() + ": R1.angle      : " + 
		 QString().sprintf("%.20f (rad)", R1.angle()));
      R1.report2Log(SBLog::DBG, SBLog::NUTATION, ClassName() + ": R1(-eps_)     : ");
      Log->write(SBLog::DBG, SBLog::NUTATION, ClassName() + ": R2.angle      : " + 
		 QString().sprintf("%.16f (arcsec)", R2.angle()/SEC2RAD));
      Log->write(SBLog::DBG, SBLog::NUTATION, ClassName() + ": R2.angle      : " + 
		 QString().sprintf("%.20f (rad)", R2.angle()));
      R2.report2Log(SBLog::DBG, SBLog::NUTATION, ClassName() + ": R2(delta psi) : ");
      Log->write(SBLog::DBG, SBLog::NUTATION, ClassName() + ": R3.angle      : " + 
		 QString().sprintf("%.16f (arcsec)", R3.angle()/SEC2RAD));
      Log->write(SBLog::DBG, SBLog::NUTATION, ClassName() + ": R3.angle      : " + 
		 QString().sprintf("%.20f (rad)", R3.angle()));
      R3.report2Log(SBLog::DBG, SBLog::NUTATION, ClassName() + ": R3(eps+deps)  : ");

      this->report2Log(SBLog::DBG, SBLog::NUTATION, ClassName() + ": matrix        : ");
    };


  return *this;
};

double SBNut_IAU1980::geodesicNutation(const SBTime& T)
{
  return SEC2RAD*(-0.000153*sin(T.fArg_lPrime()) - 0.000002*sin(2.0*T.fArg_lPrime()));
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBNut_IAU2000 implementation								*/
/*												*/
/*==============================================================================================*/
/**A constructor.
 * Creates a copy of object.
 */
SBNut_IAU2000::SBNut_IAU2000(const SBConfig *Cfg_, SBEOP* EOP_) 
  : SBNutation(Cfg_, EOP_)
{
  Theory = new SBNutThIAU2000A;
};

void SBNut_IAU2000::calcGST(const SBTime& T)
{
  //  if (Cfg->nutModel()==SBConfig::NM_IAU2000)
  //    {
  //      double t = (Time.TDB() - TEphem)/36525.0;
  //      double t2=t*t;
  //      GMST+= // microarcsec v
  //	SEC2RAD*1.0e-6*(14506.0 - 274950.12*t + 117.21*t2 - 0.44*t*t2 + 18.82*t2*t2);
  //    };

  //  double	t = (T.TDB() - TEphem)/36525.0;
  double	t = (T.TT() - TEphem)/36525.0;
  double	t2 = t*t;
  double	ERA=0.0;
  double	PolinomialPart=0.0;
  double	NonPolinomialPart=0.0;

  R	= 1.00273781191135448;
  ERA	= 2.0*M_PI*(0.7790572732640 + fmod(R*(T.UT1() - TEphem), 1.0)); //rad
  
  PolinomialPart = 0.014506 + 4612.15739966*t + 1.39667721*t2 - 0.00009344*t*t2 + 0.00001882*t2*t2;
  GMST = ERA + PolinomialPart*SEC2RAD;
  
  double	A_01 = T.fArg_l();
  double	A_02 = T.fArg_lPrime();
  double	A_03 = T.fArg_F();
  double	A_04 = T.fArg_D();
  double	A_05 = T.fArg_Omega();
  double	A_07 = fmod(3.176146697 + 1021.3285546211 *t,		    2.0*M_PI);
  double	A_08 = fmod(1.753470314 +  628.3075849991 *t,		    2.0*M_PI);
  double	A_14 = fmod(                 0.024381750  *t + 0.00000538691*t*t,2.0*M_PI);


  NonPolinomialPart = 
    2640.96*sin(                    A_05)	- 0.39*cos(                    A_05)
    + 63.52*sin(		  2*A_05)	- 0.02*cos(		     2*A_05)
    + 11.75*sin(2*A_03 - 2*A_04 + 3*A_05)	+ 0.01*cos(2*A_03 - 2*A_04 + 3*A_05)
    + 11.21*sin(2*A_03 - 2*A_04 +   A_05)       + 0.01*cos(2*A_03 - 2*A_04 +   A_05)
    -  4.55*sin(2*A_03 - 2*A_04 + 2*A_05)
    +  2.02*sin(2*A_03          + 3*A_05)
    +  1.98*sin(2*A_03          +   A_05)
    -  1.72*sin(                  3*A_05)
    -  1.41*sin(  A_02          +   A_05)       - 0.01*cos(  A_02          +   A_05)
    -  1.26*sin(  A_02          -   A_05)       - 0.01*cos(  A_02          -   A_05)
    -  0.63*sin(  A_01          -   A_05)
    -  0.63*sin(  A_01          +   A_05)
    +  0.46*sin(  A_02 + 2*A_03 - 2*A_04 + 3*A_05)
    +  0.45*sin(  A_02 + 2*A_03 - 2*A_04 +   A_05)
    +  0.36*sin(4*A_03 - 4*A_04 + 4*A_05)
    -  0.24*sin(  A_03 -   A_04 +   A_05 - 8*A_07 + 12*A_08) - 0.12*cos(A_03 - A_04 + A_05 - 8*A_07 + 12*A_08)
    +  0.32*sin(2*A_03)
    +  0.28*sin(2*A_03          + 2*A_05)
    +  0.27*sin(  A_01 + 2*A_03 + 3*A_05)
    +  0.26*sin(  A_01 + 2*A_03 +   A_05)
    -  0.21*sin(2*A_03 - 2*A_04)
    +  0.19*sin(  A_02 - 2*A_03 + 2*A_04 - 3*A_05)
    +  0.18*sin(  A_02 - 2*A_03 + 2*A_04 -   A_05)
    -  0.10*sin(8*A_07 - 13*A_08 -  A_14)	+ 0.05*cos(8*A_07 - 13*A_08 - A_14)
    +  0.15*sin(2*A_04)
    -  0.14*sin(2*A_01 - 2*A_03 -   A_05)
    +  0.14*sin(  A_01 - 2*A_04 +   A_05)
    -  0.14*sin(  A_02 + 2*A_03 - 2*A_04 + 2*A_05)
    +  0.14*sin(  A_01 - 2*A_04 -   A_05)
    +  0.13*sin(4*A_03 - 2*A_04 + 4*A_05)
    -  0.11*sin(2*A_03 - 2*A_04 + 4*A_05)
    +  0.11*sin(  A_01 - 2*A_03 - 3*A_05)
    +  0.11*sin(  A_01 - 2*A_03 -   A_05)
    -  0.87*sin(  A_05)*t;
  
  GST = GMST + eqEquinox() + NonPolinomialPart*SEC2RAD*1.0e-6;
};

const Matrix3& SBNut_IAU2000::operator()(const SBTime& T)
{
  double t = (T.TT() - TEphem)/36525.0;
  //  double t = (T.TDB() - TEphem)/36525.0;
  DPsi = DEps = 0.0;

  Eps = EphemEps + SEC2RAD*(-46.84024*t - 0.00059*t*t + 0.001813*t*t*t);

  Theory->calc(T);

  DeltaEps = Theory->deltaEps();
  DeltaPsi = Theory->deltaPsi();

  if (EOP) 
    {
      DeltaPsi += (DPsi = EOP->dPsi(T));
      DeltaEps += (DEps = EOP->dEps(T));
    };
  
  (Matrix3&)*this = R1(-Eps)*R2(DeltaPsi)*R3(Eps + DeltaEps);

  DR2dpsi(DeltaPsi);
  DR3deps(Eps + DeltaEps);
  

  if (Cfg->nutEquinox()==SBConfig::NEE_SOVERS)
    /*
      O.J.Sovers, C.S.Jacobs
      "Observation Model and Parameter Partials for the JPL VLBI 
      Parameter Estimation Software "MODEST"-1996"
      page 32 says:
      "   By the very definition of "mean of date" and "true of date", nutation causes a difference
      in the hour angles of the mean equinox of date and the true equinox of date. This difference,
      called the "equation of equinoxes", is denoted by {\alpha_E} and is obtained as follows:"
    */
    AlphaE = atan2(mat[0][1], mat[0][0]);
  else
    /*
      IERS Technical Note 21,
      IERS Conventions (1996)
      page 21 says:
      " (iii) accumulated precession and nutation in right ascension (Aoki and Kinoshita, 1983),
      GST = GMST + \Delta\psi*cos(\epsilon_E) + 0".0026*sin(\Omega) + 0".000063*sin(2*\Omega),
      where \Omega is the mean longitude of the ascending node of the lunar orbit. The last two
      terms have not been included in the IERS Standards previosly. They should not be included
      in (iii) until 1 January 1997 when their use will begin."
    */
    AlphaE = DeltaPsi*cos(Eps)/* + SEC2RAD*(0.00264*sin(T.fArg_Omega()) + 0.000063*sin(2.0*T.fArg_Omega()))*/;

  // calc GMST and GST:
  calcGST(T);
  
  // make a report:
  if (Log->isEligible(SBLog::DBG, SBLog::NUTATION))
    {
      //Log->write(SBLog::DBG, SBLog::NUTATION, ClassName() + ": Arg(TT)       : " + T.TT().toString());
      Log->write(SBLog::DBG, SBLog::NUTATION, ClassName() + ": Arg(TDB)       : " + T.TDB().toString());

      Log->write(SBLog::DBG, SBLog::NUTATION, ClassName() + ": Geodetic Nut. : " + 
		 QString().sprintf("%.16f (arcsec)", geodesicNutation(T)/SEC2RAD));

      Log->write(SBLog::DBG, SBLog::NUTATION, ClassName() + ": Delta Psi     : " + 
		 QString().sprintf("%20.16f (arcsec)", DeltaPsi/SEC2RAD));
      Log->write(SBLog::DBG, SBLog::NUTATION, ClassName() + ": Delta Eps     : " + 
		 QString().sprintf("%20.16f (arcsec)", DeltaEps/SEC2RAD));

      Log->write(SBLog::DBG, SBLog::NUTATION, ClassName() + ": Mean obliquity: " + rad2dmslStr(Eps));
      Log->write(SBLog::DBG, SBLog::NUTATION, ClassName() + ": True obliquity: " + 
		 rad2dmslStr(Eps+DeltaEps));

      Log->write(SBLog::DBG, SBLog::NUTATION, ClassName() + ": AlphaE        : " + 
		 QString().sprintf("%20.16f (arcsec)", AlphaE/SEC2RAD));
      Log->write(SBLog::DBG, SBLog::NUTATION, ClassName() + ": AlphaE        : " + 
		 QString().sprintf("%24.20f (rad)", AlphaE));


      Log->write(SBLog::DBG, SBLog::NUTATION, ClassName() + ": R1.angle      : " + 
		 QString().sprintf("%.16f (arcsec)", R1.angle()/SEC2RAD));
      Log->write(SBLog::DBG, SBLog::NUTATION, ClassName() + ": R1.angle      : " + 
		 QString().sprintf("%.20f (rad)", R1.angle()));
      R1.report2Log(SBLog::DBG, SBLog::NUTATION, ClassName() + ": R1(-eps_)     : ");
      Log->write(SBLog::DBG, SBLog::NUTATION, ClassName() + ": R2.angle      : " + 
		 QString().sprintf("%.16f (arcsec)", R2.angle()/SEC2RAD));
      Log->write(SBLog::DBG, SBLog::NUTATION, ClassName() + ": R2.angle      : " + 
		 QString().sprintf("%.20f (rad)", R2.angle()));
      R2.report2Log(SBLog::DBG, SBLog::NUTATION, ClassName() + ": R2(delta psi) : ");
      Log->write(SBLog::DBG, SBLog::NUTATION, ClassName() + ": R3.angle      : " + 
		 QString().sprintf("%.16f (arcsec)", R3.angle()/SEC2RAD));
      Log->write(SBLog::DBG, SBLog::NUTATION, ClassName() + ": R3.angle      : " + 
		 QString().sprintf("%.20f (rad)", R3.angle()));
      R3.report2Log(SBLog::DBG, SBLog::NUTATION, ClassName() + ": R3(eps+deps)  : ");

      this->report2Log(SBLog::DBG, SBLog::NUTATION, ClassName() + ": matrix        : ");
    };


  return *this;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBPrecNutNRO implementation								*/
/*												*/
/*==============================================================================================*/
/**A constructor.
 * Creates a copy of object.
 */
SBPrecNutNRO::SBPrecNutNRO(const SBConfig *Cfg_, SBEOP* EOP_) 
  : Matrix3(), R(Z_AXIS), Q()
{
  Cfg	   = Cfg_;
  EOP	   = EOP_;
  X=Y=S=0.0;
  DX = DY = 0.0;
  if (!EOP)
    Log->write(SBLog::WRN, SBLog::NUTATION, ClassName() + ": Warning, EOP database is NULL");
  /*
  switch (Cfg->nutModel())
    {
    default:
    case SBConfig::NM_IAU1980: Theory = new SBNROThIAU1980; break;
    case SBConfig::NM_IAU1996: Theory = new SBNROThIAU1996; break;
    case SBConfig::NM_IAU2000: Theory = new SBNROThIAU2000A; break;
    };
  */
  Theory = new SBNROThIAU2000A;
};

const Matrix3& SBPrecNutNRO::operator()(const SBTime& T)
{
  Theory->calc(T);
  X=Theory->x();
  Y=Theory->y();
  S=Theory->s();
  DX = DY = 0.0;

  if (EOP) 
    {
      X += (DX = EOP->dPsi(T));
      Y += (DY = EOP->dEps(T));
      S += -0.5*(DX * DY);
    };

  double a = 0.5 + 0.125*(X*X + Y*Y);
  Q = Matrix3(1.0 - a*X*X,	-a*X*Y,		X,
	      -a*X*Y,		1.0 - a*Y*Y,	Y,
	      -X,		-Y,		1.0 - a*(X*X + Y*Y));
  return ((Matrix3&)*this = Q*R(S));
};

double SBPrecNutNRO::geodesicNutation(const SBTime& T)
{
  return (-0.0000609*sin(T.fArg_lPrime()) - 0.0000008*sin(2.0*T.fArg_lPrime()))*
    SBNutation::ephemEps()*SEC2RAD;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBPolus implementation									*/
/*												*/
/*==============================================================================================*/
/**A constructor.
 * Creates a copy of object.
 */
SBPolus::SBPolus(SBEOP* EOP_) 
  : Matrix3(), R1(X_AXIS), R2(Y_AXIS),
    DR1dy(X_AXIS),
    DR2dx(Y_AXIS)
{
  Xp=Yp=0.0; 
  if (!(EOP=EOP_))
    Log->write(SBLog::WRN, SBLog::NUTATION, ClassName() + ": Warning, EOP database is NULL");
};

const Matrix3& SBPolus::operator()(const SBTime& T, SBFrame* Frame)
{
  Xp = Yp = 0.0;

  if (EOP)
    {
      Xp = EOP->dX(T);
      Yp = EOP->dY(T);
    };

  double	Xp_total=Xp, Yp_total=Yp;

  if (Frame)
    {
      Xp_total += Frame->diurnalDX();
      Yp_total += Frame->diurnalDY();
    };
    

  (Matrix3&)*this = R1(Yp_total)*R2(Xp_total);
  DR1dy(Yp_total);
  DR2dx(Xp_total);
  
  // make a report:
  if (Log->isEligible(SBLog::DBG, SBLog::PMOTION))
    {
      Log->write(SBLog::DBG, SBLog::PMOTION, ClassName() + ": Arg(UTC) : " + T.UTC().toString());

      Log->write(SBLog::DBG, SBLog::PMOTION, ClassName() + ": Polus X  : " + 
		 QString().sprintf("%16.6f (arcsec)", Xp_total/SEC2RAD));
      Log->write(SBLog::DBG, SBLog::PMOTION, ClassName() + ": Polus Y  : " + 
		 QString().sprintf("%16.6f (arcsec)", Yp_total/SEC2RAD));
      Log->write(SBLog::DBG, SBLog::PMOTION, ClassName() + ": Diurnal X: " + 
		 QString().sprintf("%16.6f (arcsec)", Frame->diurnalDX()/SEC2RAD));
      Log->write(SBLog::DBG, SBLog::PMOTION, ClassName() + ": Diurnal Y: " + 
		 QString().sprintf("%16.6f (arcsec)", Frame->diurnalDX()/SEC2RAD));

      Log->write(SBLog::DBG, SBLog::NUTATION, ClassName() + ": R1.angle : " + 
		 QString().sprintf("%.16f (arcsec)", R1.angle()/SEC2RAD));
      Log->write(SBLog::DBG, SBLog::NUTATION, ClassName() + ": R1.angle : " + 
		 QString().sprintf("%.20f (rad)", R1.angle()));
      R1.report2Log(SBLog::DBG, SBLog::NUTATION, ClassName() + ": R1(y)    : ");
      Log->write(SBLog::DBG, SBLog::NUTATION, ClassName() + ": R2.angle : " + 
		 QString().sprintf("%.16f (arcsec)", R2.angle()/SEC2RAD));
      Log->write(SBLog::DBG, SBLog::NUTATION, ClassName() + ": R2.angle : " + 
		 QString().sprintf("%.20f (rad)", R2.angle()));
      R2.report2Log(SBLog::DBG, SBLog::NUTATION, ClassName() + ": R2(x)    : ");

      report2Log(SBLog::DBG, SBLog::PMOTION, ClassName() + ": matrix   : ");
    };

  return *this;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBPolusNRO implementation								*/
/*												*/
/*==============================================================================================*/
/**A constructor.
 * Creates a copy of object.
 */
SBPolusNRO::SBPolusNRO(SBEOP* EOP_) 
  : Matrix3(), R1(X_AXIS), R2(Y_AXIS), R3(Z_AXIS),
    DR1dy(X_AXIS),
    DR2dx(Y_AXIS)
{
  Xp=Yp=s_prime=0.0;
  if (!(EOP=EOP_))
    Log->write(SBLog::WRN, SBLog::NUTATION, ClassName() + ": Warning, EOP database is NULL");
};

const Matrix3& SBPolusNRO::operator()(const SBTime& T, SBFrame* Frame)
{
  Xp = Yp = s_prime = 0.0;

  if (EOP)
    {
      Xp = EOP->dX(T);
      Yp = EOP->dY(T);
    };

  double	Xp_total=Xp, Yp_total=Yp;
  
  if (Frame)
    {
      Xp_total += Frame->diurnalDX();
      Yp_total += Frame->diurnalDY();
    };
  
  s_prime = -47.0e-6*(T.TT() - TEphem)/36525.0*SEC2RAD;

  (Matrix3&)*this = R3(-s_prime)*R1(Yp_total)*R2(Xp_total);

  DR1dy(Yp_total);
  DR2dx(Xp_total);

  return *this;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBFrame implementation									*/
/*												*/
/*==============================================================================================*/
SBFrame::SBFrame(const SBConfig *Cfg_, SBRunManager *Mgr_, const QString& Name_) 
  : Matrix3(), SBNamed(Name_), Time(), CRS2TRS(m3E),
    DQ_dt(m3E), 
    DQ_dUT(m3E),
    DQ_dxp(m3E),
    DQ_dyp(m3E),
    DQ_dpsi(m3E),
    DQ_deps(m3E),

    DQ_dprec(m3E)
{
  Cfg = Cfg_;
  Mgr = Mgr_;
  EOP = SetUp->loadEOP();
  EOP -> setPtNum(Cfg->eopNumIntrplPoints());
  EOP -> setMgr(Mgr);
  Time.setEOP(EOP);

  switch (Cfg->eopDiurnal())
    {
    default:
    case SBConfig::EDM_NONE:		EOPdiurnal = new SBOTideEOPGeneral; break; // shunt
      //    case SBConfig::EDM_Ray1995:		EOPdiurnal = new SBOTideEOPRay1994_1996; break; // recommended
    case SBConfig::EDM_Ray1995:		EOPdiurnal = new SBOTideEOPRay1994_2003; break; // recommended
    case SBConfig::EDM_JPL92:		EOPdiurnal = new SBOTideEOP_JPL92;  break;
    case SBConfig::EDM_HD1994:		EOPdiurnal = new SBOTideEOP_HD1994; break;
    case SBConfig::EDM_GSFC95:		EOPdiurnal = new SBOTideEOP_GSFC95; break;
    case SBConfig::EDM_Gipson1995:	EOPdiurnal = new SBOTideEOP_Gipson1995; break;
    };

  switch (Cfg->eopDiurnalAtm())
    {
    default:
    case SBConfig::EDMA_NONE:	EOPdiurnalAtm = new SBOTideEOPGeneral; break; // shunt
    case SBConfig::EDMA_BBP:	EOPdiurnalAtm = new SBATideEOP_BBP_02; break;
    };
  
  switch (Cfg->eopDiurnalNonTidal())
    {
    default:
    case SBConfig::EDMNT_NONE:	EOPdiurnalNTd = new SBOTideEOPGeneral; break; // shunt
    case SBConfig::EDMNT_Viron:	EOPdiurnalNTd = new SBNonTideEOP_Viron_02; break;
    };
  
  if (Cfg->nutModel() == SBConfig::NM_IAU2000)
    EOPdiurnalGrv = new SBGravTideEOP_B_02;
  else
    EOPdiurnalGrv = new SBOTideEOPGeneral;

  UT1_UTC = 0.0;
};

SBFrame::~SBFrame()
{
  if (EOPdiurnal) delete EOPdiurnal; 

  if (EOPdiurnalAtm) delete EOPdiurnalAtm; 
  if (EOPdiurnalNTd) delete EOPdiurnalNTd; 
  if (EOPdiurnalGrv) delete EOPdiurnalGrv; 

  if (EOP) delete EOP;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBFrameClassic implementation								*/
/*												*/
/*==============================================================================================*/
SBFrameClassic::SBFrameClassic(const SBConfig *Cfg_, SBRunManager *Mgr_, const QString& Name_)
  : SBFrame(Cfg_, Mgr_, Name_), R(), W(EOP), R1(Z_AXIS), R2(Y_AXIS), R3(X_AXIS), Bias(m3E)
{
  DAleph0	= 0.0;
  DPsi0		= 0.0;
  DEpsilon0	= 0.0;
  switch (Cfg->nutModel())
    {
    default:
    case SBConfig::NM_IAU1980:
      P = new SBPrec_IAU1976;
      N = new SBNut_IAU1980(Cfg_, EOP);
      break;
    case SBConfig::NM_IAU2000:
      DAleph0	= -0.01460*SEC2RAD;
      DPsi0	= -0.0417750*SEC2RAD;
      DEpsilon0	= -0.0068192*SEC2RAD;
      Bias = R1(-DAleph0)*R2(-DPsi0*SBNutation::ephemEps())*R3(DEpsilon0);
      P = new SBPrec_IAU2000;
      N = new SBNut_IAU2000(Cfg_, EOP);
      break;
    };
};

SBFrameClassic::~SBFrameClassic()
{
  if (P)
    {
      delete P;
      P = NULL;
    };
  if (N)
    {
      delete N;
      N = NULL;
    };
};

void SBFrameClassic::calc(const SBMJD& UTC)
{
  EOP->interpolateEOPs(UTC);
  Time.setUTC(UTC);
  EOPdiurnal->calc(Time);

  EOPdiurnalAtm->calc(Time);
  EOPdiurnalNTd->calc(Time);
  EOPdiurnalGrv->calc(Time);

  (*N)(Time);

  double H = -(N->gst() + diurnalDUT1()*2.0*M_PI);	// rad

  DQ_dUT = Bias*(*P)(Time)**N*R.dRdt()(H)*W(Time, this)*(-2.0*M_PI*N->r());
  DQ_dt  = DQ_dUT/86400.0;

  (Matrix3&)*this = Bias**P**N*R(H)*W;
  CRS2TRS = ~*this;


  UT1_UTC = EOP->dUT1_UTC(UTC);

  // make a report:
  if (Log->isEligible(SBLog::DBG, SBLog::R3))
    {
      Log->write(SBLog::DBG, SBLog::R3, ClassName() + ": Arg(UT1): " + Time.UT1().toString());
      
      //      Log->write(SBLog::DBG, SBLog::R3, ClassName() + ": GMST@0h : " + 
      //		 QString().sprintf("%.16f (days)", GMST0h));
      //      Log->write(SBLog::DBG, SBLog::R3, ClassName() + ": GMST@0h : " + rad2hmsStr(GMST0h));

      Log->write(SBLog::DBG, SBLog::R3, ClassName() + ": GMST    : " + rad2dmslStr(gmst()));
      Log->write(SBLog::DBG, SBLog::R3, ClassName() + ": GMST    : " + rad2hmsStr(gmst()));


      Log->write(SBLog::DBG, SBLog::R3, ClassName() + ": GST     : " + rad2dmslStr(gst()));
      Log->write(SBLog::DBG, SBLog::R3, ClassName() + ": GST     : " + rad2hmsStr(gst()));

      //      Log->write(SBLog::DBG, SBLog::R3, ClassName() + ": GMST0h  : " + 
      //		 QString().sprintf("%.20f (rad)", GMST0h));
      //      Log->write(SBLog::DBG, SBLog::R3, ClassName() + ": GMST    : " + 
      //		 QString().sprintf("%.20f (rad)", GMST));
      //      Log->write(SBLog::DBG, SBLog::R3, ClassName() + ": GST     : " + 
      //		 QString().sprintf("%.20f (rad)", GST));


      Log->write(SBLog::DBG, SBLog::R3, ClassName() + ": DeltaMu : " + 
		 QString().sprintf("%.16f (arcsec)", N->eqEquinox()/SEC2RAD));
      Log->write(SBLog::DBG, SBLog::R3, ClassName() + ": DeltaMu : " + 
		 QString().sprintf("%.20f (rad)", N->eqEquinox()));
      Log->write(SBLog::DBG, SBLog::R3, ClassName() + ":-H       : " + 
		 QString().sprintf("%22.18f (rad)", H));
      Log->write(SBLog::DBG, SBLog::R3, ClassName() + ": r       : " + 
		 QString().sprintf("%.16f", N->r()));
      

      Log->write(SBLog::DBG, SBLog::R3, ClassName() + ": R.angle : " + 
		 QString().sprintf("%.16f (arcsec)", R.angle()/SEC2RAD));
      Log->write(SBLog::DBG, SBLog::R3, ClassName() + ": R.angle : " + 
		 QString().sprintf("%.20f (rad)", R.angle()));
      R.report2Log(SBLog::DBG, SBLog::R3, "Rotation Matrix(-GST) : ");

      report2Log(SBLog::DBG, SBLog::R3, ClassName() + ": matrix  : ");
    };

};

const Matrix3& SBFrameClassic::dQ_dxp()
{
  DQ_dxp = Bias**P**N*R*W.r1y()*W.dR2dx();
  return DQ_dxp;
};

const Matrix3& SBFrameClassic::dQ_dyp()
{
  DQ_dyp = Bias**P**N*R*W.dR1dy()*W.r2x();
  return DQ_dyp;
};

const Matrix3& SBFrameClassic::dQ_dpsi()
{
  DQ_dpsi = Bias**P*(N->r1()*N->dR2dpsi()*N->r3()*R - cos(N->meanEps())*(*N*R.dRdt()))*W;
  return DQ_dpsi;
};

const Matrix3& SBFrameClassic::dQ_deps()
{
  DQ_deps = Bias**P*N->r1()*N->r2()*N->dR3deps()*R*W;
  return DQ_deps;
};


const Matrix3& SBFrameClassic::dQ_dprec()
{
  DQ_dprec = Bias*P->dP_dpsi()**N*R*W;
  return DQ_dprec;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBFrameNRO implementation								*/
/*												*/
/*==============================================================================================*/
void SBFrameNRO::calc(const SBMJD& UTC)
{
  EOP->interpolateEOPs(UTC);
  Time.setUTC(UTC);

  EOPdiurnal->calc(Time);

  EOPdiurnalAtm->calc(Time);
  EOPdiurnalNTd->calc(Time);
  EOPdiurnalGrv->calc(Time);


  //  double t = (Time.TT() - TEphem)/36525.0;
  PN(Time);
  ERA = 2.0*M_PI*(0.7790572732640 + fmod(1.00273781191135448*(Time.UT1() - TEphem), 1.0));
  ERA += diurnalDUT1()*2.0*M_PI;

  UT1_UTC = EOP->dUT1_UTC(UTC);

  //  GMST = ERA + SEC2RAD*(0.014506 + 4612.15739966*t + 1.39667721*t*t - 
  //			0.00009344*t*t*t + 0.00001882*t*t*t*t) + PN.theory()->dT();
  //  GST = GMST;


  DQ_dUT = PN*R.dRdt()(-ERA)*W(Time, this)*(-2.0*M_PI*1.00273781191135448);
  DQ_dt  = DQ_dUT/86400.0;

  //  (Matrix3&)*this = PN*R(-ERA)*W(Time, this);
  (Matrix3&)*this = PN*R(-ERA)*W;
  CRS2TRS = ~*this;
};

const Matrix3& SBFrameNRO::dQ_dxp()
{
  DQ_dxp = PN*R*W.r3s()*W.r1y()*W.dR2dx();
  return DQ_dxp;
};

const Matrix3& SBFrameNRO::dQ_dyp()
{
  DQ_dyp = PN*R*W.r3s()*W.dR1dy()*W.r2x();
  return DQ_dyp;
};

// here dQ/dX:
const Matrix3& SBFrameNRO::dQ_dpsi()
{
  double X = PN.x();
  double Y = PN.y();
  double a = 0.5 + 0.125*(X*X + Y*Y);
  RotDerMatrix	dRSdX(Z_AXIS);
  Matrix3	dQdX( -0.25*X*X*X -2.0*a*X,	-0.25*X*X*Y - a*Y,	1.0,
		      -0.25*X*X*Y - a*Y,	-0.25*X*Y*Y,		0.0,
		      -1.0,			 0.0,			-0.25*X*(X*X + Y*Y) - 2.0*a*X);
  //  Matrix3	dQdX( -(1.0+0.25*Y*Y)*X - 0.5*X*X*X,   -0.5*Y - 3.0/8.0*X*X*Y - 0.125*Y*Y*Y,   1.0,
  //		      -0.5*Y - 3.0/8.0*X*X*Y - 0.125*Y*                         -0.25*X*Y*Y,   0.0,
  //		      -1.0,      0.0,                          -X - 0.5*X*X*X - 0.5*X*Y*Y);
  DQ_dpsi = (dQdX*PN.rs() - 0.5*Y*(PN.q()*dRSdX(PN.s())))*R*W;
  //DQ_dpsi = dQdX*PN.rs()*R*W;
  return DQ_dpsi;
};

// here dQ/dY:
//const Matrix3& SBFrameNRO::dQ_deps()
//{
//  double X = PN.x();
//  double Y = PN.y();
//  //  double a = 0.5 + 0.125*(X*X + Y*Y);
//  //  DQ_deps = Matrix3(0.0,           -a*X,    0.0,
//  //		    -a*X,      -2.0*a*Y,    1.0,
//  //		    0.0,           -1.0,   -2.0*a*Y)*PN.rs()*R*W;
//  DQ_deps = Matrix3(-0.25*X*X*Y,                    -0.5*X - 0.125*X*X*X - 3.0/8.0*X*Y*Y,  0.0,
//		    -0.5*X - 0.125*X*X*X - 3.0/8.0*X*Y*Y,  -(1.0+0.25*X*X)*Y - 0.5*Y*Y*Y,  1.0,
//		    0.0,                  -1.0,        -Y - 0.5*X*X*Y - 0.5*Y*Y*Y)*PN.rs()*R*W;
//  return DQ_deps;
//};
const Matrix3& SBFrameNRO::dQ_deps()
{
  double X = PN.x();
  double Y = PN.y();
  double a = 0.5 + 0.125*(X*X + Y*Y);
  RotDerMatrix	dRSdY(Z_AXIS);
  Matrix3	dQdY( -0.25*X*X*Y,		-0.25*X*Y*Y - a*X,	0.0,
		      -0.25*X*Y*Y - a*X,	-0.25*Y*Y*Y - 2.0*a*Y,	1.0,
		       0.0,			-1.0,			-0.25*Y*(X*X + Y*Y) - 2.0*a*Y);
  DQ_deps = (dQdY*PN.rs() - 0.5*X*(PN.q()*dRSdY(PN.s())))*R*W;
  //DQ_deps = dQdY*PN.rs()*R*W;
  return DQ_deps;
};
/*==============================================================================================*/







// real models:
/*==============================================================================================*/
/*												*/
/* class SBSTideUT1 implementation								*/
/*												*/
/*==============================================================================================*/
void SBSTideUT1::calc(const SBTime& T)
{
  double	Xi;
  double	sx, cx;
  int		i;
  DUT1S = 0.0;
  for (i=0; i<62; i++)
    {
      Xi = fmod(Tab[i].N[0]*T.fArg_l() + Tab[i].N[1]*T.fArg_lPrime() +
		Tab[i].N[2]*T.fArg_F() + Tab[i].N[3]*T.fArg_D()      + 
		Tab[i].N[4]*T.fArg_Omega(), 2.0*M_PI);
      sincos(Xi, &sx, &cx);
      DUT1S += Tab[i].B*sx + Tab[i].C*cx;
    };
  DUT1S *= 1.0e-4/86400.0;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBOTideEOPRay1994_1996 implementation							*/
/*												*/
/*==============================================================================================*/
void SBOTideEOPRay1994_1996::calc(const SBTime& T)
{
  double	Xi;
  double	sx, cx;
  int		i;
  DUT1D = DXD = DYD = 0.0;
  //
  // Warning:	the equation for \theta have been changed according to 
  //		D.D.McCarthy "Errata for the IERS Conventions (1996)", item 29.
  double Theta = DEG2RAD*(100.4606184 + 360.98564737*(T.TT() - TEphem));
  for (i=0; i<8; i++)
    {
      Xi = fmod(Tab[i].c[0]*T.fArg_l() + Tab[i].c[1]*T.fArg_lPrime() +
		Tab[i].c[2]*T.fArg_F() + Tab[i].c[3]*T.fArg_D()      + 
		Tab[i].c[4]*T.fArg_Omega() + Tab[i].c[5]*Theta + Tab[i].Phi*DEG2RAD, 2.0*M_PI);
      sincos(Xi, &sx, &cx);
      DUT1D += Tab[i].D*sx + Tab[i].E*cx;
      DXD   += Tab[i].F*sx + Tab[i].G*cx;
      DYD   += Tab[i].H*sx + Tab[i].K*cx;
    };
  DUT1D *= 1.0e-4/86400.0;
  DXD	*= 1.0e-3*SEC2RAD;
  DYD	*= 1.0e-3*SEC2RAD;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBOTideEOPRay1994_2003 implementation							*/
/*												*/
/*==============================================================================================*/
void SBOTideEOPRay1994_2003::calc(const SBTime& T)
{
  double	Arg;
  double	sa, ca;
  int		i;
  DUT1D = DXD = DYD = 0.0;

  for (i=0; i<71; i++)
    {
      Arg = fmod(Tab[i].k[0]*T.fArg_theta() +
		 Tab[i].k[1]*T.fArg_l() + Tab[i].k[2]*T.fArg_lPrime() +
		 Tab[i].k[3]*T.fArg_F() + Tab[i].k[4]*T.fArg_D() + 
		 Tab[i].k[5]*T.fArg_Omega(), 2.0*M_PI);
      sincos(Arg, &sa, &ca);
      DXD   += Tab[i].A*sa + Tab[i].B*ca;
      DYD   += Tab[i].C*sa + Tab[i].D*ca;
      DUT1D += Tab[i].E*sa + Tab[i].F*ca;
    };
  DUT1D *= 1.0e-6/86400.0;
  DXD	*= 1.0e-6*SEC2RAD;
  DYD	*= 1.0e-6*SEC2RAD;

  // For Test Purposes:
  //  DUT1D  = 0.0;
  //  DXD	= 0.0;
  //  DYD	= 0.0;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBOTideEOPGenS implementation								*/
/*												*/
/*==============================================================================================*/
void SBOTideEOPGenS::calc(const SBTime& T)
{
  double	Arg;
  double	sa, ca;
  int		i;
  DUT1D = DXD = DYD = 0.0;

  for (i=0; i<N; i++)
    {
      Arg = fmod(Tab[i].k[0]*T.fArg_l()     + Tab[i].k[1]*T.fArg_lPrime() +
		 Tab[i].k[2]*T.fArg_F()     + Tab[i].k[3]*T.fArg_D()      + 
		 Tab[i].k[4]*T.fArg_Omega() + Tab[i].k[5]*T.fArg_theta(), 2.0*M_PI);
      sincos(Arg, &sa, &ca);
      DXD   += Tab[i].A[0]*ca + Tab[i].B[0]*sa;
      DYD   += Tab[i].A[1]*ca + Tab[i].B[1]*sa;
      DUT1D += Tab[i].A[2]*ca + Tab[i].B[2]*sa;
    };
  DXD	*= 1.0e-6*SEC2RAD;
  DYD	*= 1.0e-6*SEC2RAD;
  DUT1D *= 1.0e-7/86400.0;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBNutThIAU1980 implementation								*/
/*												*/
/*==============================================================================================*/
void SBNutThIAU1980::calc(const SBTime& T)
{
  double	ARGUMENT, t;
  double	sa, ca;
  int		i;
  DeltaEpsT = DeltaPsiT = 0.0;
  t = (T.TT() - TEphem)/36525.0;

  for (i=105; -1<i; i--)
    {
      ARGUMENT = fmod(Tab[i].N[0]*T.fArg_l() + Tab[i].N[1]*T.fArg_lPrime() +
		      Tab[i].N[2]*T.fArg_F() + Tab[i].N[3]*T.fArg_D()      + 
		      Tab[i].N[4]*T.fArg_Omega(), 2.0*M_PI);
      sincos(ARGUMENT, &sa, &ca);
      DeltaPsiT += (Tab[i].A[0] + Tab[i].A[1]*t)*sa;
      DeltaEpsT += (Tab[i].B[0] + Tab[i].B[1]*t)*ca;
    };
  DeltaPsiT *= SEC2RAD*0.0001;
  DeltaEpsT *= SEC2RAD*0.0001;

  // make a report:
  if (Log->isEligible(SBLog::DBG, SBLog::NUTATION))
    {
      Log->write(SBLog::DBG, SBLog::NUTATION, ClassName() + ": Arg(TT)           : " + 
		 T.TT().toString());
      Log->write(SBLog::DBG, SBLog::NUTATION, ClassName() + ": Delta Psi (Theory): " + 
		 QString().sprintf("%20.16f (arcsec)", DeltaPsiT/SEC2RAD));
      Log->write(SBLog::DBG, SBLog::NUTATION, ClassName() + ": Delta Eps (Theory): " + 
		 QString().sprintf("%20.16f (arcsec)", DeltaEpsT/SEC2RAD));
    };
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBNutThIAU1996 implementation								*/
/*												*/
/*==============================================================================================*/
void SBNutThIAU1996::calc(const SBTime& T)
{
  double	ARGUMENT, l_ve, l_e, l_ma, l_ju, l_sa, p_a, t=0.0;
  double	sa, ca;
  int		i;
  t = (T.TT() - TEphem)/36525.0;
  DeltaEpsT = DeltaPsiT = 0.0;
  for (i=0; i<263; i++)
    {
      ARGUMENT = fmod(Tab[i].N[0]*T.fArg_l() + Tab[i].N[1]*T.fArg_lPrime() +
		      Tab[i].N[2]*T.fArg_F() + Tab[i].N[3]*T.fArg_D() + 
		      Tab[i].N[4]*T.fArg_Omega(), 2.0*M_PI);
      sincos(ARGUMENT, &sa, &ca);
      DeltaPsiT += (Tab[i].A[0] + Tab[i].A[1]*t)*sa + Tab[i].C[0]*ca;
      DeltaEpsT += (Tab[i].B[0] + Tab[i].B[1]*t)*ca + Tab[i].C[1]*sa;
    };

  l_ve	= fmod(181.979800853 + 58517.8156748    *t,		    360.0)*DEG2RAD;
  l_e	= fmod(100.466448494 + 35999.3728521    *t,		    360.0)*DEG2RAD;
  l_ma	= fmod(355.433274605 + 19140.299314     *t,		    360.0)*DEG2RAD;
  l_ju	= fmod( 34.351483900 +  3034.90567464   *t,		    360.0)*DEG2RAD;
  l_sa	= fmod( 50.0774713998+  1222.11379404   *t,		    360.0)*DEG2RAD;
  p_a	= fmod(                    1.39697137214*t + 0.0003086*t*t, 360.0)*DEG2RAD;

  for (i=0; i<112; i++)
    {
      ARGUMENT = fmod(PlTab[i].N[0]*l_ve + PlTab[i].N[1]*l_e + PlTab[i].N[2]*l_ma + 
		      PlTab[i].N[3]*l_ju + PlTab[i].N[4]*l_sa + PlTab[i].N[5]*p_a + 
		      PlTab[i].N[6]*T.fArg_D() + PlTab[i].N[7]*T.fArg_F() + 
		      PlTab[i].N[8]*T.fArg_l() + PlTab[i].N[9]*T.fArg_Omega(), 2.0*M_PI);
      
      // Warning:	the formulae for \Delta\psi and \Delta\epsilon have been changed
      //		according to D.D.McCarthy "Errata for the IERS Conventions (1996)", 
      //		item 32.
      sincos(ARGUMENT, &sa, &ca);
      DeltaPsiT += PlTab[i].A[0]*sa + PlTab[i].A[1]*ca;
      DeltaEpsT += PlTab[i].B[0]*ca + PlTab[i].B[1]*sa;
    };

  DeltaPsiT *= SEC2RAD*0.000001;
  DeltaEpsT *= SEC2RAD*0.000001;

  // make a report:
  if (Log->isEligible(SBLog::DBG, SBLog::NUTATION))
    {
      Log->write(SBLog::DBG, SBLog::NUTATION, ClassName() + ": Arg(TT): " + T.TT().toString());
      
      Log->write(SBLog::DBG, SBLog::NUTATION, ClassName() + ": Delta Psi (Theory): " + 
		 QString().sprintf("%20.16f (arcsec)", DeltaPsiT/SEC2RAD));
      Log->write(SBLog::DBG, SBLog::NUTATION, ClassName() + ": Delta Eps (Theory): " + 
		 QString().sprintf("%20.16f (arcsec)", DeltaEpsT/SEC2RAD));
    };
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBNutThIAU2000A implementation								*/
/*												*/
/*==============================================================================================*/
void SBNutThIAU2000A::calc(const SBTime& T)
{
  double	ARGUMENT, l_me, l_ve, l_e, l_ma, l_ju, l_sa, l_ur, l_ne, p_a, t=0.0;
  double	sa, ca;
  int		i;
  t = (T.TT() - TEphem)/36525.0;
  DeltaEpsT = DeltaPsiT = 0.0;
  for (i=0; i<NSolLun; i++)
    {
      ARGUMENT = fmod(SLTab[i].N[0]*T.fArg_l() + SLTab[i].N[1]*T.fArg_lPrime() +
		      SLTab[i].N[2]*T.fArg_F() + SLTab[i].N[3]*T.fArg_D() + 
		      SLTab[i].N[4]*T.fArg_Omega(), 2.0*M_PI);
      sincos(ARGUMENT, &sa, &ca);
      DeltaPsiT += (SLTab[i].Psi_in[0] + SLTab[i].Psi_in[1]*t)*sa + 
	(SLTab[i].Psi_out[0] + SLTab[i].Psi_out[1]*t)*ca;
      DeltaEpsT += (SLTab[i].Eps_in[0] + SLTab[i].Eps_in[1]*t)*ca + 
	(SLTab[i].Eps_out[0] + SLTab[i].Eps_out[1]*t)*sa;
    };

  l_me	= fmod(4.402608842 + 2608.7903141574 *t,		    2.0*M_PI);
  l_ve	= fmod(3.176146697 + 1021.3285546211 *t,		    2.0*M_PI);
  l_e	= fmod(1.753470314 +  628.3075849991 *t,		    2.0*M_PI);
  l_ma	= fmod(6.203480913 +  334.0612426700 *t,		    2.0*M_PI);
  l_ju	= fmod(0.599546497 +   52.9690962641 *t,		    2.0*M_PI);
  l_sa	= fmod(0.874016757 +   21.3299104960 *t,		    2.0*M_PI);
  l_ur	= fmod(5.481293871 +    7.4781598567 *t,		    2.0*M_PI);
  l_ne	= fmod(5.321159000 +    3.8127774000 *t,		    2.0*M_PI);
  p_a	= fmod(                 0.02438175   *t + 0.00000538691*t*t,2.0*M_PI);

  for (i=0; i<NPlanet; i++)
    {
      ARGUMENT = fmod(PlTab[i].N[ 0]*T.fArg_l() + PlTab[i].N[1]*T.fArg_lPrime() + 
		      PlTab[i].N[ 2]*T.fArg_F() + PlTab[i].N[3]*T.fArg_D() + 
		      PlTab[i].N[ 4]*T.fArg_Omega() +
		      PlTab[i].N[ 5]*l_me + PlTab[i].N[ 6]*l_ve + PlTab[i].N[ 7]*l_e + 
		      PlTab[i].N[ 8]*l_ma + PlTab[i].N[ 9]*l_ju + PlTab[i].N[10]*sa + 
		      PlTab[i].N[11]*l_ur + PlTab[i].N[12]*l_ne + PlTab[i].N[13]*p_a, 2.0*M_PI);
      
      // Warning:	the formulae for \Delta\psi and \Delta\epsilon have been changed
      //		according to D.D.McCarthy "Errata for the IERS Conventions (1996)", 
      //		item 32.
      sincos(ARGUMENT, &sa, &ca);
      DeltaPsiT += PlTab[i].A[0]*sa + PlTab[i].A[1]*ca;
      DeltaEpsT += PlTab[i].B[0]*ca + PlTab[i].B[1]*sa;
    };

  DeltaPsiT *= SEC2RAD*0.001;
  DeltaEpsT *= SEC2RAD*0.001;

  // make a report:
  if (Log->isEligible(SBLog::DBG, SBLog::NUTATION))
    {
      Log->write(SBLog::DBG, SBLog::NUTATION, ClassName() + ": Arg(TT):            " + T.TT().toString());
      
      Log->write(SBLog::DBG, SBLog::NUTATION, ClassName() + ": Delta Psi (Theory): " + 
		 QString().sprintf("%20.16f (arcsec)", DeltaPsiT/SEC2RAD));
      Log->write(SBLog::DBG, SBLog::NUTATION, ClassName() + ": Delta Eps (Theory): " + 
		 QString().sprintf("%20.16f (arcsec)", DeltaEpsT/SEC2RAD));
    };
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBNROThIAU2000A implementation								*/
/*												*/
/*==============================================================================================*/
double SBNROThIAU2000A::sum(int N, const GeneralTableEntry* Table, double* Arg)
{
  int		i,j;
  double	sa, ca, ret;
  double	ARGUMENT;
  const GeneralTableEntry	*tab = Table;
  ret = 0.0;
  for (i=0; i<N; i++, tab++)
    {
      ARGUMENT = 0.0;
      for (j=0; j<14; j++) 
	if (tab->N[j]!=0)
	  ARGUMENT += tab->N[j]**(Arg+j);
      ARGUMENT = fmod(ARGUMENT, 2.0*M_PI);
      sincos(ARGUMENT, &sa, &ca);
      ret += tab->S*sa + tab->C*ca;
    };
  return ret;
};

void SBNROThIAU2000A::calc(const SBTime& T)
{
  double	t = (T.TT() - TEphem)/36525.0;
  double	arg[14];

  X = Y = S = DT = 0.0;
  
  arg[ 0] = T.fArg_l();
  arg[ 1] = T.fArg_lPrime();
  arg[ 2] = T.fArg_F();
  arg[ 3] = T.fArg_D();
  arg[ 4] = T.fArg_Omega();
  arg[ 5] = fmod(4.402608842 + 2608.7903141574 *t,		    2.0*M_PI);
  arg[ 6] = fmod(3.176146697 + 1021.3285546211 *t,		    2.0*M_PI);
  arg[ 7] = fmod(1.753470314 +  628.3075849991 *t,		    2.0*M_PI);
  arg[ 8] = fmod(6.203480913 +  334.0612426700 *t,		    2.0*M_PI);
  arg[ 9] = fmod(0.599546497 +   52.9690962641 *t,		    2.0*M_PI);
  arg[10] = fmod(0.874016757 +   21.3299104960 *t,		    2.0*M_PI);
  arg[11] = fmod(5.481293872 +    7.4781598567 *t,		    2.0*M_PI);
  arg[12] = fmod(5.311886287 +    3.8133035638 *t,		    2.0*M_PI);
  arg[13] = fmod(                 0.024381750  *t + 0.00000538691*t*t,2.0*M_PI);
  
  X = -16616.99 + sum(NXT0, XTabT0, arg) + 
    t*(2004191742.88 + sum(NXT1, XTabT1, arg) +
       t*(-427219.05 + sum(NXT2, XTabT2, arg) +
	  t*(-198620.54 + sum(NXT3, XTabT3, arg) +
	     t*(-46.05 + sum(NXT4, XTabT4, arg) + t*5.98))));

  Y = -6950.78 + sum(NYT0, YTabT0, arg) + 
    t*(-25381.99 + sum(NYT1, YTabT1, arg) +
       t*(-22407250.99 + sum(NYT2, YTabT2, arg) +
	  t*(1842.28 + sum(NYT3, YTabT3, arg) +
	     t*(1113.06 + sum(NYT4, YTabT4, arg) + t*0.99))));

  S = 94.0 + sum(NDThetaT0, DThetaTabT0, arg) + 
    t*(3808.35 + sum(NDThetaT1, DThetaTabT1, arg) +
       t*(-119.94 + sum(NDThetaT2, DThetaTabT2, arg) +
  	  t*(-72574.09 + sum(NDThetaT3, DThetaTabT3, arg) +
  	     t*(27.70 + sum(NDThetaT4, DThetaTabT4, arg) + t*15.61))));
  
  DT = sum(NDtT0, DtTabT0, arg) + t*sum(NDtT1, DtTabT1, arg);

  X  *= SEC2RAD*1.0e-6;
  Y  *= SEC2RAD*1.0e-6;
  S  *= SEC2RAD*1.0e-6;
  S  -= X*Y/2.0;
  DT *= SEC2RAD*1.0e-6;
};

/*==============================================================================================*/






/*==============================================================================================*/
/*												*/
/* class SBATideEOP_BBP_02 implementation							*/
/*
 *  DIURNAL AND SEMIDIURNAL VARIATIONS
 *  OF POLAR MOTION DUE TO ATMOSPHERIC TIDES
 *  
 *  Atmospheric tides (both thermal and tidal) cause diurnal and semidiurnal components in the 
 *  polar motion. Most important contributions have been computed by Brzezinski, Bizouard and 
 *  Petrov (2002) ["Influence of the atmosphere on Earth Rotation : what new can be learned from 
 *  the recent atmospheric angular momentum estimates?", Surveys in Geophysics, 23, 33-69, 2002] 
 *  from NCEP/NCAR Angular Atmospheric Momentum data. Corresponding model includes 6 periodic 
 *  circular components (4 prograde diurnal terms, 2 semidiurnal terms) under the form :
 *  
 *  Dx - i*Dy = S_j i*(a^{in}_j + i*a^{op}_j)*e^{i*x_j}
 *  
 *  or :
 *  
 *  Dx =  S_j (-a^{op}_j *cos(x_j) - a^{in}_j *sin(x_j))
 *  Dy = -S_j ( a^{in}_j *cos(x_j) - a^{op}_j *sin(x_j))
 *  
 *  
 *  where xi is integer linear combination of Delaunay arguments, GMST + p :
 *  xi = a1 l + a2 l' + a3 D + a4 F + a5 W + a6 (GMST +p)
 */
/*												*/
/*==============================================================================================*/
void SBATideEOP_BBP_02::calc(const SBTime& T)
{
  double	x_j;
  double	s_xj, c_xj;
  int		i;
  DUT1D = DXD = DYD = 0.0;

  for (i=0; i<6; i++)
    {
      x_j = fmod(Tab[i].k[0]*T.fArg_l()		+
		 Tab[i].k[1]*T.fArg_lPrime()	+
		 Tab[i].k[2]*T.fArg_F()		+
		 Tab[i].k[3]*T.fArg_D()		+
		 Tab[i].k[4]*T.fArg_Omega()	+
		 Tab[i].k[5]*T.fArg_theta(),	2.0*M_PI);
      sincos(x_j, &s_xj, &c_xj);
      DXD   +=  -Tab[i].A_op*c_xj - Tab[i].A_in*s_xj;
      DYD   += -(Tab[i].A_in*c_xj - Tab[i].A_op*s_xj);
      DUT1D +=   Tab[i].a   *c_xj + Tab[i].b   *s_xj;
    };
  DUT1D *= 1.0e-6/86400.0;
  DXD	*= 1.0e-6*SEC2RAD;
  DYD	*= 1.0e-6*SEC2RAD;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBNonTideEOP_Viron_02 implementation							*/
/*
 *  POLAR MOTION INDUCED BY THE NON-TIDAL VARIATIONS
 *  IN OCEANIC ANGULAR MOMENTUM
 *  
 *  Non-tidal fluctuations in water heigh and currents (partly caused by atmospheric wind 
 *  and pressure variations) induce variation of equatorial oceanic angular momentum (OAM). 
 *  de Viron, Goosse, Bizouard and Lambert have derived such an OAM series spanning 
 *  1997.0-2002.0, and computed the associated perturbations on nutation, diurnal and subdiurnal 
 *  polar motion, and UT1 (communication done at EGS General Assembly 2002) for the span 
 *  1997.0-2002.0. The atmospheric perturbations, as derived from Atmospheric Angular Momentum (AAM) 
 *  series of the NCEP/NCAR reanalyis project, were considered additionaly. We present both OAM, 
 *  AAM and cumulated effects.
 *  
 *  For high frequency PM, there is only one prominent wave under the form :
 *  
 *  Dx - i*Dy = S_j (a^{in}_j + i*a^{op}_j)*e^{i*x_j}
 *
 *  or :
 *
 *  Dx =  S_j (a^{in}_j *cos(x_j) - a^{op}_j *sin(x_j))
 *  Dy = -S_j (a^{op}_j *cos(x_j) + a^{in}_j *sin(x_j))
 *  
 *  where xi is integer linear combination of Delaunay arguments, GMST + p :
 *  xi = a1*l + a2*l' + a3*D + a4*F + a5*W + a6*(GMST +p)
 */
/*												*/
/*==============================================================================================*/
void SBNonTideEOP_Viron_02::calc(const SBTime& T)
{
  double	x_j;
  double	s_xj, c_xj;
  int		i;
  DUT1D = DXD = DYD = 0.0;

  for (i=0; i<1; i++)
    {
      x_j = fmod(Tab[i].k[0]*T.fArg_l()		+
		 Tab[i].k[1]*T.fArg_lPrime()	+
		 Tab[i].k[2]*T.fArg_F()		+
		 Tab[i].k[3]*T.fArg_D()		+
		 Tab[i].k[4]*T.fArg_Omega()	+
		 Tab[i].k[5]*T.fArg_theta(),	2.0*M_PI);
      sincos(x_j, &s_xj, &c_xj);
      DXD   +=   Tab[i].A_in*c_xj - Tab[i].A_op*s_xj;
      DYD   += -(Tab[i].A_op*c_xj + Tab[i].A_in*s_xj);
      DUT1D +=   Tab[i].a   *c_xj + Tab[i].b   *s_xj;
    };
  DUT1D *= 1.0e-6/86400.0;
  DXD	*= 1.0e-6*SEC2RAD;
  DYD	*= 1.0e-6*SEC2RAD;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBGravTideEOP_B_02 implementation							*/
/*
 *  POLAR MOTION INDUCED BY THE LUNI-SOLAR GRAVITATIONAL TIDES
 *  
 *  Table with the harmonic development of the model of polar motion due to the lunisolar 
 *  torque on the triaxial Earth, corresponding to that what is frequently referred to as 
 *  high-frequency nutation. The coefficients have been computed for the model of non-rigid 
 *  Earth and refer to the conventional intermediate pole (CIP) which was defined by Resolution 
 *  B1.7 of the last IAU General Assembly. This table is based on the discussion and comparisons 
 *  done during the last 3 months by the Sub-WG "Subdiurnal Nutations" of the IAU Commission 19 
 *  WG on Nutation. Two models for non-rigid Earth were taken into account in this discussion, 
 *  one by Mathews and Bretagnon (2002, Proc. JSR 2001) and the other one by Brzezinski (2001, 
 *  Proc. JSR 2000), Brzezinski and Capitaine (2002, Proc. JSR 2001). The papers describing these 
 *  theories are in preparation and should be soon submitted for publication in the reviewed 
 *  international journals. After discussing several details and introducing all the necessary 
 *  corrections, Sonny and myself could reach a consensus on the attached table.
 *
 *  Coefficients in microarcseconds 
 *  Cut-off: 0.5 muas on the amplitude defined as the 
 *  square root of the sin and cos coefficients 
 *  of PM X or PM Y, whichever is larger 
 */
/*												*/
/*==============================================================================================*/
void SBGravTideEOP_B_02::calc(const SBTime& T)
{
  double	x_j;
  double	s_xj, c_xj;
  int		i;
  DUT1D = DXD = DYD = 0.0;

  for (i=0; i<10; i++)
    {
      x_j = fmod(Tab[i].k[0]*T.fArg_theta()	+
		 Tab[i].k[1]*T.fArg_l()		+
		 Tab[i].k[2]*T.fArg_lPrime()	+
		 Tab[i].k[3]*T.fArg_F()		+
		 Tab[i].k[4]*T.fArg_D()		+
		 Tab[i].k[5]*T.fArg_Omega(),	2.0*M_PI);
      sincos(x_j, &s_xj, &c_xj);
      DXD   += Tab[i].SinX*s_xj + Tab[i].CosX*c_xj;
      DYD   += Tab[i].SinY*s_xj + Tab[i].CosY*c_xj;
    };
  //  DUT1D *= 1.0e-6/86400.0;
  DXD	*= 1.0e-6*SEC2RAD;
  DYD	*= 1.0e-6*SEC2RAD;
};
/*==============================================================================================*/




/*==============================================================================================*/

