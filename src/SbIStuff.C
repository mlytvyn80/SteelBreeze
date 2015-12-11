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

#include "SbIStuff.H"

#include <qapplication.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qfiledialog.h> 
#include <qframe.h>
#include <qgroupbox.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlistbox.h>
#include <qpushbutton.h>
#include <qtooltip.h>
#include <qvalidator.h>

#include "SbIMainWin.H"
#include "SbIPlot.H"
#include "SbSetup.H"

#include "SbGeoAtmLoad.H"
#include "SbGeoEop.H"
#include "SbGeoEphem.H"
#include "SbGeoProject.H"






/*==============================================================================================*/
/*												*/
/* SBEOPListItem..										*/
/*												*/
/*==============================================================================================*/
SBEOPListItem::SBEOPListItem(QListView* parent, SBEOPEntry *EOPEntry_)
  : QListViewItem(parent)
{
  EOPEntry = EOPEntry_;
  if (!EOPEntry) EOPEntry = new SBEOPEntry;
};

QString SBEOPListItem::text(int col) const
{
  TmpString = "NULL";
  if (EOPEntry)
    switch (col)
      {
      case 0: TmpString.sprintf(" %7.1f",  (double)*EOPEntry);
	break;
      case 1: TmpString.sprintf("%8.5f",  EOPEntry->xp());
	break;
      case 2: TmpString.sprintf("%8.5f",  EOPEntry->yp());
	break;
      case 3: TmpString.sprintf("%8.5f",  EOPEntry->ut());
	break;
      case 4: TmpString.sprintf("%8.5f",  EOPEntry->lod());
	break;
      case 5: TmpString.sprintf("%8.5f",  EOPEntry->psi());
	break;
      case 6: TmpString.sprintf("%8.5f",  EOPEntry->eps());
	break;
      };
  return TmpString;
};

