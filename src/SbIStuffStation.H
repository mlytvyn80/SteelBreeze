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
 * Overview:	GUI for Site/Station;
 *
 */

#ifndef SB_I_STUFF_STATION_H
#define SB_I_STUFF_STATION_H


#include <config.h>
#include <qdialog.h>
#include <qlistview.h>
#include <qtabdialog.h>
#include <qwizard.h>


#include "SbGeneral.H"
#include "SbInterface.H"
#include "SbGeoStations.H"

class QCheckBox;
class QComboBox;
class QLineEdit;
class QMultiLineEdit;
class QBoxLayout;
class QGroupBox;
class QLabel;
class QRadioButton;




/*==============================================================================================*/
/*												*/
/* SBStationListItem..										*/
/*												*/
/*==============================================================================================*/
class SBStationListItem : public QListViewItem
{
private:
  SBStation	*Station;
  bool		isFaked;
public:
  SBStationListItem(QListView*, SBStation*);
  ~SBStationListItem();
  virtual QString ClassName() const {return "SBStationListItem";};
  virtual QString text(int) const;
  virtual QString key(int, bool) const;
  //  const QPixmap * pixmap (int) const;
  //  friend class SBStuffStations;
  friend class SBSiteEditor;
  friend class SBStationEditor;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBSiteListItem..										*/
/*												*/
/*==============================================================================================*/
class SBSiteListItem : public QListViewItem
{
private:
  SBSite	*Site;
  bool		isFaked;
public:
  SBSiteListItem(QListView*, SBSite*);
  ~SBSiteListItem();
  virtual QString ClassName() const {return "SBSiteListItem";};
  virtual QString text(int) const;
  virtual QString key(int, bool) const;
  //  const QPixmap * pixmap (int) const;
  friend class SBStuffStations;
  friend class SBSiteEditor;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBEccListItem..										*/
/*												*/
/*==============================================================================================*/
class SBEccListItem : public QListViewItem
{
private:
  SBStationEcc*		StEcc;
  SBEccentricity*	Ecc;
public:
  SBEccListItem(QListView*, SBStationEcc*, SBEccentricity*);
  virtual QString ClassName() const {return "SBEccListItem";};
  virtual QString text(int) const;
  virtual QString key(int, bool) const;

  SBStationEcc*		stEcc() {return StEcc;};
  SBEccentricity*	ecc() {return Ecc;};

  //  const QPixmap * pixmap (int) const;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBCoordsListItem..										*/
/*												*/
/*==============================================================================================*/
class SBCoordsListItem : public QListViewItem
{
private:
  SBCoordinates*		Coords;
public:
  SBCoordsListItem(QListView* parent_, SBCoordinates* Coords_) : QListViewItem(parent_){Coords=Coords_;};
  virtual QString ClassName() const {return "SBCoordsListItem";};

  SBCoordinates*	coords() {return Coords;};

  virtual QString text(int) const;
  virtual QString key(int, bool) const;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBStuffStations..										*/
/*												*/
/*==============================================================================================*/
class SBStuffStations : public QDialog
{
  Q_OBJECT    
    public:
  SBStuffStations(QWidget* =0, const char* =0, bool =FALSE, WFlags =0);
  ~SBStuffStations();
  
  virtual QString ClassName() const {return "SBStuffStations";};
  
  private 
    slots:
  void importData();
  void exportData();

  void accept();
  void reject();
  void reloadTRF();

  void editEntryA(QListViewItem*);
  void insertEntryA();
  void deleteEntryA();
  void editEntryS(QListViewItem*);
  void insertEntryS();
  void deleteEntryS();
  void browseEcc();

  void TRFModified(bool);


 private:
  SB_TRF	*Wtrf;
  QString	str;
  
  QListView	*SitesLV;
  QListView	*AliasesLV;

  QLineEdit	*TRFEpoch;
  QLineEdit	*TRFLabel;
  QLineEdit	*TRFComment;
  QLabel	*NumSA;
  bool		isModifiedTRF;
  int		numOpenedWindows;

  void drawInfo();
  
 protected:
  virtual void closeEvent(QCloseEvent *e)
    {if (numOpenedWindows) e->ignore(); else delete this;};
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBStuffStationsEcc..										*/
/*												*/
/*==============================================================================================*/
class SBStuffStationsEcc : public QDialog
{
  Q_OBJECT    
public:
  SBStuffStationsEcc(SB_TRF*, QWidget* =0, const char* =0, bool =FALSE, WFlags =0);
  ~SBStuffStationsEcc();
  
  virtual QString ClassName() const {return "SBStuffStationsEcc";};
  
  
  signals:
  void eccModified(bool);
  
