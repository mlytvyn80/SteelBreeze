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

#ifndef SB_GEO_H
#define SB_GEO_H

#include <config.h>
#include <qdatetime.h>
#include <qdict.h>
#include <qtextstream.h>

#include "SbGeneral.H"
#include "SbGeoTime.H"



#ifndef RAD2DEG
#define RAD2DEG		(180.0/M_PI)	/**< radians to degrees */
#endif
#ifndef RAD2HR
#define RAD2HR		(12.0 /M_PI)	/**< radians to hours */
#endif
#ifndef DEG2RAD
#define DEG2RAD		(M_PI/180.0)	/**< degrees to radians */
#endif
#ifndef HR2RAD
#define HR2RAD		(M_PI/ 12.0)	/**< hours to radians */
#endif
#ifndef SEC2RAD
#define SEC2RAD		(DEG2RAD/3600.0)/**< arc seconds to radians */
#endif
#ifndef RAD2SEC
#define RAD2SEC		(RAD2DEG*3600.0)/**< radians to arc seconds  */
#endif
#ifndef DAY2SEC
#define DAY2SEC		(86400.0)	/**< seconds in one day */
#endif
#ifndef RAD2MAS
#define RAD2MAS		(RAD2SEC*1000.0)/**< radians to mas */
#endif
#ifndef RAD2MS
#define RAD2MS		(RAD2HR*3600.0*1000.0)/**< radians to ms */
#endif


inline double sign(const double x) {return x<0.0?-1.0:1.0;};
inline double sign(const double a1, const double a2) {return fabs(a1)*sign(a2);}; // copysign(a1, a2)???

/**Converts Hour, Min and Sec to Radians. */
inline double hms2rad(int h, int m, double s)
{return fmod((double)((h + m/60.0 + s/3600.0)*HR2RAD), 2.0*M_PI);};
/**Converts Degree, Min and Sec to Radians. */
inline double dms2rad(int d, int m, double s)
{return drem((d + m/60.0 + s/3600.0)*DEG2RAD, 2.0*M_PI);};
/**Converts Radians to Hour, Min and Sec. */
void rad2hms(double, int&, int&, double&);
/**Converts Radians to Degree, Min and Sec. [-M_PI; M_PI]*/
void rad2dms(double, int&, int&, double&);
/**Converts Radians to Degree, Min and Sec. [0; 2.0*M_PI]*/
void rad2dmsl(double, int&, int&, double&);
/**Converts Radians to string with Hour, Min and Sec. */
QString	rad2hmsStr(double);
/**Converts Radians to string with Degree, Min and Sec. [-M_PI; M_PI]*/
QString	rad2dmsStr(double);
/**Converts Radians to string with Degree, Min and Sec. [0; 2.0*M_PI]*/
QString	rad2dmslStr(double);



class SBInstitutionList;
extern SBInstitutionList	*Institutions;


/*
  1995 IERS Annual Report (1996)
  page V-3 says:
  The data on Earth rotation and reference frames include series of the 
  Earth Orientation Parameters (EOP), Sets of Station Coordinates (SSC),
  and Radio Source Coordinates (RSC). They include also series of the 
  Atmospheric Angular Momentum (AAM). The data sets are labelled as folows.
  
  EOP(AAAAA)XX E YY		for Earth Orientation Parameters
  SSC(AAAAA)XX E YY		for Sets of Station Coordinates
  SSV(AAAAA)XX E YY		for Sets of Station Velocities
  RSC(AAAAA)XX E YY		for Radio Source Coordinates
  AAM(AAAAA)XX E YY		for Atmospheric Angular Momentum

enum SolutionType {TYPE_EOP, TYPE_SSC, TYPE_SSV, TYPE_RSC, TYPE_AAM, TYPE_UNKN};

  where AAAAA= acronym of the institute producing the results 
  (one to five letters)
  
  XX= year-1990 when the data set was computed
  E= the observing technique
  
  R:	Radio interferometry
  A:	optical astrometry
  M:	Lunar laser ranging
  P:	Global Positioning System
  L:	satellite laser ranging
  D:	satellite Doppler tracking
  C:	conventional geodesy (for SSC)
  *:	atmospheric data
  */

