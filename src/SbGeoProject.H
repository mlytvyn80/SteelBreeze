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
 * Overview:	describes models' configuration (SBConfig);
 *			  project - a config and a set of sessions (SBProject);
 *			  task manager (SBRunManager);
 *
 */


#ifndef SB_GEO_PROJECT_H
#define SB_GEO_PROJECT_H

#include <config.h>
#include <stdlib.h>

#include <qdatastream.h>
#include <qlist.h>
#include <qstring.h>
#include <qtextstream.h>

#include "SbGeoObs.H"
#include "SbGeoObsVLBI.H"
#include "SbGeoParameter.H"



class SBConfig;
extern const SBConfig DefaultConfig;
extern SBConfig Config;
class SB_CRF;
class SB_TRF;


class SBParametersDesc;
/*==============================================================================================*/
/**Runtime configuration of the models.
 *
 *
 */
/*==============================================================================================*/
class SBConfig
{
public:
  enum MSysTransform		{STR_Classic, STR_NRO};
  enum MSolidTideLd		{ST_IERS92, ST_IERS96};
  enum MSolidTideDegree		{STDEGREE_2, STDEGREE_3};
  enum MEOPDiurnal		{EDM_NONE, EDM_Ray1995, EDM_JPL92, EDM_HD1994, EDM_GSFC95, EDM_Gipson1995};
  enum MEOPDiurnalAtm		{EDMA_NONE, EDMA_BBP};
  enum MEOPDiurnalNonTidal	{EDMNT_NONE, EDMNT_Viron};
  enum MNutModel		{NM_IAU1980, NM_IAU1996, NM_IAU2000};
  enum MNutEqEquinox		{NEE_IERS92, NEE_SOVERS, NEE_IERS96};
  enum MEphModel		{EM_LEDE200, EM_LEDE403, EM_LEDE405};
  enum MPlateMotion		{PM_NNR_NUVEL1A, PM_NUVEL1A, PM_NONE};
  enum MRefrDryZenith		{RDZ_NONE, RDZ_Saastamoinen, RDZ_Hopfield};
  enum MRefrWetZenith		{RWZ_NONE, RWZ_Saastamoinen, RWZ_Hopfield};
  enum MRefrDryMapping		{RDM_NONE, RDM_Chao, RDM_CfA2p2, RDM_Ifadis, RDM_MTT, RDM_NMFH2};
  enum MRefrWetMapping		{RWM_NONE, RWM_Chao,             RWM_Ifadis, RWM_MTT, RDM_NMFW2};

private:
  bool			IsChanged;
  // Parameters:
  SBParametersDesc	P;

  //System transformation:
  MSysTransform		SysTransform;		//!< Type of CRF<->TRF system transformation

  //Light propagation/Ephemerides:
  MEphModel		EphemModel;		//!< Ephemerides model
  bool			IsEphBody[11];		//!< Celestial bodies taking into account
  bool			IsGravDelayHO;		//!< High order effects
  double		GravHOThreshold;	//!< Threshold for high order effects, rad

  //Solid Tide Loading:
  MSolidTideLd		SolidTideLd;		//!< Site displacement load model due to solid tide
  MSolidTideDegree	SolidTideDegree;	//!< Maximum degree of solid tide loading model
  bool			IsSolidTideAnelastic;	//!< Anelastic or elastic model
  bool			IsSolidTideLatDepend;	//!< Include Solid Tide Latitude dependence
  bool			IsRemovePermTide;	//!< Remove permanent tide

  //EOP:
  MEOPDiurnal		EOPDiurnal;		//!< Take into account diurnal variations in EOP
  MEOPDiurnalAtm	EOPDiurnalAtm;		//!< Take into account diurnal variations in EOP (atmosphere)
  MEOPDiurnalNonTidal	EOPDiurnalNonTidal;	//!< Take into account diurnal variations in EOP (non-tidal)
  int			EOPNumIntrplPoints;	//!< Number of points for interpolation
  bool			IsEOP_IAU2K_XY;		//!< Nutations in EOP are dX and dY w/ respect to IAU2000
  bool			IsEOPResrerved_1;	//!< Reserved for use with EOP
  
  //Nutation:
  MNutModel		NutModel;		//!< Nutation theory model
  MNutEqEquinox		NutEquinox;		//!< Model of the Equation of equinoxes
  bool			IsNutGeodNut;		//!< Take into account geodesic nutation
  
