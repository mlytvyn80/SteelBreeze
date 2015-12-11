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


#include "SbGeoAtmLoad.H"
#include "SbGeoDelay.H"
#include "SbGeoProject.H"
#include "SbGeoObsVLBI.H"
#include "SbGeoEphem.H"
#include "SbGeoRefFrame.H"
#include "SbGeoEop.H"
#include "SbGeoStations.H"
#include "SbGeoSources.H"
#include "SbGeoTide.H"
#include "SbGeoRefraction.H"



/*==============================================================================================*/
/*												*/
/* constants description									*/
/*												*/
/*==============================================================================================*/
const double SBDelay::CLight = 299792458.0;
const double SBDelay::AEarth =   6378136.6;
const double SBDelay::Gamma  =         1.0;


/*==============================================================================================*/
/*												*/
/* class SBDelay implementation									*/
/*												*/
/*==============================================================================================*/
SBDelay::SBDelay(const SBConfig* Cfg_, SBFrame* Frame_, SBEphem* Ephem_, 
		 SBTideLd* Tide_, SBRefraction* Refr_)
  : t1(TZero), 
    K(v3Zero), x1(v3Zero), x2(v3Zero), b(v3Zero), b_TRS(v3Zero), X1(v3Zero), X2(v3Zero),
    k1(v3Zero), k2(v3Zero), k1r(v3Zero), k2r(v3Zero), Ve(v3Zero), w1(v3Zero), w2(v3Zero)
{
  Cfg   = Cfg_;
  Frame = Frame_;
  Ephem = Ephem_;
  Tide	= Tide_;
  Refr  = Refr_;

  Mgr	= NULL;
  St_1	= NULL;
  St_2	= NULL;
  So	= NULL;
  Obs	= NULL;

  GravDelay   = 0.0;
  VacuumDelay = 0.0;
  GeomDelay   = 0.0;
  TotalDelay  = 0.0;
  AntennaDelay= 0.0;
  DTAtm1 = 0.0;
  DTAtm2 = 0.0;
  TotZenithDelay1 = 0.0;
  TotZenithDelay2 = 0.0;
  kbc		  = 0.0;
  DTau_Term_1	  = 0.0;
  DTau_Term_2	  = 0.0;
  DTau_Term_3	  = 0.0;
};

SBDelay::~SBDelay()
{
};

