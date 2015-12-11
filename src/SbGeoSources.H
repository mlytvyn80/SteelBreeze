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

#ifndef SB_GEO_SOURCES_H
#define SB_GEO_SOURCES_H

#include <config.h>
#include <stdlib.h>
#include <math.h>

#include <qdatastream.h>
#include <qlist.h>
#include <qtextstream.h>
#include <qstring.h>

#include "SbGeneral.H"
#include "SbGeo.H"
#include "SbGeoTime.H"
#include "SbVector3.H"

class SB_CRF;


/*==============================================================================================*/
/*												*/
/* class SBTS_car										*/
/*												*/
/* a text stream for input *.car files								*/
/* (physical characteristics of radio sources)							*/
/*==============================================================================================*/
class SBTS_car : public QTextStream
{};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBTS_dico										*/
/*												*/
/* a text stream for input *.dico files								*/
/* (dictionary of aliases of radio sources)							*/
/*==============================================================================================*/
class SBTS_dico : public QTextStream
{};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBSource										*/
/*												*/
/*==============================================================================================*/
class SBParameter;
class SBRunManager;
class SBTS_rsc;
class SBTS_report;
class SBTS_report4IVS;
class SBSource : public SBNamed, public SBAttributed
{
  friend class SB_CRF;
  friend SBTS_dico  &operator<<(SBTS_dico&, SB_CRF&);

public:
  enum SStatus		{STATUS_DEF, STATUS_CAND, STATUS_OTHER, STATUS_NEW, STATUS_UNKN};
  enum SType		{TYPE_QUASAR, TYPE_GALAXY, TYPE_BL_LAC, 
			 TYPE_BL_LAC_C, TYPE_OTHER, TYPE_UNKN};
  enum SSpectrumClass	{SP_SEYFERT, SP_SEYFERT1, SP_SEYFERT1_2, SP_SEYFERT2, SP_SEYFERT3, 
			 SP_HP, SP_OTHER, SP_UNKN};

  enum Attributes
  {
    OrigImported	= 1<<0,		//!< Info origin: imported;
    OrigEdited		= 1<<1,		//!< Info origin: has been edited (alterated by hand);
    OrigObsData		= 1<<2,		//!< Info origin: from obs data files;
    OrigEstimated	= 1<<3,		//!< Info origin: has been estimated;
    StatusSuspected	= 1<<4,		//!< a la `substatus': "indicates that the source is suspected to be not suitable to define the axes in the future
    Analysed		= 1<<5,		//!< temporary attr;
    AnalysConstr	= 1<<6,		//!< the coordinates of a source were used in constraint equations;
    Reserved_08		= 1<<7,		//!< reserved;
    Reserved_09		= 1<<8,		//!< reserved;
    Reserved_10		= 1<<9,		//!< reserved;
    Reserved_11		= 1<<10,	//!< reserved;
    Reserved_12		= 1<<11,	//!< reserved;
    Reserved_13		= 1<<12,	//!< reserved;
    Reserved_14		= 1<<13,	//!< reserved;
    Reserved_15		= 1<<14,	//!< reserved;
    Reserved_16		= 1<<15,	//!< reserved;
    Reserved_17		= 1<<16		//!< reserved;
  };

private:
  double		RA;		/* Right Ascension */
  double		dRA;		/* uncert. of the Right Ascension */
  double		DN;		/* Declination to the North */
  double		dDN;		/* uncert. of the Declination to the North */
  QString		ICRF;		/* ICRF name */
  QString		ObsName;	/* how it's called in obs.data */
  SStatus		Status;		/* source status */
  SType			Type;		/* source type */
  SSpectrumClass	SpectrumClass;	/* source spectrum */
  bool			IsHLink;	/* source serves to link the Hipparcos stellar RF */
  float 		RedShift;	/* redshift */
  float 		Flux1;		/* flux @  6cm */
  float 		Flux2;		/* flux @ 15cm (or 11cm) */
  float 		SpIdx;		/* spectral index from Preston et al. */
  float 		Mv;		/* mv */
  int			NObsApr;	/* number of delays (and rates) */
  int			NSessApr;	/* number of sessions of observations */
  SBMJD			MeanMJD;	/* mean MJD of observation span */
  SBMJD			FirstMJD;	/* first MJD of observation span */
  SBMJD			LastMJD;	/* last MJD of observation span */
  double		RA_DN;		/* correlation RA-DN */
  // new form ICRF-Ext1
  int			StructIdxX;
  int			StructIdxS;

  QList<QString>*	aList;		/* for temporary use */

  // estimated parameters:
  SBParameter*		P_RA;		//!< estimation of the right ascension;
  SBParameter*		P_DN;		//!< estimation of the declination;
  double		RAt;
  double		DNt;