  //Station motion:
  MPlateMotion		PlateMotion;		//!< Tectonic plate motion model
  bool			IsUseStationVelocity;	//!< Use velocity of a station from a catalogue
  bool			IsRollBack2Plates;	//!< Use plate motion vel. if V=0
  bool			IsUseAploEphem;		//!< Use atmoshperic loading ephemerides

  //Refraction:
  MRefrDryZenith	RefrDryZenith;		//!< Hydrostatic zenith delay model;
  MRefrWetZenith	RefrWetZenith;		//!< Wet zenith delay model;
  MRefrDryMapping	RefrDryMapping;		//!< Hydrostatic mapping function model;
  MRefrWetMapping	RefrWetMapping;		//!< Wet mapping function model;
  bool			IsAltAxsCorr;		//!< Take into axis offset altitude correction;
  bool			IsRollBack2NMF;		//!< Use NMFs in the case of bad/art metheo;

  //Data processing:
  int			QCodeThreshold;
  int			DUFlagThreshold;
  bool			IsUseDelay;
  bool			IsUseRate;
  bool			IsUseBadIon;

  //Solution reports:
  bool			ReportMaps;
  bool			ReportCRF_Changes;
  bool			ReportTRF_Changes;
  bool			ReportCRF;
  bool			ReportTRF_SINEX;
  bool			ReportEOP;
  bool			ReportTechDesc;
  bool			ReportNormalEqs;	//!< Make report of NEQ in SINEX format;
  bool			ReportNormalEqsSRCs;	//!< Include sources in NEQ report;
  bool			ReportNormalEqsSTCs;	//!< Include stations in NEQ report;
  bool			ReportNormalEqsEOPs;	//!< Include EOP in NEQ report;
  bool			ReportTroposhere;
  double		TropSamplInterval;
  int			TropStartSec;
  
  //Alternative DB:
  bool			UseAltStations;
  bool			UseAltSources;
  bool			UseAltEOPs;
  QString		AltStations;
  QString		AltSources;
  QString		AltEOPs;

  //Run-Time options:
  bool			IsGenerateDSINEXes;

  //--

public:
  /** A constructor. */
  SBConfig();
  /** A constructor. */
  SBConfig(const SBConfig& Cfg_) : P() {*this=Cfg_;};
  /** A destructor. */
  virtual ~SBConfig(){};
  SBConfig& operator= (const SBConfig&);
  /** Refers to a class name (debug info) */
  virtual QString ClassName() const {return "SBConfig";};
  
  //--------
  //System Transformation:
  MSysTransform		sysTransform() const {return SysTransform;}
  void			setSysTransform(MSysTransform E_) {SysTransform = E_;};

  //Light propagation/Ephemerides:
  MEphModel		ephemModel() const {return EphemModel;};
  bool			isEphBody(int i) const {return 0<=i&&i<11?IsEphBody[i]:FALSE;};
  bool			isGravDelayHO() const {return IsGravDelayHO;};
  double		gravHOThreshold() const {return GravHOThreshold;};
  void			setEphemModel(MEphModel E_) {EphemModel = E_;};
  void			setIsEphBody(int i, bool Is_) {if (0<=i&&i<11) IsEphBody[i]=Is_;};
  void			setIsGravDelayHO(bool Is_) {IsGravDelayHO=Is_;};
  void			setGravHOThreshold(double d_) {GravHOThreshold=d_;};
  
  //Solid Tide Loading:
  MSolidTideLd		solidTideLd() const {return SolidTideLd;};
  MSolidTideDegree	solidTideDegree() const {return SolidTideDegree;};
  bool			isSolidTideAnelastic() const {return IsSolidTideAnelastic;};
  bool			isSolidTideLatDepend() const {return IsSolidTideLatDepend;};
  bool			isRemovePermTide() const {return IsRemovePermTide;};
  void			setSolidTideLd(MSolidTideLd S_) {SolidTideLd=S_;};
  void			setSolidTideDegree(MSolidTideDegree D_) {SolidTideDegree=D_;};
  void			setIsSolidTideAnelastic(bool Is_) {IsSolidTideAnelastic=Is_;};
  void			setIsSolidTideLatDepend(bool Is_) {IsSolidTideLatDepend=Is_;};
  void			setIsRemovePermTide(bool Is_) {IsRemovePermTide=Is_;};
  
