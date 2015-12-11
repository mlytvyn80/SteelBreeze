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

#ifndef SB_GEO_STATIONS_H
#define SB_GEO_STATIONS_H


#undef ELEV_DEBUG_OUTPUT
//#define ELEV_DEBUG_OUTPUT


#include <config.h>
#include <qintdict.h>

#include "SbVector3.H"
#include "SbGeo.H"
#include "SbGeoTime.H"



//external constants:
class SBStationID;
class SBStation;
class SB_TRF;
extern const SBStationID SidZero;




/*==============================================================================================*/
/*												*/
/* class SBTS_site1994										*/
/*												*/
/* a text stream for input iers.site files (ITRF 1994 version)					*/
/* (catalogue of IERS sites)									*/
/*==============================================================================================*/
class SBTS_site1994 : public QTextStream
{};


/*==============================================================================================*/
/*												*/
/* class SBTS_site2000										*/
/*												*/
/* a text stream for input iers.site files (ITRF 2000 version)					*/
/* (catalogue of IERS sites)									*/
/*==============================================================================================*/
class SBTS_site2000 : public QTextStream
{};


/*==============================================================================================*/
/*												*/
/* class SBTS_dir1994										*/
/*												*/
/* a text stream for input iers_dir.sta files (ITRF 1994 format)				*/
/* (directory of IERS stations)									*/
/*==============================================================================================*/
class SBTS_dir1994 : public QTextStream
{};


/*==============================================================================================*/
/*												*/
/* class SBTS_dir2000										*/
/*												*/
/* a text stream for input iers_dir.sta files (ITRF 2000 format)				*/
/* (directory of IERS stations)									*/
/*==============================================================================================*/
class SBTS_dir2000 : public QTextStream
{};


/*==============================================================================================*/
/*												*/
/* class SBTS_blq										*/
/*												*/
/* a text stream for input *.blq files								*/
/* (ocean loading coefficients, VLBI CALC format)						*/
/*==============================================================================================*/
class SBTS_blq : public QTextStream
{};


/*==============================================================================================*/
/*												*/
/* class SBTS_blqReq										*/
/*												*/
/* a text stream for output *.blqReq files							*/
/* (an auxiliary file)										*/
/*==============================================================================================*/
class SBTS_blqReq : public QTextStream
{};


class SBRunManager;
/*==============================================================================================*/
/*												*/
/* class SBTS_SINEX										*/
/*												*/
/* a text stream for I/O SINEX files								*/
/*												*/
/*==============================================================================================*/
class SBTS_SINEX : public QTextStream
{
private:
  SBRunManager	*RunMgr;
public:
  SBTS_SINEX()				: QTextStream()		{RunMgr=NULL;};
  SBTS_SINEX(QByteArray a, int mode)	: QTextStream(a, mode)	{RunMgr=NULL;};
  SBTS_SINEX(QIODevice* iod)		: QTextStream(iod)	{RunMgr=NULL;};
  SBTS_SINEX(QString& str, int fmode)	: QTextStream(str,fmode){RunMgr=NULL;};
  SBTS_SINEX(QString* str, int fmode)	: QTextStream(str,fmode){RunMgr=NULL;};
  SBTS_SINEX(FILE* fh, int mode)	: QTextStream(fh, mode)	{RunMgr=NULL;};
  SBRunManager*		runMgr() const {return RunMgr;};
  void			setRunMgr(SBRunManager *Mgr_) {RunMgr = Mgr_;};
};


class SBOLoadCarrier;
/*==============================================================================================*/
/*												*/
/* class SBOLoad										*/
/*												*/
/*==============================================================================================*/
class SBOLoad
{
  friend class SBSite;
  friend class SBStation;
public:
  enum PickUpBy {P_DomesNum=0, P_CDPNum=1, P_Domes=2, P_Name=3, P_Distance=4, P_Manual=5, P_NA=6};

private:
  double	Amplitude[3][11];
  double	Phase    [3][11];
  double	Dist; // needs only for looking up a site/station
  PickUpBy	PickedUp;
  QString	Info;
  QString	CommentLine1;
  QString	CommentLine2;

public:
  SBOLoad();
  SBOLoad(const SBOLoad&);
  virtual ~SBOLoad(){};
  virtual QString ClassName() const {return "SBOLoad";};
  virtual SBOLoad& operator= (const SBOLoad&);
  virtual bool operator==(const SBOLoad&) const;
  virtual bool operator!=(const SBOLoad&) const;

  /* interface: */
  inline double 	amplitude(int i, int j)	const {return Amplitude[i][j];};
  inline double 	phase    (int i, int j)	const {return Phase    [i][j];};
  inline double 	dist     ()		const {return Dist;};
  inline QString 	info     ()		const {return Info;};
  inline QString 	commentLine1()		const {return CommentLine1;};
  inline QString 	commentLine2()		const {return CommentLine2;};
  inline PickUpBy 	pickedUp ()		const {return PickedUp;};

  inline void setAmplitude(const double A_[3][11])
    {for (int c=0; c<3; c++) for (int j=0; j<11; j++) Amplitude[c][j] = A_[c][j];};
  inline void setAmplitude(int c, int j, double a)
    {if ((0<=c && c<3) && (0<=j && j<11)) Amplitude[c][j] = a;};
  inline void setPhase    (const double P_[11][3])
    {for (int c=0; c<3; c++) for (int j=0; j<11; j++) Phase    [c][j] = P_[c][j];};
  inline void setPhase    (int c, int j, double p) 
    {if ((0<=c && c<3) && (0<=j && j<11)) Phase[c][j] = p;};
  inline void setDist    (double Dist_)       {Dist = Dist_;};
  inline void setInfo    (const char* Info_)  {Info = Info_;};
  inline void setPickedUp(PickUpBy PickedUp_) {PickedUp = PickedUp_;};
  inline void setCommentLine1(const char* CommentLine1_) {CommentLine1 = CommentLine1_;};
  inline void setCommentLine2(const char* CommentLine2_) {CommentLine2 = CommentLine2_;};

