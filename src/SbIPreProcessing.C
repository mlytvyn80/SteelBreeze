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

#include "SbIPreProcessing.H"

#include <qaccel.h>
#include <qapplication.h>
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qspinbox.h>
#include <qtabwidget.h>



#include "SbSetup.H"
#include "SbGeoDelay.H"
#include "SbGeoProject.H"
#include "SbVector.H"
#include "SbIMainWin.H"
#include "SbIPlot.H"





/*==============================================================================================*/
/*												*/
/* SBVLBIObsLI..										*/
/*												*/
/*												*/
/*==============================================================================================*/
QString SBVLBIObsPPLI::text(int col) const
{
  double Scale  = SetUp->scale();
  TmpString = "NULL";
  if (Obs && Sess)
    switch (col)
      {
      case  0: TmpString.sprintf("%6d", Obs->idx());
	break;
      case  1: TmpString = (const char*)(" " + Obs->toString(SBMJD::F_HHMMSS) + " ");
	break;
      case  2: TmpString = Sess->stationInfo(Obs->station_1Idx())->aka().copy();
	break;
      case  3: TmpString = Sess->stationInfo(Obs->station_2Idx())->aka().copy();
	break;
      case  4: TmpString = Sess->sourceInfo(Obs->sourceIdx())->aka().copy();
	break;
      case  5: TmpString.sprintf(Scale*Obs->delayErr()<1000.0?"%10.6f":"%14.6g", Scale*Obs->delayErr());
	break;
      case  6: 
	if (Obs->delayRes()==0.0)
	  TmpString = "0.0";
	else
	  TmpString.sprintf("%10.6f", Scale*Obs->delayRes());
	break;
      case  7: TmpString.sprintf("%3d", Obs->ambiguityFactor());
	break;
      case  8: TmpString = Obs->qualCode().copy();
	break;
      case  9: TmpString.sprintf("%d:%d", Obs->dUFlag(), Obs->rUFlag());
	break;
      case 10: TmpString.sprintf("%3d", Obs->ionErrorFlag());
	break;
      case 11: 
	TmpString = "";
	if (Obs->isAttr(SBObsVLBIEntry::BadCable1))
	  TmpString+="c";
	if (Obs->isAttr(SBObsVLBIEntry::BadMeteo1))
	  TmpString+="m";
	break;
      case 12: 
	TmpString = "";
	if (Obs->isAttr(SBObsVLBIEntry::BadCable2))
	  TmpString+="c";
	if (Obs->isAttr(SBObsVLBIEntry::BadMeteo2))
	  TmpString+="m";
	break;
      case 13: TmpString.sprintf("%c", Obs->isAttr(SBObsVLBIEntry::breakClock1)?'B':' ');
	break;
      case 14: TmpString.sprintf("%c", Obs->isAttr(SBObsVLBIEntry::breakClock2)?'B':' ');
	break;
      case 15: TmpString.sprintf("%c", Obs->isAttr(SBObsVLBIEntry::notValid)?'X':' ');
	break;
      };
  return TmpString;
};

QString SBVLBIObsPPLI::key(int col, bool) const
{
  TmpString = "NULL";
  if (Obs && Sess)
    switch (col)
      {
      case  0: TmpString.sprintf("%06d", Obs->idx());
	break;
      case  1: TmpString.sprintf("%12.6f", (double)Obs->mjd());
	break;
      case  2: TmpString = Sess->stationInfo(Obs->station_1Idx())->aka().copy();
	break;
      case  3: TmpString = Sess->stationInfo(Obs->station_2Idx())->aka().copy();
	break;
      case  4: TmpString = Sess->sourceInfo(Obs->sourceIdx())->aka().copy();
	break;
      case  5: TmpString.sprintf("%010.6f", 1.0e9*Obs->delayErr());
	break;
      case  6: TmpString.sprintf("%030.16f", 1000+Obs->delayRes());
	break;
      case  7: TmpString.sprintf("%03d", 100+Obs->ambiguityFactor());
	break;
      case  8: TmpString = Obs->qualCode().copy();
	break;
      case  9: TmpString.sprintf("%02d:%02d", 10+Obs->dUFlag(), 10+Obs->rUFlag());
	break;
      case 10: TmpString.sprintf("%03d", 10+Obs->ionErrorFlag());
	break;
      case 11: 
	TmpString = "";
	if (Obs->isAttr(SBObsVLBIEntry::BadCable1))
	  TmpString+="c";
	if (Obs->isAttr(SBObsVLBIEntry::BadMeteo1))
	  TmpString+="m";
	break;
      case 12: 
	TmpString = "";
	if (Obs->isAttr(SBObsVLBIEntry::BadCable2))
	  TmpString+="c";
	if (Obs->isAttr(SBObsVLBIEntry::BadMeteo2))
	  TmpString+="m";
	break;
      case 13: TmpString.sprintf("%c", Obs->isAttr(SBObsVLBIEntry::breakClock1)?'B':' ');
	break;
      case 14: TmpString.sprintf("%c", Obs->isAttr(SBObsVLBIEntry::breakClock2)?'B':' ');
	break;
      case 15: TmpString.sprintf("%c", Obs->isAttr(SBObsVLBIEntry::notValid)?'X':' ');
	break;
      };
  return TmpString;
};