  //EOP:
  MEOPDiurnal		eopDiurnal() const {return EOPDiurnal;};
  MEOPDiurnalAtm	eopDiurnalAtm() const {return EOPDiurnalAtm;};
  MEOPDiurnalNonTidal	eopDiurnalNonTidal() const {return EOPDiurnalNonTidal;};
  int			eopNumIntrplPoints() const {return EOPNumIntrplPoints;};
  bool			isEOP_IAU2K_XY() const {return IsEOP_IAU2K_XY;};
  bool			isEOPResrerved_1() const {return IsEOPResrerved_1;};
  void			setEopDiurnal(MEOPDiurnal E_) {EOPDiurnal=E_;};
  void			setEopDiurnalAtm(MEOPDiurnalAtm E_) {EOPDiurnalAtm=E_;};
  void			setEopDiurnalNonTidal(MEOPDiurnalNonTidal E_) {EOPDiurnalNonTidal=E_;};
  void			setEOPNumIntrplPoints(int N_) {EOPNumIntrplPoints = N_;};
  void			setIsEOP_IAU2K_XY(bool Is_)   {IsEOP_IAU2K_XY = Is_;};
  void			setIsEOPResrerved_1(bool Is_) {IsEOPResrerved_1 = Is_;};
  
  //Nutation:
  MNutModel		nutModel() const {return NutModel;};
  MNutEqEquinox		nutEquinox() const {return NutEquinox;};
  bool			isNeedGeodNut() const {return IsNutGeodNut;};
  void			setNutModel(MNutModel M_) {NutModel=M_;};
  void			setNutEquinox(MNutEqEquinox E_) {NutEquinox=E_;};
  void			setIsNeedGeodNut(bool Is_) {IsNutGeodNut=Is_;};

  //Station motion:
  MPlateMotion		plateMotion() const {return PlateMotion;};
  void			setPlateMotion(MPlateMotion P_) {PlateMotion=P_;};
  bool			isUseStationVelocity() const {return IsUseStationVelocity;};
  bool			isRollBack2Plates() const {return IsRollBack2Plates;};
  bool			isUseAploEphem() const {return IsUseAploEphem;};
  void			setIsUseStationVelocity(bool Is_) {IsUseStationVelocity = Is_;};
  void			setIsRollBack2Plates(bool Is_) {IsRollBack2Plates = Is_;};
  void			setIsUseAploEphem(bool Is_) {IsUseAploEphem = Is_;};
  
  //Refraction:
  MRefrDryZenith	refrDryZenith() const {return RefrDryZenith;};
  MRefrWetZenith	refrWetZenith() const {return RefrWetZenith;};
  MRefrDryMapping	refrDryMapping()const {return RefrDryMapping;};
  MRefrWetMapping	refrWetMapping()const {return RefrWetMapping;};
  bool			isAltAxsCorr()	const {return IsAltAxsCorr;};
  bool			isRollBack2NMF()const {return IsRollBack2NMF;};
  void			setRefrDryZenith(MRefrDryZenith DZ_)  {RefrDryZenith=DZ_;};
  void			setRefrWetZenith(MRefrWetZenith WZ_)  {RefrWetZenith=WZ_;};
  void			setRefrDryMapping(MRefrDryMapping DM_) {RefrDryMapping=DM_;};
  void			setRefrWetMapping(MRefrWetMapping WM_) {RefrWetMapping=WM_;};
  void			setIsAltAxsCorr(bool Is_) {IsAltAxsCorr=Is_;};
  void			setIsRollBack2NMF(bool Is_) {IsRollBack2NMF=Is_;};
  
  //Data processing:
  int			qCodeThreshold() const {return QCodeThreshold;};
  int			dUFlagThreshold() const {return DUFlagThreshold;};
  bool			isUseDelay () const {return IsUseDelay;};
  bool			isUseRate  () const {return IsUseRate;};
  bool			isUseBadIon() const {return IsUseBadIon;};
  void			setQCodeThreshold(int Threshold_)  {QCodeThreshold = Threshold_;};
  void			setDUFlagThreshold(int Threshold_) {DUFlagThreshold= Threshold_;};
  void			setIsUseDelay (bool Is_) {IsUseDelay = Is_;};
  void			setIsUseRate  (bool Is_) {IsUseRate  = Is_;};
  void			setIsUseBadIon(bool Is_) {IsUseBadIon= Is_;};

