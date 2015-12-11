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

#ifndef SB_I_OBS_VLBI_H
#define SB_I_OBS_VLBI_H


#include <config.h>
#include <qdialog.h>
#include <qtabdialog.h>


#include "SbGeneral.H"
#include "SbInterface.H"
#include "SbGeoObsVLBI.H"
#include "SbGeoSources.H"


class QBoxLayout;
class QCheckBox;
class QComboBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QListBox;
class QMultiLineEdit;
class QRadioButton;
class SBPlotCarrier;
class SBProject;



enum BrowseMode {BM_GENERAL, BM_PROJECT, BM_PREPROCESS, BM_UNKN};



/*==============================================================================================*/
/*												*/
/* SBVLBISesInfoLI..										*/
/*												*/
/*==============================================================================================*/
class SBVLBISesInfoLI : public QListViewItem
{
protected:
  SBVLBISesInfo		*SI;
  BrowseMode		Mode;

public:
  SBVLBISesInfoLI(QListView* parent_, SBVLBISesInfo* SI_, BrowseMode Mode_) 
    : QListViewItem(parent_)
    {SI=SI_?SI_:NULL; Mode = Mode_;};
  ~SBVLBISesInfoLI(){SI=NULL;};
  SBVLBISesInfo* sessionInfo(){return SI;};
  virtual QString ClassName() const {return "SBVLBISesInfoLI";};
  virtual QString text(int) const;
  virtual QString key(int, bool) const;
  //  const QPixmap * pixmap (int) const;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBVLBISesPreProcLI..										*/
/*												*/
/*==============================================================================================*/
class SBVLBISesPreProcLI : public SBVLBISesInfoLI
{
public:
  SBVLBISesPreProcLI(QListView* parent_, SBVLBISesInfo* SI_, BrowseMode Mode_) 
    : SBVLBISesInfoLI(parent_, SI_, Mode_){};
  virtual QString ClassName() const {return "SBVLBISesPreProcLI";};
  virtual QString text(int) const;
  virtual QString key(int, bool) const;
  //  const QPixmap * pixmap (int) const;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBVLBIObsLI..										*/
/*												*/
/*==============================================================================================*/
class SBVLBIObsLI : public QListViewItem
{
protected:
  SBObsVLBIEntry	*Obs;
  SBVLBISession		*Sess;
public:
  SBVLBIObsLI(QListView* parent, SBVLBISession *Sess_, SBObsVLBIEntry* Obs_)
    : QListViewItem(parent){Obs=Obs_?Obs_:NULL; Sess=Sess_?Sess_:NULL;};
  SBObsVLBIEntry* obs()				{return Obs;};
  virtual QString ClassName() const		{return "SBVLBIObsLI";};
  virtual QString text(int) const;
  virtual QString key(int, bool) const;
  //  const QPixmap * pixmap (int) const;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBStaInfoLI..										*/
/*												*/
/*==============================================================================================*/
class SBStaInfoLI : public QListViewItem
{
private:
  SBStationInfo		*StaInf;
  BrowseMode		Mode;
public:
  SBStaInfoLI(QListView* parent, SBStationInfo* SI_, BrowseMode Mode_) 
    : QListViewItem(parent){StaInf=SI_?SI_:NULL; Mode = Mode_;};
  virtual QString ClassName() const {return "SBStaInfoLI";};
  SBStationInfo* stationInfo(){return StaInf;};
  virtual QString text(int) const;
  virtual QString key(int, bool) const;
  //  const QPixmap * pixmap (int) const;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBBasInfoLI..										*/
/*												*/
/*==============================================================================================*/
class SBBasInfoLI : public QListViewItem
{
private:
  SBBaseInfo		*BasInf;
public:
  SBBasInfoLI(QListView* parent, SBBaseInfo* BI_) : QListViewItem(parent) {BasInf=BI_?BI_:NULL;};
  virtual QString ClassName() const {return "SBBasInfoLI";};
  SBBaseInfo* baseInfo() {return BasInf;};
  virtual QString text(int) const;
  virtual QString key(int, bool) const;
  //  const QPixmap* pixmap(int) const;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBSouInfoLI..										*/
/*												*/
/*==============================================================================================*/
class SBSouInfoLI : public QListViewItem
{
private:
  SBSourceInfo		*SouInf;
  BrowseMode		Mode;
  SBSource::SStatus	Status;
  SBSource::SType	Type;
  bool			IsSuspected;
public:
  SBSouInfoLI(QListView* parent, SBSourceInfo* SI_, BrowseMode Mode_, 
	      SBSource::SStatus Status_=SBSource::STATUS_UNKN, SBSource::SType Type_=SBSource::TYPE_UNKN, 
	      bool IsSuspected_=FALSE) : QListViewItem(parent) 
    {SouInf=SI_?SI_:NULL; Mode = Mode_; Status=Status_; Type=Type_; IsSuspected = IsSuspected_;};
  virtual QString ClassName() const {return "SBSouInfoLI";};
  SBSourceInfo* sourceInfo() {return SouInf;};
  virtual QString text(int) const;
  virtual QString key(int, bool) const;
  //  const QPixmap*  pixmap(int) const;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBHistoryLI..										*/
/*												*/
/*==============================================================================================*/
class SBHistoryLI : public QListViewItem
{
public:
  enum OType {DBH, LOCAL, UNKN};
private:
  SBHistory::SBHistoryEntry	*HEntry;
  OType				Type;
public:
  SBHistoryLI(QListView* parent, OType Type_, SBHistory::SBHistoryEntry* HEntry_): 
    QListViewItem(parent) {if (HEntry_) HEntry=HEntry_; else HEntry=NULL; Type=Type_;};
  virtual QString ClassName() const		{return "SBHistoryLI";};
  SBHistory::SBHistoryEntry*	hEntry(){return HEntry;};
  OType				type(){return Type;};
  virtual QString text(int) const;
  virtual QString key(int, bool) const;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBClockBreakLI..										*/
/*												*/
/*==============================================================================================*/
class SBClockBreakLI : public QListViewItem
{
protected:
  SBParameterBreak	*Br;
public:
  SBClockBreakLI(QListView* parent_, SBParameterBreak* Br_) : QListViewItem(parent_) {Br = Br_;};
  SBParameterBreak* clockBreak(){return Br;};
  virtual QString ClassName() const {return "SBClockBreakLI";};
  virtual QString text(int) const;
  virtual QString key(int, bool) const;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBBrowseSessions..										*/
/*												*/
/*==============================================================================================*/
class SBBrowseSessions : public QWidget
{
protected:
  SBProject			*Project;
  SBVLBISesInfoList		*SesInfoList;
  QListView			*LVSessions;
  QBoxLayout			*Layout;
  BrowseMode			BrwsMode;
  int				NumPPSess;
  int				NumObs;

public:
  SBBrowseSessions(SBVLBISesInfoList*, BrowseMode, SBProject* =NULL, QWidget* =0, const char* =0, WFlags=0);
  virtual QString		ClassName() const {return "SBBrowseSessions";};
  void				updateList();
  QListView			*lvSessions() {return LVSessions;};
  QBoxLayout			*layout() {return Layout;};
  const SBVLBISesInfoList*	sesInfoList() const {return SesInfoList;};
  int				numPPSess() const {return NumPPSess;};
  int				numObs() const {return NumObs;};
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBBrowseStations..										*/
/*												*/
/*==============================================================================================*/
class SBBrowseStations : public QWidget
{
  Q_OBJECT    

protected:
  SBStationInfoList		*StaInfoList;
  SBListView			*LVStations;
  BrowseMode			Mode;
  SBMJD				T0;
  