QString SBEOPListItem::key(int col, bool) const
{
  TmpString = "NULL";
  if (EOPEntry)
    switch (col)
      {
      case 0: TmpString = text(0);
	break;
      case 1: TmpString.sprintf("%016.12f", double(2.0+EOPEntry->xp()));
	break;
      case 2: TmpString.sprintf("%016.12f", double(2.0+EOPEntry->yp()));
	break;
      case 3: TmpString.sprintf("%016.12f", double(2.0+EOPEntry->ut()));
	break;
      case 4: TmpString.sprintf("%016.12f", double(2.0+EOPEntry->lod()));
	break;
      case 5: TmpString.sprintf("%016.12f", double(2.0+EOPEntry->psi()));
	break;
      case 6: TmpString.sprintf("%016.12f", double(2.0+EOPEntry->eps()));
	break;
      };
  return TmpString;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBStuffEOPs..										*/
/*												*/
/*==============================================================================================*/
SBStuffEOPs::SBStuffEOPs(QWidget* parent, const char* name, bool modal, WFlags f)
  : QDialog(parent, name, modal, f)
{
  numOpenedWindows = 0;
  isModified	   = FALSE;
  PlotCarrier	   = NULL;
  setCaption("EOPs");

  QSize		Size;
  QPushButton	*OKButton, *CancelButton, *ImportButton, *DrawButton;
  QGroupBox	*gbox;
  QBoxLayout	*Layout, *SubLayout;

  if (!(Weop = SetUp->loadEOP())) 
    Weop = new SBEOP;

  gbox	 = new QGroupBox("EOP", this);
  Layout = new QHBoxLayout(gbox, 20, 20);

  SubLayout = new QVBoxLayout;
  Layout -> addLayout(SubLayout, 2);

  SubLayout -> addStretch(1);
  SubLayout -> addWidget (createInfoGroup   (gbox));
  SubLayout -> addStretch(1);
  SubLayout -> addWidget (createOptionsGroup(gbox));
  SubLayout -> addStretch(1);


  EOPsLV = new QListView(gbox);
  EOPsLV -> addColumn("Date (MJD)");
  EOPsLV -> setColumnAlignment( 0, AlignLeft);
  EOPsLV -> addColumn("Pole X(as)");
  EOPsLV -> setColumnAlignment( 1, AlignRight);
  EOPsLV -> addColumn("Pole Y(as)");
  EOPsLV -> setColumnAlignment( 2, AlignRight);
  EOPsLV -> addColumn("UT1-UTC(s)");
  EOPsLV -> setColumnAlignment( 3, AlignRight);
  EOPsLV -> addColumn("LOD(s)");
  EOPsLV -> setColumnAlignment( 4, AlignRight);
  EOPsLV -> addColumn(" Psi (as) ");
  EOPsLV -> setColumnAlignment( 5, AlignRight);
  EOPsLV -> addColumn(" Eps (as) ");
  EOPsLV -> setColumnAlignment( 6, AlignRight);
  EOPsLV -> setAllColumnsShowFocus(TRUE);

  showSeries();

  EOPsLV -> setMinimumWidth(EOPsLV->sizeHint().width());
  EOPsLV -> setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  EOPsLV -> setFocusPolicy(QWidget::StrongFocus);

  Layout  -> addWidget(EOPsLV, 10);
  Layout  -> activate();
  
  
  
  //---
  Layout = new QVBoxLayout(this, 10, 10);
  Layout -> addWidget(gbox, 10);
  SubLayout = new QHBoxLayout(10);
  Layout -> addLayout(SubLayout);

  SubLayout->addStretch(1);
  
  SubLayout-> addWidget(OKButton     = new QPushButton("OK",     this));
  SubLayout-> addWidget(ImportButton = new QPushButton("Import", this));
  SubLayout-> addWidget(DrawButton   = new QPushButton("Draw",   this));
  SubLayout-> addWidget(CancelButton = new QPushButton("Cancel", this));

  ImportButton	-> setMinimumSize(Size=ImportButton->sizeHint());
  OKButton	-> setMinimumSize(Size);
  CancelButton	-> setMinimumSize(Size);
  DrawButton	-> setMinimumSize(Size);

  connect (OKButton,	 SIGNAL(clicked()), SLOT(accept()));
  connect (CancelButton, SIGNAL(clicked()), SLOT(reject()));
  connect (ImportButton, SIGNAL(clicked()), SLOT(import()));
  connect (DrawButton,   SIGNAL(clicked()), SLOT(draw()));
  
  Layout -> activate();
};

QWidget* SBStuffEOPs::createInfoGroup(QWidget* parent)
{
  QLabel	*label;
  QGroupBox	*gbox = new QGroupBox("Info", parent);
  QGridLayout	*grid = new QGridLayout(gbox, 5,2, 20,5);


  label	= new QLabel("EOP begins:", gbox);
  label	-> setMinimumSize(label->sizeHint());
  grid  -> addWidget(label, 0,0);
  EOPTStart = new QLabel(Weop->tStart().toString(SBMJD::F_DDMonYYYY), gbox);
  EOPTStart-> setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  EOPTStart-> setMinimumSize(EOPTStart->sizeHint());
  grid  -> addWidget(EOPTStart, 0,1);

  label	= new QLabel("EOP ends:", gbox);
  label	-> setMinimumSize(label->sizeHint());
  grid  -> addWidget(label, 1,0);
  EOPTFinis = new QLabel(Weop->tFinis().toString(SBMJD::F_DDMonYYYY), gbox);
  EOPTFinis-> setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  EOPTFinis-> setMinimumSize(EOPTFinis->sizeHint());
  grid  -> addWidget(EOPTFinis, 1,1);

  label	= new QLabel("Number of points:", gbox);
  label	-> setMinimumSize(label->sizeHint());
  grid  -> addWidget(label, 2,0);
  EOPNum = new QLabel(str.setNum(Weop->count()), gbox);
  EOPNum-> setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  EOPNum-> setMinimumSize(EOPNum->sizeHint());
  grid  -> addWidget(EOPNum, 2,1);

  label	= new QLabel("EOP Epoch:", gbox);
  label	-> setMinimumSize(label->sizeHint());
  grid  -> addWidget(label, 3,0);
  EOPEpoch = new QLineEdit(gbox);
  EOPEpoch -> setText( Weop-> epoch().toString(SBMJD::F_Year));
  EOPEpoch -> setValidator(new QDoubleValidator(1950.0, 2050.0, 1, gbox));
  EOPEpoch -> setMaxLength(7);
  EOPEpoch -> setMinimumHeight(EOPEpoch->sizeHint().height());
  grid  -> addWidget(EOPEpoch, 3,1);

  label	= new QLabel("Label:", gbox);
  label	-> setMinimumSize(label->sizeHint());
  grid  -> addWidget(label, 4,0);
  EOPLabel = new QLineEdit(gbox);
  EOPLabel -> setText(Weop->label().toString());
  EOPLabel -> setMinimumHeight(EOPLabel->sizeHint().height());
  EOPLabel -> setMinimumWidth(EOPLabel->fontMetrics().width(Weop->label().toString())+10);
  grid  -> addWidget(EOPLabel, 4,1);

  grid  -> setColStretch(0, 10);
  grid  -> activate();
  return gbox;
};

QWidget* SBStuffEOPs::createOptionsGroup(QWidget* parent)
{
  QLabel	*label;
  QGroupBox	*gbox = new QGroupBox("Options", parent);
  QGridLayout	*grid = new QGridLayout(gbox, 9,3, 20,5);


  label	 = new QLabel(" shift", gbox);
  label	-> setMinimumSize(label->sizeHint());
  grid  -> addWidget(label, 0,1);
  label	 = new QLabel(" trend", gbox);
  label	-> setMinimumSize(label->sizeHint());
  grid  -> addWidget(label, 0,2);

  label	 = new QLabel("Pole X (mas): ", gbox);
  label	-> setMinimumSize(label->sizeHint());
  grid  -> addWidget(label, 1,0);
  EOPXp  = new QLineEdit(gbox);
  EOPXp	-> setText(str.sprintf("%6.3f", Weop->xp()));
  EOPXp -> setMinimumHeight(EOPXp->sizeHint().height());
  EOPXp -> setMinimumWidth(EOPXp->fontMetrics().width("+0.00001")+10);

  grid   -> addWidget(EOPXp, 1,1);
  EOPXps  = new QLineEdit(gbox);
  EOPXps -> setText(str.sprintf("%6.3f", Weop->xps()));
  EOPXps -> setMinimumHeight(EOPXps->sizeHint().height());
  EOPXps -> setMinimumWidth(EOPXps->fontMetrics().width("+0.00001")+10);
  grid   -> addWidget(EOPXps, 1,2);


  label	 = new QLabel("Pole Y (mas): ", gbox);
  label	-> setMinimumSize(label->sizeHint());
  grid  -> addWidget(label, 2,0);
  EOPYp  = new QLineEdit(gbox);
  EOPYp	-> setText(str.sprintf("%6.3f", Weop->yp()));
  EOPYp -> setMinimumHeight(EOPYp->sizeHint().height());
  grid  -> addWidget(EOPYp, 2,1);
  EOPYps = new QLineEdit(gbox);
  EOPYps-> setText(str.sprintf("%6.3f", Weop->yps()));
  EOPYps-> setMinimumHeight(EOPYps->sizeHint().height());
  grid  -> addWidget(EOPYps, 2,2);


  label	 = new QLabel("Pole UT (ms): ", gbox);
  label	-> setMinimumSize(label->sizeHint());
  grid  -> addWidget(label, 3,0);
  EOPUT  = new QLineEdit(gbox);
  EOPUT	-> setText(str.sprintf("%6.3f", Weop->ut()));
  EOPUT -> setMinimumHeight(EOPUT->sizeHint().height());
  grid  -> addWidget(EOPUT, 3,1);
  EOPUTs = new QLineEdit(gbox);
  EOPUTs-> setText(str.sprintf("%6.3f", Weop->uts()));
  EOPUTs-> setMinimumHeight(EOPUTs->sizeHint().height());
  grid  -> addWidget(EOPUTs, 3,2);

  label	 = new QLabel("Psi (mas): ", gbox);
  label	-> setMinimumSize(label->sizeHint());
  grid  -> addWidget(label, 4,0);
  EOPPsi = new QLineEdit(gbox);
  EOPPsi-> setText(str.sprintf("%6.3f", Weop->psi()));
  EOPPsi-> setMinimumHeight(EOPPsi->sizeHint().height());
  grid  -> addWidget(EOPPsi, 4,1);
  EOPPsis = new QLineEdit(gbox);
  EOPPsis-> setText(str.sprintf("%6.3f", Weop->psis()));
  EOPPsis-> setMinimumHeight(EOPPsis->sizeHint().height());
  grid   -> addWidget(EOPPsis, 4,2);

  label	 = new QLabel("Eps (mas): ", gbox);
  label	-> setMinimumSize(label->sizeHint());
  grid  -> addWidget(label, 5,0);
  EOPEps = new QLineEdit(gbox);
  EOPEps-> setText(str.sprintf("%6.3f", Weop->eps ()));
  EOPEps-> setMinimumHeight(EOPEps->sizeHint().height());
  grid  -> addWidget(EOPEps, 5,1);
  EOPEpss = new QLineEdit(gbox);
  EOPEpss-> setText(str.sprintf("%6.3f", Weop->epss()));
  EOPEpss-> setMinimumHeight(EOPEpss->sizeHint().height());
  grid   -> addWidget(EOPEpss, 5,2);

  QFrame *frame = new QFrame(gbox);
  frame -> setFrameStyle(QFrame::Sunken | QFrame::HLine);
  frame -> setMinimumHeight(2*EOPEpss->sizeHint().height());
  grid  -> addMultiCellWidget(frame, 6,6, 0,2);

  EOPHiTide = new QCheckBox("HiTide variations present in UT", gbox);
  EOPHiTide -> setMinimumSize(EOPHiTide->sizeHint());
  EOPHiTide -> setChecked(Weop->utHasHiTide());
  grid  -> addMultiCellWidget(EOPHiTide, 7,7, 0,2);

  EOPLoTide = new QCheckBox("LoTide variations present in UT", gbox);
  EOPLoTide -> setMinimumSize(EOPLoTide->sizeHint());
  EOPLoTide -> setChecked(Weop->utHasHiTide());
  grid  -> addMultiCellWidget(EOPLoTide, 8,8, 0,2);
  grid  -> activate();

  QToolTip::add(EOPHiTide, "Check this box if UT high tide variations present in the EOP data");
  QToolTip::add(EOPLoTide, "Check this box if UT low tide variations present in the EOP data");

  return gbox;
};

SBStuffEOPs::~SBStuffEOPs()
{
  if (Weop) 
    {
      delete Weop;
      Weop = NULL;
    };

  if (PlotCarrier) 
    {
      delete PlotCarrier;
      PlotCarrier = NULL;
    };
};

void SBStuffEOPs::accept()
{
  if (numOpenedWindows) return; //there are open windows dependent upon us..
  QDialog::accept();

  SBMJD			date;
  bool			Is;
  double		f;

  SBSolutionName *w = new SBSolutionName(SBSolutionName::TYPE_EOP);
  w->fromString(EOPLabel->text());
  if (*w != Weop->label())
    {
      Weop->setLabel(*w);
      isModified=TRUE;
    };
  delete w;
  
  date.setMJD(SBMJD::F_Year, EOPEpoch->text());
  if (date != Weop->epoch())
    {
      Weop->setEpoch(date);
      isModified=TRUE;
    };
  
  if ((Is=EOPHiTide->isChecked())!=Weop->utHasHiTide())
    {
      Weop->setUtHasHiTide(Is);
      isModified=TRUE;
    };

  if ((Is=EOPLoTide->isChecked())!=Weop->utHasLoTide())
    {
      Weop->setUtHasLoTide(Is);
      isModified=TRUE;
    };
  
  f = EOPXp->text().toDouble(&Is);
  if (Is && f != Weop->xp())
    {
      Weop->setXp(f);
      isModified=TRUE;
    };
  f = EOPXps->text().toDouble(&Is);
  if (Is && f != Weop->xps())
    {
      Weop->setXps(f);
      isModified=TRUE;
    };

  f = EOPYp->text().toDouble(&Is);
  if (Is && f != Weop->yp())
    {
      Weop->setYp(f);
      isModified=TRUE;
    };
  f = EOPYps->text().toDouble(&Is);
  if (Is && f != Weop->yps())
    {
      Weop->setYps(f);
      isModified=TRUE;
    };

  f = EOPUT->text().toDouble(&Is);
  if (Is && f != Weop->ut())
    {
      Weop->setUt(f);
      isModified=TRUE;
    };
  f = EOPUTs->text().toDouble(&Is);
  if (Is && f != Weop->uts())
    {
      Weop->setUts(f);
      isModified=TRUE;
    };

  f = EOPPsi->text().toDouble(&Is);
  if (Is && f != Weop->psi())
    {
      Weop->setPsi(f);
      isModified=TRUE;
    };
  f = EOPPsis->text().toDouble(&Is);
  if (Is && f != Weop->psis())
    {
      Weop->setPsis(f);
      isModified=TRUE;
    };

  f = EOPEps->text().toDouble(&Is);
  if (Is && f != Weop->eps())
    {
      Weop->setEps(f);
      isModified=TRUE;
    };
  f = EOPEpss->text().toDouble(&Is);
  if (Is && f != Weop->epss())
    {
      Weop->setEpss(f);
      isModified=TRUE;
    };

  if (isModified)
    SetUp->saveEOP(Weop);
  delete this;
};

void SBStuffEOPs::reject() 
{
  if (numOpenedWindows) return; //there are open windows dependent upon us..
  QDialog::reject();
  delete this;
};

void SBStuffEOPs::modified(bool isModified_)
{
  isModified |= isModified_;
};

void SBStuffEOPs::childWindowClosed()
{
  numOpenedWindows--;
};

void SBStuffEOPs::updateEOP()
{
  if (Weop) 
    delete Weop;
  if (!(Weop = SetUp->loadEOP())) 
    Weop = new SBEOP;

  showSeries();

  EOPHiTide -> setChecked(Weop->utHasHiTide());
  EOPLoTide -> setChecked(Weop->utHasHiTide());

  EOPXp	    -> setText(str.sprintf("%6.3f", Weop->xp  ()));
  EOPXps    -> setText(str.sprintf("%6.3f", Weop->xps ()));
  EOPYp	    -> setText(str.sprintf("%6.3f", Weop->yp  ()));
  EOPYps    -> setText(str.sprintf("%6.3f", Weop->yps ()));
  EOPUT	    -> setText(str.sprintf("%6.3f", Weop->ut  ()));
  EOPUTs    -> setText(str.sprintf("%6.3f", Weop->uts ()));
  EOPPsi    -> setText(str.sprintf("%6.3f", Weop->eps ()));
  EOPPsis   -> setText(str.sprintf("%6.3f", Weop->epss()));
  EOPEps    -> setText(str.sprintf("%6.3f", Weop->psi ()));
  EOPEpss   -> setText(str.sprintf("%6.3f", Weop->psis()));
  
  EOPTStart -> setText( Weop->tStart().toString(SBMJD::F_DDMonYYYY) );
  EOPTFinis -> setText( Weop->tFinis().toString(SBMJD::F_DDMonYYYY) );
  EOPEpoch  -> setText( Weop-> epoch().toString(SBMJD::F_Year));
  EOPNum    -> setText( str.setNum(Weop->count()));
};

void SBStuffEOPs::import()
{
  if (numOpenedWindows) return; //there are opened windows dependent upon us..

  QStringList sl(QFileDialog::getOpenFileNames(0, SetUp->path2ImpEOP()));
  numOpenedWindows++;

  if (sl.count())
    {
      SBEOP*	W = new SBEOP;
      mainPrBarReset();
      mainPrBarSetTotalSteps(sl.count());
      int i=1;
      for (QStringList::Iterator it=sl.begin(); it != sl.end(); ++it)
	{ 
	  mainMessage("Processing: " + *it);
	  qApp->processEvents();
	  W->importSeries(*it);
	  mainPrBarSetProgress(i++);
	  qApp->processEvents();
	};
      mainMessage("Ready");
      SetUp->saveEOP(W);
      delete W;
      EOPsLV -> clear();
      delete Weop;
      Weop = SetUp->loadEOP();

      isModified = TRUE;
      mainPrBarReset();
      updateEOP();

      if (PlotCarrier) 
	{
	  delete PlotCarrier;
	  PlotCarrier = NULL;
	};
    };
  numOpenedWindows--;
};

void SBStuffEOPs::draw()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": SBStuffEOPsDraw selected");  

  if (!PlotCarrier)
    {
      PlotCarrier = new SBPlotCarrier(7, 0, "Earth Rotation Parameters of the soluton " + 
				      Weop->label().toString());
      PlotCarrier->setFile4SaveBaseName("eops");
      PlotCarrier->createBranch(Weop->count(), Weop->label().toString());
      PlotCarrier->columnNames()->append(new QString("Time, MJD"));
      PlotCarrier->setType(0, SBPlotCarrier::AT_MJD);
      PlotCarrier->columnNames()->append(new QString("X Pole offset, as"));
      PlotCarrier->columnNames()->append(new QString("Y Pole offset, as"));
      PlotCarrier->columnNames()->append(new QString("UT1 - UTC, s"));
      PlotCarrier->columnNames()->append(new QString("LOD, s"));
      PlotCarrier->columnNames()->append(new QString("dPsi nutation, as"));
      PlotCarrier->columnNames()->append(new QString("dEps nutation, as"));
      unsigned int i=0;
      for (SBEOPEntry *E = Weop->first(); E; E = Weop->next(), i++)
	{
	  PlotCarrier->branches()->at(0)->data()->set(i, 0,  *E);
	  PlotCarrier->branches()->at(0)->data()->set(i, 1,  E->xp());
	  PlotCarrier->branches()->at(0)->data()->set(i, 2,  E->yp());
	  PlotCarrier->branches()->at(0)->data()->set(i, 3,  E->ut());
	  PlotCarrier->branches()->at(0)->data()->set(i, 4,  E->lod());
	  PlotCarrier->branches()->at(0)->data()->set(i, 5,  E->psi());
	  PlotCarrier->branches()->at(0)->data()->set(i, 6,  E->eps());
	};
    };
  
  SBPlotDialog *EOPPlot =  new SBPlotDialog(PlotCarrier);
  connect (EOPPlot, SIGNAL(windowClosed()), SLOT(childWindowClosed()));
  numOpenedWindows++;
  EOPPlot->show();
};