  //Solution reports:
  bool			reportMaps()		const {return ReportMaps;};
  bool			reportCRF_Changes()	const {return ReportCRF_Changes;};
  bool			reportTRF_Changes()	const {return ReportTRF_Changes;};
  bool			reportCRF()		const {return ReportCRF;};
  bool			reportTRF_SINEX()	const {return ReportTRF_SINEX;};
  bool			reportEOP()		const {return ReportEOP;};
  bool			reportTechDesc()	const {return ReportTechDesc;};
  bool			reportNormalEqs()	const {return ReportNormalEqs;};
  bool			reportNormalEqsSRCs()	const {return ReportNormalEqsSRCs;};
  bool			reportNormalEqsSTCs()	const {return ReportNormalEqsSTCs;};
  bool			reportNormalEqsEOPs()	const {return ReportNormalEqsEOPs;};
  bool			reportTroposhere()	const {return ReportTroposhere;};
  double		tropSamplInterval()	const {return TropSamplInterval;};
  int			tropStartSec()		const {return TropStartSec;};
  void			setReportMaps(bool Is_)		{ReportMaps = Is_;};
  void			setReportCRF_Changes(bool Is_)	{ReportCRF_Changes = Is_;};
  void			setReportTRF_Changes(bool Is_)	{ReportTRF_Changes = Is_;};
  void			setReportCRF(bool Is_)		{ReportCRF = Is_;};
  void			setReportTRF_SINEX(bool Is_)	{ReportTRF_SINEX = Is_;};
  void			setReportEOP(bool Is_)		{ReportEOP = Is_;};
  void			setReportTechDesc(bool Is_)	{ReportTechDesc = Is_;};
  void			setReportNormalEqs(bool Is_)	{ReportNormalEqs = Is_;};
  void			setReportNormalEqsSRCs(bool Is_){ReportNormalEqsSRCs = Is_;};
  void			setReportNormalEqsSTCs(bool Is_){ReportNormalEqsSTCs = Is_;};
  void			setReportNormalEqsEOPs(bool Is_){ReportNormalEqsEOPs = Is_;};
  void			setReportTroposhere(bool Is_)	{ReportTroposhere = Is_;};
  void			setTropSamplInterval(double d_)	{TropSamplInterval = d_;};
  void			setTropStartSec(int n_)		{TropStartSec= n_;};

  //Alternative DB:
  bool			useAltStations()	const {return UseAltStations;};
  bool			useAltSources() 	const {return UseAltSources;};
  bool			useAltEOPs()		const {return UseAltEOPs;};
  const QString&	altStations()		const {return AltStations;};
  const QString&	altSources()		const {return AltSources;};
  const QString&	altEOPs()		const {return AltEOPs;};
  void			setUseAltStations(bool Is_)	{UseAltStations = Is_;};
  void			setUseAltSources(bool Is_)	{UseAltSources  = Is_;};
  void			setUseAltEOPs(bool Is_)		{UseAltEOPs     = Is_;};
  void			setAltStations(const QString& S_)  {AltStations = S_.copy();};
  void			setAltSources (const QString& S_)  {AltSources  = S_.copy();};
  void			setAltEOPs    (const QString& S_)  {AltEOPs     = S_.copy();};

  //Run-Time options:
  bool			isGenerateDSINEXes() const {return IsGenerateDSINEXes;};
  void			setIsGenerateDSINEXes(bool Is_) {IsGenerateDSINEXes = Is_;};

  //
  bool			isChanged() const {return IsChanged;};
  void			setIsChanged(bool Is_=TRUE){IsChanged = Is_;};
  const SBParametersDesc&	p() const {return P;};
  void			setP(const SBParametersDesc& P_){P=P_;};
  void			setAllPars2Type(SBParameterCfg::PType);

  /* I/O: */
  friend QDataStream &operator<<(QDataStream&, const SBConfig&);
  friend QDataStream &operator>>(QDataStream&, SBConfig&);
};
/*==============================================================================================*/



/*==============================================================================================*/
/**Project.
 *
 *
 */