  private 
  slots:
  void toggleEntryMarkEnable(int, QListViewItem*, const QPoint&, int);
  void toggleEntryMoveEnable(QListViewItem*, int);
  void editEntry(QListViewItem*);
  void stainfoModified(bool);
  
public:
  SBBrowseStations(SBStationInfoList*, BrowseMode, const SBMJD&, QWidget* =0, const char* =0, WFlags=0);
  virtual QString ClassName() const		{return "SBBrowseStations";};
  QListView			*lvStations() {return LVStations;};
  const SBStationInfoList	*staInfoList() const {return StaInfoList;};
  BrowseMode			mode() const {return Mode;};
  void				updateList();
  void				setStaInfoList(SBStationInfoList*);
  void				setT0(const SBMJD& T0_) {T0 = T0_;};

  signals:
  void widgetIsBusy(bool);
  void modified(bool);
  void refClockChanged(SBStationInfo*);
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBBrowseSources..										*/
/*												*/
/*==============================================================================================*/
class SBBrowseSources : public QWidget
{
  Q_OBJECT    
protected:
  SBSourceInfoList		*SouInfoList;
  SBListView			*LVSources;
  BrowseMode			Mode;
  
  private 
  slots:
  void toggleEntryMarkEnable(QListViewItem*, const QPoint&, int);
  void toggleEntryMoveEnable(QListViewItem*, int);

public:
  SBBrowseSources(SBSourceInfoList*, BrowseMode, SB_CRF*, QWidget*, const char* =0, WFlags=0);
  virtual QString ClassName() const		{return "SBBrowseSources";};
  QListView			*lvSources() {return LVSources;};
  const SBSourceInfoList	*souInfoList() const {return SouInfoList;};
  void				setSouInfoList(SBSourceInfoList*);
  void				updateList(SB_CRF*);
  