  private 
  slots:
  void importData();
  void exportData();
  
  void accept();
  void reject();
  
  void editEntry(QListViewItem*);
  void insertEntry();
  void deleteEntry();
  
private:
  SBEcc		*Ecc;
  SB_TRF	*Wtrf;
  
  QListView	*EccLV;
  
  bool		isModified;
  int		numOpenedWindows;
  
protected:
  virtual void closeEvent(QCloseEvent *e)
    {if (numOpenedWindows) e->ignore(); else delete this;};
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBStationImport..										*/
/*												*/
/*==============================================================================================*/
class SBStationImport : public QWizard
{
  Q_OBJECT    
public:
  SBStationImport(QWidget* =0, const char* =0, bool =FALSE, WFlags =0);
  virtual QString	ClassName() const {return "SBStationImport";};
  
private:
  enum		FileFormat  {F_UNKN=0, F_ITRF94=1, F_ITRF96=2, F_ITRF97=3, F_ITRF2000=4, F_SINEX=5};
  enum		FileType    {SITE=0, DIR=1, SSC_Z=2, DORIS=2, SSC_C=3, GPS=3, SSC_B=4, SLR=4,
			     SSC_A=5, VLBI=5, BLQ=6};
  enum		ImportMode  {REPLACE=0, APP_ONLY=1, APP_UPD=2, UPD_ONLY=3};
  enum		ImportFilter{FILT_COO=0, FILT_TECH=1, FILT_VLBI=2, FILT_ALL=3};
  
  signals:
  void stationsImported();
  
  private 
  slots:
  void		accept();

  // page 1:
  void		formatChanged(int);

  // page 2_1:
  void selectDIRfile1994 ()	{selectFile1994(  DIR);};
  void selectSITEfile1994()	{selectFile1994( SITE);};
  void selectSSCAfile1994()    	{selectFile1994(SSC_A);};
  void selectSSCBfile1994()    	{selectFile1994(SSC_B);};
  void selectSSCCfile1994()    	{selectFile1994(SSC_C);};
  void selectSSCZfile1994()    	{selectFile1994(SSC_Z);};

  void setDIRstate1994 (bool T)	{setState1994(  DIR, T);};
  void setSITEstate1994(bool T)	{setState1994( SITE, T);};
  void setSSCAstate1994(bool T)	{setState1994(SSC_A, T);};
  void setSSCBstate1994(bool T)	{setState1994(SSC_B, T);};
  void setSSCCstate1994(bool T)	{setState1994(SSC_C, T);};
  void setSSCZstate1994(bool T)	{setState1994(SSC_Z, T);};

  void fnDIRChanged1994  (const QString& FN_){fileNameChanged1994(DIR,   FN_);};
  void fnSITEChanged1994 (const QString& FN_){fileNameChanged1994(SITE,  FN_);};
  void fnSSCAChanged1994 (const QString& FN_){fileNameChanged1994(SSC_A, FN_);};
  void fnSSCBChanged1994 (const QString& FN_){fileNameChanged1994(SSC_B, FN_);};
  void fnSSCCChanged1994 (const QString& FN_){fileNameChanged1994(SSC_C, FN_);};
  void fnSSCZChanged1994 (const QString& FN_){fileNameChanged1994(SSC_Z, FN_);};


  // page 2_2:
  void selectDIRfile2000 ()	{selectFile2000(  DIR);};
  void selectSITEfile2000()	{selectFile2000( SITE);};
  void selectSSCAfile2000()    	{selectFile2000(SSC_A);};
  void selectSSCBfile2000()    	{selectFile2000(SSC_B);};
  void selectSSCCfile2000()    	{selectFile2000(SSC_C);};
  void selectSSCZfile2000()    	{selectFile2000(SSC_Z);};

  void setDIRstate2000 (bool T)	{setState2000(  DIR, T);};
  void setSITEstate2000(bool T)	{setState2000( SITE, T);};
  void setSSCAstate2000(bool T)	{setState2000(SSC_A, T);};
  void setSSCBstate2000(bool T)	{setState2000(SSC_B, T);};
  void setSSCCstate2000(bool T)	{setState2000(SSC_C, T);};
  void setSSCZstate2000(bool T)	{setState2000(SSC_Z, T);};

  void fnDIRChanged2000  (const QString& FN_){fileNameChanged2000(DIR,   FN_);};
  void fnSITEChanged2000 (const QString& FN_){fileNameChanged2000(SITE,  FN_);};
  void fnSSCAChanged2000 (const QString& FN_){fileNameChanged2000(SSC_A, FN_);};
  void fnSSCBChanged2000 (const QString& FN_){fileNameChanged2000(SSC_B, FN_);};
  void fnSSCCChanged2000 (const QString& FN_){fileNameChanged2000(SSC_C, FN_);};
  void fnSSCZChanged2000 (const QString& FN_){fileNameChanged2000(SSC_Z, FN_);};