/*==============================================================================================*/
class SBProject : public SBNamed
{
protected:
  SBConfig			Cfg;		//!< project's config
  int				Version;	//!< project's version
  SBHistory			History;	//!< project's history
  QString			Comments;	//!< some comments
  
  SBVLBISesInfoList		VLBISessionList;//!< list of VLBI sessions
  QDict<SBVLBISesInfo>		*SessionByName;

  SBStationInfoList		StationList;	//!< list of stations
  QDict<SBStationInfo>		*StationByAka;

  SBSourceInfoList		SourceList;	//!< list of sources
  QDict<SBSourceInfo>		*SourceByAka;

  SBBaseInfoList		BaseList;	//!< list of bases
  QDict<SBBaseInfo>		*BaseByAka;
  int				NumObs;
  
  void fillDicts() {fillSessionDict(); fillStationDict(); fillSourceDict(); fillBaseDict();};
  void fillSessionDict(int=5);
  void fillStationDict(int=5);
  void fillSourceDict(int=5);
  void fillBaseDict(int=5);

public:
  // constructors/destructors:
  //
  /**A constructor. */
  SBProject(const QString&);
  /**A destructor. */
  virtual ~SBProject();
  /**Refers to a class name (debug info).*/
  virtual QString ClassName() const {return "SBProject";};
  
  // interface:
  //
  //  const SBConfig&	cfg()	  const {return Cfg;};
  SBConfig&		cfg()  {return Cfg;};
  const QString&	comments()const {return Comments;};
  int			version() const {return Version;};
  SBHistory*		history()	{return &History;};
  
  void			setCfg(const SBConfig& Cfg_){Cfg=Cfg_;};
  void			incVersion(){Version++;};
  void			addHistoryLine(const QString& Txt_)
    {History.addHistoryLine(Version, Txt_);};
  bool			delHistoryLine(const SBHistory::SBHistoryEntry* E_)
    {return History.remove(E_);};
  void			setComments(const QString& Comments_){Comments=Comments_.copy();};
  const SBParametersDesc&	p() const {return Cfg.p();};
  void			setP(const SBParametersDesc& P_) {Cfg.setP(P_);};
  void			setAllPars2Type(SBParameterCfg::PType Type){Cfg.setAllPars2Type(Type);};
  
  //lists:
  /**Returns list of the VLBI sessions;*/
  SBVLBISesInfoList*	VLBIsessionList() {return &VLBISessionList;};
  /**Returns list of the stations;*/
  SBStationInfoList*	stationList() {return &StationList;};
  /**Returns list of the sources;*/
  SBSourceInfoList*	sourceList() {return &SourceList;}
  /**Returns list of the bases;*/
  SBBaseInfoList*	baseList() {return &BaseList;};

  /**Returns SessionInfo corresponding to the name;*/
  SBVLBISesInfo*	sessionByName(const char* Name_){return SessionByName->find(Name_);};
  /**Returns StationInfo corresponding to the name;*/
  SBStationInfo*	stationByAka(const char* Aka_){return StationByAka->find(Aka_);};
  /**Returns SourceInfo corresponding to the name;*/
  SBSourceInfo*		sourceByAka(const char* Aka_){return SourceByAka->find(Aka_);};
  /**Returns BaseInfo corresponding to the name;*/
  SBBaseInfo*		baseByAka(const char* Aka_){return BaseByAka->find(Aka_);};
  
  // functions:
  //
  /**Adds a session to the list of sessions, returns TRUE if success.*/
  bool			addSession(const SBVLBISet*, const SBVLBISesInfo*);
  /**Deletes a session from the list of sessions, returns TRUE if success.*/
  bool			delSession(const SBVLBISet*, const SBVLBISesInfo*);
  /**Returns number of the observations in the project.*/
  int			numObs() const {return NumObs;};
  /**Returns number of the VLBI sessions in the project.*/
  int			numVLBISessions() const {return VLBISessionList.count();};
  /**Returns number of the stations in the project.*/
  int			numStations() const {return StationList.count();};
  /**Returns number of the sources in the project.*/
  int			numSources() const {return SourceList.count();};
  /**Returns number of the bases in the project.*/
  int			numBases() const {return BaseList.count();};
  /**Returns first date of observations.*/
  const SBMJD&		tStart() const
    {
      return VLBISessionList.count()?
	((SBVLBISesInfoList&)VLBISessionList).first()->tStart():TZero;};
  /**Returns last date of observations.*/
  const SBMJD&		tFinis() const
    {
      return VLBISessionList.count()?
	((SBVLBISesInfoList&)VLBISessionList).last()->tFinis():TZero;};
  /**Returns the interval of the observations.*/
  double		tInterval() const {return tFinis() - tStart();};
  virtual void		clearRMSs(){StationList.clearRMSs(); BaseList.clearRMSs(); SourceList.clearRMSs();};

  
  // I/O:
  //
  friend QDataStream &operator<<(QDataStream&, const SBProject&);
  friend QDataStream &operator>>(QDataStream&, SBProject&);
};
/*==============================================================================================*/