  signals:
  void modified(bool);
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBBrowseBases..										*/
/*												*/
/*==============================================================================================*/
class SBBrowseBases : public QWidget
{
  Q_OBJECT    
protected:
  SBBaseInfoList		*BasInfoList;
  QListView			*LVBases;

  private 
  slots:
  void toggleEntryMarkEnable(QListViewItem*, const QPoint&, int);

public:
  SBBrowseBases(SBBaseInfoList*, QWidget* =0, const char* =0, WFlags=0);
  virtual QString ClassName() const		{return "SBBrowseBases";};
  QListView			*lvBases() {return LVBases;};
  const SBBaseInfoList		*basInfoList() const {return BasInfoList;};
  void				setBasInfoList(SBBaseInfoList*);
  void				updateList();

  signals:
  void modified(bool);
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBBrowseHistory..										*/
/*												*/
/*==============================================================================================*/
class SBBrowseHistory : public QWidget
{
  Q_OBJECT    
protected:
  SBHistory			*HistDBH;
  SBHistory			*HistLocal;
  QListView			*LVHistory;
  QLineEdit			*EHistLine;
  int				Version;

  signals:
  void modified(bool);
  void versionInced();
  
  protected
  slots:
  void deleteHistoryLine();
  void addHistoryLine();

  public
  slots:
  void incVersion() {++Version; emit versionInced();};
  
public:
  SBBrowseHistory(SBHistory*, SBHistory*, int, QWidget* =0, const char* =0, WFlags=0);
  virtual QString ClassName() const {return "SBBrowseHistory";};
  QListView		*lvHistory() {return LVHistory;};
  const SBHistory	*histLocal() const {return HistLocal;};
  const SBHistory	*histDBH() const {return HistDBH;};
  void			setHistories(SBHistory*, SBHistory*, int);
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBVLBISetViewBase..										*/
/*												*/
/*==============================================================================================*/
class SBVLBISetViewBase : public QDialog
{
  Q_OBJECT    
  private 
  slots:
  void modified(bool);
  void childWindowClosed();
  void accept();
  void reject();
  void unitsChanged(int);
  
protected:
  SBVLBISetViewBase(QWidget* =0, const char* =0, bool =FALSE, WFlags =0);
  virtual ~SBVLBISetViewBase();
  virtual QString ClassName() const {return "SBVLBISetViewBase";};
  
  SBVLBISet		*Wvlbi;
  QString		str;

  SBBrowseSessions	*brwsSes;
  QLabel		*LNumSes;
  QLabel		*LNumObs;
  QLabel		*LNumPPS;
  QRadioButton		*RBUnit[3];
  bool			isModified;
  int			numOpenedWindows;
  
  virtual void		drawInfo();
  virtual void		init()=0;
  virtual QWidget	*VLBIList(QWidget*, BrowseMode, SBProject*);
  virtual QWidget	*unitsGroup(QWidget*);
  virtual QWidget	*optionsGroup(QWidget*){return NULL;};
  virtual QWidget	*infoGroup(QWidget*);
  
  virtual void closeEvent(QCloseEvent *e){if (numOpenedWindows) e->ignore(); else delete this;};
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBVLBISetView..										*/
/*												*/
/*==============================================================================================*/
class SBVLBISetView : public SBVLBISetViewBase
{
  Q_OBJECT    
public:
  SBVLBISetView(QWidget* =0, const char* =0, bool =FALSE, WFlags =0);
  virtual ~SBVLBISetView(){};
  
  virtual QString ClassName() const {return "SBVLBISetView";};
  
  signals:
  void stBarReset();
  void stBarSetTotalSteps(int);
  void stBarSetProgress(int);
  void message(const QString&);
    
  private 
  slots:
  void networksModified(bool, SBVLBISet::NetList*);
  void masterRecordModified(bool);
  
