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




#include <qlayout.h>
#include <qpushbutton.h>

#include "SbIObsVLBIStat.H"
#include "SbSetup.H"



/*==============================================================================================*/
/*												*/
/* SBObsVLBIStatSrcLI..										*/
/*												*/
/*												*/
/*==============================================================================================*/
QString SBObsVLBIStatSrcLI::text(int col) const
{
  TmpString = "NULL";
  if (SS)
    switch (col)
      {
      case  0: TmpString = SS->name().copy();//id
	break;
      case  1: TmpString = SS->aka().copy();
	break;
      case  2: TmpString = SS->tStartTotal()==TZero?"":SS->tStartTotal().toString(SBMJD::F_DDMonYYYY).copy();
	break;
      case  3: TmpString = SS->tFinisTotal()==TZero?"":SS->tFinisTotal().toString(SBMJD::F_DDMonYYYY).copy();
	break;
      case  4: TmpString = SS->tStartProc()==TZero?"":SS->tStartProc().toString(SBMJD::F_DDMonYYYY).copy();
	break;
      case  5: TmpString = SS->tFinisProc()==TZero?"":SS->tFinisProc().toString(SBMJD::F_DDMonYYYY).copy();
	break;
      case  6: TmpString.sprintf("%.3f", double(SS->tFinisProc()-SS->tStartProc()));
	break;
      case  7: TmpString.sprintf("%d", SS->numTotal());
	break;
      case  8: TmpString.sprintf("%d", SS->numProc());
	break;
      case  9: TmpString.sprintf("%d", SS->statPerSession().count());
	break;
      };
  return TmpString;
};

QString SBObsVLBIStatSrcLI::key(int col, bool) const
{
  TmpString = "NULL";
  if (SS)
    switch (col)
      {
      case  0: TmpString = SS->name().copy();//id
	break;
      case  1: TmpString = SS->aka().copy();
	break;
      case  2: TmpString = SS->tStartTotal().toString(SBMJD::F_MJD).copy();
	break;
      case  3: TmpString = SS->tFinisTotal().toString(SBMJD::F_MJD).copy();
	break;
      case  4: TmpString = SS->tStartProc().toString(SBMJD::F_MJD).copy();
	break;
      case  5: TmpString = SS->tFinisProc().toString(SBMJD::F_MJD).copy();
	break;
      case  6: TmpString.sprintf("%020.3f", double(SS->tFinisProc()-SS->tStartProc()));
	break;
      case  7: TmpString.sprintf("%09d", SS->numTotal());
	break;
      case  8: TmpString.sprintf("%09d", SS->numProc());
	break;
      case  9: TmpString.sprintf("%09d", SS->statPerSession().count());
	break;
      };
  return TmpString;
};
/*==============================================================================================*/





/*==============================================================================================*/
/*												*/
/* SBObsVLBIStatStaLI..										*/
/*												*/
/*												*/
/*==============================================================================================*/
QString SBObsVLBIStatStaLI::text(int col) const
{
  TmpString = "NULL";
  if (SS)
    switch (col)
      {
      case  0: TmpString = SS->name().copy();//id
	break;
      case  1: TmpString = SS->aka().copy();
	break;
      case  2: TmpString = SS->tStartTotal()==TZero?"":SS->tStartTotal().toString(SBMJD::F_DDMonYYYY).copy();
	break;
      case  3: TmpString = SS->tFinisTotal()==TZero?"":SS->tFinisTotal().toString(SBMJD::F_DDMonYYYY).copy();
	break;
      case  4: TmpString = SS->tStartProc()==TZero?"":SS->tStartProc().toString(SBMJD::F_DDMonYYYY).copy();
	break;
      case  5: TmpString = SS->tFinisProc()==TZero?"":SS->tFinisProc().toString(SBMJD::F_DDMonYYYY).copy();
	break;
      case  6: TmpString.sprintf("%.3f", double(SS->tFinisProc()-SS->tStartProc()));
	break;
      case  7: TmpString.sprintf("%d", SS->numTotal());
	break;
      case  8: TmpString.sprintf("%d", SS->numProc());
	break;
      case  9: TmpString.sprintf("%d", SS->statPerSession().count());
	break;
      };
  return TmpString;
};