class SBSolution;
class SBEstimator;
class SBFrame;
class SBEphem;
class SBDelay;
class SBTideLd;
class SBRefraction;
class SBParameterList;
class SBAploEphem;
/*==============================================================================================*/
/**Class SBRunManager.
 * This class manages observations (different types: VLBI, ) data processing.
 *
 */
/*==============================================================================================*/
class SBRunManager
{
protected:
  // common stuff:
  SBProject*		Prj;			//!< Current project;
  bool			IsPrjOwner;		//!< Set if manager is project owner;
  SBSolution*		Solution;		//!< solution;
  bool			IsSolutionOwner;
  SBEstimator*		Estimator;		//!< estimator;
  
  // observation's cache:
  SBObservationList	ObsList;		//!< Current/cached list of observations;
  SBObsIterator*	ObsIt;			//!< Iterator to traverse through the obs.list;
  
  // geodynamics stuff:
  SB_TRF*		TRF;			//!< TRF;
  SBPlateMotion*	Plates;			//!< Tectonic plate motion model;
  SBPlateMotion*	RefPlates;		//!< Reference model of tectonic plate motion;
  SBFrame*		Frame;			//!< Reference frame transformer;
  SBEphem*		Ephem;			//!< Ephemeris;
  SBTideLd*		TideLd;			//!< Tide loading calculator;
  SBRefraction*		Refr;			//!< Refration calculator;
  SBAploEphem*		AploEphem;		//!< Ephemerides of atmospheric pressure loading;
  
  // VLBI specific stuff:
  SBVLBISet*		VLBI;			//!< VLBI dataset;
  bool			IsVLBIOwner;
  SB_CRF*		CRF;			//!< CRF;
  SBDelay*		Delay;			//!< Delay and rate calculator;
  int			prcsVLBISess;		//!< number of processed VLBI sessions;
  
  //well, I've got 256M RAM
  QList<SBVLBISession>	VLBISessCache;		//!< List of current opened sessions;
  SBVLBISesInfoList	VLBISessions;		//!< List of VLBI ses' infos ordered by TStart;
  SBVLBISesInfoIterator* VLBISessIt;		//!< Iterator to traverse through VLBI SessInfo.s
  SBMJD			T_2load;		//!< MJD date of the next session to load;
  SBMJD			T_2save;		//!< MJD date of the session to save; 
  SBMJD			Tb;
  SBMJD			Te;

  //parameters:
  SBParameterList	*AllGlobalList;		//!< List of all global parameters;
  SBParameterList	*GlobalList;		//!< List of global parameters;
  SBParameterList	*ArcList;		//!< List of arc parameters;
  SBParameterList	*LocalList;		//!< List of local parameters;
  SBParameterList	*StochList;		//!< List of stochastic parameters;
  
  // some utilities:
  /**Removes unused sources from the CRF catalogue.*/
  void			stripCRF();
  /**Removes unused sites and stations from the TRF catalogue.*/
  void			stripTRF();
  /**Fills a list of parameters according to the config.*/
  void			fillParameterList(SBParameterList*&, SBParameterCfg::PType, const SBMJD&);
  /**Updates the lists of parameters.
   * This function is called after each saving and loading of session.
   * The lists are containing only actual (currently processing) parameters.
   */
  void			checkParameterLists(const SBMJD&);

  inline const SBMJD&	findT_2load();
  inline SBMJD		findT_2save();
  bool			isObsEligible(SBObservation*);