  void importNGS(){import(SBVLBISession::IF_NGS);};
  void importDBH(){import(SBVLBISession::IF_DBH);};
  void import(SBVLBISession::ObsImportedFrom);
  
  void editEntry(QListViewItem*);
  void deleteEntry();
  void editNetworks();
  void browseMRecords();
  
private:
  QCheckBox		*CBDump;

  QPushButton		*Delete;
  QRadioButton		*RBIfExistedAskUser;
  QRadioButton		*RBIfExistedDrop;
  QRadioButton		*RBIfExistedImport;


  virtual void		init();
  virtual QWidget	*optionsGroup(QWidget*);
};
/*==============================================================================================*/


/*==============================================================================================*/
/*												*/
/* SBVLBISessionEditor..									*/
/*												*/
/*==============================================================================================*/
class SBVLBISessionEditor : public SBTabDialog
{
  Q_OBJECT    
public:
  SBVLBISessionEditor(SBVLBISesInfoLI*, SBVLBISet*, QWidget* =0, const char* =0, 
		      bool =FALSE, WFlags =0);
  ~SBVLBISessionEditor();
  virtual QString	ClassName() const {return "SBVLBISessionEditor";};
  
  signals:
  void			sessionModified(bool);
  void			windowClosed();
  
  protected
  slots:
  void			modified(bool);
  void			toggleEntryMarkEnable(int, QListViewItem*, const QPoint&, int);
  virtual void		makeApply();
  virtual void		histModified(bool B){if (B) isModified=TRUE;};
  void			histVerInced(){Sess->incVersion(); isModified=TRUE;};
  void			refClockChange(SBStationInfo*);

private:
  SBVLBISesInfoLI	*SIItem;
  SBVLBISesInfo		*SI;
  SBVLBISession		*Sess;
  SBVLBISet		*Wvlbi;
  
  bool			isModified;
  QString		Str;

    
  /* functions */
  virtual void		acquireData();
  QWidget		*wObservs();
  QWidget		*wParameters();
  QWidget		*wHistory();
  QWidget		*wDataPlotsByBases();
  QWidget		*wDataPlotsByStations();

  /* observations */
  QListView		*ObservLV;

  /* */
  SBBrowseStations	*brwsSta;
  SBBrowseSources	*brwsSou;
  SBBrowseBases		*brwsBas;
  SBBrowseHistory	*brwsHist;

  /* params */
  QLineEdit		*GrAmbig;
  QLineEdit		*RefFreq;
  QComboBox		*NetID;
  QComboBox		*UFlag;

  /* 4plotting:*/
  SBPlotCarrier		*PC_Bases;
  SBPlotCarrier		*PC_Stations;
  double		scale;
  QString		ScaleName;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBVLBINetworkEditor..									*/
/*												*/
/*==============================================================================================*/
class SBVLBINetworkEditor : public QDialog
{
  Q_OBJECT    
public:
  SBVLBINetworkEditor(SBVLBISet*, QWidget* =0, const char* =0, bool =FALSE, WFlags =0);
  ~SBVLBINetworkEditor();
  virtual QString ClassName() const {return "SBVLBINetworkEditor";};
  
  signals:
  void networksModified(bool, SBVLBISet::NetList*);
  
  protected
  slots:
  void accept();
  void reject();
  void addEntry();
  void editEntry();
  void deleteEntry();
  void entryModified(bool, SBVLBISet::Network*);
  
private:
  class NetworkListItem : public QListViewItem
  {
  private:
    SBVLBISet::Network		*Network;
  public:
    NetworkListItem(QListView* parent, SBVLBISet::Network* N_) : QListViewItem(parent)
      {if (N_) Network=N_; else Network=NULL;};
    virtual QString text(int) const;
    SBVLBISet::Network* network(){return Network;};
  };
  
  SBVLBISet		*Wvlbi;
  SBVLBISet::NetList	*Nets;
  bool			isModified;
  int			numOpenedWindows;
  QString		Str;
  
  QListView		*NetsLV;
protected:
  virtual void closeEvent(QCloseEvent *e){if (numOpenedWindows) e->ignore(); else delete this;};
};
/*==============================================================================================*/


 
/*==============================================================================================*/
/*												*/
/* SBVLBINetEntryEditor..									*/
/*												*/
/*==============================================================================================*/
class SBVLBINetEntryEditor : public QDialog
{
  Q_OBJECT    
public:
  SBVLBINetEntryEditor(SBVLBISet::Network*, QListViewItem*, QWidget* =0, 
		       const char* =0, bool =FALSE, WFlags =0);
  ~SBVLBINetEntryEditor();
  virtual QString ClassName() const {return "SBVLBINetEntryEditor";};
  