void SBVLBIObsPPLI::paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int align)
{
  if (Obs && Sess && (column<=6 || column==9))
    {
//    if (Obs->dUFlag()>0 && Obs->isAttr(SBObsVLBIEntry::notValid))
      if ( Obs->delayRes()==0.0 || Obs->isAttr(SBObsVLBIEntry::notValid))
        {
          if (!isSelected())
            {
//            p->setBackgroundColor(QColor(229,229,229));
              p->setBackgroundColor(QColor(209,209,209));
              p->setBackgroundMode(OpaqueMode);
            };
	}
      else if (Obs->dUFlag()>0)
        {
//        p->setBackgroundColor(QColor(233, 140, 155));
          p->setBackgroundColor(QColor(255, 120, 155));
          p->setBackgroundMode(OpaqueMode);
        }
      /*
      else if (Obs->dUFlag()==2 || Obs->dUFlag()==4)
        {
          p->setBackgroundColor(QColor(217, 141, 153));
          p->setBackgroundMode(OpaqueMode);
        }
      else if (Obs->dUFlag()>=8)
        {
          p->setBackgroundColor(QColor(248,  98, 102));
          p->setBackgroundMode(OpaqueMode);
        }
    */
    };
  QListViewItem::paintCell(p, cg, column, width, align);
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBCableLI..											*/
/*												*/
/*												*/
/*==============================================================================================*/
QString SBCableLI::text(int col) const
{
  TmpString = "NULL";
  if (SI && Par)
    switch (col)
      {
      case  0: TmpString.sprintf("%6d", SI->idx());
	break;
      case  1: TmpString = SI->aka().copy();
	break;
      case  2: TmpString = SI->toString().copy();//id
	break;
      case  3: TmpString.sprintf("%.4f", Par->v());
	break;
      case  4: TmpString.sprintf("%.4f", Par->e());
	break;
      };
  return TmpString;
};

QString SBCableLI::key(int col, bool) const
{
  TmpString = "NULL";
  if (SI && Par)
    switch (col)
      {
      case  0: TmpString.sprintf("%06d", SI->idx());
	break;
      case  1: TmpString = SI->aka().copy();
	break;
      case  2: TmpString = SI->toString().copy();//id
	break;
      case  3: TmpString.sprintf("%024.16f", Par->v());
	break;
      case  4: TmpString.sprintf("%024.16f", Par->e());
	break;
      };
  return TmpString;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBCablesBrowser..										*/
/*												*/
/*==============================================================================================*/
SBCablesBrowser::SBCablesBrowser(SBStationInfoList* Stations_, SBParameterList* Cables_, 
				 const QString& SessionName_,
				 QWidget* parent, const char* name, bool modal, WFlags f)
  : QDialog(parent, name, modal, f)
{
  Cables = Cables_;

  SBStationInfo *SI;
  for (SI=Stations_->first(); SI; SI=Stations_->next())
    Stations.inSort(new SBStationInfo(*SI));
  
  setCaption("Multipliers of Cable correction for the session " + SessionName_);
  QGroupBox	*gbox;
  QBoxLayout	*Layout, *SubLayout;

  gbox	 = new QGroupBox("Stations and Cables [" + SessionName_ + "]", this);
  Layout = new QVBoxLayout(gbox, 20, 10);

  CablesLV = new QListView(gbox);
  CablesLV -> addColumn("Idx");
  CablesLV -> setColumnAlignment( 0, AlignRight);
  CablesLV -> addColumn("DBH Name");
  CablesLV -> setColumnAlignment( 1, AlignLeft);
  CablesLV -> addColumn("Station ID");
  CablesLV -> setColumnAlignment( 2, AlignRight);
  CablesLV -> addColumn("Cable");
  CablesLV -> setColumnAlignment(3, AlignRight);
  CablesLV -> addColumn("E[Cable]");
  CablesLV -> setColumnAlignment(4, AlignRight);

  SBStationID		id;
  QString		Str;
  for (SBParameter *p=Cables->first(); p; p=Cables->next())
    {
      Str = p->name().mid(4, 9);
      if (id.isValidStr(Str))
	{
	  id.setID(Str);
	  if (!(SI=Stations.find(&id)))
	    {
	      SI=Stations.first();
	      Log->write(SBLog::ERR, SBLog::PREPROC, ClassName() + ": cannot find Station [" + 
			 id.toString() + " in the station list; parameter: [" + p->name() + "]");
	    };
	  (void) new SBCableLI(CablesLV, p, SI);
	}
      else
	Log->write(SBLog::ERR, SBLog::PREPROC, ClassName() + ": cannot convert parameter name [" +
		   p->name() + "] to Station ID");
    };
  
  CablesLV -> setAllColumnsShowFocus(TRUE);
  CablesLV -> setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  if (CablesLV->firstChild())
    CablesLV->setSelected(CablesLV->firstChild(), TRUE);

  CablesLV->setFocusPolicy(QWidget::StrongFocus);
  CablesLV->setMinimumHeight(CablesLV->sizeHint().height());
  Layout -> addWidget(CablesLV, 5);
  
  Layout -> activate();


  //---
  Layout = new QVBoxLayout(this, 10, 10);
  Layout -> addWidget(gbox, 10);

  SubLayout = new QHBoxLayout(10);
  Layout -> addLayout(SubLayout);
  SubLayout-> addStretch(1);

  QPushButton *Cancel	= new QPushButton("Close", this);
  Cancel->setDefault(TRUE);

  Cancel->setMinimumSize(Cancel->sizeHint());
  SubLayout->addWidget(Cancel);
  connect(Cancel, SIGNAL(clicked()), SLOT(reject()));
  Layout->activate();
};

SBCablesBrowser::~SBCablesBrowser()
{
  if (Cables)
    {
      delete Cables;
      Cables = NULL;
    };
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* My_QListView..										*/
/*												*/
/*												*/
/*==============================================================================================*/
My_QListView::My_QListView(QWidget* parent_, const char* name_) 
  : QListView(parent_, name_)
{
  IsMoving	= FALSE;
  MItem		= NULL;
  MCol		= -1;
  MouseButtonState = Qt::NoButton;
  connect(this, 
	  SIGNAL(pressed      (QListViewItem*, const QPoint&, int)), 
	  SLOT  (movingStarted(QListViewItem*, const QPoint&, int)));
};

void My_QListView::movingStarted(QListViewItem* item, const QPoint&, int c)
{
  if (item)
    {
      MItem	= item;
      MCol	= c;
      IsMoving	= TRUE;
      emit moveUponItem(MItem, MCol, MouseButtonState);
    };
};

void My_QListView::contentsMouseMoveEvent(QMouseEvent* e)
{
  if (e && IsMoving)
    {
      QPoint vp = contentsToViewport(e->pos());
      QListViewItem *i = itemAt(vp);
      if (i && i!=MItem)
	{
	  MItem=i;
	  emit moveUponItem(MItem, MCol, MouseButtonState);
	};
    };
  
  QListView::contentsMouseMoveEvent(e);
};

void My_QListView::contentsMousePressEvent(QMouseEvent* e)
{
  if (e)
   {
	MouseButtonState = e->button();
   }
  QListView::contentsMousePressEvent(e);
};

void My_QListView::contentsMouseReleaseEvent(QMouseEvent* e)
{
  MItem		= NULL;
  MCol		= -1;
  IsMoving	= FALSE;
  MouseButtonState = e->button();
  QListView::contentsMouseReleaseEvent(e);
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBVLBIPreProcess..										*/
/*												*/
/*==============================================================================================*/
const char* SBVLBIPreProcess::sUFlags[] = 
{
  "A","B","C","D","E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z"
};
SBVLBIPreProcess::SBVLBIPreProcess(QWidget* parent, const char* name, bool modal, WFlags f)
  : SBVLBISetViewBase(parent, name, modal, f)
{
  ObservLV	= NULL;
  CurrentSesLI	= NULL;
  SesInfo	= NULL;
  Session	= NULL;
  brwsSta	= NULL;
  brwsSou	= NULL;
  brwsBas	= NULL;
  brwsHist	= NULL;

  Clear		= NULL;
  Run		= NULL;
  Fix		= NULL;
  Ok		= NULL;
  DumpUI	= NULL;
  RestoreUI	= NULL;
  accel		= NULL;

  IsNeedEstimateCables	= FALSE;

  IsBlockedSessionList	= FALSE;
  IsBlockedStationList	= FALSE;
  IsBlockedHistoryList	= FALSE;
  IsBlockedClearButton	= FALSE;
  IsBlockedRunButton	= FALSE;
  IsBlockedFixButton	= FALSE;
  IsBlockedOkButton	= FALSE;
  IsBlockedDumpUIButton = FALSE;
  IsBlockedRestoreUIButton=FALSE;
  IsLongRun		= FALSE;

  NumAuxWindows		= 0;
  AmbigJump		= 1;


  PC_Bases	= new SBPlotCarrier( 9, 4, "unnamed yet");
  // names of columns:
  PC_Bases->columnNames()->append(new QString("Time (UTC)"));					// 0
  PC_Bases->setType(0, SBPlotCarrier::AT_MJD);
  PC_Bases->columnNames()->append(new QString("Delay(o-c), " + SetUp->scaleName()));		// 1
  PC_Bases->columnNames()->append(new QString("Delay, " + SetUp->scaleName()));			// 2
  PC_Bases->columnNames()->append(new QString("Rate(o-c), ps/s"));				// 3
  PC_Bases->columnNames()->append(new QString("Rate, ps/s"));					// 4
  PC_Bases->columnNames()->append(new QString("Ion.delay, " + SetUp->scaleName()));		// 5
  PC_Bases->columnNames()->append(new QString("Ion.rate, ps/s"));				// 6
  PC_Bases->columnNames()->append(new QString("Phase, deg"));					// 7
  PC_Bases->columnNames()->append(new QString("Correlation"));					// 8
  
  // std vars:
  PC_Bases->columnNames()->append(new QString("Delay sig, " + SetUp->scaleName()));		// 9
  PC_Bases->columnNames()->append(new QString("Rate sig, ps/s"));				//10
  PC_Bases->columnNames()->append(new QString("Ion.D. sig, " + SetUp->scaleName()));		//11
  PC_Bases->columnNames()->append(new QString("Ion.R. sig, ps/s"));				//12
  
  PC_Bases->setStdVarIdx( 1,  9);
  PC_Bases->setStdVarIdx( 2,  9);
  PC_Bases->setStdVarIdx( 3, 10);
  PC_Bases->setStdVarIdx( 4, 10);
  PC_Bases->setStdVarIdx( 5, 11);
  PC_Bases->setStdVarIdx( 6, 12);


  PC_Stations	= new SBPlotCarrier( 8, 2, "unnamed yet");
  // names of columns:
  PC_Stations->columnNames()->append(new QString("Time (UTC)"));				// 0
  PC_Stations->setType(0, SBPlotCarrier::AT_MJD);
  PC_Stations->columnNames()->append(new QString("Cable cal., " + SetUp->scaleName()));		// 1
  PC_Stations->columnNames()->append(new QString("Met.: temp., C"));				// 2
  PC_Stations->columnNames()->append(new QString("Met.: press., mb"));				// 3
  PC_Stations->columnNames()->append(new QString("Met.: rel.hum., %"));				// 4
  PC_Stations->columnNames()->append(new QString("Tot ZDelay, " + SetUp->scaleName()));		// 5
  PC_Stations->columnNames()->append(new QString("Stc.Clocks, ps"));				// 6
  PC_Stations->columnNames()->append(new QString("Stc.Zenith, cm"));				// 7
  
  // std vars:
  PC_Stations->columnNames()->append(new QString("E[Clocks], ps"));				// 8
  PC_Stations->columnNames()->append(new QString("E[WZdelay], cm"));				// 9
  PC_Stations->setStdVarIdx( 6,  8);
  PC_Stations->setStdVarIdx( 5,  9);
  PC_Stations->setStdVarIdx( 7,  9);



  PC_Sources = new SBPlotCarrier( 9, 4, "unnamed yet");
  // names of columns:
  PC_Sources->columnNames()->append(new QString("Time (UTC)"));					// 0
  PC_Sources->setType(0, SBPlotCarrier::AT_MJD);
  PC_Sources->columnNames()->append(new QString("Delay(o-c), " + SetUp->scaleName()));		// 1
  PC_Sources->columnNames()->append(new QString("Delay, " + SetUp->scaleName()));		// 2
  PC_Sources->columnNames()->append(new QString("Rate(o-c), ps/s"));				// 3
  PC_Sources->columnNames()->append(new QString("Rate, ps/s"));					// 4
  PC_Sources->columnNames()->append(new QString("Ion.delay, " + SetUp->scaleName()));		// 5
  PC_Sources->columnNames()->append(new QString("Ion.rate, ps/s"));				// 6
  PC_Sources->columnNames()->append(new QString("Phase, deg"));					// 7
  PC_Sources->columnNames()->append(new QString("Correlation"));				// 8
  
  // std vars:
  PC_Sources->columnNames()->append(new QString("Delay sig, " + SetUp->scaleName()));		// 9
  PC_Sources->columnNames()->append(new QString("Rate sig, ps/s"));				//10
  PC_Sources->columnNames()->append(new QString("Ion.D. sig, " + SetUp->scaleName()));		//11
  PC_Sources->columnNames()->append(new QString("Ion.R. sig, ps/s"));				//12
  
  PC_Sources->setStdVarIdx( 1,  9);
  PC_Sources->setStdVarIdx( 2,  9);
  PC_Sources->setStdVarIdx( 3, 10);
  PC_Sources->setStdVarIdx( 4, 10);
  PC_Sources->setStdVarIdx( 5, 11);
  PC_Sources->setStdVarIdx( 6, 12);


  // eof plots  

  Prj		= NULL;
  Solution	= NULL;
  IsRunning	= FALSE;
  init();
};

SBVLBIPreProcess::~SBVLBIPreProcess()
{
  if (Solution)
    {
      Solution->deleteSolution(); // clear a disk
      delete Solution;
      Solution = NULL;
    };
  if (Prj)
    {
      delete Prj;
      Prj = NULL;
    };
  if (PC_Bases)
    {
      delete PC_Bases;
      PC_Bases=NULL;
    };

  if (PC_Stations)
    {
      delete PC_Stations;
      PC_Stations=NULL;
    };

  if (PC_Sources)
    {
      delete PC_Sources;
      PC_Sources=NULL;
    };

  if (Session && SesInfo && isModified)
    {
      *SesInfo = *Session;
      Wvlbi->saveSession(Session);
    };
  if (Session) 
    delete Session;
};

void SBVLBIPreProcess::init()
{
  setCaption("VLBI PreProcessing");
  QBoxLayout *Layout, *SubLayout, *aLayout;

  Layout = new QVBoxLayout(this, 20, 10);
  SubLayout = new QHBoxLayout(10);
  Layout->addLayout(SubLayout);

  aLayout = new QVBoxLayout(10);
  SubLayout->addLayout(aLayout);

  aLayout->addWidget(VLBIList(this, BM_PREPROCESS, NULL));

  aLayout = new QVBoxLayout(10);
  SubLayout->addLayout(aLayout);

  aLayout->addWidget(sesTab(this), 5); // a la SessionEditor

  aLayout->addWidget(wEstPars(this));



  //---
  SubLayout = new QHBoxLayout(10);
  Layout -> addLayout(SubLayout);
  SubLayout-> addStretch(1);

  DumpUI    = new QPushButton("Save UI", this);
  RestoreUI = new QPushButton("Load UI", this);
  Clear	= new QPushButton("&Clear", this);
  Run	= new QPushButton("&PreProcess", this);
  Fix	= new QPushButton("&Fix Session", this);
  Ok	= new QPushButton("Close", this);
  Ok->setDefault(TRUE);

  QSize	BtnSize;
  Fix  ->setMinimumSize((BtnSize=Fix->sizeHint()));
  Clear->setMinimumSize(BtnSize);
  Ok   ->setMinimumSize(BtnSize);
  Run  ->setMinimumSize(BtnSize);
  DumpUI->setMinimumSize(BtnSize);
  RestoreUI->setMinimumSize(BtnSize);

  SubLayout->addWidget(DumpUI);
  SubLayout->addWidget(RestoreUI);
  SubLayout->addWidget(Clear);
  SubLayout->addWidget(Run);
  SubLayout->addWidget(Fix);
  SubLayout->addWidget(Ok);

  //  Clear->setFocusPolicy(QWidget::ClickFocus);
  Run  ->setFocus();
  Run  ->setFocusPolicy(QWidget::ClickFocus);
  //  Fix  ->setFocusPolicy(QWidget::ClickFocus);

  connect(DumpUI,     SIGNAL(clicked()), SLOT(dumpUI()));
  connect(RestoreUI,  SIGNAL(clicked()), SLOT(restoreUI()));
  connect(Clear,  SIGNAL(clicked()), SLOT(clearPars()));
  connect(Ok,     SIGNAL(clicked()), SLOT(accept()));
  connect(Run,    SIGNAL(clicked()), SLOT(preProcess()));
  connect(Fix,    SIGNAL(clicked()), SLOT(fixSession()));
  //--

  connect(this, SIGNAL(run(bool)),			MainWin,SLOT(setRunning(bool)));
  connect(this, SIGNAL(stBarReset()),			MainWin,SLOT(stBarReset()));
  connect(this, SIGNAL(stBarSetTotalSteps(int)),	MainWin,SLOT(stBarSetTotalSteps(int)));
  connect(this, SIGNAL(stBarSetProgress(int)),		MainWin,SLOT(stBarSetProgress(int)));
  connect(this, SIGNAL(message(const QString&)),	MainWin,SLOT(setMessage(const QString&)));

  Layout->activate();

  accel = new QAccel(this);
  accel->connectItem(accel->insertItem(Key_D+ALT), this, SLOT(doubleProc()));
  accel->connectItem(accel->insertItem(Key_X+ALT), this, SLOT(procScenario_1()));
  accel->connectItem(accel->insertItem(Key_L+ALT), this, SLOT(procScenario_2()));
  accel->connectItem(accel->insertItem(Key_3+ALT), this, SLOT(procScenario_3()));
  accel->connectItem(accel->insertItem(Key_T+ALT), this, SLOT(procScenario_4()));

  QAccel *accelNonBlocked = new QAccel(this);
  accelNonBlocked->connectItem(accelNonBlocked->insertItem(Key_G+ALT), this, SLOT(stopLongRun()));
};

void SBVLBIPreProcess::blockInput()
{
  QApplication::setOverrideCursor(Qt::waitCursor);

  if (accel)
    accel->setEnabled(FALSE);
  if (brwsSes)
    {
      IsBlockedSessionList = brwsSes->lvSessions()->signalsBlocked();
      brwsSes->lvSessions()->blockSignals(TRUE);
    };
  if (brwsSta)
    {
      IsBlockedStationList = brwsSta->lvStations()->signalsBlocked();
      brwsSta->lvStations()->blockSignals(TRUE);
    };
  if (brwsHist)
    {
      IsBlockedHistoryList = brwsHist->lvHistory()->signalsBlocked();
      brwsHist->lvHistory()->blockSignals(TRUE);
    };
  if (Clear)
    {
      IsBlockedClearButton = Clear->signalsBlocked();
      Clear->blockSignals(TRUE);
    };
  if (Run)
    {
      IsBlockedRunButton = Run->signalsBlocked();
      Run->blockSignals(TRUE);
    };
  if (Fix)
    {
      IsBlockedFixButton = Fix->signalsBlocked();
      Fix->blockSignals(TRUE);
    };
  if (Ok)
    {
      IsBlockedOkButton  = Ok->signalsBlocked();
      Ok->blockSignals(TRUE);
    };
  if (DumpUI)
    {
      IsBlockedDumpUIButton  = DumpUI->signalsBlocked();
      DumpUI->blockSignals(TRUE);
    };
  if (RestoreUI)
    {
      IsBlockedRestoreUIButton  = RestoreUI->signalsBlocked();
      RestoreUI->blockSignals(TRUE);
    };
};

void SBVLBIPreProcess::restoreInput()
{
  qApp->processEvents();
  if (accel)
    accel->setEnabled(TRUE);
  if (brwsSes)
    brwsSes->lvSessions()->blockSignals(IsBlockedSessionList);
  if (brwsSta)
    brwsSta->lvStations()->blockSignals(IsBlockedStationList);
  if (brwsHist)
    brwsHist->lvHistory()->blockSignals(IsBlockedHistoryList);
  if (Clear)
    Clear->blockSignals(IsBlockedClearButton);
  if (Run)
    Run->blockSignals(IsBlockedRunButton);
  if (Fix)
    Fix->blockSignals(IsBlockedFixButton);
  if (Ok)
    Ok->blockSignals(IsBlockedOkButton);
  if (DumpUI)
    DumpUI->blockSignals(IsBlockedDumpUIButton);
  if (RestoreUI)
    RestoreUI->blockSignals(IsBlockedRestoreUIButton);
  
  QApplication::restoreOverrideCursor();
};

void SBVLBIPreProcess::auxWindowsChanged(bool IsTmp)
{
  if (IsTmp)
    NumAuxWindows++;
  else
    NumAuxWindows--;
  if (NumAuxWindows>0 && brwsSes)
    brwsSes->lvSessions()->setEnabled(FALSE);
  else if (NumAuxWindows==0 && brwsSes)
    brwsSes->lvSessions()->setEnabled(TRUE);
};

QWidget* SBVLBIPreProcess::VLBIList(QWidget* parent, BrowseMode Mode_, SBProject* Prj)
{
  brwsSes = new SBBrowseSessions(Wvlbi, Mode_, Prj, parent);
  brwsSes->layout()->setMargin(0);

  if (brwsSes->lvSessions()->firstChild())
    brwsSes->lvSessions()->setSelected(brwsSes->lvSessions()->firstChild(), FALSE);
  
  connect(brwsSes->lvSessions(), 
	  //SIGNAL(currentChanged(QListViewItem*)), SLOT(currentSesChange(QListViewItem*)));
	  SIGNAL(selectionChanged(QListViewItem*)), SLOT(currentSesChange(QListViewItem*)));
  
  //  connect(this, SIGNAL(run(bool)),	brwsSes->lvSessions(), SLOT(setDisabled(bool)));
  
  if (brwsSes->lvSessions()->firstChild())
    brwsSes->lvSessions()->setSelected(brwsSes->lvSessions()->firstChild(), TRUE);
  //brwsSes->lvSessions()->setCurrentItem(0);
  
  return brwsSes;
};

QWidget* SBVLBIPreProcess::sesTab(QWidget* parent_)
{
  SesTab = new QTabWidget(parent_);
  SesTab ->addTab(wObservs(), "&Observations");
  SesTab ->addTab(wDataPlotsByBases(),    "Plots (&Bases)");
  SesTab ->addTab(wDataPlotsByStations(), "Plots (&Stations)");
  SesTab ->addTab(wDataPlotsBySources(),  "Plots (Sou&rces)");
  SesTab ->addTab(brwsSta = 
		  new SBBrowseStations(Session->stationList(), BM_GENERAL, Session->tStart()), "Stations");
  SesTab ->addTab(brwsBas = new SBBrowseBases(Session->baseList()), "Bases");
  SesTab ->addTab(brwsSou = new SBBrowseSources(Session->sourceList(), BM_GENERAL, NULL, this), "Sources");
  SesTab ->addTab(wAttributes(), "Attributes");
  SesTab ->addTab(brwsHist = new SBBrowseHistory(Session->dbhHistory(), Session->localHistory(), 
						 Session->localVersion()), "History");

  connect (brwsSta, SIGNAL(widgetIsBusy(bool)), SLOT(auxWindowsChanged(bool)));
  return SesTab;
};

QWidget* SBVLBIPreProcess::wObservs()
{
  QWidget	*W	= new QWidget(this);
  QBoxLayout	*Layout = new QVBoxLayout(W, 10, 10);
  
  //  ObservLV = new QListView(W);
  ObservLV = new My_QListView(W);
  ObservLV -> addColumn("Idx");
  ObservLV -> setColumnAlignment( 0, AlignRight);
  ObservLV -> addColumn("Time");
  ObservLV -> setColumnAlignment( 1, AlignRight);
  ObservLV -> addColumn("Station#1");
  ObservLV -> setColumnAlignment( 2, AlignLeft);
  ObservLV -> addColumn("Station#2");
  ObservLV -> setColumnAlignment( 3, AlignLeft);
  ObservLV -> addColumn("Source");
  ObservLV -> setColumnAlignment( 4, AlignLeft);
  ObservLV -> addColumn("Delay err");
  ObservLV -> setColumnAlignment( 5, AlignRight);
  ObservLV -> addColumn("Delay (O-C)");
  ObservLV -> setColumnAlignment( 6, AlignRight);
  ObservLV -> addColumn("Ambig");
  ObservLV -> setColumnAlignment( 7, AlignRight);
  ObservLV -> addColumn("QC");
  ObservLV -> setColumnAlignment( 8, AlignCenter);
  ObservLV -> addColumn("UFs");
  ObservLV -> setColumnAlignment( 9, AlignCenter);
  ObservLV -> addColumn("IonQ");
  ObservLV -> setColumnAlignment(10, AlignRight);
  ObservLV -> addColumn("?_1");
  ObservLV -> setColumnAlignment(11, AlignRight);
  ObservLV -> addColumn("?_2");
  ObservLV -> setColumnAlignment(12, AlignRight);
  ObservLV -> addColumn("Br_1");
  ObservLV -> setColumnAlignment(13, AlignCenter);
  ObservLV -> addColumn("Br_2");
  ObservLV -> setColumnAlignment(14, AlignCenter);
  ObservLV -> addColumn(" X ");
  ObservLV -> setColumnAlignment(15, AlignCenter);

  fillObsListView();

  ObservLV -> setAllColumnsShowFocus(TRUE);
  ObservLV -> setMinimumSize(ObservLV->sizeHint());
  ObservLV -> setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  if (ObservLV->firstChild())
    ObservLV -> setSelected(ObservLV->firstChild(), TRUE);
  ObservLV -> setFocusPolicy(QWidget::StrongFocus);
  Layout   -> addWidget(ObservLV,5);
  
  connect(ObservLV,
	  SIGNAL(mouseButtonClicked(int, QListViewItem*, const QPoint&, int)), 
	  SLOT(toggleEntryMarkEnable(int, QListViewItem*, const QPoint&, int)));

  connect(ObservLV,
	  SIGNAL(moveUponItem(QListViewItem*, int, Qt::ButtonState)), 
	  SLOT  (toggleEntryMoveEnable(QListViewItem*, int, Qt::ButtonState)));
  
  Layout -> activate();
  return W;
};

void SBVLBIPreProcess::toggleEntryMarkEnable(int button, QListViewItem* Item, const QPoint&, int n)
{
  if (IsRunning)
    return;
  
  if (Item) // may be NULL
    {
      SBObsVLBIEntry	*Obs = ((SBVLBIObsLI*)Item)->obs();
      bool		IsNeedUpdate = TRUE;
      switch (n)
	{
/*
	  case 7:
	    if (button==Qt::RightButton)
	      Obs->incAmbigFactor(AmbigJump);
	    else if (button==Qt::LeftButton)
	      Obs->decAmbigFactor(AmbigJump);
	    break;
*/
	  case 13:
	    Obs->xorAttr(SBObsVLBIEntry::breakClock1);
	    break;
	  case 14:
	    Obs->xorAttr(SBObsVLBIEntry::breakClock2);
	    break;
/*
	  case 15:
	    Obs->xorAttr(SBObsVLBIEntry::notValid);
	    break;
*/
	default:
	  IsNeedUpdate = FALSE;
	  break;
	};

      if (IsNeedUpdate)
	{
	  Item->repaint();
	  isModified = TRUE;
	};
    };
};

void SBVLBIPreProcess::toggleEntryMoveEnable(QListViewItem* Item, int n, Qt::ButtonState MouseButtonState)
{
  if (IsRunning)
    return;
  
  if (Item) // may be NULL
    {
      SBObsVLBIEntry	*Obs = ((SBVLBIObsLI*)Item)->obs();
      bool		IsNeedUpdate = TRUE;
      switch (n)
	{
	case 7:
	    if (MouseButtonState==Qt::RightButton)
	      Obs->incAmbigFactor(AmbigJump);
	    else if (MouseButtonState==Qt::LeftButton)
	      Obs->decAmbigFactor(AmbigJump);
	    break;
/*
	case 13:
	  Obs->xorAttr(SBObsVLBIEntry::breakClock1);
	  break;
	case 14:
	  Obs->xorAttr(SBObsVLBIEntry::breakClock2);
	  break;
*/
	case 15:
	  Obs->xorAttr(SBObsVLBIEntry::notValid);
	  break;
	default:
	  IsNeedUpdate = FALSE;
	  break;
	};

      if (IsNeedUpdate)
	{
	  Item->repaint();
	  isModified = TRUE;
	};
    };
};

QWidget* SBVLBIPreProcess::wAttributes()
{
  QWidget	*W = new QWidget(this);
  QBoxLayout	*Layout = new QHBoxLayout(W, 20, 10);
  QLabel	*label;
  QGridLayout	*grid;
  QGroupBox	*gbox;
  QBoxLayout	*subLayout;
  QString	Str;
  int		i;

  subLayout = new QVBoxLayout;
  Layout -> addLayout(subLayout);

  // attributes:
  gbox = new QGroupBox("Session's attributes", W);
  grid = new QGridLayout(gbox, 9,2, 20, 10);
  
  label	= new QLabel("DBH File Name:", gbox);
  label	-> setMinimumSize(label->sizeHint());
  grid -> addWidget(label, 0,0);
  lSesName= new QLabel(Session->name(), gbox);
  lSesName-> setMinimumSize(lSesName->sizeHint());
  grid -> addWidget(lSesName, 0,1);

  label	= new QLabel("Official Name/Code:", gbox);
  label	-> setMinimumSize(label->sizeHint());
  grid -> addWidget(label, 1,0);
  lOfficialName	= new QLabel(Session->officialName() + "/" + Session->code(), gbox);
  lOfficialName-> setMinimumSize(lOfficialName->sizeHint());
  grid -> addWidget(lOfficialName, 1,1);

  label	= new QLabel("Correlator Name:", gbox);
  label	-> setMinimumSize(label->sizeHint());
  grid -> addWidget(label, 2,0);
  lCorrName = new QLabel(Session->correlatorName(), gbox);
  lCorrName-> setMinimumSize(lCorrName->sizeHint());
  grid -> addWidget(lCorrName, 2,1);

  label	= new QLabel("Network ID:", gbox);
  label	-> setMinimumSize(label->sizeHint());
  grid -> addWidget(label, 3,0);

  NetID = new QComboBox(FALSE, gbox);
  NetID -> setInsertionPolicy(QComboBox::AtBottom);
  i = 1;
  NetID->insertItem("UNDEF");
  if (Wvlbi->networks()->count())
    for (SBVLBISet::Network *n=Wvlbi->networks()->first(); n; n=Wvlbi->networks()->next(), i++)
      {
	NetID->insertItem(n->name());
	if (n->name()==Session->networkID()) NetID->setCurrentItem(i);
      }
  else 
    NetID->insertItem("N/A");
  NetID	-> setMinimumSize(NetID->sizeHint());
  grid -> addWidget(NetID, 3,1);

  label	= new QLabel("Epoch of the first observation:", gbox);
  label	-> setMinimumSize(label->sizeHint());
  grid -> addWidget(label, 4,0);
  lTStart = new QLabel(Session->tStart().toString(SBMJD::F_Short), gbox);
  lTStart -> setMinimumSize(lTStart->sizeHint());
  grid -> addWidget(lTStart, 4,1);

  label	= new QLabel("Epoch of the last observation:", gbox);
  label	-> setMinimumSize(label->sizeHint());
  grid -> addWidget(label, 5,0);
  lTFinis = new QLabel(Session->tFinis().toString(SBMJD::F_Short), gbox);
  lTFinis -> setMinimumSize(lTFinis->sizeHint());
  grid -> addWidget(lTFinis, 5,1);

  label	= new QLabel("Mean epoch of observations:", gbox);
  label	-> setMinimumSize(label->sizeHint());
  grid -> addWidget(label, 6,0);
  lTMean = new QLabel(Session->tMean().toString(SBMJD::F_Short), gbox);
  lTMean -> setMinimumSize(lTMean->sizeHint());
  grid -> addWidget(lTMean, 6,1);

  label	= new QLabel("Version created on:", gbox);
  label	-> setMinimumSize(label->sizeHint());
  grid -> addWidget(label, 7,0);
  lTofVer = new QLabel(Session->dateCreat().toString(SBMJD::F_Short), gbox);
  lTofVer-> setMinimumSize(lTofVer->sizeHint());
  grid -> addWidget(lTofVer, 7,1);

  label	= new QLabel("Experiment description:", gbox);
  label	-> setMinimumSize(label->sizeHint());
  grid -> addWidget(label, 8,0);
  Str = Session->expDescr().stripWhiteSpace();
  lExpDescr = new QLabel(Str!=""?Str:QString(" "), gbox);
  lExpDescr-> setMinimumSize(lExpDescr->sizeHint());
  grid -> addWidget(lExpDescr, 8,1);

  grid -> activate();
  subLayout -> addWidget(gbox);


  // parameters:
  gbox = new QGroupBox("Session's parameters", W);
  grid = new QGridLayout(gbox, 3,3, 20, 10);

  label	= new QLabel("User Flag:", gbox);
  label	-> setMinimumSize(label->sizeHint());
  grid -> addWidget(label, 0,0);

  UFlag = new QComboBox(FALSE, gbox);
  UFlag -> setInsertionPolicy(QComboBox::AtBottom);

  for (i=0; i< (int)(sizeof(sUFlags)/sizeof(const char*)); i++)
    {
      UFlag->insertItem(sUFlags[i]);
      if (sUFlags[i] == Session->userFlag())
	UFlag->setCurrentItem(i);
    };
  
  UFlag	-> setMinimumSize(UFlag->sizeHint());
  grid -> addWidget(UFlag, 0,1);

  GrAmbig = new QLineEdit(gbox);
  label	= new QLabel(GrAmbig, "&Group Ambiguity spacing:", gbox);
  label	-> setMinimumSize(label->sizeHint());
  GrAmbig -> setText(Str.sprintf("%7.3f", 1.0e9*Session->grDelayAmbig()));
  GrAmbig -> setMinimumHeight(GrAmbig->sizeHint().height());
  GrAmbig -> setMinimumWidth(GrAmbig->fontMetrics().width(Str)+10);
  grid -> addWidget(label, 1,0);
  grid -> addWidget(GrAmbig, 1,1);

  label	= new QLabel("(nsec)", gbox);
  label	-> setMinimumSize(label->sizeHint());
  grid -> addWidget(label, 1,2);

  RefFreq = new QLineEdit(gbox);
  label	= new QLabel(RefFreq, "&Reference Frequency:", gbox);
  label	-> setMinimumSize(label->sizeHint());
  RefFreq -> setText(Str.sprintf("%8.3f", Session->refFreq()));
  RefFreq -> setMinimumHeight(RefFreq->sizeHint().height());
  RefFreq -> setMinimumWidth(RefFreq->fontMetrics().width(Str)+10);
  grid -> addWidget(label, 2,0);
  grid -> addWidget(RefFreq, 2,1);

  label	= new QLabel("(MHz)", gbox);
  label	-> setMinimumSize(label->sizeHint());
  grid -> addWidget(label, 2,2);
  grid -> activate();

  subLayout -> addWidget(gbox);
  subLayout -> addStretch(1);

  Layout -> addStretch(5);
  Layout -> activate();
  return W;
};

QWidget* SBVLBIPreProcess::wDataPlotsByBases()
{
  QWidget *W		= new QWidget(this);
  PlotBases		= new SBPlot(PC_Bases, W);
  QBoxLayout	*Layout = new QVBoxLayout(W, 10, 10);

  Layout->addWidget(PlotBases, 5);
  Layout->activate();

  return W;
};

QWidget* SBVLBIPreProcess::wDataPlotsByStations()
{
  QWidget	*W	= new QWidget(this);
  PlotStations		= new SBPlot(PC_Stations, W);
  QBoxLayout	*Layout = new QVBoxLayout(W, 10, 10);

  Layout->addWidget(PlotStations, 5);
  Layout->activate();

  return W;
};

QWidget* SBVLBIPreProcess::wDataPlotsBySources()
{
  QWidget	*W	= new QWidget(this);
  PlotSources		= new SBPlot(PC_Sources, W);
  QBoxLayout	*Layout = new QVBoxLayout(W, 10, 10);

  Layout->addWidget(PlotSources, 5);
  Layout->activate();

  return W;
};

QWidget* SBVLBIPreProcess::wEstPars(QWidget* parent_)
{
  QLabel	*label;
  QWidget	*W	= new QGroupBox("Options of Estimator", parent_);
  QBoxLayout	*Layout = new QHBoxLayout(W, 20, 10);
  QBoxLayout	*SubLayout;
  
  QGridLayout *grid = new QGridLayout(3,6,  5);
  Layout->addLayout(grid);
  
  
  // names:
  //  label= new QLabel("Parameter", W);
  //  label->setMinimumSize(label->sizeHint());
  //  grid ->addWidget(label, 0,0);

  label= new QLabel("N", W);
  label->setMinimumSize(label->sizeHint());
  grid ->addWidget(label, 0,1);

  label= new QLabel("L", W);
  label->setMinimumSize(label->sizeHint());
  grid ->addWidget(label, 0,2);

  label= new QLabel("S", W);
  label->setMinimumSize(label->sizeHint());
  grid ->addWidget(label, 0,3);

  label= new QLabel("PSD", W);
  label->setMinimumSize(label->sizeHint());
  grid ->addWidget(label, 0,4);

  label= new QLabel("Ambig.Jmp", W);
  label->setMinimumSize(label->sizeHint());
  grid ->addWidget(label, 0,5);


  label= new QLabel("Clocks: ", W);
  label->setMinimumSize(label->sizeHint());
  grid ->addWidget(label, 1,0);

  label= new QLabel("Zenith: ", W);
  label->setMinimumSize(label->sizeHint());
  grid ->addWidget(label, 2,0);


  // radio buttons:
  QButtonGroup* bg = new QButtonGroup(W);
  SubLayout = new QHBoxLayout(bg, 0, 5);
  for (int i=0; i<3; i++)
    {
      RBClocks[i] = new QRadioButton(bg);
      RBClocks[i]-> setMinimumSize(RBClocks[i]->sizeHint());
      SubLayout->addWidget(RBClocks[i]);
    };
  RBClocks[1]->setChecked(TRUE);
  SubLayout->activate();
  bg->setFrameStyle(QFrame::NoFrame);
  grid->addMultiCellWidget(bg, 1,1, 1,3);

  bg = new QButtonGroup(W);
  SubLayout = new QHBoxLayout(bg, 0, 5);
  for (int i=0; i<3; i++)
    {
      RBZenith[i] = new QRadioButton(bg);
      RBZenith[i]-> setMinimumSize(RBZenith[i]->sizeHint());
      SubLayout->addWidget(RBZenith[i]);
    };
  RBZenith[0]->setChecked(TRUE);
  SubLayout->activate();
  bg->setFrameStyle(QFrame::NoFrame);
  grid->addMultiCellWidget(bg, 2,2, 1,3);
  //
  
  // PSD editors:
  LEClkPSD = new QLineEdit(W);
  LEClkPSD->setText(QString().sprintf("%.2f", Config.p().clock0().whiteNoise()));
  LEClkPSD->setMinimumSize(LEClkPSD->sizeHint());
  LEClkPSD->setEnabled(FALSE);
  grid->addWidget(LEClkPSD, 1,4);
  
  LEZenPSD = new QLineEdit(W);
  LEZenPSD->setText(QString().sprintf("%.2f", Config.p().zenith().whiteNoise()));
  LEZenPSD->setMinimumSize(LEZenPSD->sizeHint());
  LEZenPSD->setEnabled(FALSE);
  grid->addWidget(LEZenPSD, 2,4);

  connect(RBClocks[2], SIGNAL(toggled(bool)), LEClkPSD, SLOT(setEnabled(bool)));
  connect(RBZenith[2], SIGNAL(toggled(bool)), LEZenPSD, SLOT(setEnabled(bool)));

  sbAmbigJump = new QSpinBox(W);
  sbAmbigJump->setMinValue(1);
  sbAmbigJump->setMaxValue(1000000000);
  sbAmbigJump->setValue   (1000000000);
  sbAmbigJump->setMinimumSize(sbAmbigJump->sizeHint());
  sbAmbigJump->setValue(1);
  grid ->addWidget(sbAmbigJump, 1, 5);
  grid ->setColStretch(5, 2);

  connect(sbAmbigJump, SIGNAL(valueChanged(int)), SLOT(ambigJumpChanged(int)));
  
  SubLayout = new QVBoxLayout();
  Layout->addLayout(SubLayout, 5);
  
  Layout->activate();
  
  return W;
};

void SBVLBIPreProcess::ambigJumpChanged(int J)
{
  AmbigJump = J;
};

void SBVLBIPreProcess::fillSessAttr()
{
  QString	Str;
  int		i;

  lSesName->setText(Session->name());
  lSesName->setMinimumSize(lSesName->sizeHint());

  lOfficialName->setText(Session->officialName() + "/" + Session->code());
  lOfficialName->setMinimumSize(lOfficialName->sizeHint());

  lCorrName->setText(Session->correlatorName());
  lCorrName->setMinimumSize(lCorrName->sizeHint());

  i = 1;
  if (Wvlbi->networks()->count())
    for (SBVLBISet::Network *n=Wvlbi->networks()->first(); n; n=Wvlbi->networks()->next(), i++)
      if (n->name()==Session->networkID()) 
	NetID->setCurrentItem(i);
  
  lTStart->setText(Session->tStart().toString(SBMJD::F_Short));
  lTStart->setMinimumSize(lTStart->sizeHint());

  lTFinis->setText(Session->tFinis().toString(SBMJD::F_Short));
  lTFinis->setMinimumSize(lTFinis->sizeHint());

  lTMean->setText(Session->tMean().toString(SBMJD::F_Short));
  lTMean->setMinimumSize(lTMean->sizeHint());

  lTofVer->setText(Session->dateCreat().toString(SBMJD::F_Short));
  lTofVer->setMinimumSize(lTofVer->sizeHint());


  Str = Session->expDescr().stripWhiteSpace();
  lExpDescr->setText(Str!=""?Str:QString(" "));
  lExpDescr->setMinimumSize(lExpDescr->sizeHint());
  
  for (i=0; i< (int)(sizeof(sUFlags)/sizeof(const char*)); i++)
    {
      if (sUFlags[i] == Session->userFlag())
	UFlag->setCurrentItem(i);
    };
  
  GrAmbig -> setText(Str.sprintf("%7.3f", 1.0e9*Session->grDelayAmbig()));
  GrAmbig -> setMinimumWidth(GrAmbig->fontMetrics().width(Str)+10);

  RefFreq -> setText(Str.sprintf("%8.3f", Session->refFreq()));
  RefFreq -> setMinimumWidth(RefFreq->fontMetrics().width(Str)+10);
};

void SBVLBIPreProcess::fillObsListView()
{
  ObservLV->clear();
  if (Solution)
    Solution->prepare4Batch(Session->name(), FALSE);

  SBObsVLBIEntry	*E;
  int			i;
  double	scale	= SetUp->scale();
  if (Session)
    for (i=0, E=Session->first(); E; i++, E=Session->next())
      {
	(void) new SBVLBIObsPPLI(ObservLV, Session, E);
	
	SBBaseInfo	*bi = NULL;
	SBStationInfo	*St1=Session->stationInfo(E->station_1Idx());
	SBStationInfo	*St2=Session->stationInfo(E->station_2Idx());
	SBSourceInfo	*Src=Session->sourceInfo(E->sourceIdx());

	SBPlotBranch	*pb = NULL;

	if (St1 && St2 && Src)
	  {
	    // fill plot carrier for bases:
	    if ((bi=Session->baseInfo(St1->aka() + ":" + St2->aka())) && !bi->isAttr(SBBaseInfo::notValid))
	      if ((pb=PC_Bases->findBranch(bi->aka())))
		{
		  pb->data()->set(pb->Idx,  0,	    E->mjd());		//
		  pb->data()->set(pb->Idx,  1,  scale*E->delayRes());	//
		  pb->data()->set(pb->Idx,  2,  scale*E->delay());	//
		  pb->data()->set(pb->Idx,  3, 1.0e12*E->rateRes());	// sec->ps
		  pb->data()->set(pb->Idx,  4, 1.0e12*E->rate());		// sec->ps
		  pb->data()->set(pb->Idx,  5,  scale*E->delayIon());	//
		  pb->data()->set(pb->Idx,  6, 1.0e12*E->rateIon());	// sec->ps
		  pb->data()->set(pb->Idx,  7, E->fringePhase());	//
		  pb->data()->set(pb->Idx,  8, E->correlation());	//
		  // std variations:
		  pb->data()->set(pb->Idx,  9,  scale*E->delayErr());	//
		  pb->data()->set(pb->Idx, 10, 1.0e12*E->rateErr());	// sec->ps
		  pb->data()->set(pb->Idx, 11,  scale*E->delayIonErr());//
		  pb->data()->set(pb->Idx, 12, 1.0e12*E->rateIonErr());	// sec->ps
		  if (! E->isAttr(SBObsVLBIEntry::processed))
		    pb->setDataAttr(pb->Idx, SBPlotCarrier::DA_IGN);
		  if (E->isAttr(SBObsVLBIEntry::breakClock1))
		    pb->setDataAttr(pb->Idx, SBPlotCarrier::DA_BAR);
		  if (E->isAttr(SBObsVLBIEntry::breakClock2))
		    pb->setDataAttr(pb->Idx, SBPlotCarrier::DA_BAR);
		  pb->Idx++;
		}
	      else // pb == NULL
		Log->write(SBLog::ERR, SBLog::INTERFACE, ClassName() + 
			   ": cannot find plot branch for base [" + bi->aka() + "]");
	    else if (!bi) // baseInfo failed
	      Log->write(SBLog::ERR, SBLog::INTERFACE, ClassName() + 
			 ": cannot find base [" + St1->aka() + ":" + St2->aka() + "] for the session " + 
			 Session->name());

	    // fill plot carrier for stations:
	    if (!St1->isAttr(SBStationInfo::notValid))
	      {
		if ((pb=PC_Stations->findBranch(St1->aka())))
		  {
		    pb->data()->set(pb->Idx,  0,	      E->mjd());		//
		    pb->data()->set(pb->Idx,  1,  scale*E->cable_1());	//
		    pb->data()->set(pb->Idx,  2,        E->atmTemp_1());	//
		    pb->data()->set(pb->Idx,  3,        E->atmPress_1());	//
		    pb->data()->set(pb->Idx,  4,        E->atmHum_1());	//
		    pb->data()->set(pb->Idx,  5,  scale*E->zenithDelay_1());//
		    if (Solution)
		      {
			SBStochParameter	*SthPar = NULL;
			if ( (SthPar = Solution->stochastic().find("St: " + St1->toString() + ": Clock_0")) )
			  {
			    pb->data()->set(pb->Idx,  6,  SthPar->v(E->mjd())*1.0e12);	//
			    pb->data()->set(pb->Idx,  8,  SthPar->e(E->mjd())*1.0e12);	//
			  };
			if ( (SthPar = Solution->stochastic().find("St: " + St1->toString() + ": Zenith ")) )
			  {
			    pb->data()->set(pb->Idx,  7,  SthPar->v(E->mjd())*1.0e2);	//
			    pb->data()->set(pb->Idx,  9,  SthPar->e(E->mjd())*1.0e2);	//
			  };
		      };
		    if (! E->isAttr(SBObsVLBIEntry::processed))
		      pb->setDataAttr(pb->Idx, SBPlotCarrier::DA_IGN);
		    
		    // std variations:
		    pb->Idx++;
		  }
		else // pb == NULL
		  Log->write(SBLog::ERR, SBLog::INTERFACE, ClassName() + 
			     ": cannot find plot branch for the station [" + St1->aka() + "]");
	      }
	    if (!St2->isAttr(SBStationInfo::notValid))
	      {
		if ((pb=PC_Stations->findBranch(St2->aka())))
		  {
		    pb->data()->set(pb->Idx,  0,	      E->mjd());		//
		    pb->data()->set(pb->Idx,  1,  scale*E->cable_2());	//
		    pb->data()->set(pb->Idx,  2,        E->atmTemp_2());	//
		    pb->data()->set(pb->Idx,  3,        E->atmPress_2());	//
		    pb->data()->set(pb->Idx,  4,        E->atmHum_2());	//
		    pb->data()->set(pb->Idx,  5,  scale*E->zenithDelay_2());//
		    if (Solution)
		      {
			SBStochParameter	*SthPar = NULL;
			if ( (SthPar = Solution->stochastic().find("St: " + St2->toString() + ": Clock_0")) )
			  {
			    pb->data()->set(pb->Idx,  6,  SthPar->v(E->mjd())*1.0e12);	//
			    pb->data()->set(pb->Idx,  8,  SthPar->e(E->mjd())*1.0e12);	//
			  };
			if ( (SthPar = Solution->stochastic().find("St: " + St2->toString() + ": Zenith ")) )
			  {
			    pb->data()->set(pb->Idx,  7,  SthPar->v(E->mjd())*1.0e2);	//
			    pb->data()->set(pb->Idx,  9,  SthPar->e(E->mjd())*1.0e2);	//
			  };
		      };
		    if (! E->isAttr(SBObsVLBIEntry::processed))
		      pb->setDataAttr(pb->Idx, SBPlotCarrier::DA_IGN);
		    // std variations:
		    pb->Idx++;
		  }
		else // pb == NULL
		  Log->write(SBLog::ERR, SBLog::INTERFACE, ClassName() + 
			     ": cannot find plot branch for the station [" + St2->aka() + "]");
	      }

	    // fill plot carrier for sources:
	    if (!Src->isAttr(SBSourceInfo::notValid))
	      {
		if ((pb=PC_Sources->findBranch(Src->aka())))
		  {
		    pb->data()->set(pb->Idx,  0,	    E->mjd());	//
		    pb->data()->set(pb->Idx,  1,  scale*E->delayRes());	//
		    pb->data()->set(pb->Idx,  2,  scale*E->delay());	//
		    pb->data()->set(pb->Idx,  3, 1.0e12*E->rateRes());	// sec->ps
		    pb->data()->set(pb->Idx,  4, 1.0e12*E->rate());		// sec->ps
		    pb->data()->set(pb->Idx,  5,  scale*E->delayIon());	//
		    pb->data()->set(pb->Idx,  6, 1.0e12*E->rateIon());	// sec->ps
		    pb->data()->set(pb->Idx,  7, E->fringePhase());	//
		    pb->data()->set(pb->Idx,  8, E->correlation());	//
		    // std variations:
		    pb->data()->set(pb->Idx,  9,  scale*E->delayErr());	//
		    pb->data()->set(pb->Idx, 10, 1.0e12*E->rateErr());	// sec->ps
		    pb->data()->set(pb->Idx, 11,  scale*E->delayIonErr());//
		    pb->data()->set(pb->Idx, 12, 1.0e12*E->rateIonErr());	// sec->ps
		    if (! E->isAttr(SBObsVLBIEntry::processed))
		      pb->setDataAttr(pb->Idx, SBPlotCarrier::DA_IGN);
		    if (E->isAttr(SBObsVLBIEntry::breakClock1))
		      pb->setDataAttr(pb->Idx, SBPlotCarrier::DA_BAR);
		    if (E->isAttr(SBObsVLBIEntry::breakClock2))
		      pb->setDataAttr(pb->Idx, SBPlotCarrier::DA_BAR);
		    pb->Idx++;
		  }
		else // pb == NULL
		  Log->write(SBLog::ERR, SBLog::INTERFACE, ClassName() + 
			     ": cannot find plot branch for source [" + Src->aka() + "]");
	      };

	  }
	else
	  {
	    Log->write(SBLog::ERR, SBLog::INTERFACE, ClassName() + 
		       ": cannot find plot branch: St1/St2/Src is/are NULL");
	    if (!St1)
	      Log->write(SBLog::ERR, SBLog::INTERFACE, ClassName() + 
			 ": St1 == NULL");
	    if (!St2)
	      Log->write(SBLog::ERR, SBLog::INTERFACE, ClassName() + 
			 ": St2 == NULL");
	    if (!Src)
	      Log->write(SBLog::ERR, SBLog::INTERFACE, ClassName() + 
			 ": Src == NULL");
	    std::cerr << "Epoch: " << E->toString() << "\nSt1Idx= " << E->station_1Idx() 
		      << "\nSt2Idx= " << E->station_2Idx() << "\nSrcIdx= " << E->sourceIdx() << "\n";

	  };
      };
  
};

void SBVLBIPreProcess::updateSession()
{
  if (Session)
    {
      CurrentSesLI->repaint();
      
      // clear & prepare plot carriers:
      PC_Bases->setName("Plots for the session " + Session->name() + ", splitted by bases");
      PC_Bases->setFile4SaveBaseName(Session->name() + "_Bases");
      
      PC_Bases->branches()->clear();
      if (Session->baseList())
	for (SBBaseInfo* E=Session->baseList()->first(); E; E=Session->baseList()->next())
	  if (!E->isAttr(SBBaseInfo::notValid))
	    PC_Bases->createBranch(E->num(), E->aka()); 
      
      PC_Stations->setName("Plots for the session " + Session->name() + ", splitted by stations");
      PC_Stations->setFile4SaveBaseName(Session->name() + "_Stations");
      
      PC_Stations->branches()->clear();
      // split data by stations:
      if (Session->stationList())
	for (SBStationInfo* E=Session->stationList()->first(); E; E=Session->stationList()->next())
	  if (!E->isAttr(SBStationInfo::notValid))
	    {
	      PC_Stations->createBranch(E->num(), E->aka());
	    };

      PC_Sources->setName("Plots for the session " + Session->name() + ", splitted by sources");
      PC_Sources->setFile4SaveBaseName(Session->name() + "_Sources");
      
      PC_Sources->branches()->clear();
      // split data by sources:
      if (Session->sourceList())
	for (SBSourceInfo* E=Session->sourceList()->first(); E; E=Session->sourceList()->next())
	  if (!E->isAttr(SBSourceInfo::notValid))
	    {
	      PC_Sources->createBranch(E->num(), E->aka());
	    };

      // first tab:
      fillObsListView();
      
      // second tab:
      PlotBases   ->dataChanged();
      
      // third tab:
      PlotStations->dataChanged();

      // fourth tab:
      PlotSources->dataChanged();
      
      // fifth tab:
      brwsSta->setStaInfoList(Session->stationList());
      brwsSta->setT0(Session->tStart());
      
      // sixth tab:
      brwsBas->setBasInfoList(Session->baseList());

      // seventh tab:
      brwsSou->setSouInfoList(Session->sourceList());
            
      // eighth tab:
      fillSessAttr();
      
      // nineth tab:
      brwsHist->setHistories(Session->dbhHistory(), Session->localHistory(), Session->localVersion());

      
    };
};

void SBVLBIPreProcess::currentSesChange(QListViewItem* SesItem_)
{
  if (IsRunning)
    return;

  if (!SesItem_)
    return;
  if (((SBVLBISesPreProcLI*)SesItem_)->sessionInfo() == NULL)
    return;

  blockInput();

  if (Solution)
    {
      Solution->deleteSolution(); // clear a disk
      delete Solution;
      Solution = NULL;
    };
  if (Prj)
    {
      delete Prj;
      Prj = NULL;
    };

  CurrentSesLI = (SBVLBISesPreProcLI*)SesItem_;
  SesInfo = CurrentSesLI->sessionInfo();
  //  std::cout<< SesInfo->name() << "\n";
  
  if (Session)
    {
      delete Session;
      Session = NULL;
    };
  
  if (!(Session=Wvlbi->loadSession(SesInfo->name()))) // just to explain before getting SIGSEGV ;)
    Log->write(SBLog::ERR, SBLog::PREPROC, ClassName() + ": got an empty session"); 

  
  Prj = new SBProject("PreProcessing");
  Prj->addSession(Wvlbi, SesInfo);
  
  Solution = new SBSolution(Prj);
  
  if (ObservLV)
    updateSession();
  else // it is the first session, just create branches for plot carriers:
    {
      PC_Bases->setName("Plots for the session " + Session->name() + ", splitted by bases");
      PC_Bases->setFile4SaveBaseName(Session->name() + "_Bases");
      
      PC_Bases->branches()->clear();
      if (Session->baseList())
	for (SBBaseInfo* E=Session->baseList()->first(); E; E=Session->baseList()->next())
	  if (!E->isAttr(SBBaseInfo::notValid))
	    PC_Bases->createBranch(E->num(), E->aka()); 
      
      PC_Stations->setName("Plots for the session " + Session->name() + ", splitted by stations");
      PC_Stations->setFile4SaveBaseName(Session->name() + "_Stations");
      
      PC_Stations->branches()->clear();
      // split data by stations:
      if (Session->stationList())
	for (SBStationInfo* E=Session->stationList()->first(); E; E=Session->stationList()->next())
	  if (!E->isAttr(SBStationInfo::notValid))
	    PC_Stations->createBranch(E->num(), E->aka());

      PC_Sources->setName("Plots for the session " + Session->name() + ", splitted by sources");
      PC_Sources->setFile4SaveBaseName(Session->name() + "_Sources");
      
      PC_Sources->branches()->clear();
      // split data by sources:
      if (Session->sourceList())
	for (SBSourceInfo* E=Session->sourceList()->first(); E; E=Session->sourceList()->next())
	  if (!E->isAttr(SBSourceInfo::notValid))
	    PC_Sources->createBranch(E->num(), E->aka());

    };
  
  restoreInput();
};

void SBVLBIPreProcess::fixSession()
{
  if (IsRunning)
    return;
  
  blockInput();
  Session->addAttr(SBVLBISesInfo::preProc);
  Session->addHistoryLine("Session has been preprocessed.");
  
  saveSession();
  SetUp->saveVLBI(Wvlbi);
  restoreInput();
};

void SBVLBIPreProcess::preProcess()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": preProcess selected");
  
  if (IsRunning)
    return;
  
  emit run( (IsRunning=TRUE) );
  blockInput();
  Wvlbi->saveSession(Session);
  
  SBParametersDesc ParDescr;
  // preparing the project:
  if (!Prj)
    {
      Prj = new SBProject("PreProcessing");
      Prj->addSession(Wvlbi, SesInfo);
    };
  
  if (!Solution)
    Solution = new SBSolution(Prj);

  // update the PSD values (if necessary):
  double		d;
  bool			IsOK;
  SBParameterCfg	P_cfg;
  SBParametersDesc	P_desc = Config.p();
  
  P_cfg = Config.p().clock0();
  d = LEClkPSD->text().toDouble(&IsOK);
  if (IsOK && d!=P_cfg.whiteNoise())
    {
      P_cfg.setWhiteNoise(d);
      P_desc.setClock0(P_cfg);
      Config.setP(P_desc);
      Config.setIsChanged();
    };
  P_cfg = Config.p().zenith();
  d = LEZenPSD->text().toDouble(&IsOK);
  if (IsOK && d!=P_cfg.whiteNoise())
    {
      P_cfg.setWhiteNoise(d);
      P_desc.setZenith(P_cfg);
      Config.setP(P_desc);
      Config.setIsChanged();
    };
  //
  
  Prj->setCfg(Config); // general config, could be changed between two calls, so set up it..
  
  // clear and set up parameters
  Prj->setAllPars2Type(SBParameterCfg::PT_NONE);
  ParDescr = Prj->p();
  
  if (RBClocks[1]->isChecked())
    {
      ParDescr.setClock0Type(SBParameterCfg::PT_LOC);
      ParDescr.setClock1Type(SBParameterCfg::PT_LOC);
      ParDescr.setClock2Type(SBParameterCfg::PT_LOC);
    }
  else if (RBClocks[2]->isChecked())
    ParDescr.setClock0Type(SBParameterCfg::PT_STH);      
  
  if (RBZenith[1]->isChecked())
    ParDescr.setZenithType(SBParameterCfg::PT_LOC);
  else if (RBZenith[2]->isChecked())
    ParDescr.setZenithType(SBParameterCfg::PT_STH);
  
  if (IsNeedEstimateCables)
    {
      P_cfg = ParDescr.cable();
      P_cfg .setConvAPriori(1.0e6);
      ParDescr.setCable(P_cfg);
      ParDescr.setCableType(SBParameterCfg::PT_LOC);
    }
  else
    ParDescr.setCableType(SBParameterCfg::PT_NONE);
    
  Prj->setP(ParDescr);
  

  // running the project:
  SBRunManager *Runner=new SBRunManager(Prj, Wvlbi, Solution);
  Runner->process();
  delete Runner;
  
  
  // saving the results and deleting the project:
  if (Session)
    {
      delete Session;
      Session = NULL;
    };
  
  if (!(Session=Wvlbi->loadSession(SesInfo->name()))) // just to explain before geting SIGSEGV ;)
    Log->write(SBLog::ERR, SBLog::PREPROC, ClassName() + ": got an empty session"); 
  
  //  CurrentSesLI->repaint();
  
  SBParameter		*P;
  SBStationInfo		*SI = NULL;
  SBStationID		id;
  QString		Str;
  for (P=Solution->local().first(); P; P=Solution->local().next())
    {
      // stations:
      if (P->name().contains("St: "))
	{
	  // split for branches:
	  Str = P->name().mid(4, 9);
	  if (id.isValidStr(Str))
	    id.setID(Str);
	  else
	    Log->write(SBLog::ERR, SBLog::PREPROC, ClassName() + ": cannot convert parameter name [" +
		       P->name() + "] to Station ID");
	  if (id.isValidId())
	    {
	      if ((SI=Session->stationList()->find(&id)))
		{
		  if (P->name().contains("Clock_0"))
		    {
		      SI->setCl_V(0,  SI->cl_V(0) + P->v()*1.0e9);
		      SI->setCl_E(0,  P->e()*1.0e9);
		      P->setV(0.0);
		      P->setS(0.0);
		      Solution->setIsLocalModified(TRUE);
		    }
		  else if (P->name().contains("Clock_1"))
		    {
		      SI->setCl_V(1,  SI->cl_V(1) + P->v()*1.0e9);
		      SI->setCl_E(1,  P->e()*1.0e9);
		      P->setV(0.0);
		      P->setS(0.0);
		      Solution->setIsLocalModified(TRUE);
		    }
		  else if (P->name().contains("Clock_2"))
		    {
		      SI->setCl_V(2,  SI->cl_V(2) + P->v()*1.0e9);
		      SI->setCl_E(2,  P->e()*1.0e9);
		      P->setV(0.0);
		      P->setS(0.0);
		      Solution->setIsLocalModified(TRUE);
		    }
		  else if (P->name().contains("Clock_3"))
		    {
		      SI->setCl_V(3,  SI->cl_V(3) + P->v()*1.0e9);
		      SI->setCl_E(3,  P->e()*1.0e9);
		      P->setV(0.0);
		      P->setS(0.0);
		      Solution->setIsLocalModified(TRUE);
		    }
		  else if (P->name().contains("Clock_4"))
		    {
		      SI->setCl_V(4,  SI->cl_V(4) + P->v()*1.0e9);
		      SI->setCl_E(4,  P->e()*1.0e9);
		      P->setV(0.0);
		      P->setS(0.0);
		      Solution->setIsLocalModified(TRUE);
		    }
		  else if (P->name().contains("Clock_5"))
		    {
		      SI->setCl_V(5,  SI->cl_V(5) + P->v()*1.0e9);
		      SI->setCl_E(5,  P->e()*1.0e9);
		      P->setV(0.0);
		      P->setS(0.0);
		      Solution->setIsLocalModified(TRUE);
		    }
		  else if (P->name().contains("Clock_6"))
		    {
		      SI->setCl_V(6,  SI->cl_V(6) + P->v()*1.0e9);
		      SI->setCl_E(6,  P->e()*1.0e9);
		      P->setV(0.0);
		      P->setS(0.0);
		      Solution->setIsLocalModified(TRUE);
		    }
		  else if (P->name().contains("Clock_7"))
		    {
		      SI->setCl_V(7,  SI->cl_V(7) + P->v()*1.0e9);
		      SI->setCl_E(7,  P->e()*1.0e9);
		      P->setV(0.0);
		      P->setS(0.0);
		      Solution->setIsLocalModified(TRUE);
		    }
		  else if (P->name().contains("Clock_8"))
		    {
		      SI->setCl_V(8,  SI->cl_V(8) + P->v()*1.0e9);
		      SI->setCl_E(8,  P->e()*1.0e9);
		      P->setV(0.0);
		      P->setS(0.0);
		      Solution->setIsLocalModified(TRUE);
		    }
		  else if (P->name().contains("Clock_9"))
		    {
		      SI->setCl_V(9,  SI->cl_V(9) + P->v()*1.0e9);
		      SI->setCl_E(9,  P->e()*1.0e9);
		      P->setV(0.0);
		      P->setS(0.0);
		      Solution->setIsLocalModified(TRUE);
		    }
		  else if (P->name().contains("Zenith"))
		    {
		      SI->setZw   (SI->zw() + P->v());
		      SI->setErrZw(P->e());
		      P->setV(0.0);
		      P->setS(0.0);
		      Solution->setIsLocalModified(TRUE);
		    };
		}
	      else
		Log->write(SBLog::ERR, SBLog::PREPROC, ClassName() + 
			   ": cannot find station in the list for ID [" + id.toString() + "]");

	    };
	};
    };
  Solution->submitLocalParameters(NULL, NULL);

  saveSession();

  //  currentSesChange(CurrentSesLI);
  delete Session;

  if (!(Session=Wvlbi->loadSession(SesInfo->name()))) // just to explain before geting SIGSEGV ;)
    Log->write(SBLog::ERR, SBLog::PREPROC, ClassName() + ": got an empty session"); 

  updateSession();
  restoreInput();
  emit run( (IsRunning=FALSE) );
};

void SBVLBIPreProcess::clearPars()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": clearPars selected");

  if (IsRunning)
    return;

  blockInput();

  for (SBStationInfo *SI=Session->stationList()->first(); SI; SI=Session->stationList()->next())
    {
      SI->clearClocks();
      SI->setZw   (0.0);
      SI->setErrZw(0.0);
    };

  Session->clearRMSs();
  Session->delAttr(SBVLBISesInfo::preProc);
  Session->addHistoryLine("Session's a priori parameters have been cleaned.");

  saveSession();
  delete Session;

  if (!(Session=Wvlbi->loadSession(SesInfo->name()))) // just to explain before geting SIGSEGV ;)
    Log->write(SBLog::ERR, SBLog::PREPROC, ClassName() + ": got an empty session"); 
  updateSession();

  if (Solution)
    {
      Solution->deleteSolution();
      delete Solution;
      Solution = new SBSolution(Prj);
    };

  // force buttons to `local' state
  if (RBClocks[2]->isChecked())
    RBClocks[1]->setChecked(TRUE);
  if (RBZenith[2]->isChecked())
    RBZenith[1]->setChecked(TRUE);

  restoreInput();
};

