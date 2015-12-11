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

#ifndef SB_I_MAINWIN_H
#define SB_I_MAINWIN_H


#include <config.h>
#include <qwidget.h>
#include <qmainwindow.h>
#include <qprogressbar.h> 
#include <qtoolbutton.h>

#include "SbGeneral.H"
#include "SbInterface.H"
#include "SbSetup.H"


class SBMainWindow;
extern SBMainWindow	*MainWin;



class QSplitter;
class QLabel;
class SBGenInfoPanel;
class SBDetInfoPanel;
/*==============================================================================================*/
/*												*/
/* SBCentralPanel..										*/
/*												*/
/*==============================================================================================*/
class SBCentralPanel : public QWidget
{
  friend class SBMainWindow;
  Q_OBJECT
public:
  SBCentralPanel(QWidget* =0, const char* =0);
  ~SBCentralPanel();
  virtual QString ClassName() const {return "SBCentralPanel";};  
  
private:
  QLabel*	TRFName;
  QLabel*	CRFName;
  QLabel*	EOPName;
  QLabel*	PrjName;
  QLabel*	Status;

  QLabel*	Operation;
  QLabel*	ProcSess;
  QLabel*	ProcObs;
  QLabel*	SessName;
  QLabel*	CasheSizes;
  

  QFrame*	genInfoPanel(QWidget*, const char*);
  QFrame*	detInfoPanel(QWidget*, const char*);
};
/*==============================================================================================*/



class QPopupMenu;
/*==============================================================================================*/
/*												*/
/* SBMainWindow..										*/
/*												*/
/*==============================================================================================*/
class SBMainWindow : public QMainWindow
{
  Q_OBJECT
public:
  SBMainWindow(QWidget* =0, const char* =0, WFlags =0);
  virtual ~SBMainWindow();
  
  virtual QString ClassName() const {return "SBMainWindow";};
  
  public 
  slots:
  void ControlConfigure();
  void ControlDefault();
  void ControlSelectFont();
  void ControlFlushLog();

  void ControlStyleDefault();
  void ControlStyleMotifPlus();
  void ControlStyleCDE();
  void ControlStyleSGI();
  void ControlStyleInterlace();
  void ControlStyleWindows();
  void ControlStylePlatinum();
  
  void StuffStations();
  void StuffSources();
  void StuffEOPs();
  void StuffVLBIObs();
  void StuffEccenticities();
  void StuffEphemerides();
  void StuffAplo();
  void StuffPlateMotion();
  void StuffModels();
  
  void StuffModelsNutPsi();
  void StuffModelsNutEps();
  void StuffModelsUT1High();
  void StuffModelsUT1Low();
  void StuffModelsUT1Diurn();
  void StuffModelsPolDiurn();
  
  void ExecuteGenProj();
  void ExecuteSelProj();
  void ExecuteEditProj();
  void ExecuteModels();
  void ExecuteParameters();
  void ExecuteRunProj();
  void ExecuteDoubleRunProj();
  void ExecuteReportProj();
  void ExecuteBrowseSolution();
  void ExecuteDrop();
  
  void UtilitiesPreProc();
  void UtilitiesSpectrum();
  void UtilitiesFilters();
  void UtilitiesDumpUI();
  void UtilitiesRestoreUI();

  void UtilitiesCollectStat4Prj();
  void UtilitiesCollectStat4All();
  void UtilitiesBrowseStat4Prj();
  void UtilitiesBrowseStat4All();


  
  void TestPrecession();
  void TestNutation();
  void TestRotation();
  void TestEphemerides();
  void TestEOPs();
  void TestDiurnalEOPs();
  void TestSolidTide();
  void TestOceanTide();
  void TestPoleTide();
  void TestAplo();
  void TestJustATest();
  
  void HelpHelp();
  void HelpAboutQt();
  void HelpAbout();
 
  void setMessage(const QString&);
  void toggleToolBar();
  void toggleStatusBar();
  void toggleTBLabels();

  void stBarReset();
  void stBarSetTotalSteps(int);
  void stBarSetProgress  (int);

  // General Info:
  void setTRFName(const QString&);
  void setCRFName(const QString&);
  void setEOPName(const QString&);
  void setPrjName(const QString&);
  void setStatus(const QString&);

  // Detail Info:
  void setOperation(const QString&);
  void setProcSes (int, int);
  void setProcObs (int, int);
  void setSessName(const QString&);
  void setCacheSizes(int, int, int, int);

  // 
  void setRunning(bool);
  void stopRunning(void);

  //  public 
  signals:
  void run(bool);
  
protected:
  SBCentralPanel	*CentralWidget;
  
  virtual void	resizeEvent(QResizeEvent*);
  virtual void	moveEvent  (QMoveEvent*);
  void		changeStyle(SBSetUp::GUIStyle);

  /* icon sets: */
  QIconSet		*isStop;

  /* buttons: */
  QToolButton		*bStop;
  QToolButton		*bExit;

  /* menu: */
  QPopupMenu		*ControlMenu;
  QPopupMenu		*StyleMenu;
  void			setupMenu();
  int			toolbarChk;
  int			statusbarChk;
  int			labelsChk;
  int			Styles[8]; //number of styles + 1

  /* status bar: */
  QLabel*		StBarLabel;
  QProgressBar*		StBarProgress;
  QLabel*		StBarMessage;
  void			setupStatusBar();

  /* tool bar: */
  QToolBar		*ToolBar;
  void			setupToolBar();
};
/*==============================================================================================*/


#endif //SB_I_MAINWIN_H