  signals:
  void networkModified(bool, SBVLBISet::Network*);
  
  protected
  slots:
  void accept();
  void reject(){QDialog::reject();delete this;};
  void delKey();
  void addKey();
  void modKey();
  void showKey(int);
  
private:
  SBVLBISet::Network	*Network;
  QListViewItem		*LI;
  bool			isModified;
  QString		Str;
  QLineEdit		*ID;
  QMultiLineEdit	*Descr;
  QLineEdit		*Key;
  QListBox		*KeyList;
protected:
  virtual void closeEvent(QCloseEvent){delete this;};
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBMasterRecBrowser..										*/
/*												*/
/*==============================================================================================*/
class SBMasterRecBrowser : public QDialog
{
  Q_OBJECT    
public:
  SBMasterRecBrowser(SBVLBISet*, QWidget* =0, const char* =0, bool =FALSE, WFlags =0);
  ~SBMasterRecBrowser();
  virtual QString ClassName() const	{return "SBMasterRecBrowser";};


  signals:
  void masterRecordModified(bool);
  
  protected
  slots:
  void accept()				{QDialog::accept(); delete this;};
  void reject()				{QDialog::reject(); delete this;};
  

private:
  class SBMRListItem : public QListViewItem
  {
  private:
    SBMasterRecord		*Rec;
    QString			MFName;
  public:
    SBMRListItem(QListView* parent, SBMasterRecord* R_, const QString& MFName_) 
      : QListViewItem(parent) {Rec=R_; MFName=MFName_.copy();};
    virtual QString text(int) const;
    virtual QString key(int, bool) const;
    SBMasterRecord* record(){return Rec;};
  };

  SBVLBISet		*VLBI;
  SBMaster		*Master;

  QListView		*MRecLV;

  bool			isModified;

protected:
  virtual void closeEvent(QCloseEvent)	{delete this;};
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBClockBreakEditor..										*/
/*												*/
/*==============================================================================================*/
class SBClockBreakEditor : public QDialog
{
  Q_OBJECT    
public:
  SBClockBreakEditor(SBClockBreakLI*, QWidget* =0, const char* =0, bool =FALSE, WFlags =0);
  virtual ~SBClockBreakEditor(){B = NULL;};
  virtual QString ClassName() const {return "SBClockBreakEditor";};
  
  signals:
  void		clockBreakModified(bool);
  
  protected
  slots:
  void		accept();
  void		reject();

private:
  SBClockBreakLI*	Bli;
  SBParameterBreak*	B;
  bool			isModified;

  // widgets:
  QLineEdit		*leYr;
  QLineEdit		*leMo;
  QLineEdit		*leDy;
  QLineEdit		*leHr;
  QLineEdit		*leMi;
  QLineEdit		*leSe;
  QLineEdit		*leVal;

  virtual void		acquireData();
};
/*==============================================================================================*/


class QSpinBox;
/*==============================================================================================*/
/*												*/
/* SBStaParsEditor..										*/
/*												*/
/*==============================================================================================*/
class SBStaParsEditor : public QDialog
{
  Q_OBJECT    
public:
  SBStaParsEditor(SBStationInfo*, const SBMJD&, QWidget* =0, const char* =0, bool =FALSE, WFlags =0);
  virtual ~SBStaParsEditor();
  virtual QString ClassName() const {return "SBStaParsEditor";};
  //virtual void closeEvent(QCloseEvent *e){if (numOpenedWindows) e->ignore(); else delete this;};
  
  signals:
  void		staInfoModified(bool);
  
  protected
  slots:
  void		accept();
  void		reject(){QDialog::reject(); delete this;};
  void		editEntry();
  void		deleteEntry();
  void		insertEntry();
  void		modified(bool);
  void		editClocksPar();
  void		editZenithPar();

private:
  SBStationInfo*	SI;
  SBMJD			T0;
  bool			isModified;

  // widgets:
  QListView		*ClockBreaksLV;
  QSpinBox		*sbOrder;
  QCheckBox		*cbClocks;
  QCheckBox		*cbZenith;

  QCheckBox		*cbAttrs[6];

  QPushButton		*CPButton;
  QPushButton		*ZPButton;

  virtual void		acquireData();
};
/*==============================================================================================*/





 


#endif //SB_I_OBS_VLBI_H
