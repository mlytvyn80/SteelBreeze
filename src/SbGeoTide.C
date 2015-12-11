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
#include "SbGeoDelay.H"
#include "SbGeoTide.H"
#include "SbGeoProject.H"
#include "SbGeoStations.H"
#include "SbGeoEop.H"
#include "SbGeoEphem.H"
#include "SbGeoRefFrame.H"


/*==============================================================================================*/
/*												*/
/* constants description									*/
/*												*/
/*==============================================================================================*/
const double SBOceanTideLd::SPEED[11]=
{
  1.40519e-04, 1.45444e-04, 1.37880e-04, 1.45842e-04, 0.72921e-04,
  0.67598e-04, 0.72523e-04, 0.64959e-04, 0.53234e-05, 0.26392e-05, 0.03982e-05
};

const double SBOceanTideLd::ANGFAC[4][11]=
{
  {  2.0, 0.0, 2.0, 2.0, 1.0 , 1.0 ,-1.0 , 1.0 , 0.0, 0.0, 2.0},
  { -2.0, 0.0,-3.0, 0.0, 0.0 ,-2.0 , 0.0 ,-3.0 , 2.0, 1.0, 0.0},
  {  0.0, 0.0, 1.0, 0.0, 0.0 , 0.0 , 0.0 , 1.0 , 0.0,-1.0, 0.0},
  {  0.0, 0.0, 0.0, 0.0, 0.25,-0.25,-0.25,-0.25, 0.0, 0.0, 0.0}
};

//that funny people need this funny date ("ICAPD - Days since JAN 0, 0.00 UT, 1975") as a reference:
const SBMJD SBOceanTideLd::_1975(1975, 1, 0); 

const double SBSolidTideLd::h_0_el	=  0.6026;
const double SBSolidTideLd::h_2_el	= -0.0006;
const double SBSolidTideLd::l_0_el	=  0.0831;
const double SBSolidTideLd::l_2_el	=  0.0002;
const double SBSolidTideLd::h_0_an	=  0.6078;
const double SBSolidTideLd::h_2_an	= -0.0006;
const double SBSolidTideLd::l_0_an	=  0.0847;
const double SBSolidTideLd::l_2_an	=  0.0002;

const SBSolidTideLdIERS96::TableEntry73a SBSolidTideLdIERS96::Table_73a_el[11] =
{
  { { 1, 0, 2, 0, 2},  -0.11, -0.01},
  { { 0, 0, 2, 0, 1},  -0.12, -0.01},
  { { 0, 0, 2, 0, 2},  -0.63, -0.04},
  { { 1, 0, 0, 0, 0},   0.07,  0.0 },
  { { 0, 1, 2,-2, 2},  -0.06,  0.0 },
  { { 0, 0, 2,-2, 2},  -1.29,  0.05},
  { { 0, 0, 0, 0,-1},  -0.23,  0.01},
  { { 0, 0, 0, 0, 0},  12.25, -0.65},
  { { 0, 0, 0, 0, 1},   1.77, -0.09},
  { { 0,-1, 0, 0, 0},  -0.51,  0.03},
  { { 0, 0,-2, 2,-2},  -0.11,  0.01}
};

const SBSolidTideLdIERS96::TableEntry73a SBSolidTideLdIERS96::Table_73a_an[11] =
{
  { { 1, 0, 2, 0, 2},  -0.09,  0.0 },
  { { 0, 0, 2, 0, 1},  -0.10,  0.0 },
  { { 0, 0, 2, 0, 2},  -0.53,  0.02},
  { { 1, 0, 0, 0, 0},   0.06,  0.0 },
  { { 0, 1, 2,-2, 2},  -0.05,  0.0 },
  { { 0, 0, 2,-2, 2},  -1.23,  0.07},
  { { 0, 0, 0, 0,-1},  -0.22,  0.01},
  { { 0, 0, 0, 0, 0},  12.04, -0.72},
  { { 0, 0, 0, 0, 1},   1.74, -0.10},
  { { 0,-1, 0, 0, 0},  -0.50,  0.03},
  { { 0, 0,-2, 2,-2},  -0.11,  0.01}
};

const SBSolidTideLdIERS96::TableEntry73b SBSolidTideLdIERS96::Table_73b_el[ 5] =
{
  { { 0, 0, 0, 0, 1},  -0.05,  0.0 , 0.0 , 0.0 },
  { { 0, 0,-2, 2,-2},   0.05,  0.0 , 0.0 , 0.0 },
  { {-1, 0, 0, 0, 0},   0.06,  0.0 , 0.0 , 0.0 },
  { { 0, 0,-2, 0,-2},   0.12,  0.0 , 0.0 , 0.0 },
  { { 0, 0,-2, 0,-1},   0.05,  0.0 , 0.0 , 0.0 }
};