void SBVLBIPreProcess::saveSession()
{
  QString Str;

  if (Wvlbi->networks()->count() && Session->networkID() != (Str=NetID->currentText()))
    Session->setNetworkID(Str);
  if (Session->userFlag() != (Str=UFlag->currentText()))
    Session->setUserFlag(Str);

  Session->checkAttributres();
  *SesInfo = *Session;

  CurrentSesLI->repaint();
 
  isModified = TRUE;
  Wvlbi->saveSession(Session);
};

void SBVLBIPreProcess::dumpUI()
{
  if (Session)
    Session->dumpUserInfo(SetUp->path2UIDump());
};

void SBVLBIPreProcess::restoreUI()
{
  if (IsRunning)
    return;

  blockInput();

  if (Session && Session->restoreUserInfo(SetUp->path2UIDump()))
    {
      saveSession();
      updateSession();
    };
  restoreInput();
};

void SBVLBIPreProcess::doubleProc()
{
  //  accel->setEnabled(FALSE);
  preProcess();
  preProcess();
  //  accel->setEnabled(TRUE);
};

void SBVLBIPreProcess::procScenario_1()
{
  //  accel->setEnabled(FALSE);
  preProcess();

  // force buttons to `local' state
  if (RBClocks[1]->isChecked())
    RBClocks[2]->setChecked(TRUE);
  if (RBZenith[1]->isChecked())
    RBZenith[2]->setChecked(TRUE);

  preProcess();
  //  accel->setEnabled(TRUE);
};