QString SBObsVLBIStatStaLI::key(int col, bool) const
{
  TmpString = "NULL";
  if (SS)
    switch (col)
      {
      case  0: TmpString = SS->name().copy();//id
	break;
      case  1: TmpString = SS->aka().copy();
	break;
      case  2: TmpString = SS->tStartTotal().toString(SBMJD::F_MJD).copy();
	break;
      case  3: TmpString = SS->tFinisTotal().toString(SBMJD::F_MJD).copy();
	break;
      case  4: TmpString = SS->tStartProc().toString(SBMJD::F_MJD).copy();
	break;
      case  5: TmpString = SS->tFinisProc().toString(SBMJD::F_MJD).copy();
	break;
      case  6: TmpString.sprintf("%020.3f", double(SS->tFinisProc()-SS->tStartProc()));
	break;
      case  7: TmpString.sprintf("%09d", SS->numTotal());
	break;
      case  8: TmpString.sprintf("%09d", SS->numProc());
	break;
      case  9: TmpString.sprintf("%09d", SS->statPerSession().count());
	break;
      };
  return TmpString;
};
/*==============================================================================================*/





/*==============================================================================================*/
/*												*/
/* SBObsVLBIStatRecordLI..									*/
/*												*/
/*												*/
/*==============================================================================================*/
QString SBObsVLBIStatRecordLI::text(int col) const
{
  TmpString = "NULL";
  if (SR)
    switch (col)
      {
      case  0: TmpString = SR->name().copy();//id
	break;
      case  1: TmpString = SR->aka().copy();
	break;
      case  2: TmpString = SR->tStartTotal()==TZero?"":SR->tStartTotal().toString(SBMJD::F_DDMonYYYY).copy();
	break;
      case  3: TmpString = SR->tFinisTotal()==TZero?"":SR->tFinisTotal().toString(SBMJD::F_DDMonYYYY).copy();
	break;
      case  4: TmpString = SR->tStartProc()==TZero?"":SR->tStartProc().toString(SBMJD::F_DDMonYYYY).copy();
	break;
      case  5: TmpString = SR->tFinisProc()==TZero?"":SR->tFinisProc().toString(SBMJD::F_DDMonYYYY).copy();
	break;
      case  6: TmpString.sprintf("%d", SR->numTotal());
	break;
      case  7: TmpString.sprintf("%d", SR->numProc());
	break;
	//      case  8: TmpString.sprintf("%10.3g", SetUp->scale()*SR->rms());
      case  8: TmpString.sprintf("%.4f", SetUp->scale()*SR->rms());
	break;
      };
  return TmpString;
};

QString SBObsVLBIStatRecordLI::key(int col, bool) const
{
  TmpString = "NULL";
  if (SR)
    switch (col)
      {
      case  0: TmpString = SR->name().copy();//id
	break;
      case  1: TmpString = SR->aka().copy();
	break;
      case  2: TmpString = SR->tStartTotal().toString(SBMJD::F_MJD).copy();
	break;
      case  3: TmpString = SR->tFinisTotal().toString(SBMJD::F_MJD).copy();
	break;
      case  4: TmpString = SR->tStartProc().toString(SBMJD::F_MJD).copy();
	break;
      case  5: TmpString = SR->tFinisProc().toString(SBMJD::F_MJD).copy();
	break;
      case  6: TmpString.sprintf("%09d", SR->numTotal());
	break;
      case  7: TmpString.sprintf("%09d", SR->numProc());
	break;
      case  8: TmpString.sprintf("%+025.6f", SetUp->scale()*SR->rms());
	break;
      };
  return TmpString;
};
/*==============================================================================================*/