const SBSolidTideLdIERS96::TableEntry73b SBSolidTideLdIERS96::Table_73b_an[ 5] =
{
  { { 0, 0, 0, 0, 1},   0.47,  0.23, 0.16, 0.07},
  { { 0, 0,-2, 2,-2},  -0.20, -0.12,-0.11,-0.05},
  { {-1, 0, 0, 0, 0},  -0.11, -0.08,-0.09,-0.04},
  { { 0, 0,-2, 0,-2},  -0.13, -0.11,-0.15,-0.07},
  { { 0, 0,-2, 0,-1},  -0.05, -0.05,-0.06,-0.03}
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBOceanTideLd implementation								*/
/*												*/
/*==============================================================================================*/
/** A constructor.
 * Creates a copy of object.
 */
SBOceanTideLd::SBOceanTideLd(const SBConfig* Cfg_) : Vector3(v3Zero)
{
  Cfg = Cfg_;
  for (int i=0; i<11; i++) ANGLE[i]=0.0;
};

/**COMPUTES THE ANGULAR ARGUMENT WHICH DEPENDS ON TIME FOR 11 TIDAL ARGUMENT CALCULATIONS.
 * This strange subroutine has been printed in the IERS Technical Note 21, 
 * IERS Conventions (1996). It is assumed that it calculates the "(omega_j*t + hi_j)" part 
 * of the equation (4), p.53 (however, it is not stated explicitly).
 */
void SBOceanTideLd::ARG_IERS(const SBTime& Time)
{
  //
  //  FRACTIONAL PART OF DAY IN SECONDS
  //
  //??  double FDAY = Time.TT().time()*86400.0;
  //??  double FDAY = Time.TAI().time()*86400.0;
  //??  double FDAY = Time.TDB().time()*86400.0;
  double FDAY = Time.UT1().time()*86400.0;
  //
  //does anybody know what is the meaning of the variable `CAPT' and what is the 
  //time scale of the input `DAY'?
  //??  double CAPT = (27392.500528 + 1.000000035*floor((Time.TT() - _1975)))/36525.0;
  //??  double CAPT = (27392.500528 + 1.000000035*floor((Time.TAI() - _1975)))/36525.0;
  //??  double CAPT = (27392.500528 + 1.000000035*floor((Time.TDB() - _1975)))/36525.0;
  double CAPT = (27392.500528 + 1.000000035*floor((Time.UT1() - _1975)))/36525.0;

  //
  // MEAN LONGITUDE OF SUN AT BEGINNING OF DAY
  //
  double H0 = (279.69668 + (36000.768930485 + 3.03e-4*CAPT)*CAPT)*DEG2RAD;
  //
  // MEAN LONGITUDE OF MOON AT BEGINNING OF DAY
  //
  double S0 = (((1.9e-6*CAPT - 0.001133)*CAPT + 481267.88314137)*CAPT + 270.434358)*DEG2RAD;
  //
  // MEAN LONGITUDE OF LUNAR PERIGEE AT BEGINNING OF DAY
  //
  double P0 = (((-1.2e-5*CAPT - 0.010325)*CAPT + 4069.0340329577)*CAPT + 334.329653)*DEG2RAD;
  
  for (int K=0; K<11; K++)
    {
      ANGLE[K] = SPEED[K]*FDAY + 
	ANGFAC[0][K]*H0 + ANGFAC[1][K]*S0 + ANGFAC[2][K]*P0 + ANGFAC[3][K]*2.0*M_PI;
      ANGLE[K] = fmod(ANGLE[K], 2.0*M_PI);
      if (ANGLE[K]<0.0) ANGLE[K] = ANGLE[K] + 2.0*M_PI;
    };
};

const Vector3& SBOceanTideLd::operator()(const SBOLoad *OLoad, 
					 const SBTime  &Time)
{
  if (!OLoad)
    {
      Log->write(SBLog::ERR, SBLog::TIDES, ClassName() + ": OLoad is NULL");
      clear();
      return *this;
    };
  
  /*
    IERS Technical Note 21,
    IERS Conventions (1996)
    page 53 says:
    "   A shorter form of (2) is obtained if the summation considers only the tidal 
    species of Table 7.1 and corrections for the modulating effect of the lunar node.
    Then, 
    
    \Delta c = \Sum ( f_j*A_cj*cos(omega_j*t + hi_j + u_j - Phi_cj) ),	(4)
    
    where f_j and u_j depend on longitude of lunar node according to Table 26 of 
    Doodson (1928)."
  */
  double	fS1N, fC1N, fS2N, fC2N, fS3N, fC3N;
  double	f[11], u[11], fTmp = Time.fArg_Omega();
  for (int i=0; i<11; i++) 
    {
      f[i] = 1.0;
      u[i] = 0.0;
    };
  
  sincos (    fTmp,  &fS1N, &fC1N);
  sincos (2.0*fTmp,  &fS2N, &fC2N);
  sincos (3.0*fTmp,  &fS3N, &fC3N);
  
  f[ 0] = 1.0004 - 0.0373*fC1N + 0.0002*fC2N              ;
  f[ 3] = 1.0241 + 0.2863*fC1N + 0.0083*fC2N - 0.0015*fC3N;
  f[ 4] = 1.0060 + 0.1150*fC1N - 0.0088*fC2N + 0.0006*fC3N;
  f[ 5] = 1.0089 + 0.1871*fC1N - 0.0147*fC2N + 0.0014*fC3N;
  f[ 8] = 1.0429 + 0.4135*fC1N - 0.0040*fC2N              ;
  f[ 9] = 1.0000 - 0.1300*fC1N + 0.0013*fC2N              ;
  
  u[ 0] =        - 2.14  *fS1N                            ;
  u[ 3] =        -17.74  *fS1N + 0.68  *fS2N - 0.04  *fS3N;
  u[ 4] =        - 8.86  *fS1N + 0.68  *fS2N - 0.07  *fS3N;
  u[ 5] =         10.80  *fS1N - 1.34  *fS2N + 0.19  *fS3N;
  u[ 8] =        -23.74  *fS1N + 2.68  *fS2N - 0.38  *fS3N;
  
  /*
    "The astronomical arguments needed in (4) can be computed with subroutune ARG below."
  */
  ARG_IERS(Time);
  
  //summation:
  clear();
  for (int i=0; i<3; i++)
    for (int j=0; j<11; j++)
      vec[i]+= f[j]*OLoad->amplitude(i,j)*cos(ANGLE[j] + (u[j] - OLoad->phase(i,j))*DEG2RAD);
  
  //make it in VEN:
  vec[1] *=-1.0;
  vec[2] *=-1.0;

  return *this;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBPolarTideLd implementation								*/
/*												*/
/*==============================================================================================*/
const Vector3& SBPolarTideLd::operator()(SBStation *Station, const SBEOP *EOP, const SBTime &T)
{
  static const SBMJD T2000(2000, 1, 1);

  if (!Station || !EOP)
    {
      Log->write(SBLog::ERR, SBLog::TIDES, ClassName() + ": Station or EOP is NULL");
      clear();
      return *this;
    };

  double s_lam, c_lam;
  sincos(Station->longitude(), &s_lam, &c_lam);
  double tet = 0.5*M_PI - Station->latitude();
  
  double m1 =   EOP->dX(T)*RAD2SEC - (0.054 + (T - T2000)*0.00083/365.25);
  double m2 = -(EOP->dY(T)*RAD2SEC - (0.357 + (T - T2000)*0.00395/365.25));

  vec[0] = -32.0e-3*sin(2.0*tet)*( m1*c_lam + m2*s_lam );
  vec[1] =   9.0e-3*cos(    tet)*( m1*s_lam - m2*c_lam );
  vec[2] =   9.0e-3*cos(2.0*tet)*( m1*c_lam + m2*s_lam );
  
  //  vec[0] = -32.0e-3*sin(2.0*tet)*( EOP->dX(T)*c_lam - EOP->dY(T)*s_lam )*RAD2SEC;
  //  vec[1] =   9.0e-3*cos(    tet)*( EOP->dX(T)*s_lam + EOP->dY(T)*c_lam )*RAD2SEC;
  //  vec[2] =   9.0e-3*cos(2.0*tet)*( EOP->dX(T)*c_lam - EOP->dY(T)*s_lam )*RAD2SEC;

  return *this;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBSolidTideLd implementation								*/
/*												*/
/*==============================================================================================*/
/**A constructor.
 * Creates a copy of object.
 */
SBSolidTideLd::SBSolidTideLd(const SBConfig* Cfg_) 
  : Vector3(v3Zero),
    TideDeg2ip(v3Zero), TideDeg3ip(v3Zero), TideDeg2op(v3Zero), TideLatDepend(v3Zero), Step1(v3Zero), 
    Step2(v3Zero), 
    Moon_TRS(v3Zero), Sun_TRS(v3Zero), FmREN(m3E)
{
  Cfg = Cfg_;
  if (Cfg->isSolidTideAnelastic())
    {
      h_0 = h_0_an;
      h_2 = h_2_an;
      l_0 = l_0_an;
      l_2 = l_2_an;
    }
  else
    {
      h_0 = h_0_el;
      h_2 = h_2_el;
      l_0 = l_0_el;
      l_2 = l_2_el;
    };
  CoeffSun = 0.0;
  CoeffMoon= 0.0;
};

double SBSolidTideLd::h2(double phi, SBStation* Station) const 
{
  double H2 = h_0;
  if (Station && Station->p_TideH2())
    H2 += Station->p_TideH2()->v();

  if (!Cfg->isSolidTideLatDepend())
    return H2;
  double sin_phi = sin(phi);
  return H2 + h_2*(3.0*sin_phi*sin_phi - 1.0)/2.0;
};

double SBSolidTideLd::l2(double phi, SBStation* Station) const 
{
  double L2 = l_0;
  if (Station && Station->p_TideL2())
    L2 += Station->p_TideL2()->v();

  if (!Cfg->isSolidTideLatDepend())
    return L2;
  double sin_phi = sin(phi);
  return L2 + l_2*(3.0*sin_phi*sin_phi - 1.0)/2.0;
};

Vector3 SBSolidTideLd::permTide(SBStation* St)
{
  double sin_phi, cos_phi;
  sincos(St->rt().phi(), &sin_phi, &cos_phi); // geocentric latitude of station
  double dr = -0.12083*(3.0/2.0*sin_phi*sin_phi - 0.5);
  double dn = -0.05071*cos_phi*sin_phi;
  return 
    St->fmVEN()*Vector3(dr, 0.0, dn);
};

Vector3 SBSolidTideLd::deg2InPhase(SBStation* Station, const Vector3& B)
{
  Vector3	Rj    = B;
  Vector3	r     = RotMatrix(Z_AXIS, Station->p_TideLag()->v())*Station->rt();
  double	r_m   = r.module();
  double	Rj_m  = Rj.module();
  double	r_m2  = r_m*r_m;
  double	Rj_m2 = Rj_m*Rj_m;
  double	r_Rj  = r.unify()*Rj.unify(); // r & Rj is now unit vectors
  double	L2    = l2(r.phi(), Station);
  double	H2    = h2(r.phi(), Station);

  return (r_m2*r_m2/Rj_m/Rj_m2)*(3.0*L2*r_Rj*Rj + (3.0*(H2/2.0 - L2)*r_Rj*r_Rj - H2/2.0)*r);
};

Vector3 SBSolidTideLd::dTide_dLag(SBStation *Station, SBEphem *Ephem)
{
  Vector3	dR_dPsi;
  Vector3	rs = RotMatrix(Z_AXIS, Station->p_TideLag()->v())*Station->rt();

  RotDerMatrix	Lp(Z_AXIS);
  Vector3	rc = Lp(Station->p_TideLag()->v())*(Station->rt()/Station->rt().module());
  
  double L2	= l2(Station->rt().phi(), Station);
  double H2	= h2(Station->rt().phi(), Station);

  double Rs	= rs.module();
  double Rs2	= Rs*Rs;
  double R2	= Moon_TRS.module();
  double R22	= R2*R2;
  double R3	= Sun_TRS.module();
  double R32    = R3*R3;
  
  double R2r	= Moon_TRS*rs/R2/Rs;
  double R3r	= Sun_TRS *rs/R3/Rs;

  double R2rc	= Moon_TRS*rc/R2;
  double R3rc	= Sun_TRS *rc/R3;

  
  dR_dPsi =
    //Moon:
    (
     (3.0*(H2-2.0*L2)*(R2r*R2rc))*rs/Rs +
     3.0*L2*R2rc*Moon_TRS/R2 +
     0.5*(3.0*(H2-2.0*L2)*R2r*R2r - H2)*rc
     )
    /Ephem->emRat()*Rs2*Rs2/R22/R2 +
    //Sun:
    (
     (3.0*(H2-2.0*L2)*(R3r*R3rc))*rs/Rs +
     3.0*L2*R3rc*Sun_TRS/R3 +
     0.5*(3.0*(H2-2.0*L2)*R3r*R3r - H2)*rc
     )
    *Ephem->sun().gm()/Ephem->earth().gm()*Rs2*Rs2/R32/R3;
  
  return dR_dPsi;
};

Vector3 SBSolidTideLd::dTide_dH2(SBStation *Station, SBEphem *Ephem)
{
  Vector3	dR_dh;

  Vector3	Ru2   = Moon_TRS.unit();
  Vector3	Ru3   = Sun_TRS .unit();
  Vector3	ru    = Station->rt().unit();

  double Rs	= Station->rt().module();
  double Rs2	= Rs*Rs;
  double R2	= Moon_TRS.module();
  double R22	= R2*R2;
  double R3	= Sun_TRS.module();
  double R32    = R3*R3;

  double R2r    = Ru2*ru;
  double R3r    = Ru3*ru;
  
  dR_dh =
    //Moon:
    (0.5*(3.0*R2r*R2r - 1.0)*ru)/Ephem->emRat()*Rs2*Rs2/R22/R2 +
    //Sun:
    (0.5*(3.0*R3r*R3r - 1.0)*ru)*Ephem->sun().gm()/Ephem->earth().gm()*Rs2*Rs2/R32/R3;
  
  return dR_dh;
};

Vector3 SBSolidTideLd::dTide_dL2(SBStation *Station, SBEphem *Ephem)
{
  Vector3	dR_dl;

  Vector3	Ru2   = Moon_TRS.unit();
  Vector3	Ru3   = Sun_TRS .unit();
  Vector3	ru    = Station->rt().unit();

  double Rs	= Station->rt().module();
  double Rs2	= Rs*Rs;
  double R2	= Moon_TRS.module();
  double R22	= R2*R2;
  double R3	= Sun_TRS.module();
  double R32    = R3*R3;

  double R2r    = Ru2*ru;
  double R3r    = Ru3*ru;
  
  dR_dl =
    //Moon:
    (3.0*R2r*(Ru2 - R2r*ru))/Ephem->emRat()*Rs2*Rs2/R22/R2 +
    //Sun:
    (3.0*R3r*(Ru3 - R3r*ru))*Ephem->sun().gm()/Ephem->earth().gm()*Rs2*Rs2/R32/R3;
  
  return dR_dl;
};

const Vector3& SBSolidTideLd::makeStep1(SBStation* Station, SBEphem* Ephem)
{
  
  TideDeg2ip = Step1 = 
    deg2InPhase(Station, Moon_TRS)/Ephem->emRat() +
    deg2InPhase(Station, Sun_TRS )*Ephem->sun().gm()/Ephem->earth().gm();
  
  double	Re = Station->rt().module();
  double	Rs = Sun_TRS.module();
  double	Rm = Moon_TRS.module();

  // used for debug output:
  CoeffMoon= Re*Re*Re*Re/Rm/Rm/Rm/Ephem->emRat();
  CoeffSun = Re*Re*Re*Re/Rs/Rs/Rs*Ephem->sun().gm()/Ephem->earth().gm();

  return Step1;
};

const Vector3& SBSolidTideLd::makeStep2(SBStation* St, const SBFrame &Frame)
{
  double dH = - 0.0253*sin(St->latitude())*cos(St->latitude())*sin(St->longitude() + Frame.gmst());
  return (Step2 = FmREN*Vector3(dH, 0.0, 0.0));
};

const Vector3& SBSolidTideLd::operator()(SBStation *Station, SBEphem *Ephem, const SBFrame &Frame)
{
  if (!Station || !Ephem)
    {//should never happend
      Log->write(SBLog::ERR, SBLog::TIDES, ClassName() + ": Station or Ephem is NULL");
      clear();
      return *this;
    };
  
  //  Ephem->calc(Frame.time().TDB());
  Moon_TRS= Frame.crs2trs()*Ephem->geoMoon().r();
  Sun_TRS = Frame.crs2trs()*(Ephem->sun().r() - Ephem->earth().r());

  // some tide displacements computated in local radial-transverse coordinate system, 
  // it is not the same as local VEN, FmREN makes transformation from this system to TRF:
  FmREN = RotMatrix(NORT, -Station->rt().lambda())*RotMatrix(EAST, Station->rt().phi());

  (Vector3&)*this = makeStep1(Station, Ephem) + makeStep2(Station, Frame);
  
  if (Cfg->isRemovePermTide()) 
    {
      Vector3 PermanentTide = permTide(Station);
      if (Log->isEligible(SBLog::DBG, SBLog::TIDES))
	PermanentTide.report2Log(SBLog::DBG, SBLog::TIDES, ClassName() + ": Permanent Tide--(m): ", 11, 8);
      
      *this-=PermanentTide;
    };

  // make a report:
  if (Log->isEligible(SBLog::DBG, SBLog::TIDES))
    {
      Log->write(SBLog::DBG, SBLog::TIDES, ClassName() + ": modes: " + 
		 (Cfg->isSolidTideAnelastic()?"Anelastic; ":"Elastic; ") +
		 (Cfg->isSolidTideLatDepend()?"With Latitude correction":"Without Latitude correction"));
      //
      Log->write(SBLog::DBG, SBLog::TIDES, ClassName() + ": Arg (TAI)          : " + 
		 Frame.time().TAI().toString(SBMJD::F_Short));
      Log->write(SBLog::DBG, SBLog::TIDES, ClassName() + ": Arg (TAI (MJD))    : " + 
		 Frame.time().TAI().toString(SBMJD::F_MJD));
      Log->write(SBLog::DBG, SBLog::TIDES, ClassName() + ": Station            : [" + 
		 Station->id().toString() + "], " + Station->name());
      
      Station->rt().report2Log(SBLog::DBG, SBLog::TIDES, ClassName() + ": Station R  (TRF, m): ", 20, 6);

      Ephem->geoMoon().r().report2Log(SBLog::DBG, SBLog::TIDES, ClassName() + ": Moon (Geo) (CRF, m): ", 20, 6);
      (Ephem->sun().r() - Ephem->earth().r()).report2Log(SBLog::DBG, SBLog::TIDES, ClassName() + 
							 ": Sun  (Geo) (CRF, m): ", 20, 6);
      
      Moon_TRS.report2Log(SBLog::DBG, SBLog::TIDES, ClassName() + ": Moon (Geo) (TRF, m): ", 20, 6);
      Sun_TRS .report2Log(SBLog::DBG, SBLog::TIDES, ClassName() + ": Sun  (Geo) (TRF, m): ", 20, 6);
      
      Frame.crs2trs() .report2Log(SBLog::DBG, SBLog::TIDES, ClassName() + ": CRF2TRF matrix     : ");
      Station->fmVEN().report2Log(SBLog::DBG, SBLog::TIDES, ClassName() + ": VEN2TRF matrix     : ");
      
      Log->write(SBLog::DBG, SBLog::TIDES, ClassName() + ": Coeff[Moon]: " + QString().sprintf("%.8g", CoeffMoon));
      Log->write(SBLog::DBG, SBLog::TIDES, ClassName() + ": Coeff[Sun ]: " + QString().sprintf("%.8g", CoeffSun));

      Step1.report2Log(SBLog::DBG, SBLog::TIDES, ClassName() + ": Step one   (TRF, m): ", 11, 8);
      Step2.report2Log(SBLog::DBG, SBLog::TIDES, ClassName() + ": Step two   (TRF, m): ", 11, 8);
    };
  return *this;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBSolidTideLdIERS92 implementation							*/
/*												*/
/*==============================================================================================*/
double SBSolidTideLdIERS92::h2(double, SBStation* Station) const 
{
  double H2 = 0.6090;
  if (Station && Station->p_TideH2())
    H2 += Station->p_TideH2()->v();
  return H2;
};

double SBSolidTideLdIERS92::l2(double, SBStation* Station) const 
{
  double L2 = 0.0852;
  if (Station && Station->p_TideL2())
    L2 += Station->p_TideL2()->v();
  return L2;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBSolidTideLdIERS96 implementation							*/
/*												*/
/*==============================================================================================*/
/** A constructor. */
SBSolidTideLdIERS96::SBSolidTideLdIERS96(const SBConfig* Cfg_) : SBSolidTideLd(Cfg_)
{
  Tab_73a=Cfg->isSolidTideAnelastic()?Table_73a_an:Table_73a_el;
  Tab_73b=Cfg->isSolidTideAnelastic()?Table_73b_an:Table_73b_el;
};

/**Calculates term from eq.(9), IERS Standards 1996.
 * Calculates corresponded j-th celestial body degree 3 in-phase tide contribution term.
 * The term is not scaled by GMj/GMearth.
 */
Vector3 SBSolidTideLdIERS96::deg3InPhase(const Vector3& St, const Vector3& B)
{
  Vector3	Rj    = B;
  Vector3	r     = St;
  double	r_m   = r.module();
  double	Rj_m  = Rj.module();
  double	r_m2  = r_m*r_m;
  double	Rj_m2 = Rj_m*Rj_m;
  double	r_Rj  = r.unify()*Rj.unify(); // r & Rj is now unit vectors

  return 
    (r_m2*r_m2*r_m/Rj_m2/Rj_m2)*
    ((h3()*(5.0*r_Rj*r_Rj - 3.0)*r_Rj/2.0)*r + (l3()*3.0/2.0*(5.0*r_Rj*r_Rj - 1.0))*(Rj - r_Rj*r));
};

/**Calculates term from eqs.(13), (14), IERS Standards 1996.
 * Calculates corresponded j-th celestial body degree 2 out-of-phase contribution from 
 * diurnal and semidiurnal tides term. The term is not scaled by GMj/GMearth.
 */
Vector3	SBSolidTideLdIERS96::deg2OutPhase(const Vector3& St, const Vector3& B)
{
  double	r_m   = St.module();
  double	Rj_m  = B .module();
  double	r_m2  = r_m*r_m;
  double	Rj_m2 = Rj_m*Rj_m;
  double	sin_2Phi = sin(2.0*B.phi()), cos_Phi = cos(B.phi());
  double	sin_phi, cos_phi, sin_2phi, cos_2phi;
  double	sin_dlam, cos_dlam, sin_2dlam, cos_2dlam;
  sincos(St.phi(),			&sin_phi,  &cos_phi);
  sincos(2.0*St.phi(),			&sin_2phi, &cos_2phi);
  sincos(St.lambda() - B.lambda(),	&sin_dlam, &cos_dlam);
  sincos(2.0*(St.lambda() - B.lambda()),&sin_2dlam,&cos_2dlam);

  double dr = 
    //Degree 2 out of phase contribution in radial displacement from diurnal tides:
    -0.5*h_Id()*sin_2Phi*sin_2phi*sin_dlam
    //Degree 2 out of phase contribution in radial displacement from semidiurnal tides:
    -0.5*h_Is()*cos_Phi*cos_Phi*cos_phi*cos_phi*sin_2dlam;

  double dn = 
    //Degree 2 out of phase contribution in transverse (north) displacement from diurnal tides:
    -l_Id()*sin_2Phi*cos_2phi*sin_dlam
    //Degree 2 out of phase contribution in transverse (north) displacement from semidiurnal tides:
    +0.5*l_Is()*cos_Phi*cos_Phi*sin_2phi*sin_2dlam;
  
  double de = 
    //Degree 2 out of phase contribution in transverse (east) displacement from diurnal tides:
    -l_Id()*sin_2Phi*sin_phi*cos_dlam
    //Degree 2 out of phase contribution in transverse (east) displacement from semidiurnal tides:
    -l_Is()*cos_Phi*cos_Phi*cos_phi*cos_2dlam;
  
  return (3.0/2.0*r_m2*r_m2/Rj_m/Rj_m2)*(FmREN*Vector3(dr, de, dn));
};

/**Calculates term from eqs.(11), (12), IERS Standards 1996.
 * Calculates transverse displacement due to l(1) term corresponded to j-th
 * celestial body. The term is not scaled by GMj/GMearth.
 */
Vector3	SBSolidTideLdIERS96::latDepend(const Vector3& St, const Vector3& B)
{
  Vector3	R = B;
  R.unify();
  double r_m   = St.module();
  double Rj_m  = B .module();
  double r_m2  = r_m*r_m;
  double Rj_m2 = Rj_m*Rj_m;
  double sin_phi, cos_phi, sin_lam, cos_lam, sin_2lam, cos_2lam;
  sincos(St.phi(), &sin_phi, &cos_phi);
  sincos(St.lambda(), &sin_lam, &cos_lam);
  sincos(2.0*St.lambda(), &sin_2lam, &cos_2lam);

  double dn =
    //transverse displacement (north) from diurnal tides:
    -l_1d()*R[Z_AXIS]*sin_phi*(R[X_AXIS]*cos_lam + R[Y_AXIS]*sin_lam)
    //transverse displacement (north) from semidiurnal tides:
    -0.5*l_1s()*cos_phi*(
			 (R[X_AXIS]*R[X_AXIS] - R[Y_AXIS]*R[Y_AXIS])*cos_2lam 
			 + 2.0*R[X_AXIS]*R[Y_AXIS]*sin_2lam
			 );
  
  double de = 
    //transverse displacement (east) from diurnal tides:
    -l_1d()*R[Z_AXIS]*cos(2.0*St.phi())*(R[Y_AXIS]*cos_lam - R[X_AXIS]*sin_lam)
    //transverse displacement (east) from semidiurnal tides:
    -0.5*l_1s()*cos_phi*sin_phi*(
				 (R[X_AXIS]*R[X_AXIS] - R[Y_AXIS]*R[Y_AXIS])*sin_2lam
				 - 2.0*R[X_AXIS]*R[Y_AXIS]*cos_2lam
				 );

  double scale = sin_phi*3.0*r_m2*r_m2/(Rj_m2*Rj_m);
  de *= scale;
  dn *= scale;
  return FmREN*Vector3(0.0, de, dn);
};

const Vector3& SBSolidTideLdIERS96::makeStep1(SBStation* Station, SBEphem* Ephem)
{
  TideDeg2ip = SBSolidTideLd::makeStep1(Station, Ephem);
  TideDeg3ip = deg3InPhase(Station->rt(), Moon_TRS)/Ephem->emRat();
  TideDeg2op = Cfg->isSolidTideAnelastic()?
    (deg2OutPhase(Station->rt(), Moon_TRS)/Ephem->emRat() + 
     deg2OutPhase(Station->rt(), Sun_TRS )*Ephem->sun().gm()/Ephem->earth().gm())
    :v3Zero;
  TideLatDepend = Cfg->isSolidTideAnelastic()&&Cfg->isSolidTideLatDepend()?
    (latDepend(Station->rt(), Moon_TRS)/Ephem->emRat() + 
     latDepend(Station->rt(), Sun_TRS )*Ephem->sun().gm()/Ephem->earth().gm())
    :v3Zero;
  
  return (Step1 = TideDeg2ip + TideDeg3ip + TideDeg2op + TideLatDepend);
};

const Vector3& SBSolidTideLdIERS96::makeStep2(SBStation* St, const SBFrame &Frame)
{
  double	dr=0.0, dn=0.0, de=0.0;
  double	sin_phi=sin(St->rt().phi());
  double	sin_2phi, cos_2phi;
  double	sin_th_l, cos_th_l, th;
  sincos(2.0*St->rt().phi(), &sin_2phi, &cos_2phi);
  
  //out of phase for K1, mm
  if (Cfg->isSolidTideAnelastic()) 
    dr=0.06*sin_2phi*cos(Frame.gmst() + M_PI + St->rt().lambda());
  
  for (int i=0; i<11; i++)
    {
      th = fmod(Frame.gmst() + M_PI - 
		(Frame.time().fArg_l()     *Tab_73a[i].N[0] + 
		 Frame.time().fArg_lPrime()*Tab_73a[i].N[1] + 
		 Frame.time().fArg_F()     *Tab_73a[i].N[2] +
		 Frame.time().fArg_D()     *Tab_73a[i].N[3] + 
		 Frame.time().fArg_Omega() *Tab_73a[i].N[4]), 
		2.0*M_PI);
      sincos(th + St->rt().lambda(), &sin_th_l, &cos_th_l);
      dr+= Tab_73a[i].DR*sin_2phi*sin_th_l;
      de+= Tab_73a[i].DT*sin_phi *cos_th_l;
      dn+= Tab_73a[i].DT*cos_2phi*sin_th_l;
    };
  Vector3 diurnFreqDepend = 1.0e-3*Vector3(dr, de, dn);
  
  dr=dn=de=0.0;
  for (int i=0; i<5; i++)
    {
      th = fmod(- (Frame.time().fArg_l()     *Tab_73b[i].N[0] + 
		   Frame.time().fArg_lPrime()*Tab_73b[i].N[1] + 
		   Frame.time().fArg_F()     *Tab_73b[i].N[2] +
		   Frame.time().fArg_D()     *Tab_73b[i].N[3] + 
		   Frame.time().fArg_Omega() *Tab_73b[i].N[4]), 2.0*M_PI);
      sincos(th, &sin_th_l, &cos_th_l);
      dr+= (3.0*sin_phi*sin_phi - 1.0)/2.0*(Tab_73b[i].DRip*cos_th_l + Tab_73b[i].DRop*sin_th_l);
      dn+= sin_2phi*(Tab_73b[i].DTip*cos_th_l + Tab_73b[i].DTop*sin_th_l);
    };
  Vector3 longFreqDepend = 1.0e-3*Vector3(dr, 0.0, dn);
  
  return (Step2 = FmREN*(diurnFreqDepend + longFreqDepend));
};

Vector3 SBSolidTideLdIERS96::permTide(SBStation* St)
{
  double sin_phi, cos_phi;
  sincos(St->rt().phi(), &sin_phi, &cos_phi);
  double f  = sqrt(5.0/4.0/M_PI)*(-0.31460);
  double dr = f*h2(St->rt().phi(), St)*(3.0/2.0*sin_phi*sin_phi - 0.5);
  double dn = f*l2(St->rt().phi(), St)*3.0*sin_phi*cos_phi;

  return St->fmVEN()*Vector3(dr, 0.0, dn);
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBTideLd implementation								*/
/*												*/
/*==============================================================================================*/
/**A constructor.
 * Creates a copy of object.
 */
SBTideLd::SBTideLd(const SBConfig* Cfg_) : Vector3(v3Zero)
{
  Cfg		= Cfg_;
  OceanTideLd	= new SBOceanTideLd(Cfg);
  PolarTideLd	= new SBPolarTideLd(Cfg);
  
  switch (Cfg->solidTideLd())
    {
    case SBConfig::ST_IERS92: 
      SolidTideLd = new SBSolidTideLdIERS92(Cfg);
      break;
    default:
    case SBConfig::ST_IERS96: 
      SolidTideLd = new SBSolidTideLdIERS96(Cfg);
      break;
    };
};

SBTideLd::~SBTideLd()
{
  Cfg = NULL;
  if (OceanTideLd) delete OceanTideLd;
  if (PolarTideLd) delete PolarTideLd;
  if (SolidTideLd) delete SolidTideLd;
};

const Vector3& SBTideLd::operator()(SBStation *Station, SBEphem *Ephem, SBFrame &Frame)
{
  Vector3	TideSolid;
  Vector3	TideOcean;
  Vector3	TidePolar;

  // make a report:
  if (Log->isEligible(SBLog::DBG, SBLog::TIDES))
    {
      Log->write(SBLog::DBG, SBLog::TIDES, ClassName() + ": Arg(TAI)           : " + 
		 Frame.time().TAI().toString());
      Log->write(SBLog::DBG, SBLog::TIDES, ClassName() + ": Station            : [" + 
		 Station->id().toString() + "], " + Station->name());
    };

  TideOcean = (*OceanTideLd)(Station->oLoad()?Station->oLoad():&Station->site()->oLoad(), Frame.time());
  TidePolar = (*PolarTideLd)(Station, Frame.eop(), Frame.time());
  TideSolid = (*SolidTideLd)(Station, Ephem, Frame);

//  TideOcean = v3Zero;
//  TidePolar = v3Zero;
//  TideSolid = v3Zero;

  (Vector3&)*this = Station->fmVEN()*(TideOcean + TidePolar) + TideSolid;

  // make a report:
  if (Log->isEligible(SBLog::DBG, SBLog::TIDES))
    {

      Station->fmVEN().report2Log(SBLog::DBG, SBLog::TIDES, ClassName() + ": VEN2TRF matrix     : ");
      TideOcean.report2Log(SBLog::DBG, SBLog::TIDES, ClassName() + ": Ocean Tide (VEN, m): ", 11, 8);
      TidePolar.report2Log(SBLog::DBG, SBLog::TIDES, ClassName() + ": Polar Tide (VEN, m): ", 11, 8);
      TideSolid.report2Log(SBLog::DBG, SBLog::TIDES, ClassName() + ": Solid Tide (TRF, m): ", 11, 8);


      report2Log(SBLog::DBG, SBLog::TIDES,           ClassName() + ": Total Tide (TRF, m): ", 11, 8);
    };

  return *this;
};
/*==============================================================================================*/
