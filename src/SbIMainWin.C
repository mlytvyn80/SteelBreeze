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

#include "SbIMainWin.H"

#include <unistd.h>


#include <qaccel.h>
#include <qapplication.h>
#include <qdatetime.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qfont.h>
#include <qfontdialog.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qmenubar.h>
#include <qmessagebox.h>
#include <qpixmap.h>
#include <qpopupmenu.h>
#include <qprogressbar.h> 
#include <qsplitter.h>
#include <qstatusbar.h>

#include <qmotifstyle.h>
#include <qwindowsstyle.h>
#include <qplatinumstyle.h>
#include <qcdestyle.h>
#include <qinterlacestyle.h>
#include <qmotifplusstyle.h>
#include <qsgistyle.h>

#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qtooltip.h>
#include <qwhatsthis.h>


#include "SbSetup.H"
#include "SbGeoEop.H"
#include "SbGeoObsVLBIStat.H"
#include "SbGeoProject.H"


#include "SbISetup.H"
#include "SbIStuff.H"
#include "SbIStuffSource.H"
#include "SbIStuffStation.H"
#include "SbIObsVLBI.H"
#include "SbISolution.H"
#include "SbITest.H"
#include "SbIProject.H"
#include "SbIPreProcessing.H"
#include "SbIObsVLBIStat.H"
#include "SbIMEM.H"
#include "SbIFilters.H"



// 
#include "xpm/Settings.xpm"
#include "xpm/Exit.xpm"
#include "xpm/Antenna.xpm"
#include "xpm/Quasar.xpm"
#include "xpm/Earth.xpm"
#include "xpm/Database.xpm"
#include "xpm/Stop.xpm"
#include "xpm/Stop_disabled.xpm"
// sources pixmaps:
#include "xpm/SrcStatusDefining.xpm"
#include "xpm/SrcStatusCandidate.xpm"
#include "xpm/SrcStatusOther.xpm"
#include "xpm/SrcStatusNew.xpm"
#include "xpm/SrcStatusUnknown.xpm"
#include "xpm/SrcHLink.xpm"



SBMainWindow	*MainWin;
bool		IsNeedToStop = FALSE;



/*==============================================================================================*/
/*												*/
/* SBCentralPanel..										*/
/*												*/
/*==============================================================================================*/
SBCentralPanel::SBCentralPanel(QWidget* parent, const char* name)
  : QWidget(parent, name)
{
  QSplitter	*Split1, *Split2;

  Split1   = new QSplitter(QSplitter::Vertical, this);
  Split2   = new QSplitter(Split1);
  Log      = new SBLogger (Split1);

  genInfoPanel(Split2, NULL);
  detInfoPanel(Split2, NULL);
  
  Split1->setResizeMode(Split2, QSplitter::KeepSize);

  QBoxLayout *Layout = new QVBoxLayout(this, 0, 3);
  Layout -> addWidget(Split1, 8);
  Layout -> activate();
};

SBCentralPanel::~SBCentralPanel()
{
};

QFrame* SBCentralPanel::genInfoPanel(QWidget* parent, const char* name)
{
  QFrame* P = new QFrame(parent, name);
  P->setFrameStyle(QFrame::Panel | QFrame::Raised);

  QGridLayout		*grid;
  QLabel		*label;

  grid = new QGridLayout(P, 5,2, 20,5);

  //
  label = new QLabel("TRF database filename:", P);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label,  0,0);

  label = new QLabel("CRF database filename:", P);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label,  1,0);

  label = new QLabel("EOP database filename:", P);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label,  2,0);

  label = new QLabel("Project name:", P);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label,  3,0);

  label = new QLabel("Status:", P);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label,  4,0);


  //
  //
  //--
  TRFName = new QLabel(SetUp?SetUp->stations():QString("undefined"), P);
  TRFName->setMinimumSize(TRFName->sizeHint());
  TRFName->setAlignment(AlignRight|AlignVCenter);
  grid->addWidget(TRFName,  0,1);

  CRFName = new QLabel(SetUp?SetUp->sources():QString("undefined"), P);
  CRFName->setMinimumSize(CRFName->sizeHint());
  CRFName->setAlignment(AlignRight|AlignVCenter);
  grid->addWidget(CRFName,  1,1);

  EOPName = new QLabel(SetUp?SetUp->eops():QString("undefined"), P);
  EOPName->setMinimumSize(EOPName->sizeHint());
  EOPName->setAlignment(AlignRight|AlignVCenter);
  grid->addWidget(EOPName,  2,1);

  PrjName = new QLabel(SetUp?SetUp->project():QString("undefined"), P);
  PrjName->setMinimumSize(PrjName->sizeHint());
  PrjName->setAlignment(AlignRight|AlignVCenter);
  grid->addWidget(PrjName,  3,1);

  Status = new QLabel("OK", P);
  Status->setMinimumSize(Status->sizeHint());
  Status->setAlignment(AlignRight|AlignVCenter);
  grid->addWidget(Status,  4,1);

  grid->activate();
  return P;
};