/**Known techniques.*/
enum Technique
{
  TECH_VLBI,			//!< R:	Radio interferometry;
  TECH_OA,			//!< A:	optical astrometry;
  TECH_LLR,			//!< M: Lunar laser ranging;
  TECH_GPS,			//!< P:	Global Positioning System;
  TECH_SLR,			//!< L:	satellite laser ranging;
  TECH_DORIS,			//!< D:	satellite Doppler tracking;
  TECH_COMBINED,		//!< C: combined data;
  TECH_CG,			//!< G:	conventional geodesy (for SSC)
  TECH_AD,			//!< *:	atmospheric data
  TECH_TIE,			//!< T: ties;
  TECH_UNKN,			//!< ?: uknown.
  TECH_ANY			//!< _: any above
};

/*
  YY= serial number of the data set in the year XX, for the technique E
  and the institute AAAAA.
 */



/*==============================================================================================*/
/*												*/
/* class Dated											*/
/*												*/
/*==============================================================================================*/
class SBDated
{
protected:
  double		Date;
  
public:
  SBDated(double d_=0.0) {Date=d_;};
  SBDated(const SBDated& d) {Date=d.Date;};
  virtual ~SBDated(){};
  virtual QString ClassName() const {return "SBDated";};

  virtual double date() const {return Date;};
  virtual void   setDate(double d_){Date=d_;};
  virtual QString date2str();
  
  inline friend int compare_dated(SBDated* d1, SBDated* d2)
    {return (d1->Date==d2->Date)?0:((d1->Date<d2->Date)?-1:1);};
  
  friend QDataStream &operator<<(QDataStream& s, const SBDated& D)	{return s<<D.Date;};
  friend QDataStream &operator>>(QDataStream& s, SBDated& D)		{return s>>D.Date;};
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBDList										*/
/*												*/
/*==============================================================================================*/
typedef QList<SBDated>		SBDoubleList;
typedef QListIterator<SBDated>	SBDIterator;

class SBDList : public SBDoubleList
{
protected:
  virtual int compareItems(QCollection::Item item1, QCollection::Item item2)
    {return compare_dated((SBDated*)item1, (SBDated*)item2);};  
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBNamed										*/
/*												*/
/*==============================================================================================*/
class SBNamed
{
protected:
  QString	Name;
public:
  SBNamed(const QString& Name_="NONAME"){Name=Name_;};
  SBNamed(const SBNamed& W) {Name=W.Name;};
  virtual ~SBNamed(){};
  virtual QString ClassName() const {return "SBNamed";};
  inline virtual SBNamed& operator= (const SBNamed& W) {Name=W.Name.copy(); return *this;};

  /* interface: */
  inline const QString& name() const {return Name;};
  inline void setName(const QString& Name_) {Name=Name_.copy();};

  /* functionality: */
  inline friend bool operator==(const SBNamed&, const SBNamed&);
  inline friend bool operator!=(const SBNamed&, const SBNamed&);
  inline friend bool operator<(const SBNamed&, const SBNamed&);
  inline friend bool operator>(const SBNamed&, const SBNamed&);
  inline friend int  cmp(const SBNamed&, const SBNamed&);