double SBDelay::calc(SBRunManager* Mgr_, 
		     SBStation *St_1_, SBStation *St_2_, 
		     SBSource *So_, 
		     SBObsVLBIEntry *Obs_, 
		     SBStationInfo* St_1Info, SBStationInfo* St_2Info)
{
  kbc	      = 0.0;
  DTau_Term_1 = 0.0;
  DTau_Term_2 = 0.0;
  DTau_Term_3 = 0.0;
  Ve.clear();
  w1.clear();
  w2.clear();

  SBMJD		t1j(TZero);
  double	c2 = cLight()*cLight();
  Vector3	r1, r2;

  Mgr	= Mgr_;
  St_1	= St_1_;
  St_2	= St_2_;
  So	= So_;
  Obs	= Obs_;

  Frame->calc(*Obs);
  //  Ephem->calc((t1=Frame->time().TT()));
  Ephem->calc((t1=Frame->time().TDB()));

  K  = So->rt();							// So               @CRS
  x1 = *Frame*((r1 = St_1->rt() + (*Tide)(St_1, Ephem, *Frame)));	// St_1(with tides) @CRS
  x2 = *Frame*((r2 = St_2->rt() + (*Tide)(St_2, Ephem, *Frame)));	// St_2(with tides) @CRS
  b  = x2 - x1;
  b_TRS = r2 - r1;							// for debug output
  
  /*
    IERS Technical Note 21,
    IERS Conventions (1996)
    page 91 says:
    "SUMMARY

    Assuming that time t1 is Atomic (TAI) time of reception of the VLBI signal at receiver 1,
    the following steps are recommended to correct the VLBI time delay for relativistic effects.

    1. Use equation 6 to estimate the barycentric station vector for receiver 1 
    [it seems not "1" but "i"]."
  */
  X1 = Ephem->earth().r() + x1;
  X2 = Ephem->earth().r() + x2;
  
  /*
    "
    2. Use equations 3,4, and 5 to estimate the vectors from the Sun, the Moon, and each planet
    except the Earth to receiver 1 ["i"?]."
  */
  double GravDelJ;
  double Uj = 0.0;
  double U_ = 0.0;

  GravDelay = 0.0;
  for (int j=0; j<11; j++)
    if (Ephem->body(j).isAttr(SBCelestBody::IsGravDelay) && Ephem->body(j).name()!="Earth")
      {
	t1j = std::min(t1, t1 - K*(Ephem->body(j).r() - X1)/cLight()/DAY2SEC);
	Ephem->calcI(j, t1j);
	double a = (1.0 + gamma())*Ephem->body(j).gm()/c2;
	Vector3 R1j = X1 - Ephem->body(j).r();
	Vector3 R2j = X2 - Ephem->earth().v()*(K*b)/cLight() - Ephem->body(j).r();
	
	/*
	  "
	  3. Use equation 1 to estimate the differential gravitational
	  delay of those bodies."
	*/
	GravDelay += (GravDelJ=a/cLight()*log((R1j.module() + K*R1j)/(R2j.module() + K*R2j)));
	Uj = Ephem->body(j).gm()/R1j.module()/c2;
	U_+=Uj;

	if (Cfg->isGravDelayHO())
	  {
	    if (acos(K*(-R1j)/R1j.module())<Cfg->gravHOThreshold())
	      {
		GravDelay += a*a/cLight()*(b*(R1j.unit() + K))/(R1j.module() + K*R1j)/(R1j.module() + K*R1j);
	      };
	    if (acos(K*(-R2j)/R2j.module())<Cfg->gravHOThreshold())
	      {
		GravDelay += a*a/cLight()*(b*(R2j.unit() + K))/(R2j.module() + K*R2j)/(R2j.module() + K*R2j);
	      };
	  };

	if (Log->isEligible(SBLog::DBG, SBLog::GRAVDELAY))
	  {
	    Log->write(SBLog::DBG, SBLog::GRAVDELAY, ClassName() + 
		       ": Celestial Body: " + Ephem->body(j).name());
	    Log->write(SBLog::DBG, SBLog::GRAVDELAY, ClassName() + 
		       ": Arg(TT)       : " + t1.toString());
	    Log->write(SBLog::DBG, SBLog::GRAVDELAY, ClassName() + 
		       ": t1j(TT)       : " + t1j.toString());
	    R1j.report2Log(SBLog::DBG, SBLog::GRAVDELAY, ClassName() + 
			   ": R<1-j>(CRS,km): ", 15, 3, 1.0e-3);
	    R2j.report2Log(SBLog::DBG, SBLog::GRAVDELAY, ClassName() + 
			   ": R<2-j>(CRS,km): ", 15, 3, 1.0e-3);
	    Log->write(SBLog::DBG, SBLog::GRAVDELAY, ClassName() + 
		       ": Grav. delay (ps)  : " + QString().sprintf("%16.8f", 1.0e12*GravDelJ));
	    Log->write(SBLog::DBG, SBLog::GRAVDELAY, ClassName() + 
		       ": Potential Uj      : " + QString().sprintf("%30.14f", Uj));
	  };
      };
  
  /*
    "
    4. Use equation 2 to find the differential delay due to the Earth.
    5. Sum to find the differential gravitational delay."
  */
  GravDelay += (GravDelJ=(1.0 + gamma())*Ephem->earth().gm()/cLight()/c2*
		log((x1.module() + K*x1)/(x2.module() + K*x2)));

  if (Log->isEligible(SBLog::DBG, SBLog::GRAVDELAY))
    {
      Log->write(SBLog::DBG, SBLog::GRAVDELAY, ClassName() + 
		 ": Celestial Body: " + Ephem->earth().name());
      Log->write(SBLog::DBG, SBLog::GRAVDELAY, ClassName() + 
		 ": Arg(TT)       : " + t1.toString());
      Log->write(SBLog::DBG, SBLog::GRAVDELAY, ClassName() + 
		 ": t1j(TT)       : " + t1j.toString());
      x1.report2Log(SBLog::DBG, SBLog::GRAVDELAY, ClassName() + 
		    ": x1    (CRS,km): ", 15, 3, 1.0e-3);
      x2.report2Log(SBLog::DBG, SBLog::GRAVDELAY, ClassName() + 
		    ": x2    (CRS,km): ", 15, 3, 1.0e-3);
      Log->write(SBLog::DBG, SBLog::GRAVDELAY, ClassName() + 
		 ": Grav. delay (ps)  : " + QString().sprintf("%16.8f", 1.0e12*GravDelJ));
      Log->write(SBLog::DBG, SBLog::GRAVDELAY, ClassName() + 
		 ": Potential Uj      : " + QString().sprintf("%30.14f", Uj));
    };
  
  /*
    "
    6. Add \Deltat_grav to the rest of the a priori vacuum delay from equation 9."
  */
  Ephem->calc(Frame->time().TDB());	// recalc ephems with proper time

  double	U   = U_;//Ephem->sun().gm()/(Ephem->earth().r() - Ephem->sun().r()).module()/c2;

  kbc = (K*b)/cLight();
  w1  = Frame->dQ_dt()*r1; // (dQ/dt)*r1
  w2  = Frame->dQ_dt()*r2; // (dQ/dt)*r2
  Ve  = Ephem->earth().v();

  DTau_Term_1 = 1.0 - (1.0 + gamma())*U - ((Ve*Ve)/2.0 + Ve*w2)/c2;
  DTau_Term_2 = (1.0 + (K*Ve)/2.0/cLight())/c2;
  DTau_Term_3 = 1.0 + (K*(Ve + w2))/cLight();
  
  VacuumDelay = (GravDelay - kbc*DTau_Term_1 - (Ve*b)*DTau_Term_2)/DTau_Term_3;
  
  /*
    "
    7. Calculate the abberated source vector for use in the calculation of the tropospheric
    propagation delay:"
  */
  k1 = K + (Ve + w1)/cLight() - K*((K*(Ve + w1))/cLight());
  k2 = K + (Ve + w2)/cLight() - K*((K*(Ve + w2))/cLight());
  k1.unify();
  k2.unify();
  
  /*
    "
    8. Add the geometric part of the tropospheric propagation delay to the vacuum delay, 
    equation 11."
  */

  double scale_for_height = exp(-0.03412476651981319179/(273.15+10.0)*St_1->height());
  bool   IsArtMeteo = St_1Info->isAttr(SBStationInfo::BadMeteo) || St_1Info->isAttr(SBStationInfo::ArtMeteo);
  DTAtm1 = (*Refr)(Obs->ambientT_1(IsArtMeteo), Obs->ambientP_1(IsArtMeteo, scale_for_height), Obs->ambientH_1(IsArtMeteo),
		   Frame, St_1, So, Frame->crs2trs()*k1, St_1Info->zw(), IsArtMeteo)/cLight();
  TotZenithDelay1 = Refr->totZenithDelay()/cLight();
  double Antenna_dL_1 = St_1->axisOffsetLenght((k1r = Refr->refrSource()), *Obs, 1.0/cLight());
  if (Mgr->prj()->cfg().p().zenith().type() != SBParameterCfg::PT_NONE)
    St_1->p_Zenith()->setD(((K*(w2 - w1))/cLight() - 1.0)*Refr->wetMap()/cLight(), *Obs);
  if (Mgr->prj()->cfg().p().atmGrad().type() != SBParameterCfg::PT_NONE)
    {
      St_1->p_AtmGradN()->setD( -cos(Refr->azimuth())*Refr->gradMap()/cLight(), *Obs);
      St_1->p_AtmGradE()->setD( -sin(Refr->azimuth())*Refr->gradMap()/cLight(), *Obs);
    };
  //--------------------------------
#ifdef ELEV_DEBUG_OUTPUT
  Refr->makeDebugReport(*Obs, St_1);
#endif
  // calculate derivatives:
  if (Mgr)  
    calcDerivatives(*Obs);


  // ! all derivatives should be called before this point !
  Frame->calc(*Obs - kbc/DAY2SEC);

  scale_for_height = exp(-0.03412476651981319179/(273.15+10.0)*St_2->height());
  IsArtMeteo       = St_2Info->isAttr(SBStationInfo::BadMeteo) || St_2Info->isAttr(SBStationInfo::ArtMeteo);
  DTAtm2 = (*Refr)(Obs->ambientT_2(IsArtMeteo), Obs->ambientP_2(IsArtMeteo, scale_for_height), Obs->ambientH_2(IsArtMeteo), 
		   Frame, St_2, So, Frame->crs2trs()*k2, St_2Info->zw(), IsArtMeteo)/cLight();
  TotZenithDelay2 = Refr->totZenithDelay()/cLight();
  double Antenna_dL_2 = St_2->axisOffsetLenght((k2r = Refr->refrSource()), *Obs, -1.0/cLight());
  if (Mgr->prj()->cfg().p().zenith().type() != SBParameterCfg::PT_NONE)
    St_2->p_Zenith()->setD(Refr->wetMap()/cLight(), *Obs);
  if (Mgr->prj()->cfg().p().atmGrad().type() != SBParameterCfg::PT_NONE)
    {
      St_2->p_AtmGradN()->setD(cos(Refr->azimuth())*Refr->gradMap()/cLight(), *Obs);
      St_2->p_AtmGradE()->setD(sin(Refr->azimuth())*Refr->gradMap()/cLight(), *Obs);
    };
  //--------------------------------
#ifdef ELEV_DEBUG_OUTPUT
  Refr->makeDebugReport(*Obs, St_2);
#endif

  AntennaDelay = (Antenna_dL_1 - Antenna_dL_2)/cLight();
  GeomDelay = VacuumDelay + AntennaDelay + DTAtm1*(K*(w2 - w1))/cLight();
  
  /*
    "
    9. The total delay can be found by adding the best estimate of the tropospheric 
    propagation delay"
  */
  TotalDelay = GeomDelay + (DTAtm2 - DTAtm1);
  

  // make a report:
  if (Log->isEligible(SBLog::DBG, SBLog::DELAYALL))
    {
      Log->write(SBLog::DBG, SBLog::DELAYALL, ClassName() + ": Arg(UTC)          : " + 
		 Obs->mjd().toString());

      Log->write(SBLog::DBG, SBLog::DELAYALL, ClassName() + ": Station #1        : " + 
		 St_1->name() + " [" + St_1->id().toString() + "]");
      Log->write(SBLog::DBG, SBLog::DELAYALL, ClassName() + ": Station #2        : " + 
		 St_2->name() + " [" + St_2->id().toString() + "]");
      Log->write(SBLog::DBG, SBLog::DELAYALL, ClassName() + ": Source            : " + So->name());


      r1.report2Log(SBLog::DBG, SBLog::DELAYALL, ClassName() + ": Station #1 @TRS   : ", 15,6);
      r2.report2Log(SBLog::DBG, SBLog::DELAYALL, ClassName() + ": Station #2 @TRS   : ", 15,6);
      (r2-r1).report2Log(SBLog::DBG, SBLog::DELAYALL, ClassName() + ": Baseline   @TRS   : ", 15,6);
      
      x1.report2Log(SBLog::DBG, SBLog::DELAYALL, ClassName() + ": Station #1 @CRS   : ", 15,6);
      x2.report2Log(SBLog::DBG, SBLog::DELAYALL, ClassName() + ": Station #2 @CRS   : ", 15,6);
      b .report2Log(SBLog::DBG, SBLog::DELAYALL, ClassName() + ": Baseline   @CRS   : ", 15,6);
      w1.report2Log(SBLog::DBG, SBLog::DELAYALL, ClassName() + ": Velocity of #1    : ", 15,6);
      w2.report2Log(SBLog::DBG, SBLog::DELAYALL, ClassName() + ": Velocity of #2    : ", 15,6);
      Ve.report2Log(SBLog::DBG, SBLog::DELAYALL, ClassName() + ": Velocity of Earth : ", 15,6);

      K .report2Log(SBLog::DBG, SBLog::DELAYALL, ClassName() + ": Source vector @CRS: ", 15,12);
      k1.report2Log(SBLog::DBG, SBLog::DELAYALL, ClassName() + ": Abberated Source@1: ", 15,12);
      k2.report2Log(SBLog::DBG, SBLog::DELAYALL, ClassName() + ": Abberated Source@2: ", 15,12);
      
      Log->write(SBLog::DBG, SBLog::DELAYALL, ClassName() +    ": Potential         : " + 
		 QString().sprintf("%26.10g", U));

      Log->write(SBLog::DBG, SBLog::DELAYALL, ClassName() + ": Grav. Delay   (ps): " + 
		 QString().sprintf("%24.8f", GravDelay*1.0e12));
      Log->write(SBLog::DBG, SBLog::DELAYALL, ClassName() + ": Anth. Delay   (ps): " + 
		 QString().sprintf("%24.8f", AntennaDelay*1.0e12));
      Log->write(SBLog::DBG, SBLog::DELAYALL, ClassName() + ": Atms. Delay   (ps): " + 
		 QString().sprintf("%24.8f", (DTAtm2 - DTAtm1)*1.0e12));

      Log->write(SBLog::DBG, SBLog::DELAYALL, ClassName() + ": Vacm. Delay   (ps): " + 
		 QString().sprintf("%24.8f", VacuumDelay*1.0e12));
      Log->write(SBLog::DBG, SBLog::DELAYALL, ClassName() + ": Geom. Delay   (ps): " + 
		 QString().sprintf("%24.8f", GeomDelay*1.0e12));
      
      Log->write(SBLog::DBG, SBLog::DELAYALL, ClassName() + ": Total Delay   (ps): " + 
		 QString().sprintf("%24.8f", TotalDelay*1.0e12));
      Log->write(SBLog::DBG, SBLog::DELAYALL, ClassName() + ": +++");
    };
  

  // --------------- tmps:
  /*=*/
  bool		IsTstOutput = TRUE;
  QString	StrTst;
  IsTstOutput = FALSE;

  if (IsTstOutput)
    {
      double	LamScale = Obs->refFreq()*1.0e6/CLight; // \nu/c

      Vector3	u(-K.at(Y_AXIS), K.at(X_AXIS), 0.0);
      Vector3	v(-K.at(X_AXIS)*K.at(Z_AXIS), -K.at(Y_AXIS)*K.at(Z_AXIS), 
		  K.at(X_AXIS)*K.at(X_AXIS) + K.at(Y_AXIS)*K.at(Y_AXIS));
      u.unify();
      v.unify();
      
      double	bv, bu/*, bw*/;

      bv = b*v*LamScale;
      bu = b*u*LamScale;
      //      bw = b*K*LamScale;

      //      double	PhiCorr;
      //      PhiCorr = 2.0*M_PI*Obs->refFreq()*1.0e6*TotalDelay;
      //      PhiCorr = 360.0*fmod(Obs->refFreq()*1.0e6*TotalDelay, 1.0);

      StrTst.sprintf("%16.9f %23.16e %23.16e  %12.10f %11.6f ", 
		     (double)(Obs->mjd()), bu, bv, Obs->correlation(), Obs->fringePhase()
		     );
      
      //      std::cout << StrTst << "\n";
      std::cout << StrTst;
    };
    /*=*/
  return TotalDelay;
};