QFrame* SBCentralPanel::detInfoPanel(QWidget* parent, const char* name)
{
  QFrame* P = new QFrame(parent, name);
  P->setFrameStyle(QFrame::Panel | QFrame::Raised);

  QGridLayout		*grid;
  QLabel		*label;

  grid = new QGridLayout(P, 5,2, 20,5);

  //
  label = new QLabel("Current operation:", P);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label,  0,0);

  //
  label = new QLabel("Processed sessions:", P);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label,  1,0);

  label = new QLabel("Processed observations:", P);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label,  2,0);

  //
  label = new QLabel("Current session:", P);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label,  3,0);

  label = new QLabel("Caches (Obs/Session/CRF/TRF):", P);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label,  4,0);

  //
  //
  //--
  Operation = new QLabel("Waiting", P);
  Operation->setMinimumSize(Operation->sizeHint());
  Operation->setAlignment(AlignRight|AlignVCenter);
  grid->addWidget(Operation,  0,1);

  ProcSess = new QLabel("[N/A]", P);
  ProcSess->setMinimumSize(ProcSess->sizeHint());
  ProcSess->setAlignment(AlignRight|AlignVCenter);
  grid->addWidget(ProcSess,  1,1);

  ProcObs = new QLabel("[N/A]", P);
  ProcObs->setMinimumSize(ProcObs->sizeHint());
  ProcObs->setAlignment(AlignRight|AlignVCenter);
  grid->addWidget(ProcObs,  2,1);

  SessName = new QLabel("<NONE>", P);
  SessName->setMinimumSize(SessName->sizeHint());
  SessName->setAlignment(AlignRight|AlignVCenter);
  grid->addWidget(SessName,  3,1);

  CasheSizes = new QLabel("0/0/0/0", P);
  CasheSizes->setMinimumSize(CasheSizes->sizeHint());
  CasheSizes->setAlignment(AlignRight|AlignVCenter);
  grid->addWidget(CasheSizes,  4,1);

  grid->setColStretch(1, 20);
  grid->activate();
  return P;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBMainWindow..										*/
/*												*/
/*==============================================================================================*/
SBMainWindow::SBMainWindow(QWidget* parent, const char* name, WFlags f)
  : QMainWindow(parent, name, f)
{
  IsNeedToStop = FALSE;
  CentralWidget = new SBCentralPanel(this);

  QDateTime time = QDateTime::currentDateTime();
  Log->write (SBLog::INF, SBLog::IO, "...started up: " +  time.toString());

  SetUp = new SBSetUp;
  SetUp	-> LoadConfig();
  SetUp	-> CheckConfig();
  SetUp -> UpdateLogger();
  Log   -> startUp();
  SetUp -> LoadStuff();
 
  // set up a toolbar:
  setupToolBar();
  // set up a statusbar:
  setupStatusBar();
  // set up a menu:
  setupMenu();
  // 
  setCentralWidget(CentralWidget);
  resize(SetUp->centralPanelSize());
  move(SetUp->centralPanelPosition());
  QApplication::setFont(SetUp->defaultFont(), TRUE);


  //  QFont f(SetUp->defaultFont());
  //  f.setBold(TRUE);
  //  QApplication::setFont(f, TRUE, "QLabel");
  //  QApplication::setFont(f, TRUE, "QCheckBox");
  //  QApplication::setFont(f, TRUE, "QRadioButton");
  //  QApplication::setFont(f, TRUE, "QHeader");
  

  setMessage("Ready");

  // init pixmaps:
  pmSrcStatus[0] = new QPixmap(SrcStatusDefining_xpm);
  pmSrcStatus[1] = new QPixmap(SrcStatusCandidate_xpm);
  pmSrcStatus[2] = new QPixmap(SrcStatusOther_xpm);
  pmSrcStatus[3] = new QPixmap(SrcStatusNew_xpm);
  pmSrcStatus[4] = new QPixmap(SrcStatusUnknown_xpm);
  pmSrcHLink     = new QPixmap(SrcHLink_xpm);
  //

  if (SetUp)
    {
      setPrjName(SetUp->project());
      setTRFName(SetUp->stations());
      setCRFName(SetUp->sources());
      setEOPName(SetUp->eops());
      SBProject* P;
      if ((P=SetUp->loadProject()))
	{
	  setProcSes (0, P->numVLBISessions());
	  setProcObs (0, P->numObs());
	  if (P) delete P;
	};
    };
  //  statusBar()->message("Loaded setup", 500);
  setSessName("<none>");
  connect(this, SIGNAL(run(bool)), SLOT(setRunning(bool)));
  
  statusBar()->message("The chances of anything coming from Mars are a million to one", 300);
};

SBMainWindow::~SBMainWindow()
{
  SetUp->setCentralPanelSize(size());
  SetUp->setCentralPanelPosition(pos());
  SetUp->SaveStuff ();
  SetUp->SaveConfig();
  
  // delete pixmaps:
  if (pmSrcStatus[0]) delete pmSrcStatus[0];
  if (pmSrcStatus[1]) delete pmSrcStatus[1];
  if (pmSrcStatus[2]) delete pmSrcStatus[2];
  if (pmSrcStatus[3]) delete pmSrcStatus[3];
  if (pmSrcStatus[4]) delete pmSrcStatus[4];
  if (pmSrcHLink) delete pmSrcHLink;

  // delete icon sets:
  if (isStop) delete isStop;
  

  delete Institutions;
  delete SetUp;
  delete Log;
};

