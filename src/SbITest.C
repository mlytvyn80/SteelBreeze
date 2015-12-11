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

#include "SbITest.H"

#include <qbuttongroup.h>
#include <qgroupbox.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qvalidator.h>

#include "SbSetup.H"
#include "SbGeoAtmLoad.H"
#include "SbGeoProject.H"
#include "SbGeoTide.H"
#include "SbGeoTime.H"
#include "SbGeoEop.H"
#include "SbGeoEphem.H"
#include "SbGeoRefFrame.H"
#include "SbGeoSources.H"
#include "SbIPlot.H"

#include "SbMathSpline.H"






/*==============================================================================================*/
/*												*/
/* SBTest..											*/
/*												*/
/*==============================================================================================*/
SBTest::SBTest(QWidget* parent, const char* name, bool modal, WFlags f)
  : QDialog(parent, name, modal, f)
{
  setCaption("Test");
  Date = new SBMJD(1999, 2, 28, 17, 15, 50);
  TStart = NULL; 
  TFinis = NULL;
  IsDateModified = FALSE;
  //createWidgets();
};

SBTest::~SBTest()
{
  if (Date) delete Date;
  if (TStart) delete TStart;
  if (TFinis) delete TFinis;
};

void SBTest::createWidgets()
{
  QGroupBox	*gbox;
  QBoxLayout	*Layout, *SubLayout;
  QGridLayout	*grid;
  QLabel	*label;

  gbox = new QGroupBox("Date (UTC)", this);
  grid = new QGridLayout(gbox, 3, 9,  20, 5);

  label= new QLabel("Year:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label,  0,0);

  label= new QLabel("Month:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label,  1,0);

  label= new QLabel("Day:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label,  2,0);

  label= new QLabel("Hour:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label,  0,3);

  label= new QLabel("Min:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label,  1,3);

  label= new QLabel("Sec:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label,  2,3);

  label= new QLabel("MJD:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label,  0,6);

  label= new QLabel("Date:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label,  1,6);

  label= new QLabel("Range:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label,  2,6);

  EYear = new QLineEdit(gbox);
  EYear -> setText(str.sprintf("%d", Date->year()));
  EYear -> setMinimumHeight(EYear->sizeHint().height());
  EYear -> setMinimumWidth (EYear->fontMetrics().width("1998")+10);
  EYear -> setValidator(new QIntValidator(1970, 2200, EYear));
  grid  -> addWidget(EYear, 0,1);

  EMonth = new QLineEdit(gbox);
  EMonth -> setText(str.sprintf("%d", Date->month()));
  EMonth -> setMinimumHeight(EMonth->sizeHint().height());
  EMonth -> setValidator(new QIntValidator(1, 12, EMonth));
  grid  -> addWidget(EMonth, 1,1);

  EDay = new QLineEdit(gbox);
  EDay -> setText(str.sprintf("%d", Date->day()));
  EDay -> setMinimumHeight(EDay->sizeHint().height());
  EDay -> setValidator(new QIntValidator(1, 31, EDay));
  grid -> addWidget(EDay, 2,1);

  EHour = new QLineEdit(gbox);
  EHour -> setText(str.sprintf("%d", Date->hour()));
  EHour -> setMinimumHeight(EHour->sizeHint().height());
  EHour -> setValidator(new QIntValidator(0, 24, EHour));
  grid  -> addWidget(EHour, 0,4);

  EMin = new QLineEdit(gbox);
  EMin -> setText(str.sprintf("%d", Date->min()));
  EMin -> setMinimumHeight(EMin->sizeHint().height());
  EMin -> setValidator(new QIntValidator(0, 60, EMin));
  grid -> addWidget(EMin, 1,4);

  ESec = new QLineEdit(gbox);
  ESec -> setText(str.sprintf("%.3f", Date->sec()));
  ESec -> setMinimumHeight(ESec->sizeHint().height());
  ESec -> setMinimumWidth (ESec->fontMetrics().width("55.555")+10);
  ESec -> setValidator(new QDoubleValidator(0.0, 60.0, 3, ESec));
  grid -> addWidget(ESec, 2,4);

  LMJD= new QLabel(str.sprintf("%.10f (days)", (double)(*Date)), gbox);
  LMJD->setMinimumSize(LMJD->sizeHint());
  grid->addWidget(LMJD,  0,7);

  LDate= new QLabel(Date->toString(SBMJD::F_Short), gbox);
  LDate->setMinimumSize(LDate->sizeHint());
  grid->addWidget(LDate,  1,7);

  LRange= new QLabel("[" + (tStart()?tStart()->toString(SBMJD::F_DDMonYYYY):QString("ALL")) 
		     + " - " + (tFinis()?tFinis()->toString(SBMJD::F_DDMonYYYY):QString("ALL")) 
		     + "]", gbox);
  LRange->setMinimumSize(LRange->sizeHint());
  grid->addWidget(LRange,  2,7);

  grid->addColSpacing(2, 20);
  grid->addColSpacing(5, 20);
  grid->activate();


  //---
  Layout = new QVBoxLayout(this, 20, 10);
  Layout -> addWidget(createWidget4Test(this), 20);

  SubLayout = new QHBoxLayout();
  Layout -> addLayout(SubLayout, 0);
  SubLayout -> addWidget(gbox);
  //
  QWidget	*aux = auxWidgetRt(this);
  if (aux)
    SubLayout -> addWidget(aux);
  //

  SubLayout = new QHBoxLayout(10);
  Layout -> addLayout(SubLayout);
  SubLayout-> addStretch(1);

  QPushButton *Ok	= new QPushButton("OK", this);
  QPushButton *Calc	= new QPushButton("Calc", this);
  QPushButton *Cancel	= new QPushButton("Cancel", this);
  Calc->setDefault(TRUE);

  QSize	BtnSize;
  Cancel->setMinimumSize((BtnSize=Cancel->sizeHint()));
  Ok    ->setMinimumSize(BtnSize);
  Calc  ->setMinimumSize(BtnSize);

  SubLayout->addWidget(Ok);
  SubLayout->addWidget(Calc);
  SubLayout->addWidget(Cancel);

  connect(Ok,	  SIGNAL(clicked()), SLOT(accept()));
  connect(Calc,   SIGNAL(clicked()), SLOT(recalc()));
  connect(Cancel, SIGNAL(clicked()), SLOT(reject()));

  Layout -> activate();
};

QWidget* SBTest::createWidget4Test(QWidget* parent)
{
  QGroupBox	*gbox = new QGroupBox("Test", parent);
  QBoxLayout	*layout = new QVBoxLayout(gbox, 20, 10);
  
  QLabel *label= new QLabel("A test", gbox);
  label->setMinimumSize(label->sizeHint());
  layout->addWidget(label);

  layout->activate();
  return gbox;
};

void SBTest::accept()
{
  QDialog::accept();
  delete this;
};

void SBTest::reject()
{
  QDialog::reject();
  delete this;
};

void SBTest::recalc()
{
  int		yr,mo,dy,hr,mi;
  double	se;
  bool		isOK=FALSE;
  SBMJD		mjdTmp;

  IsDateModified = FALSE;
  
  yr = EYear->text().toInt(&isOK);
  if (!isOK) 
    return;
  mo = EMonth->text().toInt(&isOK);
  if (!isOK) 
    return;
  dy = EDay->text().toInt(&isOK);
  if (!isOK) 
    return;
  hr = EHour->text().toInt(&isOK);
  if (!isOK) 
    return;
  mi = EMin->text().toInt(&isOK);
  if (!isOK) 
    return;
  se = ESec->text().toDouble(&isOK);
  if (!isOK) 
    return;

  mjdTmp.setMJD(yr,mo,dy,hr,mi,se);

  if (TStart&&*TStart>mjdTmp) *Date = *TStart;
  if (TFinis&&mjdTmp>*TFinis) *Date = *TFinis;

  if (*Date != mjdTmp)
    {
      *Date=mjdTmp;
      IsDateModified = TRUE;

      EYear -> setText(str.sprintf("%d", Date->year()));
      EMonth-> setText(str.sprintf("%d", Date->month()));
      EDay  -> setText(str.sprintf("%d", Date->day()));
      EHour -> setText(str.sprintf("%d", Date->hour()));
      EMin  -> setText(str.sprintf("%d", Date->min()));
      ESec  -> setText(str.sprintf("%.3f", Date->sec()));
      LMJD  -> setText(str.sprintf("%.10f (days)", (double)(*Date)));
      LDate -> setText(Date->toString(SBMJD::F_Short));
    };
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBTestMatrix..										*/
/*												*/
/*==============================================================================================*/
QWidget* SBTestMatrix::createMatrixWidget(QWidget* parent)
{
  QLabel	*label;
  QGridLayout	*grid;
  QGroupBox	*gbox;

  gbox = new QGroupBox("Matrix", parent);
  grid = new QGridLayout(gbox, 4,4,  20,5);
  gbox->setFrameStyle(QFrame::Panel|QFrame::Sunken);
  for (int i=0; i<3; i++)
    {
      label = new QLabel(gbox);
      label->setNum(i);
      label->setMinimumSize(label->sizeHint());
      grid->addWidget(label,  1+i,0);

      label = new QLabel(gbox);
      label->setNum(i);
      label->setAlignment(AlignCenter);
      label->setMinimumSize(label->sizeHint());
      grid->addWidget(label,  0,1+i);

      for (int j=0; j<3; j++)
	{
	  LMat[i][j] = 
	    new QLabel(str.sprintf("%19.16f", Mat?Mat->at((DIRECTION)i,(DIRECTION)j):1.0), gbox);
	  LMat[i][j]->setAlignment(AlignRight);
	  LMat[i][j]->setMinimumSize(LMat[i][j]->sizeHint());
	  grid->addWidget(LMat[i][j],  1+i,1+j);
	};
    };
  grid->activate();
  return gbox;
};

void SBTestMatrix::displayValues()
{  
  for (int i=0; i<3; i++) 
    for (int j=0; j<3; j++) 
      LMat[i][j]->setText(str.sprintf("% 18.16f", Mat?Mat->at((DIRECTION)i,(DIRECTION)j):1.0));
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBTestPrecession..										*/
/*												*/
/*==============================================================================================*/
SBTestPrecession::SBTestPrecession(QWidget* parent, const char* name, bool modal, WFlags f)
  : SBTestMatrix(parent, name, modal, f)
{
  setCaption("The IERS 1976 Theory of Precession");
  Pre = new SBPrec_IAU1976;
  Mat = Pre;
  Time= new SBTime;
  *Date= SBMJD::currentMJD(); //'cause TStart==TFinis==NULL
  createWidgets();
};

SBTestPrecession::~SBTestPrecession()
{
  if (Pre) delete Pre;
  if (Time) delete Time;
};

QWidget* SBTestPrecession::createWidget4Test(QWidget* parent)
{
  QLabel	*label;
  QGridLayout	*grid;
  QGroupBox	*W=new QGroupBox("Precession", parent);
  QBoxLayout	*Layout=new QVBoxLayout(W, 20,10);

  Time->setUTC(*Date);
  (*Pre)(*Time);
  
  Layout->addWidget(createMatrixWidget(W));
  Layout->addSpacing(20);

  grid = new QGridLayout(4,2, 5);
  Layout->addLayout(grid);

  label = new QLabel("Zeta:", W);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label,  0,0);

  label = new QLabel("Theta:", W);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label,  1,0);

  label = new QLabel("Z:", W);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label,  2,0);

  label = new QLabel("Time (TT scale):", W);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label,  3,0);

  LZeta = new QLabel(rad2dmsStr(Pre->zeta()), W);
  LZeta->setAlignment(AlignRight);
  LZeta->setMinimumSize(LZeta->sizeHint());
  grid->addWidget(LZeta,  0,1);

  LTheta = new QLabel(rad2dmsStr(Pre->theta()), W);
  LTheta->setMinimumSize(LTheta->sizeHint());
  LTheta->setAlignment(AlignRight);
  grid->addWidget(LTheta,  1,1);

  LZ = new QLabel(rad2dmsStr(Pre->z()), W);
  LZ->setMinimumSize(LZ->sizeHint());
  LZ->setAlignment(AlignRight);
  grid->addWidget(LZ,  2,1);

  LTT = new QLabel(Time->TT().toString(SBMJD::F_Short), W);
  LTT->setMinimumSize(LTT->sizeHint());
  LTT->setAlignment(AlignRight);
  grid->addWidget(LTT,  3,1);


  Layout->activate();
  return W;
};

void SBTestPrecession::recalc()
{
  SBTest::recalc();
  Time->setUTC(*Date);
  (*Pre)(*Time);
  displayValues();
};

void SBTestPrecession::displayValues()
{  
  SBTestMatrix::displayValues();
  LZeta ->setText(rad2dmsStr(Pre->zeta()));
  LTheta->setText(rad2dmsStr(Pre->theta()));
  LZ    ->setText(rad2dmsStr(Pre->z()));
  LTT   ->setText(Time->TT().toString(SBMJD::F_Short));
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBTestNutation..										*/
/*												*/
/*==============================================================================================*/
SBTestNutation::SBTestNutation(QWidget* parent, const char* name, bool modal, WFlags f)
  : SBTestMatrix(parent, name, modal, f)
{
  Weop = SetUp->loadEOP();

  TStart = new SBMJD(Weop->tStart());
  //  TFinis = new SBMJD(Weop->tFinis());
  TFinis = new SBMJD;  
  *TFinis = Weop->tFinis();

  Weop->setPtNum(Config.eopNumIntrplPoints());

  switch (Config.nutModel())
    {
    default:
    case SBConfig::NM_IAU1980:
      Nut = new SBNut_IAU1980(&Config, Weop);
      break;
    case SBConfig::NM_IAU2000:
      Nut = new SBNut_IAU2000(&Config, Weop);
      break;
    };
  
  Mat = Nut;
  Time= new SBTime;
  Time->setEOP(Weop);
  *Date= SBMJD::currentMJD(); //'cause TStart==TFinis==NULL

  if (*Date<*TStart) *Date=*TStart + 1.0;
  if (*Date>*TFinis) *Date=*TFinis - 1.0;

  setCaption(Nut->theory()->name());
  createWidgets();
};

SBTestNutation::~SBTestNutation()
{
  if (Nut) delete Nut;
  if (Time) delete Time;
  if (Weop) delete Weop;
};

QWidget* SBTestNutation::createWidget4Test(QWidget* parent)
{
  QLabel	*label;
  QGridLayout	*grid;
  QGroupBox	*W=new QGroupBox("Nutation", parent);
  QBoxLayout	*Layout=new QVBoxLayout(W, 20,10);

  Weop->prepare4Date(*Date);
  Weop->interpolateEOPs(*Date);
  Time->setUTC(*Date);
  (*Nut)(*Time);

  Layout->addWidget(createMatrixWidget(W));
  Layout->addSpacing(20);

  grid = new QGridLayout(9,2, 5);
  Layout->addLayout(grid);

  label = new QLabel("Nutation in longitude (theory):", W);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label,  0,0);

  label = new QLabel("Nutation in longitude:", W);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label,  1,0);

  label = new QLabel("Nutation in obliquity (theory):", W);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label,  2,0);

  label = new QLabel("Nutation in obliquity:", W);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label,  3,0);

  label = new QLabel("Equation of equinoxes:", W);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label,  4,0);

  label = new QLabel("Geodesic nutation:", W);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label,  5,0);

  label = new QLabel("Mean obliquity:", W);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label,  6,0);

  label = new QLabel("True obliquity:", W);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label,  7,0);

  label = new QLabel("Time (TT scale):", W);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label,  8,0);

 
  LThDeltaPsi = new QLabel(str.sprintf("%.5f (arcsec)", RAD2SEC*Nut->theory()->deltaPsi()), W);
  LThDeltaPsi->setAlignment(AlignRight);
  LThDeltaPsi->setMinimumSize(LThDeltaPsi->sizeHint());
  grid->addWidget(LThDeltaPsi,  0,1);

  LDeltaPsi = new QLabel(str.sprintf("%.5f (arcsec)", RAD2SEC*Nut->deltaPsi()), W);
  LDeltaPsi->setAlignment(AlignRight);
  LDeltaPsi->setMinimumSize(LDeltaPsi->sizeHint());
  grid->addWidget(LDeltaPsi,  1,1);

  LThDeltaEps = new QLabel(str.sprintf("%.5f (arcsec)", RAD2SEC*Nut->theory()->deltaEps()), W);
  LThDeltaEps->setAlignment(AlignRight);
  LThDeltaEps->setMinimumSize(LThDeltaEps->sizeHint());
  grid->addWidget(LThDeltaEps,  2,1);

  LDeltaEps = new QLabel(str.sprintf("%.5f (arcsec)", RAD2SEC*Nut->deltaEps()), W);
  LDeltaEps->setAlignment(AlignRight);
  LDeltaEps->setMinimumSize(LDeltaEps->sizeHint());
  grid->addWidget(LDeltaEps,  3,1);

  LEqEquinox = new QLabel(str.sprintf("%.5f (arcsec)", RAD2SEC*Nut->eqEquinox()), W);
  LEqEquinox->setAlignment(AlignRight);
  LEqEquinox->setMinimumSize(LEqEquinox->sizeHint());
  grid->addWidget(LEqEquinox,  4,1);

  LGeodesicNutation = 
    new QLabel(str.sprintf("%.5f (mas)", 1.0e3*RAD2SEC*Nut->geodesicNutation(*Time)), W);
  LGeodesicNutation->setAlignment(AlignRight);
  LGeodesicNutation->setMinimumSize(LGeodesicNutation->sizeHint());
  grid->addWidget(LGeodesicNutation,  5,1);

  LMeanEps = new QLabel(rad2dmsStr(Nut->meanEps()), W);
  LMeanEps->setAlignment(AlignRight);
  LMeanEps->setMinimumSize(LMeanEps->sizeHint());
  grid->addWidget(LMeanEps,  6,1);

  LTrueEps = new QLabel(rad2dmsStr(Nut->trueEps()), W);
  LTrueEps->setAlignment(AlignRight);
  LTrueEps->setMinimumSize(LTrueEps->sizeHint());
  grid->addWidget(LTrueEps,  7,1);
 
  LTT = new QLabel(Time->TT().toString(SBMJD::F_Short), W);
  LTT->setMinimumSize(LTT->sizeHint());
  LTT->setAlignment(AlignRight);
  grid->addWidget(LTT,  8,1);

  Layout->activate();
  return W;
};