void SBVLBIPreProcess::procScenario_2()
{
  IsNeedEstimateCables = TRUE;
  preProcess();
  SBParameterList	*CableList = new SBParameterList;
  CableList->setAutoDelete(TRUE);

  for (SBParameter *P=Solution->local().first(); P; P=Solution->local().next())
    if (P->name().contains("St: ") && P->name().contains("Cable"))
      CableList->inSort(new SBParameter(*P));
  
  //  CableList->report();

  SBCablesBrowser *CableBrowser = new SBCablesBrowser(Session->stationList(), CableList, Session->name(), this);
  CableBrowser->show();

  IsNeedEstimateCables = FALSE;
};

void SBVLBIPreProcess::procScenario_3()
{
  procScenario_1();
  preProcess();
};


void SBVLBIPreProcess::procScenario_4()
{
  QListViewItem	*lvItem=NULL;
  
  if (!IsLongRun)
    {
      IsLongRun = TRUE;
      while ((lvItem=brwsSes->lvSessions()->selectedItem()) && IsLongRun)
	{
	  doubleProc();
	  saveSession();
	  if ((lvItem=lvItem->itemAbove()))
	    {
	      brwsSes->lvSessions()->setSelected(lvItem, TRUE);
	      brwsSes->lvSessions()->ensureItemVisible(lvItem);
	    }
	  else
	    IsLongRun = FALSE;
	};
    }
  IsLongRun = FALSE;
};

void SBVLBIPreProcess::stopLongRun()
{
  IsLongRun=FALSE;
};
/*==============================================================================================*/



/*==============================================================================================*/