void SBMainWindow::setupToolBar()
{
  static const QString sStationsWhatThis = 
    "Click this button to open Station database.\n\n"
    "You can also select the Stations command from the Stuff menu.";
  static const QString sSourcesWhatThis = 
    "Click this button to open Sourse database.\n\n"
    "You can also select the Sourses command from the Stuff menu.";
  static const QString sEOPsWhatThis = 
    "Click this button to open EOP database.\n\n"
    "You can also select the EOPs command from the Stuff menu.";
  static const QString sVLBIWhatThis = 
    "Click this button to open VLBI database.\n\n"
    "You can also select the VLBI database command from the Stuff menu.";
  static const QString sConfigWhatThis = 
    "Click this button to Configure.\n\n"
    "You can also select the Configure command from the Control menu.";
  static const QString sExitWhatThis = 
    "Click this button to exit.";
  static const QString sStopWhatThis = 
    "Click this button to exit too.";
 

  ToolBar = new QToolBar(this);

  QToolButton *bSettings = 
    new QToolButton(QPixmap(Settings_xpm), "Configure", 
		    "Click here to configure", 
		    this, SLOT(ControlConfigure()), ToolBar);
  QToolButton *bQuasar = 
    new QToolButton(QPixmap(Quasar_xpm), "Sources", 
		    "Click here to Sources", 
		    this, SLOT(StuffSources()), ToolBar);
  QToolButton *bAntenna = 
    new QToolButton(QPixmap(Antenna_xpm), "Stations", 
		    "Click here to Stations", 
		    this, SLOT(StuffStations()), ToolBar);
  QToolButton *bEarth = 
    new QToolButton(QPixmap(Earth_xpm), "EOPs", 
		    "Click here to EOPs", 
		    this, SLOT(StuffEOPs()), ToolBar);
  QToolButton *bVLBIData = 
    new QToolButton(QPixmap(Database_xpm), "VLBI", 
		    "Click here to VLBI dataset", 
		    this, SLOT(StuffVLBIObs()), ToolBar);
  //
  bExit = new QToolButton(QPixmap(Exit_xpm), "Quit", 
			  "Click here to quit", 
			  qApp, SLOT(quit()), ToolBar);
  isStop = new QIconSet(QPixmap(Stop_xpm));
  isStop->setPixmap(QPixmap(Stop_disabled_xpm), QIconSet::Automatic, QIconSet::Disabled);
  bStop = new QToolButton(*isStop, "Stop", 
			  "Click here to stop running", 
			  this, SLOT(stopRunning()), ToolBar);
  bStop->setEnabled(FALSE);
  //
  
  ToolBar -> addSeparator();
  setUsesTextLabel(TRUE);
  setUsesBigPixmaps(TRUE);

  QSize minSize=bSettings->sizeHint();
  bQuasar  -> setMinimumSize(minSize);
  bAntenna -> setMinimumSize(minSize);
  bEarth   -> setMinimumSize(minSize);
  bVLBIData-> setMinimumSize(minSize);
  bExit    -> setMinimumSize(minSize);
  bStop    -> setMinimumSize(minSize);
        
  QWhatsThis::whatsThisButton(ToolBar);
  QWhatsThis::add(bSettings, sConfigWhatThis);
  QWhatsThis::add(bAntenna, sStationsWhatThis);
  QWhatsThis::add(bQuasar, sSourcesWhatThis);
  QWhatsThis::add(bEarth, sEOPsWhatThis);
  QWhatsThis::add(bVLBIData, sVLBIWhatThis);
  QWhatsThis::add(bExit, sExitWhatThis);
  QWhatsThis::add(bStop, sStopWhatThis);
};

void SBMainWindow::setupStatusBar()
{
  StBarMessage = new QLabel(statusBar());
  StBarMessage->setText(" 123 ");
  StBarMessage->setFrameStyle (QFrame::Panel | QFrame::Sunken);
  StBarMessage->setAlignment  (AlignVCenter | AlignLeft);
  StBarMessage->setFixedHeight(StBarMessage->sizeHint().height());
  StBarMessage->setText("");
  statusBar()->addWidget(StBarMessage, 10, FALSE);

  StBarProgress = new QProgressBar(statusBar());
  //  StBarProgress->setFrameStyle (QFrame::Panel | QFrame::Sunken);
  StBarProgress->setMinimumWidth(150);
  StBarProgress->reset();
  statusBar()->addWidget(StBarProgress, 0, TRUE);
  
  StBarLabel = new QLabel(statusBar());
  StBarLabel->setText(" " + Version.name() + " ");
  StBarLabel->setFrameStyle (QFrame::Box | QFrame::Sunken);
  StBarLabel->setAlignment  (AlignVCenter | AlignLeft);
  StBarLabel->setFixedSize(StBarLabel->sizeHint());
  statusBar()->addWidget(StBarLabel, 0, TRUE);
};