  // page 3:
  void selectNSCfileAux()	{selectFileAux (0);};
  void selectECCfileAux()	{selectFileAux (1);};
  void selectBLQfileAux()	{selectFileAux (2);};

  void setNSCstateAux  (bool T)	{setStateAux(0, T);};
  void setECCstateAux  (bool T)	{setStateAux(1, T);};
  void setBLQstateAux  (bool T)	{setStateAux(2, T);};

  void fnNSCChangedAux (const QString& FN_){fileNameChangedAux(0,   FN_);};
  void fnECCChangedAux (const QString& FN_){fileNameChangedAux(1,   FN_);};
  void fnBLQChangedAux (const QString& FN_){fileNameChangedAux(2,   FN_);};

  // page 4:
  void		modeChanged(int);
  void		filterChanged(int);
  
protected:
  virtual void	closeEvent(QCloseEvent *){delete this;};

  void		setupPage1();
  void		setupPage2_1();
  void		setupPage2_2();
  void		setupPage3();
  void		setupPage4();

  bool		checkFileExists(const QString&);
  
  
private:
  FileFormat	FFormat;

  QWidget	*page1;
  QWidget	*page2_1;
  QWidget	*page2_2;
  QWidget	*page3;
  QWidget	*page4;

  // format itrf94
  const int	NumOf1994Files;
  QString	sFileName1994[6];
  QCheckBox	*cbFileName1994[6];
  QLineEdit	*leFileName1994[6];
  QPushButton	*bFile1994[6];
  void		checkPage2_1State();


  // format itrf2000
  const int	NumOf2000Files;
  QString	sFileName2000[6];
  QCheckBox	*cbFileName2000[6];
  QLineEdit	*leFileName2000[6];
  QPushButton	*bFile2000[6];
  void		checkPage2_2State();

  // aux files (independent on format/presentation of SSC && SSV):
  const int	NumOfAuxFiles;
  QString	sFileNameAux[3];
  QCheckBox	*cbFileNameAux[3];
  QLineEdit	*leFileNameAux[3];
  QPushButton	*bFileAux[3];
  void		checkPage3State();
  

  // modes:
  ImportMode	Mode;
  ImportFilter	Filter;


  void		selectFile1994(FileType);
  void		setState1994(FileType, bool);
  void		fileNameChanged1994(FileType, const QString&);

  void		selectFile2000(FileType);
  void		setState2000(FileType, bool);
  void		fileNameChanged2000(FileType, const QString&);

  void		selectFileAux(int);
  void		setStateAux(int, bool);
  void		fileNameChangedAux(int, const QString&);


  void		importITRF();

  void		loadFile1994(SB_TRF*, const QString&, FileType);
  void		loadFile2000(SB_TRF*, const QString&, FileType);

  void		applyFilter(ImportFilter, SB_TRF*);
  bool		loadOLoad(SB_TRF*, QString);
  bool		loadNScodes(SB_TRF*, const QString&);
  bool		loadECCdat(SB_TRF*, const QString&);
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBSiteEditor..										*/
/*												*/
/*==============================================================================================*/
class SBOLoadEditor;
class SBSiteEditor : public SBTabDialog
{
  Q_OBJECT    
public:
  SBSiteEditor(SBSiteListItem*, QWidget* =0, const char* =0, bool =FALSE, WFlags =0);
  SBSiteEditor(QListView*, SB_TRF*, QWidget* =0, const char* =0, bool =FALSE, WFlags =0);
  ~SBSiteEditor();
  virtual QString ClassName() const {return "SBSiteEditor";};
  
  signals:
  void siteModified(bool);
  
  private 
  slots:
  void makeApply();
  virtual void done(int);
  //void accept();
  //void reject();
  
  void editEntry(QListViewItem*);
  void insertEntry();
  void deleteEntry();
  
  void stationModified(bool);
  
 private:
  virtual void		acquireData();
  void			init();

  QWidget*		wSite();
  QWidget*		wOLoad();

  QListView*		SitesLV;
  SBSiteListItem*	SiteItem;
  SBSite*		Site;
  SB_TRF*		Wtrf;
  bool			isModified;
  bool			isNew;
  int			numOpenedWindows;
  SBOLoadEditor*	OLoadEditor;

  /**/
  QString		str;
  QListView*		StationsLV;
  QLineEdit*		EName;
  QLineEdit*		EDome;
  QLineEdit*		ECountry;
  QLineEdit*		EPlate;

