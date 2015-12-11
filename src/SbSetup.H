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

#ifndef SB_SETUP_H
#define SB_SETUP_H

#include <config.h>
#include <qdatastream.h>
#include <qrect.h>
#include <qfont.h>
#include "SbGeneral.H"
#include "SbGeo.H"




class SBSetUp;
extern SBSetUp* SetUp;


class QFile;
class QFileInfo;
class QTextStream;
/*==============================================================================================*/
/**SBFileConv.
 *
 */
/*==============================================================================================*/
class SBFileConv : public SBNamed
{
  friend class SBFCList;
private:
  QString	ComCompr;
  QString	ComUncom;
  QString	Ext;
public:
  /** A constructor. */
  SBFileConv(const QString& Name_="NONE", const QString& ComC_="cat", const QString& ComU_="cat",
	     const QString& Ext_=" ") : SBNamed(Name_)
    {ComCompr=ComC_.copy(); ComUncom=ComU_.copy(); Ext=Ext_.copy();};
  SBFileConv(const SBFileConv& W) : SBNamed(W) 
    {ComCompr=W.ComCompr.copy(); ComUncom=W.ComUncom.copy(); Ext=W.Ext.copy();};
  virtual SBFileConv& operator= (const SBFileConv& W) 
    {
      (SBNamed&)*this=(const SBNamed&)W; 
      ComCompr=W.ComCompr.copy(); 
      ComUncom=W.ComUncom.copy(); 
      Ext=W.Ext.copy();
      return *this;
    };

  /** A destructor. */
  virtual ~SBFileConv(){};
  /** Refers to a class name (debug info) */
  virtual QString ClassName() const {return "SBFileConv";};

  // interface:
  const QString&	comCompr() const {return ComCompr;};
  const QString&	comUncom() const {return ComUncom;};
  const QString&	ext()	   const {return Ext;};
  void setComCompr(const QString& C_){ComCompr=C_.copy();};
  void setComUncom(const QString& C_){ComUncom=C_.copy();};
  void setExt     (const QString& C_){Ext=C_.copy();};

  // functions:
  FILE*		open4In(const QString&, QFile&, QDataStream&);
  FILE*		open4In(const QString&, QFile&, QTextStream&);
  FILE*		open4Out(const QString&, QFile&, QDataStream&);
  FILE*		open4Out(const QString&, QFile&, QTextStream&);
  static	void close(QFile&, FILE*&, QDataStream&);
  static	void close(QFile&, FILE*&, QTextStream&);
  bool		check();
  
  /* I/O: */
  friend QDataStream &operator<<(QDataStream& s, const SBFileConv& F)
    {return s << (const SBNamed&)F << F.ComCompr << F.ComUncom << F.Ext;};
  friend QDataStream &operator>>(QDataStream& s, SBFileConv& F)
    {return s >> (SBNamed&)F >> F.ComCompr >> F.ComUncom >> F.Ext;};
};
/*==============================================================================================*/



/*==============================================================================================*/
/**SBFCList, a list of SBFileConv`s.
 *
 */
/*==============================================================================================*/
class SBFCList : public SBNList
{
protected:
  virtual QDataStream& write(QDataStream& s, QCollection::Item Entry) const
    {return s << *(SBFileConv*)Entry;};
  virtual QDataStream& read(QDataStream& s, QCollection::Item &Entry)
    {Entry = new SBFileConv; return s >> *(SBFileConv*)Entry;};
private:
  QDict<SBFileConv>	*ExtDict;		// dictionary of items (for lookups)
  SBFileConv		*DefaultFC;
  QString		Default;
public:
  /** A constructor. */
  SBFCList();
  SBFCList(const SBFCList&);
  virtual SBFCList& operator= (const SBFCList&);
  /** A destructor. */
  virtual ~SBFCList();
  /** Refers to a class name (debug info) */
  virtual QString ClassName() const {return "SBFCList";};