void SBTestNutation::recalc()
{
  SBTest::recalc();
  Weop->prepare4Date(*Date);
  Weop->interpolateEOPs(*Date);
  Time->setUTC(*Date);
  (*Nut)(*Time);
  displayValues();
};

void SBTestNutation::displayValues()
{  
  SBTestMatrix::displayValues();
  LThDeltaPsi->setText(str.sprintf("%.5f (arcsec)", RAD2SEC*Nut->theory()->deltaPsi()));
  LDeltaPsi->setText(str.sprintf("%.5f (arcsec)", RAD2SEC*Nut->deltaPsi()));
  LThDeltaEps->setText(str.sprintf("%.5f (arcsec)", RAD2SEC*Nut->theory()->deltaEps()));
  LDeltaEps->setText(str.sprintf("%.5f (arcsec)", RAD2SEC*Nut->deltaEps()));
  LEqEquinox->setText(str.sprintf("%.5f (arcsec)", RAD2SEC*Nut->eqEquinox()));
  LGeodesicNutation->
    setText(str.sprintf("%.5f (mas)", 1.0e3*RAD2SEC*Nut->geodesicNutation(*Time)));
  LMeanEps->setText(rad2dmsStr(Nut->meanEps()));
  LTrueEps->setText(rad2dmsStr(Nut->trueEps()));
  LTT->setText(Time->TT().toString(SBMJD::F_Short));
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBTestFrame..										*/
/*												*/
/*==============================================================================================*/
SBTestFrame::SBTestFrame(QWidget* parent, const char* name, bool modal, WFlags f)
  : SBTestMatrix(parent, name, modal, f)
{
  switch (Config.sysTransform())
    {
    default:
    case SBConfig::STR_Classic:	Frame = new SBFrameClassic(&Config, NULL); break;
    case SBConfig::STR_NRO:	Frame = new SBFrameNRO(&Config, NULL); break; 
    };
  
  //  Frame = new SBFrameClassic(Config);
  Mat = Frame;

  TStart = new SBMJD(Frame->eop()->tStart());
  TFinis = new SBMJD(Frame->eop()->tFinis());

  *Date= SBMJD::currentMJD();
  if (*Date<*TStart) *Date=*TStart + 1.0;
  if (*Date>*TFinis) *Date=*TFinis - 1.0;

  setCaption(Frame->name());
  createWidgets();
};

SBTestFrame::~SBTestFrame()
{
  if (Frame) delete Frame;
};

QWidget* SBTestFrame::createWidget4Test(QWidget* parent)
{
  QLabel	*label;
  QGridLayout	*grid;
  QGroupBox	*W=new QGroupBox("System transformation", parent);
  QBoxLayout	*Layout=new QVBoxLayout(W, 20,10);

  Frame->eop()->prepare4Date(*Date);
  Frame->calc(*Date);

  Layout->addWidget(createMatrixWidget(W));
  Layout->addSpacing(20);

  grid = new QGridLayout(15,2, 5);
  Layout->addLayout(grid);

  //
  label = new QLabel("Time (TAI scale):", W);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label,  0,0);

  label = new QLabel("Time (TT scale):", W);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label,  1,0);

  label = new QLabel("Time (UT1 scale):", W);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label,  2,0);

  //
  label = new QLabel("Mean Anomaly of the Moon (l):", W);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label,  3,0);

  label = new QLabel("Mean Anomaly of the Sun (l'):", W);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label,  4,0);

  label = new QLabel("Mean Argument of Latitude of the Moon (F):", W);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label,  5,0);

  label = new QLabel("Mean Elongation of the Moon from the Sun (D):", W);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label,  6,0);

  label = new QLabel("Mean Longitude of the Asc.Node of the Moon (Omega):", W);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label,  7,0);

  label = new QLabel("Earth Rotational Angle (GMST+Pi):", W);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label,  8,0);

  //
  label = new QLabel("Mean Greenwich Sidereal Time:", W);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label,  9,0);

  label = new QLabel("Apparent Greenwich Sidereal Time:", W);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, 10,0);

  //
  label = new QLabel("Diurnal variations in EOP model name:", W);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, 11,0);

  label = new QLabel("Diurnal variations in X-coo of Polar Motion:", W);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, 12,0);

  label = new QLabel("Diurnal variations in Y-coo of Polar Motion:", W);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, 13,0);

  label = new QLabel("Diurnal variations in UT1:", W);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, 14,0);


  //--
  LTAI = new QLabel(Frame->time().TAI().toString(SBMJD::F_Short), W);
  LTAI->setMinimumSize(LTAI->sizeHint());
  LTAI->setAlignment(AlignRight);
  grid->addWidget(LTAI,  0,1);

  LTT = new QLabel(Frame->time().TT().toString(SBMJD::F_Short), W);
  LTT->setMinimumSize(LTT->sizeHint());
  LTT->setAlignment(AlignRight);
  grid->addWidget(LTT,  1,1);

  LUT1 = new QLabel(Frame->time().UT1().toString(SBMJD::F_Short), W);
  LUT1->setMinimumSize(LUT1->sizeHint());
  LUT1->setAlignment(AlignRight);
  grid->addWidget(LUT1,  2,1);

  //
  LArg0 = new QLabel(rad2dmslStr(Frame->time().fArg_l()), W);
  LArg0->setAlignment(AlignRight);
  LArg0->setMinimumSize(LArg0->sizeHint());
  grid->addWidget(LArg0,  3,1);

  LArg1 = new QLabel(rad2dmslStr(Frame->time().fArg_lPrime()), W);
  LArg1->setAlignment(AlignRight);
  LArg1->setMinimumSize(LArg1->sizeHint());
  grid->addWidget(LArg1,  4,1);

  LArg2 = new QLabel(rad2dmslStr(Frame->time().fArg_F()), W);
  LArg2->setAlignment(AlignRight);
  LArg2->setMinimumSize(LArg2->sizeHint());
  grid->addWidget(LArg2,  5,1);

  LArg3 = new QLabel(rad2dmslStr(Frame->time().fArg_D()), W);
  LArg3->setAlignment(AlignRight);
  LArg3->setMinimumSize(LArg3->sizeHint());
  grid->addWidget(LArg3,  6,1);

  LArg4 = new QLabel(rad2dmslStr(Frame->time().fArg_Omega()), W);
  LArg4->setAlignment(AlignRight);
  LArg4->setMinimumSize(LArg4->sizeHint());
  grid->addWidget(LArg4,  7,1);

  LArg5 = new QLabel(rad2dmslStr(Frame->time().fArg_theta()), W);
  LArg5->setAlignment(AlignRight);
  LArg5->setMinimumSize(LArg5->sizeHint());
  grid->addWidget(LArg5,  8,1);

  //
  LGMST = new QLabel(rad2dmslStr(Frame->gmst()), W);
  LGMST->setAlignment(AlignRight);
  LGMST->setMinimumSize(LGMST->sizeHint());
  grid->addWidget(LGMST,  9,1);

  LGST = new QLabel(rad2dmslStr(Frame->gst()), W);
  LGST->setAlignment(AlignRight);
  LGST->setMinimumSize(LGST->sizeHint());
  grid->addWidget(LGST, 10,1);

  //
  label = new QLabel(Frame->eopDiurnal()->name(), W);
  label->setAlignment(AlignRight);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, 11,1);

  LDiurnXP = new QLabel(str.sprintf("%.6f (mas)", 1.0e3*RAD2SEC*Frame->diurnalDX()), W);
  LDiurnXP->setAlignment(AlignRight);
  LDiurnXP->setMinimumSize(LDiurnXP->sizeHint());
  grid->addWidget(LDiurnXP, 12,1);

  LDiurnYP = new QLabel(str.sprintf("%.6f (mas)", 1.0e3*RAD2SEC*Frame->diurnalDY()), W);
  LDiurnYP->setAlignment(AlignRight);
  LDiurnYP->setMinimumSize(LDiurnYP->sizeHint());
  grid->addWidget(LDiurnYP, 13,1);

  LDiurnUT = new QLabel(str.sprintf("%.6f (mts)", 1.0e3*DAY2SEC*Frame->diurnalDUT1()), W);
  LDiurnUT->setAlignment(AlignRight);
  LDiurnUT->setMinimumSize(LDiurnUT->sizeHint());
  grid->addWidget(LDiurnUT, 14,1);

  Layout->activate();
  return W;
};