  /* functions: */
  inline void clear()
    {for (int c=0; c<3; c++) for (int j=0; j<11; j++){Amplitude[c][j]=0.0; Phase[c][j]=0.0;};};
  /* I/O: */
  friend QDataStream &operator<<(QDataStream&, const SBOLoad&);
  friend QDataStream &operator>>(QDataStream&, SBOLoad&);
  friend SBTS_blq &operator<<(SBTS_blq&, const SBOLoadCarrier&);
  friend SBTS_blq &operator>>(SBTS_blq&, SBOLoadCarrier&);
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBPlateEntry										*/
/*												*/
/*==============================================================================================*/
class SBPlateEntry : public SBNamed
{
private:
  double	Omega[3];
public:
  SBPlateEntry(const QString& ="UNKN", double =0.0, double =0.0, double =0.0);
  virtual ~SBPlateEntry(){};
  virtual QString ClassName() const {return "SBPlateEntry";};
  /* interface: */
  /**Returns plate's angular velocity around axis DIRECTION (nrad/yr).*/
  inline double 	omega(DIRECTION Dir) const {return Omega[Dir];};
  /**Returns station's displacement due to plate motion.
   * \param dt time interval, years
   * \param R station's vector 
   */
  Vector3	displacement(long double dt, const Vector3& R) const;
  Vector3	velocity(const Vector3& R) const;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBPlateMotion										*/
/*												*/
/*==============================================================================================*/
class SBPlateMotion : public SBNList
{
protected:
  int			N;
  const SBPlateEntry	*Plates;
  QDict<SBPlateEntry>	*PlDict;
  virtual QString	ClassName() const {return "SBPlateMotion";};
  virtual Vector3	displacement(const SBMJD&, const SBMJD&, SBStation&) const;
public:
  SBPlateMotion(const SBPlateEntry[]=NULL, int=0);
  virtual ~SBPlateMotion();
  /**Calculates station's displacement due to plate motion. 
   * This is a fake entry, returns zero displacement, (0,0,0).
   */
  virtual Vector3 operator()(const SBMJD& T_, const SBMJD& T0_, SBStation& Station) const//{return v3Zero;};
    {return displacement(T_, T0_, Station);};
  virtual Vector3 velocity(SBStation& Station) const;
  virtual QString modelName() const {return "Plate Motion Model with zero velocities";};
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBPMNUVEL1A										*/
/*												*/
/*==============================================================================================*/
class SBPMNUVEL1A : public SBPlateMotion
{
protected:
  static const SBPlateEntry	NUVEL1A[14];
public:
  SBPMNUVEL1A() : SBPlateMotion(NUVEL1A, 14){};
  virtual QString	ClassName() const {return "NUVEL1A Plate Motion Model";};
  /**Calculates station's displacement due to plate motion. 
   * Looks up the plate, then calls SBPlateEntry::displacement(long double, SBStation&)
   * \param dt time in days
   */
  virtual Vector3 operator()(const SBMJD& T_, const SBMJD& T0_, SBStation& Station) const
    {return SBPlateMotion::displacement(T_, T0_, Station);};
  virtual Vector3 velocity(SBStation& Station) const
    {return SBPlateMotion::velocity(Station);};
  virtual QString modelName() const {return "NUVEL1A Plate Motion Model";};
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBPMNNR_NUVEL1A									*/
/*												*/
/*==============================================================================================*/
class SBPMNNR_NUVEL1A : public SBPlateMotion
{
protected:
  static const SBPlateEntry	NNR_NUVEL1A[16];
public:
  SBPMNNR_NUVEL1A() : SBPlateMotion(NNR_NUVEL1A, 16){};
  virtual QString	ClassName() const {return "NNR-NUVEL1A Plate Motion Model";};
  /**Calculates station's displacement due to plate motion. 
   * Look up the plate, then calls SBPlateEntry::displacement(long double, SBStation&)
   * \param dt time in days
   */
  virtual Vector3 operator()(const SBMJD& T_, const SBMJD& T0_, SBStation& Station) const
    {return SBPlateMotion::displacement(T_, T0_, Station);};
  virtual Vector3 velocity(SBStation& Station) const
    {return SBPlateMotion::velocity(Station);};
  virtual QString modelName() const {return "NNR-NUVEL1A Plate Motion Model";};
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBCoordinates										*/
/*												*/
/*==============================================================================================*/
class SBCoordinates : public SBMJD, public SBAttributed
{
public:
  /** Attributes of the coordinates.
   */
  enum Attributes
  {
    notValid   		= 1<< 0,	//!< omit this info
    Reserved_1		= 1<< 1,	//!< reserved
  };

private:
  Vector3	Coo;		//!< coordinates
  Vector3	CooErrors;	//!< uncert. of coordinates
  Vector3	Vel;		//!< velocities
  Vector3	VelErrors;	//!< uncert. of velocities
  QString	EventName;	//!< the cause of a change in the coordinates
  bool		IsUseVels;

public:
  // constructors/destructors:
  //
  SBCoordinates();
  SBCoordinates(const SBCoordinates&);
  SBCoordinates(const Vector3&, const Vector3&, const Vector3&, const Vector3&, const SBMJD&, const QString&);
  SBCoordinates& operator= (const SBCoordinates&);
  virtual ~SBCoordinates(){};
  QString ClassName() const {return "SBCoordinates";};

  // interface:
  //
  const SBMJD&		epoch()		const {return (const SBMJD&)*this;};
  const Vector3&	coo()		const {return Coo;};
  const Vector3&	cooErrors()	const {return CooErrors;};
  const Vector3&	vel()		const {return Vel;};
  const Vector3&	velErrors()	const {return VelErrors;};
  const QString&	eventName()	const {return EventName;};
  bool			isUseVels()	const {return IsUseVels;};

  void			setCoo(const Vector3& C_){Coo = C_;};
  void			setCooErrors(const Vector3& E_){CooErrors = E_;};
  void			setVel(const Vector3& V_){Vel = V_;};
  void			setVelErrors(const Vector3& E_){VelErrors = E_;};
  void			setEventName(const QString& N_){EventName = N_;};
  void			setIsUseVels(bool Is_){IsUseVels = Is_;};

  // functions:
  //
  bool			operator< (const SBCoordinates&) const;
  bool			operator==(const SBCoordinates&) const;
  
  // I/O:
  //
  friend QDataStream &operator<<(QDataStream& s, const SBCoordinates& C)
    {return s << (const SBMJD&)C << (const SBAttributed&)C << C.Coo << C.CooErrors 
	      << C.Vel << C.VelErrors << C.EventName << (uint)C.IsUseVels;};
  friend QDataStream &operator>>(QDataStream& s, SBCoordinates& C)
    {return s >> (SBMJD&)C >> (SBAttributed&)C >> C.Coo >> C.CooErrors 
	      >> C.Vel >> C.VelErrors >> C.EventName >> (uint&)C.IsUseVels;};
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBReferencePoint									*/
/*												*/
/*==============================================================================================*/
class SBReferencePoint	: public QList<SBCoordinates>
{
protected:
  virtual int compareItems(QCollection::Item item1, QCollection::Item item2)
    {
      return 
	(*(const SBMJD*)item1) <  (*(const SBMJD*)item2)?-1:
	(*(const SBMJD*)item1) == (*(const SBMJD*)item2)?0:1;
    };
  virtual QDataStream& write(QDataStream& s, QCollection::Item Entry) const
    {return s << *(const SBCoordinates*)Entry;};
  virtual QDataStream& read(QDataStream& s, QCollection::Item &Entry)
    {Entry = new SBCoordinates; return s >> *(SBCoordinates*)Entry;};

  const Vector3		*R_0;
  const Vector3		*DR_0;
  const Vector3		*V_0;
  const Vector3		*DV_0;

public:
  // constructors/destructors:
  //
  SBReferencePoint() : QList<SBCoordinates>() {setAutoDelete(TRUE); R_0 = DR_0 = V_0 = DV_0 = NULL;};
  SBReferencePoint(const SBReferencePoint&);
  virtual ~SBReferencePoint() {};
  QString ClassName() const {return "SBReferencePoint";};

  // interface:
  //
  SBReferencePoint& operator= (const SBReferencePoint&);

