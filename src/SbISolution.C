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



#include <qaccel.h>
#include <qapplication.h>
#include <qdir.h> 
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qmessagebox.h>





#include "SbGeoDelay.H"
#include "SbGeoProject.H"

#include "SbGeoStations.H"
#include "SbGeoSources.H"


#include "SbIPlot.H"
#include "SbISolution.H"


#include "SbSetup.H"





/*==============================================================================================*/
/*												*/
/* SBParameterLI..										*/
/*												*/
/*												*/
/*==============================================================================================*/
QString SBParameterLI::text(int col) const
{
  //  static const char* sOrig [ 4] = {"NGS", "VBD", "DBH", " "};
  TmpString = "NULL";
  if (P)
    switch (col)
      {
      case  0: TmpString = P->name().copy();
	break;
      case  1: TmpString.sprintf("%.2f ", P->v()*P->scale());
	break;
      case  2: TmpString.sprintf("%.2f ", P->e()*P->scale());
	break;
      case  3: TmpString.sprintf("%d ", P->num());
	break;
      case  4: TmpString = P->tStart().toString(SBMJD::F_YYYYMMDDHHMMSSSS);
	break;
      case  5: TmpString = P->tEpoch().toString(SBMJD::F_YYYYMMDDHHMMSSSS);
	break;
      case  6: TmpString = P->tFinis().toString(SBMJD::F_YYYYMMDDHHMMSSSS);
	break;
      };
  return TmpString;
};