/*==============================================================================================*/
/*												*/
/* SBObsVLBIStatBrowser..										*/
/*												*/
/*==============================================================================================*/
SBObsVLBIStatBrowser::SBObsVLBIStatBrowser(bool IsGlobal, 
					   QWidget* parent, const char* name, bool modal, WFlags f)
  : SBTabDialog(parent, name, modal, f)
{
  Statistics = SetUp->loadObsVLBIStatistics(IsGlobal);

  if (!Statistics)
    Statistics = new SBObsVLBIStatistics("Empty Statistics");


  setCaption(Statistics->name());

  addTab(wSourceStatistics(),  "Source statistics");
  addTab(wStationStatistics(), "Station statistics");

  //  setCancelButton();

  setOkButton("Close");
};

SBObsVLBIStatBrowser::~SBObsVLBIStatBrowser()
{
  if (Statistics)
    {
      delete Statistics;
      Statistics = NULL;
    };
};

QWidget* SBObsVLBIStatBrowser::wStationStatistics()
{
  QWidget	*W	= new QWidget(this);
  QBoxLayout	*Layout = new QVBoxLayout(W, 20, 10);

  StaLV = new QListView(W);
  StaLV -> addColumn("Name");
  StaLV -> setColumnAlignment( 0, AlignLeft);
  StaLV -> addColumn("Aka");
  StaLV -> setColumnAlignment( 1, AlignLeft);
  StaLV -> addColumn("Start Total");
  StaLV -> setColumnAlignment( 2, AlignLeft);
  StaLV -> addColumn("Finis Total");
  StaLV -> setColumnAlignment( 3, AlignLeft);
  StaLV -> addColumn("Start Proc");
  StaLV -> setColumnAlignment( 4, AlignLeft);
  StaLV -> addColumn("Finis Proc");
  StaLV -> setColumnAlignment( 5, AlignLeft);
  StaLV -> addColumn("Interval");
  StaLV -> setColumnAlignment( 6, AlignRight);
  StaLV -> addColumn("Num Total");
  StaLV -> setColumnAlignment( 7, AlignRight);
  StaLV -> addColumn("Num Proc");
  StaLV -> setColumnAlignment( 8, AlignRight);
  StaLV -> addColumn("Num of Sessions");
  StaLV -> setColumnAlignment( 9, AlignRight);

  for (SBObsVLBIStatStation* SS=Statistics->stationsStatistics().first(); 
       SS; 
       SS=Statistics->stationsStatistics().next())
    {
      new SBObsVLBIStatStaLI(StaLV, SS);
//      std::cout << SS->aka() << "  " << SS->name() << "    " << SS->numTotal() << "\n";
    };
      
  StaLV -> setAllColumnsShowFocus(TRUE);
  StaLV -> setMinimumSize(StaLV->sizeHint());
  StaLV -> setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  if (StaLV->firstChild())
    StaLV -> setSelected(StaLV->firstChild(), TRUE);
  StaLV -> setFocusPolicy(QWidget::StrongFocus);
  Layout -> addWidget(StaLV, 5);
  Layout -> activate();

  connect(StaLV,
	  SIGNAL(doubleClicked(QListViewItem*)), 
	  SLOT(editStaEntry(QListViewItem*)));

  return W;
};

