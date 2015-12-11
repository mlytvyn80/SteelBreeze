/*
 *
 * This file is part of SteelBreeze.
 *
 * SteelBreeze, the geodetic VLBI data analysing software.
 * Copyright (C) 2005 Sergei Bolotin, MAO NASU, Kiev, Ukraine.
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
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qdir.h> 
#include <qfiledialog.h> 
#include <qfileinfo.h> 
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qtabwidget.h>
#include <qspinbox.h>
#include <qvalidator.h>


#include "SbSetup.H"
#include "SbIPlot.H"
#include "SbIFilters.H"





/*==============================================================================================*/
/*												*/
/* SBFilteringGauss..										*/
/*												*/
/*==============================================================================================*/
SBFilteringGauss::SBFilteringGauss(QWidget* parent, const char* name, bool modal, WFlags f)
  : QDialog(parent, name, modal, f)
{
  QBoxLayout *Layout, *SubLayout;

  setCaption("Gauss Filtering of a Time Series");

  Name = "unnamed";
  NInterpolate = 500;
  NumOfCols = 0;
  NumOfDataCols = 0;
  FWHM = 1.0;
  NperTF = 800;
  Step = 0.0;
  Epoch = 0.0;
  IsByNum = TRUE;
  IsFilterSubstracted = FALSE;
  SMode = SIG_MODE_0;
  FType = FILT_GAUSS;

  Filter = new SBFilterGauss;


  Layout = new QVBoxLayout(this, 20, 10);

  // Main Tab:
  Tab = new QTabWidget(this);
  Tab ->addTab(wOptions(), "&Options");
  Tab ->addTab(wBrowseData(),  "Plot &Data");
  Tab ->addTab(wBrowseTF(),  "Plot &Responce Function");
  Layout->addWidget(Tab, 5);


  //---
  SubLayout = new QHBoxLayout(10);
  Layout -> addLayout(SubLayout);

  SubLayout-> addStretch(1);

  GetData = new QPushButton("Get Data", this);
  Analyse = new QPushButton("Analyse", this);
  Ok      = new QPushButton("Close", this);
  Save    = new QPushButton("Output", this);
  Analyse->setDefault(TRUE);

  QSize	BtnSize;
  GetData  ->setMinimumSize((BtnSize=GetData->sizeHint()));
  Analyse  ->setMinimumSize(BtnSize);
  Ok       ->setMinimumSize(BtnSize);
  Save     ->setMinimumSize(BtnSize);

  SubLayout->addWidget(Save);
  SubLayout->addWidget(GetData);
  SubLayout->addWidget(Analyse);
  SubLayout->addWidget(Ok);

  connect(Save,     SIGNAL(clicked()), SLOT(wrote2File()));
  connect(GetData,  SIGNAL(clicked()), SLOT(readDataFile()));
  connect(Analyse,  SIGNAL(clicked()), SLOT(makeAnalysis()));
  connect(Ok,       SIGNAL(clicked()), SLOT(accept()));
  //--

  Layout->activate();

  accel = new QAccel(this);
  accel->connectItem(accel->insertItem(Key_D+ALT), this, SLOT(readDataFile()));
  accel->connectItem(accel->insertItem(Key_A+ALT), this, SLOT(makeAnalysis()));
  accel->connectItem(accel->insertItem(Key_3+ALT), this, SLOT(makeAnalysisWith3Sigma()));
  accel->connectItem(accel->insertItem(Key_S+ALT), this, SLOT(calcSigmas()));
  accel->connectItem(accel->insertItem(Key_M+ALT), this, SLOT(substractFilter()));
};

SBFilteringGauss::~SBFilteringGauss()
{
  if (Filter)
    {
      delete Filter;
      Filter = NULL;
    };
  if (PCData)
    {
      delete PCData;
      PCData = NULL;
    };
  if (PlotData)
    {
      delete PlotData;
      PlotData = NULL;
    };
};