  // functions:
  //
  SBCoordinates*	findCoords(const SBMJD&);
  bool			registerCoords(const Vector3&, const Vector3&, const Vector3&, const Vector3&, 
				       const SBMJD&, const QString&);
  const Vector3&	r(const SBMJD&);
  const Vector3&	dR(const SBMJD&);
  const Vector3&	v(const SBMJD&);
  const Vector3&	dV(const SBMJD&);
  const Vector3&	r_0() const {return R_0?*R_0:v3Zero;};
  const Vector3&	dR_0() const{return DR_0?*DR_0:v3Zero;};
  const Vector3&	v_0() const {return V_0?*V_0:v3Zero;};
  const Vector3&	dV_0() const{return DV_0?*DV_0:v3Zero;};
  void			checkRanges();

  // I/O:
  //
  friend QDataStream &operator<<(QDataStream& s, const SBReferencePoint& RP_)
    {return s << (const QList<SBCoordinates>&)RP_;};
  friend QDataStream &operator>>(QDataStream& s, SBReferencePoint& RP_)
    {s >> (QList<SBCoordinates>&)RP_; RP_.checkRanges(); return s;};
};
/*==============================================================================================*/



class SBSite;
class SBStationID;
class SBTS_ssc1994;
class SBTS_ssc2000;
class SBParameter;
class QFile;
class QTextStream;
/*==============================================================================================*/
/*												*/
/* class SBStation										*/
/*												*/
/*==============================================================================================*/
class SBStation : public SBNamed, public SBAttributed
{
  friend class SB_TRF;
  friend class SBSite;
  friend class SBStationList;
public:
  enum SPointType {Pt_Antenna, Pt_Marker, Pt_Unkn};		// "S" and "M" chars
  enum SMountType {Mnt_AZEL, Mnt_EQUA, Mnt_X_YN, Mnt_X_YE, Mnt_Richmond, Mnt_Unkn};
  enum SClassCoord{Class_A, Class_B, Class_C, Class_D, Class_E, Class_Z, Class_Unknown};
  enum Attributes
  {
    OrigImported	= 1<<0,		//!< Info origin: imported;
    OrigEdited		= 1<<1,		//!< Info origin: has been edited (alterated by hand);
    OrigObsData		= 1<<2,		//!< Info origin: from obs data files;
    OrigEstimated	= 1<<3,		//!< Info origin: has been estimated;
    VelocImported	= 1<<4,		//!< Info veloct: imported;
    VelocEdited		= 1<<5,		//!< Info veloct: has been edited (alterated by hand);
    VelocEstimated	= 1<<6,		//!< Info veloct: has been estimated;
    hasOwnOLoad		= 1<<7,		//!< station has own tide loading coeffs
    Analysed		= 1<<8,		//!< station was used in the analyses;
    Reserved_10		= 1<<9,		//!< reserved;
    Reserved_11		= 1<<10,	//!< reserved;
    Reserved_12		= 1<<11		//!< reserved;
  };
  
  
private:
  SBSite*		Site;		//!< owner
  int			CDPNum;		//!< CDP Number
  int			DomeMinor;      //!< Domes Number: Site->DomeMajor + {M|S} + DomeMinor
  Technique		Tech;		//!< station's tech
  SPointType		PointType;	//!< Reference point type: Antenna or Ground marker
  SClassCoord		ClassCoord;	//!< coordinate class;
  QString		Description;	//!< station description, gets from ITRF files
  QString		Comments;	//!< station comments, filled by us
  SMountType		MountType;	//!< Mount type
  double		AxisOffset;	//!< station axis offset, m
  //  Vector3		R;		//!< station coordinates, m
  //  Vector3		dR;		//!< uncert. of coordinates, m
  SBReferencePoint	RefPt;
  Vector3		Rt;		//!< station coordinates calculated for time t, m
  Vector3		Ecc;		//!< eccentricity for the station
  Vector3		V;		//!< station velocities, m/year
  Vector3		Vt;		//!< actually used station velocities, m/yr
  Vector3		dV;		//!< uncert. of velocities, m/year
  QString		CharID;		//!< an id;
  SBOLoad*		OLoad;		//!< Ocean Loading parameters
  
  // calcables:
  double		Height;		//!< station Height, m
  double		Longitude;	//!< station Longitude, rad
  double		Latitude;	//!< station Latitude, rad
  Matrix3		FmVEN;		//!< matrix transformation from local VEN system
  
  // estimated parameters:
  SBParameter*		P_Clock[10];	//!< clock offset;
  SBParameter*		P_Zenith;	//!< zenith delay;
  SBParameter*		P_AtmGradN;	//!< horizontal delay gradient North;
  SBParameter*		P_AtmGradE;	//!< horizontal delay gradient East;
  SBParameter*		P_Axis;		//!< axis offset;
  SBParameter*		P_DRX;		//!< correction to x-coordinate;
  SBParameter*		P_DRY;		//!< correction to y-coordinate;
  SBParameter*		P_DRZ;		//!< correction to z-coordinate;
  SBParameter*		P_DVX;		//!< correction to x-velocity;
  SBParameter*		P_DVY;		//!< correction to y-velocity;
  SBParameter*		P_DVZ;		//!< correction to z-velocity;
  SBParameter*		P_Cable;	//!< cable correction multiplier;
  // tmps:
  SBParameter*		P_TideLag;
  SBParameter*		P_TideH2;
  SBParameter*		P_TideL2;
  SBParameter*		P_Aplo;

  // aux tmp:
#ifdef ELEV_DEBUG_OUTPUT
  QFile*		OutFile;
  QTextStream*		OutStream;
  SBMJD			Label;
#endif

  //
  friend bool operator==(const SBStation& S1, const SBStation& S2)
    {return (S1.DomeMinor==S2.DomeMinor) && (S1.PointType==S2.PointType);};
  friend bool operator!=(const SBStation& S1, const SBStation& S2) {return !(S1==S2);};
  friend bool operator<(const SBStation& S1, const SBStation& S2)
    {return S1.DomeMinor==S2.DomeMinor?S1.PointType<S2.PointType:S1.DomeMinor<S2.DomeMinor;};
  inline friend bool operator>(const SBStation& S1, const SBStation& S2)
    {return S1.DomeMinor==S2.DomeMinor?S1.PointType>S2.PointType:S1.DomeMinor>S2.DomeMinor;};
  
  double		axisOffsett()	const;
  
public:
  SBStation(SBSite*, const char* ="ZABRISKI");
  SBStation(SBSite*, const SBStation&);
  virtual ~SBStation() {if (isAttr(hasOwnOLoad) && OLoad) delete OLoad; releaseParameters();};
  virtual QString ClassName() const {return "SBStation";};
  virtual SBStation& operator= (const SBStation&);
  