QWidget* SBObsVLBIStatBrowser::wSourceStatistics()
{
  QWidget	*W	= new QWidget(this);
  QBoxLayout	*Layout = new QVBoxLayout(W, 20, 10);

  SrcLV = new QListView(W);
  SrcLV -> addColumn("Name");
  SrcLV -> setColumnAlignment( 0, AlignLeft);
  SrcLV -> addColumn("Aka");
  SrcLV -> setColumnAlignment( 1, AlignLeft);
  SrcLV -> addColumn("Start Total");
  SrcLV -> setColumnAlignment( 2, AlignLeft);
  SrcLV -> addColumn("Finis Total");
  SrcLV -> setColumnAlignment( 3, AlignLeft);
  SrcLV -> addColumn("Start Proc");
  SrcLV -> setColumnAlignment( 4, AlignLeft);
  SrcLV -> addColumn("Finis Proc");
  SrcLV -> setColumnAlignment( 5, AlignLeft);
  SrcLV -> addColumn("Interval");
  SrcLV -> setColumnAlignment( 6, AlignRight);
  SrcLV -> addColumn("Num Total");
  SrcLV -> setColumnAlignment( 7, AlignRight);
  SrcLV -> addColumn("Num Proc");
  SrcLV -> setColumnAlignment( 8, AlignRight);
  SrcLV -> addColumn("Num of Sessions");
  SrcLV -> setColumnAlignment( 9, AlignRight);

  for (SBObsVLBIStatSource* SS=Statistics->sourcesStatistics().first(); 
       SS; 
       SS=Statistics->sourcesStatistics().next())
    {
      new SBObsVLBIStatSrcLI(SrcLV, SS);
      
    };
      
  SrcLV -> setAllColumnsShowFocus(TRUE);
  SrcLV -> setMinimumSize(SrcLV->sizeHint());
  SrcLV -> setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  if (SrcLV->firstChild())
    SrcLV -> setSelected(SrcLV->firstChild(), TRUE);
  SrcLV -> setFocusPolicy(QWidget::StrongFocus);
  Layout -> addWidget(SrcLV, 5);
  Layout -> activate();

  connect(SrcLV,
	  SIGNAL(doubleClicked(QListViewItem*)), 
	  SLOT(editSrcEntry(QListViewItem*)));

  return W;
};

void SBObsVLBIStatBrowser::editSrcEntry(QListViewItem* Item)
{
  SBObsVLBIStatSrc *SrcStatBrowser =  
    new SBObsVLBIStatSrc(((SBObsVLBIStatSrcLI*)Item)->sourceStatistics(), this);
  SrcStatBrowser->show();
};