// Widgets:
QWidget* SBFilteringGauss::wOptions()
{
  static const QString sFilterType[]=
  {
    "Gaussian",
    "Gaussian+Model",
  };

  static const QString sSigmasMode[]=
  {
    "Data only",
    "Data then sigmas",
    "Data and sigmas mixed"
  };

  QString	Str;
  QGridLayout	*grid	= NULL;
  QWidget	*W	= new QWidget(this);
  QBoxLayout	*Layout = new QHBoxLayout(W, 20, 10);

  QBoxLayout	*aLayout = new QVBoxLayout();
  Layout->addLayout(aLayout);

  
  // Info:
  QGroupBox* gbox = new QGroupBox("Data Info", W);
  grid = new QGridLayout(gbox, 6,2,   20,5);
  
  // file name:
  QLabel* label = new QLabel("File name:", gbox);
  label -> setMinimumSize(label->sizeHint());
  grid  -> addWidget(label, 0,0);

  lImportedFileName = new QLabel("", gbox);
  lImportedFileName -> setMinimumSize(lImportedFileName->sizeHint());
  lImportedFileName -> setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  grid  -> addWidget(lImportedFileName, 0,1);

  // num of points:
  label = new QLabel("Number of points:", gbox);
  label -> setMinimumSize(label->sizeHint());
  grid  -> addWidget(label, 1,0);

  lNumOfPts = new QLabel(Str.setNum(0), gbox);
  lNumOfPts -> setMinimumSize(lNumOfPts->sizeHint());
  lNumOfPts -> setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  grid  -> addWidget(lNumOfPts, 1,1);

  // num of columns:
  label = new QLabel("Number of columns:", gbox);
  label -> setMinimumSize(label->sizeHint());
  grid  -> addWidget(label, 2,0);

  lNumOfCols = new QLabel(Str.setNum(NumOfCols), gbox);
  lNumOfCols -> setMinimumSize(lNumOfCols->sizeHint());
  lNumOfCols -> setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  grid  -> addWidget(lNumOfCols, 2,1);

  // spacing:
  label = new QLabel("Mean Spacing (dT):", gbox);
  label -> setMinimumSize(label->sizeHint());
  grid  -> addWidget(label, 3,0);

  lDT = new QLabel(Str.sprintf("%.5g", 0.0), gbox);
  lDT -> setMinimumSize(lDT->sizeHint());
  lDT -> setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  grid  -> addWidget(lDT, 3,1);

  // spacing (min:max):
  label = new QLabel("Min:Max Spacing:", gbox);
  label -> setMinimumSize(label->sizeHint());
  grid  -> addWidget(label, 4,0);

  lDTMinMax = new QLabel(Str.sprintf("%.4g:%.4g", 0.0, 0.0), gbox);
  lDTMinMax -> setMinimumSize(lDTMinMax->sizeHint());
  lDTMinMax -> setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  grid  -> addWidget(lDTMinMax, 4,1);

  // interval:
  label = new QLabel("Interval:", gbox);
  label -> setMinimumSize(label->sizeHint());
  grid  -> addWidget(label, 5,0);

  lInterval = new QLabel(Str.sprintf("%.4g", 0.0), gbox);
  lInterval -> setMinimumSize(lInterval->sizeHint());
  lInterval -> setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  grid  -> addWidget(lInterval, 5,1);

  grid->activate();
  aLayout->addWidget(gbox);


  QButtonGroup	*butgrp = new QButtonGroup("Filter Type", W);
  QBoxLayout	*Layout_2  = new QVBoxLayout(butgrp, 20, 3);
  for (int i=0; i<2; i++)
    {
      rbFilterType[i] = new QRadioButton(sFilterType[i], butgrp);
      rbFilterType[i]-> setMinimumSize(rbFilterType[i]->sizeHint());
      Layout_2 -> addWidget(rbFilterType[i]);
    };
  rbFilterType[0]->setChecked(TRUE);
  connect(butgrp, SIGNAL(clicked(int)), SLOT(setFilterType(int)));
  Layout_2->activate();
  aLayout->addWidget(butgrp);

  
  // Options:
  butgrp	= new QButtonGroup("Options", W);
  Layout_2	= new QVBoxLayout(butgrp, 20, 3);

  for (int i=0; i<3; i++)
    {
      rbSigmasMode[i] = new QRadioButton(sSigmasMode[i], butgrp);
      rbSigmasMode[i]-> setMinimumSize(rbSigmasMode[i]->sizeHint());
      Layout_2 -> addWidget(rbSigmasMode[i]);
    };
  rbSigmasMode[SMode]->setChecked(TRUE);
  connect(butgrp, SIGNAL(clicked(int)), SLOT(setSigmasMode(int)));

  Layout_2->activate();
  aLayout->addWidget(butgrp);


  // Controls:
  butgrp= new QButtonGroup("Controls", W);
  grid  = new QGridLayout(butgrp, 5,2,   20,5);

  // num of points for the interpolation:
  QRadioButton		*rbByWhat = new QRadioButton("Number of points in interpolation:", butgrp);
  rbByWhat-> setMinimumSize(rbByWhat->sizeHint());
  grid->addWidget(rbByWhat, 0,0);
  rbByWhat->setChecked(TRUE);

  leNInterpolate = new QLineEdit(butgrp);
  leNInterpolate -> setText(Str.setNum(NInterpolate));
  leNInterpolate -> setMinimumHeight(leNInterpolate->sizeHint().height());
  leNInterpolate -> setMinimumWidth (leNInterpolate->fontMetrics().width("1000000") + 10);
  QIntValidator *iVal = new QIntValidator(leNInterpolate);
  iVal->setBottom(3);
  leNInterpolate -> setValidator(iVal);
  connect(leNInterpolate, SIGNAL(textChanged(const QString&)), SLOT(changeNInterpolate(const QString&)));
  grid  -> addWidget(leNInterpolate, 0,1);

  // or step for the interpolation:
  rbByWhat = new QRadioButton("Step in interpolation:", butgrp);
  rbByWhat-> setMinimumSize(rbByWhat->sizeHint());
  grid->addWidget(rbByWhat, 1,0);

  leStep = new QLineEdit(butgrp);
  leStep -> setText(Str.sprintf("%.6f", Step));
  leStep -> setMinimumHeight(leStep->sizeHint().height());
  leStep -> setMinimumWidth (leStep->fontMetrics().width("1000000") + 10);
  QDoubleValidator *dVal = new QDoubleValidator(leStep);
  dVal->setBottom(0.0000000001);
  leStep -> setValidator(dVal);
  connect(leStep, SIGNAL(textChanged(const QString&)), SLOT(changeStep(const QString&)));
  grid  -> addWidget(leStep, 1,1);


  label = new QLabel("FWHM:", butgrp);
  label -> setMinimumSize(label->sizeHint());
  grid  -> addWidget(label, 2,0);

  leFWHM = new QLineEdit(butgrp);
  leFWHM -> setText(Str.sprintf("%.4g", FWHM));
  leFWHM -> setMinimumHeight(leFWHM->sizeHint().height());
  //  leFStart -> setMinimumWidth (leFStart->fontMetrics().width("-99999.99999") + 10);
  connect(leFWHM, SIGNAL(textChanged(const QString&)), SLOT(changeFWHM(const QString&)));
  leFWHM -> setValidator(new QDoubleValidator(leFWHM));
  grid  -> addWidget(leFWHM, 2,1);

  label = new QLabel("Epoch:", butgrp);
  label -> setMinimumSize(label->sizeHint());
  grid  -> addWidget(label, 3,0);

  leEpoch = new QLineEdit(butgrp);
  leEpoch -> setText(Str.sprintf("%.4g", Epoch));
  leEpoch -> setMinimumHeight(leEpoch->sizeHint().height());
  //  leFStart -> setMinimumWidth (leFStart->fontMetrics().width("-99999.99999") + 10);
  connect(leEpoch, SIGNAL(textChanged(const QString&)), SLOT(changeEpoch(const QString&)));
  leEpoch -> setValidator(new QDoubleValidator(leEpoch));
  grid  -> addWidget(leEpoch, 3,1);

  grid->setRowStretch(4,  1);

  // ...
  grid->activate();
  Layout->addWidget(butgrp);
  connect(butgrp, SIGNAL(clicked(int)), SLOT(changeByNum(int)));

  Layout->activate();
  return W;
};