  // interface:
  inline int		domeMinor()	const {return DomeMinor;};
  inline Technique	tech()		const {return Tech;};
  inline int		CDP()		const {return CDPNum;};
  inline SPointType	pointType()	const {return PointType;};
  inline SClassCoord	classCoord()	const {return ClassCoord;};
  inline const QString&	description()	const {return Description;};
  inline const QString&	comments()	const {return Comments;};
  inline SMountType	mountType()	const {return MountType;};
  inline double		axisOffset()	const {return AxisOffset;};
  inline double		longitude()	const {return Longitude;};
  // Geodetic latitude of station
  inline double		latitude()	const {return Latitude;};
  inline double		height()	const {return Height;};
  // Matrix transformation VEN -> TRF
  inline const Matrix3&	fmVEN()		const {return FmVEN;};
  inline const QString&	charID()	const {return CharID;};
  inline       SBOLoad*	oLoad()		const {return OLoad;};
  inline       SBSite*	site()		const {return Site;};
  //  inline const Vector3&	r()		const {return R;};
  //  inline const Vector3&	r_err()		const {return dR;};
  // coordinates ("three of a perfect pair"):
  inline SBReferencePoint&	refPt() {return RefPt;};
  inline const Vector3&	r (const SBMJD& t) {return RefPt.r (t);};
  inline const Vector3&	dR(const SBMJD& t) {return RefPt.dR(t);};
  // first coordinates in the list, if the time range does not matter
  inline const Vector3&	r_first()	const {return RefPt.r_0();};
  inline const Vector3&	dR_first()	const {return RefPt.dR_0();};
  inline const Vector3&	rt()		const {return Rt;};
  // eccentricities:
  inline const Vector3&	ecc()		const {return Ecc;};
  // velocities:
  inline const Vector3&	v()		const {return V;};
  inline const Vector3&	v_err()		const {return dV;};
  inline const Vector3&	vt()		const {return Vt;};
  Vector3		vModel(SBRunManager*);
  inline SBParameter*	p_C0()		{return P_Clock[0];};
  inline SBParameter*	p_C1()		{return P_Clock[1];};
  inline SBParameter*	p_C2()		{return P_Clock[2];};
  inline SBParameter*	p_Clock(int i)	{return (0<=i && i<10)?P_Clock[i]:NULL;};
  inline SBParameter*	p_Zenith()	{return P_Zenith;};
  inline SBParameter*	p_AtmGradN()	{return P_AtmGradN;};
  inline SBParameter*	p_AtmGradE()	{return P_AtmGradE;};
  inline SBParameter*	p_Axis()	{return P_Axis;};
  inline SBParameter*	p_DRX()		{return P_DRX;};
  inline SBParameter*	p_DRY()		{return P_DRY;};
  inline SBParameter*	p_DRZ()		{return P_DRZ;};
  inline SBParameter*	p_DVX()		{return P_DVX;};
  inline SBParameter*	p_DVY()		{return P_DVY;};
  inline SBParameter*	p_DVZ()		{return P_DVZ;};
  inline SBParameter*	p_Cable()	{return P_Cable;};
  //
  inline SBParameter*	p_TideLag()	{return P_TideLag;};
  inline SBParameter*	p_TideH2()	{return P_TideH2;};
  inline SBParameter*	p_TideL2()	{return P_TideL2;};
  inline SBParameter*	p_Aplo()	{return P_Aplo;};
  
  inline void	setDomeMinor   (int Domes_)		{DomeMinor = Domes_;};
  inline void	setTech        (Technique Tech_)	{Tech = Tech_;};
  inline void	setCDP         (int CDPNum_)		{CDPNum = CDPNum_;};
  inline void	setPointType   (SPointType PointType_)	{PointType = PointType_;};
  inline void	setClassCoord  (SClassCoord ClassCoord_){ClassCoord = ClassCoord_;};
  inline void	setDescription (const char* Descr_)	{Description = Descr_;};
  inline void	setComments    (const char* Comments_)	{Comments = Comments_;};
  inline void	setMountType   (SMountType MountType_)	{MountType = MountType_;};
  inline void	setAxisOffset  (double AxisOffset_)	{AxisOffset = AxisOffset_;};
  //  inline void	setR	       (const Vector3& R_) 
  //    {if ((R=R_)!=v3Zero) calcRLF(R, Height, Longitude, Latitude);};
  //  inline void	setR_i	       (DIRECTION i, double d_)
  //    {R[i]=d_; if (R!=v3Zero) calcRLF(R, Height, Longitude, Latitude);};
  //  inline void	setR_err       (const Vector3& dR_)	{dR = dR_;};
  //  inline void	setR_err_i     (DIRECTION i, double d_) {dR[i]=d_;};

  // coordinates:
  inline void	setCoords      (const Vector3& R_, const Vector3& dR_, 
				const Vector3& V_, const Vector3& dV_, 
				const SBMJD& T_, const QString& Name_)
  {if (RefPt.registerCoords(R_, dR_, V_, dV_, T_, Name_)) calcRLF(R_, Height, Longitude, Latitude);};

  inline void	setEcc	       (const Vector3& E_)	{Ecc = E_;};
  inline void	setV	       (const Vector3& V_)	{V  = V_;};
  inline void	setV_i	       (DIRECTION i, double d_) {V[i]=d_;};
  inline void	setV_err       (const Vector3& dV_)	{dV = dV_;};
  inline void	setV_err_i     (DIRECTION i, double d_) {dV[i]=d_;};
  inline void	setCharID      (const char* CharID_)	{CharID = CharID_;};
  inline void	setOLoad       (const SBOLoad& OLoad_)	{*OLoad = OLoad_;};

  /* functions: */
  SBStationID	id() const;
  static void	calcRLF(Vector3, double&, double&, double&);
  inline void	updateGeodCoo(const SBMJD& t)
    {Vector3 R_= RefPt.r(t); if (R_!=v3Zero) calcRLF(R_, Height, Longitude, Latitude);};
  inline void	createOLoad(const SBOLoad& OLoad_, SBOLoad::PickUpBy PickedUp_ =SBOLoad::P_Manual)
    {
      if (OLoad) delete OLoad;
      OLoad = new SBOLoad(OLoad_);
      OLoad -> setPickedUp(PickedUp_);
      addAttr(hasOwnOLoad);
    };
  inline void	deleteOLoad()
    {
      if (OLoad) delete OLoad;
      OLoad = NULL;
      delAttr(hasOwnOLoad);
    };
  void		createParameters();
  void		releaseParameters();
  void		updateParameters(SBRunManager*, const SBMJD&, bool=TRUE);
  void		getGlobalParameters4Report(SBRunManager*);
  void		getGlobalParameters4Report4Axel(SBRunManager*); //grrrr..
  void		getGlobalParameters4UpdateTRF(SBRunManager*);
  void		calcDisplacement(SBRunManager*, const SBMJD&, bool=TRUE);
  /**Calculates the lenght caused by antenna axis offset, m.*/
  double	axisOffsetLenght(const Vector3&, const SBMJD&, double);
  QString	charID4SINEX();
  void		updateStation(const SBStation&);

#ifdef ELEV_DEBUG_OUTPUT
  QTextStream*	outStream(){return OutStream;};
  void		openOutFile(const QString&);
  void		closeOutFile();
  const SBMJD&	label() const {return Label;};
  void		setLabel(const SBMJD& t_) {Label = t_;};
#endif
  
  /* I/O: */
  friend QDataStream &operator<<(QDataStream&, const SBStation&);
  friend QDataStream &operator>>(QDataStream&, SBStation&);
  