void SBTestFrame::recalc()
{
  SBTest::recalc();
  Frame->eop()->prepare4Date(*Date);
  Frame->calc(*Date);
  displayValues();
};

void SBTestFrame::displayValues()
{  
  SBTestMatrix::displayValues();

  LTAI->setText(Frame->time().TAI().toString(SBMJD::F_Short));
  LTT->setText(Frame->time().TT().toString(SBMJD::F_Short));
  LUT1->setText(Frame->time().UT1().toString(SBMJD::F_Short));
  LArg0->setText(rad2dmslStr(Frame->time().fArg_l()));
  LArg1->setText(rad2dmslStr(Frame->time().fArg_lPrime()));
  LArg2->setText(rad2dmslStr(Frame->time().fArg_F()));
  LArg3->setText(rad2dmslStr(Frame->time().fArg_D()));
  LArg4->setText(rad2dmslStr(Frame->time().fArg_Omega()));
  LArg5->setText(rad2dmslStr(Frame->time().fArg_theta()));
  LGMST->setText(rad2dmslStr(Frame->gmst()));
  LGST->setText(rad2dmslStr(Frame->gst()));
  LDiurnXP->setText(str.sprintf("%.6f (mas)", 1.0e3*RAD2SEC*Frame->diurnalDX()));
  LDiurnYP->setText(str.sprintf("%.6f (mas)", 1.0e3*RAD2SEC*Frame->diurnalDY()));
  LDiurnUT->setText(str.sprintf("%.6f (mts)", 1.0e3*DAY2SEC*Frame->diurnalDUT1()));
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBTestEphem..										*/
/*												*/
/*==============================================================================================*/
SBTestEphem::SBTestEphem(QWidget* parent, const char* name, bool modal, WFlags f)
  : SBTest(parent, name, modal, f), v0(v3Zero)
{
  Units = U_KM;
  Frame = F_CRS;
  FrameType=FT_ORTH;

  switch (Config.ephemModel())
    {
    case SBConfig::EM_LEDE200: Weph = new SBEphemDE200(&Config); break;
    default:
    case SBConfig::EM_LEDE403: Weph = new SBEphemDE403(&Config); break;
    case SBConfig::EM_LEDE405: Weph = new SBEphemDE405(&Config); break;
    };

  switch (Config.sysTransform())
    {
    default:
    case SBConfig::STR_Classic:	Wref = new SBFrameClassic(&Config, NULL); break;
    case SBConfig::STR_NRO:	Wref = new SBFrameNRO(&Config, NULL); break; 
    };
  
  setCaption("Testing " + Weph->name());
  Scale[0] = 1.0/1000.0;
  Scale[1] = 1.0/Weph->au();
  Scale[2] = 1.0;
  
  *Date  = SBMJD::currentMJD();
  TStart = new SBMJD(Weph->tStart());
  TFinis = new SBMJD(Weph->tFinis());
  if (*Date<*TStart) *Date=*TStart;
  if (*Date>*TFinis) *Date=*TFinis;
  createWidgets();
  Origin = &v0;
  RF	 = &m0;
};

SBTestEphem::~SBTestEphem()
{
  if (Weph) delete Weph;
  if (Wref) delete Wref;
};

QWidget* SBTestEphem::createWidget4Test(QWidget* parent)
{
  static const QString sUnits[] = {QString("km"), QString("AU"), QString("unit vector")};
  static const QString sFrame[] = {QString("CRF"), QString("TRF")};
  static const QString sFrameType[] = {QString("Orthogonal"), QString("Spherical")};

  QButtonGroup	*bgrp;
  QLabel	*label;
  QRadioButton	*rb;
  QGroupBox	*W=new QGroupBox("Ephemerides", parent), *gbox;
  QBoxLayout	*Layout=new QVBoxLayout(W, 20,10), *SubLayout=new QHBoxLayout, *aLayout;
  QGridLayout	*grid;

  Layout->addLayout(SubLayout);

  gbox = new QGroupBox("Coordinates", W);
  grid = new QGridLayout(gbox, 12,5,  20,5);

  label = new QLabel("Celestial body", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label,  0,0);

  LCLabel[0] = new QLabel("X", gbox);
  LCLabel[0]->setAlignment(AlignCenter);
  LCLabel[0]->setMinimumSize(LCLabel[0]->sizeHint());
  grid->addWidget(LCLabel[0],  0,2);

  LCLabel[1] = new QLabel("Y", gbox);
  LCLabel[1]->setAlignment(AlignCenter);
  LCLabel[1]->setMinimumSize(LCLabel[1]->sizeHint());
  grid->addWidget(LCLabel[1],  0,3);

  LCLabel[2] = new QLabel("Z", gbox);
  LCLabel[2]->setAlignment(AlignCenter);
  LCLabel[2]->setMinimumSize(LCLabel[2]->sizeHint());
  grid->addWidget(LCLabel[2],  0,4);

  Weph->calc(*Date);
  for (int i=0; i<11; i++)
    {
      label = new QLabel(Weph->body(i).name(), gbox);
      label->setMinimumSize(label->sizeHint());
      grid->addWidget(label,  1+i,0);

      for (int j=0; j<3; j++)
	{
	  LCoo[i][j]=
	    new QLabel(str.sprintf("%.4f", Scale[Units]*Weph->body(i).r().at((DIRECTION)j)), gbox);
	  LCoo[i][j]->setMinimumSize(LCoo[i][j]->sizeHint());
	  LCoo[i][j]->setAlignment(AlignRight | AlignVCenter);
	  grid->addWidget(LCoo[i][j],  1+i, 2+j);
	};
    };
  SubLayout->addWidget(gbox);

  bgrp = new QButtonGroup("Origin", W);
  aLayout = new QVBoxLayout(bgrp, 20,5);
  rb = new QRadioButton("SSB", bgrp);
  rb->setMinimumSize(rb->sizeHint());
  aLayout->addWidget(rb);
  for (int i=0; i<11; i++)
    {
      rb = new QRadioButton(Weph->body(i).name(), bgrp);
      rb->setMinimumSize(rb->sizeHint());
      aLayout->addWidget(rb);
    };
  bgrp->setButton(0);
  aLayout->activate();
  SubLayout->addWidget(bgrp);
  connect(bgrp, SIGNAL(clicked(int)), SLOT(originChanged(int)));
  //
  SubLayout = new QHBoxLayout;
  Layout->addLayout(SubLayout);
  //
  bgrp = new QButtonGroup("Units", W);
  aLayout = new QVBoxLayout(bgrp, 20,5);
  for (int i=0; i<2; i++)
    aLayout->addWidget(new QRadioButton(sUnits[i], bgrp));
  bgrp->setButton(0);
  aLayout->activate();
  SubLayout->addWidget(bgrp);
  connect(bgrp, SIGNAL(clicked(int)), SLOT(unitChanged(int)));
  //
  bgrp = new QButtonGroup("Frame type", W);
  aLayout = new QVBoxLayout(bgrp, 20,5);
  for (int i=0; i<2; i++) 
    aLayout->addWidget(new QRadioButton(sFrameType[i], bgrp));
  bgrp->setButton(0);
  aLayout->activate();
  SubLayout->addWidget(bgrp);
  connect(bgrp, SIGNAL(clicked(int)), SLOT(frameTypeChanged(int)));
  //
  bgrp = new QButtonGroup("Frame", W);
  aLayout = new QVBoxLayout(bgrp, 20,5);
  for (int i=0; i<2; i++) 
    aLayout->addWidget(new QRadioButton(sFrame[i], bgrp));
  bgrp->setButton(0);
  aLayout->activate();
  SubLayout->addWidget(bgrp);
  connect(bgrp, SIGNAL(clicked(int)), SLOT(frameChanged(int)));
  
  
  Layout->activate();
  return W;
};

void SBTestEphem::frameTypeChanged(int i)
{
  FrameType=(EFrameType)i;
  displayCoo();
};

void SBTestEphem::frameChanged(int i)
{
  Frame = (EFrame)i;
  switch (Frame)
    {
    case F_CRS:
      RF = &m0;
      break;
    case F_TRS:
      Wref->calc(*Date);
      RF = Wref;
      break;
    };
  displayCoo();
};

void SBTestEphem::originChanged(int i)
{
  Origin=i>0?&(Weph->body(i-1).r()):&v0;
  displayCoo();
};

void SBTestEphem::unitChanged(int i)
{
  Units=(EUnits)i;
  displayCoo();
};

void SBTestEphem::recalc()
{
  SBTest::recalc();
  if (Weph->isOK())
    {
      if (Wref->eop()->t_4Prepare().date() != Date->date())
	Wref->eop()->prepare4Date(*Date);
      Weph->calc(*Date);
      Wref->calc(*Date);
      displayCoo();
    };
};

void SBTestEphem::displayCoo()
{
  const char* sFormat[]={"%.4f","%.6f","%.1f"};
  switch (FrameType)
    {
    default:
    case FT_ORTH: 
      LCLabel[0]->setText("X");
      LCLabel[1]->setText("Y");
      LCLabel[2]->setText("Z");
      break;
    case FT_SPH: 
      LCLabel[0]->setText(Frame==F_CRS?"Right Ascension":"Hour Angle");
      LCLabel[1]->setText("Declination");
      LCLabel[2]->setText("Distance");
      break;
    };

  Vector3 tmp;
  for (int i=0; i<11; i++)
    {
      tmp = ~*RF*(Weph->body(i).r() - *Origin);
      switch (FrameType)
	{
	default:
	case FT_ORTH: 
	  for (int j=0; j<3; j++)
	    LCoo[i][j]->
	      setText(str.sprintf(sFormat[Units], Scale[Units]*tmp.at((DIRECTION)j)));
	  break;
	case FT_SPH: 
	  LCoo[i][0]-> setText(rad2hmsStr(tmp.lambda()));
	  LCoo[i][1]-> setText(rad2dmsStr(tmp.phi()));
	  LCoo[i][2]-> setText(str.sprintf(sFormat[Units], Scale[Units]*tmp.module()));
	  break;
	};
    };
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBTestEOP..											*/
/*												*/
/*==============================================================================================*/
SBTestEOP::SBTestEOP(QWidget* parent, const char* name, bool modal, WFlags f)
  : SBTest(parent, name, modal, f)
{
  setCaption("EOP testing");

  Time	= new SBTime;

  if (!(Weop = SetUp->loadEOP())) 
    Weop = new SBEOP;

  switch (Config.nutModel())
    {
    default:
    case SBConfig::NM_IAU1980: Theory = new SBNutThIAU1980; break;
    case SBConfig::NM_IAU1996: Theory = new SBNutThIAU1996; break;
    case SBConfig::NM_IAU2000: Theory = new SBNutThIAU2000A;break;
    };

  
  Weop->setPtNum((NumOfPts=Config.eopNumIntrplPoints()));

  //  Time->setEOP(Weop);

  Step = 2.0;
  NumOfSplines = 0;


  *Date  = SBMJD::currentMJD();
 
  TStart = new SBMJD(Weop->tStart());
  TFinis = new SBMJD(Weop->tFinis());
  if (*Date<*TStart) *Date=*TStart + 1.0;
  if (*Date>*TFinis) *Date=*TFinis - 1.0;
 

  PlotCarrier = new SBPlotCarrier(9, 0, "Test of EOP, solution " + Weop->label().toString());
  PlotCarrier->setFile4SaveBaseName(Weop->label().toString());

  PlotCarrier->createBranch(NumOfPts, Weop->label().toString());
  PlotCarrier->createBranch(20, "Splines");

  PlotCarrier->columnNames()->append(new QString("Time, MJD"));
  PlotCarrier->setType(0, SBPlotCarrier::AT_MJD);
  PlotCarrier->columnNames()->append(new QString("X Pole offset, as"));
  PlotCarrier->columnNames()->append(new QString("Y Pole offset, as"));
  PlotCarrier->columnNames()->append(new QString("UT1 - UTC, s"));
  PlotCarrier->columnNames()->append(new QString("LOD, s"));
  PlotCarrier->columnNames()->append(new QString("dPsi nutation, as"));
  PlotCarrier->columnNames()->append(new QString("dEps nutation, as"));

  PlotCarrier->columnNames()->append(new QString("dPsi theory, as"));
  PlotCarrier->columnNames()->append(new QString("dEps theory, as"));
  
  fillData4Plotting();
  
  createWidgets();
};

SBTestEOP::~SBTestEOP()
{

  if (Weop) 
    {
      delete Weop;
      Weop = NULL;
    };

  if (Theory)
    {
      delete Theory;
      Theory = NULL;
    };

  if (Time)
    {
      delete Time;
      Time = NULL;
    };

  if (PlotCarrier)
    {
      delete PlotCarrier;
      PlotCarrier=NULL;
    };
};

QWidget* SBTestEOP::createWidget4Test(QWidget* parent)
{
  return (Plot = new SBPlot(PlotCarrier, parent));
};

void SBTestEOP::fillData4Plotting()
{
  SBMatrix	*B;
  unsigned int	i;

  B = PlotCarrier->branches()->at(0)->data();
  if (B->nRow() != NumOfPts)
    {
      i = B->nCol();
      B->resize(NumOfPts, i);
      Weop->setPtNum(NumOfPts);
    };

  Weop->prepare4Date(*Date);

  for (i=0; i<NumOfPts; i++)
    {
      B->set(i, 0,  Weop->cache4Interpolation()->at(i, 0));
      B->set(i, 1,  Weop->cache4Interpolation()->at(i, 1));
      B->set(i, 2,  Weop->cache4Interpolation()->at(i, 2));
      B->set(i, 3,  Weop->cache4Interpolation()->at(i, 3));
      B->set(i, 4,  Weop->cache4Interpolation()->at(i, 4));
      B->set(i, 5,  Weop->cache4Interpolation()->at(i, 5));
      B->set(i, 6,  Weop->cache4Interpolation()->at(i, 6));

      Time->setUTC(Weop->cache4Interpolation()->at(i, 0));
      Theory->calc(*Time);
      B->set(i, 7,  Weop->cache4Interpolation()->at(i, 5) + Theory->deltaPsi()/SEC2RAD);
      B->set(i, 8,  Weop->cache4Interpolation()->at(i, 6) + Theory->deltaEps()/SEC2RAD);
    };


  SBMJD t = Weop->cache4Interpolation()->at(0, 0) + Date->time();
  unsigned int	Nspl = 
    (int)ceil(24.0*(Weop->cache4Interpolation()->at(NumOfPts-1, 0) - Weop->cache4Interpolation()->at(0, 0))
	      /Step);

  NumOfSplines = Nspl;

  B = PlotCarrier->branches()->at(1)->data();
  if (B->nRow() != Nspl)
    {
      i = B->nCol();
      B->resize(Nspl, i);
    };
  
  for (i=0; i<Nspl; i++, t+= Step/24.0)
    {
      Weop->interpolateEOPs(t);
      
      B->set(i, 0,  t);
      B->set(i, 1,  Weop->dX(t)/SEC2RAD);
      B->set(i, 2,  Weop->dY(t)/SEC2RAD);
      B->set(i, 3,  Weop->dUT1_UTC(t)*DAY2SEC);
      B->set(i, 4,  Weop->dLOD(t)*DAY2SEC);
      B->set(i, 5,  Weop->dPsi(t)/SEC2RAD);
      B->set(i, 6,  Weop->dEps(t)/SEC2RAD);

      Time->setUTC(t);
      Theory->calc(*Time);
      B->set(i, 7,  Theory->deltaPsi()/SEC2RAD);
      B->set(i, 8,  Theory->deltaEps()/SEC2RAD);
    };
};

QWidget* SBTestEOP::auxWidgetRt(QWidget* parent)
{
  QLabel	*label;
  QGroupBox	*gbox;
  QGridLayout	*grid;
  
  gbox = new QGroupBox("Splines", parent);
  grid = new QGridLayout(gbox, 3,2, 20,5);

  label	= new QLabel("Number of points:", gbox);
  label	-> setMinimumSize(label->sizeHint());
  grid  -> addWidget(label, 0,0);
  ENumPts = new QLineEdit(gbox);
  ENumPts -> setText(QString().setNum(NumOfPts));
  ENumPts -> setMinimumHeight(ENumPts->sizeHint().height());
  ENumPts -> setMinimumWidth(ENumPts->fontMetrics().width("2000"));
  grid  -> addWidget(ENumPts, 0,1);

  label	= new QLabel("Step (hr):", gbox);
  label	-> setMinimumSize(label->sizeHint());
  grid  -> addWidget(label, 1,0);
  EStep = new QLineEdit(gbox);
  EStep -> setText(QString().setNum(Step));
  EStep -> setMinimumHeight(EStep->sizeHint().height());
  EStep -> setMinimumWidth(EStep->fontMetrics().width("3.00"));
  grid  -> addWidget(EStep, 1,1);

  label	= new QLabel("Num. of interp.:", gbox);
  label	-> setMinimumSize(label->sizeHint());
  grid  -> addWidget(label, 2,0);
  LNumOfSplines = new QLabel(QString().setNum(NumOfSplines), gbox);
  LNumOfSplines -> setMinimumSize(LNumOfSplines->sizeHint());
  grid  -> addWidget(LNumOfSplines, 2,1);

  grid  -> activate();
  return gbox;
};

void SBTestEOP::recalc()
{
  SBTest::recalc();
  
  unsigned int	i;
  double	f;
  bool		Is;

  i = ENumPts->text().toUInt(&Is);
  if (Is && i != NumOfPts && i>4)
    {
      NumOfPts = i;
      IsDateModified=TRUE;
    };
  
  f = EStep->text().toDouble(&Is);
  if (Is && f != Step)
    {
      Step = std::max(0.01,f);
      IsDateModified=TRUE;
    };

  if (IsDateModified)
    {
      fillData4Plotting();
      LNumOfSplines->setText(QString().setNum(NumOfSplines));
      Plot->dataChanged();
    };
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBTestDiurnEOP..										*/
/*												*/
/*==============================================================================================*/
SBTestDiurnEOP::SBTestDiurnEOP(QWidget* parent, const char* name, bool modal, WFlags f)
  : SBTest(parent, name, modal, f)
{
  setCaption("Diurnal EOP testing");

  NumOfModels	= 6+3;
  dEOPs[0] = deopRay1994	= new SBOTideEOPRay1994_1996;
  dEOPs[1] = deopRay2003	= new SBOTideEOPRay1994_2003;
  dEOPs[2] = deopJPL92		= new SBOTideEOP_JPL92;
  dEOPs[3] = deopHD1994		= new SBOTideEOP_HD1994;
  dEOPs[4] = deopGSFC95		= new SBOTideEOP_GSFC95;
  dEOPs[5] = deopGipson1995	= new SBOTideEOP_Gipson1995;

  dEOPs[6] = deopAtm		= new SBATideEOP_BBP_02;
  dEOPs[7] = deopNonTide	= new SBNonTideEOP_Viron_02;
  dEOPs[8] = deopGrav		= new SBGravTideEOP_B_02;
  

  NumOfPts	= 12;
  Step		= 2.0;

  Time	= new SBTime;
  *Date = SBMJD::currentMJD();
 

  PlotCarrier = new SBPlotCarrier(4, 0, "Test of diurnal EOP");
  PlotCarrier->setFile4SaveBaseName("DiurnalEOPs");

  for (unsigned int j=0; j<NumOfModels; j++)
    PlotCarrier->createBranch(NumOfPts, dEOPs[j]->name());

  PlotCarrier->columnNames()->append(new QString("Time, MJD"));
  PlotCarrier->setType(0, SBPlotCarrier::AT_MJD);
  PlotCarrier->columnNames()->append(new QString("X Pole offset, mas"));
  PlotCarrier->columnNames()->append(new QString("Y Pole offset, mas"));
  PlotCarrier->columnNames()->append(new QString("UT1 - UTC, ms"));
  
  fillData4Plotting();
  
  createWidgets();
};

SBTestDiurnEOP::~SBTestDiurnEOP()
{

  if (Time) 
    {
      delete Time;
      Time = NULL;
    };

  if (deopRay1994) 
    {
      delete deopRay1994;
      deopRay1994 = NULL;
    };
  if (deopRay2003) 
    {
      delete deopRay2003;
      deopRay2003 = NULL;
    };
  if (deopJPL92) 
    {
      delete deopJPL92;
      deopJPL92 = NULL;
    };
  if (deopHD1994) 
    {
      delete deopHD1994;
      deopHD1994 = NULL;
    };
  if (deopGSFC95) 
    {
      delete deopGSFC95;
      deopGSFC95 = NULL;
    };
  if (deopGipson1995) 
    {
      delete deopGipson1995;
      deopGipson1995 = NULL;
    };

  if (deopAtm) 
    {
      delete deopAtm;
      deopAtm = NULL;
    };
  if (deopNonTide) 
    {
      delete deopNonTide;
      deopNonTide = NULL;
    };
  if (deopGrav) 
    {
      delete deopGrav;
      deopGrav = NULL;
    };

  if (PlotCarrier)
    {
      delete PlotCarrier;
      PlotCarrier=NULL;
    };
};

QWidget* SBTestDiurnEOP::createWidget4Test(QWidget* parent)
{
  return (Plot = new SBPlot(PlotCarrier, parent));
};

void SBTestDiurnEOP::fillData4Plotting()
{
  SBMatrix	*B;
  unsigned int	i, j;

  
  for (j=0; j<NumOfModels; j++)
    {
      B = PlotCarrier->branches()->at(j)->data();
      if (B->nRow() != NumOfPts)
	{
	  i = B->nCol();
	  B->resize(NumOfPts, i);
	};
    };

  
  SBMJD t = *Date - NumOfPts*Step/24.0/2.0;

  for (i=0; i<NumOfPts; i++, t+= Step/24.0)
    {
      for (j=0; j<NumOfModels; j++)
	{
	  B = PlotCarrier->findBranch(dEOPs[j]->name())->data();

	  Time->setUTC(t);
	  dEOPs[j]->calc(*Time);
      
	  B->set(i, 0,  t);
	  B->set(i, 1,  dEOPs[j]->dx_xD()/SEC2RAD*1.0e3);
	  B->set(i, 2,  dEOPs[j]->dy_yD()/SEC2RAD*1.0e3);
	  B->set(i, 3,  dEOPs[j]->dUT1_UT1D()*DAY2SEC*1.0e3);
	};
    };

};

QWidget* SBTestDiurnEOP::auxWidgetRt(QWidget* parent)
{
  QLabel	*label;
  QGroupBox	*gbox;
  QGridLayout	*grid;
  
  gbox = new QGroupBox("Splines", parent);
  grid = new QGridLayout(gbox, 2,2, 20,5);

  label	= new QLabel("Number of points:", gbox);
  label	-> setMinimumSize(label->sizeHint());
  grid  -> addWidget(label, 0,0);
  ENumPts = new QLineEdit(gbox);
  ENumPts -> setText(QString().setNum(NumOfPts));
  ENumPts -> setMinimumHeight(ENumPts->sizeHint().height());
  ENumPts -> setMinimumWidth(ENumPts->fontMetrics().width("2000"));
  grid  -> addWidget(ENumPts, 0,1);

  label	= new QLabel("Step (hr):", gbox);
  label	-> setMinimumSize(label->sizeHint());
  grid  -> addWidget(label, 1,0);
  EStep = new QLineEdit(gbox);
  EStep -> setText(QString().setNum(Step));
  EStep -> setMinimumHeight(EStep->sizeHint().height());
  EStep -> setMinimumWidth(EStep->fontMetrics().width("3.00"));
  grid  -> addWidget(EStep, 1,1);

  grid  -> activate();
  return gbox;
};

void SBTestDiurnEOP::recalc()
{
  SBTest::recalc();
  
  unsigned int	i;
  double	f;
  bool		Is;

  i = ENumPts->text().toUInt(&Is);
  if (Is && i != NumOfPts && i>4)
    {
      NumOfPts = i;
      IsDateModified=TRUE;
    };
  
  f = EStep->text().toDouble(&Is);
  if (Is && f != Step)
    {
      Step = std::max(0.01,f);
      IsDateModified=TRUE;
    };

  if (IsDateModified)
    {
      fillData4Plotting();
      Plot->dataChanged();
    };
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBTestStationLI..										*/
/*												*/
/*==============================================================================================*/
QString SBTestStationLI::text(int col) const
{
  TmpString = "NULL";
  if (Station)
    switch (col)
      {
      case 0: TmpString = Station->id().toString();
	break;
      case 1: TmpString = Station->name();
	break;
      case 2: TmpString = SBSolutionName::techAbbrev(Station->tech());
	break;
      };
  return TmpString;
};

QString SBTestStationLI::key(int col, bool) const
{
  TmpString = "NULL";
  if (Station)
    switch (col)
      {
      case 0: TmpString = Station->id().toString();
	break;
      case 1: TmpString = Station->name();
	break;
      case 2: TmpString = SBSolutionName::techAbbrev(Station->tech());
	break;
      };
  return TmpString;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBTestSolidTides..										*/
/*												*/
/*==============================================================================================*/
SBTestSolidTides::SBTestSolidTides(QWidget* parent, const char* name, bool modal, WFlags f)
  : SBTest(parent, name, modal, f)
{
  setCaption("Test of Solid Earth Tide");

  VectorNames[0] = "Total";
  VectorNames[1] = "Degree 2(ip)";
  VectorNames[2] = "Degree 3(ip)";
  VectorNames[3] = "Degree 2(op)";
  VectorNames[4] = "Lat. depend";
  VectorNames[5] = "Step_1";
  VectorNames[6] = "Step_2";


  NumOfVecotors	= 7;
  NumOfPts	= 24;
  Step		= 2.0;

  for (unsigned int i=0; i<NumOfVecotors; i++)
    Vectors[i] = v3Zero;
  

  *Date = SBMJD::currentMJD();

  switch (Config.sysTransform())
    {
    default:
    case SBConfig::STR_Classic:	Frame = new SBFrameClassic(&Config, NULL); break;
    case SBConfig::STR_NRO:	Frame = new SBFrameNRO(&Config, NULL); break; 
    };
  
  Frame->eop()->setPtNum(NumOfPts);

  switch (Config.ephemModel())
    {
    case SBConfig::EM_LEDE200:	Ephem = new SBEphemDE200(&Config); break;
    default:
    case SBConfig::EM_LEDE403:	Ephem = new SBEphemDE403(&Config); break;
    case SBConfig::EM_LEDE405:	Ephem = new SBEphemDE405(&Config); break;
    };
  
  switch (Config.solidTideLd())
    {
    case SBConfig::ST_IERS92: 
      SolidTide = new SBSolidTideLdIERS92(&Config);
      break;
    default:
    case SBConfig::ST_IERS96: 
      SolidTide = new SBSolidTideLdIERS96(&Config);
      break;
    };
  
  TRF = SetUp->loadTRF();
  TRF->prepare4Run();
  Station = TRF->stations()->first();


  TStart = new SBMJD(Ephem->tStart());
  TFinis = new SBMJD(Ephem->tFinis());
  if (*TStart<Frame->eop()->tStart())
    *TStart = Frame->eop()->tStart();
  if (*TFinis>Frame->eop()->tFinis())
    *TFinis = Frame->eop()->tFinis();
  if (*Date<*TStart) *Date=*TStart + 8.0;
  if (*Date>*TFinis) *Date=*TFinis - 8.0;
   

  PlotCarrier = new SBPlotCarrier(7, 0, "Test of Solid Tides for the " + (Station?Station->name():"UNKNOWN"));
  PlotCarrier->setFile4SaveBaseName("SolidTide");

  for (unsigned int i=0; i<NumOfVecotors; i++)
    PlotCarrier->createBranch(NumOfPts, VectorNames[i]);

  PlotCarrier->columnNames()->append(new QString("Time, MJD"));
  PlotCarrier->setType(0, SBPlotCarrier::AT_MJD);
  PlotCarrier->columnNames()->append(new QString("X-coord, cm"));
  PlotCarrier->columnNames()->append(new QString("Y-coord, cm"));
  PlotCarrier->columnNames()->append(new QString("Z-coord, cm"));
  PlotCarrier->columnNames()->append(new QString("Up, cm"));
  PlotCarrier->columnNames()->append(new QString("East, cm"));
  PlotCarrier->columnNames()->append(new QString("North, cm"));
  
  createWidgets();
  fillData4Plotting();
};

SBTestSolidTides::~SBTestSolidTides()
{
  if (Frame) 
    {
      delete Frame;
      Frame = NULL;
    };

  if (Ephem) 
    {
      delete Ephem;
      Ephem = NULL;
    };

  if (SolidTide) 
    {
      delete SolidTide;
      SolidTide = NULL;
    };

  if (TRF)
    {
      TRF->finisRun();
      delete TRF;
      TRF = NULL;
    };

  if (PlotCarrier)
    {
      delete PlotCarrier;
      PlotCarrier=NULL;
    };
};

QWidget* SBTestSolidTides::createWidget4Test(QWidget* parent)
{
  return (Plot = new SBPlot(PlotCarrier, parent));
};

void SBTestSolidTides::fillData4Plotting()
{
  SBMatrix	*B;
  unsigned int	i, j;
  Vector3	TideVEN[7];
  Matrix3	ToVEN;
  bool		IsGood = FALSE;
  
  for (j=0; j<NumOfVecotors; j++)
    {
      TideVEN[j] = Vectors[j] = v3Zero;
      B = PlotCarrier->branches()->at(j)->data();
      if (B->nRow() != NumOfPts)
	{
	  i = B->nCol();
	  B->resize(NumOfPts, i);
	};
    };

  
  Frame->eop()->prepare4Date(*Date);

  if (Station && Station->rt()!=v3Zero)
    {
      IsGood = TRUE;
      ToVEN  = ~Station->fmVEN();
    };
  
  SBMJD t = *Date - NumOfPts*Step/24.0/2.0;
  for (i=0; i<NumOfPts; i++, t+= Step/24.0)
    {
      if (IsGood)
	{
	  Frame->calc(t);
	  Ephem->calc(Frame->time().TDB());
	  Vectors[0] = (*SolidTide)(Station, Ephem, *Frame);
	  Vectors[1] = SolidTide->tideDeg2ip();
	  Vectors[2] = SolidTide->tideDeg3ip();
	  Vectors[3] = SolidTide->tideDeg2op();
	  Vectors[4] = SolidTide->tideLatDepend();
	  Vectors[5] = SolidTide->step1();
	  Vectors[6] = SolidTide->step2();
	  
	  for (j=0; j<NumOfVecotors; j++)
	    TideVEN[j] = ToVEN*Vectors[j];
	};
      for (j=0; j<NumOfVecotors; j++)
	{
	  B = PlotCarrier->findBranch(VectorNames[j])->data();

	  B->set(i, 0,  t);
	  B->set(i, 1,  Vectors[j].at(X_AXIS)*1.0e2);
	  B->set(i, 2,  Vectors[j].at(Y_AXIS)*1.0e2);
	  B->set(i, 3,  Vectors[j].at(Z_AXIS)*1.0e2);

	  B->set(i, 4,  TideVEN[j].at(VERT)*1.0e2);
	  B->set(i, 5,  TideVEN[j].at(EAST)*1.0e2);
	  B->set(i, 6,  TideVEN[j].at(NORT)*1.0e2);
	};
    };
};

QWidget* SBTestSolidTides::auxWidgetRt(QWidget* parent)
{
  QWidget	*Aux = new QWidget(parent);
  QBoxLayout	*Layout = new QHBoxLayout(Aux, 0, 10), *SubLayout;

  QLabel	*label;
  QGroupBox	*gbox;
  QGridLayout	*grid;
  
  gbox = new QGroupBox("Draw Points", Aux);
  grid = new QGridLayout(gbox, 2,2, 20,5);

  label	= new QLabel("Number of points:", gbox);
  label	-> setMinimumSize(label->sizeHint());
  grid  -> addWidget(label, 0,0);
  ENumPts = new QLineEdit(gbox);
  ENumPts -> setText(QString().setNum(NumOfPts));
  ENumPts -> setMinimumHeight(ENumPts->sizeHint().height());
  ENumPts -> setMinimumWidth(ENumPts->fontMetrics().width("99999"));
  grid  -> addWidget(ENumPts, 0,1);

  label	= new QLabel("Step (hr):", gbox);
  label	-> setMinimumSize(label->sizeHint());
  grid  -> addWidget(label, 1,0);
  EStep = new QLineEdit(gbox);
  EStep -> setText(QString().setNum(Step));
  EStep -> setMinimumHeight(EStep->sizeHint().height());
  EStep -> setMinimumWidth(EStep->fontMetrics().width("999.999"));
  grid  -> addWidget(EStep, 1,1);
  grid  -> activate();
  Layout->addWidget(gbox);


  SubLayout = new QVBoxLayout(Layout);
  StLV = new QListView(Aux);
  StLV -> addColumn("DOMES");
  StLV -> setColumnAlignment( 0, AlignLeft);
  StLV -> addColumn("Station name");
  StLV -> setColumnAlignment( 1, AlignLeft);
  StLV -> addColumn("Tech");
  StLV -> setColumnAlignment( 2, AlignLeft);
  
  for (SBStation *St=TRF->stations()->first(); St; St=TRF->stations()->next())
    (void) new SBTestStationLI(StLV, St);

  StLV -> setAllColumnsShowFocus(TRUE);
  //  StLV -> setMinimumSize(StLV->sizeHint());
  StLV->setMinimumWidth(StLV->sizeHint().width());
  StLV -> setMinimumHeight(40);
  StLV -> setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  if (StLV->firstChild())
    {
      StLV->setSelected(StLV->firstChild(), TRUE);
      StLV->setMaximumHeight(6*StLV->firstChild()->height());
    };
  StLV->setFocusPolicy(QWidget::StrongFocus);
  SubLayout->addWidget(StLV, 0);

  connect(StLV, SIGNAL(selectionChanged(QListViewItem*)), SLOT(stationChange(QListViewItem*)));

  Layout->activate();
  return Aux;
};

void SBTestSolidTides::recalc()
{
  SBTest::recalc();
  
  unsigned int	i;
  double	f;
  bool		Is;

  i = ENumPts->text().toUInt(&Is);
  if (Is && i != NumOfPts && i>4)
    {
      NumOfPts = i;
      IsDateModified=TRUE;
      Frame->eop()->setPtNum(NumOfPts);
    };
  
  f = EStep->text().toDouble(&Is);
  if (Is && f != Step)
    {
      Step = std::max(0.01,f);
      IsDateModified=TRUE;
    };

  if (IsDateModified)
    {
      fillData4Plotting();
      Plot->dataChanged();
    };
};

void SBTestSolidTides::stationChange(QListViewItem *LI)
{
  SBStation		*st=((SBTestStationLI*)LI)->station();

  if (st && Station != st)
    {
      Station = st;
      PlotCarrier->setName("Test of Solid Tides for the " + (Station?Station->name():"UNKNOWN"));
      recalc();
      fillData4Plotting();
      Plot->dataChanged();
    };
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBTestOceanTides..										*/
/*												*/
/*==============================================================================================*/
SBTestOceanTides::SBTestOceanTides(QWidget* parent, const char* name, bool modal, WFlags f)
  : SBTest(parent, name, modal, f)
{
  setCaption("Test of Ocean Tide Loading");

  VectorNames[0] = "Displacement";

  NumOfVecotors	= 1;
  NumOfPts	= 24;
  Step		= 2.0;

  for (unsigned int i=0; i<NumOfVecotors; i++)
    Vectors[i] = v3Zero;
  

  *Date = SBMJD::currentMJD();

  switch (Config.sysTransform())
    {
    default:
    case SBConfig::STR_Classic:	Frame = new SBFrameClassic(&Config, NULL); break;
    case SBConfig::STR_NRO:	Frame = new SBFrameNRO(&Config, NULL); break; 
    };
  
  Frame->eop()->setPtNum(NumOfPts);

  OceanTide = new SBOceanTideLd(&Config);
  
  TRF = SetUp->loadTRF();
  TRF->prepare4Run();
  Station = TRF->stations()->first();


  TStart = new SBMJD(Frame->eop()->tStart());
  TFinis = new SBMJD(Frame->eop()->tFinis());

  if (*Date<*TStart) *Date=*TStart + 8.0;
  if (*Date>*TFinis) *Date=*TFinis - 8.0;
   

  PlotCarrier = new SBPlotCarrier(4, 0, "Test of Ocean Tides for the " + (Station?Station->name():"UNKNOWN"));
  PlotCarrier->setFile4SaveBaseName("OceanTide");

  for (unsigned int i=0; i<NumOfVecotors; i++)
    PlotCarrier->createBranch(NumOfPts, VectorNames[i]);

  PlotCarrier->columnNames()->append(new QString("Time, MJD"));
  PlotCarrier->setType(0, SBPlotCarrier::AT_MJD);
  PlotCarrier->columnNames()->append(new QString("Up, cm"));
  PlotCarrier->columnNames()->append(new QString("East, cm"));
  PlotCarrier->columnNames()->append(new QString("North, cm"));
  
  createWidgets();
  fillData4Plotting();
};

SBTestOceanTides::~SBTestOceanTides()
{
  if (Frame) 
    {
      delete Frame;
      Frame = NULL;
    };

  if (OceanTide)
    {
      delete OceanTide;
      OceanTide = NULL;
    };

  if (TRF)
    {
      TRF->finisRun();
      delete TRF;
      TRF = NULL;
    };

  if (PlotCarrier)
    {
      delete PlotCarrier;
      PlotCarrier=NULL;
    };
};

QWidget* SBTestOceanTides::createWidget4Test(QWidget* parent)
{
  return (Plot = new SBPlot(PlotCarrier, parent));
};

void SBTestOceanTides::fillData4Plotting()
{
  SBMatrix	*B;
  unsigned int	i, j;
  
  for (j=0; j<NumOfVecotors; j++)
    {
      Vectors[j] = v3Zero;
      B = PlotCarrier->branches()->at(j)->data();
      if (B->nRow() != NumOfPts)
	{
	  i = B->nCol();
	  B->resize(NumOfPts, i);
	};
    };
  
  Frame->eop()->prepare4Date(*Date);
  SBMJD t = *Date - NumOfPts*Step/24.0/2.0;

  for (i=0; i<NumOfPts; i++, t+= Step/24.0)
    {
      if (Station && Station->rt()!=v3Zero)
	{
	  Frame->calc(t);
	  Vectors[0] = (*OceanTide)(Station->oLoad()?Station->oLoad():&Station->site()->oLoad(), Frame->time());
	};
      for (j=0; j<NumOfVecotors; j++)
	{
	  B = PlotCarrier->findBranch(VectorNames[j])->data();

	  B->set(i, 0,  t);
	  B->set(i, 1,  Vectors[j].at(VERT)*1.0e2);
	  B->set(i, 2,  Vectors[j].at(EAST)*1.0e2);
	  B->set(i, 3,  Vectors[j].at(NORT)*1.0e2);
	};
    };
};

QWidget* SBTestOceanTides::auxWidgetRt(QWidget* parent)
{
  QWidget	*Aux = new QWidget(parent);
  QBoxLayout	*Layout = new QHBoxLayout(Aux, 0, 10), *SubLayout;

  QLabel	*label;
  QGroupBox	*gbox;
  QGridLayout	*grid;
  
  gbox = new QGroupBox("Draw Points", Aux);
  grid = new QGridLayout(gbox, 2,2, 20,5);

  label	= new QLabel("Number of points:", gbox);
  label	-> setMinimumSize(label->sizeHint());
  grid  -> addWidget(label, 0,0);
  ENumPts = new QLineEdit(gbox);
  ENumPts -> setText(QString().setNum(NumOfPts));
  ENumPts -> setMinimumHeight(ENumPts->sizeHint().height());
  ENumPts -> setMinimumWidth(ENumPts->fontMetrics().width("99999"));
  grid  -> addWidget(ENumPts, 0,1);

  label	= new QLabel("Step (hr):", gbox);
  label	-> setMinimumSize(label->sizeHint());
  grid  -> addWidget(label, 1,0);
  EStep = new QLineEdit(gbox);
  EStep -> setText(QString().setNum(Step));
  EStep -> setMinimumHeight(EStep->sizeHint().height());
  EStep -> setMinimumWidth(EStep->fontMetrics().width("999.999"));
  grid  -> addWidget(EStep, 1,1);
  grid  -> activate();
  Layout->addWidget(gbox);


  SubLayout = new QVBoxLayout(Layout);
  StLV = new QListView(Aux);
  StLV -> addColumn("DOMES");
  StLV -> setColumnAlignment( 0, AlignLeft);
  StLV -> addColumn("Station name");
  StLV -> setColumnAlignment( 1, AlignLeft);
  StLV -> addColumn("Tech");
  StLV -> setColumnAlignment( 2, AlignLeft);
  
  for (SBStation *St=TRF->stations()->first(); St; St=TRF->stations()->next())
    (void) new SBTestStationLI(StLV, St);

  StLV -> setAllColumnsShowFocus(TRUE);
  //  StLV -> setMinimumSize(StLV->sizeHint());
  StLV->setMinimumWidth(StLV->sizeHint().width());
  StLV -> setMinimumHeight(40);
  StLV -> setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  if (StLV->firstChild())
    {
      StLV->setSelected(StLV->firstChild(), TRUE);
      StLV->setMaximumHeight(6*StLV->firstChild()->height());
    };
  StLV->setFocusPolicy(QWidget::StrongFocus);
  SubLayout->addWidget(StLV, 0);

  connect(StLV, SIGNAL(selectionChanged(QListViewItem*)), SLOT(stationChange(QListViewItem*)));

  Layout->activate();
  return Aux;
};

void SBTestOceanTides::recalc()
{
  SBTest::recalc();
  
  unsigned int	i;
  double	f;
  bool		Is;

  i = ENumPts->text().toUInt(&Is);
  if (Is && i != NumOfPts && i>4)
    {
      NumOfPts = i;
      IsDateModified=TRUE;
      Frame->eop()->setPtNum(NumOfPts);
    };
  
  f = EStep->text().toDouble(&Is);
  if (Is && f != Step)
    {
      Step = std::max(0.01,f);
      IsDateModified=TRUE;
    };

  if (IsDateModified)
    {
      fillData4Plotting();
      Plot->dataChanged();
    };
};

void SBTestOceanTides::stationChange(QListViewItem *LI)
{
  SBStation		*st=((SBTestStationLI*)LI)->station();

  if (st && Station != st)
    {
      Station = st;
      PlotCarrier->setName("Test of Ocean Tides for the " + (Station?Station->name():"UNKNOWN"));
      recalc();
      fillData4Plotting();
      Plot->dataChanged();
    };
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBTestPolarTides..										*/
/*												*/
/*==============================================================================================*/
SBTestPolarTides::SBTestPolarTides(QWidget* parent, const char* name, bool modal, WFlags f)
  : SBTest(parent, name, modal, f)
{
  setCaption("Test of Polar Tide");

  VectorNames[0] = "Displacement";

  NumOfVecotors	= 1;
  NumOfPts	= 24;
  Step		= 2.0;

  for (unsigned int i=0; i<NumOfVecotors; i++)
    Vectors[i] = v3Zero;
  

  *Date = SBMJD::currentMJD();

  switch (Config.sysTransform())
    {
    default:
    case SBConfig::STR_Classic:	Frame = new SBFrameClassic(&Config, NULL); break;
    case SBConfig::STR_NRO:	Frame = new SBFrameNRO(&Config, NULL); break; 
    };
  
  Frame->eop()->setPtNum(NumOfPts);

  PolarTide = new SBPolarTideLd(&Config);
  
  TRF = SetUp->loadTRF();
  TRF->prepare4Run();
  Station = TRF->stations()->first();


  TStart = new SBMJD(Frame->eop()->tStart());
  TFinis = new SBMJD(Frame->eop()->tFinis());

  if (*Date<*TStart) *Date=*TStart + 8.0;
  if (*Date>*TFinis) *Date=*TFinis - 8.0;
   
  PlotCarrier = new SBPlotCarrier(4, 0, "Test of Polar Tides for the " + (Station?Station->name():"UNKNOWN"));
  PlotCarrier->setFile4SaveBaseName("OceanTide");

  for (unsigned int i=0; i<NumOfVecotors; i++)
    PlotCarrier->createBranch(NumOfPts, VectorNames[i]);

  PlotCarrier->columnNames()->append(new QString("Time, MJD"));
  PlotCarrier->setType(0, SBPlotCarrier::AT_MJD);
  PlotCarrier->columnNames()->append(new QString("Up, cm"));
  PlotCarrier->columnNames()->append(new QString("East, cm"));
  PlotCarrier->columnNames()->append(new QString("North, cm"));
  
  createWidgets();
  fillData4Plotting();
};

SBTestPolarTides::~SBTestPolarTides()
{
  if (Frame) 
    {
      delete Frame;
      Frame = NULL;
    };

  if (PolarTide)
    {
      delete PolarTide;
      PolarTide = NULL;
    };

  if (TRF)
    {
      TRF->finisRun();
      delete TRF;
      TRF = NULL;
    };

  if (PlotCarrier)
    {
      delete PlotCarrier;
      PlotCarrier=NULL;
    };
};

QWidget* SBTestPolarTides::createWidget4Test(QWidget* parent)
{
  return (Plot = new SBPlot(PlotCarrier, parent));
};

void SBTestPolarTides::fillData4Plotting()
{
  SBMatrix	*B;
  unsigned int	i, j;
  
  for (j=0; j<NumOfVecotors; j++)
    {
      Vectors[j] = v3Zero;
      B = PlotCarrier->branches()->at(j)->data();
      if (B->nRow() != NumOfPts)
	{
	  i = B->nCol();
	  B->resize(NumOfPts, i);
	};
    };
  
  Frame->eop()->prepare4Date(*Date);
  SBMJD t = *Date - NumOfPts*Step/24.0/2.0;

  for (i=0; i<NumOfPts; i++, t+= Step/24.0)
    {
      if (Station && Station->rt()!=v3Zero)
	{
	  Frame->calc(t);
	  Vectors[0] = (*PolarTide)(Station, Frame->eop(), Frame->time());
	};
      for (j=0; j<NumOfVecotors; j++)
	{
	  B = PlotCarrier->findBranch(VectorNames[j])->data();

	  B->set(i, 0,  t);
	  B->set(i, 1,  Vectors[j].at(VERT)*1.0e2);
	  B->set(i, 2,  Vectors[j].at(EAST)*1.0e2);
	  B->set(i, 3,  Vectors[j].at(NORT)*1.0e2);
	};
    };
};

QWidget* SBTestPolarTides::auxWidgetRt(QWidget* parent)
{
  QWidget	*Aux = new QWidget(parent);
  QBoxLayout	*Layout = new QHBoxLayout(Aux, 0, 10), *SubLayout;

  QLabel	*label;
  QGroupBox	*gbox;
  QGridLayout	*grid;
  
  gbox = new QGroupBox("Draw Points", Aux);
  grid = new QGridLayout(gbox, 2,2, 20,5);

  label	= new QLabel("Number of points:", gbox);
  label	-> setMinimumSize(label->sizeHint());
  grid  -> addWidget(label, 0,0);
  ENumPts = new QLineEdit(gbox);
  ENumPts -> setText(QString().setNum(NumOfPts));
  ENumPts -> setMinimumHeight(ENumPts->sizeHint().height());
  ENumPts -> setMinimumWidth(ENumPts->fontMetrics().width("99999"));
  grid  -> addWidget(ENumPts, 0,1);

  label	= new QLabel("Step (hr):", gbox);
  label	-> setMinimumSize(label->sizeHint());
  grid  -> addWidget(label, 1,0);
  EStep = new QLineEdit(gbox);
  EStep -> setText(QString().setNum(Step));
  EStep -> setMinimumHeight(EStep->sizeHint().height());
  EStep -> setMinimumWidth(EStep->fontMetrics().width("999.999"));
  grid  -> addWidget(EStep, 1,1);
  grid  -> activate();
  Layout->addWidget(gbox);


  SubLayout = new QVBoxLayout(Layout);
  StLV = new QListView(Aux);
  StLV -> addColumn("DOMES");
  StLV -> setColumnAlignment( 0, AlignLeft);
  StLV -> addColumn("Station name");
  StLV -> setColumnAlignment( 1, AlignLeft);
  StLV -> addColumn("Tech");
  StLV -> setColumnAlignment( 2, AlignLeft);
  
  for (SBStation *St=TRF->stations()->first(); St; St=TRF->stations()->next())
    (void) new SBTestStationLI(StLV, St);

  StLV -> setAllColumnsShowFocus(TRUE);
  //  StLV -> setMinimumSize(StLV->sizeHint());
  StLV->setMinimumWidth(StLV->sizeHint().width());
  StLV -> setMinimumHeight(40);
  StLV -> setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  if (StLV->firstChild())
    {
      StLV->setSelected(StLV->firstChild(), TRUE);
      StLV->setMaximumHeight(6*StLV->firstChild()->height());
    };
  StLV->setFocusPolicy(QWidget::StrongFocus);
  SubLayout->addWidget(StLV, 0);

  connect(StLV, SIGNAL(selectionChanged(QListViewItem*)), SLOT(stationChange(QListViewItem*)));

  Layout->activate();
  return Aux;
};

void SBTestPolarTides::recalc()
{
  SBTest::recalc();
  
  unsigned int	i;
  double	f;
  bool		Is;

  i = ENumPts->text().toUInt(&Is);
  if (Is && i != NumOfPts && i>4)
    {
      NumOfPts = i;
      IsDateModified=TRUE;
      Frame->eop()->setPtNum(NumOfPts);
    };
  
  f = EStep->text().toDouble(&Is);
  if (Is && f != Step)
    {
      Step = std::max(0.01,f);
      IsDateModified=TRUE;
    };

  if (IsDateModified)
    {
      fillData4Plotting();
      Plot->dataChanged();
    };
};

void SBTestPolarTides::stationChange(QListViewItem *LI)
{
  SBStation		*st=((SBTestStationLI*)LI)->station();

  if (st && Station != st)
    {
      Station = st;
      PlotCarrier->setName("Test of Ocean Tides for the " + (Station?Station->name():"UNKNOWN"));
      recalc();
      fillData4Plotting();
      Plot->dataChanged();
    };
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBTestAPLoad..										*/
/*												*/
/*==============================================================================================*/
SBTestAPLoad::SBTestAPLoad(QWidget* parent, const char* name, bool modal, WFlags f)
  : SBTest(parent, name, modal, f)
{
  setCaption("Test of Atmospheric Pressure Loading");

  NumOfPts	= 24;
  Step		= 2.0;

  *Date = SBMJD::currentMJD();

  if (!(AploEphem=SetUp->loadAploEphem()))
    AploEphem = new SBAploEphem;
  
  
  TRF = SetUp->loadTRF();
  TRF->prepare4Run();
  Station = TRF->stations()->first();

  
  TStart = new SBMJD(AploEphem->tStart());
  TFinis = new SBMJD(AploEphem->tFinis());

  if (*Date<*TStart) *Date=*TStart + NumOfPts*AploEphem->interval()/2.0;
  if (*Date>*TFinis) *Date=*TFinis - NumOfPts*AploEphem->interval()/2.0;
   
  PlotCarrier = new SBPlotCarrier(4, 0, "Test of Atmosheric Pressure Loading for the " + 
				  (Station?Station->name():"UNKNOWN"));
  PlotCarrier->setFile4SaveBaseName("APLoad");

  PlotCarrier->createBranch(NumOfPts, "Ephemerides");
  PlotCarrier->createBranch(72, "Splines");
  
  PlotCarrier->columnNames()->append(new QString("Time, MJD"));
  PlotCarrier->setType(0, SBPlotCarrier::AT_MJD);
  PlotCarrier->columnNames()->append(new QString("Up, cm"));
  PlotCarrier->columnNames()->append(new QString("East, cm"));
  PlotCarrier->columnNames()->append(new QString("North, cm"));
  
  createWidgets();
  
  fillData4Plotting();
};

SBTestAPLoad::~SBTestAPLoad()
{
  if (AploEphem)
    {
      delete AploEphem;
      AploEphem = NULL;
    };

  if (TRF)
    {
      TRF->finisRun();
      delete TRF;
      TRF = NULL;
    };

  if (PlotCarrier)
    {
      delete PlotCarrier;
      PlotCarrier=NULL;
    };
};

QWidget* SBTestAPLoad::createWidget4Test(QWidget* parent)
{
  return (Plot = new SBPlot(PlotCarrier, parent));
};

void SBTestAPLoad::fillData4Plotting()
{
  if (Station)
    {
      SBMatrix	*B;
      unsigned int	i, N_;
      
      B = PlotCarrier->branches()->at(0)->data();
      
      AploEphem->registerStation(Station);
      AploEphem->getReady(*Date - NumOfPts*AploEphem->interval()/2.0, 
			  *Date + NumOfPts*AploEphem->interval()/2.0);
      SBSpline*	Spl = Station?AploEphem->spline4station(Station->id()):NULL;
      if (Spl)
	{
	  N_ = Spl->b()->nRow();
	  if (B->nRow() != N_)
	    {
	      i = B->nCol();
	      B->resize(N_, i);
	    };
	  for (i=0; i<N_; i++)
	    {
	      B->set(i, 0,  Spl->b()->at(i, 0));
	      B->set(i, 1,  100.0*Spl->b()->at(i, 1));
	      B->set(i, 2,  100.0*Spl->b()->at(i, 2));
	      B->set(i, 3,  100.0*Spl->b()->at(i, 3));
	    };
	}
      else
	{
	  if (B->nRow() != NumOfPts)
	    {
	      i = B->nCol();
	      B->resize(NumOfPts, i);
	    };
	  for (i=0; i<NumOfPts; i++)
	    {
	      B->set(i, 0,  *Date + i*AploEphem->interval() - NumOfPts/2*AploEphem->interval());
	      B->set(i, 1,  0.0);
	      B->set(i, 2,  0.0);
	      B->set(i, 3,  0.0);
	    };
	};
      
      
      unsigned int	Nspl = (unsigned int)(NumOfPts*AploEphem->interval()/Step*24.0);
      
      B = PlotCarrier->branches()->at(1)->data();
      if (B->nRow() != Nspl)
	{
	  i = B->nCol();
	  B->resize(Nspl, i);
	};
      
      SBMJD t = *Date - NumOfPts*AploEphem->interval()/2.0;
      Vector3	D(v3Zero);
      for (i=0; i<Nspl; i++, t+= Step/24.0)
	{
	  B->set(i, 0,  t);
	  D = 100.0*AploEphem->displacement(Station->id(), t); // in cm
	  B->set(i, 1,  D.at(X_AXIS));
	  B->set(i, 2,  D.at(Y_AXIS));
	  B->set(i, 3,  D.at(Z_AXIS));
	};
      
      AploEphem->dismissed();
    };
};

QWidget* SBTestAPLoad::auxWidgetRt(QWidget* parent)
{
  QWidget	*Aux = new QWidget(parent);
  QBoxLayout	*Layout = new QHBoxLayout(Aux, 0, 10), *SubLayout;

  QLabel	*label;
  QGroupBox	*gbox;
  QGridLayout	*grid;
  
  gbox = new QGroupBox("Draw Points", Aux);
  grid = new QGridLayout(gbox, 2,2, 20,5);

  label	= new QLabel("Number of points:", gbox);
  label	-> setMinimumSize(label->sizeHint());
  grid  -> addWidget(label, 0,0);
  ENumPts = new QLineEdit(gbox);
  ENumPts -> setText(QString().setNum(NumOfPts));
  ENumPts -> setMinimumHeight(ENumPts->sizeHint().height());
  ENumPts -> setMinimumWidth(ENumPts->fontMetrics().width("99999"));
  grid  -> addWidget(ENumPts, 0,1);

  label	= new QLabel("Step (hr):", gbox);
  label	-> setMinimumSize(label->sizeHint());
  grid  -> addWidget(label, 1,0);
  EStep = new QLineEdit(gbox);
  EStep -> setText(QString().setNum(Step));
  EStep -> setMinimumHeight(EStep->sizeHint().height());
  EStep -> setMinimumWidth(EStep->fontMetrics().width("999.999"));
  grid  -> addWidget(EStep, 1,1);
  grid  -> activate();
  Layout->addWidget(gbox);


  SubLayout = new QVBoxLayout(Layout);
  StLV = new QListView(Aux);
  StLV -> addColumn("DOMES");
  StLV -> setColumnAlignment( 0, AlignLeft);
  StLV -> addColumn("Station name");
  StLV -> setColumnAlignment( 1, AlignLeft);
  StLV -> addColumn("Tech");
  StLV -> setColumnAlignment( 2, AlignLeft);
  
  for (SBStation *St=TRF->stations()->first(); St; St=TRF->stations()->next())
    (void) new SBTestStationLI(StLV, St);

  StLV -> setAllColumnsShowFocus(TRUE);
  //  StLV -> setMinimumSize(StLV->sizeHint());
  StLV->setMinimumWidth(StLV->sizeHint().width());
  StLV -> setMinimumHeight(40);
  StLV -> setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  connect(StLV, SIGNAL(selectionChanged(QListViewItem*)), SLOT(stationChange(QListViewItem*)));
  if (StLV->firstChild())
    {
      StLV->setSelected(StLV->firstChild(), TRUE);
      StLV->setMaximumHeight(6*StLV->firstChild()->height());
    };
  StLV->setFocusPolicy(QWidget::StrongFocus);
  SubLayout->addWidget(StLV, 0);

  //  connect(StLV, SIGNAL(selectionChanged(QListViewItem*)), SLOT(stationChange(QListViewItem*)));

  Layout->activate();
  return Aux;
};

void SBTestAPLoad::recalc()
{
  SBTest::recalc();
  
  unsigned int	i;
  double	f;
  bool		Is;

  i = ENumPts->text().toUInt(&Is);
  if (Is && i != NumOfPts && i>4)
    {
      NumOfPts = i;
      IsDateModified=TRUE;
      //      Frame->eop()->setPtNum(NumOfPts);
    };
  
  f = EStep->text().toDouble(&Is);
  if (Is && f != Step)
    {
      Step = std::max(0.01,f);
      IsDateModified=TRUE;
    };

  if (IsDateModified)
    {
      fillData4Plotting();
      Plot->dataChanged();
    };
};

void SBTestAPLoad::stationChange(QListViewItem *LI)
{
  SBStation		*st=((SBTestStationLI*)LI)->station();

  if (st && Station != st)
    {
      Station = st;
      PlotCarrier->setName("Test of Atmospheric Pressure Loading for the " + 
			   (Station?Station->name():"UNKNOWN"));
      recalc();
      fillData4Plotting();
      Plot->dataChanged();
    };
};
/*==============================================================================================*/



/*==============================================================================================*/