QWidget* SBFilteringGauss::wBrowseData()
{
  QWidget	*W	= new QWidget(this);
  QBoxLayout	*Layout = new QHBoxLayout(W, 20, 10);

  PCData = new SBPlotCarrier(2, 1, "Time Series");
  PCData->setAbsFile4SaveBaseName(SetUp->path2TimeSeries() + Name + ".data");

  PCData->setStdVarIdx(1, 2);
  PCData->createBranch(1, "tseries");

  PCData->branches()->at(0)->data()->set(0, 0, 0.0);
  PCData->branches()->at(0)->data()->set(0, 1, 0.0);
  PCData->branches()->at(0)->data()->set(0, 2, 0.0);
  PCData->branches()->at(0)->setDataAttr(0, SBPlotCarrier::DA_IGN);

  PCData->columnNames()->append(new QString("Argument"));
  PCData->columnNames()->append(new QString("Data"));
  PCData->columnNames()->append(new QString("Std.Vars"));

  PlotData = new SBPlot(PCData, W);
  Layout->addWidget(PlotData);
  Layout->activate();
  return W;
};

QWidget* SBFilteringGauss::wBrowseTF()
{
  QWidget	*W	= new QWidget(this);
  QBoxLayout	*Layout = new QHBoxLayout(W, 20, 10);

  PCTF = new SBPlotCarrier(2, 0, "Transfer function");
  PCTF->setAbsFile4SaveBaseName(SetUp->path2TimeSeries() + Name + ".tr_func");
  PCTF->columnNames()->append(new QString("Periods"));
  PCTF->columnNames()->append(new QString("Transfer function"));

  PCTF->createBranch(NperTF, "Transfer function");
  
  SBPlotBranch	*pb= PCTF->branches()->at(0);
  SBMatrix	*B = pb->data();
  for (unsigned int i=0; i<NperTF; i++)
    {
      B ->set(i,0, 0.0);
      B ->set(i,1, 0.0);
      pb->setDataAttr(i, SBPlotCarrier::DA_IGN);
    };
  
  PlotTF = new SBPlot(PCTF, W,
		      SBPlot::PM_HAS_HAVE_ZERO   | SBPlot::PM_WO_DOTS |
		      SBPlot::PM_WO_BRANCH_NAMES | SBPlot::PM_LINES   );
  Layout->addWidget(PlotTF);
  Layout->activate();
  return W;
};