  friend SBTS_dir1994 &operator<<(SBTS_dir1994&, const SBStation&);
  friend SBTS_dir1994 &operator>>(SBTS_dir1994&, SBStation&);
  friend SBTS_dir2000 &operator<<(SBTS_dir2000&, const SBStation&);
  friend SBTS_dir2000 &operator>>(SBTS_dir2000&, SBStation&);
  friend SBTS_ssc1994 &operator<<(SBTS_ssc1994&, const SBStation&);
  friend SBTS_ssc2000 &operator<<(SBTS_ssc2000&, const SBStation&);
  friend SBTS_blqReq  &operator<<(SBTS_blqReq&,  const SBStation&);
};
/*==============================================================================================*/




/*==============================================================================================*/
/*												*/
/* class SBTS_ssc1994										*/
/*												*/
/* a text stream for input *.SSC? files (ITRF 1994 version)					*/
/* (coordinates of IERS stations)								*/
/*==============================================================================================*/
class SBTS_ssc1994 : public QTextStream
{
public:
  SBStation::SClassCoord	ClassCoord;
  SBTS_ssc1994(SBStation::SClassCoord ClassCoord_) : QTextStream()
    {ClassCoord = ClassCoord_;};
  SBTS_ssc1994(SBStation::SClassCoord ClassCoord_, QByteArray a, int mode) : QTextStream(a, mode)
    {ClassCoord = ClassCoord_;};
  SBTS_ssc1994(SBStation::SClassCoord ClassCoord_, QIODevice *d) : QTextStream(d)
    {ClassCoord = ClassCoord_;};
  SBTS_ssc1994(SBStation::SClassCoord ClassCoord_, FILE* fh, int mode) : QTextStream(fh,mode)
    {ClassCoord = ClassCoord_;};
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBTS_ssc2000										*/
/*												*/
/* a text stream for input *.SSC files (ITRF 2000 version)					*/
/* (coordinates of IERS stations)								*/
/*==============================================================================================*/
class SBTS_ssc2000 : public QTextStream
{};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBStationID										*/
/*												*/
/*==============================================================================================*/
class SBStationID
{
  friend class SB_TRF;
  friend class SBStation;
  friend class SBSite;
private:
  int				DomeMajor;
  SBStation::SPointType		PtType;
  int				DomeMinor;
public:
  SBStationID() {DomeMajor=99999; PtType=SBStation::Pt_Unkn; DomeMinor=0;};
  SBStationID(int DomeMajor_, SBStation::SPointType PtType_, int DomeMinor_) 
    {DomeMajor=DomeMajor_; PtType=PtType_; DomeMinor=DomeMinor_;};
  SBStationID(const SBStationID& ID) 
    {DomeMajor=ID.DomeMajor; PtType=ID.PtType; DomeMinor=ID.DomeMinor;};
  QString ClassName() const {return "SBStationID";};
  SBStationID& operator= (const SBStationID& ID)
    {DomeMajor=ID.DomeMajor; PtType=ID.PtType; DomeMinor=ID.DomeMinor; return *this;};

  /* interface: */
  inline int		domeMajor()	const {return DomeMajor;};
  inline int		domeMinor()	const {return DomeMinor;};
  inline SBStation::SPointType ptType()	const {return PtType;};
  inline void setDomeMajor(int Dome_) {DomeMajor = Dome_;};
  inline void setDomeMinor(int Dome_) {DomeMinor = Dome_;};
  inline void setPtType   (SBStation::SPointType PtType_){PtType = PtType_;};

  /* functions: */
  inline friend bool operator==(const SBStationID& S1, const SBStationID& S2)
    {
      return 
	(S1.DomeMajor==S2.DomeMajor) &&	(S1.DomeMinor==S2.DomeMinor) &&	(S1.PtType==S2.PtType);
    };
  inline friend bool operator!=(const SBStationID& S1, const SBStationID& S2){return !(S1==S2);};
  inline friend bool operator<(const SBStationID& S1, const SBStationID& S2)
    {
      if (S1.DomeMajor<S2.DomeMajor) return TRUE;
      else if (S1.DomeMajor>S2.DomeMajor) return FALSE;
      else return S1.DomeMinor==S2.DomeMinor?S1.PtType<S2.PtType:S1.DomeMinor<S2.DomeMinor;
    };
  inline friend bool operator>(const SBStationID& S1, const SBStationID& S2)
    {
      if (S1.DomeMajor>S2.DomeMajor) return TRUE;
      else if (S1.DomeMajor<S2.DomeMajor) return FALSE;
      else return S1.DomeMinor==S2.DomeMinor?S1.PtType>S2.PtType:S1.DomeMinor>S2.DomeMinor;
    };
  static bool	isValidStr(const QString&);
  inline bool	isValidId() const
    {return (9999<DomeMajor && DomeMajor<=99999 && 0<DomeMinor && DomeMinor<=999 
	     && PtType!=SBStation::Pt_Unkn)?TRUE:FALSE;};
  void		setID(const QString&);
  void		setID(const SBStationID& ID)
    {DomeMajor=ID.DomeMajor; PtType=ID.PtType; DomeMinor=ID.DomeMinor;};
  const QString	toString() const
    {
      QString Str;
      return Str.sprintf("%05d%c%03d", DomeMajor, 
			 PtType==SBStation::Pt_Antenna?'S':'M', DomeMinor);
    };
  
  /* I/O: */
  friend QDataStream &operator<<(QDataStream& s, const SBStationID& ID)
    {return s << ID.DomeMajor << (uint)ID.PtType << ID.DomeMinor;};
  friend QDataStream &operator>>(QDataStream& s, SBStationID& ID)
    {return s >> ID.DomeMajor >> (uint&)ID.PtType >> ID.DomeMinor;};
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBStationIDList									*/
/*												*/
/*==============================================================================================*/
class SBStationIDList : public SBNList
{
protected:
  virtual int compareItems(QCollection::Item item1, QCollection::Item item2)
    {
      if (*(SBStationID*)item1==*(SBStationID*)item2) return 0;
      return (*(SBStationID*)item1 < *(SBStationID*)item2)?-1:1;
    };
  virtual QDataStream& write(QDataStream& s, QCollection::Item Entry) const
    {return s << *(SBStationID*)Entry;};
  virtual QDataStream& read(QDataStream& s, QCollection::Item &Entry)
    {
      Entry = new SBStationID;
      return s >> *(SBStationID*)Entry;
    };
};
/*==============================================================================================*/
class SBStationIDIterator : public QListIterator<SBStationID>
{
public:
  SBStationIDIterator(const SBStationIDList& S) 
    : QListIterator<SBStationID>((const QList<SBStationID> &)S){};
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBStationList										*/
/*												*/
/*==============================================================================================*/
class SBStationList : public SBNList
{
protected:
  virtual int compareItems(QCollection::Item item1, QCollection::Item item2)
    {
      if (*(SBStation*)item1==*(SBStation*)item2) return 0; 
      return (*(SBStation*)item1 < *(SBStation*)item2)?-1:1;
    };
  virtual QDataStream& write(QDataStream& s, QCollection::Item Entry) const
    {return s << *(SBStation*)Entry;};
  virtual QDataStream& read(QDataStream& s, QCollection::Item &Entry)
    {
      Entry = new SBStation(NULL); 
      return s >> *(SBStation*)Entry;
    };
public:
  virtual SBStation*	first()	    {return (SBStation*)(SBNList::first());};
  virtual SBStation*	next()	    {return (SBStation*)(SBNList::next());};
  virtual SBStation*	at(uint idx){return (SBStation*)(SBNList::at(idx));};
};
/*==============================================================================================*/
class SBStationIterator : public QListIterator<SBStation>
{
public:
  SBStationIterator(const SBStationList& S) 
    : QListIterator<SBStation>((const QList<SBStation> &)S){};
};
/*==============================================================================================*/
/*  example:											*/
/*  SBStationIterator it(Site.stations());							*/
/*  for (SBStation* Station = it.toFirst(); it.current(); Station = ++it) s << *Station;	*/
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBSite											*/
/*												*/
/*==============================================================================================*/
class SBSite : public SBNamed
{
  friend class SB_TRF;
  friend class SBStation;
  friend class SBSiteEditor;
private:
  int			DomeMajor;	/* site Domes number (left part)			*/
  QString		Country;	/* Country						*/
  QString		Plate;		/* tectonic plate					*/
  SBOLoad		OLoad;		/* site OLoad parameters				*/
  Vector3		R;		/* mean site coordinates, m				*/
  Vector3		V;		/* site velocities, m/year				*/
  Vector3		dV;		/* uncert. of velocities, m/year			*/
  double		Longitude;
  double		Latitude;
  double		Height;
  SBStationList		Stations;	/* list of site's sttaions				*/
  QIntDict<SBStation>*	St_S;
  QIntDict<SBStation>*	St_M;
  void			copyStationsList(const SBStationList&);
  void			updateStationsList(const SBStationList&);
  void			prepareDicts();
  