  // functions:
  /**Lookups a type, returns NULL if failed. */
  SBFileConv*	guessFileType(const QString&);
  SBFileConv*	defaultFC(){return DefaultFC;};
  QString	defaultName(){return Default;};
  QString	defaultExt(){return DefaultFC? "."+DefaultFC->ext(): QString("");};
  void		setDefault(const QString&);
  FILE*		open4In(const QString&, QFile&, QDataStream&);
  FILE*		open4In(const QString&, QFile&, QTextStream&);
  FILE*		open4Out(const QString&, QFile&, QDataStream&);
  FILE*		open4Out(const QString&, QFile&, QTextStream&);
  FILE*		open4OutDflt(const QString&, QFile&, QDataStream&);
  FILE*		open4OutDflt(const QString&, QFile&, QTextStream&);
  void		close(QFile& file, FILE*& p, QDataStream& ds) {SBFileConv::close(file, p, ds);};
  void		close(QFile& file, FILE*& p, QTextStream& ts) {SBFileConv::close(file, p, ts);};
  
  virtual void clear(){ExtDict->clear(); SBNList::clear();};
  virtual bool insert(const SBFileConv*);
  virtual bool remove(const SBFileConv* N){ExtDict->remove(N->ext()); return SBNList::remove(N);};

  /* I/O: */
  friend QDataStream &operator<<(QDataStream&, const SBFCList&);
  friend QDataStream &operator>>(QDataStream&, SBFCList&);
};
/*==============================================================================================*/



class SBEOP;
class SB_CRF;
class SB_TRF;
class SBVLBISet;
class SBProject;
class SBAploEphem;
class SBObsVLBIStatistics;
/*==============================================================================================*/
/*												*/
/* class SBSetUp										*/
/*												*/
/*==============================================================================================*/
class SBSetUp
{
public:
  enum DUnits {U_SEC=0, U_METER=1, U_INCH=2};
  enum GUIStyle {S_Default=0, S_MotifPlus=1, S_CDE=2, S_SGI=3, S_Interlace=4, S_Windows=5, S_Platinum=6};

private:
  static const char* sInstitutions;

  /* file name of the setup: */
  QString		SetUpFName;
  bool			HaveToSave;
  

  /*         we need to store these:       */
  /*---------------  v2.0.1  --------------*/
  /*      directories:                     */
  QString		HomeDir;
  QString		DataDir;
  QString		CompileDir;
  QString		StuffDir;
  QString		OutputDir;
  QString		SolutionsDir;
  QString		ProjectDir;
  QString		LogDir;
  QString		WorkDir;
  QString		ImportDir;
  QString		ImportEOPDir;
  QString		ImportITRFDir;
  QString		ImportICRFDir;
  QString		ImportOLoadDir;
  QString		ImportMFilesDir;
  QString		ImportAploDir;
  /*      file names:                      */
  QString		Stations;
  QString		Sources;
  QString		EOPs;
  QString		Ephem;
  QString		Project;


  /*---------------  v2.0.2  --------------*/
  /*     SBLogger:                      */
  int			LogStoreInFile;
  int			LogTimeLabel;
  QString		LogFileName;
  uint			LogFacilities[4];
  uint			LogCapacity;
  /*     SBCentarlPanel:                */
  QPoint		CentralPanelPosition;
  QSize			CentralPanelSize;
  GUIStyle		GUI_Style;
  QFont			DefaultFont;
  /*     Units:                         */
  DUnits		Units;
  /*     Compress/decompress:           */
  SBFCList		FCL;


  /* functions: */
  void			CheckDir(const QString, bool=TRUE);
  
public:
  SBSetUp();
  SBSetUp(const SBSetUp&);
  virtual ~SBSetUp();
  virtual QString ClassName() const {return "SBSetUp";};
  
  virtual void SetDefaults();
  virtual void PeekUpOldConfig(QDataStream&);
  virtual void CheckConfig();
  
  virtual void SaveConfig();
  virtual void SaveStuff();
  
  virtual void LoadConfig();
  virtual void LoadStuff();
  