void SBStuffEOPs::showSeries()
{
  //bool		IsUseProgressBar=FALSE;
  int		n=0;
  EOPsLV -> clear();
  //if (Weop->count()>5000)
  //  {
  //    mainPrBarSetTotalSteps(Weop->count());
  //    IsUseProgressBar=TRUE;
  mainMessage("Preparing EOP browser");
  qApp->processEvents();
  //  };
  
  for (SBEOPEntry* W=Weop->first(); W; W=Weop->next(), n++)
    {
      (void) new SBEOPListItem(EOPsLV, W);
      //    if (IsUseProgressBar && !(n%2000))
      //	{
      //	  inPrBarSetProgress(n);
      //	  pp->processEvents();
      //	};
    };
  //EOPsLV -> setSelected(EOPsLV->firstChild(), TRUE);
  //mainPrBarReset();
  mainMessage("Ready");
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBStuffEphem..										*/
/*												*/
/*==============================================================================================*/
SBStuffEphem::SBStuffEphem(QWidget* parent, const char* name, bool modal, WFlags f)
  : QDialog(parent, name, modal, f)
{
  setCaption("Ephemerides");

  QLabel	*label;
  QGroupBox	*gbox;
  QBoxLayout	*Layout, *SubLayout;
  QGridLayout	*grid;

  switch (Config.ephemModel())
    {
    case SBConfig::EM_LEDE200: Weph = new SBEphemDE200(&Config); break;
    default:
    case SBConfig::EM_LEDE403: Weph = new SBEphemDE403(&Config); break;
    case SBConfig::EM_LEDE405: Weph = new SBEphemDE405(&Config); break;
    };

  gbox	 = new QGroupBox(Weph->name(), this);
  grid	 = new QGridLayout(gbox, 14, 5,  20, 10);

  //--
  grid -> setRowStretch(0, 5);
  label= new QLabel("Status:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid ->addWidget(label, 2,0);
  
  LStatus= new QLabel(Weph->isOK()?"OK":"Broken", gbox);
  LStatus->setMinimumSize(LStatus->sizeHint());
  grid -> addWidget(LStatus, 2,1);

  label= new QLabel("First date:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid ->addWidget(label, 3,0);
  
  LTStart= new QLabel(Weph->tStart().toString(SBMJD::F_Short), gbox);
  LTStart->setMinimumSize(LTStart->sizeHint());
  grid -> addWidget(LTStart, 3,1);

  label= new QLabel("Last date:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid ->addWidget(label, 4,0);
  
  LTFinis= new QLabel(Weph->tFinis().toString(SBMJD::F_Short), gbox);
  LTFinis->setMinimumSize(LTFinis->sizeHint());
  grid -> addWidget(LTFinis, 4,1);

  label= new QLabel("Record's interval:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid ->addWidget(label, 5,0);
  
  LDt= new QLabel(str.sprintf("%4.1f days", Weph->dt()), gbox);
  LDt->setMinimumSize(LDt->sizeHint());
  grid -> addWidget(LDt, 5,1);

  label= new QLabel("Number of coefficients:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid ->addWidget(label, 6,0);
  
  LNCoef= new QLabel(str.sprintf("%d", Weph->nRecCoef()), gbox);
  LNCoef->setMinimumSize(LNCoef->sizeHint());
  grid -> addWidget(LNCoef, 6,1);

  label= new QLabel("Number of records:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid ->addWidget(label, 7,0);
  
  LNRecs= new QLabel(str.sprintf("%d", Weph->numRecs()), gbox);
  LNRecs->setMinimumSize(LNCoef->sizeHint());
  grid -> addWidget(LNRecs, 7,1);

  label= new QLabel("Astronomical Unit (m):", gbox);
  label->setMinimumSize(label->sizeHint());
  grid ->addWidget(label, 8,0);
  
  LAU= new QLabel(str.sprintf("%16.3f", Weph->au()), gbox);
  LNRecs->setMinimumSize(LAU->sizeHint());
  grid -> addWidget(LAU, 8,1);

  label= new QLabel("Speed of light (m/s):", gbox);
  label->setMinimumSize(label->sizeHint());
  grid ->addWidget(label, 9,0);
  
  LCLight= new QLabel(str.sprintf("%13.3f", Weph->cLight()), gbox);
  LCLight->setMinimumSize(LCLight->sizeHint());
  grid -> addWidget(LCLight, 9,1);

  label= new QLabel("Earth-Moon mass ratio:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid ->addWidget(label, 10,0);
  
  LEMRat= new QLabel(str.sprintf("%18.15f", Weph->emRat()), gbox);
  LEMRat->setMinimumSize(LEMRat->sizeHint());
  grid -> addWidget(LEMRat, 10,1);

  grid -> setColStretch(2, 5);
  //

  label= new QLabel("Body", gbox);
  label->setMinimumSize(label->sizeHint());
  grid ->addWidget(label, 1,3);
  label= new QLabel("Reciprocal solar mass", gbox);
  label->setMinimumSize(label->sizeHint());
  grid ->addWidget(label, 1,4);

  for (int i=0; i<11; i++)
    {
      label= new QLabel(Weph->body(i).name(), gbox);
      label->setMinimumSize(label->sizeHint());
      grid ->addWidget(label, 2+i,3);

      LBodyMass[i]= new QLabel(str.sprintf("%.4f", Weph->sun().gm()/Weph->body(i).gm()), gbox);
      LBodyMass[i]->setAlignment(AlignRight | AlignVCenter);
      LBodyMass[i]->setMinimumSize(LBodyMass[i]->sizeHint());
      grid -> addWidget(LBodyMass[i], 2+i, 4);
    };

  grid -> setRowStretch(13, 5);
  grid -> activate();
  

  //---
  Layout = new QVBoxLayout(this, 10, 10);
  Layout -> addWidget(gbox, 10);
  SubLayout = new QHBoxLayout(10);
  Layout -> addLayout(SubLayout);
  SubLayout-> addStretch(1);
  
  QPushButton *Ok	= new QPushButton("OK", this);
  QPushButton *Import	= new QPushButton("Import", this);
  QPushButton *Cancel	= new QPushButton("Cancel", this);
  Ok->setDefault(TRUE);

  QSize	BtnSize;
  Cancel->setMinimumSize((BtnSize=Cancel->sizeHint()));
  Ok    ->setMinimumSize(BtnSize);
  Import->setMinimumSize(BtnSize);

  SubLayout->addWidget(Ok);
  SubLayout->addWidget(Import);
  SubLayout->addWidget(Cancel);

  connect(Ok,	  SIGNAL(clicked()), SLOT(accept()));
  connect(Import, SIGNAL(clicked()), SLOT(import()));
  connect(Cancel, SIGNAL(clicked()), SLOT(reject()));

  connect(this, SIGNAL(stBarReset()),			MainWin,SLOT(stBarReset()));
  connect(this, SIGNAL(stBarSetTotalSteps(int)),	MainWin,SLOT(stBarSetTotalSteps(int)));
  connect(this, SIGNAL(stBarSetProgress(int)),		MainWin,SLOT(stBarSetProgress(int)));
  connect(this, SIGNAL(message(const QString&)),	MainWin,SLOT(setMessage(const QString&)));

  Layout -> activate();
};

SBStuffEphem::~SBStuffEphem()
{
  if (Weph) delete Weph;
};

void SBStuffEphem::accept()
{
  QDialog::accept();
  delete this;
};

void SBStuffEphem::reject() 
{
  QDialog::reject();
  delete this;
};

void SBStuffEphem::updateEphem()
{
  LStatus->setText(Weph->isOK()?"OK":"Broken");
  LTStart->setText(Weph->tStart().toString(SBMJD::F_Short));
  LTFinis->setText(Weph->tFinis().toString(SBMJD::F_Short));
  LDt    ->setText(str.sprintf("%4.1f days", Weph->dt()));
  LNCoef ->setText(str.sprintf("%d", Weph->nRecCoef()));
  LNRecs ->setText(str.sprintf("%d", Weph->numRecs()));
  LAU	 ->setText(str.sprintf("%16.3f", Weph->au()));
  LCLight->setText(str.sprintf("%13.3f", Weph->cLight()));
  LEMRat ->setText(str.sprintf("%18.15f", Weph->emRat()));
  for (int i=0; i<11; i++)
    LBodyMass[i]->setText(str.sprintf("%.4f", Weph->sun().gm()/Weph->body(i).gm()));
};

void SBStuffEphem::import()
{
  bool isModified = FALSE;
  QStringList sl(QFileDialog::getOpenFileNames(NULL, SetUp->path2Import()));
  if (sl.count())
    for ( QStringList::Iterator it = sl.begin(); it != sl.end(); ++it ) 
      if (Weph->import((*it).latin1())) isModified=TRUE;
  if (isModified) updateEphem();
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBAploHeaderLI..										*/
/*												*/
/*==============================================================================================*/
QString SBAploHeaderLI::text(int col) const
{
  TmpString = "NULL";

  if (Header)
    switch (col)
      {
      case 0: 
	TmpString = Header->tStart().toString(SBMJD::F_YYYYMMDDHHMMSSSS);
	break;
      case 1: 
	TmpString = Header->tFinis().toString(SBMJD::F_YYYYMMDDHHMMSSSS);
	break;
      case 2: 
	TmpString.sprintf("%.4f", Header->interval());
	break;
      };
  return TmpString;
};

QString SBAploHeaderLI::key(int col, bool) const
{
  TmpString = "NULL";

  if (Header)
    switch (col)
      {
      case 0: 
	TmpString = Header->tStart().toString(SBMJD::F_INTERNAL);
	break;
      case 1: 
	TmpString = Header->tFinis().toString(SBMJD::F_INTERNAL);
	break;
      case 2: 
	TmpString.sprintf("%09.4f", Header->interval());
	break;
      };
  return TmpString;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBAploEntryLI..										*/
/*												*/
/*==============================================================================================*/
QString SBAploEntryLI::text(int col) const
{
  TmpString = "NULL";

  if (Entry)
    switch (col)
      {
      case 0: 
	TmpString = Entry->name();
	break;
      case 1: 
	TmpString = Entry->id().toString();
	break;
      case 2: 
	TmpString.sprintf("%d", Entry->count());
	break;
      };
  return TmpString;
};

QString SBAploEntryLI::key(int col, bool) const
{
  TmpString = "NULL";

  if (Entry)
    switch (col)
      {
      case 0: 
	TmpString = Entry->name();
	break;
      case 1: 
	TmpString = Entry->id().toString();
	break;
      case 2: 
	TmpString.sprintf("%08d", Entry->count());
	break;
      };
  return TmpString;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBStuffAplo..										*/
/*												*/
/*==============================================================================================*/
SBStuffAplo::SBStuffAplo(QWidget* parent, const char* name, bool modal, WFlags f)
  : QDialog(parent, name, modal, f)
{
  setCaption("Atmosperic pressure loading Ephemerides");

  //  PlotCarrier		= NULL;
  IsBlockedOkButton	= FALSE;
  IsBlockedDrawButton	= FALSE;
  IsBlockedImportButton	= FALSE;
  IsBlockedImportHPSButton= FALSE;
  IsBlockedCancelButton	= FALSE;

  QLabel	*label;
  QGroupBox	*gbox;
  QBoxLayout	*Layout, *SubLayout;
  QGridLayout	*grid, *grd;

  isTRFModified = FALSE;
  isModified	= FALSE;
  if (!(AploEphem=SetUp->loadAploEphem()))
    {
      AploEphem = new SBAploEphem;
      isModified= TRUE;
    };
  

  Layout = new QVBoxLayout(this, 20, 10);
  grid	 = new QGridLayout(2, 2, 10);
  Layout -> addLayout(grid);
  
  
  HeadersLV = new QListView(this);
  HeadersLV -> addColumn("Start");
  HeadersLV -> setColumnAlignment( 0, AlignLeft);
  HeadersLV -> addColumn("Finis");
  HeadersLV -> setColumnAlignment( 1, AlignLeft);
  HeadersLV -> addColumn("Step");
  HeadersLV -> setColumnAlignment( 2, AlignRight);

  for (SBAploHeader *H=AploEphem->first(); H; H=AploEphem->next())
    (void) new SBAploHeaderLI(HeadersLV, H);
  
  HeadersLV->setAllColumnsShowFocus(TRUE);
  HeadersLV->setMinimumSize(HeadersLV->sizeHint());
  HeadersLV->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  if (HeadersLV->firstChild())
    HeadersLV->setSelected(HeadersLV->firstChild(), TRUE);
  HeadersLV->setFocusPolicy(QWidget::StrongFocus);

  grid ->addWidget(HeadersLV, 0, 0);

    
  //--
  gbox	 = new QGroupBox("Ephemerides Info", this);
  grd	 = new QGridLayout(gbox, 4, 2,  20, 10);

  label= new QLabel("Status:", gbox);
  label->setMinimumSize(label->sizeHint());
  grd ->addWidget(label, 0,0);
  
  LStatus= new QLabel(AploEphem->isOK()?"OK":"Broken", gbox);
  LStatus->setMinimumSize(LStatus->sizeHint());
  grd -> addWidget(LStatus, 0,1);

  label= new QLabel("First date:", gbox);
  label->setMinimumSize(label->sizeHint());
  grd ->addWidget(label, 1,0);
  
  LTStart= new QLabel(AploEphem->tStart().toString(SBMJD::F_Short), gbox);
  LTStart->setMinimumSize(LTStart->sizeHint());
  grd -> addWidget(LTStart, 1,1);

  label= new QLabel("Last date:", gbox);
  label->setMinimumSize(label->sizeHint());
  grd ->addWidget(label, 2,0);
  
  LTFinis= new QLabel(AploEphem->tFinis().toString(SBMJD::F_Short), gbox);
  LTFinis->setMinimumSize(LTFinis->sizeHint());
  grd -> addWidget(LTFinis, 2,1);

  label= new QLabel("Number of Chunks:", gbox);
  label->setMinimumSize(label->sizeHint());
  grd ->addWidget(label, 3,0);
  
  LNumChunks= new QLabel(QString().setNum(AploEphem->count()), gbox);
  LNumChunks->setMinimumSize(LTFinis->sizeHint());
  grd -> addWidget(LNumChunks, 3,1);
  grd -> activate();

  grid ->addWidget(gbox, 1, 0);


  //--
  Chunk = NULL;
  
  if (AploEphem->count())
    AploEphem->loadChunk(AploEphem->first(), Chunk);
  
  EntriesLV = new QListView(this);
  EntriesLV -> addColumn("Aka");
  EntriesLV -> setColumnAlignment( 0, AlignLeft);
  EntriesLV -> addColumn("DOMES");
  EntriesLV -> setColumnAlignment( 1, AlignRight);
  EntriesLV -> addColumn("count");
  EntriesLV -> setColumnAlignment( 2, AlignRight);

  if (Chunk)
    for (SBAploEntry *E=Chunk->first(); E; E=Chunk->next())
      (void) new SBAploEntryLI(EntriesLV, E);
  
  EntriesLV->setAllColumnsShowFocus(TRUE);
  EntriesLV->setMinimumSize(EntriesLV->sizeHint());
  EntriesLV->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  if (EntriesLV->firstChild())
    EntriesLV->setSelected(EntriesLV->firstChild(), TRUE);
  EntriesLV->setFocusPolicy(QWidget::StrongFocus);

  grid ->addWidget(EntriesLV, 0, 1);


  

  //--
  //  grid ->addWidget(gbox, 1, 1);




  //---
  SubLayout = new QHBoxLayout(10);
  Layout -> addLayout(SubLayout);
  SubLayout-> addStretch(1);
  
  Ok	= new QPushButton("OK", this);
  Draw  = new QPushButton("Draw", this);
  Import= new QPushButton("Import", this);
  ImportHPS= new QPushButton("Import HPS", this);
  Cancel= new QPushButton("Cancel", this);
  Ok->setDefault(TRUE);

  QSize	BtnSize;
  ImportHPS->setMinimumSize((BtnSize=ImportHPS->sizeHint()));
  Cancel->setMinimumSize(BtnSize);
  Ok    ->setMinimumSize(BtnSize);
  Draw  ->setMinimumSize(BtnSize);
  Import->setMinimumSize(BtnSize);

  SubLayout->addWidget(Ok);
  SubLayout->addWidget(Draw);
  SubLayout->addWidget(Import);
  SubLayout->addWidget(ImportHPS);
  SubLayout->addWidget(Cancel);

  connect(Ok,		SIGNAL(clicked()), SLOT(accept()));
  connect(Draw,		SIGNAL(clicked()), SLOT(draw()));
  connect(Import,	SIGNAL(clicked()), SLOT(import()));
  connect(ImportHPS,	SIGNAL(clicked()), SLOT(importHPS()));
  connect(Cancel,	SIGNAL(clicked()), SLOT(reject()));

  connect(this, SIGNAL(stBarReset()),			MainWin,SLOT(stBarReset()));
  connect(this, SIGNAL(stBarSetTotalSteps(int)),	MainWin,SLOT(stBarSetTotalSteps(int)));
  connect(this, SIGNAL(stBarSetProgress(int)),		MainWin,SLOT(stBarSetProgress(int)));
  connect(this, SIGNAL(message(const QString&)),	MainWin,SLOT(setMessage(const QString&)));

  //  connect(HeadersLV, SIGNAL(selectionChanged(QListViewItem*)), SLOT(headerChanged(QListViewItem*)));
  connect(HeadersLV, SIGNAL(currentChanged(QListViewItem*)), SLOT(headerChanged(QListViewItem*)));

  Layout -> activate();
};

SBStuffAplo::~SBStuffAplo()
{
  if (AploEphem) 
    {
      delete AploEphem;
      AploEphem = NULL;
    };
  //  if (PlotCarrier) 
  //    {
  //      delete PlotCarrier;
  //      PlotCarrier = NULL;
  //    };
};

void SBStuffAplo::accept()
{
  QDialog::accept();
  if (isModified)
    SetUp->saveAploEphem(AploEphem);
  delete this;
};

void SBStuffAplo::reject() 
{
  QDialog::reject();
  delete this;
};

void SBStuffAplo::headerChanged(QListViewItem* Item)
{
  if (Item)
    {
      if (Chunk)
	{
	  delete Chunk;
	  Chunk=NULL;
	};
      SBAploHeader *hdr = ((SBAploHeaderLI*)Item)->header();
      
      if (hdr && AploEphem->count())
	AploEphem->loadChunk(hdr, Chunk);
      
      EntriesLV->clear();
      if (Chunk)
	{
	  for (SBAploEntry *E=Chunk->first(); E; E=Chunk->next())
	    (void) new SBAploEntryLI(EntriesLV, E);
	  if (EntriesLV->firstChild())
	    EntriesLV->setSelected(EntriesLV->firstChild(), TRUE);
	};
    };
};

void SBStuffAplo::updateEphem()
{
  //
  HeadersLV->clear();
  if (AploEphem)
    {
      for (SBAploHeader *H=AploEphem->first(); H; H=AploEphem->next())
	(void) new SBAploHeaderLI(HeadersLV, H);
      if (HeadersLV->firstChild())
	HeadersLV->setSelected(HeadersLV->firstChild(), TRUE);
      if (EntriesLV->firstChild())
	EntriesLV->setSelected(EntriesLV->firstChild(), TRUE);
    };

  //
  LStatus->setText(AploEphem->isOK()?"OK":"Broken");
  LTStart->setText(AploEphem->tStart().toString(SBMJD::F_Short));
  LTFinis->setText(AploEphem->tFinis().toString(SBMJD::F_Short));
  LNumChunks->setText(QString().setNum(AploEphem->count()));
};

void SBStuffAplo::import()
{
  QStringList sl(QFileDialog::getOpenFileNames(NULL, SetUp->path2ImpAplo()));
  if (sl.count())
    {
      bool NeedSaveTRF = FALSE;
      blockInput();
      SB_TRF* TRF = SetUp->loadTRF();

      //      TRF->prepare4Run();
      for (QStringList::Iterator it=sl.begin(); it!=sl.end(); ++it)
	if (AploEphem->importEphem(*it, TRF, NeedSaveTRF))
	  isModified=TRUE;
      //TRF->finisRun();

      if (NeedSaveTRF)
	SetUp->saveTRF(TRF);

      delete TRF;

      if (isModified) 
	updateEphem();
      restoreInput();
    };
};

void SBStuffAplo::importHPS()
{
  QString FileName(QFileDialog::getOpenFileName(SetUp->path2ImpAplo()));

  bool NeedSaveTRF = FALSE;
  blockInput();
  SB_TRF* TRF = SetUp->loadTRF();
  
  //      TRF->prepare4Run();
  AploEphem->importHPS(FileName, TRF);
  isModified=TRUE;
  //TRF->finisRun();
  
  if (NeedSaveTRF)
    SetUp->saveTRF(TRF);
  
  delete TRF;
  
  if (isModified)
    updateEphem();
  restoreInput();
};

void SBStuffAplo::draw()
{
  SBAploEntry *Entry = NULL;
  if (Chunk && (Entry=((SBAploEntryLI*)(EntriesLV->currentItem()))->entry()))
    {
      SBPlotCarrier	*PlotCarrier = new SBPlotCarrier(4, 0, 
							 "Atmosperic Pressure loading for the site " + 
							 Entry->name() + " [" + Entry->id().toString() + "]");
      PlotCarrier->setFile4SaveBaseName("aplo");
      PlotCarrier->createBranch(Entry->count(), Entry->id().toString());

      PlotCarrier->columnNames()->append(new QString("Time, MJD"));
      PlotCarrier->setType(0, SBPlotCarrier::AT_MJD);
      PlotCarrier->columnNames()->append(new QString("Up, m"));
      PlotCarrier->columnNames()->append(new QString("East, m"));
      PlotCarrier->columnNames()->append(new QString("North, m"));
      
      unsigned int i=0;
      for (SBAploDRec *DRec=Entry->first(); DRec; DRec=Entry->next(), i++)
	{
	  PlotCarrier->branches()->at(0)->data()->set(i, 0,  *DRec);
	  PlotCarrier->branches()->at(0)->data()->set(i, 1,  DRec->displacement().at(VERT));
	  PlotCarrier->branches()->at(0)->data()->set(i, 2,  DRec->displacement().at(EAST));
	  PlotCarrier->branches()->at(0)->data()->set(i, 3,  DRec->displacement().at(NORT));
	};
      
      SBPlotDialog *Plot =  new SBPlotDialog(PlotCarrier, TRUE);
      Plot->show();
    };
};

void SBStuffAplo::blockInput()
{
  QApplication::setOverrideCursor(Qt::waitCursor);

  if (Ok)
    {
      IsBlockedOkButton  = Ok->signalsBlocked();
      Ok->blockSignals(TRUE);
    };
  if (Draw)
    {
      IsBlockedDrawButton  = Draw->signalsBlocked();
      Draw->blockSignals(TRUE);
    };
  if (Import)
    {
      IsBlockedImportButton  = Import->signalsBlocked();
      Import->blockSignals(TRUE);
    };
  if (ImportHPS)
    {
      IsBlockedImportHPSButton  = Import->signalsBlocked();
      ImportHPS->blockSignals(TRUE);
    };
  if (Cancel)
    {
      IsBlockedCancelButton  = Cancel->signalsBlocked();
      Cancel->blockSignals(TRUE);
    };
};

void SBStuffAplo::restoreInput()
{
  qApp->processEvents();

  if (Ok)
    Ok->blockSignals(IsBlockedOkButton);
  if (Draw)
    Draw->blockSignals(IsBlockedDrawButton);
  if (Import)
    Import->blockSignals(IsBlockedImportButton);
  if (ImportHPS)
    ImportHPS->blockSignals(IsBlockedImportHPSButton);
  if (Cancel)
    Cancel->blockSignals(IsBlockedCancelButton);
  
  QApplication::restoreOverrideCursor();
};
/*==============================================================================================*/


/*==============================================================================================*/