  /* I/O: */
  inline friend QDataStream &operator<<(QDataStream&, const SBNamed&);
  inline friend QDataStream &operator>>(QDataStream&, SBNamed&);
};

/**\relates SBNamed
 * Compares two instances of SBNamed.
 */
bool operator==(const SBNamed& N1, const SBNamed& N2){return (N1.name()==N2.name());};

/**\relates SBNamed
 * Compares two instances of SBNamed.
 */
bool operator!=(const SBNamed& N1, const SBNamed& N2){return (N1.name()!=N2.name());};

/**\relates SBNamed
 * Compares two instances of SBNamed.
 */
bool operator<(const SBNamed& N1, const SBNamed& N2) {return (N1.name()<N2.name());};

/**\relates SBNamed
 * Compares two instances of SBNamed.
 */
bool operator>(const SBNamed& N1, const SBNamed& N2) {return (N1.name()>N2.name());};

/**\relates SBNamed
 * Compares two instances of SBNamed, returns (-1:0:+1).
 */
int cmp(const SBNamed& N1, const SBNamed& N2) {return N1<N2?-1:N1==N2?0:1;};

/**\relates SBNamed
 * Saves object to the data stream.
 */
QDataStream &operator<<(QDataStream& s, const SBNamed& W){return s << W.name();};

/**\relates SBNamed
 * Loads object from the data stream.
 */
QDataStream &operator>>(QDataStream& s, SBNamed& W)      {return s >> W.Name;};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBNList										*/
/*												*/
/*==============================================================================================*/
typedef QListIterator<SBNamed>	SBNIterator;
class SBNList : public QList<SBNamed>
{
protected:
  virtual int compareItems(QCollection::Item item1, QCollection::Item item2)
    {
      if (*(SBNamed*)item1==*(SBNamed*)item2) return 0; 
      return (*(SBNamed*)item1 < *(SBNamed*)item2)?-1:1;
    };
  virtual QDataStream& write(QDataStream& s, QCollection::Item Entry) const
    {return s << *(SBNamed*)Entry;};
  virtual QDataStream& read(QDataStream& s, QCollection::Item &Entry)
    {Entry = new SBNamed; return s >> *(SBNamed*)Entry;};
};
/*==============================================================================================*/



/*==============================================================================================*/
/**SBInstitution.
 *
 */
/*==============================================================================================*/
class SBInstitution : public SBNamed
{
private:
  QString	Descr;
public:
  /** A constructor. */
  SBInstitution(const QString& Name_="NONE", const QString& Descr_="unknown")
    : SBNamed(Name_) {Descr=Descr_.copy();};
  SBInstitution(const SBInstitution& I) 
    : SBNamed(I) {Descr=I.Descr.copy();};
  /** A destructor. */
  virtual ~SBInstitution(){};
  /** Refers to a class name (debug info) */
  virtual QString ClassName() const {return "SBInstitution";};

  // interface:
  const QString&	descr() const {return Descr;};
  void			setDescr(const QString& D_){Descr=D_.copy();};