  virtual const SBSetUp& operator=(const SBSetUp&);
  friend QDataStream &operator<<(QDataStream&, const SBSetUp&);
  friend QDataStream &operator>>(QDataStream&, SBSetUp&);
  
  
  /*  for data exange: */  
  virtual const QString&	homeDir() const {return HomeDir;};
  virtual const QString&	dataDir() const {return DataDir;};
  virtual const QString&	compileDir() const {return CompileDir;};
  virtual const QString&	stuffDir() const {return StuffDir;};
  virtual const QString&	outputDir() const {return OutputDir;};
  virtual const QString&	solutionsDir() const {return SolutionsDir;};
  virtual const QString&	projectDir() const {return ProjectDir;};
  virtual const QString&	logDir() const {return LogDir;};
  virtual const QString&	workDir() const {return WorkDir;};
  virtual const QString&	importDir() const {return ImportDir;};
  virtual const QString&	importEOPDir() const {return ImportEOPDir;};
  virtual const QString&	importITRFDir() const {return ImportITRFDir;};
  virtual const QString&	importICRFDir() const {return ImportICRFDir;};
  virtual const QString&	importOLoadDir() const {return ImportOLoadDir;};
  virtual const QString&	importMFilesDir() const {return ImportMFilesDir;};
  virtual const QString&	importAploDir() const {return ImportAploDir;};
  virtual const QString&	stations() const {return Stations;};
  virtual const QString&	sources() const {return Sources;};
  virtual const QString&	eops() const {return EOPs;};
  virtual const QString&	ephem() const {return Ephem;};
  virtual const QString&	project() const {return Project;};
  virtual	int		logStoreInFile() const {return LogStoreInFile;};
  virtual	int		logTimeLabel() const {return LogTimeLabel;};
  virtual const QString&	logFileName()  const {return LogFileName;};
  virtual	uint		logFacilities(SBLog::LogLevel _) const {return LogFacilities[_];};
  virtual	uint		logCapacity() const {return LogCapacity;};
  virtual const QPoint&		centralPanelPosition() const {return CentralPanelPosition;};
  virtual const QSize&		centralPanelSize() const {return CentralPanelSize;};
  virtual const QFont&		defaultFont() const {return DefaultFont;};
  virtual	GUIStyle	guiStyle() const {return GUI_Style;};
  virtual	DUnits		units() const {return Units;};
  virtual SBFCList*		fcList() {return &FCL;};
  
  virtual void setHomeDir	(const QString& S) {HomeDir=S.copy();};
  virtual void setDataDir	(const QString& S) {DataDir=S.copy();};
  virtual void setCompileDir	(const QString& S) {CompileDir=S.copy();};
  virtual void setStuffDir	(const QString& S) {StuffDir=S.copy();};
  virtual void setOutputDir	(const QString& S) {OutputDir=S.copy();};
  virtual void setSolutionsDir	(const QString& S) {SolutionsDir=S.copy();};
  virtual void setProjectDir	(const QString& S) {ProjectDir=S.copy();};
  virtual void setLogDir	(const QString& S) {LogDir=S.copy();};
  virtual void setWorkDir	(const QString& S) {WorkDir=S.copy();};
  virtual void setImportDir	(const QString& S) {ImportDir=S.copy();};
  virtual void setImportEOPDir	(const QString& S) {ImportEOPDir=S.copy();};
  virtual void setImportITRFDir	(const QString& S) {ImportITRFDir=S.copy();};
  virtual void setImportICRFDir	(const QString& S) {ImportICRFDir=S.copy();};
  virtual void setImportOLoadDir(const QString& S) {ImportOLoadDir=S.copy();};
  virtual void setImportMFilesDir(const QString& S) {ImportMFilesDir=S.copy();};
  virtual void setImportAploDir (const QString& S) {ImportAploDir=S.copy();};
  virtual void setStations	(const QString& S) {mainSetTRFName(Stations=S.copy());};
  virtual void setSources	(const QString& S) {mainSetCRFName(Sources=S.copy());};
  virtual void setEOPs		(const QString& S) {mainSetEOPName(EOPs=S.copy());};
  virtual void setEphem		(const QString& S) {Ephem=S.copy();};
  virtual void setProject	(const QString& S) {mainSetPrjName(Project=S.copy());};
  virtual void setLogStoreInFile(const int V)	   {LogStoreInFile=V;};
  virtual void setLogTimeLabel	(const int V)      {LogTimeLabel=V;};
  virtual void setLogFileName	(const QString& S) {LogFileName=S.copy();};
  virtual void setLogFacilities (SBLog::LogLevel Lvl, const uint V) {LogFacilities[Lvl]=V;};
  virtual void setLogCapacity	(const uint V)     {LogCapacity=V;};
  virtual void setCentralPanelPosition(const QPoint& P) {CentralPanelPosition=P;};
  virtual void setCentralPanelSize(const QSize& S) {CentralPanelSize=S;};
  virtual void setDefaultFont   (const QFont& F)   {DefaultFont=F;};
  virtual void setGUIStyle      (const GUIStyle S) {GUI_Style=S;};
  virtual void setUnits		(const DUnits U)   {Units=U;};
  
  
  