// slots/actions:
void SBFilteringGauss::accept()
{
  QDialog::accept();
  delete this;
};

void SBFilteringGauss::reject() 
{
  QDialog::reject();
  delete this;
};

void SBFilteringGauss::wrote2File()
{
  QString		Str, FileName;
  unsigned int		i,j;
  
  if (Filter->isOK())
    {
      FileName = SetUp->path2TimeSeries() + Name + ".flt";

      QFile f(FileName);
      f.open(IO_WriteOnly);
      QTextStream	s(&f);

      for (i=0; i<NInterpolate; i++)
	{
	  Str.sprintf("%16.8f  ", PCData->branches()->at(NumOfDataCols)->data()->at(i, 0));
	  s << Str;
	  for (j=0; j<NumOfDataCols; j++)
	    {
	      Str.sprintf("%16.8g ", PCData->branches()->at(NumOfDataCols+j)->data()->at(i, 1));
	      s << Str;
	    };
	  s << "\n";
	};
      
      f.close();
      s.unsetDevice();
      Log->write(SBLog::DBG, SBLog::DATA, 
		 ClassName() + ": the filtered data have been saved in the file [" + FileName + "]");

      if (IsFilterSubstracted)
	{
	  FileName = SetUp->path2TimeSeries() + Name + ".flt.sub";
	  f.setName(FileName);
	  f.open(IO_WriteOnly);
	  s.setDevice(&f);
	  SBDataSeries		*series = Filter->series();
	  SBDataSeriesEntry	*E=NULL;

	  i=0;
	  for (E=series->first(); E; i++, E=series->next())
	    {
	      Str.sprintf("%16.8f  ", E->t());
	      s << Str;
	      for (j=0; j<NumOfCols; j++)
		{
		  Str.sprintf("%16.8f ", E->data()->at(j));
		  s << Str;
		};
	      s << "\n";
	    };
	  f.close();
	  s.unsetDevice();
	  Log->write(SBLog::DBG, SBLog::DATA, 
		     ClassName() + ": the data-filter series has been saved in the file [" + FileName + "]");
	};
    };
};