void SBDelay::calcDerivatives(const SBMJD& t)
{

  // Stations:
  if (Mgr->prj()->cfg().p().staCoo().type() != SBParameterCfg::PT_NONE)
    {
      Vector3 dTau_dR = ((Frame->T()*K)/cLight()*DTau_Term_1 + (Frame->T()*Ve)*DTau_Term_2)/DTau_Term_3;

      St_1->p_DRX()->setD( dTau_dR.at(X_AXIS), t);
      St_1->p_DRY()->setD( dTau_dR.at(Y_AXIS), t);
      St_1->p_DRZ()->setD( dTau_dR.at(Z_AXIS), t);

      St_2->p_DRX()->setD(-dTau_dR.at(X_AXIS), t);
      St_2->p_DRY()->setD(-dTau_dR.at(Y_AXIS), t);
      St_2->p_DRZ()->setD(-dTau_dR.at(Z_AXIS), t);
    };

  if (Mgr->prj()->cfg().p().staVel().type() != SBParameterCfg::PT_NONE)
    {
      //      Vector3 dTau_dV = (Frame->T()*K)/cLight()*(t - Mgr->trf()->epoch())/365.25; // meter/year
      Vector3 dTau_dV = ((Frame->T()*K)/cLight()*DTau_Term_1 + (Frame->T()*Ve)*DTau_Term_2)/DTau_Term_3
	*(t - Mgr->trf()->epoch())/365.25; // meter/year;

      St_1->p_DVX()->setD( dTau_dV.at(X_AXIS), t);
      St_1->p_DVY()->setD( dTau_dV.at(Y_AXIS), t);
      St_1->p_DVZ()->setD( dTau_dV.at(Z_AXIS), t);
      
      St_2->p_DVX()->setD(-dTau_dV.at(X_AXIS), t);
      St_2->p_DVY()->setD(-dTau_dV.at(Y_AXIS), t);
      St_2->p_DVZ()->setD(-dTau_dV.at(Z_AXIS), t);
    };
 
  if (Mgr->prj()->cfg().p().love_h2().type() != SBParameterCfg::PT_NONE)
    {
      Vector3 dTau_dR = ((Frame->T()*K)/cLight()*DTau_Term_1 + (Frame->T()*Ve)*DTau_Term_2)/DTau_Term_3;
      St_1->p_TideH2()->setD( dTau_dR*Tide->solidTideLd()->dTide_dH2(St_1, Ephem), t);
      St_2->p_TideH2()->setD(-dTau_dR*Tide->solidTideLd()->dTide_dH2(St_2, Ephem), t);
    };
  if (Mgr->prj()->cfg().p().love_l2().type() != SBParameterCfg::PT_NONE)
    {
      Vector3 dTau_dR = ((Frame->T()*K)/cLight()*DTau_Term_1 + (Frame->T()*Ve)*DTau_Term_2)/DTau_Term_3;
      St_1->p_TideL2()->setD( dTau_dR*Tide->solidTideLd()->dTide_dL2(St_1, Ephem), t);
      St_2->p_TideL2()->setD(-dTau_dR*Tide->solidTideLd()->dTide_dL2(St_2, Ephem), t);
    };
  if (Mgr->prj()->cfg().p().tideLag().type() != SBParameterCfg::PT_NONE)
    {
      Vector3 dTau_dR = ((Frame->T()*K)/cLight()*DTau_Term_1 + (Frame->T()*Ve)*DTau_Term_2)/DTau_Term_3;
      St_1->p_TideLag()->setD( dTau_dR*Tide->solidTideLd()->dTide_dLag(St_1, Ephem), t);
      St_2->p_TideLag()->setD(-dTau_dR*Tide->solidTideLd()->dTide_dLag(St_2, Ephem), t);
    };
  //
  // --tests
  /*
    Vector3	dR_dAplo;
    dR_dAplo = St_1->fmVEN()*Mgr->aploEphem()->displacement(St_1->id(), t);
    St_1->p_Aplo()->setD( dTau_dR*dR_dAplo, t);
    dR_dAplo = St_2->fmVEN()*Mgr->aploEphem()->displacement(St_2->id(), t);
    St_2->p_Aplo()->setD(-dTau_dR*dR_dAplo, t);
    };
    -- */
  // --tests
  
  
  // Sources:
  if (Mgr->prj()->cfg().p().srcCoo().type() != SBParameterCfg::PT_NONE)
    {
      Vector3 Tmp1(-sin(So->ra_t())*cos(So->dn_t()), cos(So->ra_t())*cos(So->dn_t()),             0.0);
      Vector3 Tmp2(-cos(So->ra_t())*sin(So->dn_t()),-sin(So->ra_t())*sin(So->dn_t()), cos(So->dn_t()));
      
      //      So->p_RA()->setD( -b*Tmp1/cLight(), t);
      //      So->p_DN()->setD( -b*Tmp2/cLight(), t);
      So->p_RA()->setD( 
		       ( -(Tmp1*b)/cLight()*DTau_Term_1*DTau_Term_3 + kbc*DTau_Term_1/cLight()*(Tmp1*(Ve+w2)) )
		       /DTau_Term_3/DTau_Term_3, t);
      So->p_DN()->setD(
		       ( -(Tmp2*b)/cLight()*DTau_Term_1*DTau_Term_3 + kbc*DTau_Term_1/cLight()*(Tmp2*(Ve+w2)) )
		       /DTau_Term_3/DTau_Term_3, t);
    };

  // EOPs:
  if (Mgr->prj()->cfg().p().polusUT().type() != SBParameterCfg::PT_NONE)
    {
      double dTau_dUT = - (K*(Frame->dQ_dUT()*b_TRS))/cLight();
      Frame->eop()->p_Put()->setD(dTau_dUT*DTau_Term_1/DTau_Term_3, t);
    };

  if (Mgr->prj()->cfg().p().polusXY().type() != SBParameterCfg::PT_NONE)
    {
      double dTau_dx = - (K*(Frame->dQ_dxp()*b_TRS))/cLight();
      double dTau_dy = - (K*(Frame->dQ_dyp()*b_TRS))/cLight();

      Frame->eop()->p_Px()->setD(dTau_dx*DTau_Term_1/DTau_Term_3, t);
      Frame->eop()->p_Py()->setD(dTau_dy*DTau_Term_1/DTau_Term_3, t);
    };

  if (Mgr->prj()->cfg().p().polusNut().type() != SBParameterCfg::PT_NONE)
    {
      double dTau_dPsi = - (K*(Frame->dQ_dpsi()*b_TRS))/cLight();
      double dTau_dEps = - (K*(Frame->dQ_deps()*b_TRS))/cLight();

      Frame->eop()->p_Npsi()->setD(dTau_dPsi*DTau_Term_1/DTau_Term_3, t);
      Frame->eop()->p_Neps()->setD(dTau_dEps*DTau_Term_1/DTau_Term_3, t);
    };

  //tmps:
  if (Mgr->prj()->cfg().p().test().type() != SBParameterCfg::PT_NONE)
    {
      // S1: T = 0.9999998692 (d)
      double Phi = Frame->time().omega();

      double dTau_dx = - (K*(Frame->dQ_dxp()*b_TRS))/cLight();
      double dTau_dy = - (K*(Frame->dQ_dyp()*b_TRS))/cLight();

      Frame->eop()->p_Xp_m2()->setD((dTau_dx*cos(-2.0*Phi) - dTau_dy*sin(-2.0*Phi))*DTau_Term_1/DTau_Term_3, t);
      Frame->eop()->p_Yp_m2()->setD((dTau_dx*sin(-2.0*Phi) + dTau_dy*cos(-2.0*Phi))*DTau_Term_1/DTau_Term_3, t);

      Frame->eop()->p_Xp_p1()->setD((dTau_dx*cos(     Phi) - dTau_dy*sin(     Phi))*DTau_Term_1/DTau_Term_3, t);
      Frame->eop()->p_Yp_p1()->setD((dTau_dx*sin(     Phi) + dTau_dy*cos(     Phi))*DTau_Term_1/DTau_Term_3, t);
      Frame->eop()->p_Xp_p2()->setD((dTau_dx*cos( 2.0*Phi) - dTau_dy*sin( 2.0*Phi))*DTau_Term_1/DTau_Term_3, t);
      Frame->eop()->p_Yp_p2()->setD((dTau_dx*sin( 2.0*Phi) + dTau_dy*cos( 2.0*Phi))*DTau_Term_1/DTau_Term_3, t);


      Frame->eop()->p_Xp_m3()->setD((dTau_dx*cos(-3.0*Phi) - dTau_dy*sin(-3.0*Phi))*DTau_Term_1/DTau_Term_3, t);
      Frame->eop()->p_Yp_m3()->setD((dTau_dx*sin(-3.0*Phi) + dTau_dy*cos(-3.0*Phi))*DTau_Term_1/DTau_Term_3, t);
      Frame->eop()->p_Xp_p3()->setD((dTau_dx*cos( 3.0*Phi) - dTau_dy*sin( 3.0*Phi))*DTau_Term_1/DTau_Term_3, t);
      Frame->eop()->p_Yp_p3()->setD((dTau_dx*sin( 3.0*Phi) + dTau_dy*cos( 3.0*Phi))*DTau_Term_1/DTau_Term_3, t);


      Frame->eop()->p_Xp_m4()->setD((dTau_dx*cos(-4.0*Phi) - dTau_dy*sin(-4.0*Phi))*DTau_Term_1/DTau_Term_3, t);
      Frame->eop()->p_Yp_m4()->setD((dTau_dx*sin(-4.0*Phi) + dTau_dy*cos(-4.0*Phi))*DTau_Term_1/DTau_Term_3, t);
      Frame->eop()->p_Xp_p4()->setD((dTau_dx*cos( 4.0*Phi) - dTau_dy*sin( 4.0*Phi))*DTau_Term_1/DTau_Term_3, t);
      Frame->eop()->p_Yp_p4()->setD((dTau_dx*sin( 4.0*Phi) + dTau_dy*cos( 4.0*Phi))*DTau_Term_1/DTau_Term_3, t);


      double dTau_dUT = - (K*(Frame->dQ_dUT()*b_TRS))/cLight();
      Frame->eop()->p_UT_c1()->setD(dTau_dUT*cos(    Phi)*DTau_Term_1/DTau_Term_3, t);
      Frame->eop()->p_UT_s1()->setD(dTau_dUT*sin(    Phi)*DTau_Term_1/DTau_Term_3, t);
      Frame->eop()->p_UT_c2()->setD(dTau_dUT*cos(2.0*Phi)*DTau_Term_1/DTau_Term_3, t);
      Frame->eop()->p_UT_s2()->setD(dTau_dUT*sin(2.0*Phi)*DTau_Term_1/DTau_Term_3, t);

      Frame->eop()->p_UT_c3()->setD(dTau_dUT*cos(3.0*Phi)*DTau_Term_1/DTau_Term_3, t);
      Frame->eop()->p_UT_s3()->setD(dTau_dUT*sin(3.0*Phi)*DTau_Term_1/DTau_Term_3, t);
      Frame->eop()->p_UT_c4()->setD(dTau_dUT*cos(4.0*Phi)*DTau_Term_1/DTau_Term_3, t);
      Frame->eop()->p_UT_s4()->setD(dTau_dUT*sin(4.0*Phi)*DTau_Term_1/DTau_Term_3, t);


      // 4test:
      Frame->eop()->p_Xp_m5()->setD((dTau_dx*cos(-5.0*Phi) - dTau_dy*sin(-5.0*Phi))*DTau_Term_1/DTau_Term_3, t);
      Frame->eop()->p_Yp_m5()->setD((dTau_dx*sin(-5.0*Phi) + dTau_dy*cos(-5.0*Phi))*DTau_Term_1/DTau_Term_3, t);
      Frame->eop()->p_Xp_p5()->setD((dTau_dx*cos( 5.0*Phi) - dTau_dy*sin( 5.0*Phi))*DTau_Term_1/DTau_Term_3, t);
      Frame->eop()->p_Yp_p5()->setD((dTau_dx*sin( 5.0*Phi) + dTau_dy*cos( 5.0*Phi))*DTau_Term_1/DTau_Term_3, t);
      Frame->eop()->p_UT_c5()->setD(dTau_dUT*cos( 5.0*Phi)*DTau_Term_1/DTau_Term_3, t);
      Frame->eop()->p_UT_s5()->setD(dTau_dUT*sin( 5.0*Phi)*DTau_Term_1/DTau_Term_3, t);

      Frame->eop()->p_Xp_m6()->setD((dTau_dx*cos(-6.0*Phi) - dTau_dy*sin(-6.0*Phi))*DTau_Term_1/DTau_Term_3, t);
      Frame->eop()->p_Yp_m6()->setD((dTau_dx*sin(-6.0*Phi) + dTau_dy*cos(-6.0*Phi))*DTau_Term_1/DTau_Term_3, t);
      Frame->eop()->p_Xp_p6()->setD((dTau_dx*cos( 6.0*Phi) - dTau_dy*sin( 6.0*Phi))*DTau_Term_1/DTau_Term_3, t);
      Frame->eop()->p_Yp_p6()->setD((dTau_dx*sin( 6.0*Phi) + dTau_dy*cos( 6.0*Phi))*DTau_Term_1/DTau_Term_3, t);
      Frame->eop()->p_UT_c6()->setD(dTau_dUT*cos( 6.0*Phi)*DTau_Term_1/DTau_Term_3, t);
      Frame->eop()->p_UT_s6()->setD(dTau_dUT*sin( 6.0*Phi)*DTau_Term_1/DTau_Term_3, t);

      Frame->eop()->p_Xp_m7()->setD((dTau_dx*cos(-7.0*Phi) - dTau_dy*sin(-7.0*Phi))*DTau_Term_1/DTau_Term_3, t);
      Frame->eop()->p_Yp_m7()->setD((dTau_dx*sin(-7.0*Phi) + dTau_dy*cos(-7.0*Phi))*DTau_Term_1/DTau_Term_3, t);
      Frame->eop()->p_Xp_p7()->setD((dTau_dx*cos( 7.0*Phi) - dTau_dy*sin( 7.0*Phi))*DTau_Term_1/DTau_Term_3, t);
      Frame->eop()->p_Yp_p7()->setD((dTau_dx*sin( 7.0*Phi) + dTau_dy*cos( 7.0*Phi))*DTau_Term_1/DTau_Term_3, t);
      Frame->eop()->p_UT_c7()->setD(dTau_dUT*cos( 7.0*Phi)*DTau_Term_1/DTau_Term_3, t);
      Frame->eop()->p_UT_s7()->setD(dTau_dUT*sin( 7.0*Phi)*DTau_Term_1/DTau_Term_3, t);

      Frame->eop()->p_Xp_m8()->setD((dTau_dx*cos(-8.0*Phi) - dTau_dy*sin(-8.0*Phi))*DTau_Term_1/DTau_Term_3, t);
      Frame->eop()->p_Yp_m8()->setD((dTau_dx*sin(-8.0*Phi) + dTau_dy*cos(-8.0*Phi))*DTau_Term_1/DTau_Term_3, t);
      Frame->eop()->p_Xp_p8()->setD((dTau_dx*cos( 8.0*Phi) - dTau_dy*sin( 8.0*Phi))*DTau_Term_1/DTau_Term_3, t);
      Frame->eop()->p_Yp_p8()->setD((dTau_dx*sin( 8.0*Phi) + dTau_dy*cos( 8.0*Phi))*DTau_Term_1/DTau_Term_3, t);
      Frame->eop()->p_UT_c8()->setD(dTau_dUT*cos( 8.0*Phi)*DTau_Term_1/DTau_Term_3, t);
      Frame->eop()->p_UT_s8()->setD(dTau_dUT*sin( 8.0*Phi)*DTau_Term_1/DTau_Term_3, t);

      Frame->eop()->p_Xp_m9()->setD((dTau_dx*cos(-9.0*Phi) - dTau_dy*sin(-9.0*Phi))*DTau_Term_1/DTau_Term_3, t);
      Frame->eop()->p_Yp_m9()->setD((dTau_dx*sin(-9.0*Phi) + dTau_dy*cos(-9.0*Phi))*DTau_Term_1/DTau_Term_3, t);
      Frame->eop()->p_Xp_p9()->setD((dTau_dx*cos( 9.0*Phi) - dTau_dy*sin( 9.0*Phi))*DTau_Term_1/DTau_Term_3, t);
      Frame->eop()->p_Yp_p9()->setD((dTau_dx*sin( 9.0*Phi) + dTau_dy*cos( 9.0*Phi))*DTau_Term_1/DTau_Term_3, t);
      Frame->eop()->p_UT_c9()->setD(dTau_dUT*cos( 9.0*Phi)*DTau_Term_1/DTau_Term_3, t);
      Frame->eop()->p_UT_s9()->setD(dTau_dUT*sin( 9.0*Phi)*DTau_Term_1/DTau_Term_3, t);

      Frame->eop()->p_Xp_m10()->setD((dTau_dx*cos(-10.0*Phi) - dTau_dy*sin(-10.0*Phi))*DTau_Term_1/DTau_Term_3, t);
      Frame->eop()->p_Yp_m10()->setD((dTau_dx*sin(-10.0*Phi) + dTau_dy*cos(-10.0*Phi))*DTau_Term_1/DTau_Term_3, t);
      Frame->eop()->p_Xp_p10()->setD((dTau_dx*cos( 10.0*Phi) - dTau_dy*sin( 10.0*Phi))*DTau_Term_1/DTau_Term_3, t);
      Frame->eop()->p_Yp_p10()->setD((dTau_dx*sin( 10.0*Phi) + dTau_dy*cos( 10.0*Phi))*DTau_Term_1/DTau_Term_3, t);
      Frame->eop()->p_UT_c10()->setD(dTau_dUT*cos( 10.0*Phi)*DTau_Term_1/DTau_Term_3, t);
      Frame->eop()->p_UT_s10()->setD(dTau_dUT*sin( 10.0*Phi)*DTau_Term_1/DTau_Term_3, t);
    };
};
/*==============================================================================================*/