  /*-- our clients --*/
  virtual void UpdateLogger();


  /*-- real functions --*/
  virtual const QString path2Home() const {return HomeDir;};
  virtual const QString path2Data() const;
  virtual const QString path2Log() const;
  virtual const QString path2Compil() const;
  virtual const QString path2Stuff() const;
  virtual const QString path2StuffAplo() const {return path2Stuff() + "Aplo/";};
  virtual const QString path2StuffObsStat() const {return path2Stuff() + "ObsStat/";};
  virtual const QString path2UIDump() const;
  virtual const QString path2Output() const;
  virtual const QString path2Sols(const QString&) const;
  virtual const QString path2Proj() const;
  virtual const QString path2Work() const;
  virtual const QString path2Import() const;
  virtual const QString path2ImpEOP() const;
  virtual const QString path2ImpITRF() const;
  virtual const QString path2ImpICRF() const;
  virtual const QString path2ImpOLoad() const;
  virtual const QString path2ImpMFiles() const;
  virtual const QString path2ImpAplo() const;
  virtual const QString EOPfileName() const {return path2Stuff() + EOPs + ".bin";};
  virtual const QString EphemfileName() const {return path2Stuff() + Ephem + ".bin";};
  virtual const QString CRFfileName() const {return path2Stuff() + Sources + ".bin";};
  virtual const QString TRFfileName() const {return path2Stuff() + Stations + ".bin";};
  virtual const QString AploFileName() const {return path2Stuff() + "aplo.bin";};
  virtual const QString VLBIfileName() const {return path2Stuff() + "vlbi.bin";};
  virtual const QString modelsFileName() const {return path2Stuff() + "models.bin";};
  virtual const QString masterFileName() const {return path2Stuff() + "masters.bin";};
  virtual const QString ProjectFileName() const {return path2Proj() + Project;};
  //  virtual const QString SolutionFileName() const {return path2Sols() + Project;};
  virtual const QString ObsStatFileName(bool IsGlb) const 
    {return path2StuffObsStat() + (IsGlb?"VLBI.bin":"Projects/" + Project + ".bin");};

  const		QString path2TimeSeries() const {return HomeDir + "/TimeSeries/";};

  double		scale() const 
    {
      double s=1.0;
      switch (Units)
	{
	default:
	case U_SEC:   s = 1.0e+12;		break;
	case U_METER: s = 100.0*299792458.0;	break;
	case U_INCH:  s = 25.4*299792458.0;	break;
	};
      return s;
    };
  QString		scaleName() const 
    {
      QString sn="lapot";
      switch (Units)
	{
	default:
	case U_SEC:   sn = "(ps)";	break;
	case U_METER: sn = "(cm)";	break;
	case U_INCH:  sn = "(in)";	break;
	};
      return sn;
    };


  /*-- stored in stuff dir: --*/
  virtual const QString	InstitutionsFileName() {return path2Stuff() + sInstitutions;};

  SBEOP* loadEOP(SBEOP* =NULL);
  void saveEOP(SBEOP*);
  SB_CRF* loadCRF(SB_CRF* =NULL);
  void saveCRF(SB_CRF*);
  SB_TRF* loadTRF(SB_TRF* =NULL);
  void saveTRF(SB_TRF*);
  SBVLBISet* loadVLBI(SBVLBISet* =NULL);
  void saveVLBI(SBVLBISet*);
  SBProject* loadProject(SBProject* =NULL);
  void saveProject(SBProject*);
  SBAploEphem* loadAploEphem(SBAploEphem* =NULL);
  void saveAploEphem(SBAploEphem*);
  SBObsVLBIStatistics* loadObsVLBIStatistics(bool);
  void saveObsVLBIStatistics(SBObsVLBIStatistics*);
};
/*==============================================================================================*/








#endif //SB_SETUP_H