void SBMainWindow::setupMenu()
{
  QPopupMenu  *pm, *pma;

  //	menu Control:
  ControlMenu = pm = new QPopupMenu(this);
  pm ->insertItem("&Configure",			this, SLOT(ControlConfigure()));
  pm ->insertItem("&Default config",		this, SLOT(ControlDefault()));
  pm ->insertItem("&Flush log",			this, SLOT(ControlFlushLog()));
  pm ->insertSeparator();
  toolbarChk = pm->insertItem("Toggle toolbar",	this, SLOT(toggleToolBar()), CTRL+Key_T);
  labelsChk =  pm->insertItem("Toggle labels",	this, SLOT(toggleTBLabels()), CTRL+Key_L);
  statusbarChk=pm->insertItem("Toggle status bar",this, SLOT(toggleStatusBar()), CTRL+Key_B);
  pm ->insertSeparator();
  pm ->insertItem("Select &Font",		this, SLOT(ControlSelectFont()));
  StyleMenu = pma = new QPopupMenu(this);
  Styles[0] =  pma->insertItem("Motif (Qt Default)",	this, SLOT(ControlStyleDefault()));
  Styles[1] =  pma->insertItem("Motif Plus",		this, SLOT(ControlStyleMotifPlus()));
  Styles[2] =  pma->insertItem("CDE",			this, SLOT(ControlStyleCDE()));
  Styles[3] =  pma->insertItem("SGI",			this, SLOT(ControlStyleSGI()));
  Styles[4] =  pma->insertItem("Interlace",		this, SLOT(ControlStyleInterlace()));
  Styles[5] =  pma->insertItem("Windows",		this, SLOT(ControlStyleWindows()));
  Styles[6] =  pma->insertItem("Platinum",		this, SLOT(ControlStylePlatinum()));
  pm ->insertItem("Select Style", pma);
  pm ->insertSeparator();
  pm ->insertItem("&Quit",			qApp, SLOT(quit()), CTRL+Key_Q);
  menuBar()->insertItem("&Control", pm);
  ControlMenu->setCheckable(TRUE);
  ControlMenu->setItemChecked(toolbarChk, TRUE);
  ControlMenu->setItemChecked(labelsChk, TRUE);
  ControlMenu->setItemChecked(statusbarChk, TRUE);
  StyleMenu->setItemChecked((Styles[7]=Styles[SBSetUp::S_Default]), TRUE);
  changeStyle(SetUp->guiStyle());
  

  //	menu Stuff:
  pm = new QPopupMenu(this);
  //  pma= new QPopupMenu(this);
  //  pma->insertItem("Nutation in longitude..",	this, SLOT(StuffModelsNutPsi()));
  //  pma->insertItem("Nutation in obliquity..",	this, SLOT(StuffModelsNutEps()));
  //  pma->insertItem("UT1 hight tidal variations..",this, SLOT(StuffModelsUT1High()));
  //  pma->insertItem("UT1 low tidal variations..", this, SLOT(StuffModelsUT1Low()));
  //  pma->insertItem("UT1 diurnal variations..",   this, SLOT(StuffModelsUT1Diurn()));
  //  pma->insertItem("Polar diurnal variations..", this, SLOT(StuffModelsPolDiurn()));
  pm ->insertItem("Stations..",			this, SLOT(StuffStations()));
  pm ->insertItem("Sources..",			this, SLOT(StuffSources()));
  pm ->insertItem("EOPs...",			this, SLOT(StuffEOPs()));
  pm ->insertItem("Observations..",		this, SLOT(StuffVLBIObs()));
  pm ->insertItem("Ephemerides..",		this, SLOT(StuffEphemerides()));
  pm ->insertItem("Atm. Loading..",		this, SLOT(StuffAplo()));
  pm ->insertItem("Eccenticities..",		this, SLOT(StuffEccenticities()));
  pm ->insertItem("Plate Motion Model",		this, SLOT(StuffPlateMotion()));
  pm ->insertItem("Generic Models",		this, SLOT(StuffModels()));
  //  pm ->insertItem("Review Models", pma);
  menuBar()->insertItem("&Stuff", pm);

  //	menu Project:
  pm = new QPopupMenu(this);
  pm ->insertItem("Create project..",		this, SLOT(ExecuteGenProj()));
  pm ->insertItem("Select project..",		this, SLOT(ExecuteSelProj()));
  pm ->insertItem("Edit project..",		this, SLOT(ExecuteEditProj()));
  pm ->insertItem("Project's Models..",		this, SLOT(ExecuteModels()));
  pm ->insertItem("Parameters to estimate..",	this, SLOT(ExecuteParameters()));
  pm ->insertSeparator();
  pm ->insertItem("Run project",		this, SLOT(ExecuteRunProj()));
  pm ->insertItem("Double Run",			this, SLOT(ExecuteDoubleRunProj()));
  pm ->insertSeparator();
  pm ->insertItem("Report project",		this, SLOT(ExecuteReportProj()));
  pm ->insertItem("Browse the Solution",	this, SLOT(ExecuteBrowseSolution()));
  pm ->insertSeparator();
  pm ->insertItem("Clear project parameters",	this, SLOT(ExecuteDrop()));
  menuBar()->insertItem("&Project", pm);
  
  //	menu Util:
  pm  = new QPopupMenu(this);
  pm ->insertItem("VLBI Data Preprocessing..",	this, SLOT(UtilitiesPreProc()));
  pm ->insertSeparator();
  pm ->insertItem("VLBI User Info Dump",	this, SLOT(UtilitiesDumpUI()));
  pm ->insertItem("VLBI User Info Restore",	this, SLOT(UtilitiesRestoreUI()));
  pm ->insertSeparator();
  pm ->insertItem("Collect Statistics for Current Project",	this, SLOT(UtilitiesCollectStat4Prj()));
  pm ->insertItem("Collect Statistics for All VLBI Data",	this, SLOT(UtilitiesCollectStat4All()));
  pm ->insertSeparator();
  pm ->insertItem("Browse Statistics for Current Project",	this, SLOT(UtilitiesBrowseStat4Prj()));
  pm ->insertItem("Browse Statistics for All VLBI Data",	this, SLOT(UtilitiesBrowseStat4All()));
  pm ->insertSeparator();
  pm ->insertItem("Spectrum analyser..",	this, SLOT(UtilitiesSpectrum()));
  pm ->insertItem("Gauss Filter..",		this, SLOT(UtilitiesFilters()));
  menuBar()->insertItem("&Utilities", pm);

  //	menu Test:
  pm  = new QPopupMenu(this);
  pm ->insertItem("Matrix of the Precession..",	this, SLOT(TestPrecession()));
  pm ->insertItem("Matrix of the Nutation..",	this, SLOT(TestNutation()));
  pm ->insertItem("Matrix of the Transformation..",	this, SLOT(TestRotation()));
  pm ->insertSeparator();
  pm ->insertItem("Planets/Lunar Ephemerides..",	this, SLOT(TestEphemerides()));
  pm ->insertSeparator();
  pm ->insertItem("EOPs..",			this, SLOT(TestEOPs()));
  pm ->insertItem("Diurnal EOPs..",		this, SLOT(TestDiurnalEOPs()));
  pm ->insertSeparator();
  pm ->insertItem("Solid Tide..",		this, SLOT(TestSolidTide()));
  pm ->insertItem("Ocean Tide..",		this, SLOT(TestOceanTide()));
  pm ->insertItem("Pole Tide..",		this, SLOT(TestPoleTide()));
  pm ->insertItem("Atmospheric Loading..",	this, SLOT(TestAplo()));
  pm ->insertSeparator();
  pm ->insertItem("Test..",			this, SLOT(TestJustATest()));
  menuBar()->insertItem("&Test", pm);
  menuBar()->insertSeparator();

  //	menu Help:
  pm  = new QPopupMenu(this);
  pm ->insertItem("&Help",			this, SLOT(HelpHelp()));
  pm ->insertItem("About &Qt",			this, SLOT(HelpAboutQt()));
  pm ->insertItem("&About",			this, SLOT(HelpAbout()));
  menuBar()->insertItem("&Help", pm);

  menuBar()->setSeparator(QMenuBar::InWindowsStyle);
};