  void			constrainClocks(bool);
  void			constraintStationCoord(bool);
  void			constraintStationVeloc(bool);
  void			constraintSourceCoord(bool);
  void			constraintEOP(bool);
  

  // mode 1:
  void			process_m1();
  bool			prepare4Run_m1();
  void			finisRun_m1();
  void			saveVLBISessions_m1();
  void			loadVLBISession_m1(const QString&);


  // mode 2:
  /**Starts data processing using mode 2.*/
  void			process_m2();
  /**Prepares the lists for processing. 
   * Returns TRUE if at least one observation exists in a list.
   */
  bool			prepare4Run_m2();
  /**Makes a post-processing manipulations.*/
  void			finisRun_m2();
  /**Saves processed VLBI session.*/
  void			saveVLBISessions_m2(const SBMJD&);
  /**Loads a VLBI session.*/
  void			loadVLBISessions_m2(SBMJD&);
  /**Makes the succeeding observation current and returns the new current observation.
   * Retuns NULL if the current obs() is the last in the list.
   */
  SBObservation*	nextObs_m2();
  
public:
  // constructors/destructors:
  //
  /**A constructor.
   * Creates a copy of object.
   * \param Prj_ the project.
   */
  SBRunManager(SBProject* =NULL, SBVLBISet* =NULL, SBSolution* =NULL);
  /**A destructor.*/
  virtual ~SBRunManager();
  /**Refers to a class name (debug info).*/
  virtual QString ClassName() const {return "SBRunManager";};
  
  // interface:
  //
  SBProject*		prj()  {return Prj;};
  SBVLBISet*		vlbi() const {return VLBI;};
  SBFrame*		frame() const {return Frame;};
  const SBEphem*	ephem() const {return Ephem;};
  SB_TRF*		trf() {return TRF;};
  const SBPlateMotion*	plates() const {return Plates;};
  const SBPlateMotion*	refPlates() const {return RefPlates;};
  SB_CRF*		crf() const {return CRF;};
  SBAploEphem*		aploEphem() {return AploEphem;};
  //
  SBObservationList*	obsList() {return &ObsList;};
  SBSolution*		solution() {return Solution;};
  SBEstimator*		estimator() {return Estimator;};
  SBObsIterator*	obsIt() {return ObsIt;};
  SBTideLd*		tideLd() {return TideLd;};
  SBRefraction*		refr() {return Refr;};
  SBDelay*		delay() {return Delay;};
    /**Returns the list of all global parameters.*/
  SBParameterList*	allGlobalList() {return AllGlobalList;};
    /**Returns the list of global parameters.*/
  SBParameterList*	globalList()	{return GlobalList;};
    /**Returns the list of arc parameters.*/
  SBParameterList*	arcList()	{return ArcList;};
    /**Returns the list of local parameters.*/
  SBParameterList*	localList()	{return LocalList;};
    /**Returns the list of stochastic parameters.*/
  SBParameterList*	stochList()	{return StochList;};


  // functions:
  //
  /**Returns a list of currently processing sessions.*/
  QString		listOfCurrentSessNames();
  /**Makes a data processing for observations described in Project.*/
  void			process() {process_m1();};

  void			makeReports();
  void			makeReportMaps();
  void			makeReportTRFVariations();
  void			makeReportCRFVariations();
  void			makeReportCRFVariations4IVS();
  void			makeReportTRF();
  void			makeReportCRF();
  void			makeReportEOP();
  void			makeReportTroposphere();
  void			makeReportTechDescription();
  void			makeReportNormalEqs();
  void			makeReportSessionStatistics();


  /**Returns the number of current opened VLBI sessions.*/
  int			numOpenedVLBISessions() const {return VLBISessCache.count();};
  
  // I/O:
  //
};

inline const SBMJD& SBRunManager::findT_2load()
{
  // VLBISessions.current() have to point to the next (unloaded) session, so
  return VLBISessIt->current()?VLBISessIt->current()->tStart():Te;
};

inline SBMJD SBRunManager::findT_2save()
{
  SBMJD t(Te);
  if (VLBISessCache.count())
    for (SBVLBISession *S=VLBISessCache.first(); S; S=VLBISessCache.next()) 
      if (S->last()->mjd()<t) t=*S->last();
  return t;
};
/*==============================================================================================*/






#endif //SB_GEO_PROJECT_H