  /* I/O: */
  friend QDataStream &operator<<(QDataStream& s, const SBInstitution& I)
    {return s << (const SBNamed&)I << I.Descr;};
  friend QDataStream &operator>>(QDataStream& s, SBInstitution& I)
    {return s >> (SBNamed&)I >> I.Descr;};
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBInstitutionList									*/
/*												*/
/*==============================================================================================*/
class SBInstitutionList : public SBNList
{
protected:
  virtual 
  QDataStream& write (QDataStream& s, QCollection::Item Entry) const
    {return s << *(SBInstitution*)Entry;};
  virtual QDataStream& read(QDataStream& s, QCollection::Item &Entry)
    {Entry = new SBInstitution; return s >> *(SBInstitution*)Entry;};
  QDict<SBInstitution>	*InstByAcronym;
  bool			IsChanged;
public:
  SBInstitutionList();
  ~SBInstitutionList();
  virtual SBInstitutionList& operator= (const SBInstitutionList&);
  virtual QString ClassName() const {return "SBInstitutionList";};
  virtual SBInstitution	*find(const QString& A_){return InstByAcronym->find(A_);};
  virtual bool		insert(const QString&, const QString&);
  virtual bool		insert(const SBInstitution*);
  virtual bool		remove(const SBInstitution*);
  virtual bool		remove(const QString&);
  bool			isChanged() const {return IsChanged;};
  virtual QString descr(const QString& A_) const
    {SBInstitution *W=InstByAcronym->find(A_); return W?W->descr():QString("missed entry");};
  friend QDataStream &operator<<(QDataStream&, const SBInstitutionList&);
  friend QDataStream &operator>>(QDataStream&, SBInstitutionList&);
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBSolutionName										*/
/*												*/
/*==============================================================================================*/
class SBSolutionName
{
public:
  /*
    1995 IERS Annual Report (1996)
    page V-3 says:
    The data on Earth rotation and reference frames include series of the 
    Earth Orientation Parameters (EOP), Sets of Station Coordinates (SSC),
    and Radio Source Coordinates (RSC). They include also series of the 
    Atmospheric Angular Momentum (AAM). The data sets are labelled as folows.
    
    EOP(AAAAA)XX E YY		for Earth Orientation Parameters
    SSC(AAAAA)XX E YY		for Sets of Station Coordinates
    SSV(AAAAA)XX E YY		for Sets of Station Velocities
    RSC(AAAAA)XX E YY		for Radio Source Coordinates
    AAM(AAAAA)XX E YY		for Atmospheric Angular Momentum
  */
  enum SNType {TYPE_EOP, TYPE_SSC, TYPE_SSV, TYPE_RSC, TYPE_AAM, TYPE_UNKN};
  /*
    where AAAAA= acronym of the institute producing the results 
    (one to five letters)
    
    XX= year-1990 when the data set was computed
    E= the observing technique
    
    R:	Radio interferometry
    A:	optical astrometry
    M:	Lunar laser ranging
    P:	Global Positioning System
    L:	satellite laser ranging
    D:	satellite Doppler tracking
    C:	conventional geodesy (for SSC)
    *:	atmospheric data
    
    YY= serial number of the data set in the year XX, for the technique E
    and the institute AAAAA.
  */
  
protected:
  SNType		Type;
  Technique		Tech;
  QString		Acronym;
  int			Year;
  int			Serial;
  
public:
  static const QString TypeNames[];
  static const QString TechNames[];
  static const QString Types[];
  static const QString Techs[];
  static const QString TechAbbrev[];
  
  // constructors/destructors:
  //
  SBSolutionName(SNType Type_=TYPE_UNKN) {setAttr(Type_, "UNKN");};
  SBSolutionName(const SBSolutionName& Solution)
    {setAttr(Solution.Type, Solution.Acronym, Solution.Year,
	     Solution.Tech, Solution.Serial);};
  SBSolutionName(SNType Type_, const QString& Acronym_,
		 int Year_=0, Technique Tech_=TECH_VLBI, int Serial_=1)
    {setAttr(Type_, Acronym_, Year_, Tech_, Serial_);}
  
  virtual ~SBSolutionName(){};
  
  // interface:
  //
  SNType		type   () const {return Type;};
  Technique		tech   () const {return Tech;};
  QString		acronym() const {return Acronym;};
  int			year   () const {return Year;};
  int			serial () const {return Serial;};
  void			setType   (SNType Type_)  {Type	= Type_;};
  void			setTech   (Technique Tech_)	{Tech	= Tech_;};
  void			setAcronym(const QString& Acronym_){Acronym= Acronym_.copy();};
  void			setYear   (int Year_)           {Year	= Year_;};
  void			setSerial (int Serial_)         {Serial	= Serial_;};
  void			setAttr   (SNType Type_, const QString& Acronym_,
				   int Year_=0, Technique Tech_=TECH_VLBI, int Serial_=1)
    {
      Type	= Type_;
      Tech	= Tech_;
      Acronym	= Acronym_.copy();
      Year	= Year_?Year_:QDate::currentDate().year();
      Serial	= Serial_;      
    };
  
  
  // functions:
  //
  const QString		Type2Str   () const {return (-1<Type && Type<6)?TypeNames[Type]:TypeNames[5];};
  const QString		Tech2Str   () const {return (-1<Tech && Tech<11)?TechNames[Tech]:TechNames[10];};
  const QString		Acronym2Str() const {return Institutions->descr(Acronym);};
  static const QString& techAbbrev(Technique T) {return (-1<T && T<11)?TechAbbrev[T]:TechAbbrev[10];};
  static const QString& techName(Technique T){return (-1<T && T<11)?TechNames[T]:TechNames[10];};
  QString toString() const;
  void			fromString (const QString&);
  bool			operator== (const SBSolutionName& label)
    {return  (Type == label.Type) && (Tech == label.Tech) &&
       (Acronym == label.Acronym) && (Year == label.Year) && (Serial == label.Serial);	     
    };
  bool			operator!=(const SBSolutionName& label) {return !(*this==label);};