void SBMainWindow::toggleToolBar()
{
  if (ToolBar->isVisible())
    {
      ToolBar->hide();
      ControlMenu->setItemChecked(toolbarChk, FALSE);
    } 
  else 
    {
      ToolBar->show();
      ControlMenu->setItemChecked(toolbarChk, TRUE);
    };
};

void SBMainWindow::toggleTBLabels()
{
  if (usesTextLabel())
    {
      setUsesTextLabel(FALSE);
      ControlMenu->setItemChecked(labelsChk, FALSE);
    } 
  else 
    {
      setUsesTextLabel(TRUE);
      ControlMenu->setItemChecked(labelsChk, TRUE);
    };
};

void SBMainWindow::toggleStatusBar()
{
  if (statusBar()->isVisible())
    {
      statusBar()->hide();
      ControlMenu->setItemChecked(statusbarChk, FALSE);
    } 
  else 
    {
      statusBar()->show();
      ControlMenu->setItemChecked(statusbarChk, TRUE);
    };
};

void SBMainWindow::setMessage(const QString& message_)
{
  StBarMessage->setText(message_);
};

void SBMainWindow::resizeEvent(QResizeEvent* QRE)
{
  QMainWindow::resizeEvent(QRE);
  SetUp->setCentralPanelSize(size());
};

void SBMainWindow::moveEvent(QMoveEvent* QME)
{
  QMainWindow::moveEvent(QME);
  SetUp->setCentralPanelPosition(pos());
};

void SBMainWindow::setRunning(bool isRun)
{
  bStop->setEnabled( isRun);
  bExit->setEnabled(!isRun);
};

void SBMainWindow::stopRunning(void)
{
  IsNeedToStop = TRUE;
};

void SBMainWindow::ControlConfigure()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": ControlConfigure selected");
  (new SBSetupDialog)->show();
};

void SBMainWindow::ControlDefault()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": ControlDefault selected");
  SetUp->SetDefaults();
};

void SBMainWindow::ControlSelectFont()
{
  bool	ok=FALSE;
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": ControlSelectFont selected");
  QFont f = QFontDialog::getFont(&ok, SetUp->defaultFont());
  if (ok)
    {
      QApplication::setFont(f, TRUE);
      SetUp->setDefaultFont(f);
    };
};

void SBMainWindow::ControlStyleDefault()
{
  changeStyle(SBSetUp::S_Default);
};

void SBMainWindow::ControlStyleMotifPlus()
{
  changeStyle(SBSetUp::S_MotifPlus);
};

void SBMainWindow::ControlStyleCDE()
{
  changeStyle(SBSetUp::S_CDE);
};

void SBMainWindow::ControlStyleSGI()
{
  changeStyle(SBSetUp::S_SGI);
};

void SBMainWindow::ControlStyleInterlace()
{
  changeStyle(SBSetUp::S_Interlace);
};

void SBMainWindow::ControlStyleWindows()
{
  changeStyle(SBSetUp::S_Windows);
};

void SBMainWindow::ControlStylePlatinum()
{
  changeStyle(SBSetUp::S_Platinum);
};

void SBMainWindow::changeStyle(SBSetUp::GUIStyle Style)
{
  StyleMenu->setItemChecked(Styles[7], FALSE);
  switch (Style)
    {
    default:
    case SBSetUp::S_Default:
      QApplication::setStyle(new QMotifStyle(TRUE));
      break;
    case SBSetUp::S_MotifPlus:
      QApplication::setStyle(new QMotifPlusStyle(TRUE));
      break;
    case SBSetUp::S_CDE:
      QApplication::setStyle(new QCDEStyle(TRUE));
      break;
    case SBSetUp::S_SGI:
      QApplication::setStyle(new QSGIStyle(TRUE));
      break;
    case SBSetUp::S_Interlace:
      QApplication::setStyle(new QInterlaceStyle);
      break;
    case SBSetUp::S_Windows:
      QApplication::setStyle(new QWindowsStyle);
      break;
    case SBSetUp::S_Platinum:
      QApplication::setStyle(new QPlatinumStyle);
      break;
    };
  StyleMenu->setItemChecked((Styles[7]=Styles[Style]), TRUE);
  SetUp->setGUIStyle(Style);
};