  inline friend bool operator==(const SBSite& S1, const SBSite& S2)
    {return (S1.DomeMajor==S2.DomeMajor);};
  inline friend bool operator!=(const SBSite& S1, const SBSite& S2)
    {return (S1.DomeMajor!=S2.DomeMajor);};
  inline friend bool operator<(const SBSite& S1, const SBSite& S2)
    {return S1.DomeMajor<S2.DomeMajor;};
  inline friend bool operator>(const SBSite& S1, const SBSite& S2)
    {return S1.DomeMajor>S2.DomeMajor;};
public:
  SBSite(const QString& ="Zabriski Point");
  SBSite(const SBSite&);
  virtual ~SBSite(){if (St_S) delete St_S; if (St_M) delete St_M;};
  virtual QString ClassName() const {return "SBSite";};
  virtual SBSite& operator= (const SBSite&);
  
  /* interface: */
  inline int		domeMajor()	const {return DomeMajor;};
  inline QString	country()	const {return Country;};
  inline QString	plate()		const {return Plate;};
  inline SBOLoad&	oLoad()		      {return OLoad;};
  inline const Vector3&	r()		const {return R;};
  inline const Vector3&	v()		const {return V;};
  inline const Vector3&	v_err()		const {return dV;};
  inline double		longitude()	const {return Longitude;};
  inline double		latitude()	const {return Latitude;};
  inline double		height()	const {return Height;};

  inline void setDomeMajor(int Domes_)		   {DomeMajor= Domes_;};
  inline void setCountry  (const char* Country_)   {Country  = Country_;};
  inline void setPlate    (const char* Plate_)     {Plate    = Plate_;};
  inline void setOLoad	  (const SBOLoad& OLoad_)  {OLoad    = OLoad_;};
  inline void setV	  (const Vector3& V_)	   {V	     = V_;};
  inline void setV_err	  (const Vector3& dV_)	   {dV	     = dV_;};
  inline void setLongitude(double Longitude_)	   {Longitude= Longitude_;};
  inline void setLatitude (double Latitude_)	   {Latitude = Latitude_;};
  inline void setHeight (double Height_)	   {Height   = Height_;};

  SBStationList&	stations() {return Stations;};
  SBStation*		operator[](const SBStationID& ID){return find(ID);};
  SBStation*		find(const SBStationID&);
  virtual void		addStation(SBStation*);
  virtual bool		delStation(SBStation*);
  
  /* functions: */
  void			calcRLF();
  int			firstUnusedNumber(SBStation::SPointType);
  int			lastUnusedNumber(SBStation::SPointType);
  void			updateSite(const SBSite&);
  //  void			updateRefTpsOnly(const SBSite&);

  /* I/O: */
  friend QDataStream &operator<<(QDataStream&, const SBSite&);
  friend QDataStream &operator>>(QDataStream&, SBSite&);

  friend SBTS_site1994 &operator<<(SBTS_site1994&, const SBSite&);
  friend SBTS_site1994 &operator>>(SBTS_site1994&, SBSite&);
  friend SBTS_site2000 &operator<<(SBTS_site2000&, const SBSite&);
  friend SBTS_site2000 &operator>>(SBTS_site2000&, SBSite&);

  friend SBTS_dir1994 &operator<<(SBTS_dir1994&, const SBSite&);
  friend SBTS_dir1994 &operator>>(SBTS_dir1994&, SBSite&);
  friend SBTS_dir1994 &operator>>(SBTS_dir1994&, SB_TRF&);
  friend SBTS_dir2000 &operator<<(SBTS_dir2000&, const SBSite&);
  friend SBTS_dir2000 &operator>>(SBTS_dir2000&, SBSite&);
  friend SBTS_dir2000 &operator>>(SBTS_dir2000&, SB_TRF&);
  friend SBTS_ssc1994 &operator<<(SBTS_ssc1994&, const SBSite&);
  friend SBTS_ssc1994 &operator>>(SBTS_ssc1994&, SB_TRF&);
  friend SBTS_ssc2000 &operator<<(SBTS_ssc2000&, const SBSite&);
  friend SBTS_ssc2000 &operator>>(SBTS_ssc2000&, SB_TRF&);
  friend SBTS_blqReq  &operator<<(SBTS_blqReq&,  const SBSite&);
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBEccentricity										*/
/*												*/
/*==============================================================================================*/
class SBEccentricity
{
public:
  enum EccType		{ET_NEU, ET_XYZ};

private:
  QString	Aka;		//!< aka of a station
  SBMJD		TStart;		//!< first obs with the ecc
  SBMJD		TFinis;		//!< last obs with the ecc
  Vector3	Ecc;		//!< vector of eccentricity
  EccType	Type;		//!< the type of the Ecc vector
  QString	SessionName;	//!< name of the session

public:
  // constructors/destructors:
  //
  SBEccentricity();
  SBEccentricity(const SBEccentricity&);
  SBEccentricity& operator= (const SBEccentricity&);
  virtual ~SBEccentricity(){};
  QString ClassName() const {return "SBEccentricity";};

  // interface:
  //
  const QString&	aka()	 const {return Aka;};
  const SBMJD&		tStart() const {return TStart;};
  const SBMJD&		tFinis() const {return TFinis;};
  const Vector3&	ecc()	 const {return Ecc;};
  EccType		type()   const {return Type;};
  const QString&	sessionName()  const {return SessionName;};

  void			setAka(const QString& Aka_) {Aka = Aka_;};
  void			setTStart(const SBMJD& T_)  {TStart = T_;};
  void			setTFinis(const SBMJD& T_)  {TFinis = T_;};
  void			setEcc(const Vector3& V_)   {Ecc = V_;};
  void			setType(EccType Type_)      {Type = Type_;};
  void			setSessionName(const QString& SN_) {SessionName = SN_;};
  