  QGroupBox*		NameGBox;
  
  QLineEdit*		EV [3];
  QLineEdit*		EdV[3];
  
protected:
  virtual void closeEvent(QCloseEvent *e){if (numOpenedWindows) e->ignore(); else delete this;};
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBStationEditor..										*/
/*												*/
/*==============================================================================================*/
class SBStationEditor : public SBTabDialog
{
  Q_OBJECT    
    public:
  SBStationEditor(SBStationListItem*, QWidget* =0, const char* =0, bool =FALSE, WFlags =0);
  SBStationEditor(QListView*, SBSite*, QWidget* =0, const char* =0, bool =FALSE, WFlags =0);
  ~SBStationEditor();
  virtual QString ClassName() const {return "SBStationEditor";};
  
 signals:
  void stationModified(bool);
  
  protected
    slots:
  virtual void	makeApply();
  void		addCoordinates();
  void		delCoordinates();
  void		editCoordinates(QListViewItem*);
  void		modified(bool);

  
 private:
  virtual void		acquireData();
  void			init();
  QWidget*		wNames();
  QWidget*		wTechs();
  QWidget*		wCoordinates();
  QWidget*		wCoordList(QWidget*);
  QWidget*		wStats();
  QWidget*		wOLoad();

  SBOLoadEditor*	OLoadEditor;
  QListView*		StationsLV;
  SBStationListItem*	StationItem;
  SBStation*		Station;
  SBSite*		Site;
  bool			isModified;
  bool			isNew;
  QString		Str;

  /* names */
  QLineEdit*		EDome;
  QRadioButton*		RBPtType[2];
  QLineEdit*		EName;
  QLineEdit*		ECDP;
  QLineEdit*		ECharID;
  QLineEdit*		EDescr;
  QMultiLineEdit*	MEComments;
  QGroupBox*		NameGBox;

  /* tech */
  QRadioButton		*RBTech[11];

  /* coordinates */
  QListView		*CoordsLV;
  QLineEdit		*EV[3];
  QLineEdit		*EdV[3];
  //  QRadioButton*		RBClassCoord[7];
  QRadioButton*		RBMountType[6];
  QLineEdit*		EAxisOffset;
  QLabel*		LLongitude;
  QLabel*		LLatitude;
  QLabel*		LHeight;
  
  /* attributes */
  QCheckBox*		CDOrigin[4];
  QCheckBox*		CDAttr  [4];
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBOLoadEditor..										*/
/*												*/
/*==============================================================================================*/
class SBOLoadEditor : public QWidget
{
  Q_OBJECT    
    public:
  SBOLoadEditor(SBOLoad*, bool, QWidget* =0, const char* =0);
  ~SBOLoadEditor();
  virtual QString ClassName() const {return "SBOLoadEditor";};
  virtual bool		collectData();

  protected
    slots:
  void createOLoad();
  void deleteOLoad();
  
 private:
  bool			isSite;
  bool			isDisabled;
  bool			isModified;
  SBOLoad*		OLoad;
  QLineEdit*		EAmplitude[3][11];
  QLineEdit*		EPhase[3][11];

  QLineEdit*		EInfo;
  QLineEdit*		EComment1;
  QLineEdit*		EComment2;

  QLabel*		LStatus;

  friend class SBStationEditor;
  friend class SBSiteEditor;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBCoordsEditor..										*/
/*												*/
/*==============================================================================================*/
class SBCoordsEditor : public QDialog
{
  Q_OBJECT    
public:
  SBCoordsEditor(SBCoordsListItem*, SBStation*, QWidget* =0, const char* =0, bool =FALSE, WFlags =0);
  virtual ~SBCoordsEditor(){};
  virtual QString ClassName() const {return "SBCoordsEditor";};
  
  signals:
  void		coordinatesModified(bool);
  
  protected
  slots:
  void		accept();
  void		reject();

private:
  SBStation		*Station;
  SBCoordsListItem	*Cooli;
  SBCoordinates		*Coo;
  bool			isModified;

  // widgets:
  QLineEdit		*leYr;
  QLineEdit		*leMo;
  QLineEdit		*leDy;
  QLineEdit		*leHr;
  QLineEdit		*leMi;
  QLineEdit		*leSe;

  QLineEdit		*le_R[3];
  QLineEdit		*leDR[3];
  QLineEdit		*le_V[3];
  QLineEdit		*leDV[3];

  QCheckBox		*cbIsUseVels;
  QLineEdit		*leEventName;

  virtual void		acquireData();
};
/*==============================================================================================*/




 


#endif //SB_I_STUFF_STATION_H
