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
#include "SbIMEM.H"





/*==============================================================================================*/
/*												*/
/* SBMEMECoeffLI..										*/
/*												*/
/*												*/
/*==============================================================================================*/
QString SBMEMECoeffLI::text(int col) const
{
  TmpString = "NULL";
  switch (col)
    {
    case  0: TmpString.sprintf("%4d ", Idx);
      break;
    case  1: TmpString.sprintf("%.4f ", C.real());
      break;
    case  2: TmpString.sprintf("%.4f ", C.imag());
      break;
    };
  return TmpString;
};

QString SBMEMECoeffLI::key(int col, bool) const
{
  TmpString = "NULL";
  switch (col)
    {
    case  0: TmpString.sprintf("%09d ", Idx);
      break;
    case  1: TmpString.sprintf("%020.16f ", C.real());
      break;
    case  2: TmpString.sprintf("%020.16f ", C.imag());
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
SBSpectrumAnalyserMEM::SBSpectrumAnalyserMEM(QWidget* parent, const char* name, bool modal, WFlags f)
  : QDialog(parent, name, modal, f)
{
  QBoxLayout *Layout, *SubLayout;

  setCaption("Time Series Spectrum Analyser");


  MESA = new SBMEM;
  MEMType = SBMEM::Type_Recursion;
  MESA->setType(MEMType);

  M    = 2;
  Mmin = 2;
  Mmax = 2;
  FPE_Mstart = Mmin;
  FPE_Mfinis = Mmax;
  Name = "unnamed";
  IsAutoCalc = TRUE;
  IsScanning = FALSE;
  IsScanStop = FALSE;
  NSp  = 1000;
  MinFPEOrder = 0;
  MinFPE = 0.0;

  IsPlotFreq = TRUE;
  FStart = 0.0;
  FFinis = 0.0;
  PStart = 0.0;
  PFinis = 0.0;
  FLine  = 0.0;

  ScaleCPD2CPY = FALSE;
  Str = "";

  Layout = new QVBoxLayout(this, 20, 10);

  // Main Tab:
  Tab = new QTabWidget(this);
  Tab ->addTab(wOptions(), "&Options");
  Tab ->addTab(wBrowseData(),  "Plot &Data");
  Tab ->addTab(wBrowseSpectrum(),  "Plot &Spectrum");
  Tab ->addTab(wMEMCoeffList(),  "Plot FPE");
  Layout->addWidget(Tab, 5);


  //---
  SubLayout = new QHBoxLayout(10);
  Layout -> addLayout(SubLayout);

  sbMval = new QSpinBox(this);
  sbMval->setMinValue(Mmin);
  sbMval->setMaxValue(10000000);
  sbMval->setValue   (10000000);
  sbMval->setMinimumSize(sbMval->sizeHint());
  sbMval->setMaxValue(Mmax);
  sbMval->setValue(M);
  SubLayout->addWidget(sbMval);
  connect(sbMval, SIGNAL(valueChanged(int)), SLOT(changeFilterOrder(int)));

  SubLayout-> addStretch(1);

  GetData = new QPushButton("Get Data", this);
  Analyse = new QPushButton("Analyse", this);
  Ok      = new QPushButton("Close", this);
  Scan    = new QPushButton("Scan", this);
  ResetLimits = new QPushButton("Reset Limits", this);
  Analyse->setDefault(TRUE);

  QSize	BtnSize;
  GetData  ->setMinimumSize((BtnSize=GetData->sizeHint()));
  Analyse  ->setMinimumSize(BtnSize);
  Ok       ->setMinimumSize(BtnSize);
  Scan     ->setMinimumSize(BtnSize);
  ResetLimits->setMinimumSize(BtnSize);

  SubLayout->addWidget(ResetLimits);
  SubLayout->addWidget(GetData);
  SubLayout->addWidget(Analyse);
  SubLayout->addWidget(Scan);
  SubLayout->addWidget(Ok);

  connect(ResetLimits,  SIGNAL(clicked()), SLOT(resetLimits()));
  connect(GetData,  SIGNAL(clicked()), SLOT(readDataFile()));
  connect(Analyse,  SIGNAL(clicked()), SLOT(makeAnalysis()));
  connect(Ok,       SIGNAL(clicked()), SLOT(accept()));
  connect(Scan,     SIGNAL(clicked()), SLOT(scan4FPE()));
  //--

  //  connect(this, SIGNAL(run(bool)),			MainWin,SLOT(setRunning(bool)));
  //  connect(this, SIGNAL(stBarReset()),			MainWin,SLOT(stBarReset()));
  //  connect(this, SIGNAL(stBarSetTotalSteps(int)),	MainWin,SLOT(stBarSetTotalSteps(int)));
  //  connect(this, SIGNAL(stBarSetProgress(int)),		MainWin,SLOT(stBarSetProgress(int)));
  //  connect(this, SIGNAL(message(const QString&)),	MainWin,SLOT(setMessage(const QString&)));

  Layout->activate();

  accel = new QAccel(this);
  accel->connectItem(accel->insertItem(Key_D+ALT), this, SLOT(readDataFile()));
  accel->connectItem(accel->insertItem(Key_A+ALT), this, SLOT(makeAnalysis()));
  accel->connectItem(accel->insertItem(Key_L+ALT), this, SLOT(checkLine()));
};

SBSpectrumAnalyserMEM::~SBSpectrumAnalyserMEM()
{
  if (MESA)
    {
      delete MESA;
      MESA = NULL;
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
  if (PCSpectrum)
    {
      delete PCSpectrum;
      PCSpectrum = NULL;
    };
  if (PlotSpectrum)
    {
      delete PlotSpectrum;
      PlotSpectrum = NULL;
    };
  if (PCFPE)
    {
      delete PCFPE;
      PCFPE = NULL;
    };
  if (PlotFPE)
    {
      delete PlotFPE;
      PlotFPE = NULL;
    };
};

// Widgets:
QWidget* SBSpectrumAnalyserMEM::wOptions()
{
  static const QString sMEMTypes[]=
  {
    "Recursion",
    "Least Squares Forward",
    "Least Squares Backward",
    "Least Squares Forward/Backward"
  };

  QGridLayout	*grid	= NULL;
  QWidget	*W	= new QWidget(this);
  QBoxLayout	*Layout = new QHBoxLayout(W, 20, 10);

  QBoxLayout	*aLayout = new QVBoxLayout();
  Layout->addLayout(aLayout);

  
  // Info:
  QGroupBox* gbox = new QGroupBox("Data Info", W);
  grid = new QGridLayout(gbox, 8,2,   20,5);
  
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

  lNumOfPts = new QLabel(Str.setNum(MESA->n()), gbox);
  lNumOfPts -> setMinimumSize(lNumOfPts->sizeHint());
  lNumOfPts -> setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  grid  -> addWidget(lNumOfPts, 1,1);

  // interval:
  label = new QLabel("Spacing (dT):", gbox);
  label -> setMinimumSize(label->sizeHint());
  grid  -> addWidget(label, 2,0);

  lDT = new QLabel(Str.sprintf("%.4g", MESA->dt()), gbox);
  lDT -> setMinimumSize(lDT->sizeHint());
  lDT -> setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  grid  -> addWidget(lDT, 2,1);

  // shift (R):
  label = new QLabel("Shift (R):", gbox);
  label -> setMinimumSize(label->sizeHint());
  grid  -> addWidget(label, 3,0);

  lShiftR = new QLabel(Str.sprintf("%.4g", MESA->shiftR()), gbox);
  lShiftR -> setMinimumSize(lShiftR->sizeHint());
  lShiftR -> setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  grid  -> addWidget(lShiftR, 3,1);

  // trend (R):
  label = new QLabel("Trend (R):", gbox);
  label -> setMinimumSize(label->sizeHint());
  grid  -> addWidget(label, 4,0);

  lTrendR = new QLabel(Str.sprintf("%.4g", MESA->trendR()), gbox);
  lTrendR -> setMinimumSize(lTrendR->sizeHint());
  lTrendR -> setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  grid  -> addWidget(lTrendR, 4,1);

  // shift (I):
  label = new QLabel("Shift (I):", gbox);
  label -> setMinimumSize(label->sizeHint());
  grid  -> addWidget(label, 5,0);

  lShiftI = new QLabel(Str.sprintf("%.4g", MESA->shiftI()), gbox);
  lShiftI -> setMinimumSize(lShiftI->sizeHint());
  lShiftI -> setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  grid  -> addWidget(lShiftI, 5,1);

  // trend (I):
  label = new QLabel("Trend (I):", gbox);
  label -> setMinimumSize(label->sizeHint());
  grid  -> addWidget(label, 6,0);

  lTrendI = new QLabel(Str.sprintf("%.4g", MESA->trendI()), gbox);
  lTrendI -> setMinimumSize(lTrendI->sizeHint());
  lTrendI -> setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  grid  -> addWidget(lTrendI, 6,1);

  // trend (I):
  label = new QLabel("Sample Variance:", gbox);
  label -> setMinimumSize(label->sizeHint());
  grid  -> addWidget(label, 7,0);

  lSampleVar = new QLabel(Str.sprintf("%.6g", MESA->sampleVar()), gbox);
  lSampleVar -> setMinimumSize(lSampleVar->sizeHint());
  lSampleVar -> setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  grid  -> addWidget(lSampleVar, 7,1);



  grid->activate();
  aLayout->addWidget(gbox);


  // MEM Types:
  QButtonGroup	*butgrp = new QButtonGroup("MEM Type", W);
  QBoxLayout	*Layout_2  = new QVBoxLayout(butgrp, 20, 3);
  for (int i=0; i<4; i++)
    {
      rbMEMType[i] = new QRadioButton(sMEMTypes[i], butgrp);
      rbMEMType[i]-> setMinimumSize(rbMEMType[i]->sizeHint());
      Layout_2 -> addWidget(rbMEMType[i]);
    };
  rbMEMType[(int)MESA->type()]->setChecked(TRUE);
  connect(butgrp, SIGNAL(clicked(int)), SLOT(setMEMType(int)));
  Layout_2->activate();
  aLayout->addWidget(butgrp);

  
  // Options:
  gbox		= new QGroupBox("Options", W);
  Layout_2	= new QVBoxLayout(gbox, 20, 3);

  //  Layout_2->addSpacing(10);
  cbAssumeEquiDistant = new QCheckBox("Assume data are equidistant", gbox);
  cbAssumeEquiDistant->setMinimumSize(cbAssumeEquiDistant->sizeHint());
  cbAssumeEquiDistant->setChecked(FALSE);
  connect(cbAssumeEquiDistant,	SIGNAL(toggled(bool)), SLOT(setMEMAssumeEquiDistant(bool)));
  Layout_2->addWidget(cbAssumeEquiDistant);

  cbFillMissingImag = new QCheckBox("Fill missed Imag part with zeros", gbox);
  cbFillMissingImag->setMinimumSize(cbFillMissingImag->sizeHint());
  cbFillMissingImag->setChecked(FALSE);
  connect(cbFillMissingImag,	SIGNAL(toggled(bool)), SLOT(setMEMFillMissingImag(bool)));
  Layout_2->addWidget(cbFillMissingImag);

  cbRmShifts = new QCheckBox("Remove mean values", gbox);
  cbRmShifts->setMinimumSize(cbRmShifts->sizeHint());
  connect(cbRmShifts, SIGNAL(toggled(bool)), SLOT(setMEMRemoveShifts(bool)));
  cbRmShifts->setChecked(FALSE);
  Layout_2->addWidget(cbRmShifts);

  cbRmTrends = new QCheckBox("Remove trends from data", gbox);
  cbRmTrends->setMinimumSize(cbRmTrends->sizeHint());
  connect(cbRmTrends, SIGNAL(toggled(bool)), SLOT(setMEMRemoveTrends(bool)));
  cbRmTrends->setChecked(TRUE);
  Layout_2->addWidget(cbRmTrends);

  Layout_2->activate();
  aLayout->addWidget(gbox);


  // Controls:
  //  gbox		= new QGroupBox("Controls", W);
  butgrp= new QButtonGroup("Controls", W);
  grid  = new QGridLayout(butgrp, 17,2,   20,5);

  // num of points on the Spectrum plot:
  label = new QLabel("Number of points on the Spectrum plot:", butgrp);
  label -> setMinimumSize(label->sizeHint());
  grid  -> addWidget(label, 0,0);

  leNumOfSpPts = new QLineEdit(butgrp);
  leNumOfSpPts -> setText(Str.setNum(NSp));
  leNumOfSpPts -> setMinimumHeight(leNumOfSpPts->sizeHint().height());
  leNumOfSpPts -> setMinimumWidth (leNumOfSpPts->fontMetrics().width("1000000") + 10);
  leNumOfSpPts -> setValidator(new QIntValidator(leNumOfSpPts));
  connect(leNumOfSpPts, SIGNAL(textChanged(const QString&)), SLOT(changeNumOfSpPts(const QString&)));
  label -> setBuddy (leNumOfSpPts);
  grid  -> addWidget(leNumOfSpPts, 0,1);

  QRadioButton *rbRanges = new QRadioButton("Frequency", butgrp);
  rbRanges-> setMinimumSize(rbRanges->sizeHint());
  connect(rbRanges, SIGNAL(toggled(bool)), SLOT(setRangeFreq(bool)));
  rbRanges->setChecked(TRUE);
  grid->addMultiCellWidget(rbRanges, 1,1, 0,1);

  label = new QLabel("From:", butgrp);
  label -> setMinimumSize(label->sizeHint());
  grid  -> addWidget(label, 2,0);

  leFStart = new QLineEdit(butgrp);
  leFStart -> setText(Str.setNum(FStart));
  leFStart -> setMinimumHeight(leFStart->sizeHint().height());
  //  leFStart -> setMinimumWidth (leFStart->fontMetrics().width("-99999.99999") + 10);
  connect(leFStart, SIGNAL(textChanged(const QString&)), SLOT(changeFStart(const QString&)));
  dvFStart = new QDoubleValidator(leFStart);
  leFStart -> setValidator(dvFStart);
  grid  -> addWidget(leFStart, 2,1);

  label = new QLabel("To:", butgrp);
  label -> setMinimumSize(label->sizeHint());
  grid  -> addWidget(label, 3,0);

  leFFinis = new QLineEdit(butgrp);
  leFFinis -> setText(Str.setNum(FFinis));
  leFFinis -> setMinimumHeight(leFFinis->sizeHint().height());
  connect(leFFinis, SIGNAL(textChanged(const QString&)), SLOT(changeFFinis(const QString&)));
  dvFFinis = new QDoubleValidator(leFFinis);
  leFFinis -> setValidator(dvFFinis);
  grid  -> addWidget(leFFinis, 3,1);
  

  rbRanges = new QRadioButton("Periods", butgrp);
  rbRanges-> setMinimumSize(rbRanges->sizeHint());
  grid->addMultiCellWidget(rbRanges, 4,4, 0,1);

  label = new QLabel("From:", butgrp);
  label -> setMinimumSize(label->sizeHint());
  grid  -> addWidget(label, 5,0);

  lePStart = new QLineEdit(butgrp);
  lePStart -> setText(Str.setNum(PStart));
  lePStart -> setMinimumHeight(lePStart->sizeHint().height());
  connect(lePStart, SIGNAL(textChanged(const QString&)), SLOT(changePStart(const QString&)));
  dvPStart = new QDoubleValidator(lePStart);
  lePStart -> setValidator(dvPStart);
  grid  -> addWidget(lePStart, 5,1);

  label = new QLabel("To:", butgrp);
  label -> setMinimumSize(label->sizeHint());
  grid  -> addWidget(label, 6,0);

  lePFinis = new QLineEdit(butgrp);
  lePFinis -> setText(Str.setNum(PFinis));
  lePFinis -> setMinimumHeight(lePFinis->sizeHint().height());
  connect(lePFinis, SIGNAL(textChanged(const QString&)), SLOT(changePFinis(const QString&)));
  dvPFinis = new QDoubleValidator(lePFinis);
  lePFinis -> setValidator(dvPFinis);
  grid  -> addWidget(lePFinis, 6,1);


  //
  label = new QLabel("Scan for FPE from m:", butgrp);
  label -> setMinimumSize(label->sizeHint());
  grid  -> addWidget(label, 8,0);
  
  leFPEMstart = new QLineEdit(butgrp);
  leFPEMstart -> setText(Str.setNum(Mmin));
  leFPEMstart -> setMinimumHeight(leFPEMstart->sizeHint().height());
  leFPEMstart -> setValidator((ivMscan = new QIntValidator(leFPEMstart)));
  ivMscan->setBottom(Mmin);
  ivMscan->setTop(Mmax);
  connect(leFPEMstart, SIGNAL(textChanged(const QString&)), SLOT(changeFPE_Mstart(const QString&)));
  grid  -> addWidget(leFPEMstart, 8,1);

  label = new QLabel("Scan for FPE to m:", butgrp);
  label -> setMinimumSize(label->sizeHint());
  grid  -> addWidget(label, 9,0);
  
  leFPEMfinis = new QLineEdit(butgrp);
  leFPEMfinis -> setText(Str.setNum(Mmax));
  leFPEMfinis -> setMinimumHeight(leFPEMfinis->sizeHint().height());
  leFPEMfinis -> setValidator(ivMscan);
  connect(leFPEMfinis, SIGNAL(textChanged(const QString&)), SLOT(changeFPE_Mfinis(const QString&)));
  grid  -> addWidget(leFPEMfinis, 9,1);

  cbScaleCPD2CPY = new QCheckBox("Scale Frequency cpd->cpy", butgrp);
  cbScaleCPD2CPY->setMinimumSize(cbScaleCPD2CPY->sizeHint());
  cbScaleCPD2CPY->setChecked(ScaleCPD2CPY);
  connect(cbScaleCPD2CPY, SIGNAL(toggled(bool)), SLOT(setScalingCPD2CPY(bool)));
  grid->addMultiCellWidget(cbScaleCPD2CPY, 11,11, 0,1);


  label = new QLabel("Minimum FPE:", butgrp);
  label -> setMinimumSize(label->sizeHint());
  grid  -> addWidget(label, 13,0);

  lMinFPE = new QLabel(Str.sprintf("%.4g", MinFPE), butgrp);
  lMinFPE -> setMinimumSize(lMinFPE->sizeHint());
  grid  -> addWidget(lMinFPE, 13,1);

  label = new QLabel("Order of Min FPE:", butgrp);
  label -> setMinimumSize(label->sizeHint());
  grid  -> addWidget(label, 14,0);

  lMinFPEOrder = new QLabel(Str.setNum(MinFPEOrder), butgrp);
  lMinFPEOrder -> setMinimumSize(lMinFPEOrder->sizeHint());
  grid  -> addWidget(lMinFPEOrder, 14,1);

  label = new QLabel("Check a line at a frequency:", butgrp);
  label -> setMinimumSize(label->sizeHint());
  grid  -> addWidget(label, 15,0);
  
  leLine2Check = new QLineEdit(butgrp);
  leLine2Check -> setText(Str.setNum(FLine));
  leLine2Check -> setMinimumHeight(leLine2Check->sizeHint().height());
  dvFLine = new QDoubleValidator(leLine2Check);
  leLine2Check -> setValidator(dvFLine);
  connect(leLine2Check, SIGNAL(textChanged(const QString&)), SLOT(changeLine2Check(const QString&)));
  grid  -> addWidget(leLine2Check, 15,1);



  grid->setRowStretch( 7,  1);
  grid->setRowStretch(10,  1);
  grid->setRowStretch(12,  1);

  grid->setRowStretch(16,  2);


  grid->addRowSpacing( 0, 40);
  grid->addRowSpacing( 1, 40);
  grid->addRowSpacing( 4, 40);
  grid->addRowSpacing(10, 20);

  // ...
  grid->activate();
  Layout->addWidget(butgrp);


  Layout->activate();
  return W;
};

QWidget* SBSpectrumAnalyserMEM::wBrowseData()
{
  QWidget	*W	= new QWidget(this);
  QBoxLayout	*Layout = new QHBoxLayout(W, 20, 10);

  PCData = new SBPlotCarrier(2, 0, "Time Series");
  PCData->setAbsFile4SaveBaseName(SetUp->path2TimeSeries() + Name + ".data");

  PCData->createBranch(1, "Real part");
  PCData->createBranch(1, "Imag part");

  PCData->branches()->at(0)->data()->set(0, 0, 0.0);
  PCData->branches()->at(0)->data()->set(0, 1, 0.0);
  PCData->branches()->at(0)->setDataAttr(0, SBPlotCarrier::DA_IGN);
  PCData->branches()->at(1)->data()->set(0, 0, 0.0);
  PCData->branches()->at(1)->data()->set(0, 1, 0.0);
  PCData->branches()->at(1)->setDataAttr(0, SBPlotCarrier::DA_IGN);

  PCData->columnNames()->append(new QString("Argument"));
  //  PCData->setType(0, SBPlotCarrier::AT_MJD);
  PCData->columnNames()->append(new QString("Data"));

  PlotData = new SBPlot(PCData, W);
  Layout->addWidget(PlotData);
  Layout->activate();
  return W;
};

QWidget* SBSpectrumAnalyserMEM::wBrowseSpectrum()
{
  QWidget	*W	= new QWidget(this);
  QBoxLayout	*Layout = new QHBoxLayout(W, 20, 10);

  PCSpectrum = new SBPlotCarrier(4, 0, "Spectrum");
  PCSpectrum->setAbsFile4SaveBaseName(SetUp->path2TimeSeries() + Name + ".spectrum");

  PCSpectrum->createBranch(1, "Spectrum");

  PCSpectrum->branches()->at(0)->data()->set(0, 0, 0.0);
  PCSpectrum->branches()->at(0)->data()->set(0, 1, 0.0);
  PCSpectrum->branches()->at(0)->data()->set(0, 2, 0.0);
  PCSpectrum->branches()->at(0)->data()->set(0, 3, 0.0);
  PCSpectrum->branches()->at(0)->setDataAttr(0, SBPlotCarrier::DA_IGN);

  PCSpectrum->columnNames()->append(new QString("Frequency"));
  PCSpectrum->columnNames()->append(new QString("PSD (log)"));
  PCSpectrum->columnNames()->append(new QString("PSD (lin)"));
  PCSpectrum->columnNames()->append(new QString("PSD (sqrt)"));

  PlotSpectrum = new SBPlot(PCSpectrum, W,
			    /*SBPlot::PM_HAS_HAVE_ZERO | */ SBPlot::PM_WO_DOTS |
			    SBPlot::PM_WO_BRANCH_NAMES | SBPlot::PM_LINES
			    //			    | SBPlot::PM_IMPULSE
			    );
  //				SBPlot::PM_HAS_HAVE_ZERO | SBPlot::PM_IMPULSE | SBPlot::PM_WO_BRANCH_NAMES |
  //				SBPlot::PM_WO_AXIS_NAMES);
  Layout->addWidget(PlotSpectrum);
  
  Layout->activate();
  return W;
};

QWidget* SBSpectrumAnalyserMEM::wMEMCoeffList()
{
  QWidget	*W	= new QWidget(this);
  QBoxLayout	*Layout = new QHBoxLayout(W, 20, 10);

  PCFPE = new SBPlotCarrier(3, 0, "Spectrum");
  PCFPE->setAbsFile4SaveBaseName(SetUp->path2TimeSeries() + Name + ".fpe");

  PCFPE->createBranch(1, "FPE");
  PCFPE->branches()->at(0)->data()->set(0, 0, 0.0);
  PCFPE->branches()->at(0)->data()->set(0, 1, 0.0);
  PCFPE->branches()->at(0)->data()->set(0, 2, 0.0);

  PCFPE->branches()->at(0)->setDataAttr(0, SBPlotCarrier::DA_IGN);

  PCFPE->columnNames()->append(new QString("Order of the filter (M)"));
  PCFPE->columnNames()->append(new QString("FPE (log)"));
  PCFPE->columnNames()->append(new QString("FPE (lin)"));

  PlotFPE = new SBPlot(PCFPE, W, SBPlot::PM_WO_BRANCH_NAMES);
  Layout->addWidget(PlotFPE);
  Layout->activate();
  return W;
};

// slots/actions:
void SBSpectrumAnalyserMEM::accept()
{
  QDialog::accept();
  delete this;
};

void SBSpectrumAnalyserMEM::reject() 
{
  QDialog::reject();
  delete this;
};

void SBSpectrumAnalyserMEM::setMEMAssumeEquiDistant(bool Is_)
{
  if (MESA)
    {
      if (Is_)
	MESA->addAttr(SBMEM::ignoreNotEquidistant);
      else
	MESA->delAttr(SBMEM::ignoreNotEquidistant);
    };
};

void SBSpectrumAnalyserMEM::setMEMFillMissingImag(bool Is_)
{
  if (MESA)
    {
      if (Is_)
	MESA->addAttr(SBMEM::fillMissingImag);
      else
	MESA->delAttr(SBMEM::fillMissingImag);
    };
};

void SBSpectrumAnalyserMEM::setMEMRemoveShifts(bool Is_)
{
  if (MESA)
    {
      if (Is_)
	MESA->addAttr(SBMEM::removeShifts);
      else
	MESA->delAttr(SBMEM::removeShifts);
    };
};

void SBSpectrumAnalyserMEM::setMEMRemoveTrends(bool Is_)
{
  if (MESA)
    {
      if (Is_)
	MESA->addAttr(SBMEM::removeTrends);
      else
	MESA->delAttr(SBMEM::removeTrends);
    };
};

void SBSpectrumAnalyserMEM::setRangeFreq(bool Is_)
{
  if (IsPlotFreq != Is_)
    {
      IsPlotFreq = Is_;
      redrawSpectrumPlot();
    };
};

void SBSpectrumAnalyserMEM::setMEMType(int id_)
{
  if (0<=id_ && id_<=3 && (MEMType != (SBMEM::MEMType)id_))
    {
      MEMType = (SBMEM::MEMType)id_;
      MinFPEOrder = 0;
      MinFPE = 0.0;

      MESA->setType(MEMType);
      Mmax = MESA->validM();
      sbMval->setMaxValue(Mmax);
      ivMscan->setTop(Mmax);
      leFPEMfinis->setText(Str.setNum(Mmax));
    };
};

void SBSpectrumAnalyserMEM::setScalingCPD2CPY(bool Is_)
{
  double f0=0.0;
  if (ScaleCPD2CPY != Is_)
    {
      ScaleCPD2CPY = Is_;
      if (ScaleCPD2CPY)
	{
	  FStart *= 365.25;
	  FFinis *= 365.25;
	  FLine  *= 365.25;
	  f0 = MESA->fN()*365.25;
	  dvFStart->setBottom(-f0);
	  dvFFinis->setTop(f0);
	  dvFLine->setBottom(-f0);
	  dvFLine->setTop(f0);
	}
      else
	{
	  FStart /= 365.25;
	  FFinis /= 365.25;
	  FLine  /= 365.25;
	  f0 = MESA->fN();
	  dvFStart->setBottom(-f0);
	  dvFFinis->setTop(f0);
	  dvFLine->setBottom(-f0);
	  dvFLine->setTop(f0);
	};
      browseLimits();
      redrawSpectrumPlot();
    };
};


void SBSpectrumAnalyserMEM::readDataFile()
{
  QString	FName(QFileDialog::getOpenFileName(SetUp->path2TimeSeries(), QString::null,
						   this, "Select a file with time series"));
  if (!FName.isEmpty())
    {
      MESA->readDataFile(FName);
    };
  
  if (MESA->isOK())
    {
      QFileInfo fiName(FName);

      Name = fiName.baseName() + "." + fiName.extension();
      lImportedFileName->setText(Name);
      setCaption("Time Series Spectrum Analyser [" + Name + "]");
      
      // adjust max M value:
      Mmax = MESA->validM();
      sbMval->setMaxValue(Mmax);
      resetLimits();
      
      MinFPEOrder = 0;
      MinFPE = 0.0;

      leFPEMstart->setText(Str.setNum(Mmin));
      leFPEMfinis->setText(Str.setNum(Mmax));

      dvFStart->setBottom(FStart);
      dvFFinis->setTop(FFinis);
      dvFLine->setBottom(FStart);
      dvFLine->setTop(FFinis);
      ivMscan->setBottom(Mmin);
      ivMscan->setTop(Mmax);

      //      dvPStart->setBottom(FStart);
      //      dvPFinis->setTop(FFinis);

      // plot data:
      makeAnalysis();
    }
  else
    {
      lImportedFileName	-> setText("");
      lNumOfPts		-> setText("0");
      Log->write(SBLog::INF, SBLog::DATA, ClassName() + ": the file [" + 
		 Name + "] does not contain usefull data");
    };
  lNumOfPts->setText(Str.setNum(MESA->n()));
  lDT->setText(Str.sprintf("%.4g", MESA->dt()));
  lShiftR->setText(Str.sprintf("%.4g", MESA->shiftR()));
  lTrendR->setText(Str.sprintf("%.4g", MESA->trendR()));
  lShiftI->setText(Str.sprintf("%.4g", MESA->shiftI()));
  lTrendI->setText(Str.sprintf("%.4g", MESA->trendI()));
  lSampleVar->setText(Str.sprintf("%.6g", MESA->sampleVar()));

  redrawDataPlot();
};

void SBSpectrumAnalyserMEM::collectData()
{

};

void SBSpectrumAnalyserMEM::makeAnalysis()
{
  if (MESA->isOK())
    {
      QApplication::setOverrideCursor(Qt::waitCursor);
      qApp->processEvents();
      
      collectData();
      MESA->setM(M);
      if (!MESA->isReady())
	{
	  MESA->prepareSpectrum();
	  if (MinFPEOrder==0 || (MinFPEOrder && MESA->fpe()<MinFPE))
	    {
	      MinFPEOrder = MESA->m();
	      MinFPE = MESA->fpe();
	      lMinFPE->setText(Str.sprintf("%.4g", MinFPE));
	      lMinFPEOrder->setText(Str.setNum(MinFPEOrder));
	    };
	};
      qApp->processEvents();
      redrawSpectrumPlot();
      qApp->processEvents();
      redrawFPEPlot();
      QApplication::restoreOverrideCursor();
      qApp->processEvents();
    }
  else
    Log->write(SBLog::INF, SBLog::DATA, ClassName() + ": the file [" + 
	       Name + "] does not contain usefull data");
};

void SBSpectrumAnalyserMEM::scan4FPE()
{
  if (MESA->isOK())
    {
      if (IsScanning)
	{
	  IsScanStop = TRUE;
	  Ok->blockSignals(FALSE);
	}
      else
	{
	  IsScanning = TRUE;
	  Scan->setText("Stop");
	  Ok->blockSignals(TRUE);
	  for (int i=FPE_Mstart; i<=FPE_Mfinis && !IsScanStop; i++)
	    {
	      M = i;
	      sbMval->setValue(M);
	      makeAnalysis();
	    };
	  IsScanning = FALSE;
	  IsScanStop = FALSE;  
	  Scan->setText("Scan");
	  Ok->blockSignals(FALSE);
	};
    }
  else
    Log->write(SBLog::INF, SBLog::DATA, ClassName() + ": the file [" + 
	       Name + "] does not contain usefull data");
};

void SBSpectrumAnalyserMEM::resetLimits()
{
  FStart = -MESA->fN();
  FFinis =  MESA->fN();
  if (ScaleCPD2CPY)
    {
      FStart*=365.25;
      FFinis*=365.25;
    };
  PStart = -2.0*MESA->interval();
  PFinis =  2.0*MESA->interval();

  browseLimits();
};

void SBSpectrumAnalyserMEM::browseLimits()
{
  leFStart->setText(Str.sprintf("%.4g", FStart));
  leFFinis->setText(Str.sprintf("%.4g", FFinis));
  lePStart->setText(Str.sprintf("%.4g", PStart));
  lePFinis->setText(Str.sprintf("%.4g", PFinis));
  leLine2Check->setText(Str.sprintf("%.6g", FLine));
};

void SBSpectrumAnalyserMEM::changeFilterOrder(int M_)
{
  M=M_;
  if (IsAutoCalc)
    makeAnalysis();
};

void SBSpectrumAnalyserMEM::changeNumOfSpPts(const QString& Str_)
{
  int				d;
  bool				isOK;
  d = Str_.toInt(&isOK);
  if (isOK) 
    NSp = d;
};

void SBSpectrumAnalyserMEM::changeFStart(const QString& Str_)
{
  double			f;
  bool				isOK;
  f = Str_.toDouble(&isOK);
  if (isOK) 
    FStart = f;
};

void SBSpectrumAnalyserMEM::changeFFinis(const QString& Str_)
{
  double			f;
  bool				isOK;
  f = Str_.toDouble(&isOK);
  if (isOK) 
    FFinis = f;
};

void SBSpectrumAnalyserMEM::changeLine2Check(const QString& Str_)
{
  double			f;
  bool				isOK;
  f = Str_.toDouble(&isOK);
  if (isOK) 
    FLine = f;
};

void SBSpectrumAnalyserMEM::changePStart(const QString& Str_)
{
  double			f;
  bool				isOK;
  f = Str_.toDouble(&isOK);
  if (isOK) 
    PStart = f;
};

void SBSpectrumAnalyserMEM::changePFinis(const QString& Str_)
{
  double			f;
  bool				isOK;
  f = Str_.toDouble(&isOK);
  if (isOK) 
    PFinis = f;
};

void SBSpectrumAnalyserMEM::changeFPE_Mstart(const QString& Str_)
{
  int				d;
  bool				isOK;
  d = Str_.toInt(&isOK);
  if (isOK) 
    FPE_Mstart = d;
};

void SBSpectrumAnalyserMEM::changeFPE_Mfinis(const QString& Str_)
{
  int				d;
  bool				isOK;
  d = Str_.toInt(&isOK);
  if (isOK) 
    FPE_Mfinis = d;
};

void SBSpectrumAnalyserMEM::redrawDataPlot()
{
  SBPlotBranch		*pb = NULL;

  PCData->branches()->clear();
  if (MESA->isOK())
    {
      PCData->createBranch(MESA->n(), "Real part");
      PCData->createBranch(MESA->n(), "Imag part");
      PCData->setAbsFile4SaveBaseName(SetUp->path2TimeSeries() + Name + ".data");
      
      for (unsigned int i=0; i<MESA->n(); i++)
	{
	  pb = PCData->branches()->at(0);
	  pb->data()->set(i, 0, MESA->x()->at(i));
	  pb->data()->set(i, 1, MESA->z_r()->at(i));
	  
	  pb = PCData->branches()->at(1);
	  pb->data()->set(i, 0, MESA->x()->at(i));
	  pb->data()->set(i, 1, MESA->z_i()->at(i));
	};
    }
  else // clear the plot
    {
      PCData->createBranch(1, "Real part");
      PCData->createBranch(1, "Imag part");
      PCData->branches()->at(0)->data()->set(0, 0, 0.0);
      PCData->branches()->at(0)->data()->set(0, 1, 0.0);
      PCData->branches()->at(0)->setDataAttr(0, SBPlotCarrier::DA_IGN);
      PCData->branches()->at(1)->data()->set(0, 0, 0.0);
      PCData->branches()->at(1)->data()->set(0, 1, 0.0);
      PCData->branches()->at(1)->setDataAttr(0, SBPlotCarrier::DA_IGN);
    };
  PlotData->dataChanged();
};

void SBSpectrumAnalyserMEM::redrawSpectrumPlot()
{
  PCSpectrum->branches()->clear();
  if (MESA->isReady())
    {
      SBMatrix		*B=NULL;
      double		x, psd;
      
      PCSpectrum->setAbsFile4SaveBaseName(SetUp->path2TimeSeries() + Name + ".spectrum");
      PCSpectrum->createBranch(NSp+1, "Spectrum");
      B = PCSpectrum->branches()->at(0)->data();
      
      if (IsPlotFreq)
	{
	  if (ScaleCPD2CPY)
	    *(PCSpectrum->columnNames()->at(0))="Frequency, cpy";
	  else
	    *(PCSpectrum->columnNames()->at(0))="Frequency";

	  x = FStart;
	  double	m1, m2, m3;
	  m1 = m2 = m3 = 0.0;
	  for (unsigned int i=0; i<=NSp; i++)
	    {
	      psd = MESA->psd(ScaleCPD2CPY?x/365.25:x);
	      B->set(i, 0, x);
	      B->set(i, 2, psd);
	      m2 += psd;
	      if (psd>0.0)
		{
		  B->set(i, 1, 10.0*log10(psd));
		  B->set(i, 3, sqrt(psd));
		  m1 += B->at(i, 1);
		  m3 += B->at(i, 3);
		}
	      else
		{
		  B->set(i, 1, 0.0);
		  B->set(i, 3, 0.0);
		  PCSpectrum->branches()->at(0)->setDataAttr(i, SBPlotCarrier::DA_IGN);
		};
	      
	      x+=(FFinis - FStart)/NSp;
	    };
	  if (FLine!=0.0)
	    {
	      PCSpectrum->createBranch(1, "ZLine");
	      PCSpectrum->branches()->at(1)->data()->set(0, 0, FLine);
	      PCSpectrum->branches()->at(1)->data()->set(0, 1, m1/NSp);
	      PCSpectrum->branches()->at(1)->data()->set(0, 2, m2/NSp);
	      PCSpectrum->branches()->at(1)->data()->set(0, 3, m3/NSp);
	      PCSpectrum->branches()->at(1)->setDataAttr(0, SBPlotCarrier::DA_BAR);
	    };
	}
      else
	{
	  double gap = 1.0/MESA->fN();

	  *(PCSpectrum->columnNames()->at(0))="Period";
	  x = PStart;
	  for (unsigned int i=0; i<=NSp; i++)
	    {
	      if (x<-gap || gap<x)
		{
		  psd	= MESA->psd(1.0/x);
		  
		  B->set(i, 0, x);
		  B->set(i, 2, psd);
		  if (psd>0.0)
		    {
		      B->set(i, 1, 10.0*log10(psd));
		      B->set(i, 3, sqrt(psd));
		    }
		  else
		    {
		      B->set(i, 1, 0.0);
		      B->set(i, 3, 0.0);
		      PCSpectrum->branches()->at(0)->setDataAttr(i, SBPlotCarrier::DA_IGN);
		    };
		}
	      else
		{
		  B->set(i, 0, 0.0);
		  B->set(i, 1, 0.0);
		  B->set(i, 2, 0.0);
		  PCSpectrum->branches()->at(0)->setDataAttr(i, SBPlotCarrier::DA_IGN);
		};
	      x+=(PFinis - PStart)/NSp;
	    };
	};
    }
  else // clear the plot
    {
      PCSpectrum->createBranch(1, "Spectrum");
      PCSpectrum->branches()->at(0)->data()->set(0, 0, 0.0);
      PCSpectrum->branches()->at(0)->data()->set(0, 1, 0.0);
      PCSpectrum->branches()->at(0)->data()->set(0, 2, 0.0);
      PCSpectrum->branches()->at(0)->data()->set(0, 3, 0.0);
      PCSpectrum->branches()->at(0)->setDataAttr(0, SBPlotCarrier::DA_IGN);
    };
  PlotSpectrum->dataChanged();
};

void SBSpectrumAnalyserMEM::redrawFPEPlot()
{
  SBMatrix		*B =NULL;
  SBPlotBranch		*pb=NULL;
  double		fpe = 0.0;

  // if the order has been changed, clear all:
  if (!PCFPE->branches()->count() || (PCFPE->branches()->count() && PCFPE->branches()->at(0)->nR()!=Mmax))
    {
      PCFPE->branches()->clear();
      PCFPE->setAbsFile4SaveBaseName(SetUp->path2TimeSeries() + Name + ".fpe");

      PCFPE->createBranch(Mmax, "FPE");
      pb= PCFPE->branches()->at(0);
      B = pb->data();
      for (unsigned int i=0; i<Mmax; i++)
	{
	  B ->set(i,0, 0.0);
	  B ->set(i,1, 0.0);
	  B ->set(i,2, 0.0);
	  pb->setDataAttr(i, SBPlotCarrier::DA_IGN);
	};
    };
  if (MESA->isReady() && (fpe = MESA->fpe())>0.0)
    {
      pb= PCFPE->branches()->at(0);
      B = pb->data();
      B ->set(MESA->m()-1,0, MESA->m());
      
      B ->set(MESA->m()-1,1, log10(fpe));
      B ->set(MESA->m()-1,2, fpe);
      pb->setDataAttr(MESA->m()-1, 0);
    };
  PlotFPE->dataChanged();
};

void SBSpectrumAnalyserMEM::checkLine()
{
  SBMatrix		*B  = NULL;
  SBPlotBranch		*pb = NULL;
  unsigned int		LinIdx = 2;
  unsigned int		LogIdx = 1;
  bool			IsGotMax = FALSE;
  bool			IsGotLeft = FALSE;
  bool			IsGotRight = FALSE;

  if (PCSpectrum && MESA->isReady() && (FLine != 0.0))
    {
      pb = PCSpectrum->findBranch("Spectrum");
      if (pb && pb->nR()>3)
	{
	  unsigned int	NR = pb->nR();
	  B = pb->data();
	  if (B->at(1,0)<FLine && FLine<B->at(NR-2,0))
	    {
	      //	  Log->write(SBLog::DBG, SBLog::DATA, ClassName() + ": got it!");
	      double		F_left = 0.0, F_right = 0.0;
	      double		F_mleft = 0.0, F_mright = 0.0;
	      double		F_max = 0.0;
	      double		PSD_max = 0.0;
	      double		psd = 0.0;
	      double		Q = 0.0;

	      unsigned int	idx = 0;
	      unsigned int	Idx_max = 0;
	      unsigned int	i = 0;

	      unsigned int	LeftIdx = 0, RightIdx = 0;
	      

	      Idx_max = 0;
	      F_max = 0.0;
	      idx = 0;
	      while (B->at(idx, 0) < FLine)
		idx++;

	      // determine left and right limits of the peak:
	      // to the right:
	      psd = B->at((i=idx-1), LogIdx);
	      if (B->at(idx-1, LogIdx) <= B->at(idx, LogIdx)) // skip maximum
		{
		  while (i<NR && psd<=B->at(++i, LogIdx))
		    psd = B->at(i, LogIdx);
		  Idx_max = i-1;
		  F_max = B->at(Idx_max, 0);
		};
	      while (i<NR-1 && psd>B->at(++i, LogIdx))
		psd = B->at(i, LogIdx);
	      if (i==NR-1)
		Log->write(SBLog::INF, SBLog::DATA, ClassName() +
			   ": could not determine the right minimum: reached end of the right wing of the plot");
	      RightIdx = i-1;
	      F_right  = B->at(RightIdx, 0);
	      
	      // to the left:
	      psd = B->at((i=idx), LogIdx);
	      if (B->at(i-1, LogIdx) >= B->at(i, LogIdx)) // skip maximum
		{
		  while (0<i && psd<=B->at(--i, LogIdx))
		    psd = B->at(i, LogIdx);
		  Idx_max = i+1;
		  F_max = B->at(Idx_max, 0);
		};
	      while (0<i && psd>B->at(--i, LogIdx))
		psd = B->at(i, LogIdx);
	      if (i==0)
		Log->write(SBLog::INF, SBLog::DATA, ClassName() +
			   ": could not determine the left minimum: reached end of the left wing of the plot");
	      LeftIdx = i+1;
	      F_left  = B->at(LeftIdx, 0);
	      
	      Log->write(SBLog::INF, SBLog::DATA, ClassName() + ": found left-right limits of the peak: " +
			 Str.sprintf("[%d:%d] (%.6g:%.6g); ApprMax at [%d] (%.6g)", 
				     LeftIdx, RightIdx, F_left, F_right, Idx_max, F_max));

	      // determine maximum:
	      if (Idx_max>1 && Idx_max<NR-1)
		{
		  psd = B->at(Idx_max-2, LinIdx);
		  double FStep = (B->at(Idx_max+2, 0) - B->at(Idx_max-2, 0))/2000.0;
		  double f = B->at(Idx_max-2, 0) + FStep;
		  double psd_n = 0.0;
		  idx = 0;
		  while (idx<2001 && psd<(psd_n=MESA->psd(ScaleCPD2CPY?f/365.25:f)))
		    {
		      idx++;
		      f += FStep;
		      psd = psd_n;
		    };
		  if (idx && idx<2001)
		    {
		      F_max = f - FStep;
		      IsGotMax = TRUE;
		      Log->write(SBLog::DBG, SBLog::DATA, ClassName() + ": exact maximum of the peak: " +
				 Str.sprintf("%.8g (%.12g)", F_max, FStep));
		    }
		  else
		    Log->write(SBLog::INF, SBLog::DATA, ClassName() + 
			       ": could not determine maximum: reached end of the ranges, idx=" + 
			       Str.setNum(idx) + "; increase number of points!");

		}
	      else
		Log->write(SBLog::INF, SBLog::DATA, ClassName() + ": could not determine maximum");
	      
	      // determine FWHM:
	      if (IsGotMax)
		{
		  PSD_max = MESA->psd(ScaleCPD2CPY?F_max/365.25:F_max);
		  double HalfMax = PSD_max/2.0;

		  // left wing:
		  idx = Idx_max;
		  while (idx>1 && B->at(idx, LinIdx)>HalfMax)
		    idx--;
		  if (idx>1 && idx<Idx_max)
		    {
		      /*
			Log->write(SBLog::INF, SBLog::DATA, ClassName() + ": got left HM: " + 
			Str.sprintf("idx: %d; Psd: %.8g [%.8g: (%.8g) :%.8g]", 
			idx, HalfMax,
			B->at(idx-1, LinIdx), B->at(idx, LinIdx), B->at(idx+1, LinIdx)));
		      */
		      double FStep = (B->at(idx+1, 0) - B->at(idx, 0))/2000.0;
		      double f = B->at(idx, 0) - 200*FStep;
		      idx = 0;
		      while(idx<2401 && MESA->psd(ScaleCPD2CPY?f/365.25:f)<HalfMax)
			{
			  idx++;
			  f += FStep;
			};
		      if (idx && idx<2401)
			{
			  F_mleft = f - FStep;
			  IsGotLeft = TRUE;
			  Log->write(SBLog::DBG, SBLog::DATA, ClassName() + ": left half maximum: " +
				     Str.sprintf("%.8g (%.12g)", F_mleft, FStep));
			}
		      else
			Log->write(SBLog::INF, SBLog::DATA, ClassName() + 
				   ": could not determine left half maximum: reached end of the ranges, idx=" + 
				   Str.setNum(idx));
		    }
		  else
		    Log->write(SBLog::INF, SBLog::DATA, ClassName() + 
			       ": could not determine left HM: reached end of the ranges, idx=" + 
			       Str.setNum(idx) + "; increase number of points!");

		  // right wing:
		  idx = Idx_max;
		  while (idx<NR-1 && B->at(idx, LinIdx)>HalfMax)
		    idx++;
		  if (idx>Idx_max && idx<NR-1)
		    {
		      /*
			Log->write(SBLog::INF, SBLog::DATA, ClassName() + ": got right HM: " + 
			Str.sprintf("idx: %d; Psd: %.8g [%.8g: (%.8g) :%.8g]", 
			idx, HalfMax,
			B->at(idx-1, LinIdx), B->at(idx, LinIdx), B->at(idx+1, LinIdx)));
		      */
		      double FStep = (B->at(idx, 0) - B->at(idx-1, 0))/2000.0;
		      double f = B->at(idx-1, 0) - 200*FStep;
		      idx = 0;
		      while(idx<2401 && MESA->psd(ScaleCPD2CPY?f/365.25:f)>HalfMax)
			{
			  idx++;
			  f += FStep;
			};
		      if (idx && idx<2401)
			{
			  F_mright = f - FStep;
			  IsGotRight = TRUE;
			  Log->write(SBLog::DBG, SBLog::DATA, ClassName() + ": right half maximum: " +
				     Str.sprintf("%.8g (%.12g)", F_mright, FStep));
			}
		      else
			Log->write(SBLog::INF, SBLog::DATA, ClassName() + 
				   ": could not determine right half maximum: reached end of the ranges, idx=" + 
				   Str.setNum(idx));
		    }
		  else
		    Log->write(SBLog::INF, SBLog::DATA, ClassName() + 
			       ": could not determine right HM: reached end of the ranges, idx=" + Str.setNum(idx));

		  double	Ps = 0.0;
		  for(idx = LeftIdx; idx<=RightIdx; idx++)
		    Ps += (B->at(idx, LinIdx) + B->at(idx-1, LinIdx))*(B->at(idx, 0) - B->at(idx-1, 0))/2.0;
		  
		  if (ScaleCPD2CPY)
		    Ps /= 365.25;
		  if (IsGotLeft && IsGotRight)
		    {
		      Q = fabs(F_max)/(F_mright - F_mleft);

		      // 22  -0.89927  406.16   3.1796099   0.20350782 1101   0.100063
		      Log->write(SBLog::INF, SBLog::DATA, ClassName() + 
				 Str.sprintf(": Parameters of the line: MaxFreq: %.8g; Q: %.8g; Amplitude: %.8g", 
					     F_max, Q, sqrt(Ps)));
		      Log->write(SBLog::INF, SBLog::DATA, ClassName() + 
				 Str.sprintf(": (M, F, P, Q, Amp, N, SmplVar): %3d  %9.6f(%s) %6.2f(d) %6.3f %8.6f %4d %8.6f", 
					     MESA->m(), F_max, ScaleCPD2CPY?"cpy":"cpd",
					     (ScaleCPD2CPY?365.25:1.0)/F_max, Q, sqrt(Ps), 
					     MESA->n(), MESA->sampleVar()));
		      
		    };
		};
	      
	    }
	  else 
	    Log->write(SBLog::INF, SBLog::DATA, ClassName() + ": the line " + Str.sprintf("%.6g", FLine) +
		       " is out of limits of the plot");
	};
    };
};
/*==============================================================================================*/


/*==============================================================================================*/