  // functions:
  //
  bool			operator< (const SBEccentricity&) const;
  bool			operator==(const SBEccentricity&) const;
  
  // I/O:
  //
  friend QDataStream &operator<<(QDataStream& s, const SBEccentricity& Ecc)
    {
//    if (Ecc.SessionName == "")
      s << Ecc.Aka << Ecc.TStart << Ecc.TFinis << Ecc.Ecc << (uint)Ecc.Type << Ecc.SessionName;
//    return s << Ecc.Aka << Ecc.TStart << Ecc.TFinis << Ecc.Ecc << (uint)Ecc.Type << Ecc.SessionName;
      return s;
    };
  friend QDataStream &operator>>(QDataStream& s, SBEccentricity& Ecc)
    {return s >> Ecc.Aka >> Ecc.TStart >> Ecc.TFinis >> Ecc.Ecc >> (uint&)Ecc.Type >> Ecc.SessionName;};
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBEccentriList										*/
/*												*/
/*==============================================================================================*/
class SBEccentriList	: public QList<SBEccentricity>
{
protected:
  virtual int compareItems(QCollection::Item item1, QCollection::Item item2)
    {
      return 
	(*(const SBEccentricity*)item1) <  (*(const SBEccentricity*)item2)?-1:
	(*(const SBEccentricity*)item1) == (*(const SBEccentricity*)item2)?0:1;
    };
  virtual QDataStream& write(QDataStream& s, QCollection::Item Entry) const
    {return s << *(const SBEccentricity*)Entry;};
  virtual QDataStream& read(QDataStream& s, QCollection::Item &Entry)
    {Entry = new SBEccentricity; return s >> *(SBEccentricity*)Entry;};
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBStationEcc										*/
/*												*/
/*==============================================================================================*/
class SBStationEcc : public SBStationID
{
private:
  SBEccentriList	Eccs;

public:
  // constructors/destructors:
  //
  SBStationEcc() : SBStationID() {Eccs.setAutoDelete(TRUE);};
  SBStationEcc(const SBStationEcc& Ecc_) : SBStationID(Ecc_) {Eccs.setAutoDelete(TRUE);};
  virtual ~SBStationEcc() {};
  QString ClassName() const {return "SBStationEcc";};

  // interface:
  //
  void			setID(const SBStationID& ID_) {(SBStationID&)*this = ID_;};
  SBEccentriList&	eccs() {return Eccs;};
  
  // functions:
  //
  SBEccentricity*	findEcc(const SBMJD&);
  bool			registerEcc(const SBEccentricity&);
  
  // I/O:
  //
  friend QDataStream &operator<<(QDataStream& s, const SBStationEcc& StEcc)
    {return s << (const SBStationID&)StEcc << StEcc.Eccs;};
  friend QDataStream &operator>>(QDataStream& s, SBStationEcc& StEcc)
    {return s >> (SBStationID&)StEcc >> StEcc.Eccs;};
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBEcc											*/
/*												*/
/*==============================================================================================*/
class SBEcc : public QList<SBStationEcc>
{
protected:
  virtual int compareItems(QCollection::Item item1, QCollection::Item item2)
    {
      return 
	(*(const SBStationEcc*)item1)< (*(const SBStationEcc*)item2)?-1:
	(*(const SBStationEcc*)item1)==(*(const SBStationEcc*)item2)?0:1;
    };
  virtual QDataStream& write(QDataStream& s, QCollection::Item Entry) const
    {return s << *(const SBStationEcc*)Entry;};
  virtual QDataStream& read(QDataStream& s, QCollection::Item &Entry)
    {Entry = new SBStationEcc; return s >> *(SBStationEcc*)Entry;};


  QDict<SBStationEcc>		*EccByID;

  void				fillDicts();
public:
  // constructors/destructors:
  //
  SBEcc();
  SBEcc(const SBEcc&);
  virtual ~SBEcc();
  QString ClassName() const {return "SBEcc";};

  // interface:
  //
  QDict<SBStationEcc>*		eccByID() {return EccByID;};
  
  // functions:
  //
  SBEccentricity*		findEcc(const SBStationID&, const SBMJD&);
  bool				registerEcc(const SBStationID&, const SBEccentricity&);
  bool				importEccDat(const QString&, SB_TRF*);

  
  // I/O:
  //
  friend QDataStream &operator<<(QDataStream& s, const SBEcc& Ecc)
    {return s << (const QList<SBStationEcc>&)Ecc;};
  friend QDataStream &operator>>(QDataStream& s, SBEcc& Ecc)
    {s >> (QList<SBStationEcc>&)Ecc; Ecc.fillDicts(); return s;};
};
/*==============================================================================================*/





class SBVLBISession;
/*==============================================================================================*/
/*												*/
/* class SB_TRF											*/
/*												*/
/*==============================================================================================*/
class SB_TRF : public SBCatalog
{
protected:
  QIntDict<SBSite>	*SiteByDome;
  QIntDict<SBStation>	*StationByCDP;
  SBNList		*StationNames;
  QList<SBStation>	*Stations;		//!< list of stations (for runner/estimator)
  SBEcc			ECCs;

  virtual int compareItems(QCollection::Item item1, QCollection::Item item2)
    {SBSite *s1=(SBSite*)item1, *s2=(SBSite*)item2; return *s1<*s2?-1:*s1==*s2?0:1;};
  virtual QDataStream& write(QDataStream& s, QCollection::Item Entry) const
    {return s << *(SBSite*)Entry;};
  virtual QDataStream& read(QDataStream& s, QCollection::Item &Entry)
    {Entry = new SBSite; return s >> *(SBSite*)Entry;};

public:
  SB_TRF() : SBCatalog() 
    {
      SiteByDome   = NULL;
      StationByCDP = NULL;
      StationNames = NULL;
      Stations     = NULL;
      Label.setType(SBSolutionName::TYPE_SSC); 
      prepareDicts();
    };
  SB_TRF(const SB_TRF& TRF_) : SBCatalog(TRF_) {Stations=NULL; prepareDicts();};
  virtual ~SB_TRF()
    {
      if (SiteByDome) 
	{
	  delete SiteByDome;
	  SiteByDome = NULL;
	};
      if (StationByCDP) 
	{
	  delete StationByCDP;
	  StationByCDP = NULL;
	};
      if (StationNames) 
	{
	  delete StationNames;
	  StationNames = NULL;
	};
      if (Stations) 
	{
	  delete Stations;
	  Stations = NULL;
	};
    };
  virtual QString	ClassName() const {return "SB_TRF";};
  
  
  /* type conversion: */
  virtual void		clear() {SiteByDome->clear(); StationByCDP->clear(); SBCatalog::clear();};
  virtual SBSite*	at(uint i) {return (SBSite*)(SBCatalog::at(i));};
  virtual void		insert(const SBSite* N){inSort(N);};
  virtual void		inSort(const SBSite*);
  virtual SBSite*	first() {return (SBSite*)(SBCatalog::first());};
  virtual SBSite*	next () {return (SBSite*)(SBCatalog::next ());};
  virtual SBSite*	find(const SBSite& N) const 
    {return (SBSite*)(SiteByDome->find(N.domeMajor()));};
  virtual SBSite*	find(int     DomeMajor_) const 
    {return (SBSite*)(SiteByDome->find(  DomeMajor_) );};
  virtual SBStation*	find(const SBStationID& ID_) const /* ! returns Station, not a Site */
    {SBSite* W; if ((W=SiteByDome->find(ID_.DomeMajor))) return W->find(ID_); else return NULL;};
  virtual SBSite*	operator[](const SBSite& N)		const {return find(N);};
  virtual SBSite*	operator[](int DomeMajor_)		const {return find(DomeMajor_);};
  virtual SBStation*	operator[](const SBStationID& Id_)	const {return find(Id_);};