void SBFilteringGauss::readDataFile()
{
  QString	Str;
  QString       FName(QFileDialog::getOpenFileName(SetUp->path2TimeSeries(), QString::null,
						   this, "Select a file with time series"));
  if (!FName.isEmpty())
    {
      Log->write(SBLog::DBG, SBLog::DATA, ClassName() + ": reading the file [" + FName + "]");
      Filter->readDataFile(FName);
    };
  
  if (Filter->isOK())
    {
      IsFilterSubstracted = FALSE;
      QFileInfo fiName(FName);

      Name = fiName.baseName() + "." + fiName.extension();
      setCaption("Gauss Filtering of a Time Series [" + Name + "]");

      NumOfCols = Filter->series()->numOfDataColumns();

      if (IsByNum)
	Step = (Filter->tFinis() - Filter->tStart())/(NInterpolate-1);
      else
	NInterpolate = (int)floor((Filter->tFinis() - Filter->tStart())/Step) + 1;
      
      if (SMode!=SIG_MODE_0)
	{
	  //check
	  if (!(NumOfCols%2))
	    NumOfDataCols = NumOfCols/2;
	  else
	    {
	      SMode = SIG_MODE_0;
	      NumOfDataCols = NumOfCols;
	      rbSigmasMode[SMode]->setChecked(TRUE);
	      Log->write(SBLog::INF, SBLog::DATA, ClassName() + ": the file [" + FName + 
			 "] contains even data columns (" + Str.setNum(NumOfCols) +
			 "), cannot assign sigmas col");
	    };
	}
      else
	NumOfDataCols = NumOfCols;
    }
  else
    {
      IsByNum = TRUE;
      Step = 0.0;
      Name = "";
      lNumOfPts	-> setText("0");
      Log->write(SBLog::INF, SBLog::DATA, ClassName() + ": the file [" + 
	  	 Name + "] does not contain usefull data");
    };
  lImportedFileName->setText(Name);
  lNumOfPts->setText(Str.setNum(Filter->n()));
  lNumOfCols->setText(Str.setNum(NumOfCols));
  lDT->setText(Str.sprintf("%.5g", Filter->series()->dT()));
  lDTMinMax->setText(Str.sprintf("%.4g:%.4g", Filter->series()->dTMin(), Filter->series()->dTMax()));
  lInterval->setText(Str.sprintf("%.4g", Filter->tFinis() - Filter->tStart()));
  leStep->setText(Str.sprintf("%.6f", Step));
  leNInterpolate->setText(Str.setNum(NInterpolate));
  redrawDataPlot();
  redrawTFPlot();
};

void SBFilteringGauss::collectData()
{

};

void SBFilteringGauss::makeAnalysis()
{
  if (FType==FILT_GAUSS_APRIORY)
    redrawDataPlot_ExpMode();
  else
    redrawDataPlot();
};

void SBFilteringGauss::substractFilter()
{
  if (Filter->isOK())
    {
      QApplication::setOverrideCursor(Qt::waitCursor);
      qApp->processEvents();

      Filter->substractFilter(FType==FILT_GAUSS_APRIORY, SMode);
      makeAnalysis();

      QApplication::restoreOverrideCursor();
      qApp->processEvents();
      IsFilterSubstracted = TRUE;
    };
};

void SBFilteringGauss::makeAnalysisWith3Sigma()
{
  double	f = 1.0;
  if (Filter->isOK())
    {
      for (unsigned int j=0; j<NumOfDataCols; j++)
	{
	  switch (SMode)
	    {
	    case SIG_MODE_0: f *= Filter->sigma(  j);   break;
	    case SIG_MODE_1: f *= Filter->sigma(  j);   break;
	    case SIG_MODE_2: f *= Filter->sigma(2*j);   break;
	    };
	};
      if (f>=0.0)
	{
	  Filter->addAttr(SBFilterGauss::IsOmit3Sigma);
	  redrawDataPlot();
	  Filter->delAttr(SBFilterGauss::IsOmit3Sigma);
	}
      else
	Log->write(SBLog::INF, SBLog::DATA, ClassName() + 
		   ": cannot calculate interpolation with 3Sigma limits; sigmas are zeros" );
    };  
};

void SBFilteringGauss::changeNInterpolate(const QString& Str)
{
  int				d;
  bool				isOK;
  d = Str.toInt(&isOK);
  if (isOK)
    NInterpolate = d;
};

void SBFilteringGauss::changeStep(const QString& Str)
{
  double			f;
  bool				isOK;
  f = Str.toDouble(&isOK);
  if (isOK) 
    Step = f;
};

void SBFilteringGauss::changeEpoch(const QString& Str)
{
  double			f;
  bool				isOK;
  f = Str.toDouble(&isOK);
  if (isOK) 
    Epoch = f;
};