  // I/O:
  //
  friend QDataStream &operator<<(QDataStream&, const SBSolutionName&);
  friend QDataStream &operator>>(QDataStream&, SBSolutionName&);
  friend QTextStream &operator<<(QTextStream&, const SBSolutionName&);
  friend QTextStream &operator>>(QTextStream&, SBSolutionName&);  
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBCatalog										*/
/*												*/
/*==============================================================================================*/
class SBCatalog : public SBNList
{
protected:
  SBSolutionName	Label;
  SBMJD			Epoch;
  QString		Comment;
  QDict<SBNamed>	*Dict;		// dictionary of items (for lookups)
  QDict<QString>	*Aliases;	// dictionary of aliases (alternate lookup)
public:
  SBCatalog();
  SBCatalog(const SBCatalog& Catalog);
  virtual ~SBCatalog(){if (Dict) delete Dict; if (Aliases) delete Aliases;};
  virtual QString ClassName() const {return "SBCatalog";};
  
  /*  interface:  */
  void	setLabel  (const SBSolutionName& Label_){Label  = Label_;};
  void	setEpoch  (const SBMJD& Epoch_)		{Epoch  = Epoch_;};
  void	setComment(const QString& Comment_)	{Comment= Comment_;};
  const SBSolutionName& label  () const		{return Label;};
  const SBMJD&		epoch  () const		{return Epoch;};
  const QString&	comment() const		{return Comment;};
  QDict<QString>*	aliasDict(){return Aliases;};


  /* functionality: */
  virtual void prepareAliases4Output(){};
  virtual void releaseAliases4Output(){};
  virtual void collectObjAliases(QString){};
  virtual void releaseObjAliases(QString){};
  virtual void prepareReferences(){};
  virtual SBNList* referenceList(){return this;};

  /*  virtual void append(const SBNamed*)=0;*/
  virtual void clear() {Dict->clear(); SBNList::clear();};
  virtual SBNamed* find(const SBNamed& N) const {return Dict->find(N.name());};
  virtual SBNamed* find(const QString& S) const {return Dict->find(S);};
  virtual SBNamed* operator[](const SBNamed& N)	const {return find(N);};
  virtual SBNamed* operator[](const QString& S)	const {return find(S);};
  virtual void insert(const SBNamed* N){SBNList::inSort(N); Dict->insert(N->name(), N);};
  virtual void inSort(const SBNamed* N){SBNList::inSort(N); Dict->insert(N->name(), N);};
  virtual bool remove(){Dict->remove(current()->name()); return SBNList::remove();};
  virtual bool remove(uint i){Dict->remove(at(i)->name()); return SBNList::remove(i);};
  virtual bool remove(const SBNamed* N){Dict->remove(N->name()); return SBNList::remove(N);};
  virtual bool removeFirst(){Dict->remove(getFirst()->name()); return SBNList::removeFirst();};
  virtual bool removeLast(){Dict->remove(getLast()->name()); return SBNList::removeLast();};
  virtual SBNamed* take(){Dict->take(current()->name()); return SBNList::take();};
  virtual SBNamed* take(uint i){Dict->take(at(i)->name()); return SBNList::take(i);};

  /*  i/o:  */
  friend QDataStream &operator<<(QDataStream&, const SBCatalog&);
  friend QDataStream &operator>>(QDataStream&, SBCatalog&);
};
/*==============================================================================================*/









#endif	//SB_GEO_H