void SBMainWindow::ControlFlushLog()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": ControlFlushLog selected");
  Log->clearSpool();
};

void SBMainWindow::StuffStations()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": StuffStations selected");
  (new SBStuffStations)->show();
};

void SBMainWindow::StuffSources()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": StuffSources selected");
  (new SBStuffSources)->show();
};

void SBMainWindow::StuffEOPs()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": StuffEOPs selected");
  (new SBStuffEOPs)->show();
};

void SBMainWindow::StuffVLBIObs()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": UtilitiesEditObs selected");
  (new SBVLBISetView)->show();
};

void SBMainWindow::StuffEphemerides()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": StuffEphemerides selected");
  (new SBStuffEphem)->show();
};

void SBMainWindow::StuffAplo()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": StuffAplo selected");
  (new SBStuffAplo)->show();
};

void SBMainWindow::StuffEccenticities()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": StuffEccenticities selected");
};

void SBMainWindow::StuffPlateMotion()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": StuffPlateMotion selected");
};

void SBMainWindow::StuffModels()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": StuffModels selected");
  (new SBModelEditor(&Config))->show();
};

void SBMainWindow::StuffModelsNutPsi()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": StuffModelsNutPsi selected");
};

void SBMainWindow::StuffModelsNutEps()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": StuffModelsNutEps selected");
};

void SBMainWindow::StuffModelsUT1High()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": StuffModelsUT1High selected");
};

void SBMainWindow::StuffModelsUT1Low()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": StuffModelsUT1Low selected");
};

void SBMainWindow::StuffModelsUT1Diurn()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": StuffModelsUT1Diurn selected");
};

void SBMainWindow::StuffModelsPolDiurn()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": StuffModelsPolDiurn selected");
};

void SBMainWindow::ExecuteGenProj()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": ExecuteGenProj selected");
  (new SBProjectCreate)->show();
};

void SBMainWindow::ExecuteSelProj()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": ExecuteSelProj selected");
  (new SBProjectSel)->show();
};

void SBMainWindow::ExecuteEditProj()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": ExecuteEditProj selected");
  if (QFileInfo(SetUp->ProjectFileName()).exists())
    (new SBProjectEdit)->show();
  else
    Log->write(SBLog::WRN, SBLog::INTERFACE, ClassName() + 
	       ": can't edit project: project [" + SetUp->project() + "] doesn't exist");
};

void SBMainWindow::ExecuteModels()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": ExecuteModels selected");
  if (QFileInfo(SetUp->ProjectFileName()).exists())
    (new SBModelEditor(NULL))->show();
  else
    Log->write(SBLog::WRN, SBLog::INTERFACE, ClassName() + 
	       ": can't edit models: project [" + SetUp->project() + "] doesn't exist");
};

void SBMainWindow::ExecuteParameters()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": ExecuteParameters selected");
  if (QFileInfo(SetUp->ProjectFileName()).exists())
    (new SBParametersEditor(NULL))->show();
  else
    Log->write(SBLog::WRN, SBLog::INTERFACE, ClassName() + 
	       ": can't edit parameters: project [" + SetUp->project() + "] doesn't exist");
};

void SBMainWindow::ExecuteRunProj()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": ExecuteRunTask selected");
  if (QFileInfo(SetUp->ProjectFileName()).exists())
    {
      emit run(TRUE);
      qApp->processEvents();
      stBarReset();
      Log->write(SBLog::DBG, SBLog::RUN, ClassName() + ": Starting project executing");

      SBRunManager *Runner=new SBRunManager();
      Runner->process();
      delete Runner;
      
      if(IsNeedToStop)
	{
	  Log->write(SBLog::DBG, SBLog::RUN, ClassName() + ": Project aborted");
	  IsNeedToStop=FALSE;
	}
      else
	Log->write(SBLog::DBG, SBLog::RUN, ClassName() + ": Project finished");
      
      stBarReset();
      emit run(FALSE);
    }
  else
    Log->write(SBLog::WRN, SBLog::INTERFACE, ClassName() + 
	       ": can't run project: project [" + SetUp->project() + "] doesn't exist");
};

void SBMainWindow::ExecuteDoubleRunProj()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": ExecuteRunTask selected");
  if (QFileInfo(SetUp->ProjectFileName()).exists())
    {
      emit run(TRUE);
      qApp->processEvents();
      stBarReset();
      Log->write(SBLog::DBG, SBLog::RUN, ClassName() + ": Starting project executing");

      SBRunManager *Runner=new SBRunManager();
      Runner->process();
      delete Runner;

      // make a double:
      stBarReset();
      Log->write(SBLog::DBG, SBLog::RUN, ClassName() + ": Starting project executing");

      Runner=new SBRunManager();
      Runner->process();
      delete Runner;
      
      if(IsNeedToStop)
	{
	  Log->write(SBLog::DBG, SBLog::RUN, ClassName() + ": Project aborted");
	  IsNeedToStop=FALSE;
	}
      else
	Log->write(SBLog::DBG, SBLog::RUN, ClassName() + ": Project finished");
      
      stBarReset();
      emit run(FALSE);
    }
  else
    Log->write(SBLog::WRN, SBLog::INTERFACE, ClassName() + 
	       ": can't run project: project [" + SetUp->project() + "] doesn't exist");
};