  virtual bool		remove()
    {
      SiteByDome->remove(((SBSite*)current())->domeMajor()); 
      return SBCatalog::remove();
    };
  virtual bool		remove(int DomeMajor_) /* not i-th element! */
    {
      SBSite* S = SiteByDome->find(DomeMajor_);
      SiteByDome->remove(DomeMajor_);
      return SBCatalog::remove(S);
    };
  virtual bool		remove(const SBSite* N)
    {
      SiteByDome->remove(N->domeMajor()); 
      return SBCatalog::remove(N);
    };
  virtual bool		removeFirst()
    {
      SiteByDome->remove(((SBSite*)getFirst())->domeMajor()); 
      return SBCatalog::removeFirst();
    };
  virtual bool		removeLast()
    {
      SiteByDome->remove(((SBSite*)getLast())->domeMajor()); 
      return SBCatalog::removeLast();
    };
  virtual SBSite*	take()
    {
      SiteByDome->take(((SBSite*)current())->domeMajor()); 
      return (SBSite*)SBCatalog::take();
    };
  virtual SBSite*	take(uint i)
    {
      SiteByDome->take(((SBSite*)at(i))->domeMajor()); 
      return (SBSite*)SBCatalog::take(i);
    };
  QList<SBStation>*	stations() {return Stations;};
  
  /* functions: */
  bool			setSiteName(int, const QString&);
  int			lastUnusedNumber() const;

  void			prepareDicts();
  
  virtual void		prepareAliases4Output(){};
  virtual void		releaseAliases4Output(){};
  virtual void		collectObjAliases(QString){};
  virtual void		releaseObjAliases(QString){};
  virtual void		prepareReferences();
  virtual void		prepare4Run();
  virtual void		prepareEccs4Session(const SBMJD&, SBVLBISession*);
  virtual void		finisRun();
  virtual SBNList*	referenceList(){return StationNames;};

  SBEcc&		ecc() {return ECCs;};

  virtual SBStation*	lookupStation(const QString&);
  virtual SBStation*	lookupStation(int CDP_) {return StationByCDP->find(CDP_);};
  virtual SBStationID	lookupID(const QString&);
  virtual SBStation*	lookupNearest(const SBMJD&, const Vector3&, double&, Technique=TECH_VLBI);
   
  /*  i/o:  */
  friend QTextStream	&operator<<(QTextStream&, const SB_TRF&);
  friend QTextStream	&operator>>(QTextStream&, SB_TRF&);
  
  friend QDataStream	&operator<<(QDataStream&, const SB_TRF&);
  friend QDataStream	&operator>>(QDataStream&, SB_TRF&);

  friend SBTS_site1994	&operator<<(SBTS_site1994&, const SB_TRF&);
  friend SBTS_site1994	&operator>>(SBTS_site1994&, SB_TRF&);
  friend SBTS_site2000	&operator<<(SBTS_site2000&, const SB_TRF&);
  friend SBTS_site2000	&operator>>(SBTS_site2000&, SB_TRF&);

  friend SBTS_dir1994	&operator<<(SBTS_dir1994&, const SB_TRF&);
  friend SBTS_dir1994	&operator>>(SBTS_dir1994&, SB_TRF&);
  friend SBTS_dir2000	&operator<<(SBTS_dir2000&, const SB_TRF&);
  friend SBTS_dir2000	&operator>>(SBTS_dir2000&, SB_TRF&);

  friend SBTS_ssc1994	&operator<<(SBTS_ssc1994&, const SB_TRF&);
  friend SBTS_ssc1994	&operator>>(SBTS_ssc1994&, SB_TRF&);
  friend SBTS_ssc2000	&operator<<(SBTS_ssc2000&, const SB_TRF&);
  friend SBTS_ssc2000	&operator>>(SBTS_ssc2000&, SB_TRF&);

  friend SBTS_blq	&operator<<(SBTS_blq&,     const SB_TRF&);
  friend SBTS_blqReq	&operator<<(SBTS_blqReq&,  const SB_TRF&);

  friend SBTS_SINEX	&operator<<(SBTS_SINEX&, SB_TRF&);
  friend SBTS_SINEX	&operator>>(SBTS_SINEX&, SB_TRF&);

private: /* disables (is it right?)*/
  virtual SBSite*	find(const QString& S){return (SBSite*)(SBCatalog::find(S));};
  virtual SBSite* operator[](const QString& S){return find(S);};
};
/*==============================================================================================*/
class SBSiteIterator : public QListIterator<SBSite>
{
 public:
  SBSiteIterator (const SB_TRF& TRF) : QListIterator<SBSite>((const QList<SBSite> &)TRF){};
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBOLoadCarrier										*/
/*												*/
/*==============================================================================================*/
class SBOLoadCarrier : public SBNamed
{
 private:
  SBOLoad		OLoad;
  double		Longitude;
  double		Latitude;
  //QString		SiteName; //it's hard to extract real site name from blq
  QString		CharID;
  int			CDPNum;
  SBStationID		Id;
  bool			isSuccessfulLoad;
 public:
  SBOLoadCarrier();
  SBOLoadCarrier(const SBOLoadCarrier&);
  SBOLoadCarrier(SBSite&);
  SBOLoadCarrier(const SBStation&);
  virtual ~SBOLoadCarrier(){};
  virtual QString ClassName() const {return "SBOLoadCarrier";};
  virtual SBOLoadCarrier& operator= (const SBOLoadCarrier&);
  virtual bool operator==(const SBOLoadCarrier&) const;
  virtual bool operator!=(const SBOLoadCarrier&) const;

  /* interface: */
  virtual	bool IsSuccessfulLoad() const {return isSuccessfulLoad;};
  virtual const SBStationID& id() const {return Id;};
  virtual	int CDP() const {return CDPNum;};
  virtual const SBOLoad& oLoad() const {return OLoad;};
  virtual const QString& charID() const {return CharID;};


  /* functions: */
  double calcDistance(double, double) const;
  /* I/O: */
  friend SBTS_blq &operator<<(SBTS_blq&, const SBOLoadCarrier&);
  friend SBTS_blq &operator>>(SBTS_blq&, SBOLoadCarrier&);
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBOLoadList										*/
/*												*/
/*==============================================================================================*/
class SBOLoadList : public SBNList
{
  friend SBTS_blq &operator>>(SBTS_blq&, SBOLoadList&);
};
/*==============================================================================================*/




#endif //SB_GEO_STATIONS_H