  // f..d SINEX!
  QString		SINEXCharID;
  
public:
  SBSource(const QString& ="0000+000", double=0.0, double=0.0);
  SBSource(const SBSource&);
  virtual ~SBSource() {if (aList) delete aList;};
  virtual QString ClassName() const {return "SBSource";};
  virtual SBSource& operator= (const SBSource&);

  /* interface: */
  inline double		ra()		const {return RA;};
  inline double		dn()		const {return DN;};
  inline double		ra_t()		const {return RAt;};
  inline double		dn_t()		const {return DNt;};
  inline double		ra_err()	const {return dRA;};
  inline double		dn_err()	const {return dDN;};
  inline QString	ICRFName()	const {return ICRF;};
  inline QString	obsName()	const {return ObsName;};
  inline SStatus	status()	const {return Status;};
  inline SType		type()		const {return Type;};
  inline SSpectrumClass spectrum()	const {return SpectrumClass;};
  inline float		redShift()	const {return RedShift;};
  inline float		flux1()		const {return Flux1;};
  inline float		flux2()		const {return Flux2;};
  inline float		spIdx()		const {return SpIdx;};
  inline float		mv()		const {return Mv;};
  inline int		nObsApr()	const {return NObsApr;};
  inline int		nSessApr()	const {return NSessApr;};
  inline const SBMJD&	meanMJD()	const {return MeanMJD;};
  inline const SBMJD&	firstMJD()	const {return FirstMJD;};
  inline const SBMJD&	lastMJD()	const {return LastMJD;};
  inline double		corrRA_DN()	const {return RA_DN;};
  inline bool		isHLink()	const {return IsHLink;};
  inline int		structIdxX()	const {return StructIdxX;};
  inline int		structIdxS()	const {return StructIdxS;};

  inline SBParameter*	p_RA()		{return P_RA;};
  inline SBParameter*	p_DN()		{return P_DN;};

  QList<QString>*	aliasesList(){return aList;};
  inline void		clearAliasesList(){if (aList) delete aList; aList = NULL;};

  inline void setRA       (double RA_)		{RA		= RA_;		};
  inline void setDN       (double DN_)		{DN		= DN_;		};
  inline void setRA_err   (double dRA_)		{dRA		= dRA_;		};
  inline void setDN_err   (double dDN_)		{dDN		= dDN_;		};
  inline void setICRFName (const QString& ICRF_){ICRF		= ICRF_;	};
  inline void setObsName  (const QString& ObsN_){ObsName	= ObsN_;	};
  inline void setStatus   (SStatus Status_)	{Status		= Status_;	};
  inline void setType     (SType Type_)		{Type		= Type_;	};
  inline void setSpectrum (SSpectrumClass Sp_)	{SpectrumClass	= Sp_;		};
  inline void setRedShift (float RedShift_)	{RedShift	= RedShift_;	};
  inline void setFlux1    (float Flux1_)	{Flux1		= Flux1_;	};
  inline void setFlux2    (float Flux2_)	{Flux2		= Flux2_;	};
  inline void setSpIdx	  (float SpIdx_)	{SpIdx		= SpIdx_;	};
  inline void setMv       (float Mv_)		{Mv		= Mv_;		};
  inline void setNObsApr  (int NObsApr_)	{NObsApr	= NObsApr_;	};
  inline void setNSessApr (int NSessApr_)	{NSessApr	= NSessApr_;	};
  inline void setMeanMJD  (const SBMJD& aMJD_)	{MeanMJD	= aMJD_;	};
  inline void setFirstMJD (const SBMJD& aMJD_)	{FirstMJD	= aMJD_;	};
  inline void setLastMJD  (const SBMJD& aMJD_)	{LastMJD	= aMJD_;	};
  inline void setCorrRA_DN(double RA_DN_)	{RA_DN		= RA_DN_;	};
  inline void setIsHLink  (bool IsHLink_)	{IsHLink	= IsHLink_;	};
  inline void setStructIdxX(int Idx_)		{StructIdxX	= Idx_;		};
  inline void setStructIdxS(int Idx_)		{StructIdxS	= Idx_;		};


  const QString&	sinexCharID() const {return SINEXCharID;};
  void			setSINEXCharID(const QString& S_){SINEXCharID = S_;};
  
  /* functions: */
  static bool isValidICRFName(QString);
  void createParameters();
  void releaseParameters();
  void updateParameters(SBRunManager*, const SBMJD&, bool=TRUE);
  void getGlobalParameters4Report(SBRunManager*);
  // collect results and statistics for reports (global + local parameters):
  void getParameters4Report(SBRunManager*);
  

  QString RA2str() const;
  QString DN2str() const;
  virtual Vector3	rt() const 
    {
      double sinRA, cosRA, sinDN, cosDN;
      sincos (ra_t(), &sinRA, &cosRA);
      sincos (dn_t(), &sinDN, &cosDN);
      return Vector3(cosRA*cosDN, sinRA*cosDN, sinDN);
    };
  