void SBMainWindow::ExecuteReportProj()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": ExecuteReportTask selected");
  if (QFileInfo(SetUp->ProjectFileName()).exists())
    {
      QApplication::setOverrideCursor(Qt::waitCursor);  
      SBRunManager *Runner=new SBRunManager();
      Runner->makeReports();
      delete Runner;
      QApplication::restoreOverrideCursor();
    }
  else
    Log->write(SBLog::WRN, SBLog::INTERFACE, ClassName() + 
	       ": can't run project: project [" + SetUp->project() + "] doesn't exist");
};

void SBMainWindow::ExecuteBrowseSolution()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": ExecuteBrowseSolution selected");
  (new SBSolutionBrowser)->show();
};

void SBMainWindow::ExecuteDrop()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": ExecuteDrop selected");
  if (QMessageBox::warning(0, Version.selfName(),
			   "Are you sure to delete the solution of the project?\n\nCurrent Project: \"" +
			   SetUp->project() + "\"\n", "No", "Yes"))
    {
      SBProject *Prj = SetUp->loadProject();
      SBSolution *Solution = new SBSolution(Prj, FALSE);
      Solution->deleteSolution();
      delete Solution;
      delete Prj;
    };
};

void SBMainWindow::UtilitiesPreProc()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": UtilitiesPreProc selected");
  (new SBVLBIPreProcess)->show();
};

void SBMainWindow::UtilitiesSpectrum()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": UtilitiesSpectrum selected");
  (new SBSpectrumAnalyserMEM)->show();
};

void SBMainWindow::UtilitiesFilters()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": UtilitiesFilters selected");
  (new SBFilteringGauss)->show();
};

void SBMainWindow::UtilitiesDumpUI()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": UtilitiesDumpUI selected");
  SBVLBISet *Wvlbi=SetUp->loadVLBI();
  //  Wvlbi->dumpUserInfo(&mainPrBarSetProgress);
  Wvlbi->dumpUserInfo();
  delete Wvlbi;
};

void SBMainWindow::UtilitiesRestoreUI()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": UtilitiesRestoreUI selected");
  SBVLBISet *Wvlbi=SetUp->loadVLBI();
  Wvlbi->restoreUserInfo();
  delete Wvlbi;
};

void SBMainWindow::UtilitiesCollectStat4Prj()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": UtilitiesCollectStat4Prj selected");
  SBProject *Prj=SetUp->loadProject();
  if (Prj)
    {
      SBObsVLBIStatistics* Statistics = new SBObsVLBIStatistics("Statistics for the project " + Prj->name());
      Statistics->collectStatistics(Prj->VLBIsessionList());
      SetUp->saveObsVLBIStatistics(Statistics);
      delete Statistics;
      delete Prj;
    }
  else
    Log->write(SBLog::ERR, SBLog::INTERFACE, ClassName() + ": loaded NULL project");
};

void SBMainWindow::UtilitiesCollectStat4All()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": UtilitiesCollectStat4All selected");
  SBObsVLBIStatistics* Statistics = new SBObsVLBIStatistics("Statistics for all VLBI data");
  Statistics->collectStatistics();
  SetUp->saveObsVLBIStatistics(Statistics);
  delete Statistics;
};

void SBMainWindow::UtilitiesBrowseStat4Prj()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": UtilitiesBrowseStat4Prj selected");
  (new SBObsVLBIStatBrowser)->show();
};

void SBMainWindow::UtilitiesBrowseStat4All()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": UtilitiesBrowseStat4All selected");
  (new SBObsVLBIStatBrowser(TRUE))->show();
};

void SBMainWindow::TestPrecession()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": TestPrecession selected");
  (new SBTestPrecession)->show();
};

void SBMainWindow::TestNutation()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": TestNutation selected");
  (new SBTestNutation)->show();
};

void SBMainWindow::TestRotation()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": TestRotation selected");
  (new SBTestFrame)->show();
};

void SBMainWindow::TestEphemerides()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": TestEphemerides selected");
  (new SBTestEphem)->show();
};

void SBMainWindow::TestEOPs()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": TestEOPs selected");
  (new SBTestEOP)->show();
};

void SBMainWindow::TestDiurnalEOPs()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": TestDiurnalEOPs selected");
  (new SBTestDiurnEOP)->show();
};

void SBMainWindow::TestSolidTide()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": TestSolidTide selected");
  (new SBTestSolidTides)->show();
};

void SBMainWindow::TestOceanTide()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": TestOceanTide selected");
  (new SBTestOceanTides)->show();
};

void SBMainWindow::TestPoleTide()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": TestPoleTide selected");
  (new SBTestPolarTides)->show();
};

void SBMainWindow::TestAplo()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": TestAplo selected");
  (new SBTestAPLoad)->show();
};

void SBMainWindow::TestJustATest()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": TestJustATest selected");

  /*
   * the body of a test
   *
   */  

  //  int	*qqq;
  //  qqq = NULL;
  //  std::cout << *qqq << "\n";
  
  
  /* end of a test */
};

void SBMainWindow::HelpHelp()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": HelpHelp selected");
};

void SBMainWindow::HelpAboutQt()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": HelpAboutQt selected");
  QMessageBox::aboutQt(this, Version.selfName());
};