void SBFilteringGauss::changeFWHM(const QString& Str)
{
  double			f;
  bool				isOK;
  f = Str.toDouble(&isOK);
  if (isOK)
    {
      FWHM = f;
      if (Filter)
	{
	  Filter->setFWHM(FWHM);
	  redrawTFPlot();
	};
    };
};

void SBFilteringGauss::setSigmasMode(int id)
{
  if (0<=id && id<3)
    SMode = (SIGMASMODE)id;
};

void SBFilteringGauss::setFilterType(int id)
{
  if (0<=id && id<3)
    FType = (FILTERTYPE)id;
};

void SBFilteringGauss::changeByNum(int id)
{
  if (id==0)
    {
      IsByNum = TRUE;
      if (Filter->isOK())
	{
	  Step = (Filter->tFinis() - Filter->tStart())/(NInterpolate - 1);
	  leStep->setText(QString("").sprintf("%.6f", Step));
	};
    }
  else if (id==1)
    {
      IsByNum = FALSE;
      if (Filter->isOK())
	{
	  if (Step==0.0)
	    Step = 10.0;
	  NInterpolate = (int)floor((Filter->tFinis() - Filter->tStart())/Step) + 1;
	  leNInterpolate->setText(QString("").setNum(NInterpolate));
	};
    };
};

void SBFilteringGauss::redrawDataPlot()
{
  SBPlotBranch		*pb = NULL;
  SBMatrix		*B=NULL;
  QString		Str;
  double		t;

  PCData->branches()->clear();
  
  if (Filter->isOK())
    {
      PCData->setAbsFile4SaveBaseName(SetUp->path2TimeSeries() + Name + ".data");

      // data:
      for (unsigned int j=0; j<NumOfDataCols; j++)
	{
	  PCData->createBranch(Filter->n(), Str.sprintf("data column #%d", j));
	  pb = PCData->branches()->at(j);
	  B  = pb->data();
	  for (unsigned int i=0; i<Filter->n(); i++)
	    {
	      B->set(i, 0, Filter->series()->at(i)->t());
	      switch (SMode)
		{
		case SIG_MODE_0: 
		  B->set(i, 1, Filter->series()->at(i)->data()->at(j)); 
		  B->set(i, 2, 0.0);
		  break;
		case SIG_MODE_1: 
		  B->set(i, 1, Filter->series()->at(i)->data()->at(j)); 
		  B->set(i, 2, Filter->series()->at(i)->data()->at(NumOfDataCols+j)); 
		  break;
		case SIG_MODE_2: 
		  B->set(i, 1, Filter->series()->at(i)->data()->at(2*j)); 
		  B->set(i, 2, Filter->series()->at(i)->data()->at(2*j+1)); 
		  break;
		};
	    };
	};

      // filtering:
      for (unsigned int j=0; j<NumOfDataCols; j++)
	{
	  if (IsByNum)
	    {
	      Step = (Filter->tFinis() - Filter->tStart())/(NInterpolate - 1);
	      leStep->setText(QString("").sprintf("%.6f", Step));
	    }
	  else
	    {
	      NInterpolate = (int)floor((Filter->tFinis() - Filter->tStart())/Step) + 1;
	      leNInterpolate->setText(QString("").setNum(NInterpolate));
	    };
	  if (Epoch!=0.0)
	    {
	      t  = Epoch + floor((Filter->tFinis()-Epoch)/Step)*Step - (NInterpolate - 1)*Step;
	      if (t<Filter->tFinis()-Filter->series()->dT())
		{
		  t+=Step;
		  NInterpolate--;
		};
	    }
	  else
	    t  = floor(Filter->tFinis()) - (NInterpolate - 1)*Step;

	  PCData->createBranch(NInterpolate, Str.sprintf("filter column #%d", j));
	  pb = PCData->branches()->at(NumOfDataCols+j);
	  B  = pb->data();
	  for (unsigned int i=0; i<NInterpolate; i++)
	    {
	      B->set(i, 0, t);
	      switch (SMode)
		{
		case SIG_MODE_0: 
		  B->set(i, 1, Filter->interpolate(t, j, 0));
		  B->set(i, 2, Filter->stdVar());
		  break;
		case SIG_MODE_1: 
		  B->set(i, 1, Filter->interpolate(t, j, NumOfDataCols+j)); 
		  B->set(i, 2, Filter->stdVar());
		  break;
		case SIG_MODE_2: 
		  B->set(i, 1, Filter->interpolate(t, 2*j, 2*j+1));
		  B->set(i, 2, Filter->stdVar());
		  break;
		};
	      t += Step;
	    };
	};
    }
  else // clear the plot
    {
      PCData->createBranch(1, "tseries");
      PCData->branches()->at(0)->data()->set(0, 0, 0.0);
      PCData->branches()->at(0)->data()->set(0, 1, 0.0);
      PCData->branches()->at(0)->data()->set(0, 2, 0.0);
      PCData->branches()->at(0)->setDataAttr(0, SBPlotCarrier::DA_IGN);
    };
  PlotData->dataChanged();
};