QString SBParameterLI::key(int col, bool) const
{
  TmpString = "NULL";
  if (P)
    switch (col)
      {
      case  0: TmpString = P->name().copy();
	break;
      case  1: TmpString.sprintf("%020.16f ", P->v()*P->scale());
	break;
      case  2: TmpString.sprintf("%020.16f ", P->e()*P->scale());
	break;
      case  3: TmpString.sprintf("%09d ", P->num());
	break;
      case  4: TmpString.sprintf("%f", (double)(P->tStart()));
	break;
      case  5: TmpString.sprintf("%f", (double)(P->tEpoch()));
	break;
      case  6: TmpString.sprintf("%f", (double)(P->tFinis()));
	break;
      };
  return TmpString;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBSolutionBatchLI..										*/
/*												*/
/*												*/
/*==============================================================================================*/
QString SBSolutionBatchLI::text(int col) const
{
  TmpString = "NULL";
  if (S)
    switch (col)
      {
      case  0: TmpString = S->name().copy();
	break;
      case  1: TmpString.sprintf("%.4f ", Scale*S->WRMS_unit());
	break;
      case  2: TmpString.sprintf("%.4f ", Scale*S->VarFactor_unit());
	break;
      case  3: TmpString.sprintf("%d ", S->numObs());
	break;
      case  4: TmpString.sprintf("%d ", S->numPars());
	break;

      };
  return TmpString;
};

QString SBSolutionBatchLI::key(int col, bool) const
{
  TmpString = "NULL";
  if (S)
    switch (col)
      {
      case  0: TmpString = S->tStart().toString(SBMJD::F_INTERNAL);
	break;
      case  1: TmpString.sprintf("%020.16f", Scale*S->WRMS_unit());
	break;
      case  2: TmpString.sprintf("%020.16f", Scale*S->VarFactor_unit());
	break;
      case  3: TmpString.sprintf("%09d", S->numObs());
	break;
      case  4: TmpString.sprintf("%09d", S->numPars());
	break;
      };
  return TmpString;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBSolutionBrowser..										*/
/*												*/
/*==============================================================================================*/
SBSolutionBrowser::SBSolutionBrowser(QWidget* parent, const char* name, bool modal, WFlags f)
  : SBTabDialog(parent, name, modal, f)
{
  isModified	= FALSE;

  IsGlobalPars	= FALSE;
  IsLocalSta	= FALSE;
  IsLocalSou	= FALSE;
  IsLocalEOP	= FALSE;
  IsStochSta	= FALSE;
  IsStochSou	= FALSE;
  IsStochEOP	= FALSE;

  PC_SessRMS		  = NULL;
  PC_LocalStPars	  = NULL;
  PC_LocalSoPars	  = NULL;
  PC_LocalEOPPars	  = NULL;
  StochasticsStationsPlot = NULL;
  StochasticsStationsPC   = NULL;
  StochasticsSourcesPlot  = NULL;
  StochasticsSourcesPC    = NULL;
  StochasticsEOPsPlot	  = NULL;
  StochasticsEOPsPC	  = NULL;

  QApplication::setOverrideCursor(Qt::waitCursor);

  mainSetOperation("Loading Project");
  Prj = SetUp->loadProject();
  mainSetOperation("Creating Solution");
  Solution = new SBSolution(Prj);

  Scale	= SetUp->scale();
  ScaleName = SetUp->scaleName();
  
  setCaption("Solution of the project: " + Prj->name());

  lookupParameters();
  addTab(wGeneralInfo(), "General Info");
  addTab(wWRMSs(),       "WRMS per session");

  if (IsGlobalPars)
    addTab(wGlobalPars(),  "Global Par.");
  if (IsLocalSta)
    addTab(wLocalStPars(), "Loc.Par.(Station)");
  if (IsLocalSou)
    addTab(wLocalSoPars(), "Loc.Par.(Source)");
  if (IsLocalEOP)
    addTab(wLocalEOPPars(),"Loc.Par.(EOP)");
  if (IsStochSta)
    addTab(wStochStPars(), "Stoch.Par.(Stations)");
  if (IsStochSou)
    addTab(wStochSoPars(), "Stoch.Par.(Sources)");
  if (IsStochEOP)
    addTab(wStochEOPPars(), "Stoch.Par.(EOP)");

  
  QString	Dir2Output = SetUp->path2Output() + Prj->name() + "/";
  QDir		dir;
  if (!dir.exists(Dir2Output) && !dir.mkdir(Dir2Output))
    {
      Log->write(SBLog::ERR, SBLog::RUN, ClassName() + ": can't create dir [" + Dir2Output + "]");
      QMessageBox::critical(0, Version.selfName(), 
			    QString("Can't create output dir\n " + Dir2Output +
				    "\nfor the project [" + Prj->name() + "]"));
      // exit(1); ?
    };

  
  //  setCancelButton();
  setHelpButton("Dump");
  connect(this, SIGNAL(helpButtonPressed()), SLOT(dumpSolution()));


  QAccel *accel = new QAccel(this);
  accel->connectItem(accel->insertItem(Key_C+ALT), this, SLOT(updateCRF()));
  accel->connectItem(accel->insertItem(Key_T+ALT), this, SLOT(updateTRF()));


  setOkButton("Close");
  QApplication::restoreOverrideCursor();
};

SBSolutionBrowser::~SBSolutionBrowser()
{
  if (StochasticsStationsPC)
    {
      delete StochasticsStationsPC;
      StochasticsStationsPC = NULL;
    };

  if (StochasticsSourcesPC)
    {
      delete StochasticsSourcesPC;
      StochasticsSourcesPC = NULL;
    };

  if (StochasticsEOPsPC)
    {
      delete StochasticsEOPsPC;
      StochasticsEOPsPC = NULL;
    };

  if (PC_SessRMS)
    {
      delete PC_SessRMS;
      PC_SessRMS = NULL;
    };

  if (PC_LocalStPars)
    {
      delete PC_LocalStPars;
      PC_LocalStPars = NULL;
    };

  if (PC_LocalSoPars)
    {
      delete PC_LocalSoPars;
      PC_LocalSoPars = NULL;
    };

  if (PC_LocalEOPPars)
    {
      delete PC_LocalEOPPars;
      PC_LocalEOPPars = NULL;
    };

  if (Solution) 
    {
      delete Solution;
      Solution = NULL;
    };
  
  if (Prj) 
    {
      delete Prj;
      Prj = NULL;
    };
};
 
void SBSolutionBrowser::modified(bool isModified_)
{
  isModified |= isModified_;
};

void SBSolutionBrowser::dumpSolution()
{
  if (Solution)
    {
      QApplication::setOverrideCursor(Qt::waitCursor);  
      Solution->dumpParameters();
      QApplication::restoreOverrideCursor();
    };
};

QWidget* SBSolutionBrowser::wGeneralInfo()
{
  QGridLayout	*grid;
  QLabel	*label;
  QWidget	*W	= new QWidget(this);

  QBoxLayout	*Layout = new QHBoxLayout(W, 20, 10);

  QBoxLayout	*aLayout = new QVBoxLayout();
  Layout->addLayout(aLayout);

  // general stats:
  QGroupBox* gbox = new QGroupBox("General Statistics", W);
  grid = new QGridLayout(gbox, 4,2,   20,5);
  
  label = new QLabel("Weighted Root Means Squares:", gbox);
  label -> setMinimumSize(label->sizeHint());
  grid  -> addWidget(label, 0,0);

  lGenWRMS = new QLabel(QString().sprintf("%.4f", 
					  Scale*Solution->statGeneral()->WRMS_unit()) + ScaleName, gbox);
  lGenWRMS -> setMinimumSize(lGenWRMS->sizeHint());
  lGenWRMS -> setAlignment(Qt::AlignRight);
  grid  -> addWidget(lGenWRMS, 0,1);

  label = new QLabel("Variance Factor:", gbox);
  label -> setMinimumSize(label->sizeHint());
  grid  -> addWidget(label, 1,0);

  lGenVarF = new QLabel(QString().sprintf("%.4f", 
					  Scale*Solution->statGeneral()->VarFactor_unit()) + ScaleName, gbox);
  lGenVarF -> setMinimumSize(lGenVarF->sizeHint());
  lGenVarF -> setAlignment(Qt::AlignRight);
  grid  -> addWidget(lGenVarF, 1,1);

  label = new QLabel("Num. of Observations/Constrains:", gbox);
  label -> setMinimumSize(label->sizeHint());
  grid  -> addWidget(label, 2,0);

  lGenNumObs = new QLabel(QString().sprintf("%d/%d", 
					    Solution->statGeneral()->numObs(), 
					    Solution->statGeneral()->numConstr()), gbox);
  lGenNumObs -> setMinimumSize(lGenNumObs->sizeHint());
  lGenNumObs -> setAlignment(Qt::AlignRight);
  grid  -> addWidget(lGenNumObs, 2,1);

  label = new QLabel("Number of Parameters:", gbox);
  label -> setMinimumSize(label->sizeHint());
  grid  -> addWidget(label, 3,0);

  lGenNumPars = new QLabel(QString().sprintf("%d", Solution->statGeneral()->numPars()), gbox);
  lGenNumPars -> setMinimumSize(lGenNumPars->sizeHint());
  lGenNumPars -> setAlignment(Qt::AlignRight);
  grid  -> addWidget(lGenNumPars, 3,1);

  grid  ->activate();
  aLayout->addWidget(gbox);


  // list of batches and statistics:
  gbox = new QGroupBox("Statistics per batch", W);
  QBoxLayout	*bLayout = new QVBoxLayout(gbox, 20, 10);
  BatchesLV = new QListView(gbox);
  BatchesLV -> addColumn("Batch Name");
  BatchesLV -> setColumnAlignment( 0, AlignRight);
  BatchesLV -> addColumn("WRMS," + ScaleName);
  BatchesLV -> setColumnAlignment( 1, AlignRight);
  BatchesLV -> addColumn("Var.Fact.," + ScaleName);
  BatchesLV -> setColumnAlignment( 2, AlignRight);
  BatchesLV -> addColumn("N. Obs");
  BatchesLV -> setColumnAlignment( 3, AlignRight);
  BatchesLV -> addColumn("N. Pars");
  BatchesLV -> setColumnAlignment( 4, AlignRight);

  SBStatistics *Stat=NULL;
  for (Stat = Solution->statByBatch().first(); Stat; Stat = Solution->statByBatch().next())
    {
      (void) new SBSolutionBatchLI(BatchesLV, Stat, Scale);
    };
  
  BatchesLV->setAllColumnsShowFocus(TRUE);
  BatchesLV->setMinimumSize(BatchesLV->sizeHint());
  BatchesLV->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  if (BatchesLV->firstChild())
    BatchesLV->setSelected(BatchesLV->firstChild(), TRUE);
  BatchesLV->setFocusPolicy(QWidget::StrongFocus);
  bLayout->addWidget(BatchesLV);
  bLayout->activate();
  aLayout->addWidget(gbox);


  Layout->activate();
  return W;
};

QWidget* SBSolutionBrowser::wGlobalPars()
{
  QWidget	*W	= new QWidget(this);

  QBoxLayout	*Layout = new QVBoxLayout(W, 20, 10);

  // global parameters:
  GlobalParsLV = new QListView(W);
  GlobalParsLV -> addColumn("Name");
  GlobalParsLV -> setColumnAlignment( 0, AlignRight);
  GlobalParsLV -> addColumn("Value");
  GlobalParsLV -> setColumnAlignment( 1, AlignRight);
  GlobalParsLV -> addColumn("Error");
  GlobalParsLV -> setColumnAlignment( 2, AlignRight);
  GlobalParsLV -> addColumn("Number");
  GlobalParsLV -> setColumnAlignment( 3, AlignRight);
  GlobalParsLV -> addColumn("Start");
  GlobalParsLV -> setColumnAlignment( 4, AlignRight);
  GlobalParsLV -> addColumn("Epoch");
  GlobalParsLV -> setColumnAlignment( 5, AlignRight);
  GlobalParsLV -> addColumn("Finis");
  GlobalParsLV -> setColumnAlignment( 6, AlignRight);

  SBParameter	*P;
  for (P=Solution->global().first(); P; P=Solution->global().next())
    (void) new SBParameterLI(GlobalParsLV, P);
  
  GlobalParsLV -> setAllColumnsShowFocus(TRUE);
  GlobalParsLV -> setMinimumSize(GlobalParsLV->sizeHint());
  GlobalParsLV -> setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  if (GlobalParsLV->firstChild())
    GlobalParsLV -> setSelected(GlobalParsLV->firstChild(), TRUE);
  GlobalParsLV -> setFocusPolicy(QWidget::StrongFocus);
  Layout  -> addWidget(GlobalParsLV);
  Layout  -> activate();

  return W;
};

QWidget* SBSolutionBrowser::wWRMSs()
{
  QWidget	*W = new QWidget(this);
  QBoxLayout	*Layout = new QVBoxLayout(W, 20, 10);

  // should we split sessions by "Network names"?

  // plot of the wrms by session:
  PC_SessRMS = new SBPlotCarrier(1 + 1, 0, 
				 "Plot of the session's wrms, " + SetUp->scaleName() + ". Project " +
				 Prj->name());
  PC_SessRMS->setFile4SaveBaseName(Prj->name() + "/" + "WRMS");
  PC_SessRMS->createBranch(Prj->VLBIsessionList()->count(), "Sessions");

  SBPlotBranch	*pb = PC_SessRMS->branches()->at(0);
  if (pb)
    {
      pb->Idx = 0;
      for (SBVLBISesInfo *SI=Prj->VLBIsessionList()->first(); SI; SI=Prj->VLBIsessionList()->next())
	{
	  pb->data()->set(pb->Idx,  0,	SI->tMean());
	  pb->data()->set(pb->Idx,  1,	SI->dWRMS()*SetUp->scale());
	  pb->Idx++;
	};
    };
  
  PC_SessRMS->columnNames()->append(new QString("Time, MJD"));		// 0
  PC_SessRMS->setType(0, SBPlotCarrier::AT_MJD);
  PC_SessRMS->columnNames()->append(new QString("WRMS," + SetUp->scaleName()));		// 1

  SBPlot *wrmsPlot = new SBPlot(PC_SessRMS, W, 
				SBPlot::PM_HAS_HAVE_ZERO | SBPlot::PM_IMPULSE);
  //				SBPlot::PM_HAS_HAVE_ZERO | SBPlot::PM_IMPULSE | SBPlot::PM_WO_BRANCH_NAMES |
  //				SBPlot::PM_WO_AXIS_NAMES);
  Layout->addWidget(wrmsPlot);
  
  Layout -> activate();
  return W;
};

void SBSolutionBrowser::lookupParameters()
{
  QString		Str, *sTmp=NULL;
  SBParameter		*P;
  SBStochParameter	*Ps;
  SBStationInfo		*SI = NULL;
  SBStationID		id;
  SBSourceInfo		*SrcI = NULL, sID("BUFF");

  SBStatistics		*Stat;
  LocalStPars.clear();
  LocalSoPars.clear();
  LocalEOPPars.clear();

  LocStationParameters.setAutoDelete(TRUE);
  LocStations.setAutoDelete(TRUE);

  LocSourceParameters.setAutoDelete(TRUE);
  LocSources.setAutoDelete(TRUE);

  if (Solution->global().count())
    IsGlobalPars = TRUE;
  
  for (Stat = Solution->statByBatch().first(); Stat; Stat = Solution->statByBatch().next())
    {
      Solution->prepare4Batch(Stat->name());
      for (P=Solution->local().first(); P; P=Solution->local().next())
	{
	  // EOPs:
	  if (P->name().contains("EOP: "))
	    {
	      if ( !(Ps = LocalEOPPars.find(P->name())) )
		LocalEOPPars.inSort( (Ps = new SBStochParameter(P->name(), P->scale())) );
	      Ps -> addPar(P);
	    };
	  // stations:
	  if (P->name().contains("St: "))
	    {
	      if ( !(Ps = LocalStPars.find(P->name())) )
		{
		  //		  std::cout << "inserting " << P->name() << "\n";
		  LocalStPars.inSort( (Ps = new SBStochParameter(P->name(), P->scale())) );
		};
	      Ps -> addPar(P);
	      //	      std::cout << Ps->name() << ": count: " << Ps->count() << "\n";

	      // split for branches:
	      Str = P->name().mid(4, 9);
	      if (id.isValidStr(Str))
		id.setID(Str);
	      else
		Log->write(SBLog::ERR, SBLog::ESTIMATOR, ClassName() + ": cannot convert parameter name [" +
			   P->name() + "] to Station ID");
	      if (id.isValidId())
		{
		  if (!(SI=LocStations.find(&id))) // new station
		    {
		      SBStationInfo* siTMP=NULL;
		      siTMP = Prj->stationList()->find(&id);
		      if (siTMP)
			{
			  //SI = new SBStationInfo(id, LocStations.count(), Prj->stationList()->find(&id)->aka());
			  SI = new SBStationInfo(id, LocStations.count(), siTMP->aka());
			}
		      else
			{
			  Log->write(SBLog::INF, SBLog::ESTIMATOR, ClassName() + 
				     ": cannot find aka for the station [" + id.toString() + "]; parameter: " + 
				     P->name() + "; batch: " + Stat->name());
			  SI = new SBStationInfo(id, LocStations.count(), "UNKNOWN");
			};
		      SI ->setNum(0);
		      LocStations.inSort(SI);
		    };
		};
	      // split for columns (parsing parameters names):
	      Str = P->name().mid(15);
	      sTmp = LocStationParameters.first();
	      while (sTmp && *sTmp!=Str)
		sTmp = LocStationParameters.next();
	      if (!sTmp) // not found
		LocStationParameters.append(new QString(Str));
	    };
	  // sources:
	  if (P->name().contains("So: "))
	    {
	      if ( !(Ps = LocalSoPars.find(P->name())) )
		LocalSoPars.inSort( (Ps = new SBStochParameter(P->name(), P->scale())) );
	      Ps -> addPar(P);

	      // split for branches:
	      Str = P->name().mid(4, 8);
	      sID.setName(Str);
	      if ( !(SrcI = LocSources.find(&sID)) ) // new source
		{
		  SrcI = new SBSourceInfo(Str, LocSources.count(), Prj->sourceList()->find(&sID)->aka());
		  SrcI ->setNum(0);
		  LocSources.inSort(SrcI);
		};
	      // split for columns (parsing parameters names):
	      Str = P->name().mid(14);
	      sTmp = LocSourceParameters.first();
	      while (sTmp && *sTmp!=Str)
		sTmp = LocSourceParameters.next();
	      if (!sTmp) // not found
		LocSourceParameters.append(new QString(Str));
	    };
	};
      for (Ps=Solution->stochastic().first(); Ps; Ps=Solution->stochastic().next())
	{
	  // stations:
	  if (Ps->name().contains("St: "))
	    IsStochSta = TRUE;
	  // sources:
	  if (Ps->name().contains("So: "))
	    IsStochSou = TRUE;
	  // EOP:
	  if (Ps->name().contains("EOP: "))
	    IsStochEOP = TRUE;
	};
    };

  for (Ps = LocalStPars.first(); Ps; Ps = LocalStPars.next())
    {
      SI = LocStations.first();
      while (SI && !Ps->name().contains(SI->toString()))
	SI=LocStations.next();
      if (SI /*&& SI->num()<Ps->count()*/)
	SI->setNum(Ps->count());
    };
  for (Ps = LocalSoPars.first(); Ps; Ps = LocalSoPars.next())
    {
      SrcI = LocSources.first();
      while (SrcI && !Ps->name().contains(SrcI->name()))
	SrcI = LocSources.next();
      if (SrcI /*&& SrcI->num()<Ps->count()*/)
	SrcI->setNum(Ps->count());
    };

  if (LocalStPars.count())
    IsLocalSta = TRUE;

  if (LocalSoPars.count())
    IsLocalSou = TRUE;

  if (LocalEOPPars.count())
    IsLocalEOP = TRUE;

};

QWidget* SBSolutionBrowser::wLocalStPars()
{
  QWidget	*W	= new QWidget(this);
  QBoxLayout	*Layout = new QVBoxLayout(W, 20, 10);


  SBPlotBranch	*pb = NULL;

  SBParameter		*P;
  SBStochParameter	*Ps;

  SBStationInfo		*SI = NULL;
  SBStationID		id;
  QString		Str, *sTmp=NULL;


  // create plot carrier for stations:
  unsigned int	NumStColumns = LocStationParameters.count();
  PC_LocalStPars = new SBPlotCarrier(1 + 2*NumStColumns, NumStColumns, 
				     "Plots of the local parameters (stations) for the solution " + 
				     Prj->name());
  PC_LocalStPars->setFile4SaveBaseName(Prj->name() + "/" + "Loc_Stations");

  if (LocStations.count())
    for (SI=LocStations.first(); SI; SI=LocStations.next())
      {
	//	std::cout << SI->aka() << ": " << SI->num() << "\n";
	PC_LocalStPars->createBranch(SI->num(), SI->aka());
      };

  PC_LocalStPars->columnNames()->append(new QString("Time, UTC"));		// 0
  PC_LocalStPars->setType(0, SBPlotCarrier::AT_MJD);


  for (sTmp=LocStationParameters.first(); sTmp; sTmp=LocStationParameters.next())
    PC_LocalStPars->columnNames()->append(new QString(*sTmp));
  
  for (sTmp=LocStationParameters.first(); sTmp; sTmp=LocStationParameters.next())
    PC_LocalStPars->columnNames()->append(new QString("d(" + *sTmp + ")"));

  unsigned int	i=0;
  for (sTmp=LocStationParameters.first(); sTmp; i++, sTmp=LocStationParameters.next())
    {
      PC_LocalStPars->columnNames()->append(new QString("E[" + *sTmp + "]"));
      PC_LocalStPars->setStdVarIdx( 1 + i,                 2*NumStColumns + 1 + i);
      PC_LocalStPars->setStdVarIdx( 1 + NumStColumns + i,  2*NumStColumns + 1 + i);
    };

  for (Ps = LocalStPars.first(); Ps; Ps = LocalStPars.next())
    {
      Str = Ps->name().mid(4, 9);
      if (id.isValidStr(Str))
	id.setID(Str);
      else
	Log->write(SBLog::ERR, SBLog::ESTIMATOR, ClassName() + ": cannot convert parameter name [" +
		   Ps->name() + "] to Station ID");
      if (id.isValidId())
	{
	  if ((SI=LocStations.find(&id)))
	    {
	      //	      Str = SI->toString() + ", aka " + SI->aka();
	      Str = SI->aka();
	      if ((pb=PC_LocalStPars->findBranch(Str)))
		{
		  Str = Ps->name().mid(15);
		  sTmp = LocStationParameters.first();
		  while (sTmp && *sTmp!=Str)
		    sTmp = LocStationParameters.next();
		  if (!sTmp) // not found
		    Log->write(SBLog::ERR, SBLog::INTERFACE, ClassName() + 
			       ": cannot find plot column for station parameter [" + Str + "]");
		  else
		    {
		      i = LocStationParameters.at()+1;
		      pb->Idx = 0;
		      for (P=Ps->first(); P; P=Ps->next())
			{
			  pb->data()->set(pb->Idx,  0,			P->tEpoch());
			  pb->data()->set(pb->Idx,  i,			P->v()*P->scale());
			  pb->data()->set(pb->Idx,  i +   NumStColumns,	P->s()*P->scale());
			  pb->data()->set(pb->Idx,  i + 2*NumStColumns,	P->e()*P->scale());
			  pb->Idx++;
			};
		    };
		}
	      else // pb == NULL
		Log->write(SBLog::ERR, SBLog::INTERFACE, ClassName() + 
			   ": cannot find plot branch for station [" + Str + "]");
	    };
	};
    };

  SBPlot *StPlot = new SBPlot(PC_LocalStPars, W);
  Layout->addWidget(StPlot, 5);
  
  Layout -> activate();
  return W;
};

QWidget* SBSolutionBrowser::wLocalSoPars()
{
  QWidget	*W	= new QWidget(this);
  QBoxLayout	*Layout = new QVBoxLayout(W, 20, 10);

  SBPlotBranch	*pb = NULL;

  SBParameter		*P;
  SBStochParameter	*Ps;

  SBSourceInfo		*SI = NULL, sID("BUFF");
  QString		Str, *sTmp=NULL;

  // create plot carrier for sources:
  unsigned int	NumSoColumns = LocSourceParameters.count();
  PC_LocalSoPars = new SBPlotCarrier(1 + 2*NumSoColumns, NumSoColumns, 
				     "Plots of the local parameters (sources) for the solution " + 
				     Prj->name());
  PC_LocalSoPars->setFile4SaveBaseName(Prj->name() + "/" + "Loc_Sources");

  if (LocSources.count())
    for (SI=LocSources.first(); SI; SI=LocSources.next())
      PC_LocalSoPars->createBranch(SI->num(), SI->aka());
  
  PC_LocalSoPars->columnNames()->append(new QString("Time, MJD"));		// 0
  PC_LocalSoPars->setType(0, SBPlotCarrier::AT_MJD);

  for (sTmp=LocSourceParameters.first(); sTmp; sTmp=LocSourceParameters.next())
    PC_LocalSoPars->columnNames()->append(new QString(*sTmp));
  
  for (sTmp=LocSourceParameters.first(); sTmp; sTmp=LocSourceParameters.next())
    PC_LocalSoPars->columnNames()->append(new QString("d(" + *sTmp + ")"));

  unsigned int	i=0;
  for (sTmp=LocSourceParameters.first(); sTmp; i++, sTmp=LocSourceParameters.next())
    {
      PC_LocalSoPars->columnNames()->append(new QString("E[" + *sTmp + "]"));
      PC_LocalSoPars->setStdVarIdx( 1 + i,                 2*NumSoColumns + 1 + i);
      PC_LocalSoPars->setStdVarIdx( 1 + NumSoColumns + i,  2*NumSoColumns + 1 + i);
    };

  for (Ps = LocalSoPars.first(); Ps; Ps = LocalSoPars.next())
    {
      Str = Ps->name().mid(4, 8);
      sID.setName(Str);
      if ((SI=LocSources.find(&sID)))
	{
	  Str = SI->aka();
	  if ( (pb=PC_LocalSoPars->findBranch(Str)) )
	    {
	      Str = Ps->name().mid(14);
	      sTmp = LocSourceParameters.first();
	      while (sTmp && *sTmp!=Str)
		sTmp = LocSourceParameters.next();
	      if (!sTmp) // not found
		Log->write(SBLog::ERR, SBLog::INTERFACE, ClassName() + 
			   ": cannot find plot column for the source parameter [" + Str + "]");
	      else
		{
		  i = LocSourceParameters.at() + 1;
		  pb->Idx = 0;
		  for (P=Ps->first(); P; P=Ps->next())
		    {
		      pb->data()->set(pb->Idx,  0,			P->tEpoch());
		      pb->data()->set(pb->Idx,  i,			P->v()*P->scale());
		      pb->data()->set(pb->Idx,  i +   NumSoColumns,	P->s()*P->scale());
		      pb->data()->set(pb->Idx,  i + 2*NumSoColumns,	P->e()*P->scale());
		      pb->Idx++;
		    };
		};
	    }
	  else // pb == NULL
	    Log->write(SBLog::ERR, SBLog::INTERFACE, ClassName() + 
		       ": cannot find plot branch for the source [" + Str + "]");
	};
    };
  
  SBPlot *SoPlot = new SBPlot(PC_LocalSoPars, W);
  Layout->addWidget(SoPlot, 5);
  
  Layout -> activate();
  return W;
};

QWidget* SBSolutionBrowser::wLocalEOPPars()
{
  QWidget	*W	= new QWidget(this);
  QBoxLayout	*Layout = new QVBoxLayout(W, 20, 10);


  SBPlotBranch	*pb = NULL;

  SBParameter		*P;
  SBStochParameter	*Ps;

  // create plot carrier for stations:
  unsigned int	NumEOPColumns = LocalEOPPars.count();
  PC_LocalEOPPars = new SBPlotCarrier(1 + 2*NumEOPColumns, NumEOPColumns, 
				      "Plots of the local parameters (EOP) for the solution " + 
				      Prj->name());
  PC_LocalEOPPars->setFile4SaveBaseName(Prj->name() + "/" + "Loc_EOP");
  
  if (LocalEOPPars.count())
    PC_LocalEOPPars->createBranch(LocalEOPPars.first()->count(), "EOP");
  
  PC_LocalEOPPars->columnNames()->append(new QString("Time, MJD"));		// 0
  PC_LocalEOPPars->setType(0, SBPlotCarrier::AT_MJD);


  for (Ps=LocalEOPPars.first(); Ps; Ps=LocalEOPPars.next())
    PC_LocalEOPPars->columnNames()->append(new QString(Ps->name()));

  for (Ps=LocalEOPPars.first(); Ps; Ps=LocalEOPPars.next())
    PC_LocalEOPPars->columnNames()->append(new QString("d(" + Ps->name() + ")"));

  unsigned int	i=0;
  for (Ps=LocalEOPPars.first(); Ps; i++, Ps=LocalEOPPars.next())
    {
      PC_LocalEOPPars->columnNames()->append(new QString("E[" + Ps->name() + "]"));
      PC_LocalEOPPars->setStdVarIdx( 1 + i,  2*NumEOPColumns + 1 + i);
      PC_LocalEOPPars->setStdVarIdx( 1 + NumEOPColumns + i,  2*NumEOPColumns + 1 + i);
    };

  if ((pb=PC_LocalEOPPars->branches()->first()))
    {
      i = 1;
      for (Ps = LocalEOPPars.first(); Ps; i++, Ps = LocalEOPPars.next())
	{
	  pb->Idx = 0;
	  for (P=Ps->first(); P; P=Ps->next())
	    {
	      pb->data()->set(pb->Idx,  0,			P->tEpoch());
	      pb->data()->set(pb->Idx,  i,			P->v()*P->scale());
	      pb->data()->set(pb->Idx,  i +   NumEOPColumns,	P->s()*P->scale());
	      pb->data()->set(pb->Idx,  i + 2*NumEOPColumns,	P->e()*P->scale());
	      pb->Idx++;
	    };
	};
    }
  else // pb == NULL
    Log->write(SBLog::ERR, SBLog::INTERFACE, ClassName() + ": cannot find plot branch for EOP ");
  
  SBPlot *StPlot = new SBPlot(PC_LocalEOPPars, W);
  Layout->addWidget(StPlot, 5);
  
  Layout -> activate();
  return W;
};

QWidget* SBSolutionBrowser::wStochStPars()
{
  QListView		*Batch4StochLV;

  QWidget	*W	= new QWidget(this);
  QBoxLayout	*Layout = new QVBoxLayout(W, 20, 10);
  QBoxLayout	*aLayout= new QHBoxLayout(Layout);
  
  Batch4StochLV = new QListView(W);
  Batch4StochLV -> addColumn("Name");
  Batch4StochLV -> setColumnAlignment( 0, AlignRight);


  SBStatistics *Stat=NULL;
  for (Stat = Solution->statByBatch().first(); Stat; Stat = Solution->statByBatch().next())
    {
      (void) new SBSolutionBatchLI(Batch4StochLV, Stat, Scale);
    };
  
  Batch4StochLV -> setAllColumnsShowFocus(TRUE);
  Batch4StochLV -> setMinimumSize(Batch4StochLV->sizeHint());
  Batch4StochLV -> setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  if (Batch4StochLV->firstChild())
    Batch4StochLV -> setSelected(Batch4StochLV->firstChild(), TRUE);
  Batch4StochLV -> setFocusPolicy(QWidget::StrongFocus);
  aLayout   -> addWidget(Batch4StochLV);

  if (Batch4StochLV->firstChild())
    batch4StochStChanged(Batch4StochLV->firstChild());
  else
    {
      StochasticsStationsPC = 
	new SBPlotCarrier(0, 0, 
			  "Plots of the stochastic parameters (stations) for the solution " + Prj->name());
      StochasticsStationsPC->setFile4SaveBaseName(Prj->name() + "/" + "[EMPTY]_Stoch_Stations");
    };

  StochasticsStationsPlot = new SBPlot(StochasticsStationsPC, W);
  aLayout->addWidget(StochasticsStationsPlot, 5);


  connect(Batch4StochLV, SIGNAL(selectionChanged(QListViewItem *)), 
	  SLOT(batch4StochStChanged(QListViewItem*)));

  Layout -> activate();
  return W;
};

void SBSolutionBrowser::batch4StochStChanged(QListViewItem* LI)
{
  SBStochParameter	*Ps;
  SBStochParameter	*StPar;
  QString		Str, *sTmp=NULL;

  QList<QString>	StationParameters;
  StationParameters.setAutoDelete(TRUE);


  SBStationInfoList	Stations;
  SBStationInfo		*SI = NULL;
  SBStationID		id;

  SBPlotBranch		*pb = NULL;


  StochStPars.setAutoDelete(FALSE);
  StochStPars.clear();
  Stations.setAutoDelete(TRUE);
  Stations.clear();

  QApplication::setOverrideCursor(Qt::waitCursor);  
  SBStatistics	*Stat = ((SBSolutionBatchLI*)LI)->statistics();
  if (Stat)
    {
      Solution->prepare4Batch(Stat->name());
      for (Ps=Solution->stochastic().first(); Ps; Ps=Solution->stochastic().next())
	{
	  // stations:
	  if (Ps->name().contains("St: "))
	    {
	      if ( !(StPar = StochStPars.find(Ps->name())) )
		StochStPars.inSort( Ps );

	      // split for branches:
	      Str = Ps->name().mid(4, 9);
	      if (id.isValidStr(Str))
		id.setID(Str);
	      else
		Log->write(SBLog::ERR, SBLog::ESTIMATOR, ClassName() + ": cannot convert parameter name [" +
			   Ps->name() + "] to Station ID");
	      if (id.isValidId())
		{
		  if (!(SI=Stations.find(&id))) // new station
		    {
		      SI = new SBStationInfo(id, Stations.count(), Prj->stationList()->find(&id)->aka());
		      SI ->setNum(0);
		      Stations.inSort(SI);
		    };
		};
	      // split for columns:
	      Str = Ps->name().mid(15);
	      sTmp = StationParameters.first();
	      while (sTmp && *sTmp!=Str)
		sTmp = StationParameters.next();
	      if (!sTmp) // not found
		StationParameters.append(new QString(Str));
	    };
	};

      // fill numbers of points per branch
      for (Ps = StochStPars.first(); Ps; Ps = StochStPars.next())
	{
	  SI = Stations.first();
	  while (SI && !Ps->name().contains(SI->toString()))
	    SI=Stations.next();
	  if (SI)
	    SI->setNum(Ps->count());
	};
      
      // create plot carrier for stations:
      unsigned int	NumStColumns = StationParameters.count();
      if (!StochasticsStationsPC)
	{
	  StochasticsStationsPC = 
	    new SBPlotCarrier(1 + 2*NumStColumns, NumStColumns, 
			      "Plots of the stochastic parameters (stations) for the solution " +
			      Prj->name());

	  StochasticsStationsPC->columnNames()->append(new QString("Time, MJD"));
	  StochasticsStationsPC->setType(0, SBPlotCarrier::AT_MJD);

	  for (sTmp=StationParameters.first(); sTmp; sTmp=StationParameters.next())
	    StochasticsStationsPC->columnNames()->append(new QString(*sTmp));
	  
	  for (sTmp=StationParameters.first(); sTmp; sTmp=StationParameters.next())
	    StochasticsStationsPC->columnNames()->append(new QString("d(" + *sTmp + ")"));
	  
	  unsigned int	i=0;
	  for (sTmp=StationParameters.first(); sTmp; i++, sTmp=StationParameters.next())
	    {
	      StochasticsStationsPC->columnNames()->append(new QString("E[" + *sTmp + "]"));
	      StochasticsStationsPC->setStdVarIdx( 1 + i,  2*NumStColumns + 1 + i);
	      StochasticsStationsPC->setStdVarIdx( 1 + NumStColumns + i,  2*NumStColumns + 1 + i);
	    };
	};

      StochasticsStationsPC->setFile4SaveBaseName(Prj->name() + "/" + 
						  "[" + Stat->name() + "]" + "_Stoch_Stations");
      
      if (StochasticsStationsPC->branches()->count())
	StochasticsStationsPC->branches()->clear();

      if (Stations.count())
	for (SI=Stations.first(); SI; SI=Stations.next())
	  //	  StochasticsStationsPC->createBranch(SI->num(), SI->toString() + ", aka " + SI->aka());
	  StochasticsStationsPC->createBranch(SI->num(), SI->aka());
      
      for (Ps = StochStPars.first(); Ps; Ps = StochStPars.next())
	{
	  Str = Ps->name().mid(4, 9);
	  if (id.isValidStr(Str))
	    id.setID(Str);
	  else
	    Log->write(SBLog::ERR, SBLog::ESTIMATOR, ClassName() + ": cannot convert parameter name [" +
		       Ps->name() + "] to Station ID");
	  if (id.isValidId())
	    {
	      if ((SI=Stations.find(&id)))
		{
		  //		  Str = SI->toString() + ", aka " + SI->aka();
		  Str = SI->aka();
		  if ((pb=StochasticsStationsPC->findBranch(Str)))
		    {
		      Str = Ps->name().mid(15);
		      sTmp = StationParameters.first();
		      while (sTmp && *sTmp!=Str)
			sTmp = StationParameters.next();
		      if (!sTmp) // not found
			Log->write(SBLog::ERR, SBLog::INTERFACE, ClassName() + 
				   ": cannot find plot column for station parameter [" + Str + "]");
		      else
			{
			  unsigned int i = StationParameters.at()+1;
			  pb->Idx = 0;
			  SBParameter	*P;
			  for (P=Ps->first(); P; P=Ps->next())
			    {
			      pb->data()->set(pb->Idx,  0,			P->tEpoch());
			      pb->data()->set(pb->Idx,  i,			P->v()*P->scale());
			      pb->data()->set(pb->Idx,  i+NumStColumns,		P->s()*P->scale());
			      pb->data()->set(pb->Idx,  i+2*NumStColumns,	P->e()*P->scale());
			      pb->Idx++;
			    };
			};
		    }
		  else // pb == NULL
		    Log->write(SBLog::ERR, SBLog::INTERFACE, ClassName() + 
			       ": cannot find plot branch for station [" + Str + "]");
		};
	    };
	};
      if (StochasticsStationsPlot)
	StochasticsStationsPlot->dataChanged();
    };
  QApplication::restoreOverrideCursor();
};

QWidget* SBSolutionBrowser::wStochSoPars()
{
  QListView		*Batch4StochLV;

  QWidget	*W	= new QWidget(this);
  QBoxLayout	*Layout = new QVBoxLayout(W, 20, 10);
  QBoxLayout	*aLayout= new QHBoxLayout(Layout);
  
  Batch4StochLV = new QListView(W);
  Batch4StochLV -> addColumn("Name");
  Batch4StochLV -> setColumnAlignment( 0, AlignRight);


  SBStatistics *Stat=NULL;
  for (Stat = Solution->statByBatch().first(); Stat; Stat = Solution->statByBatch().next())
    {
      (void) new SBSolutionBatchLI(Batch4StochLV, Stat, Scale);
    };
  
  Batch4StochLV -> setAllColumnsShowFocus(TRUE);
  Batch4StochLV -> setMinimumSize(Batch4StochLV->sizeHint());
  Batch4StochLV -> setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  if (Batch4StochLV->firstChild())
    Batch4StochLV -> setSelected(Batch4StochLV->firstChild(), TRUE);
  Batch4StochLV -> setFocusPolicy(QWidget::StrongFocus);
  aLayout   -> addWidget(Batch4StochLV);

  if (Batch4StochLV->firstChild())
    batch4StochSoChanged(Batch4StochLV->firstChild());
  else
    {
      StochasticsSourcesPC = 
	new SBPlotCarrier(0, 0, 
			  "Plots of the stochastic parameters (sources) for the solution " + Prj->name());
      StochasticsSourcesPC->setFile4SaveBaseName(Prj->name() + "/" + "[EMPTY]_Stoch_Sources");
    };

  StochasticsSourcesPlot = new SBPlot(StochasticsSourcesPC, W);
  aLayout->addWidget(StochasticsSourcesPlot, 5);

  connect(Batch4StochLV, SIGNAL(selectionChanged(QListViewItem *)), 
	  SLOT(batch4StochSoChanged(QListViewItem*)));

  Layout -> activate();
  return W;
};

void SBSolutionBrowser::batch4StochSoChanged(QListViewItem* LI)
{
  SBPlotBranch		*pb = NULL;
  SBStochParameter	*Ps = NULL;
  SBStochParameter	*SrcPar = NULL;
  SBSourceInfoList	Sources;
  SBSourceInfo		*SI = NULL, sID("BUFF");
  QList<QString>	SourceParameters;
  QString		Str, *sTmp=NULL;

  Sources.setAutoDelete(TRUE);
  Sources.clear();
 
  StochSoPars.setAutoDelete(FALSE);
  StochSoPars.clear();
  SourceParameters.setAutoDelete(TRUE);

  QApplication::setOverrideCursor(Qt::waitCursor);  
  SBStatistics	*Stat = ((SBSolutionBatchLI*)LI)->statistics();
  if (Stat)
    {
      Solution->prepare4Batch(Stat->name());
      for (Ps=Solution->stochastic().first(); Ps; Ps=Solution->stochastic().next())
	{
	  // sources:
	  if (Ps->name().contains("So: "))
	    {
	      if ( !(SrcPar = StochSoPars.find(Ps->name())) )
		StochSoPars.inSort( Ps );

	      // split for branches:
	      Str = Ps->name().mid(4,8);
	      sID.setName(Str);
	      if ( !(SI=Sources.find(&sID)) ) // new sources
		{
		  SI = new SBSourceInfo(Str, Sources.count(), Prj->sourceList()->find(&sID)->aka());
		  SI ->setNum(0);
		  Sources.inSort(SI);
		};
	      
	      // split for columns (parsing parameters names):
	      Str = Ps->name().mid(14);
	      sTmp = SourceParameters.first();
	      while (sTmp && *sTmp!=Str)
		sTmp = SourceParameters.next();
	      if (!sTmp) // not found
		SourceParameters.append(new QString(Str));
	    };
	};

      // fill numbers of points per branch
      for (Ps = StochSoPars.first(); Ps; Ps = StochSoPars.next())
	{
	  SI = Sources.first();
	  while (SI && !Ps->name().contains(SI->name()))
	    SI=Sources.next();
	  if (SI)
	    SI->setNum(Ps->count());
	};
      
      // create plot carrier for sources:
      unsigned int	NumSoColumns = SourceParameters.count();
      if (!StochasticsSourcesPC)
	{
	  StochasticsSourcesPC = 
	    new SBPlotCarrier(1 + 2*NumSoColumns, NumSoColumns, 
			      "Plots of the stochastic parameters (sources) for the solution " +
			      Prj->name());

	  StochasticsSourcesPC->columnNames()->append(new QString("Time, UTC"));
	  StochasticsSourcesPC->setType(0, SBPlotCarrier::AT_MJD);

	  for (sTmp=SourceParameters.first(); sTmp; sTmp=SourceParameters.next())
	    StochasticsSourcesPC->columnNames()->append(new QString(*sTmp));
	  
	  for (sTmp=SourceParameters.first(); sTmp; sTmp=SourceParameters.next())
	    StochasticsSourcesPC->columnNames()->append(new QString("d(" + *sTmp + ")"));
	  
	  unsigned int	i=0;
	  for (sTmp=SourceParameters.first(); sTmp; i++, sTmp=SourceParameters.next())
	    {
	      StochasticsSourcesPC->columnNames()->append(new QString("E[" + *sTmp + "]"));
	      StochasticsSourcesPC->setStdVarIdx( 1 + i,  2*NumSoColumns + 1 + i);
	      StochasticsSourcesPC->setStdVarIdx( 1 + NumSoColumns + i,  2*NumSoColumns + 1 + i);
	    };
	};

      StochasticsSourcesPC->setFile4SaveBaseName(Prj->name() + "/" + 
						 "[" + Stat->name() + "]" + "_Stoch_Sources");
      
      if (StochasticsSourcesPC->branches()->count())
	StochasticsSourcesPC->branches()->clear();

      if (Sources.count())
	for (SI=Sources.first(); SI; SI=Sources.next())
	  StochasticsSourcesPC->createBranch(SI->num(), SI->aka());
      
      for (Ps = StochSoPars.first(); Ps; Ps = StochSoPars.next())
	{
	  Str = Ps->name().mid(4,8);
	  sID.setName(Str);
	  if ( (SI=Sources.find(&sID)) )
	    {
	      Str = SI->aka();
	      if ((pb=StochasticsSourcesPC->findBranch(Str)))
		{
		  Str = Ps->name().mid(14);
		  sTmp = SourceParameters.first();
		  while (sTmp && *sTmp!=Str)
		    sTmp = SourceParameters.next();
		  if (!sTmp) // not found
		    Log->write(SBLog::ERR, SBLog::INTERFACE, ClassName() + 
			       ": cannot find plot column for the source parameter [" + Str + "]");
		  else
		    {
		      unsigned int i = SourceParameters.at() + 1;
		      pb->Idx = 0;
		      SBParameter	*P;
		      for (P=Ps->first(); P; P=Ps->next())
			{
			  pb->data()->set(pb->Idx,  0,			P->tEpoch());
			  pb->data()->set(pb->Idx,  i,			P->v()*P->scale());
			  pb->data()->set(pb->Idx,  i+NumSoColumns,	P->s()*P->scale());
			  pb->data()->set(pb->Idx,  i+2*NumSoColumns,	P->e()*P->scale());
			  pb->Idx++;
			};
		    };
		}
	      else // pb == NULL
		Log->write(SBLog::ERR, SBLog::INTERFACE, ClassName() + 
			   ": cannot find plot branch for the source [" + Str + "]");
	    };
	};
      if (StochasticsSourcesPlot)
	StochasticsSourcesPlot->dataChanged();
    };

  QApplication::restoreOverrideCursor();
};

QWidget* SBSolutionBrowser::wStochEOPPars()
{
  QListView		*Batch4StochLV;

  QWidget	*W	= new QWidget(this);
  QBoxLayout	*Layout = new QVBoxLayout(W, 20, 10);
  QBoxLayout	*aLayout= new QHBoxLayout(Layout);
  
  Batch4StochLV = new QListView(W);
  Batch4StochLV -> addColumn("Name");
  Batch4StochLV -> setColumnAlignment( 0, AlignRight);

  SBStatistics *Stat=NULL;
  for (Stat = Solution->statByBatch().first(); Stat; Stat = Solution->statByBatch().next())
    {
      (void) new SBSolutionBatchLI(Batch4StochLV, Stat, Scale);
    };
  
  Batch4StochLV -> setAllColumnsShowFocus(TRUE);
  //  Batch4StochLV -> setMinimumSize(Batch4StochLV->sizeHint());
  Batch4StochLV -> setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  if (Batch4StochLV->firstChild())
    Batch4StochLV -> setSelected(Batch4StochLV->firstChild(), TRUE);
  Batch4StochLV -> setFocusPolicy(QWidget::StrongFocus);
  aLayout   -> addWidget(Batch4StochLV);

  if (Batch4StochLV->firstChild())
    batch4StochEOPChanged(Batch4StochLV->firstChild());
  else
    {
      StochasticsEOPsPC = 
	new SBPlotCarrier(0, 0, 
			  "Plots of the stochastic parameters (EOP) for the solution " + Prj->name());
      StochasticsEOPsPC->setFile4SaveBaseName(Prj->name() + "/" + "[EMPTY]_Stoch_EOP");
    };

  StochasticsEOPsPlot = new SBPlot(StochasticsEOPsPC, W);
  aLayout->addWidget(StochasticsEOPsPlot, 5);


  connect(Batch4StochLV, SIGNAL(selectionChanged(QListViewItem *)), 
	  SLOT(batch4StochEOPChanged(QListViewItem*)));

  Layout -> activate();
  return W;
};

void SBSolutionBrowser::batch4StochEOPChanged(QListViewItem* LI)
{
  SBPlotBranch		*pb = NULL;

  SBStochParameter	*Ps;
  SBStochParameter	*EOPPar;


  StochEOPPars.setAutoDelete(FALSE);
  StochEOPPars.clear();

  QApplication::setOverrideCursor(Qt::waitCursor);
  SBStatistics	*Stat = ((SBSolutionBatchLI*)LI)->statistics();
  if (Stat)
    {
      Solution->prepare4Batch(Stat->name());
      for (Ps=Solution->stochastic().first(); Ps; Ps=Solution->stochastic().next())
	{
	  // EOP:
	  if (Ps->name().contains("EOP: "))
	    {
	      if ( !(EOPPar = StochEOPPars.find(Ps->name())) )
		StochEOPPars.inSort( Ps );
	    };
	};

      // create plot carrier for stations:
      unsigned int	NumEOPColumns = StochEOPPars.count();
      if (!StochasticsEOPsPC)
	{
	  StochasticsEOPsPC = 
	    new SBPlotCarrier(1 + 2*NumEOPColumns, NumEOPColumns, 
			      "Plots of the stochastic parameters (EOP) for the solution " +
			      Prj->name());

	  StochasticsEOPsPC->columnNames()->append(new QString("Time, MJD"));
	  StochasticsEOPsPC->setType(0, SBPlotCarrier::AT_MJD);

	  for (Ps=StochEOPPars.first(); Ps; Ps=StochEOPPars.next())
	    StochasticsEOPsPC->columnNames()->append(new QString(Ps->name()));
	  
	  for (Ps=StochEOPPars.first(); Ps; Ps=StochEOPPars.next())
	    StochasticsEOPsPC->columnNames()->append(new QString("d(" + Ps->name() + ")"));

	  unsigned int	i=0;
	  for (Ps=StochEOPPars.first(); Ps; i++, Ps=StochEOPPars.next())
	    {
	      StochasticsEOPsPC->columnNames()->append(new QString("E[" + Ps->name() + "]"));
	      StochasticsEOPsPC->setStdVarIdx( 1 + i,  2*NumEOPColumns + 1 + i);
	      StochasticsEOPsPC->setStdVarIdx( 1 + NumEOPColumns + i,  2*NumEOPColumns + 1 + i);
	    };
	};

      StochasticsEOPsPC->setFile4SaveBaseName(Prj->name() + "/" + 
					      "[" + Stat->name() + "]" + "_Stoch_EOP");
      
      if (StochasticsEOPsPC->branches()->count())
	StochasticsEOPsPC->branches()->clear();
      
      if (StochEOPPars.count())
	StochasticsEOPsPC->createBranch(StochEOPPars.first()->count(), "EOP");

      if ( (pb=StochasticsEOPsPC->branches()->first()) )
	{
	  unsigned int	i=1;
	  for (Ps = StochEOPPars.first(); Ps; i++, Ps = StochEOPPars.next())
	    {
	      pb->Idx = 0;
	      SBParameter	*P;
	      for (P=Ps->first(); P; P=Ps->next())
		{
		  pb->data()->set(pb->Idx,  0,				P->tEpoch());
		  pb->data()->set(pb->Idx,  i,				P->v()*P->scale());
		  pb->data()->set(pb->Idx,  i +   NumEOPColumns,	P->s()*P->scale());
		  pb->data()->set(pb->Idx,  i + 2*NumEOPColumns,	P->e()*P->scale());
		  pb->Idx++;
		};
	    };
	};
      
      if (StochasticsEOPsPlot)
	StochasticsEOPsPlot->dataChanged();
    };
  QApplication::restoreOverrideCursor();
};

void SBSolutionBrowser::updateCRF()
{
  SBRunManager		*Runner=new SBRunManager(Prj, NULL, Solution);
  SB_CRF		*CRF = SetUp->loadCRF();

  SBSource		*Src = NULL;
  SBSourceInfo		*SI  = NULL;  
  SBParameterList	*LastGlb = NULL;
  SBSymMatrix		*LastP = NULL;
  int			NumOfUpdatedSrcs=0;


  QApplication::setOverrideCursor(Qt::waitCursor);
  Log->write(SBLog::DBG, SBLog::DATA, ClassName() + ": Updating CRF database has been started");

  Solution->loadGlbCor(LastGlb, LastP);
  CRF->prepare4Run();
  
  for (Src = CRF->first(); Src; Src = CRF->next())
    if ((SI = Prj->sourceList()->find(Src)) &&
	!SI->isAttr(SBSourceInfo::notValid) && 
	SI->isAttr(SBSourceInfo::EstCoo))
      {
	// coordinates:
	Src->getGlobalParameters4Report(Runner);
	Src->addAttr(SBSource::OrigEstimated);
	if (LastGlb && LastP)
	  {
	    int		idx_RA=0, idx_DN=0;
	    if ( (idx_RA=LastGlb->idx(Src->p_RA()))!=-1 && (idx_DN=LastGlb->idx(Src->p_DN()))!=-1)
	      Src->setCorrRA_DN(LastP->at(idx_RA, idx_DN)/
				sqrt(LastP->at(idx_RA, idx_RA)*LastP->at(idx_DN, idx_DN)));
	  };
	Src->setRA(Src->ra_t());
	Src->setDN(Src->dn_t());
	NumOfUpdatedSrcs++;
      };

  CRF->finisRun();
  SetUp->saveCRF(CRF);
  delete CRF;


  if (LastGlb)
    delete LastGlb;
  if (LastP)
    delete LastP;

  delete Runner;

  QApplication::restoreOverrideCursor();
  Log->write(SBLog::DBG, SBLog::DATA, ClassName() + ": Updating CRF database has been finished, " + QString("_").setNum(NumOfUpdatedSrcs) +
	     " sources have been updated");
};

void SBSolutionBrowser::updateTRF()
{
  SBRunManager		*Runner=new SBRunManager(Prj, NULL, Solution);
  SB_TRF		*TRF = SetUp->loadTRF();

  SBSite		*Site = NULL;
  SBStation		*Station = NULL;
  SBStationInfo		*SI  = NULL;  
  int			NumOfUpdatedStas=0;

  QApplication::setOverrideCursor(Qt::waitCursor);
  Log->write(SBLog::DBG, SBLog::DATA, ClassName() + ": Updating TRF database has been started");

  TRF->prepare4Run();

  
  for (Site = TRF->first(); Site; Site = TRF->next())
    for (Station = Site->stations().first(); Station; Station = Site->stations().next())
      {

	if ((SI = Prj->stationList()->find(&Station->id())) &&
	    !SI->isAttr(SBStationInfo::notValid) && SI->isAttr(SBStationInfo::EstCoo))
	  {
	    Station->getGlobalParameters4UpdateTRF(Runner);
	    Station->addAttr(SBStation::OrigEstimated);
	    NumOfUpdatedStas++;
	  };
      };

  TRF->finisRun();
  SetUp->saveTRF(TRF);

  delete TRF;
  delete Runner;

  QApplication::restoreOverrideCursor();
  Log->write(SBLog::DBG, SBLog::DATA, ClassName() + ": Updating TRF database has been finished, " + QString("_").setNum(NumOfUpdatedStas) +
	     " stations have been updated");
};
/*==============================================================================================*/


/*==============================================================================================*/