void SBMainWindow::HelpAbout()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": HelpAbout selected");

  QString Tmp = 
    "This is " + Version.selfName() + " version " + Version.toString() + ".\n"
    "The software for data processing of the geodetic VLBI observations.\n\n"

    "The main purpose of the " + Version.selfName() + " is to make the VLBI data\n"
    "processing easy and clear to understand procedure.\n\n"

    "It is developing at:\n"
    "\t1) Main Astronomical Observatory, Kiev (http://www.mao.kiev.ua);\n"
    "List of authors:\n"
    "\t1) Sergei Bolotin (mailto:bolotin@mao.kiev.ua);\n\n"

    "This software is distributed under the GNU Public License,\n"
    "see http://www.gnu.org for more info.\n\n"
    
    "The binary assembled by " + SBCompilerUserName + " <" + SBCompilerLoginName +
    "@" + SBCompilerHostName + "." + SBCompilerDomainName + ">\non " + SBCompilerDate + 
    ", " + SBCompilerTime + " with compiler " + SBCompilerName + " ver. " + SBCompilerVersion + 
    "\nfor " + SBCompilerOS + "\n\n" + 
    Version.selfName() + " home page: \thttp://sourceforge.net/projects/steelbreeze\n" +
    Version.selfName() + " ftp site: \tftp://steelbreeze.sourceforge.net/pub/steelbreeze/\n" +
    Version.selfName() + " mail lists: \n"
    "announcements: \tmailto:steelbreeze-announce@lists.sourceforge.net\n"
    "developers: \tmailto:steelbreeze-develop@lists.sourceforge.net\n";
  
  QMessageBox::about(this, Version.selfName(), Tmp);
};

void SBMainWindow::stBarReset ()
{
  StBarProgress->reset();
};

void SBMainWindow::stBarSetTotalSteps (int ts)
{
  StBarProgress->setTotalSteps(ts);
};

void SBMainWindow::stBarSetProgress   (int p)
{
  StBarProgress->setProgress(p);
};

void SBMainWindow::setTRFName(const QString& N_)
{
  CentralWidget->TRFName->setText(N_);
  CentralWidget->TRFName->setMinimumSize(CentralWidget->TRFName->sizeHint());
};

void SBMainWindow::setCRFName(const QString& N_)
{
  CentralWidget->CRFName->setText(N_);
  CentralWidget->CRFName->setMinimumSize(CentralWidget->CRFName->sizeHint());
};

void SBMainWindow::setEOPName(const QString& N_)
{
  CentralWidget->EOPName->setText(N_);
  CentralWidget->EOPName->setMinimumSize(CentralWidget->EOPName->sizeHint());
};

void SBMainWindow::setPrjName(const QString& N_)
{
  CentralWidget->PrjName->setText(N_);
  CentralWidget->PrjName->setMinimumSize(CentralWidget->PrjName->sizeHint());
};

void SBMainWindow::setStatus(const QString& S_)
{
  CentralWidget->Status->setText(S_);
  CentralWidget->Status->setMinimumSize(CentralWidget->Status->sizeHint());
};

void SBMainWindow::setOperation(const QString& O_)
{
  CentralWidget->Operation->setText(O_);
  CentralWidget->Operation->setMinimumSize(CentralWidget->Operation->sizeHint());
};

void SBMainWindow::setProcSes(int n, int Total)
{
  CentralWidget->ProcSess->setText(QString("_").sprintf("%d/%d", n, Total));
  CentralWidget->ProcSess->setMinimumSize(CentralWidget->ProcSess->sizeHint());
};

void SBMainWindow::setProcObs(int n, int Total)
{
  CentralWidget->ProcObs->setText(QString("_").sprintf("%d/%d", n, Total));
  CentralWidget->ProcObs->setMinimumSize(CentralWidget->ProcObs->sizeHint());
};

void SBMainWindow::setSessName(const QString& N_)
{
  CentralWidget->SessName->setText(N_);
  CentralWidget->SessName->setMinimumSize(CentralWidget->SessName->sizeHint());
};

void SBMainWindow::setCacheSizes(int nObs, int nSes, int nCRF, int nTRF)
{
  CentralWidget->CasheSizes->setText(QString("_").sprintf("%d/%d/%d/%d", nObs, nSes, nCRF, nTRF));
  CentralWidget->CasheSizes->setMinimumSize(CentralWidget->CasheSizes->sizeHint());
};
/*==============================================================================================*/



/*==============================================================================================*/
void mainMessage(const QString& S)	{MainWin->setMessage(S);};
void mainPrBarReset()			{MainWin->stBarReset();};
void mainPrBarSetTotalSteps(int ts)	{MainWin->stBarSetTotalSteps(ts);};
void mainPrBarSetProgress(int p)	{MainWin->stBarSetProgress(p);};
//General Info:
void mainSetTRFName(const QString& S)	{MainWin->setTRFName(S);};
void mainSetCRFName(const QString& S)	{MainWin->setCRFName(S);};
void mainSetEOPName(const QString& S)	{MainWin->setEOPName(S);};
void mainSetPrjName(const QString& S)	{MainWin->setPrjName(S);};
void mainSetStatus(const QString& S)	{MainWin->setStatus(S);};
//Detail Info:
void mainSetOperation(const QString& S)	{MainWin->setOperation(S);};
void mainSetProcSes(int n, int Total)	{MainWin->setProcSes(n, Total);};
void mainSetProcObs(int n, int Total)	{MainWin->setProcObs(n, Total);};
void mainSetSessName(const QString& N_)	{MainWin->setSessName(N_);};
void mainSetCacheSizes(int nObs, int nSes, int nCRF, int nTRF)
{MainWin->setCacheSizes(nObs, nSes, nCRF, nTRF);};
/*==============================================================================================*/