void SBFilteringGauss::redrawTFPlot()
{
  SBPlotBranch	*pb= PCTF->branches()->at(0);
  SBMatrix	*B = pb->data();

  if (Filter->isOK())
    {
      double P = (Filter->tFinis()-Filter->tStart())/2.0;
      double p = Filter->series()->dT();
      for (unsigned int i=0; i<NperTF; i++)
	{
	  B ->set(i,0, p);
	  B ->set(i,1, Filter->respFunc(1/p));
	  pb->setDataAttr(i, 0);
	  p+= P/(NperTF-1);
	};
    }
  else
    for (unsigned int i=0; i<NperTF; i++)
      {
	B ->set(i,0, 0.0);
	B ->set(i,1, 0.0);
	pb->setDataAttr(i, SBPlotCarrier::DA_IGN);
      };
  PlotTF->dataChanged();
};

void SBFilteringGauss::calcSigmas()
{
  if (Filter->isOK())
    {
      for (unsigned int j=0; j<NumOfDataCols; j++)
	{
	  switch (SMode)
	    {
	    case SIG_MODE_0: 
	      Filter->calcSigma(j, 0);
	      Log->write(SBLog::INF, SBLog::DATA, ClassName() + ": data column #" + QString("").setNum(j) +
			 ": sigma: " + QString("").sprintf("%.14g", Filter->sigma(j)));
	      break;
	    case SIG_MODE_1: 
	      Filter->calcSigma(j, NumOfDataCols+j);
	      Log->write(SBLog::INF, SBLog::DATA, ClassName() + ": data column #" + QString("").setNum(j) +
			 ": sigma: " + QString("").sprintf("%.14g", Filter->sigma(j)));
	      break;
	    case SIG_MODE_2: 
	      Filter->calcSigma(2*j, 2*j+1);
	      Log->write(SBLog::INF, SBLog::DATA, ClassName() + ": data column #" + QString("").setNum(j) +
			 ": sigma: " + QString("").sprintf("%.14g", Filter->sigma(2*j)));
	      break;
	    };
	};
    };  
};