  /* I/O: */
  friend QDataStream &operator<<(QDataStream&, const SBSource&);
  friend QDataStream &operator>>(QDataStream&, SBSource&);
  
  friend SBTS_rsc  &operator<<(SBTS_rsc&, const SBSource&);
  friend SBTS_rsc  &operator>>(SBTS_rsc&, SBSource&);
  friend SBTS_car  &operator<<(SBTS_car&, const SBSource&);
  friend SBTS_car  &operator>>(SBTS_car&, SBSource&);

  friend SBTS_report &operator<<(SBTS_report&, const SBSource&);
  friend SBTS_report4IVS &operator<<(SBTS_report4IVS&, const SBSource&);
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBTS_rsc										*/
/*												*/
/* a text stream for input *.rsc files								*/
/* (coordinates of radio sources)								*/
/*==============================================================================================*/
class SBTS_rsc : public QTextStream
{
private:
  SBSource::SStatus	AssignedStatus;
public:
  SBTS_rsc() : QTextStream() {AssignedStatus = SBSource::STATUS_UNKN;};
  void			setAssignedStatus(SBSource::SStatus AS_) {AssignedStatus = AS_;};
  SBSource::SStatus	assignedStatus() const {return AssignedStatus;};
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBTS_report										*/
/*												*/
/* a text stream for output a solution								*/
/* (coordinates of radio sources)								*/
/*==============================================================================================*/
class SBTS_report : public QTextStream
{
};
/*==============================================================================================*/


/*==============================================================================================*/
/*												*/
/* class SBTS_report4IVS									*/
/*												*/
/* a text stream for output a solution								*/
/* (coordinates of radio sources)								*/
/*==============================================================================================*/
class SBTS_report4IVS : public QTextStream
{
};
/*==============================================================================================*/


/*==============================================================================================*/
/*												*/
/* class SB_CRF											*/
/*												*/
/*==============================================================================================*/
class SB_CRF : public SBCatalog
{
protected:
  virtual QDataStream& write(QDataStream& s, QCollection::Item Entry) const
    {return s << *(SBSource*)Entry;};
  virtual QDataStream& read(QDataStream& s, QCollection::Item &Entry)
    {Entry = new SBSource; return s >> *(SBSource*)Entry;};

public:
  inline SB_CRF() : SBCatalog() 
  {
    Label.setType(SBSolutionName::TYPE_RSC);
    Label.setTech(TECH_VLBI);
  };
  virtual ~SB_CRF()      {};

  virtual QString ClassName() const {return "SB_CRF";};


  /* type conversion: */
  virtual SBSource* find(const SBSource& N) const {return (SBSource*)(SBCatalog::find(N));};
  virtual SBSource* find(const QString&  S) const {return (SBSource*)(SBCatalog::find(S));};
  virtual SBSource* operator[](const SBSource& N) const {return find(N);};
  virtual SBSource* operator[](const QString&  S) const {return find(S);};
  virtual SBSource* first() {return (SBSource*)(SBCatalog::first());};
  virtual SBSource* next () {return (SBSource*)(SBCatalog::next ());};
  
  
  /* functions: */
  virtual void prepareAliases4Output();
  virtual void releaseAliases4Output();
  virtual void collectObjAliases(QString);
  virtual void releaseObjAliases(QString);
  virtual SBSource* lookupAlias(const QString&);
  virtual SBSource* lookupNearest(double, double, double&);
  virtual void prepare4Run();
  virtual void finisRun();


   
  /*  i/o:  */
  friend QTextStream &operator<<(QTextStream&, const SB_CRF&);
  friend QTextStream &operator>>(QTextStream&, SB_CRF&);


  friend SBTS_dico   &operator<<(SBTS_dico&, SB_CRF&);
  friend SBTS_dico   &operator>>(SBTS_dico&, SB_CRF&);
  friend SBTS_rsc    &operator<<(SBTS_rsc&, const SB_CRF&);
  friend SBTS_rsc    &operator>>(SBTS_rsc&, SB_CRF&);
  friend SBTS_car    &operator<<(SBTS_car&, const SB_CRF&);
  friend SBTS_car    &operator>>(SBTS_car&, SB_CRF&);

  friend QDataStream &operator<<(QDataStream&, const SB_CRF&);
  friend QDataStream &operator>>(QDataStream&, SB_CRF&);

  friend SBTS_report    &operator<<(SBTS_report&, const SB_CRF&);
  friend SBTS_report4IVS&operator<<(SBTS_report4IVS&, const SB_CRF&);
};
/*==============================================================================================*/
class SBSourceIterator : public QListIterator<SBSource>
{
public:
  SBSourceIterator (const SB_CRF& CRF) 
    : QListIterator<SBSource>((const QList<SBSource> &)CRF){};
};
/*==============================================================================================*/








#endif //SB_GEO_SOURCES_H