void SBObsVLBIStatBrowser::editStaEntry(QListViewItem* Item)
{
  SBObsVLBIStatSta *StaStatBrowser =  
    new SBObsVLBIStatSta(((SBObsVLBIStatStaLI*)Item)->stationStatistics(), this);
  StaStatBrowser->show();
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBObsVLBIStatSrc..										*/
/*												*/
/*==============================================================================================*/
SBObsVLBIStatSrc::SBObsVLBIStatSrc(SBObsVLBIStatSource* SrcStat_, 
				 QWidget* parent, const char* name, bool modal, WFlags f)
  : QDialog(parent, name, modal, f)
{
  SrcStat = SrcStat_;

  setCaption("Statistics for the source " + SrcStat->name());
  QBoxLayout	*Layout = new QVBoxLayout(this, 20, 10), *SubLayout;

  SessionLV = new QListView(this);
  SessionLV -> addColumn("Session");
  SessionLV -> setColumnAlignment( 0, AlignLeft);
  SessionLV -> addColumn("Aka");
  SessionLV -> setColumnAlignment( 1, AlignLeft);
  SessionLV -> addColumn("Start Total");
  SessionLV -> setColumnAlignment( 2, AlignLeft);
  SessionLV -> addColumn("Finis Total");
  SessionLV -> setColumnAlignment( 3, AlignLeft);
  SessionLV -> addColumn("Start Proc");
  SessionLV -> setColumnAlignment( 4, AlignLeft);
  SessionLV -> addColumn("Finis Proc");
  SessionLV -> setColumnAlignment( 5, AlignLeft);
  SessionLV -> addColumn("Num Total");
  SessionLV -> setColumnAlignment( 6, AlignRight);
  SessionLV -> addColumn("Num Proc");
  SessionLV -> setColumnAlignment( 7, AlignRight);
  SessionLV -> addColumn("RMS");
  SessionLV -> setColumnAlignment( 8, AlignRight);

  for (SBObsVLBIStatRecord* SR=SrcStat->statPerSession().first(); SR; SR=SrcStat->statPerSession().next())
    {
      new SBObsVLBIStatRecordLI(SessionLV, SR);
    };
      
  SessionLV -> setAllColumnsShowFocus(TRUE);
  SessionLV -> setMinimumSize(SessionLV->sizeHint());
  SessionLV -> setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  if (SessionLV->firstChild())
    SessionLV -> setSelected(SessionLV->firstChild(), TRUE);
  SessionLV -> setFocusPolicy(QWidget::StrongFocus);
  Layout -> addWidget(SessionLV, 5);


  //---
  SubLayout = new QHBoxLayout(10);
  Layout -> addLayout(SubLayout);
  SubLayout-> addStretch(1);

  QPushButton *Ok = new QPushButton("Close", this);
  Ok->setDefault(TRUE);
  Ok->setMinimumSize(Ok->sizeHint());

  SubLayout->addWidget(Ok);

  connect(Ok, SIGNAL(clicked()), SLOT(accept()));
  Layout->activate();
};

SBObsVLBIStatSrc::~SBObsVLBIStatSrc()
{
  SrcStat = NULL;
};
 
void SBObsVLBIStatSrc::accept()
{
  acquireData();
  QDialog::accept();
  delete this;
};

void SBObsVLBIStatSrc::acquireData()
{
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBObsVLBIStatSta..										*/
/*												*/
/*==============================================================================================*/
SBObsVLBIStatSta::SBObsVLBIStatSta(SBObsVLBIStatStation* StaStat_, 
				 QWidget* parent, const char* name, bool modal, WFlags f)
  : QDialog(parent, name, modal, f)
{
  StaStat = StaStat_;

  setCaption("Statistics for the station " + StaStat->name());
  QBoxLayout	*Layout = new QVBoxLayout(this, 20, 10), *SubLayout;

  SessionLV = new QListView(this);
  SessionLV -> addColumn("Session");
  SessionLV -> setColumnAlignment( 0, AlignLeft);
  SessionLV -> addColumn("Aka");
  SessionLV -> setColumnAlignment( 1, AlignLeft);
  SessionLV -> addColumn("Start Total");
  SessionLV -> setColumnAlignment( 2, AlignLeft);
  SessionLV -> addColumn("Finis Total");
  SessionLV -> setColumnAlignment( 3, AlignLeft);
  SessionLV -> addColumn("Start Proc");
  SessionLV -> setColumnAlignment( 4, AlignLeft);
  SessionLV -> addColumn("Finis Proc");
  SessionLV -> setColumnAlignment( 5, AlignLeft);
  SessionLV -> addColumn("Num Total");
  SessionLV -> setColumnAlignment( 6, AlignRight);
  SessionLV -> addColumn("Num Proc");
  SessionLV -> setColumnAlignment( 7, AlignRight);
  SessionLV -> addColumn("RMS");
  SessionLV -> setColumnAlignment( 8, AlignRight);

  for (SBObsVLBIStatRecord* SR=StaStat->statPerSession().first(); SR; SR=StaStat->statPerSession().next())
    {
      new SBObsVLBIStatRecordLI(SessionLV, SR);
    };
      
  SessionLV -> setAllColumnsShowFocus(TRUE);
  SessionLV -> setMinimumSize(SessionLV->sizeHint());
  SessionLV -> setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  if (SessionLV->firstChild())
    SessionLV -> setSelected(SessionLV->firstChild(), TRUE);
  SessionLV -> setFocusPolicy(QWidget::StrongFocus);
  Layout -> addWidget(SessionLV, 5);


  //---
  SubLayout = new QHBoxLayout(10);
  Layout -> addLayout(SubLayout);
  SubLayout-> addStretch(1);

  QPushButton *Ok = new QPushButton("Close", this);
  Ok->setDefault(TRUE);
  Ok->setMinimumSize(Ok->sizeHint());

  SubLayout->addWidget(Ok);

  connect(Ok, SIGNAL(clicked()), SLOT(accept()));
  Layout->activate();
};

SBObsVLBIStatSta::~SBObsVLBIStatSta()
{
  StaStat = NULL;
};
 
void SBObsVLBIStatSta::accept()
{
  acquireData();
  QDialog::accept();
  delete this;
};

void SBObsVLBIStatSta::acquireData()
{
};
/*==============================================================================================*/





/*==============================================================================================*/