void SBFilteringGauss::redrawDataPlot_ExpMode()
{
  SBPlotBranch		*pb = NULL;
  SBMatrix		*B=NULL;
  QString		Str;
  SBNamed		nm;
  double		t, T0;

  PCData->branches()->clear();
  if (Filter->isOK())
    {
      PCData->setAbsFile4SaveBaseName(SetUp->path2TimeSeries() + Name + ".data");

      if (!Filter->modelList()->count())
	for (unsigned int j=0; j<NumOfDataCols; j++)
	  switch (SMode)
	    {
	    case SIG_MODE_0: 
	      Filter->addModel(j, 0);
	      break;
	    case SIG_MODE_1: 
	      Filter->addModel(j, NumOfDataCols+j);
	      break;
	    case SIG_MODE_2: 
	      Filter->addModel(2*j, 2*j+1);
	      break;
	    };
      Filter->prepareModels();
      
      // data:
      for (unsigned int j=0; j<NumOfDataCols; j++)
	{
	  PCData->createBranch(Filter->n(), Str.sprintf("data col.#%d", j));
	  nm.setName(Str);
	  pb = PCData->branches()->find(&nm);
	  //	  std::cout << "Got " << pb->name() << "\n";
	  B  = pb->data();
	  for (unsigned int i=0; i<Filter->n(); i++)
	    {
	      B->set(i, 0, Filter->seriesMinusModel()->at(i)->t());
	      switch (SMode)
		{
		case SIG_MODE_0: 
		  B->set(i, 1, Filter->seriesMinusModel()->at(i)->data()->at(j)); 
		  B->set(i, 2, 0.0);
		  break;
		case SIG_MODE_1: 
		  B->set(i, 1, Filter->seriesMinusModel()->at(i)->data()->at(j)); 
		  B->set(i, 2, Filter->seriesMinusModel()->at(i)->data()->at(NumOfDataCols+j)); 
		  break;
		case SIG_MODE_2: 
		  B->set(i, 1, Filter->seriesMinusModel()->at(i)->data()->at(2*j)); 
		  B->set(i, 2, Filter->seriesMinusModel()->at(i)->data()->at(2*j+1)); 
		  break;
		};
	    };
	};

      // filtering:
      for (unsigned int j=0; j<NumOfDataCols; j++)
	{
	  if (IsByNum)
	    {
	      Step = (Filter->tFinis() - Filter->tStart())/(NInterpolate - 1);
	      leStep->setText(QString("").sprintf("%.6f", Step));
	    }
	  else
	    {
	      NInterpolate = (int)floor((Filter->tFinis() - Filter->tStart())/Step) + 1;
	      leNInterpolate->setText(QString("").setNum(NInterpolate));
	    };
	  if (Epoch!=0.0)
	    {
	      t  = Epoch + floor((Filter->tFinis()-Epoch)/Step)*Step - (NInterpolate - 1)*Step;
	      if (t<Filter->tFinis()-Filter->seriesMinusModel()->dT())
		{
		  t+=Step;
		  NInterpolate--;
		};
	    }
	  else
	    t  = floor(Filter->tFinis()) - (NInterpolate - 1)*Step;
	  T0=t;

	  PCData->createBranch(NInterpolate, Str.sprintf("filter col.#%d", j));
	  nm.setName(Str);
	  pb = PCData->branches()->find(&nm);
	  B  = pb->data();
	  for (unsigned int i=0; i<NInterpolate; i++)
	    {
	      B->set(i, 0, t);
	      switch (SMode)
		{
		case SIG_MODE_0: 
		  B->set(i, 1, Filter->interpolateMinusModel(t, j, 0));
		  B->set(i, 2, Filter->stdVar());
		  break;
		case SIG_MODE_1: 
		  B->set(i, 1, Filter->interpolateMinusModel(t, j, NumOfDataCols+j)); 
		  B->set(i, 2, Filter->stdVar());
		  break;
		case SIG_MODE_2: 
		  B->set(i, 1, Filter->interpolateMinusModel(t, 2*j, 2*j+1));
		  B->set(i, 2, Filter->stdVar());
		  break;
		};
	      t += Step;
	    };

	  // Models:
	  SBFilterModel		*model=NULL;
	  if (SMode==SIG_MODE_2)
	    model=Filter->modelByIdx()->find(2*j);
	  else
	    model=Filter->modelByIdx()->find(j);
	  if (model)
	    {
	      // Full Model:
	      t  = T0;
	      PCData->createBranch(NInterpolate, Str.sprintf("full model col.#%d", j));
	      nm.setName(Str);
	      pb = PCData->branches()->find(&nm);
	      B  = pb->data();
	      for (unsigned int i=0; i<NInterpolate; i++)
		{
		  B->set(i, 0, t);
		  B->set(i, 1, model->modelFull(t));
		  B->set(i, 2, 0.0);
		  t += Step;
		};
	      //
	      // Retained model:
	      PCData->createBranch(NInterpolate, Str.sprintf("retained model col.#%d", j));
	      nm.setName(Str);
	      pb = PCData->branches()->find(&nm);
	      B  = pb->data();
	      t  = T0;
	      for (unsigned int i=0; i<NInterpolate; i++)
		{
		  B->set(i, 0, t);
		  B->set(i, 1, model->modelKeep(t));
		  B->set(i, 2, 0.0);
		  t += Step;
		};
	    }
	  else
	    Log->write(SBLog::ERR, SBLog::DATA, ClassName() + ": cannot find model for column #" + 
		       Str.setNum(j));
	  //


	};
    }
  else // clear the plot
    {
      PCData->createBranch(1, "tseries");
      PCData->branches()->at(0)->data()->set(0, 0, 0.0);
      PCData->branches()->at(0)->data()->set(0, 1, 0.0);
      PCData->branches()->at(0)->data()->set(0, 2, 0.0);
      PCData->branches()->at(0)->setDataAttr(0, SBPlotCarrier::DA_IGN);
    };
  PlotData->dataChanged();
};
/*==============================================================================================*/


/*==============================================================================================*/
