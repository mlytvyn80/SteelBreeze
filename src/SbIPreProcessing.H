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

#ifndef SB_I_PRE_PROCESSING_H
#define SB_I_PRE_PROCESSING_H


#include <config.h>

#include "SbGeneral.H"
#include "SbInterface.H"
#include "SbGeoObsVLBI.H"


#include "SbIObsVLBI.H"






/*==============================================================================================*/
/*												*/
/* SBVLBIObsPPLI..										*/
/*												*/
/*==============================================================================================*/
class SBVLBIObsPPLI : public SBVLBIObsLI
{
public:
  SBVLBIObsPPLI(QListView* parent_, SBVLBISession *Sess_, SBObsVLBIEntry* Obs_)
    : SBVLBIObsLI(parent_, Sess_, Obs_){};
  virtual QString ClassName() const		{return "SBVLBIObsPPLI";};
  virtual QString text(int) const;
  virtual QString key(int, bool) const;
  virtual void paintCell(QPainter *, const QColorGroup &, int, int, int);
};
/*==============================================================================================*/


/*==============================================================================================*/
/*												*/
/* SBCableLI..											*/
/*												*/
/*==============================================================================================*/
class SBCableLI : public QListViewItem
{
protected:
  SBParameter		*Par;
  SBStationInfo		*SI;

public:
  SBCableLI(QListView* parent_, SBParameter *Par_, SBStationInfo* SI_)
    : QListViewItem(parent_) {Par=Par_; SI=SI_;};
  virtual QString ClassName() const		{return "SBCableLI";};
  virtual QString text(int) const;
  virtual QString key(int, bool) const;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBCablesBrowser..										*/
/*												*/
/*==============================================================================================*/
class SBCablesBrowser : public QDialog
{
  Q_OBJECT    
public:
  SBCablesBrowser(SBStationInfoList*, SBParameterList*, const QString&,
		  QWidget* =0, const char* =0, bool =FALSE, WFlags =0);
  virtual ~SBCablesBrowser();
  virtual QString ClassName() const {return "SBCablesBrowser";};
  //virtual void closeEvent(QCloseEvent *e){if (numOpenedWindows) e->ignore(); else delete this;};
  
  protected
  slots:
  void		reject(){QDialog::reject(); delete this;};

private:
  SBStationInfoList	Stations;
  SBParameterList	*Cables;
  
  // widgets:
  QListView		*CablesLV;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* My_QListView..										*/
/*												*/
/*==============================================================================================*/
class My_QListView : public QListView
{
  Q_OBJECT    

  private 
  slots:
  void		movingStarted(QListViewItem*, const QPoint&, int);

private:
  bool			IsMoving;
  int			MCol;
  QListViewItem		*MItem;
  Qt::ButtonState	MouseButtonState;
protected:
  void		contentsMouseMoveEvent(QMouseEvent*);
  void		contentsMousePressEvent(QMouseEvent*);
  void		contentsMouseReleaseEvent(QMouseEvent*);
public:
  My_QListView(QWidget* =0, const char* =0);

  signals:
  void		moveUponItem(QListViewItem*, int, Qt::ButtonState);
};
/*==============================================================================================*/


class QLabel;
class QSpinBox;
class QTabWidget;
class SBPlot;
class SBProject;
class SBSolution;
/*==============================================================================================*/
/*												*/
/* SBVLBIPreProcess..										*/
/*												*/
/*==============================================================================================*/
class SBVLBIPreProcess : public SBVLBISetViewBase
{
  Q_OBJECT    
public:
  SBVLBIPreProcess(QWidget* =0, const char* =0, bool =FALSE, WFlags =0);
  virtual ~SBVLBIPreProcess();
  
  virtual QString ClassName() const {return "SBVLBIPreProcess";};
  
  signals:
  void		stBarReset();
  void		stBarSetTotalSteps(int);
  void		stBarSetProgress(int);
  void		message(const QString&);
  void		run(bool);
    
  private 
  slots:

  void		auxWindowsChanged(bool);
  void		currentSesChange(QListViewItem*);
  void		preProcess();
  void		doubleProc();
  void		procScenario_1();
  void		procScenario_2();
  void		procScenario_3();
  void		procScenario_4();
  void		stopLongRun();
  void		fixSession();
  void		clearPars();
  void		toggleEntryMarkEnable(int, QListViewItem*, const QPoint&, int);
  void		toggleEntryMoveEnable(QListViewItem*, int, Qt::ButtonState);
  void		dumpUI();
  void		restoreUI();
  void		ambigJumpChanged(int);

private:
  bool			IsNeedEstimateCables;

  static const char*	sUFlags[];

  virtual QWidget	*VLBIList(QWidget*, BrowseMode, SBProject*);
  virtual void		init();
  virtual QWidget	*sesTab(QWidget*);

  QWidget		*wEstPars(QWidget*);

  QWidget		*wObservs();
  QWidget		*wAttributes();
  QWidget		*wDataPlotsByBases();
  QWidget		*wDataPlotsByStations();
  QWidget		*wDataPlotsBySources();

  void			saveSession();
  void			updateSession();
  void			fillObsListView();
  void			fillSessAttr();

  void			blockInput();
  void			restoreInput();

  // widgets:
  QTabWidget		*SesTab;

  // buttons:
  QPushButton		*Clear;
  QPushButton		*Run;
  QPushButton		*Fix;
  QPushButton		*Ok;
  QPushButton		*DumpUI;
  QPushButton		*RestoreUI;
  bool			IsBlockedSessionList;
  bool			IsBlockedStationList;
  bool			IsBlockedHistoryList;

  bool			IsBlockedClearButton;
  bool			IsBlockedRunButton;
  bool			IsBlockedFixButton;
  bool			IsBlockedOkButton;

  bool			IsBlockedDumpUIButton;
  bool			IsBlockedRestoreUIButton;
  volatile bool			IsLongRun;

  int			NumAuxWindows;

  //
  QRadioButton		*RBClocks[3];
  QRadioButton		*RBZenith[3];
  QLineEdit		*LEClkPSD;
  QLineEdit		*LEZenPSD;

  int			AmbigJump;

  // current session:
  SBVLBISesPreProcLI	*CurrentSesLI;
  SBVLBISesInfo		*SesInfo;
  SBVLBISession		*Session;

  /* observations */
  My_QListView		*ObservLV;
  /* */
  SBBrowseStations	*brwsSta;
  SBBrowseSources	*brwsSou;
  SBBrowseBases		*brwsBas;
  SBBrowseHistory	*brwsHist;

  /* params */
  QLabel		*lSesName;
  QLabel		*lOfficialName;
  QLabel		*lCorrName;
  QLabel		*lTStart;
  QLabel		*lTFinis;
  QLabel		*lTMean;
  QLabel		*lTofVer;
  QLabel		*lExpDescr;

  QLineEdit		*GrAmbig;
  QLineEdit		*RefFreq;
  QComboBox		*NetID;
  QComboBox		*UFlag;
  QSpinBox		*sbAmbigJump;

  /* 4plotting:*/
  SBPlotCarrier		*PC_Bases;
  SBPlotCarrier		*PC_Stations;
  SBPlotCarrier		*PC_Sources;
  SBPlot		*PlotBases;
  SBPlot		*PlotStations;
  SBPlot		*PlotSources;

  // geo stuff:
  SBProject		*Prj;
  SBSolution		*Solution;
  bool			IsRunning;

  // shortcuts:
  QAccel		*accel;
};
/*==============================================================================================*/







#endif //SB_I_PRE_PROCESSING_H
