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

#include "SbIStuffStation.H"

#include <qapplication.h>
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qdir.h>
#include <qfiledialog.h>
#include <qframe.h>
#include <qgroupbox.h>
#include <qheader.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlistbox.h>
#include <qmessagebox.h>
#include <qmultilinedit.h>
#include <qpainter.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qtooltip.h>
#include <qvalidator.h>
#include <qvbuttongroup.h>


#include "SbGeoEop.H"
#include "SbIStuffSource.H"
#include "SbSetup.H"





/*==============================================================================================*/
/*												*/
/* SBStationListItem..										*/
/*												*/
/*==============================================================================================*/
SBStationListItem::SBStationListItem(QListView* parent, SBStation *Station_)
  : QListViewItem(parent)
{
  isFaked = FALSE;
  if (Station_) Station = Station_;
  else 
    {
      Station = new SBStation(NULL);
      isFaked = TRUE;
    };
};

SBStationListItem::~SBStationListItem()
{
  if (isFaked && Station) delete Station;
  Station=NULL;
};

QString SBStationListItem::text(int col) const
{
  //  static const QString sClassC[] = {"A", "B", "C", "D", "E", "Z", " "};
  static const QString sMntType[] = {"AZEL", "EQUA", "X_YN", "X_YE", "RICH", " "};
  TmpString = "NULL";
  if (Station)
    switch (col)
      {
      case 0: TmpString.sprintf("%c%03d ", Station->pointType()
				==SBStation::Pt_Antenna?'S':'M', Station->domeMinor());
      break;
      case 1: TmpString = (const char*)(Station->name() + " ");
	break;
      case 2: 
	if (Station->CDP()<9999) TmpString.sprintf(" %04d ", Station->CDP());
	else TmpString = "    ";
	break;
      case 3: TmpString = (const char*)Station->charID();
	break;
      case 4: TmpString = SBSolutionName::techAbbrev(Station->tech());
	break;
      case 5: TmpString.sprintf("%d ", Station->refPt().count());
	break;
      case 6: TmpString = Station->isAttr(SBStation::hasOwnOLoad)?"Own":"Site's";
	break;
      case 7: TmpString = sMntType[Station->mountType()];
	break;
      case 8: TmpString = (Station->site()->r()==v3Zero || Station->r_first()==v3Zero)?" ":
	(const char*)TmpString.sprintf("%9.3f", (Station->site()->r()-Station->r_first()).module());
	break;
      case 9: TmpString = (Station->site()->v()==v3Zero || Station->v()==v3Zero)?" ":
	(const char*)TmpString.sprintf("%7.4f", (Station->site()->v()-Station->v()).module());
	break;
      };
  return TmpString;
};

QString SBStationListItem::key(int col, bool) const
{
  static const QString sTech   [] = {"A","H","D","C","B","E","F","I","J","G","Z"};
  //  static const QString sClassC [] = {"A", "B", "C", "D", "E", "Z", "_"};
  static const QString sMntType[] = {"AZEL", "EQUA", "X_YN", "X_YE", "RICH", "_"};
  TmpString = "NULL";
  if (Station)
    switch (col)
      {
      case 0: TmpString.sprintf("%c%03d", Station->pointType()
				==SBStation::Pt_Antenna?'S':'M', Station->domeMinor());
      break;
      case 1: TmpString = (const char*)Station->name();
	break;
      case 2: TmpString.sprintf("%04d", Station->CDP());
	break;
      case 3: TmpString = Station->charID()!="    "?(const char*)Station->charID():"____";
	break;
      case 4: TmpString = sTech[Station->tech()];
	break;
      case 5: TmpString.sprintf("%04d ", Station->refPt().count());
	break;
      case 6: TmpString = Station->isAttr(SBStation::hasOwnOLoad)?"A":"B";
	break;
      case 7: TmpString = sMntType[Station->mountType()];
	break;
      case 8: TmpString = (Station->site()->r()==v3Zero || Station->r_first()==v3Zero)?" ":
	(const char*)TmpString.sprintf("%013.4f", (Station->site()->r()-Station->r_first()).module());
	break;
      case 9: TmpString = (Station->site()->v()==v3Zero || Station->v()==v3Zero)?" ":
	(const char*)TmpString.sprintf("%013.4f", (Station->site()->v()-Station->v()).module());
	break;
      };
  return TmpString;
};

/* till the next time...
const QPixmap * SBStationListItem::pixmap (int col) const
{
  QPixmap *pm=0;
  if (Station)
    switch (col)
      {
      case 0:
	pm = pmSrcStatus[Source->status()];
	break;
      case 2: pm=0;
	break;
      case 3: pm=0;
	break;
      case 4: pm=0;
	break;
      default: pm=0;
	break;
      };
  return pm;
};
*/
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBSiteListItem..										*/
/*												*/
/*==============================================================================================*/
SBSiteListItem::SBSiteListItem(QListView* parent, SBSite *Site_)
  : QListViewItem(parent)
{
  isFaked = FALSE;
  if (Site_) Site = Site_;
  else 
    {
      Site = new SBSite;
      isFaked = TRUE;
    };
};

SBSiteListItem::~SBSiteListItem()
{
  if (isFaked && Site) delete Site;
  Site=NULL;
};

QString SBSiteListItem::text(int col) const
{
  static const QString sOLoad[] = {"DOME", "CDP", "SITE", "NAME", "DIST", "MANUAL", "NONE"};
  int		d;
  int		m;
  double	s;
  TmpString = "NULL";
  if (Site)
    switch (col)
      {
      case 0: TmpString.sprintf("%05d", Site->domeMajor());
	break;
      case 1: TmpString = (const char*)Site->name();
	break;
      case 2: TmpString = (const char*)Site->country();
	break;
      case 3: TmpString = (const char*)Site->plate();
	break;
      case 4: 
	rad2dmsl(Site->longitude(), d, m, s);
	TmpString.sprintf(" %4d %02d %04.1f", d,m,s);
	break;
      case 5:
	rad2dms(Site->latitude(), d, m, s);
	TmpString.sprintf(" %3d %02d %04.1f", d,abs(m),fabs(s));
	break;
      case 6: TmpString.sprintf(" %6.1f", Site->height());
	break;
      case 7: TmpString = sOLoad[Site->oLoad().pickedUp()];
	break;
      case 8: TmpString.sprintf("% 3d", Site->stations().count());
	break;
      };
  return TmpString;
};

QString SBSiteListItem::key(int col, bool) const
{
  static const QString sOLoad[] = {"0", "1", "2", "3", "4", "5", "6"};
  TmpString = "NULL";
  if (Site)
    switch (col)
      {
      case 0: TmpString.sprintf("%05d", Site->domeMajor());
	break;
      case 1: TmpString = (const char*)Site->name();
	break;
      case 2: TmpString = (const char*)Site->country();
	break;
      case 3: TmpString = (const char*)Site->plate();
	break;
      case 4: TmpString.sprintf("%014.12f", Site->longitude());
	break;
      case 5: TmpString.sprintf("%014.12f", 7.0+Site->latitude());
	break;
      case 6: TmpString.sprintf("%08.1f", 4000.0+Site->height());
	break;
      case 7: TmpString = sOLoad[Site->oLoad().pickedUp()];
	break;
      case 8: TmpString.sprintf("% 3d", Site->stations().count());
	break;
      };
  return TmpString;
};

/* till the next time...
const QPixmap * SBSiteListItem::pixmap (int col) const
{
  QPixmap *pm=0;
  if (Site)
    switch (col)
      {
      case 0:
	pm = pmSrcStatus[Source->status()];
	break;
      case 2: pm=0;
	break;
      case 3: pm=0;
	break;
      case 4: pm=0;
	break;
      default: pm=0;
	break;
      };
  return pm;
};
*/
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBEccListItem..										*/
/*												*/
/*==============================================================================================*/
SBEccListItem::SBEccListItem(QListView* parent_, SBStationEcc* StEcc_, SBEccentricity* Ecc_)
  : QListViewItem(parent_)
{
  StEcc = StEcc_;
  Ecc   = Ecc_;
};

QString SBEccListItem::text(int col) const
{
  TmpString = "NULL";

  if (StEcc && Ecc)
    switch (col)
      {
      case 0: TmpString = StEcc->toString().copy();
	break;
      case 1: TmpString = Ecc->aka().copy();
	break;
      case 2: TmpString = Ecc->tStart().toString(SBMJD::F_DateShort).copy();
	break;
      case 3: TmpString = Ecc->tFinis().toString(SBMJD::F_DateShort).copy();
	break;
      case 4: TmpString = Ecc->type()==SBEccentricity::ET_NEU?"NEU":"XYZ";
	break;
      case 5: TmpString = Ecc->sessionName().copy();
	break;
      case 6: TmpString.sprintf("%.5f", Ecc->ecc().at(X_AXIS));
	break;
      case 7: TmpString.sprintf("%.5f", Ecc->ecc().at(Y_AXIS));
	break;
      case 8: TmpString.sprintf("%.5f", Ecc->ecc().at(Z_AXIS));
	break;
      };
  return TmpString;
};

QString SBEccListItem::key(int col, bool) const
{
  TmpString = "NULL";

  if (StEcc && Ecc)
    switch (col)
      {
      case 0: TmpString = StEcc->toString().copy();
	break;
      case 1: TmpString = Ecc->aka().copy();
	break;
      case 2: TmpString = Ecc->tStart().toString(SBMJD::F_INTERNAL).copy();
	break;
      case 3: TmpString = Ecc->tFinis().toString(SBMJD::F_INTERNAL).copy();
	break;
      case 4: TmpString = Ecc->type()==SBEccentricity::ET_NEU?"NEU":"XYZ";
	break;
      case 5: TmpString = Ecc->sessionName().copy();
	break;
      case 6: TmpString.sprintf("%20.5f", Ecc->ecc().at(X_AXIS));
	break;
      case 7: TmpString.sprintf("%20.5f", Ecc->ecc().at(Y_AXIS));
	break;
      case 8: TmpString.sprintf("%20.5f", Ecc->ecc().at(Z_AXIS));
	break;
      };
  return TmpString;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBCoordsListItem..										*/
/*												*/
/*==============================================================================================*/
QString SBCoordsListItem::text(int col) const
{
  TmpString = "NULL";

  if (Coords)
    switch (col)
      {
      case  0: TmpString = Coords->toString(SBMJD::F_DateShort);
	break;
      case  1: TmpString.sprintf("%.3f", Coords->coo().at(X_AXIS));
	break;
      case  2: TmpString.sprintf("%.3f", Coords->coo().at(Y_AXIS));
	break;
      case  3: TmpString.sprintf("%.3f", Coords->coo().at(Z_AXIS));
	break;
      case  4: TmpString.sprintf("%.3f", Coords->cooErrors().at(X_AXIS));
	break;
      case  5: TmpString.sprintf("%.3f", Coords->cooErrors().at(Y_AXIS));
	break;
      case  6: TmpString.sprintf("%.3f", Coords->cooErrors().at(Z_AXIS));
	break;
      case  7: TmpString.sprintf("%.3f", Coords->vel().at(X_AXIS));
	break;
      case  8: TmpString.sprintf("%.3f", Coords->vel().at(Y_AXIS));
	break;
      case  9: TmpString.sprintf("%.3f", Coords->vel().at(Z_AXIS));
	break;
      case 10: TmpString.sprintf("%.3f", Coords->velErrors().at(X_AXIS));
	break;
      case 11: TmpString.sprintf("%.3f", Coords->velErrors().at(Y_AXIS));
	break;
      case 12: TmpString.sprintf("%.3f", Coords->velErrors().at(Z_AXIS));
	break;
      case 13: TmpString.sprintf("%s", Coords->isUseVels()?"Y":"N");
	break;
      case 14: TmpString = Coords->eventName();
	break;
      };
  return TmpString;
};

QString SBCoordsListItem::key(int col, bool) const
{
  TmpString = "NULL";

  if (Coords)
    switch (col)
      {
      case  0: TmpString = Coords->toString(SBMJD::F_INTERNAL);
	break;
      case  1: TmpString.sprintf("%20.5f", Coords->coo().at(X_AXIS));
	break;
      case  2: TmpString.sprintf("%20.5f", Coords->coo().at(Y_AXIS));
	break;
      case  3: TmpString.sprintf("%20.5f", Coords->coo().at(Z_AXIS));
	break;
      case  4: TmpString.sprintf("%20.5f", Coords->cooErrors().at(X_AXIS));
	break;
      case  5: TmpString.sprintf("%20.5f", Coords->cooErrors().at(Y_AXIS));
	break;
      case  6: TmpString.sprintf("%20.5f", Coords->cooErrors().at(Z_AXIS));
	break;
      case  7: TmpString.sprintf("%20.5f", Coords->vel().at(X_AXIS));
	break;
      case  8: TmpString.sprintf("%20.5f", Coords->vel().at(Y_AXIS));
	break;
      case  9: TmpString.sprintf("%20.5f", Coords->vel().at(Z_AXIS));
	break;
      case 10: TmpString.sprintf("%20.5f", Coords->velErrors().at(X_AXIS));
	break;
      case 11: TmpString.sprintf("%20.5f", Coords->velErrors().at(Y_AXIS));
	break;
      case 12: TmpString.sprintf("%20.5f", Coords->velErrors().at(Z_AXIS));
	break;
      case 13: TmpString.sprintf("%s", Coords->isUseVels()?"A":"B");
	break;
      case 14: TmpString = Coords->eventName();
	break;
      };
  return TmpString;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBStuffStationsEcc..										*/
/*												*/
/*==============================================================================================*/
SBStuffStationsEcc::SBStuffStationsEcc(SB_TRF *Wtrf_, QWidget* parent, const char* name, bool modal, WFlags f)
  : QDialog(parent, name, modal, f)
{
  numOpenedWindows = 0;
  
  setCaption("Eccentricities");

  Wtrf	 = Wtrf_;
  Ecc    = &Wtrf->ecc();
  
  QPushButton	*AddButton, *DeleteButton, *OKButton, *CancelButton, *ImportButton, *ExportButton;
  QSize		Size;
  QGroupBox	*gbox;
  QBoxLayout	*Layout, *SubLayout;
  
  gbox	 = new QGroupBox("Ecc Database", this);
  Layout = new QVBoxLayout(gbox, 20, 10);
  
  // set up list box:
  EccLV = new QListView(gbox);
  EccLV -> addColumn("Station ID");
  EccLV -> setColumnAlignment( 0, AlignRight);
  EccLV -> addColumn("DBH Name");
  EccLV -> setColumnAlignment( 1, AlignLeft);
  EccLV -> addColumn("Start");
  EccLV -> setColumnAlignment( 2, AlignRight);
  EccLV -> addColumn("Finis");
  EccLV -> setColumnAlignment( 3, AlignRight);
  EccLV -> addColumn("Type");
  EccLV -> setColumnAlignment( 4, AlignCenter);
  EccLV -> addColumn("Session");
  EccLV -> setColumnAlignment( 5, AlignRight);
  EccLV -> addColumn("X/N");
  EccLV -> setColumnAlignment( 6, AlignRight);
  EccLV -> addColumn("Y/E");
  EccLV -> setColumnAlignment( 7, AlignRight);
  EccLV -> addColumn("Z/U");
  EccLV -> setColumnAlignment( 8, AlignRight);

  for (SBStationEcc *StEcc = Ecc->first(); StEcc; StEcc = Ecc->next())
    {
      for (SBEccentricity *E = StEcc->eccs().first(); E; E = StEcc->eccs().next())
	{
	  (void) new SBEccListItem(EccLV, StEcc, E);
	};
    };

  EccLV -> setAllColumnsShowFocus(TRUE);
  EccLV -> setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  EccLV -> setMinimumSize(EccLV->sizeHint());
  EccLV -> setFocusPolicy(QWidget::StrongFocus);
  if (Ecc->count() && EccLV->firstChild())
    EccLV -> setSelected(EccLV->firstChild(), TRUE);
  Layout   -> addWidget(EccLV, 10);

  Layout->activate();
  

  //---
  Layout = new QVBoxLayout(this, 10, 10);
  Layout->addWidget(gbox, 10);
  Layout->addLayout(SubLayout = new QHBoxLayout(10));
  
  SubLayout->addStretch(1);
  SubLayout->addWidget(OKButton = new QPushButton("OK", this));
  SubLayout->addWidget(AddButton = new QPushButton("Add", this));
  SubLayout->addWidget(DeleteButton = new QPushButton("Delete", this));
  SubLayout->addWidget(ImportButton = new QPushButton("Import..", this));
  SubLayout->addWidget(ExportButton = new QPushButton("Export", this));
  SubLayout->addWidget(CancelButton = new QPushButton("Cancel", this));

  OKButton	-> setFocusPolicy(QWidget::ClickFocus);
  AddButton	-> setFocusPolicy(QWidget::ClickFocus);
  DeleteButton	-> setFocusPolicy(QWidget::ClickFocus);
  ImportButton	-> setFocusPolicy(QWidget::ClickFocus);
  ExportButton	-> setFocusPolicy(QWidget::ClickFocus);
  CancelButton	-> setFocusPolicy(QWidget::ClickFocus);

  ImportButton	-> setMinimumSize(Size=ImportButton->sizeHint());
  OKButton	-> setMinimumSize(Size);
  AddButton	-> setMinimumSize(Size);
  DeleteButton	-> setMinimumSize(Size);
  ExportButton	-> setMinimumSize(Size);
  CancelButton	-> setMinimumSize(Size);

  Layout->activate();

  connect(OKButton,	SIGNAL(clicked()), SLOT(accept()));
  connect(CancelButton, SIGNAL(clicked()), SLOT(reject()));
  connect(AddButton,	SIGNAL(clicked()), SLOT(insertEntry()));
  connect(DeleteButton, SIGNAL(clicked()), SLOT(deleteEntry()));
  connect(ImportButton, SIGNAL(clicked()), SLOT(importData()));
  connect(ExportButton, SIGNAL(clicked()), SLOT(exportData()));

  connect (EccLV,   SIGNAL(doubleClicked(QListViewItem*)), SLOT(editEntry(QListViewItem*)));
  connect (EccLV  , SIGNAL(returnPressed(QListViewItem*)), SLOT(editEntry(QListViewItem*)));

  isModified = FALSE;
};

SBStuffStationsEcc::~SBStuffStationsEcc()
{
  emit eccModified(isModified);
};

void SBStuffStationsEcc::accept()
{
  if (numOpenedWindows) return; //there are open windows dependent upon us..

  delete this;
};

void SBStuffStationsEcc::reject() 
{
  if (numOpenedWindows) return;
  QDialog::reject();
  delete this;
};

void SBStuffStationsEcc::importData() // quick and dirty
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": import selected");
  
  bool IsLoaded = FALSE;
  
  if (numOpenedWindows) return; //there are opened windows dependent upon us..

  QString FileName( QFileDialog::getOpenFileName(SetUp->path2Import(), "*.ecc *.ECC") );
  numOpenedWindows++;
  if (FileName.length()>0)
    {
      Log->write(SBLog::DBG, SBLog::STATION, ClassName() + ": importing the file `" + FileName + "'");
      IsLoaded = Wtrf->ecc().importEccDat(FileName, Wtrf);
    };
  
  if (IsLoaded)
    {
      isModified = TRUE;

    };
  numOpenedWindows--;
};

void SBStuffStationsEcc::exportData()
{
};

void SBStuffStationsEcc::editEntry(QListViewItem* /*SiteItem*/)
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": editEntry selected");
};
  
void SBStuffStationsEcc::insertEntry()
{  
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": insertEntry selected");
};
  
void SBStuffStationsEcc::deleteEntry()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": deleteEntry selected");

  SBEccListItem* EccItem = (SBEccListItem*)EccLV->currentItem();
  SBStationEcc*	 StEcc = EccItem->stEcc();
  SBEccentricity* Ecc  = EccItem->ecc();

  if (EccItem && StEcc && Ecc)
    {
      SBEccListItem* EccNextItem = (SBEccListItem*)EccItem->itemBelow();
      if (!EccNextItem) EccNextItem = (SBEccListItem*)EccItem->itemAbove();

      QString Str;
      Str = StEcc->toString().copy() + " (" + Ecc->aka() + ") " +
	"[" + Ecc->tStart().toString(SBMJD::F_DateShort) + ":" + Ecc->tFinis().toString(SBMJD::F_DateShort) + "]";
      
      if (!QMessageBox::warning(this, "Delete?", "Are you sure to delete an eccentricity entry\n \""
				+ Str + "\"?\n", "Yes, del them!", "No, let it live.."))
	{
	  if (StEcc->eccs().remove(Ecc))
	    {
	      delete EccItem;
	      if (EccNextItem) EccLV->setSelected(EccNextItem, TRUE);
	      Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + 
			 ": " + Str + ", rest in peace..");
	      isModified = TRUE;
	    }
	  else
	    Log->write(SBLog::ERR, SBLog::INTERFACE, ClassName() + 
		       ": cannot remove Ecc (" + Str + ") from the list");
	}
      else
	Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() +
		   ": relax, `" + Str + "', it's just a joke.. ");
    };

};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBStuffStations..										*/
/*												*/
/*==============================================================================================*/
SBStuffStations::SBStuffStations(QWidget* parent, const char* name, bool modal, WFlags f)
  : QDialog(parent, name, modal, f)
{
  numOpenedWindows = 0;
  
  setCaption("Sites");
  
  QPushButton	*AddButton, *DeleteButton, *OKButton, *CancelButton, *ImportButton, *ExportButton;
  QPushButton	*EccButton;
  QSize		Size;
  QGroupBox	*gbox;
  QBoxLayout	*Layout, *SubLayout, *aLayout, *bLayout;
  QLabel	*label;
  QFrame	*frame;
  
  if (!(Wtrf=SetUp->loadTRF())) Wtrf = new SB_TRF;
  if (!Wtrf->count()) 
    {
      SBSite *Site = new SBSite;
      Site->addStation(new SBStation(Site));
      Wtrf->inSort(Site);
    };
  
  gbox	 = new QGroupBox("TRF", this);
  Layout = new QVBoxLayout(gbox, 20, 10);
  
  SubLayout = new QHBoxLayout(10);
  Layout -> addLayout(SubLayout, 10);
  
  
  //--
  aLayout = new QVBoxLayout(10);
  SubLayout -> addLayout(aLayout, 10);
  
  
  // set up aliases list box:
  AliasesLV = new QListView(gbox);
  AliasesLV -> addColumn(" Station's Alias ");
  AliasesLV -> addColumn("  Refers to ");
  AliasesLV -> setColumnAlignment( 1, AlignLeft);

  QDictIterator<QString> it(*Wtrf->aliasDict());
  while (it.current())
    {
      (void) new SBAliasListItem(AliasesLV, QString(it.currentKey()), *it.current());
      ++it;
    };
  AliasesLV -> setAllColumnsShowFocus(TRUE);
  AliasesLV -> setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  AliasesLV -> setMinimumSize(AliasesLV->sizeHint());
  AliasesLV -> setFocusPolicy(QWidget::StrongFocus);
  if (Wtrf->aliasDict()->count() && AliasesLV->firstChild())
    AliasesLV -> setSelected(AliasesLV->firstChild(), TRUE);
  aLayout   -> addWidget(AliasesLV, 10);

  //and it's buttons:
  aLayout->addLayout(bLayout = new QHBoxLayout(4));
  bLayout->addStretch(1);
  bLayout->addWidget(AddButton = new QPushButton("Add..", gbox));
  bLayout->addWidget(DeleteButton = new QPushButton("Delete", gbox));

  DeleteButton->setMinimumSize(Size=DeleteButton->sizeHint());
  AddButton->setMinimumSize(Size);
  AddButton->setFocusPolicy(QWidget::ClickFocus);
  DeleteButton->setFocusPolicy(QWidget::ClickFocus);

  connect (AddButton, SIGNAL(clicked()), SLOT(insertEntryA()));
  connect (DeleteButton, SIGNAL(clicked()), SLOT(deleteEntryA()));

  //info's fields:
  QGridLayout  *grid = new QGridLayout(5,3);
  aLayout -> addLayout(grid);
  grid -> setColStretch(1,5);
  
  frame = new QFrame(gbox);
  frame -> setFrameStyle(QFrame::Sunken | QFrame::HLine);
  frame -> setMinimumHeight(6);
  grid-> addMultiCellWidget(frame, 0, 0, 0, 2);
  
  // info:
  label	= new QLabel("Sites/Aliases:", gbox);
  label	-> setMinimumSize(label->sizeHint());
  grid -> addMultiCellWidget(label, 1,1, 0,1);

  str.sprintf(" %d/%d", Wtrf->count(), Wtrf->aliasDict()->count());
  NumSA	= new QLabel(str, gbox);
  NumSA	-> setMinimumSize(NumSA->sizeHint());
  grid -> addWidget(NumSA, 1, 2);

  frame = new QFrame(gbox);
  frame -> setFrameStyle(QFrame::Sunken | QFrame::HLine);
  frame -> setMinimumHeight(6);
  grid-> addMultiCellWidget(frame, 2, 2, 0, 2);

  label	= new QLabel("Label:", gbox);
  label	-> setMinimumSize(label->sizeHint());
  grid -> addWidget(label, 3, 0);

  TRFLabel = new QLineEdit(gbox);
  TRFLabel -> setText(Wtrf->label().toString());
  TRFLabel -> setMinimumHeight(TRFLabel->sizeHint().height());
  TRFLabel -> setMinimumWidth (TRFLabel->fontMetrics().width(Wtrf->label().toString()) + 10);
  grid -> addMultiCellWidget(TRFLabel, 3,3, 1,2);

  label	= new QLabel("Epoch:", gbox);
  label	-> setMinimumSize(label->sizeHint());
  grid -> addWidget(label, 4, 0);

  TRFEpoch = new QLineEdit(gbox);
  TRFEpoch -> setText(Wtrf->epoch().toString(SBMJD::F_Year));
  TRFEpoch -> setValidator(new QDoubleValidator(1900.0, 2200.0, 1, gbox));
  TRFEpoch -> setMinimumHeight(TRFEpoch->sizeHint().height());
  TRFEpoch -> setMinimumWidth (TRFEpoch->fontMetrics().width(Wtrf->label().toString()) + 10);
  grid -> addMultiCellWidget(TRFEpoch, 4,4, 1,2);
  //--  

  

  //  set up sources list box:
  SitesLV = new QListView(gbox);
  SitesLV -> addColumn("DOMES");
  SitesLV -> setColumnAlignment( 0, AlignLeft);
  SitesLV -> addColumn("Name");
  SitesLV -> setColumnAlignment( 1, AlignLeft);
  SitesLV -> addColumn("Country");
  SitesLV -> setColumnAlignment( 2, AlignLeft);
  SitesLV -> addColumn("Plate");
  SitesLV -> setColumnAlignment( 3, AlignLeft);
  SitesLV -> addColumn("Long");
  SitesLV -> setColumnAlignment( 4, AlignRight);
  SitesLV -> addColumn("Lat");
  SitesLV -> setColumnAlignment( 5, AlignRight);
  SitesLV -> addColumn("Height");
  SitesLV -> setColumnAlignment( 6, AlignRight);
  SitesLV -> addColumn("OLoad");
  SitesLV -> setColumnAlignment( 7, AlignCenter);
  SitesLV -> addColumn("Num");
  SitesLV -> setColumnAlignment( 8, AlignRight);

  for (SBSite* W=Wtrf->first(); W; W=Wtrf->next())
    (void) new SBSiteListItem(SitesLV, W);
  SitesLV -> setAllColumnsShowFocus(TRUE);
  SitesLV -> setMinimumSize(SitesLV->sizeHint());
  SitesLV -> setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  if (SitesLV->firstChild())
  SitesLV -> setSelected(SitesLV->firstChild(), TRUE);
  SitesLV -> setFocusPolicy(QWidget::StrongFocus);
  SubLayout -> addWidget(SitesLV,5);

  // comments:
  bLayout = new QHBoxLayout(10);
  Layout -> addLayout(bLayout);

  label	= new QLabel("Comment:", gbox);
  label	-> setMinimumSize(label->sizeHint());
  bLayout-> addWidget(label);

  TRFComment = new QLineEdit(gbox);
  TRFComment -> setText(Wtrf->comment());
  TRFComment -> setMinimumHeight(TRFComment->sizeHint().height());
  bLayout    -> addWidget(TRFComment, 1);
  Layout->activate();
  

  //---
  Layout = new QVBoxLayout(this, 10, 10);
  Layout->addWidget(gbox, 10);
  Layout->addLayout(SubLayout = new QHBoxLayout(10));
  
  SubLayout->addStretch(1);
  SubLayout->addWidget(OKButton	    = new QPushButton("OK", this));
  SubLayout->addWidget(EccButton    = new QPushButton("Eccentricities", this));
  SubLayout->addWidget(AddButton    = new QPushButton("Add", this));
  SubLayout->addWidget(DeleteButton = new QPushButton("Delete", this));
  SubLayout->addWidget(ImportButton = new QPushButton("Import..", this));
  SubLayout->addWidget(ExportButton = new QPushButton("Export", this));
  SubLayout->addWidget(CancelButton = new QPushButton("Cancel", this));

  OKButton	-> setFocusPolicy(QWidget::ClickFocus);
  EccButton	-> setFocusPolicy(QWidget::ClickFocus);
  AddButton	-> setFocusPolicy(QWidget::ClickFocus);
  DeleteButton	-> setFocusPolicy(QWidget::ClickFocus);
  ImportButton	-> setFocusPolicy(QWidget::ClickFocus);
  ExportButton	-> setFocusPolicy(QWidget::ClickFocus);
  CancelButton	-> setFocusPolicy(QWidget::ClickFocus);

  EccButton	-> setMinimumSize(Size=EccButton->sizeHint());
  ImportButton	-> setMinimumSize(Size);
  OKButton	-> setMinimumSize(Size);
  AddButton	-> setMinimumSize(Size);
  DeleteButton	-> setMinimumSize(Size);
  ExportButton	-> setMinimumSize(Size);
  CancelButton	-> setMinimumSize(Size);

  Layout->activate();

  connect(OKButton,	SIGNAL(clicked()), SLOT(accept()));
  connect(CancelButton, SIGNAL(clicked()), SLOT(reject()));
  connect(EccButton,	SIGNAL(clicked()), SLOT(browseEcc()));
  connect(AddButton,	SIGNAL(clicked()), SLOT(insertEntryS()));
  connect(DeleteButton, SIGNAL(clicked()), SLOT(deleteEntryS()));
  connect(ImportButton, SIGNAL(clicked()), SLOT(importData()));
  connect(ExportButton, SIGNAL(clicked()), SLOT(exportData()));

  connect (SitesLV,   SIGNAL(doubleClicked(QListViewItem*)), SLOT(editEntryS(QListViewItem*)));
  connect (AliasesLV, SIGNAL(doubleClicked(QListViewItem*)), SLOT(editEntryA(QListViewItem*)));
  connect (SitesLV  , SIGNAL(returnPressed(QListViewItem*)), SLOT(editEntryS(QListViewItem*)));
  connect (AliasesLV, SIGNAL(returnPressed(QListViewItem*)), SLOT(editEntryA(QListViewItem*)));

  isModifiedTRF = FALSE;
};

SBStuffStations::~SBStuffStations()
{
  if (Wtrf) delete Wtrf;
};

void SBStuffStations::accept()
{
  if (numOpenedWindows) return; //there are open windows dependent upon us..

  bool		isNeed2Save = isModifiedTRF;
  QString	a;
  SBMJD		date;

  QDialog::accept();

  str = TRFLabel->text();
  SBSolutionName *w = new SBSolutionName(SBSolutionName::TYPE_SSC);
  w->fromString(str);
  if (*w!=Wtrf->label())
    {
      Wtrf->setLabel(*w);
      isNeed2Save = TRUE;
    };
  delete w;

  str = TRFEpoch->text();
  if (Wtrf->epoch().toString(SBMJD::F_Year) != str)
    {
      date.setMJD(SBMJD::F_Year, str);
      Wtrf->setEpoch(date);
      isNeed2Save = TRUE;
    };

  if (Wtrf->comment() != TRFComment->text())
    {
      Wtrf->setComment(TRFComment->text());
      isNeed2Save = TRUE;
    };

  if (isNeed2Save) SetUp->saveTRF(Wtrf);
  delete this;
};

void SBStuffStations::reject() 
{
  if (numOpenedWindows) return;
  QDialog::reject();
  delete this;
};

void SBStuffStations::reloadTRF()
{
  AliasesLV -> clear();
  SitesLV   -> clear();

  if (Wtrf) delete Wtrf;
  if (!(Wtrf=SetUp->loadTRF())) Wtrf = new SB_TRF;
  if (!Wtrf->count()) 
    {    
      SBSite *Site = new SBSite;
      Site->addStation(new SBStation(Site));
      Wtrf->inSort(Site);
    };
  QListViewItem *Q;

  // redraw aliases:
  QDictIterator<QString> it(*Wtrf->aliasDict());
  while (it.current())
    {
      Q = new SBAliasListItem(AliasesLV, QString(it.currentKey()), *it.current());
      ++it;
    };
  if (Wtrf->aliasDict()->count() && AliasesLV->firstChild())
    AliasesLV->setSelected(AliasesLV->firstChild(), TRUE);


  // redraw sources:
  for (SBSite* W=Wtrf->first(); W; W=Wtrf->next())
    Q = new SBSiteListItem(SitesLV, W); // use Q just to escape g++ warning
  if (SitesLV->firstChild())
    SitesLV -> setSelected(SitesLV->firstChild(), TRUE);
  
  drawInfo();
};

void SBStuffStations::drawInfo()
{
  TRFLabel -> setText(Wtrf->label().toString());
  TRFLabel -> setMinimumHeight(TRFLabel->sizeHint().height());
  TRFLabel -> setMinimumWidth (TRFLabel->fontMetrics().width(Wtrf->label().toString())+10);
  TRFEpoch -> setText(Wtrf->epoch().toString(SBMJD::F_Year));
  TRFComment -> setText(Wtrf->comment());

  NumSA	-> setText(str.sprintf("%d/%d", Wtrf->count(), Wtrf->aliasDict()->count()));
};

void SBStuffStations::browseEcc()
{
  if (numOpenedWindows) return; //there are dependants..
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": browseEcc selected");
  
  SBStuffStationsEcc *EccWin = new SBStuffStationsEcc(Wtrf);
  connect (EccWin, SIGNAL(eccModified(bool)), SLOT(TRFModified(bool)));
  numOpenedWindows++;
  EccWin->show();
};

void SBStuffStations::importData()
{
  if (numOpenedWindows) return; //there are dependants..
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": SBStuffStationsImport selected");  

  if (isModifiedTRF) 
    {
      SetUp->saveTRF(Wtrf);
      isModifiedTRF = FALSE;
    };
  
  SBStationImport *importWin = new SBStationImport();
  connect (importWin, SIGNAL(stationsImported()), SLOT(reloadTRF()));
  importWin->show();
};

void SBStuffStations::exportData()
{
  static const QString suffix[] = 
  {".site", "_dir.sta", ".SSCA", ".SSCB", ".SSCC", ".SSCD", ".SSCZ", ".blq", ".blqReq"};

  QFile		f;
  QTextStream	*s=NULL;
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": SBStuffStationsExport selected");

  for (int i=0; i<9; i++)
    {
      f.setName(SetUp->path2Output() + SetUp->stations() + suffix[i]);
      if (f.open(IO_WriteOnly))
	{      
	  switch (i)
	    {
	    case 0:  s = new SBTS_site1994; break;
	    case 1:  s = new SBTS_dir1994 ; break;
	    case 2:  s = new SBTS_ssc1994(SBStation::Class_A); break;
	    case 3:  s = new SBTS_ssc1994(SBStation::Class_B); break;
	    case 4:  s = new SBTS_ssc1994(SBStation::Class_C); break;
	    case 5:  s = new SBTS_ssc1994(SBStation::Class_D); break;
	    case 6:  s = new SBTS_ssc1994(SBStation::Class_Z); break;
	    case 7:  s = new SBTS_blq; break;
	    case 8:  s = new SBTS_blqReq; break;
	    };
	  s->setDevice(&f);
	  switch (i)
	    {
	    case 0:  *(SBTS_site1994*)s << *Wtrf; break;
	    case 1:  *(SBTS_dir1994* )s << *Wtrf; break;
	    case 7:  *(SBTS_blq* )s << *Wtrf; break;
	    case 8:  *(SBTS_blqReq* )s << *Wtrf; break;
	    default: *(SBTS_ssc1994* )s << *Wtrf; break;
	    };
	  f.close();
	  s->unsetDevice();
	  if (s) delete s;
	  Log->write(SBLog::DBG, SBLog::IO, ClassName() + ": file "
		     + (QString)f.name() + " has been saved");
	}
      else    
	Log->write(SBLog::ERR, SBLog::IO, ClassName() + ": could not write " + (QString)f.name());
    };
};

void SBStuffStations::editEntryA(QListViewItem* AkaItem)
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": SBStuffStationEditAlias selected");

  SBAliasEditor *AEditor =  new SBAliasEditor ((SBAliasListItem*)AkaItem, Wtrf);
  connect (AEditor, SIGNAL(AliasModified(bool)), SLOT(TRFModified(bool)));
  numOpenedWindows++;
  AEditor->show();
};

void SBStuffStations::insertEntryA()
{  
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": SBStuffStationInsertAlias selected");

  SBAliasEditor *AEditor =  new SBAliasEditor (AliasesLV, Wtrf);
  connect (AEditor, SIGNAL(AliasModified(bool)), SLOT(TRFModified(bool)));
  numOpenedWindows++;
  AEditor->show();
};

void SBStuffStations::deleteEntryA()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": SBStuffStationDeleteAlias selected");
  if (numOpenedWindows) return; //to avoid deletion of an entry being edited..
  
  SBAliasListItem* AkaItem = (SBAliasListItem*)AliasesLV->currentItem();
  if (AkaItem)
    {
      QListViewItem* AkaNextItem = AkaItem->itemBelow();
      if (!AkaNextItem) AkaNextItem = AkaItem->itemAbove();
      str = (AkaItem->Alias + "<-->" + AkaItem->Reference).copy();
      if (!QMessageBox::warning(this, "Delete?", "Are you sure to delete an alias entry\n \""
				+ str + "\"?\n", "Yes, del them!", "No, let it live.."))
	{
	  if (Wtrf->aliasDict()->remove(str.left(8)))
	    {
	      delete AkaItem;
	      if (AkaNextItem) AliasesLV->setSelected(AkaNextItem, TRUE);
	      Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + 
			 ": " + str + ", rest in peace..");
	      isModifiedTRF = TRUE;
	      NumSA -> setText(str.sprintf("%d/%d", Wtrf->count(), Wtrf->aliasDict()->count()));
	    }
	  else
	    Log->write(SBLog::ERR, SBLog::INTERFACE, ClassName() + 
		       ": `" + str + "' not found in aliases");
	}
      else
	Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() +  
		   ": relax, `" + str + "', it's just a joke.. ");
    };
};

void SBStuffStations::editEntryS(QListViewItem* SiteItem)
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": SBStuffStationEditSite selected");
  
  SBSiteEditor *SEditor =  new SBSiteEditor((SBSiteListItem*)SiteItem);
  connect (SEditor, SIGNAL(siteModified(bool)), SLOT(TRFModified(bool)));
  numOpenedWindows++;
  SEditor->show();
};
  
void SBStuffStations::insertEntryS()
{  
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": SBStuffStationInsertSite selected");
  
  SBSiteEditor *SEditor =  new SBSiteEditor(SitesLV, Wtrf);
  connect (SEditor, SIGNAL(siteModified(bool)), SLOT(TRFModified(bool)));
  numOpenedWindows++;
  SEditor->show();
};
  
void SBStuffStations::deleteEntryS()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": SBStuffStationDeleteSite selected");

  SBSiteListItem* SiteItem = (SBSiteListItem*)SitesLV->currentItem();
  if (SiteItem)
    {
      QListViewItem* SiteNextItem = SiteItem->itemBelow();
      if (!SiteNextItem) SiteNextItem = SiteItem->itemAbove();
      str = SiteItem->Site->name().copy();
      if (!QMessageBox::warning(this, "Delete?", "Are you sure to delete site entry\n \""
				+ str + "\"?\n", "Yes, del them!", "No, let it live.."))
	{
	  if (Wtrf->remove(SiteItem->Site))
	    {
	      delete SiteItem;
	      if (SiteNextItem) SitesLV->setSelected(SiteNextItem, TRUE);
	      Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + 
			 ": " + str + ", rest in peace..");
	      isModifiedTRF = TRUE;
	      str.sprintf("%d/%d", Wtrf->count(), Wtrf->aliasDict()->count());
	      NumSA -> setText(str);
	    }
	  else
	    Log->write(SBLog::ERR, SBLog::INTERFACE, ClassName() + 
		       ": " + str + " not found in database");
	}
      else
	Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": relax, " + str + 
		   ", it's just a joke.. ");
      
    };
};

void SBStuffStations::TRFModified(bool Modified)
{
  if (!numOpenedWindows) 
    std::cerr << "your Editor's windows are diyng often than have been born\n";
  numOpenedWindows--;
  if (Modified) 
    {
      isModifiedTRF = TRUE;
      NumSA->setText(str.sprintf(" %d/%d", Wtrf->count(), Wtrf->aliasDict()->count()));
    };
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBStationImport..										*/
/*												*/
/*==============================================================================================*/
SBStationImport::SBStationImport(QWidget* parent, const char* name, bool modal, WFlags f)
  : QWizard(parent, name, modal, f), NumOf1994Files(6), NumOf2000Files(6), NumOfAuxFiles(3)
{
  FFormat = F_UNKN;
  Mode    = REPLACE;
  Filter  = FILT_COO;

  setupPage1();
  setupPage2_1();
  setupPage2_2();
  setupPage3();
  setupPage4();
};

void SBStationImport::formatChanged(int id)
{
  if (page2_1)
    setAppropriate(page2_1, FALSE);
  if (page2_2)
    setAppropriate(page2_2, FALSE);
  
  switch (id)
    {
    case 0:   
      FFormat = F_ITRF94;
      if (page2_1)
	setAppropriate(page2_1, TRUE);
      break;
    case 1:   FFormat = F_ITRF96;
      break;
    case 2:   FFormat = F_ITRF97;
      break;
    case 3:   
      FFormat = F_ITRF2000;
      if (page2_2)
	setAppropriate(page2_2, TRUE);
      break;
    case 4:   FFormat = F_SINEX;
      break;
    default:  FFormat = F_UNKN;
      break;
    };

  if ( (FFormat != F_UNKN) && (FFormat != F_ITRF96) && (FFormat != F_ITRF97) && (FFormat != F_SINEX) )
    setNextEnabled(page1, TRUE);
  else
    setNextEnabled(page1, FALSE);
};

void SBStationImport::modeChanged(int id)
{
  switch (id)
    {
    case 0:   Mode = REPLACE;
      break;
    case 1:   Mode = APP_ONLY;
      break;
    case 2:   Mode = APP_UPD;
      break;
    case 3:   Mode = UPD_ONLY;
      break;
    };
};

void SBStationImport::filterChanged(int id)
{
  switch (id)
    {
    case 0:   Filter = FILT_COO;
      break;
    case 1:   Filter = FILT_TECH;
      break;
    case 2:   Filter = FILT_VLBI;
      break;
    case 3:   Filter = FILT_ALL;
      break;
    };
};

bool SBStationImport::checkFileExists(const QString& fil)
{
  QDir dir(fil);
  return dir.exists(fil);
};

void SBStationImport::setupPage1()
{
  static const QString sFormat[]=
  {
    "SSC and SSV form the ITRF-94 solution",
    "SINEX data form the ITRF-96 solution",
    "SINEX data form the ITRF-97 solution",
    "SSC and SSV form the ITRF-2000 solution",
    "General SINEX data"
  };
  QRadioButton	*rbFormat[5];

  page1	= new QWidget(this);
  QBoxLayout	*Layout=new QVBoxLayout(page1, 20,10);


  QButtonGroup *gbox = new QVButtonGroup("TRF known formats", page1);
  for (int i=0; i<5; i++)
    {
      rbFormat[i] = new QRadioButton(sFormat[i], gbox);
      rbFormat[i] -> setMinimumSize(rbFormat[i]->sizeHint());
    };

  Layout->addStretch(1);
  Layout->addWidget(gbox);
  Layout->addStretch(1);
  
  Layout->activate();
  addPage(page1, "Select format of imported files");
  
  connect (gbox, SIGNAL(clicked(int)), this, SLOT(formatChanged(int)));
  setNextEnabled(page1, FALSE);
};

void SBStationImport::setupPage2_1()
{
  static const	QString LabelCB[]=
  {
    "SITE file:",
    "DIR file:",
    "SSC class Z file:",
    "SSC class C file:",
    "SSC class B file:",
    "SSC class A file:"
  };
  static const	QString ImportFile[] = 
  {
    "iers.site", "iers_dir.sta", "ITRF94.SSCZ", "ITRF94.SSCC", "ITRF94.SSCB", "ITRF94.SSCA"
  };

  page2_1 = new QWidget(this);

  QGridLayout	*grid = new QGridLayout(page2_1,  NumOf1994Files+2,3,  20, 5);
  bool		IsNextEnabled = TRUE;

  for (int i=0; i<NumOf1994Files; i++)
    {
      cbFileName1994[i] = new QCheckBox(LabelCB[i], page2_1);
      cbFileName1994[i] -> setChecked(TRUE);
      cbFileName1994[i] -> setMinimumSize(cbFileName1994[i]->sizeHint());
      cbFileName1994[i] -> setFocusPolicy(QWidget::ClickFocus);
      grid -> addWidget(cbFileName1994[i], i+1, 0);
      
      sFileName1994[i] = SetUp->path2ImpITRF() + "itrf94/" + ImportFile[i];
      
      leFileName1994[i] = new QLineEdit(page2_1);
      leFileName1994[i] -> setText(sFileName1994[i]);
      leFileName1994[i] -> setMinimumHeight(leFileName1994[i]->sizeHint().height());
      leFileName1994[i] -> setMinimumWidth (leFileName1994[i]->fontMetrics().width(sFileName1994[i]) + 50);
      grid -> addWidget(leFileName1994[i], i+1, 1);
      
      bFile1994[i] = new QPushButton("Select file..", page2_1);
      bFile1994[i] -> setMinimumSize(bFile1994[i]->sizeHint());
      bFile1994[i] -> setFocusPolicy(QWidget::ClickFocus);
      grid -> addWidget(bFile1994[i], i+1, 2);

      IsNextEnabled = IsNextEnabled && checkFileExists(sFileName1994[i]);
    };
  
  connect(cbFileName1994[0], SIGNAL(toggled(bool)), SLOT(setSITEstate1994(bool)));
  connect(cbFileName1994[1], SIGNAL(toggled(bool)), SLOT(setDIRstate1994 (bool)));
  connect(cbFileName1994[2], SIGNAL(toggled(bool)), SLOT(setSSCZstate1994(bool)));
  connect(cbFileName1994[3], SIGNAL(toggled(bool)), SLOT(setSSCCstate1994(bool)));
  connect(cbFileName1994[4], SIGNAL(toggled(bool)), SLOT(setSSCBstate1994(bool)));
  connect(cbFileName1994[5], SIGNAL(toggled(bool)), SLOT(setSSCAstate1994(bool)));

  connect(bFile1994[0], SIGNAL(clicked()), SLOT(selectSITEfile1994()));
  connect(bFile1994[1], SIGNAL(clicked()), SLOT(selectDIRfile1994 ()));
  connect(bFile1994[2], SIGNAL(clicked()), SLOT(selectSSCZfile1994()));
  connect(bFile1994[3], SIGNAL(clicked()), SLOT(selectSSCCfile1994()));
  connect(bFile1994[4], SIGNAL(clicked()), SLOT(selectSSCBfile1994()));
  connect(bFile1994[5], SIGNAL(clicked()), SLOT(selectSSCAfile1994()));

  connect(leFileName1994[0], SIGNAL(textChanged(const QString&)), SLOT(fnSITEChanged1994(const QString&)));
  connect(leFileName1994[1], SIGNAL(textChanged(const QString&)), SLOT(fnDIRChanged1994 (const QString&)));
  connect(leFileName1994[2], SIGNAL(textChanged(const QString&)), SLOT(fnSSCZChanged1994(const QString&)));
  connect(leFileName1994[3], SIGNAL(textChanged(const QString&)), SLOT(fnSSCCChanged1994(const QString&)));
  connect(leFileName1994[4], SIGNAL(textChanged(const QString&)), SLOT(fnSSCBChanged1994(const QString&)));
  connect(leFileName1994[5], SIGNAL(textChanged(const QString&)), SLOT(fnSSCAChanged1994(const QString&)));
  
  grid -> setColStretch(1, 5);
  grid -> setRowStretch(0, 1);
  grid -> setRowStretch(NumOf1994Files+1, 1);
  grid -> activate();

  addPage(page2_1, "Files to Load");
  setNextEnabled(page2_1, IsNextEnabled);
};

void SBStationImport::setupPage2_2()
{
  static const	QString LabelCB[]=
  {
    "SITE file:",
    "DIR file:",
    "DORIS SSC file:",
    "GPS SSC file:",
    "SLR SSC file:",
    "VLBI SSC file:"
  };
  static const	QString ImportFile[] = 
  {
    "iers.site", "iers_dir.sta", 
    "ITRF2000_DORIS.SSC", "ITRF2000_GPS.SSC", "ITRF2000_SLR.SSC", "ITRF2000_VLBI.SSC"
  };

  page2_2 = new QWidget(this);

  QGridLayout	*grid = new QGridLayout(page2_2,  NumOf2000Files+2,3,  20, 5);
  bool		IsNextEnabled = TRUE;

  for (int i=0; i<NumOf2000Files; i++)
    {
      cbFileName2000[i] = new QCheckBox(LabelCB[i], page2_2);
      cbFileName2000[i] -> setChecked(TRUE);
      cbFileName2000[i] -> setMinimumSize(cbFileName2000[i]->sizeHint());
      cbFileName2000[i] -> setFocusPolicy(QWidget::ClickFocus);
      grid -> addWidget(cbFileName2000[i], i+1, 0);

      sFileName2000[i] = SetUp->path2ImpITRF() + "itrf2000/" + ImportFile[i];
      
      leFileName2000[i] = new QLineEdit(page2_2);
      leFileName2000[i] -> setText(sFileName2000[i]);
      leFileName2000[i] -> setMinimumHeight(leFileName2000[i]->sizeHint().height());
      leFileName2000[i] -> setMinimumWidth (leFileName2000[i]->fontMetrics().width(sFileName2000[i]) + 50);
      grid -> addWidget(leFileName2000[i], i+1, 1);
      
      bFile2000[i] = new QPushButton("Select file..", page2_2);
      bFile2000[i] -> setMinimumSize(bFile2000[i]->sizeHint());
      bFile2000[i] -> setFocusPolicy(QWidget::ClickFocus);
      grid -> addWidget(bFile2000[i], i+1, 2);

      IsNextEnabled = IsNextEnabled && checkFileExists(sFileName2000[i]);
    };
  
  connect(cbFileName2000[0], SIGNAL(toggled(bool)), SLOT(setSITEstate2000(bool)));
  connect(cbFileName2000[1], SIGNAL(toggled(bool)), SLOT(setDIRstate2000 (bool)));
  connect(cbFileName2000[2], SIGNAL(toggled(bool)), SLOT(setSSCZstate2000(bool)));
  connect(cbFileName2000[3], SIGNAL(toggled(bool)), SLOT(setSSCCstate2000(bool)));
  connect(cbFileName2000[4], SIGNAL(toggled(bool)), SLOT(setSSCBstate2000(bool)));
  connect(cbFileName2000[5], SIGNAL(toggled(bool)), SLOT(setSSCAstate2000(bool)));

  connect(bFile2000[0], SIGNAL(clicked()), SLOT(selectSITEfile2000()));
  connect(bFile2000[1], SIGNAL(clicked()), SLOT(selectDIRfile2000 ()));
  connect(bFile2000[2], SIGNAL(clicked()), SLOT(selectSSCZfile2000()));
  connect(bFile2000[3], SIGNAL(clicked()), SLOT(selectSSCCfile2000()));
  connect(bFile2000[4], SIGNAL(clicked()), SLOT(selectSSCBfile2000()));
  connect(bFile2000[5], SIGNAL(clicked()), SLOT(selectSSCAfile2000()));
  
  connect(leFileName2000[0], SIGNAL(textChanged(const QString&)), SLOT(fnSITEChanged2000(const QString&)));
  connect(leFileName2000[1], SIGNAL(textChanged(const QString&)), SLOT(fnDIRChanged2000 (const QString&)));
  connect(leFileName2000[2], SIGNAL(textChanged(const QString&)), SLOT(fnSSCZChanged2000(const QString&)));
  connect(leFileName2000[3], SIGNAL(textChanged(const QString&)), SLOT(fnSSCCChanged2000(const QString&)));
  connect(leFileName2000[4], SIGNAL(textChanged(const QString&)), SLOT(fnSSCBChanged2000(const QString&)));
  connect(leFileName2000[5], SIGNAL(textChanged(const QString&)), SLOT(fnSSCAChanged2000(const QString&)));

  grid -> setColStretch(1, 5);
  grid -> setRowStretch(0, 1);
  grid -> setRowStretch(NumOf2000Files+1, 1);
  grid -> activate();

  addPage(page2_2, "Files to Load");
  setNextEnabled(page2_2, IsNextEnabled);
};

void SBStationImport::setupPage3()
{
  static const QString LabelCB[]=
  {
    "IVS Network Station codes:",
    "Eccentricity vectors:",
    "Ocean load coefficients:"
  };
  static const QString ImportFile[] = {"ns-codes.txt", "ECCDAT.ecc", "olrs18.blq"};

  bool IsNextEnabled = TRUE;
  page3  = new QWidget(this);
  QGridLayout *grid = new QGridLayout(page3,  NumOfAuxFiles+2,3,  20, 5);

  for (int i=0; i<NumOfAuxFiles; i++)
    {
      cbFileNameAux[i] = new QCheckBox(LabelCB[i], page3);
      cbFileNameAux[i] -> setChecked(TRUE);
      cbFileNameAux[i] -> setMinimumSize(cbFileNameAux[i]->sizeHint());
      cbFileNameAux[i] -> setFocusPolicy(QWidget::ClickFocus);
      grid -> addWidget(cbFileNameAux[i], i+1, 0);

      sFileNameAux[i] = i!=2?SetUp->path2Import() + ImportFile[i]:SetUp->path2ImpOLoad() + ImportFile[i];
      
      leFileNameAux[i] = new QLineEdit(page3);
      leFileNameAux[i] -> setText(sFileNameAux[i]);
      leFileNameAux[i] -> setMinimumHeight(leFileNameAux[i]->sizeHint().height());
      leFileNameAux[i] -> setMinimumWidth (leFileNameAux[i]->fontMetrics().width(sFileNameAux[i]) + 50);
      grid -> addWidget(leFileNameAux[i], i+1, 1);
      
      bFileAux[i] = new QPushButton("Select file..", page3);
      bFileAux[i] -> setMinimumSize(bFileAux[i]->sizeHint());
      bFileAux[i] -> setFocusPolicy(QWidget::ClickFocus);
      grid -> addWidget(bFileAux[i], i+1, 2);

      IsNextEnabled = IsNextEnabled && checkFileExists(sFileNameAux[i]);
    };
  
  connect(cbFileNameAux[0], SIGNAL(toggled(bool)), SLOT(setNSCstateAux(bool)));
  connect(cbFileNameAux[1], SIGNAL(toggled(bool)), SLOT(setECCstateAux(bool)));
  connect(cbFileNameAux[2], SIGNAL(toggled(bool)), SLOT(setBLQstateAux(bool)));

  connect(bFileAux[0], SIGNAL(clicked()), SLOT(selectNSCfileAux()));
  connect(bFileAux[1], SIGNAL(clicked()), SLOT(selectECCfileAux()));
  connect(bFileAux[2], SIGNAL(clicked()), SLOT(selectBLQfileAux()));

  connect(leFileNameAux[0], SIGNAL(textChanged(const QString&)), SLOT(fnNSCChangedAux(const QString&)));
  connect(leFileNameAux[1], SIGNAL(textChanged(const QString&)), SLOT(fnECCChangedAux(const QString&)));
  connect(leFileNameAux[2], SIGNAL(textChanged(const QString&)), SLOT(fnBLQChangedAux(const QString&)));

  grid -> setColStretch(1, 5);
  grid -> setRowStretch(0, 1);
  grid -> setRowStretch(NumOfAuxFiles+1, 1);
  grid -> activate();

  addPage(page3, "Files to Load (auxiliary)");
  setNextEnabled(page3, IsNextEnabled);
};

void SBStationImport::setupPage4()
{
  static const QString sMode[]=
  {
    "Replace current database with imported one",
    "Import only new stations (which are not exist in current database)",
    "Insert new stations and update the present ones",
    "Just update present stations, ignore other info"
  };

  static const QString sFilter[]=
  {
    "At least coordinates are known",
    "At least technique is known",
    "VLBI stations whith known coordinates",
    "All stations"
  };

  page4	= new QWidget(this);
  QBoxLayout	*Layout=new QVBoxLayout(page4, 20,10);
  QRadioButton	*rbMode[4];
  QRadioButton	*rbFilter[4];

  QButtonGroup *gbox = new QVButtonGroup("Import mode", page4);
  for (int i=0; i<4; i++)
    {
      rbMode[i] = new QRadioButton(sMode[i], gbox);
      rbMode[i] -> setMinimumSize(rbMode[i]->sizeHint());
    };
  rbMode[Mode] -> setChecked(TRUE);
  connect (gbox, SIGNAL(clicked(int)), this, SLOT(modeChanged(int)));
  Layout->addWidget(gbox);

  gbox = new QVButtonGroup("Import filter", page4);
  for (int i=0; i<4; i++)
    {
      rbFilter[i] = new QRadioButton(sFilter[i], gbox);
      rbFilter[i] -> setMinimumSize(rbFilter[i]->sizeHint());
    };
  rbFilter[Filter] -> setChecked(TRUE);
  connect (gbox, SIGNAL(clicked(int)), this, SLOT(filterChanged(int)));
  Layout->addWidget(gbox);

  Layout -> activate();

  addPage(page4, "Import Options");
  setFinishEnabled(page4, TRUE);
};

void SBStationImport::selectFile1994(FileType Type)
{
  static const QString filter[] = 
  {"*.site", "*_dir.sta", "*.SSCZ", "*.SSCC", "*.SSCB", "*.SSCA"};
  QString str = QFileDialog::getOpenFileName(SetUp->path2ImpITRF()+ "itrf94/", filter[Type], this);
  if (!str.isNull()) 
    {
      sFileName1994 [Type]= str.copy();
      leFileName1994[Type]->setText(sFileName1994[Type]);
      checkPage2_1State();
    };
};

void SBStationImport::setState1994(FileType Type, bool T)
{
  leFileName1994[Type]->setEnabled(T);
  bFile1994     [Type]->setEnabled(T);
  checkPage2_1State();
};

void SBStationImport::fileNameChanged1994(FileType Type, const QString& /*FName*/)
{
  sFileName1994[Type] = leFileName1994[Type]->text();
  checkPage2_1State();
};

void SBStationImport::checkPage2_1State()
{
  bool IsNextEnabled = TRUE;
  for (int i=0; i<NumOf1994Files; i++)
    if (cbFileName1994[i]->isChecked())
      IsNextEnabled = IsNextEnabled && checkFileExists(sFileName1994[i]);
  setNextEnabled(page2_1, IsNextEnabled);
};

void SBStationImport::selectFile2000(FileType Type)
{
  static const QString filter[] = 
  {"*.site", "*_dir.sta", "*.SSC", "*.SSC", "*.SSC","*.SSC"};
  QString str = QFileDialog::getOpenFileName(SetUp->path2ImpITRF()+ "itrf2000/", filter[Type], this);
  if (!str.isNull()) 
    {
      sFileName2000 [Type]= str.copy();
      leFileName2000[Type]->setText(sFileName2000[Type]);
      checkPage2_2State();
    };
};

void SBStationImport::setState2000(FileType Type, bool T)
{
  leFileName2000[Type]->setEnabled(T);
  bFile2000     [Type]->setEnabled(T);
  checkPage2_2State();
};

void SBStationImport::fileNameChanged2000(FileType Type, const QString& /*FName*/)
{
  sFileName2000[Type] = leFileName2000[Type]->text();
  checkPage2_2State();
};

void SBStationImport::checkPage2_2State()
{
  bool IsNextEnabled = TRUE;
  for (int i=0; i<NumOf2000Files; i++)
    if (cbFileName2000[i]->isChecked())
      IsNextEnabled = IsNextEnabled && checkFileExists(sFileName2000[i]);
  setNextEnabled(page2_2, IsNextEnabled);
};
//
void SBStationImport::selectFileAux(int Type)
{
  static const QString filter[] = {"*.txt", "*.ecc", "*.blq"};
  QString str = Type!=2?
    QFileDialog::getOpenFileName(SetUp->path2Import(),   filter[Type], this):
    QFileDialog::getOpenFileName(SetUp->path2ImpOLoad(), filter[Type], this);
  if (!str.isNull())
    {
      sFileNameAux [Type] = str.copy();
      leFileNameAux[Type]->setText(sFileNameAux[Type]);
      checkPage3State();
    };
};

void SBStationImport::setStateAux(int Type, bool T)
{
  leFileNameAux[Type]->setEnabled(T);
  bFileAux     [Type]->setEnabled(T);
  checkPage3State();
};

void SBStationImport::fileNameChangedAux(int Type, const QString& /*FName*/)
{
  sFileNameAux[Type] = leFileNameAux[Type]->text();
  checkPage3State();
};

void SBStationImport::checkPage3State()
{
  bool IsNextEnabled = TRUE;
  for (int i=0; i<NumOfAuxFiles; i++)
    if (cbFileNameAux[i]->isChecked())
      IsNextEnabled = IsNextEnabled && checkFileExists(sFileNameAux[i]);
  setNextEnabled(page3, IsNextEnabled);
};

void SBStationImport::accept()
{
  QWizard::accept();
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": accept() selected");

  // importing
  switch (FFormat)
    {
    case F_ITRF94:
      importITRF();
      break;
    case F_ITRF96:
      Log->write(SBLog::WRN, SBLog::INTERFACE, ClassName() + 
		 ": Sorry, import of SINEX format not supported yet");
      break;
    case F_ITRF97:
      Log->write(SBLog::WRN, SBLog::INTERFACE, ClassName() + 
		 ": Sorry, import of SINEX format not supported yet");
      break;
    case F_ITRF2000:
      importITRF();
      break;
    case F_SINEX:
      Log->write(SBLog::WRN, SBLog::INTERFACE, ClassName() + 
		 ": Sorry, import of SINEX format not supported yet");
      break;
    default:
      Log->write(SBLog::WRN, SBLog::INTERFACE, ClassName() + ": Unknown type format for ITRF importing");
      break;
    };
  
  delete this;
};

void SBStationImport::importITRF()
{
  int		i;
  SB_TRF	*W = new SB_TRF;
  SB_TRF	*OldTRF = new SB_TRF;
  SBSite	*NewS;
  SBSite	*OldS;
  QFileInfo	finfo;
  bool		isNeedSave = FALSE;
  bool		isRead = FALSE;
  QString	sFiles = "";
  QString	Str;
  SBSolutionName	Label;

  static const QString sFilter[]= {"FILT_COO", "FILT_TECH", "FILT_VLBI", "FILT_ALL"};
  static const QString sMode[]= {"REPLACE", "APP_ONLY", "APP_UPD", "UPD_ONLY"};

  // reading site, dir.sta, ssc files
  if (FFormat == F_ITRF94)
    {
      for (i=0; i<NumOf1994Files; i++)
	{
	  if (cbFileName1994[i]->isChecked())
	    {
	      sFileName1994[i] = leFileName1994[i]->text();
	      finfo.setFile(sFileName1994[i]);
	      if (finfo.exists() && finfo.isReadable() && !finfo.isDir())
		{
		  loadFile1994(W, sFileName1994[i], (FileType)i);
		  sFiles += " " + finfo.baseName() + "." + finfo.extension();
		  isRead = TRUE; // hope, we've got something
		}
	      else 
		Log->write(SBLog::WRN, SBLog::INTERFACE, 
			   ClassName() + ": could not access `" + sFileName1994[i] + "'; ignored");
	    };
	};
      Label = W->label();
      Label.setAcronym("ITRF");
      Label.setYear(1994);
      Label.setTech(TECH_COMBINED);
      Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": Import type  : ITRF-1994");
    }
  else if (FFormat == F_ITRF2000)
    {
      for (i=0; i<NumOf2000Files; i++)
	{
	  if (cbFileName2000[i]->isChecked())
	    {
	      sFileName2000[i] = leFileName2000[i]->text();
	      finfo.setFile(sFileName2000[i]);
	      if (finfo.exists() && finfo.isReadable() && !finfo.isDir())
		{
		  loadFile2000(W, sFileName2000[i], (FileType)i);
		  sFiles += " " + finfo.baseName() + "." + finfo.extension();
		  isRead = TRUE; // hope, we've got something
		}
	      else 
		Log->write(SBLog::WRN, SBLog::INTERFACE, 
			   ClassName() + ": could not access `" + sFileName2000[i] + "'; ignored");
	    };
	};
      
      Label = W->label();
      Label.setAcronym("ITRF");
      Label.setYear(2000);
      Label.setTech(TECH_COMBINED);
      Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": Import type  : ITRF-2000");
    }
  else 
    return;

  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": Import filter: " + sFilter[Filter]);
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": Import mode  : " + sMode[Mode]);

  if (isRead) 
    {
      applyFilter(Filter, W);

      // define sites' velocity
      SBSiteIterator it(*W);
      for (NewS=it.toFirst(); it.current(); NewS=++it)
	{
	  double  d = 20.0;
	  Vector3 V = v3Zero;
	  Vector3 dV= v3Zero;
	  SBStationIterator is(NewS->stations());
	  for (SBStation *Q=is.toFirst(); is.current(); Q=++is)
	    if (Q->v()!=v3Zero && d>Q->v_err().module()) 
	      {
		V = Q->v(); 
		dV= Q->v_err(); 
		d = Q->v_err().module();
	      };
	  if (d<20.0) 
	    {
	      NewS->setV(V);
	      NewS->setV_err(dV);
	    };
	};
    };

  SetUp->loadTRF(OldTRF);

  switch (Mode)
    {
    case REPLACE:
      isNeedSave = TRUE;
      break;
    case APP_ONLY:
      // sites:
      for (NewS=W->first(); NewS; NewS=W->next())
	if (!OldTRF->find(*NewS)) 
	  {
	    OldTRF->inSort(new SBSite(*NewS));
	    isNeedSave = TRUE;
	  };
      // aliases:
      {
	QDictIterator<QString> it_ao(*W->aliasDict());
	while (it_ao.current())
	  {
	    if (!OldTRF->aliasDict()->find(it_ao.currentKey()))
	      {
		OldTRF->aliasDict()->insert(it_ao.currentKey(), new QString(it_ao.current()->data()));
		isNeedSave = TRUE;
	      };
	    ++it_ao;
	  };
      }
      delete W;
      W = OldTRF;
      OldTRF = NULL;
      break;
    case APP_UPD:
      // sites:
      if (W->count())
	isNeedSave = TRUE;
      for (NewS=W->first(); NewS; NewS=W->next())
	if (!(OldS=OldTRF->find(*NewS))) 
	  OldTRF->inSort(new SBSite(*NewS));
	else
	  OldS->updateSite(*NewS);
      // aliases:
      if (W->aliasDict()->count())
	isNeedSave = TRUE;
      {
	QDictIterator<QString> it_au(*W->aliasDict());
	while (it_au.current())
	  {
	    if (!OldTRF->aliasDict()->find(it_au.currentKey()))
	      OldTRF->aliasDict()->insert (it_au.currentKey(), new QString(it_au.current()->data()));
	    else
	      OldTRF->aliasDict()->replace(it_au.currentKey(), new QString(it_au.current()->data()));
	    ++it_au;
	  };
      };
      delete W;
      W = OldTRF;
      OldTRF = NULL;
      break;
    case UPD_ONLY:
      // sites:
      for (OldS=OldTRF->first(); OldS; OldS=OldTRF->next())
	if ((NewS=W->find(*OldS)))
	  {
	    OldS->updateSite(*NewS);
	    //	    *OldS = *NewS;
	    isNeedSave = TRUE;
	  };
      // aliases:
      {
	QDictIterator<QString> it_uo(*OldTRF->aliasDict());
	while (it_uo.current())
	  {
	    QString *StrPtr;
	    if ((StrPtr=W->aliasDict()->find(it_uo.currentKey())))
	      {
		OldTRF->aliasDict()->replace(it_uo.currentKey(), new QString(StrPtr->data()));
		isNeedSave = TRUE;
	      };
	    ++it_uo;
	  };
      };
      delete W;
      W = OldTRF;
      OldTRF = NULL;
      break;
    };
  
  
  // now, load aux files:
  // here `W' is now new TRF after applying a filter and merged with the old one (if necessary)
  W->prepareDicts();
  // we need the known aliases before importing ECC-data
  // read 'IVS Network Station codes' file:
  if (cbFileNameAux[0]->isChecked())
    {
      sFileNameAux[0] = leFileNameAux[0]->text();
      finfo.setFile(sFileNameAux[0]);
      if (finfo.exists() && finfo.isReadable() && !finfo.isDir())
	{
	  if (loadNScodes(W, sFileNameAux[0]))
	    {
	      sFiles += " " + finfo.baseName() + "." + finfo.extension();
	      isNeedSave = TRUE;
	    };
	}
      else 
	Log->write(SBLog::WRN, SBLog::INTERFACE, 
		   ClassName() + ": could not access `" + sFileNameAux[0] + "'; ignored");
    };

  // read Eccentricities data file:
  if (cbFileNameAux[1]->isChecked())
    {
      sFileNameAux[1] = leFileNameAux[1]->text();
      finfo.setFile(sFileNameAux[1]);
      if (finfo.exists() && finfo.isReadable() && !finfo.isDir())
	{
	  if (loadECCdat(W, sFileNameAux[1]))
	    {
	      sFiles += " " + finfo.baseName() + "." + finfo.extension();
	      isNeedSave = TRUE;
	    };
	}
      else 
	Log->write(SBLog::WRN, SBLog::INTERFACE, 
		   ClassName() + ": could not access `" + sFileNameAux[1] + "'; ignored");
    };
  
  // read blq file:
  if (cbFileNameAux[2]->isChecked())
    {
      sFileNameAux[2] = leFileNameAux[2]->text();
      finfo.setFile(sFileNameAux[2]);
      if (finfo.exists() && finfo.isReadable() && !finfo.isDir())
	{
	  if (loadOLoad(W, sFileNameAux[2]))
	    {
	      sFiles += " " + finfo.baseName() + "." + finfo.extension();
	      isNeedSave = TRUE;
	    };
	}
      else 
	Log->write(SBLog::WRN, SBLog::INTERFACE, 
		   ClassName() + ": could not access `" + sFileNameAux[2] + "'; ignored");
    };

  
  if (isNeedSave)
    {      
      // add aliases:

      W->setComment(W->comment() + "; Imported from: " + sFiles);
      W->setLabel(Label);
      SetUp->saveTRF(W);
      emit stationsImported();
    };

  if (W) delete W;
  if (OldTRF) delete OldTRF;
};

void SBStationImport::loadFile1994(SB_TRF* W, const QString& FileName, FileType Type)
{
  QTextStream	*s;
  QFile	f(FileName);
  if (f.open(IO_ReadOnly))
    {
      switch (Type)
	{
	case SSC_A:
	case SSC_B:
	case SSC_C:
	case SSC_Z:
	  s = new SBTS_ssc1994(SBStation::Class_Z);
	  s->setDevice(&f);
	  *(SBTS_ssc1994*)s >> *W;
	  break;
	case DIR:
	  s = new SBTS_dir1994;
	  s->setDevice(&f);
	  *(SBTS_dir1994*)s >> *W;
	  break;
	case SITE:
	  s = new SBTS_site1994;
	  s->setDevice(&f);
	  *(SBTS_site1994*)s >> *W;
	  break;
	case BLQ: //special case
	  f.close();
	  return;
	  break;
	default:
	  f.close();
	  Log->write(SBLog::ERR, SBLog::IO, ClassName() +  ": loadFile: wrong Type, *&%&*^");
	  return;
	};
      f.close();
      s->unsetDevice();
      delete s;
      Log->write(SBLog::DBG, SBLog::IO, ClassName() + ": file " + FileName + " has been loaded");
    }
  else    
    Log->write(SBLog::WRN, SBLog::IO, ClassName() + ": could not read " + FileName + "; ignored");
};

void SBStationImport::loadFile2000(SB_TRF* W, const QString& FileName, FileType Type)
{
  QTextStream	*s;
  QFile	f(FileName);
  if (f.open(IO_ReadOnly))
    {
      switch (Type)
	{
	case SSC_A:
	case SSC_B:
	case SSC_C:
	case SSC_Z:
	  s = new SBTS_ssc2000;
	  s->setDevice(&f);
	  *(SBTS_ssc2000*)s >> *W;
	  break;
	case DIR:
	  s = new SBTS_dir2000;
	  s->setDevice(&f);
	  *(SBTS_dir2000*)s >> *W;
	  break;
	case SITE:
	  s = new SBTS_site2000;
	  s->setDevice(&f);
	  *(SBTS_site2000*)s >> *W;
	  break;
	case BLQ: //special case
	  f.close();
	  return;
	  break;
	default:
	  f.close();
	  Log->write(SBLog::ERR, SBLog::IO, ClassName() +  ": loadFile: wrong Type, *&%&*^");
	  return;
	};
      f.close();
      s->unsetDevice();
      delete s;
      Log->write(SBLog::DBG, SBLog::IO, ClassName() + ": file " + FileName + " has been loaded");
    }
  else    
    Log->write(SBLog::WRN, SBLog::IO, ClassName() + ": could not read " + FileName + "; ignored");
};

void SBStationImport::applyFilter(ImportFilter F, SB_TRF* W)
{
  SBStation*	Station;
  SBSite*	Site;

  SBSiteIterator it(*W);
  while ((Site=it.current()))
    {
      ++it;
      SBStationIterator is(Site->stations());
      while ((Station=is.current()))
	{
	  ++is;
	  switch (F)
	    {
	    case FILT_COO:
	      if (Station->r_first()==v3Zero) Site->delStation(Station);
	      break;
	    case FILT_TECH:
	      if (Station->tech()==TECH_UNKN) Site->delStation(Station);
	      break;
	    case FILT_VLBI:
	      if (Station->tech()!=TECH_VLBI) Site->delStation(Station);
	      break;
	    case FILT_ALL:
	    default:
	      break;
	    };
	};
      if (!Site->stations().count()) W->remove(Site);
    };
};

bool SBStationImport::loadOLoad(SB_TRF* W, QString FileName)
{
  bool		isModified = FALSE;
  int		Num = 0;
  double	Dist= 2048.0E3;

  QString	Str;
  SBOLoadList	OLL;
  OLL.setAutoDelete(TRUE);

  QFile	f(FileName);
  if (f.open(IO_ReadOnly))
    {
      SBTS_blq  s;
      s.setDevice(&f);
      s >> OLL;
      f.close();
      s.unsetDevice();
      Log->write(SBLog::DBG, SBLog::IO, ClassName() + ": file " + FileName + " has been loaded");
    }
  else
    {
      Log->write(SBLog::WRN, SBLog::IO, ClassName() + ": could not read " + FileName + "; ignored");
      return FALSE;
    };
  
  if (!(Num=OLL.count())) return FALSE;
  if (!W->count()) return FALSE;
  
  QFileInfo finfo(f);
  FileName = finfo.baseName() + "." + finfo.extension();
  SBStation		*Station;
  SBSite		*Site;
  SBOLoadCarrier       	*OLC;

  // check for DOMES Numbers:
  SBNIterator it_olc(OLL);
  while ((OLC=(SBOLoadCarrier*)it_olc.current()))
    {
      ++it_olc;
      if (OLC->id().isValidId())
	{
	  if ((Station=(*W)[OLC->id()]))
	    {
	      if ((Mode==REPLACE) ||
		  (Mode==APP_ONLY && !Station->oLoad()) ||
		  (Mode==APP_UPD) ||
		  (Mode==UPD_ONLY && Station->oLoad()))
		{
		  Station->createOLoad(OLC->oLoad(), SBOLoad::P_DomesNum);
		  Station->oLoad()->
		    setDist((Dist=Station->r_first()!=v3Zero?
			     OLC->calcDistance(Station->longitude(), Station->latitude()):4096.0));
		  Station->oLoad()->setInfo("Loaded from the file \"" + FileName 
					    + "\"; pickuped by station's DOMES Number, (d: " + 
					    Str.setNum(Dist/1000.0, 'f',3) + "km)");
		  isModified = TRUE;
		  Log->write(SBLog::DBG, SBLog::IO, ClassName() + 
			     ": station " + OLC->name() + " successfuly pickuped; DOMES Number: [" + 
			     OLC->id().toString() + "]");
		}
	      else
		Log->write(SBLog::DBG, SBLog::IO, ClassName() + ": station " + OLC->name() + 
			   " with DOMES Number ["  + OLC->id().toString() + 
			   "] ignored because of import mode");
	      OLL.remove(OLC);
	    }
	  else
	    Log->write(SBLog::DBG, SBLog::IO, ClassName() + ": station " + OLC->name() + 
		       " not found in database; DOMES Number is ["  + OLC->id().toString() + "]");
	};
    };

  if (isModified)
    Log->write(SBLog::DBG, SBLog::IO, ClassName() + 
	       Str.sprintf(": Picked up by DOMES Number: %d stations inserted; %d entries left", 
			   Num-OLL.count(), OLL.count()));

  QIntDict<SBStation> *StationsByCDP   = new QIntDict<SBStation>(Num);
  QDict   <SBStation> *StationsByName  = new QDict   <SBStation>(Num);
  QDict   <SBStation> *StationsByCharID= new QDict   <SBStation>(Num);
  StationsByCDP   -> setAutoDelete(FALSE);
  StationsByName  -> setAutoDelete(FALSE);
  StationsByCharID-> setAutoDelete(FALSE);

  SBSiteIterator it(*W);
  while ((Site=it.current()))
    {
      ++it;
      SBStationIterator is(Site->stations());
      while ((Station=is.current()))
	{
	  ++is;
	  StationsByName->insert((const char*)Station->name(), Station);
	  if (999<Station->CDP() && Station->CDP()<9999)
	    StationsByCDP->insert(Station->CDP(), Station);
	  if (Station->charID()!="    ")
	    StationsByCharID->insert(Station->charID(), Station);
	};
    };

  if ((Num=OLL.count()) && StationsByCDP->count())
    {
      it_olc.toFirst();
      while ((OLC=(SBOLoadCarrier*)it_olc.current()))
	{
	  ++it_olc;
	  if (999<OLC->CDP() && OLC->CDP()<9999)
	    {
	      if ((Station=StationsByCDP->find(OLC->CDP())))
		{
		  if ((Mode==REPLACE) ||
		      (Mode==APP_ONLY && !Station->oLoad()) ||
		      (Mode==APP_UPD) ||
		      (Mode==UPD_ONLY &&  Station->oLoad()))
		    {
		      Station->createOLoad(OLC->oLoad(), SBOLoad::P_CDPNum);
		      Station->oLoad()->
			setDist((Dist=Station->r_first()!=v3Zero?
				 OLC->calcDistance(Station->longitude(), Station->latitude()):
				 4096.0));
		      Station->oLoad()->setInfo("Loaded from the file \"" + FileName 
						+ "\"; pickuped by CDP Number, (d: " + 
						Str.setNum(Dist/1000.0, 'f',3) + "km)");
		      isModified = TRUE;
		      Log->write(SBLog::DBG, SBLog::IO, ClassName() + 
				 ": station " + OLC->name() + " successfuly pickuped");
		    };
		  OLL.remove(OLC);
		}
	      else
		Log->write(SBLog::DBG, SBLog::IO, ClassName() + ": station " + OLC->name() + 
			   " not found in database; CDP is ["  + Str.setNum(OLC->CDP()) + "]");
	    };
	};
    };
  delete StationsByCDP;
  
  if (isModified) 
    Log->write(SBLog::DBG, SBLog::IO, ClassName() + 
	       Str.sprintf(": Picked up by CDP Number: %d stations inserted; %d entries left", 
			   Num-OLL.count(), OLL.count()));
  if (!(Num=OLL.count()))
    {
      delete StationsByName;
      delete StationsByCharID;
      return isModified;
    };

  if ((Num=OLL.count()))  
    {
      it_olc.toFirst();
      int i;
      while ((OLC=(SBOLoadCarrier*)it_olc.current()))
	{
	  ++it_olc;
	  i = OLC->id().domeMajor();
	  if (9999<i && i<99999)
	    {
	      if ((Site=W->find(i)))
		{
		  if ((Mode==REPLACE) ||
		      (Mode==APP_ONLY && Site->oLoad().pickedUp()==SBOLoad::P_NA) ||
		      (Mode==APP_UPD) ||
		      (Mode==UPD_ONLY && Site->oLoad().pickedUp()!=SBOLoad::P_NA))
		    {
		      Site->setOLoad(OLC->oLoad());
		      Site->oLoad().setPickedUp(SBOLoad::P_Domes);
		      Site->calcRLF();
		      Site->oLoad().
			setDist((Dist=Site->r()!=v3Zero?
				 OLC->calcDistance(Site->longitude(), Site->latitude()):4096.0));
		      Site->oLoad().setInfo("Loaded from the file \"" + FileName 
					    + "\"; pickuped by site's DOMES Number, (d: " + 
					    Str.setNum(Dist/1000.0, 'f',3) + "km)");
		      isModified = TRUE;
		      Log->write(SBLog::DBG, SBLog::IO, ClassName() + 
				 ": site " + OLC->name() + " successfuly pickuped");
		    };
		  OLL.remove(OLC);
		}
	      else
		Log->write(SBLog::DBG, SBLog::IO, ClassName() + ": site " + OLC->name() + 
			   " not found in database; DOMES Number is ["  + Str.setNum(i) + "]");
	    };
	};
    };
  if (isModified) Log->write(SBLog::DBG, SBLog::IO, ClassName() + 
			     Str.sprintf(": Picked up by site's DOMES Number: %d stations inserted;"
					 " %d entries left", Num-OLL.count(), OLL.count()));
  
  if ((Num=OLL.count()) && StationsByName->count())
    {
      it_olc.toFirst();
      while ((OLC=(SBOLoadCarrier*)it_olc.current()))
	{
	  ++it_olc;
	  if ((Station=StationsByName->find((const char*)OLC->name())))
	    {
	      if ((Mode==REPLACE) ||
		  (Mode==APP_ONLY && !Station->oLoad()) ||
		  (Mode==APP_UPD) ||
		  (Mode==UPD_ONLY &&  Station->oLoad()))
		{
		  Station->createOLoad(OLC->oLoad(), SBOLoad::P_Name);
		  Station->oLoad()->
		    setDist((Dist=Station->r_first()!=v3Zero?
			     OLC->calcDistance(Station->longitude(), Station->latitude()):4096.0));
		  Station->oLoad()->setInfo("Loaded from the file \"" + FileName 
					    + "\"; pickuped by station name, (d: " + 
					    Str.setNum(Dist/1000.0, 'f',3) + "km)");
		  isModified = TRUE;
		  Log->write(SBLog::DBG, SBLog::IO, ClassName() + 
			     ": station " + OLC->name() + " successfuly pickuped");
		};
	      OLL.remove(OLC);
	    }
	  else
	    Log->write(SBLog::DBG, SBLog::IO, ClassName() + ": station " + OLC->name() + 
		       " not found in database; name is ["  + OLC->name() + "]");
	};
    };
  delete StationsByName;

  if (isModified) 
    Log->write(SBLog::DBG, SBLog::IO, ClassName() + 
	       Str.sprintf(": Picked up by station name: %d stations inserted; %d entries left", 
			   Num-OLL.count(), OLL.count()));

  if ((Num=OLL.count()) && StationsByCharID->count())
    {
      it_olc.toFirst();
      while ((OLC=(SBOLoadCarrier*)it_olc.current()))
	{
	  ++it_olc;
	  if ((Station=StationsByCharID->find((const char*)OLC->charID())))
	    {
	      if ((Mode==REPLACE) ||
		  (Mode==APP_ONLY && !Station->oLoad()) ||
		  (Mode==APP_UPD) ||
		  (Mode==UPD_ONLY &&  Station->oLoad()))
		{
		  Station->createOLoad(OLC->oLoad(), SBOLoad::P_Name);
		  Station->oLoad()->
		    setDist((Dist=Station->r_first()!=v3Zero?
			     OLC->calcDistance(Station->longitude(), Station->latitude()):4096.0));
		  Station->oLoad()->setInfo("Loaded from the file \"" + FileName 
					    + "\"; pickuped by station CharID, (d: " + 
					    Str.setNum(Dist/1000.0, 'f',3) + "km)");
		  isModified = TRUE;
		  Log->write(SBLog::DBG, SBLog::IO, ClassName() + 
			     ": station " + OLC->name() + " successfuly pickuped by char id");
		};
	      OLL.remove(OLC);
	    }
	  // well, it's a strange, but sometimes happend..
	  else if ((Station=StationsByCharID->find((const char*)OLC->name())) &&
		   Station->r_first()!=v3Zero && 
		   (Dist=OLC->calcDistance(Station->longitude(), Station->latitude()))<2048.0) 
	    //otherwice, it's just a coinsidence..
	    {
	      if ((Mode==REPLACE) ||
		  (Mode==APP_ONLY && !Station->oLoad()) ||
		  (Mode==APP_UPD) ||
		  (Mode==UPD_ONLY &&  Station->oLoad()))
		{
		  Station->createOLoad(OLC->oLoad(), SBOLoad::P_Name);
		  Station->oLoad()->setDist(Dist);
		  Station->oLoad()->setInfo("Loaded from the file \"" + FileName 
					    + "\"; pickuped by CharID means StationName, (d: " + 
					    Str.setNum(Dist/1000.0, 'f',3) + "km)");
		  isModified = TRUE;
		  Log->
		    write(SBLog::DBG, SBLog::IO, ClassName() + ": station " + 
			  Station->name() + " successfuly pickuped by char id [" + 
			  OLC->name() + "]");
		};
	      OLL.remove(OLC);
	    };
	};
    };
  delete StationsByCharID;

  if (isModified) 
    Log->write(SBLog::DBG, SBLog::IO, ClassName() + 
	       Str.sprintf(": Picked up by station char id: %d stations inserted; %d entries left",
			   Num-OLL.count(), OLL.count()));
  
  //check sites, propagate OLoads:
  it.toFirst();
  while ((Site=it.current()))
    {
      ++it;
      if (Site->oLoad().pickedUp()==SBOLoad::P_NA || Site->oLoad().pickedUp()==SBOLoad::P_Distance)
	{
	  SBStation *st_w=NULL;
	  SBStationIterator is(Site->stations());
	  while ((Station=is.current()))
	    {
	      ++is;
	      if (Station->isAttr(SBStation::hasOwnOLoad) && Station->oLoad())
		{
		  if (!st_w) st_w=Station;
		  else if (st_w->oLoad()->pickedUp()>Station->oLoad()->pickedUp()) st_w=Station;
		};
	    };
	  if (st_w && st_w->oLoad() && st_w->oLoad()->pickedUp()!=SBOLoad::P_NA) 
	    {
	      Site->setOLoad(*st_w->oLoad());
	      Site->oLoad().setInfo(Site->oLoad().info() + "; get from station \"" + st_w->name()
				    + "\", DOMES Number: [" + st_w->id().toString() + "]");
	      isModified = TRUE;
	    };
	};
    };
  

  //sites of last resort:
  //we have to load blq again...
  Log->write(SBLog::DBG, SBLog::IO, ClassName() + ": reloading the file"); 
  OLL.clear();
  if (!f.open(IO_ReadOnly)) return isModified;
  SBTS_blq  s;
  s.setDevice(&f);
  s >> OLL;
  f.close();
  s.unsetDevice();

  Log->write(SBLog::DBG, SBLog::IO, ClassName() + ": dealing with the sites of last resort");
  it.toFirst();
  while ((Site=it.current()))
    {
      ++it;
      if (Site->oLoad().pickedUp()==SBOLoad::P_NA || Site->oLoad().pickedUp()==SBOLoad::P_Distance)
	{
	  Site->calcRLF();
	  if (Site->r()!=v3Zero)
	    {
	      Dist = 1024000.0;
	      double MinDist = 
		Site->oLoad().pickedUp()==SBOLoad::P_Distance?Site->oLoad().dist():Dist;
	      
	      SBOLoadCarrier *ol_w = NULL;
	      it_olc.toFirst();
	      while ((OLC=(SBOLoadCarrier*)it_olc.current()))
		{
		  ++it_olc;
		  Dist = OLC->calcDistance(Site->longitude(), Site->latitude());
		  if (MinDist>Dist)
		    {
		      MinDist=Dist;
		      ol_w = OLC;
		    };
		};
	      if (ol_w)
		{
		  Site->setOLoad(ol_w->oLoad());
		  Site->oLoad().setPickedUp(SBOLoad::P_Distance);
		  Site->oLoad().setDist(MinDist);
		  Site->oLoad().setInfo("get from nearest station \"" + ol_w->name() + 
					"\", distance about : " + 
					Str.setNum(MinDist/1000.0, 'f',3) + 
					"km; loaded from the file \"" + FileName + "\"");
		  Log->write(SBLog::DBG, SBLog::IO, ClassName() + ": site " + Site->name() + 
			     " pickuped by distance: " + Str + "km, from the nearest station " 
			     + ol_w->name());
		  isModified = TRUE;
		};
	    };
	};
    };
  return isModified;
};

bool SBStationImport::loadNScodes(SB_TRF* W, const QString& FileName)
{
  bool		isModified = FALSE;
  bool		isOK;  

  QString	Str, a;

  QString	sCode;
  QString	sAlias; // "IVS Station name"
  QString	sComments;
  int		CDPNumber;
  SBStationID	ID;
  SBStation	*Station, *sta;

  QFile	f(FileName);
  if (f.open(IO_ReadOnly))
    {
      QTextStream  s;
      s.setDevice(&f);
      //
      // quick & dirty:
      while (!s.atEnd())
	{
	  Str = s.readLine();
	  if (Str.mid(0,1) != "*")
	    {
	      //          1         2         3         4         5         6       
	      //01234567890123456789012345678901234567890123456789012345678901234567
	      //*C- Name---- --DOMES-- CDP- Comments/description
	      //*cc nnnnnnnn ssssstmmm mmmm -----------------------------------
	      // 14 GOLDMARS 40405S001 1514 70-m DSS14 at Goldstone, CA, USA
	      // 45 DSS45    50103S010 1545 34-m HEF at Tidbinbilla, Australia
	      a = Str.mid(13,9);
	      if (SBStationID::isValidStr(a))
		{
		  ID.setID(a);
		  sCode  = Str.mid(1, 2);
		  sAlias = Str.mid(4, 8);
		  sComments = Str.mid(28, 100);
		  
		  CDPNumber = Str.mid(23, 4).toInt(&isOK);
		  if (!isOK || CDPNumber<999)
		    CDPNumber=0;
		  
		  if ( (Station=W->find(ID)) )
		    {
		      if ( !(sta=W->lookupStation(sAlias)) )
			{
			  W->aliasDict()->insert((const char*)sAlias, new QString(Station->name() + " " +
										  Station->id().toString()));
			  if (CDPNumber)
			    Station->setCDP(CDPNumber);
			  Station->setCharID(sCode);
			  if (Station->comments() == "no comment")
			    Station->setComments("Old description: " + Station->description());
			  else
			    Station->setComments(Station->comments() + ";\nOld description: " + 
						 Station->description());
			  Station->setDescription(sComments);

			  isModified = TRUE;
			}
		      else if (sta->id() != Station->id())
			Log->write(SBLog::INF, SBLog::IO, ClassName() + ": alias: [" + sAlias +
				   "] already registered for the station `" + sta->name() +
				   "', ID: [" + sta->id().toString() + "]; entry ignored (fix by hand)");
		    }
		  else
		    {
		      Log->write(SBLog::DBG, SBLog::IO, ClassName() + ": not found ID: [" + ID.toString() +
				 "] in the database");

		      // lookup a site:
		      SBSite *Site;
		      if ( !(Site = W->find(ID.domeMajor())) )
			{
			  // create a new site:
			  Site = new SBSite(QString(sAlias + " Region").simplifyWhiteSpace());
			  Site-> setDomeMajor(ID.domeMajor());
			  W->inSort(Site);
			  Log->write(SBLog::INF, SBLog::DATA, ClassName() + ": created the site `" + 
				     Site->name() + "' according to the file " + FileName);
			};
		      
		      Station = new SBStation(Site, sAlias.simplifyWhiteSpace());
		      Station->setPointType(ID.ptType());
		      Station->setDomeMinor(ID.domeMinor());
		      Station->setTech(TECH_VLBI);
		      if (CDPNumber)
			Station->setCDP(CDPNumber);
		      Station->setCharID(sCode);
		      Station->setDescription(sComments);
		      Station->setComments("The station was not found in the TRF catalogue,\n"
					   "data was imported from the file of `IVS Station names', " +
					   FileName);
		      
		      Site->addStation(Station);
		      Log->write(SBLog::INF, SBLog::DATA, ClassName() + ": added the station `" + 
				 Station->name() + "' [" + Station->id().toString() + "] to the site `" +
				 Site->name() + "' according to the file " + FileName);
		      
		      if (!W->aliasDict()->find(sAlias))
			W->aliasDict()->insert((const char*)sAlias, new QString(Station->name() + " " +
										Station->id().toString()));
		      isModified = TRUE;
		    };
		}
	      else if (a != "---------")
		Log->write(SBLog::DBG, SBLog::IO, ClassName() + ": not a valid ID: [" + a + "]");
	    };
	};
      f.close();
      s.unsetDevice();
      Log->write(SBLog::DBG, SBLog::IO, ClassName() + ": file " + FileName + " has been loaded");
    }
  else
    {
      Log->write(SBLog::WRN, SBLog::IO, ClassName() + ": could not read " + FileName + "; ignored");
      return FALSE;
    };
  
  return isModified;
};

bool SBStationImport::loadECCdat(SB_TRF* W, const QString& FileName)
{
  bool		isModified = FALSE;

  if (W->ecc().importEccDat(FileName, W))
    {
      isModified = TRUE;
      Log->write(SBLog::DBG, SBLog::IO, ClassName() + ": file " + FileName + " has been loaded");
    }
  else
    {
      Log->write(SBLog::WRN, SBLog::IO, ClassName() + ": could not read " + FileName + "; ignored");
      return FALSE;
    };
  
  return isModified;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBSiteEditor..										*/
/*												*/
/*==============================================================================================*/
SBSiteEditor::SBSiteEditor(SBSiteListItem* SiteItem_, QWidget* parent, 
			   const char* name, bool modal, WFlags f)
  : SBTabDialog(parent, name, modal, f)
{
  SiteItem	= SiteItem_;
  SitesLV	= SiteItem->listView();
  Site		= SiteItem->Site;
  Wtrf		= NULL;
  
  isNew		= FALSE;
  init();
};

SBSiteEditor::SBSiteEditor(QListView* SitesLV_, SB_TRF* Wtrf_, QWidget* parent, 
			   const char* name, bool modal, WFlags f)
  : SBTabDialog(parent, name, modal, f)
{
  SiteItem	= NULL;
  SitesLV	= SitesLV_;
  Site		= new SBSite;
  Site		-> addStation(new SBStation(Site));
  Wtrf		= Wtrf_;

  isNew		= TRUE;
  init();
};

SBSiteEditor::~SBSiteEditor()
{
  emit siteModified(isModified);
  if (isNew && !isModified && Site) delete Site;
};

void SBSiteEditor::init()
{
  isModified		= FALSE;
  numOpenedWindows	= 0;
  setCaption("Site editor");

  addTab(wSite(), "Site info");
  addTab(wOLoad(), "Ocean Loading");

  setCancelButton();
  setApplyButton();
  connect(this, SIGNAL(applyButtonPressed()), SLOT(makeApply()));
};  

QWidget* SBSiteEditor::wSite()
{
  QWidget*	W = new QWidget(this);

  static const QString lab[3]={"Vx:","Vy:","Vz:"};
    
  QGroupBox	*gbox;
  QBoxLayout	*Layout, *SubLayout, *aLayout;
  QLabel	*label;
  
  Layout = new QHBoxLayout(W, 20, 10);
  
  SubLayout = new QVBoxLayout();
  Layout -> addLayout(SubLayout, 10);
  //  SubLayout -> addStretch();

  
  //info's fields:
  gbox = new QGroupBox("Site credentials", W);
  QGridLayout  *grid = new QGridLayout(gbox, 4,2, 20,5);
  
  label	= new QLabel("DOMES:", gbox);
  label	-> setMinimumSize(label->sizeHint());
  grid -> addWidget(label, 0,0);

  str = str.sprintf("%05d",Site->domeMajor());
  EDome = new QLineEdit(gbox);
  EDome -> setText(str);
  EDome -> setMinimumSize(EDome->sizeHint());
  EDome -> setEnabled(isNew);
  grid -> addWidget(EDome, 0,1);

  label	= new QLabel("Site name:", gbox);
  label	-> setMinimumSize(label->sizeHint());
  grid -> addWidget(label, 1,0);

  EName = new QLineEdit(gbox);
  EName -> setText(Site->name());
  EName -> setMinimumSize(EName->sizeHint());
  grid -> addWidget(EName, 1,1);


  label	= new QLabel("Country:", gbox);
  label	-> setMinimumSize(label->sizeHint());
  grid -> addWidget(label, 2,0);

  ECountry = new QLineEdit(gbox);
  ECountry -> setText(Site->country());
  ECountry -> setMinimumSize(ECountry->sizeHint());
  grid -> addWidget(ECountry, 2,1);


  label	= new QLabel("Tectonic plate:", gbox);
  label	-> setMinimumSize(label->sizeHint());
  grid -> addWidget(label, 3,0);

  EPlate = new QLineEdit(gbox);
  EPlate -> setText(Site->plate());
  EPlate -> setMinimumSize(EPlate->sizeHint());
  grid -> addWidget(EPlate, 3,1);
  grid -> activate();
  SubLayout -> addWidget(gbox);
  //--  

  SubLayout -> addStretch(1);

  gbox = new QGroupBox("Site velocities", W);
  grid = new QGridLayout(gbox, 4,3, 20,5);
  for (int i=0; i<3; i++)
    {
      label	= new QLabel(lab[i], gbox);
      label	-> setMinimumSize(label->sizeHint());
      grid -> addWidget(label, 1+i,0);
      
      str = str.sprintf("%12.4f", Site->v().at((DIRECTION)i));
      EV[i] = new QLineEdit(gbox);
      EV[i] -> setText(str);
      EV[i] -> setMinimumHeight(EV[i]->sizeHint().height());
      EV[i] -> setMinimumWidth (EV[i]->fontMetrics().width(str)+10);
      grid -> addWidget(EV[i], 1+i,1);

      str = str.sprintf("%5.4f", Site->v_err().at((DIRECTION)i));
      EdV[i] = new QLineEdit(gbox);
      EdV[i] -> setText(str);
      EdV[i] -> setMinimumHeight(EdV[i]->sizeHint().height());
      EdV[i] -> setMinimumWidth (EdV[i]->fontMetrics().width(str)+10);
      grid -> addWidget(EdV[i], 1+i,2);
    };
  label	= new QLabel("Velocity (m/y)", gbox);
  label	-> setMinimumSize(label->sizeHint());
  grid -> addWidget(label, 0,1);

  label	= new QLabel("uncert.", gbox);
  label	-> setMinimumSize(label->sizeHint());
  grid -> addWidget(label, 0,2);
  
  grid -> activate();
  SubLayout -> addWidget(gbox);
  //  SubLayout -> addStretch(1);

  
  //  set up stations list box:
  StationsLV = new QListView(W);
  StationsLV -> addColumn("DOMES");
  StationsLV -> setColumnAlignment( 0, AlignLeft);
  StationsLV -> addColumn("Station name");
  StationsLV -> setColumnAlignment( 1, AlignLeft);
  StationsLV -> addColumn("CDP");
  StationsLV -> setColumnAlignment( 2, AlignLeft);
  StationsLV -> addColumn("Char ID");
  StationsLV -> setColumnAlignment( 3, AlignLeft);
  StationsLV -> addColumn("Tech");
  StationsLV -> setColumnAlignment( 4, AlignLeft);
  StationsLV -> addColumn("N Pts");
  StationsLV -> setColumnAlignment( 5, AlignRight);
  StationsLV -> addColumn("OLoad");
  StationsLV -> setColumnAlignment( 6, AlignLeft);
  StationsLV -> addColumn("Mnt Type");
  StationsLV -> setColumnAlignment( 7, AlignLeft);
  StationsLV -> addColumn("Distance");
  StationsLV -> setColumnAlignment( 8, AlignRight);
  StationsLV -> addColumn("dVeloc");
  StationsLV -> setColumnAlignment( 9, AlignRight);
  
  SBStationIterator it(Site->stations());
  for (SBStation* W=it.toFirst(); it.current(); W=++it)
    (void) new SBStationListItem(StationsLV, W);
  StationsLV -> setAllColumnsShowFocus(TRUE);
  StationsLV -> setMinimumSize(StationsLV->sizeHint());
  StationsLV -> setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  if (StationsLV->firstChild())
    StationsLV -> setSelected(StationsLV->firstChild(), TRUE);
  StationsLV -> setFocusPolicy(QWidget::StrongFocus);

  SubLayout = new QVBoxLayout();
  Layout -> addLayout(SubLayout);
  SubLayout -> addWidget(StationsLV,5);

  aLayout = new QHBoxLayout();
  SubLayout -> addLayout(aLayout);

  //---
  aLayout->addStretch(1);
  QPushButton	*bAdd, *bDel;
  QSize		Size;
  
  aLayout-> addWidget(bAdd = new QPushButton("Add..", W));
  aLayout-> addWidget(bDel = new QPushButton("Delete", W));

  bAdd->setFocusPolicy(QWidget::ClickFocus);
  bDel->setFocusPolicy(QWidget::ClickFocus);
  bDel->setMinimumSize(Size=bDel->sizeHint());
  bAdd->setMinimumSize(Size);

  connect (bAdd, SIGNAL(clicked()), SLOT(insertEntry()));
  connect (bDel, SIGNAL(clicked()), SLOT(deleteEntry()));

  connect (StationsLV, SIGNAL(doubleClicked(QListViewItem*)), SLOT(editEntry(QListViewItem*)));
  connect (StationsLV, SIGNAL(returnPressed(QListViewItem*)), SLOT(editEntry(QListViewItem*)));

  Layout -> activate();
  return W;
};

QWidget* SBSiteEditor::wOLoad()
{
  QWidget*	W = new QWidget(this);
  QBoxLayout	*Layout;
  
  Layout = new QVBoxLayout(W, 10, 10);
  
  OLoadEditor = new SBOLoadEditor(&(Site->OLoad), TRUE, W);
  Layout -> addWidget(OLoadEditor, 10);
  Layout -> activate();
  return W;
};

void SBSiteEditor::makeApply()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": applying");
  acquireData();
  if (isModified) 
    {
      if (isNew) 
	{
	  SBSite *Q;
	  if ((Q=Wtrf->find(*Site)))
	    {
	      str = str.sprintf("%05d",Site->domeMajor());
	      QMessageBox::warning(this, "Site already defined",
				   "The site \"" + Site->name() + "\" already present "
				   "in database.\n"
				   "See " + str + " dome number for \"" + Q->name()
				   + "\" site.\n\n"
				   "It's impossible to add an object with the same ID,\n"
				   "but you can adit that entry (make a double click or\n"
				   "press Enter when a keyboard focus in a list)\n", 
				   "OK");
	      return;
	    };
	  
	  Wtrf->inSort(Site);
	  SiteItem = new SBSiteListItem(SitesLV, Site);
	  SitesLV->setSelected(SiteItem, TRUE);
	  // not works... ;-(
	  // SourcesLV->ensureItemVisible(SrcItem);
	  QListViewItem *item = SiteItem;
	  QListViewItem *firstItem = SitesLV->firstChild();
	  int h=0;
	  do 
	    {
	      h += item->totalHeight();
	      item=item->itemAbove();
	    } 
	  while (item && item!=firstItem);
	  SitesLV->ensureVisible(0, h + SiteItem->totalHeight()/2);
	  isNew = FALSE;
	  EDome->setEnabled(FALSE);
	}
      else SiteItem->repaint();
    };
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": done");
};

void SBSiteEditor::done(int r)
{
  if (numOpenedWindows) return;
  QDialog::done(r);
  if (r==QDialog::Accepted)	acquireData();
  delete this;
};

void SBSiteEditor::editEntry(QListViewItem* StationItem)
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": SBStuffStationEditStation selected");
  
  SBStationEditor *SEditor =  new SBStationEditor((SBStationListItem*)StationItem);
  connect (SEditor, SIGNAL(stationModified(bool)), SLOT(stationModified(bool)));
  numOpenedWindows++;
  SEditor->show();
};

void SBSiteEditor::insertEntry()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": SBStuffStationInsertStation selected");
  
  SBStationEditor *SEditor =  new SBStationEditor(StationsLV, Site);
  connect (SEditor, SIGNAL(stationModified(bool)), SLOT(stationModified(bool)));
  numOpenedWindows++;
  SEditor->show();
};

void SBSiteEditor::deleteEntry()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": SBStuffStationDeleteStation selected");

  SBStationListItem* StationItem = (SBStationListItem*)StationsLV->currentItem();
  if (StationItem)
    {
      QListViewItem* StationNextItem = StationItem->itemBelow();
      if (!StationNextItem) StationNextItem = StationItem->itemAbove();
      str = StationItem->Station->name().copy();
      if (!QMessageBox::warning(this, "Delete?", "Are you sure to delete station entry\n \""
				+ str + "\"?\n", "Yes, del them!", "No, let it live.."))
	{
	  Site->delStation(StationItem->Station);
	  
	  delete StationItem;
	  if (StationNextItem) StationsLV->setSelected(StationNextItem, TRUE);
	  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName()+": "+str+", rest in peace..");
	  isModified = TRUE;
	}
      else
	Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": relax, " + str + 
		   ", it's just a joke.. ");
      
    };
};
  
void SBSiteEditor::stationModified(bool Modified)
{
  if (!numOpenedWindows) 
    std::cerr << "your Editor's windows are diyng often than have been born\n";
  numOpenedWindows--;
  if (Modified) 
    {
      isModified = TRUE;
    };
};
  
void SBSiteEditor::acquireData()
{
  bool		isOK;
  int		d;
  Vector3	v;

  if (isNew)
    {
      str = EDome->text();
      d = str.toInt(&isOK);
      if (isOK && d!=Site->domeMajor())
	{
	  isModified = TRUE;
	  Site->setDomeMajor(d);
	};
    };
  
  if (Site->name() != EName->text())
    {
      Site->setName(EName->text());
      isModified = TRUE;
    };
  if (Site->country() != ECountry->text())
    {
      Site->setCountry(ECountry->text());
      isModified = TRUE;
    };
  if (Site->plate() != EPlate->text())
    {
      Site->setPlate(EPlate->text());
      isModified = TRUE;
    };

  str = EV[0]->text();
  v[X_AXIS]= str.toFloat(&isOK);
  if (isOK)
    {
      str = EV[1]->text();
      v[Y_AXIS]= str.toFloat(&isOK);
      if (isOK)
	{	  
	  str = EV[2]->text();
	  v[Z_AXIS]= str.toFloat(&isOK);
	  if (isOK && (v != Site->v()))
	    {
	      isModified = TRUE;
	      Site->setV(v); 
	    };
	};
    };

  str = EdV[0]->text();
  v[X_AXIS]= str.toFloat(&isOK);
  if (isOK)
    {
      str = EdV[1]->text();
      v[Y_AXIS]= str.toFloat(&isOK);
      if (isOK)
	{	  
	  str = EdV[2]->text();
	  v[Z_AXIS]= str.toFloat(&isOK);
	  if (isOK && (v != Site->v_err()))
	    {
	      isModified = TRUE;
	      Site->setV_err(v); 
	    };
	};
    };

  if (OLoadEditor->collectData()) isModified = TRUE;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBStationEditor..										*/
/*												*/
/*==============================================================================================*/
SBStationEditor::SBStationEditor(SBStationListItem* StationItem_, QWidget* parent,
			       const char* name, bool modal, WFlags f)
  : SBTabDialog(parent, name, modal, f)
{
  StationItem	= StationItem_;
  StationsLV	= StationItem->listView();
  Station	= StationItem->Station;
  Site		= Station->site();
  
  isNew		= FALSE;
  init();
};

SBStationEditor::SBStationEditor(QListView* StationsLV_, SBSite* Site_, QWidget* parent,
			       const char* name, bool modal, WFlags f)
  : SBTabDialog(parent, name, modal, f)
{
  StationItem	= NULL;
  StationsLV	= StationsLV_;
  Site		= Site_;
  Station	= new SBStation(Site);

  isNew		= TRUE;
  init();
};

void SBStationEditor::init()
{
  isModified	= FALSE;
  setCaption("Station Editor");

  addTab(wNames(), "Name");
  addTab(wTechs(), "Tech");
  addTab(wCoordinates(), "Coordinates");
  addTab(wStats(), "Status");
  addTab(wOLoad(), "Station's Ocean Loading Parameters");

  setCancelButton();
  setApplyButton();
  connect(this, SIGNAL(applyButtonPressed()), SLOT(makeApply()));
};

SBStationEditor::~SBStationEditor()
{
  emit stationModified(isModified);
  if (isNew && !isModified && Station) delete Station;
};
 
void SBStationEditor::makeApply()
{
  int		d;
  int		m;
  double	s;
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": applying");
  acquireData();
  if (isModified) 
    {
      Station->addAttr(SBStation::OrigEdited);
      Station->updateGeodCoo(TZero);
      rad2dmsl(Station->longitude(), d,m,s);
      Str.sprintf("%4d %02d %08.5f", d,m,s);
      LLongitude -> setText(Str);
      LLongitude -> setMinimumSize(LLongitude->sizeHint());
      
      rad2dms(Station->latitude(), d,m,s);
      Str.sprintf("%3d %02d %08.5f", d,abs(m),fabs(s));
      LLatitude -> setText(Str);
      LLatitude -> setMinimumSize(LLatitude->sizeHint());
      
      Str.sprintf("%.4f", Station->height());
      LHeight -> setText(Str);
      LHeight -> setMinimumSize(LHeight->sizeHint());
      
      if (isNew) 
	{
	  SBStation* Q;
	  if ((Q=Site->find(Station->id())))
	    {
	      QMessageBox::warning(this, "Station already defined", 
				   "The station with DOMES number \"" + 
				   Station->id().toString() + "\"\n(name " + 
				   Q->name() + ") already present "
				   "in database.\n\n"
				   "It's impossible to add an object with the same ID,\n"
				   "but you can adit that entry (make a double click or\n"
				   "press Enter when a keyboard focus in a list)\n", 
				   "OK");
	      return;
	    };
	  	  
	  Site->addStation(Station);
	  StationItem = new SBStationListItem(StationsLV, Station);
	  StationsLV->setSelected(StationItem, TRUE);
	  // not works... ;-(
	  // SourcesLV->ensureItemVisible(SrcItem);
	  QListViewItem *item = StationItem;
	  QListViewItem *firstItem = StationsLV->firstChild();
	  int h=0;
	  do 
	    {
	      h += item->totalHeight();
	      item=item->itemAbove();
	    } 
	  while (item && item!=firstItem);
	  StationsLV->ensureVisible(0, h + StationItem->totalHeight()/2);
	  isNew = FALSE;
	  EDome->setEnabled(FALSE);
	}
      else StationItem->repaint();
    };
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": done");
};

QWidget* SBStationEditor::wNames()
{
  QWidget*	W = new QWidget(this);

  QBoxLayout*	Layout;
  QBoxLayout*	bLayout;
  QGridLayout*	grid;
  QLabel*	label;
  QGroupBox*	gbox;

  static const QString sPtType[] = 
  {
    "\"S\", Antenna axis intersection",
    "\"M\", Fixed marker"
  };


  if (!isNew) Str = " \"" + Station->name() + "\" [" + Station->id().toString() + "] credentials";
  else Str = "New Station credentials";

  NameGBox = gbox = new QGroupBox((const char*)Str, W);
  Layout = new QVBoxLayout(gbox, 20, 10);

  // Names:
  grid    = new QGridLayout(2, 4, 5);
  Layout -> addLayout(grid);

  label    = new QLabel("Name:", gbox);
  label	  -> setMinimumSize(label->sizeHint());
  grid	  -> addWidget(label, 0,0);

  EName	   = new QLineEdit(gbox);
  EName	  -> setText(Station->name());
  EName   -> setMinimumWidth (EName->fontMetrics().width(Station->name()) + 2);
  EName   -> setMinimumHeight(EName->sizeHint().height());
  grid    -> addWidget(EName, 0,1);

  label   = new QLabel("DOMES:", gbox);
  label	 -> setMinimumSize(label->sizeHint());
  grid	 -> addWidget(label, 1,0);

  EDome	   = new QLineEdit(gbox);
  EDome	  -> setText(Str.sprintf("%03d", Station->domeMinor()));
  EDome   -> setMinimumSize(EDome->sizeHint());
  EDome	  -> setEnabled(isNew);
  grid    -> addWidget(EDome, 1,1);

  label    = new QLabel("CDP number:", gbox);
  label	  -> setMinimumSize(label->sizeHint());
  grid	  -> addWidget(label, 0,2);

  ECDP	   = new QLineEdit(gbox);
  ECDP	  -> setText(Str.sprintf("%4d",Station->CDP()));
  ECDP    -> setMinimumSize(ECDP->sizeHint());
  grid    -> addWidget(ECDP, 0,3);
  
  label    = new QLabel("Char ID:", gbox);
  label	  -> setMinimumSize(label->sizeHint());
  grid	  -> addWidget(label, 1,2);

  ECharID  = new QLineEdit(gbox);
  ECharID -> setText(Station->charID());
  ECharID -> setMinimumSize(ECharID->sizeHint());
  grid    -> addWidget(ECharID, 1,3);

  grid	  -> setColStretch(1,1);
  grid	  -> setColStretch(3,1);
  

  QButtonGroup* BGPtType = new QButtonGroup("Point Type", gbox);
  bLayout = new QVBoxLayout(BGPtType, 20, 5);
  for (int i=0; i<2; i++)
    {
      RBPtType[i] = new QRadioButton(sPtType[i], BGPtType);
      RBPtType[i]-> setMinimumSize(RBPtType[i]->sizeHint());
      RBPtType[i]-> setEnabled(isNew);
      bLayout -> addWidget(RBPtType[i]);
    };
  RBPtType[Station->pointType()]->setChecked(TRUE);
  bLayout -> activate();  
  Layout -> addWidget(BGPtType);
  //--

  // comments & description:  
  grid    = new QGridLayout(4, 2, 5);
  Layout -> addLayout(grid, 5);

  label   = new QLabel("Description:", gbox);
  label	 -> setMinimumSize(label->sizeHint());
  grid	 -> addWidget(label, 0,0);

  EDescr   = new QLineEdit(gbox);
  EDescr  -> setText(Station->description());
  EDescr  -> setMinimumSize(EDescr->sizeHint());
  grid    -> addWidget(EDescr, 0,1);

  label   = new QLabel("Comments:", gbox);
  label	 -> setMinimumSize(label->sizeHint());
  grid	 -> addWidget(label, 1,0);

  MEComments = new QMultiLineEdit(gbox);
  MEComments-> setText(Station->comments());
  grid    -> addMultiCellWidget(MEComments, 1,3, 1,1);
  grid	  -> setColStretch(1,1);
  grid	  -> setRowStretch(2,1);
  grid	  -> setRowStretch(3,1);
  
  Layout-> activate();


  Layout  = new QVBoxLayout(W, 10);
  Layout  -> addWidget(gbox);
  Layout  -> activate();
  
  return W;
};

QWidget* SBStationEditor::wTechs()
{
  QWidget*	W = new QWidget(this);

  QBoxLayout*	Layout;
  QBoxLayout*	aLayout;  
  QGroupBox*	gbox;

  gbox = new QGroupBox("Station Technique", W);
  Layout = new QVBoxLayout(gbox, 20, 10);

  QButtonGroup* BGTech = new QButtonGroup("Technique", gbox);
  aLayout = new QVBoxLayout(BGTech, 20, 5);
  for (int i=0; i<11; i++)
    {
      RBTech[i] = new QRadioButton(SBSolutionName::techName((Technique)i), BGTech);
      RBTech[i]-> setMinimumSize(RBTech[i]->sizeHint());
      aLayout -> addWidget(RBTech[i]);
    };
  RBTech[Station->tech()]->setChecked(TRUE);
  aLayout -> activate();
  Layout->addStretch(1);
  Layout->addWidget(BGTech);
  Layout->addStretch(1);

  Layout -> activate();

  Layout  = new QVBoxLayout(W, 10);
  Layout  -> addWidget(gbox);
  Layout  -> activate();
  
  return W;
};

QWidget* SBStationEditor::wCoordList(QWidget* parent_)
{
  QSize		Size;
  QWidget*	W = new QWidget(parent_);
  QBoxLayout*	Layout = new QVBoxLayout(W, 0, 10);
  
  // set up list box:
  CoordsLV = new QListView(W);
  CoordsLV -> addColumn("Since");
  CoordsLV -> setColumnAlignment( 0, AlignLeft);
  CoordsLV -> addColumn("Rx (m)");
  CoordsLV -> setColumnAlignment( 1, AlignRight);
  CoordsLV -> addColumn("Ry (m)");
  CoordsLV -> setColumnAlignment( 2, AlignRight);
  CoordsLV -> addColumn("Rz (m)");
  CoordsLV -> setColumnAlignment( 3, AlignRight);
  CoordsLV -> addColumn("dRx (m)");
  CoordsLV -> setColumnAlignment( 4, AlignRight);
  CoordsLV -> addColumn("dRy (m)");
  CoordsLV -> setColumnAlignment( 5, AlignRight);
  CoordsLV -> addColumn("dRz (m)");
  CoordsLV -> setColumnAlignment( 6, AlignRight);
  CoordsLV -> addColumn("Vx (m/y)");
  CoordsLV -> setColumnAlignment( 7, AlignRight);
  CoordsLV -> addColumn("Vy (m/y)");
  CoordsLV -> setColumnAlignment( 8, AlignRight);
  CoordsLV -> addColumn("Vz (m/y)");
  CoordsLV -> setColumnAlignment( 9, AlignRight);
  CoordsLV -> addColumn("dVx (m/y)");
  CoordsLV -> setColumnAlignment(10, AlignRight);
  CoordsLV -> addColumn("dVy (m/y)");
  CoordsLV -> setColumnAlignment(11, AlignRight);
  CoordsLV -> addColumn("dVz (m/y)");
  CoordsLV -> setColumnAlignment(12, AlignRight);
  CoordsLV -> addColumn("V?");
  CoordsLV -> setColumnAlignment(13, AlignCenter);
  CoordsLV -> addColumn("Comments");
  CoordsLV -> setColumnAlignment(14, AlignLeft);

  for (SBCoordinates *Coo = Station->refPt().first(); Coo; Coo = Station->refPt().next())
    (void) new SBCoordsListItem(CoordsLV, Coo);

  CoordsLV -> setAllColumnsShowFocus(TRUE);
  CoordsLV -> setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  //  CoordsLV -> setMinimumSize(CoordsLV->sizeHint());
  CoordsLV -> setMinimumWidth (CoordsLV->sizeHint().width());
  CoordsLV -> setMinimumHeight(CoordsLV->header()->sizeHint().height());

  CoordsLV->setFocusPolicy(QWidget::StrongFocus);
  if (Station->refPt().count() && CoordsLV->firstChild())
    CoordsLV->setSelected(CoordsLV->firstChild(), TRUE);
  Layout->addWidget(CoordsLV);
  
  //---
  QBoxLayout	*SubLayout = new QHBoxLayout(10);
  Layout->addLayout(SubLayout);
  
  QPushButton	*AddButton, *DeleteButton;

  SubLayout->addStretch(1);
  SubLayout->addWidget(AddButton = new QPushButton("Add", W));
  SubLayout->addWidget(DeleteButton = new QPushButton("Delete", W));

  AddButton	-> setFocusPolicy(QWidget::ClickFocus);
  DeleteButton	-> setFocusPolicy(QWidget::ClickFocus);

  DeleteButton	-> setMinimumSize(Size=DeleteButton->sizeHint());
  AddButton	-> setMinimumSize(Size);

  Layout->activate();

  connect(AddButton,	SIGNAL(clicked()), SLOT(addCoordinates()));
  connect(DeleteButton, SIGNAL(clicked()), SLOT(delCoordinates()));
  connect(CoordsLV,	SIGNAL(doubleClicked(QListViewItem*)), SLOT(editCoordinates(QListViewItem*)));
  connect(CoordsLV,	SIGNAL(returnPressed(QListViewItem*)), SLOT(editCoordinates(QListViewItem*)));

  return W;
};

QWidget* SBStationEditor::wCoordinates()
{
  QWidget*	W = new QWidget(this);
  QBoxLayout*	Layout;
  //  QBoxLayout*	SubLayout;
  QBoxLayout*	aLayout;
  QBoxLayout*	bLayout;
  QGroupBox*	gbox;
  QGridLayout*	grid;
  QLabel*	label;
  int		d;
  int		m;
  double	s;
  
  /*
    static const QString sClassC[] = 
    {"Class A", "Class B", "Class C", "Class D", "Class E", "Class Z", "Unknown class"};
  */

  //  static const QString sLabR[] =  {"Rx, m:", "Ry, m:", "Rz, m:"};
  static const QString sLabV[] =  {"Vx, m/y:", "Vy, m/y:", "Vz, m/y:"};
  static const QString sMntType[] =
  {
    "Azimuth-elevation",
    "Equatorial",
    "X-Y, X axis North",
    "X-Y, X axis East",
    "Richmond (unique)",
    "Undefined"
  };
  
  QWidget *q;
  gbox	  = new QGroupBox("Coordinates", W);

  Layout  = new QVBoxLayout(gbox, 20,10);
  Layout->addWidget((q=wCoordList(gbox)), 1);

  grid    = new QGridLayout(2,2,   20);
  Layout -> addLayout(grid);

  // one:
  QGridLayout	*grid2 = new QGridLayout(3, 3, 5);
  grid->addLayout(grid2, 0,0);
  for (int i=0; i<3; i++)
    {
      label  = new QLabel(sLabV[i], gbox);
      label -> setMinimumSize(label->sizeHint());
      grid2  -> addWidget(label, i,0);

      EV[i]= new QLineEdit(gbox);
      EV[i]-> setText(Str.sprintf("%6.4f", Station->v().at((DIRECTION)i)));
      EV[i]-> setMinimumWidth (EV[i]->fontMetrics().width(Str) + 2);
      EV[i]-> setMinimumHeight(EV[i]->sizeHint().height());
      grid2   -> addWidget(EV[i], i,1);
      
      EdV[i]= new QLineEdit(gbox);
      EdV[i]-> setText(Str.sprintf("%6.4f", Station->v_err().at((DIRECTION)i)));
      EdV[i]-> setMinimumWidth (EdV[i]->fontMetrics().width(Str) + 2);
      EdV[i]-> setMinimumHeight(EdV[i]->sizeHint().height());
      grid2   -> addWidget(EdV[i], i,2);
    };
  setTabOrder(EV[0], EV[1]);
  setTabOrder(EV[1], EV[2]);
  setTabOrder(EV[2], EdV[0]);
  setTabOrder(EdV[0], EdV[1]);
  setTabOrder(EdV[1], EdV[2]);

  // two:
  // Info about the Coordinates:
  QButtonGroup* BGInfo = new QButtonGroup("Geodetic coordinates:", gbox);
  aLayout = new QVBoxLayout(BGInfo, 20, 5);
  grid2    = new QGridLayout(3, 2, 5);
  aLayout -> addStretch(3);
  aLayout -> addLayout(grid2);

  label  = new QLabel("Longitude:", BGInfo);
  label -> setMinimumSize(label->sizeHint());
  grid2  -> addWidget(label, 0,0);
  label  = new QLabel("Latitude:", BGInfo);
  label -> setMinimumSize(label->sizeHint());
  grid2   -> addWidget(label, 1,0);
  label  = new QLabel("Height (m):", BGInfo);
  label -> setMinimumSize(label->sizeHint());
  grid2   -> addWidget(label, 2,0);

  rad2dmsl(Station->longitude(), d, m, s);
  Str.sprintf("%4d %02d %08.5f", d,m,s);
  LLongitude  = new QLabel(Str, BGInfo);
  LLongitude -> setAlignment(AlignRight | AlignVCenter);
  LLongitude -> setMinimumSize(LLongitude->sizeHint());
  grid2   -> addWidget(LLongitude, 0,1);

  rad2dms(Station->latitude(), d, m, s);
  Str.sprintf("%3d %02d %08.5f", d,abs(m),fabs(s));
  LLatitude  = new QLabel(Str, BGInfo);
  LLatitude -> setMinimumSize(LLatitude->sizeHint());
  LLatitude -> setAlignment(AlignRight | AlignVCenter);
  grid2  -> addWidget(LLatitude, 1,1);

  Str.sprintf("%.4f", Station->height());
  LHeight  = new QLabel(Str, BGInfo);
  LHeight -> setMinimumSize(LHeight->sizeHint());
  LHeight -> setAlignment(AlignRight | AlignVCenter);
  grid2  -> addWidget(LHeight, 2,1);
  
  aLayout -> addStretch(10);
  aLayout -> activate();
  grid->addWidget(BGInfo, 1, 0);

  /*
    // three:
    // Class of the Coordinates:
    QButtonGroup* BGClass = new QButtonGroup("Coordinates Class", gbox);
    aLayout = new QVBoxLayout(BGClass, 20, 5);
    for (int i=0; i<7; i++)
    {
    RBClassCoord[i] = new QRadioButton(sClassC[i], BGClass);
    RBClassCoord[i]-> setMinimumSize(RBClassCoord[i]->sizeHint());
    aLayout -> addWidget(RBClassCoord[i]);
    };
    RBClassCoord[Station->classCoord()]->setChecked(TRUE);
    aLayout -> activate();
    grid->addWidget(BGClass, 1, 0);
  */
  
  // four:
  // Mount type:
  aLayout = new QVBoxLayout(10);
  grid->addMultiCellLayout(aLayout, 0,1, 1,1);
  QButtonGroup* BGMntType = new QButtonGroup("Mount Type", gbox);
  bLayout = new QVBoxLayout(BGMntType, 20, 3);
  for (int i=0; i<6; i++)
    {
      RBMountType[i] = new QRadioButton(sMntType[i], BGMntType);
      RBMountType[i]-> setMinimumSize(RBMountType[i]->sizeHint());
      bLayout -> addWidget(RBMountType[i]);
    };
  RBMountType[Station->mountType()]->setChecked(TRUE);
  bLayout -> activate();

  aLayout -> addWidget(BGMntType);

  bLayout = new QHBoxLayout();
  aLayout -> addLayout(bLayout);
  label  = new QLabel("Axis offset:", gbox);
  label -> setMinimumSize(label->sizeHint());
  bLayout  -> addWidget(label);
  
  EAxisOffset = new QLineEdit(gbox);
  EAxisOffset-> setText(Str.sprintf("%8.5f", Station->axisOffset()));
  EAxisOffset-> setMinimumWidth (EAxisOffset->fontMetrics().width(Str) + 2);
  EAxisOffset-> setMinimumHeight(EAxisOffset->sizeHint().height());
  bLayout  -> addWidget(EAxisOffset);

  //  Layout-> activate();

  //-
  Layout = new QVBoxLayout(W, 10);
  Layout-> addWidget(gbox);
  Layout-> activate();
  return W;
};

QWidget* SBStationEditor::wStats()
{
  static const QString cOrig[] =
  {
    "has been imported",
    "has been edited",
    "from data files",
    "has been estimated"
  };

  static const QString cAttr[] =
  {
    "Was Analysed",
    "Attribute #02",
    "Attribute #03",
    "Attribute #04"
  };

  QWidget*	W = new QWidget(this);
  
  QBoxLayout*	Layout;
  QBoxLayout*	aLayout;
  QButtonGroup* BGw;
  //  QGridLayout*	grid;
  //  QLabel*	label;
  QGroupBox*	gbox;

  gbox	  = new QGroupBox("Station status", W);

  Layout  = new QVBoxLayout(gbox, 20, 20);

  //--
  BGw = new QButtonGroup("Origin", gbox);
  aLayout = new QVBoxLayout(BGw, 20, 5);
  for (int i=0; i<4; i++)
    {
      CDOrigin[i] = new QCheckBox(cOrig[i], BGw);
      CDOrigin[i]-> setMinimumSize(CDOrigin[i]->sizeHint());
      aLayout -> addWidget(CDOrigin[i]);
    };
  CDOrigin[0]->setChecked(Station->isAttr(SBStation::OrigImported));
  CDOrigin[1]->setChecked(Station->isAttr(SBStation::OrigEdited));
  CDOrigin[2]->setChecked(Station->isAttr(SBStation::OrigObsData));
  CDOrigin[3]->setChecked(Station->isAttr(SBStation::OrigEstimated));
  aLayout -> activate();  
  Layout->addStretch(1);
  Layout->addWidget(BGw);
  Layout->addStretch(1);

  BGw = new QButtonGroup("Attributes", gbox);
  aLayout = new QVBoxLayout(BGw, 20, 5);
  for (int i=0; i<4; i++)
    {
      CDAttr[i] = new QCheckBox(cAttr[i], BGw);
      CDAttr[i]-> setMinimumSize(CDAttr[i]->sizeHint());
      aLayout -> addWidget(CDAttr[i]);
    };
  CDAttr[ 0]->setChecked(Station->isAttr(SBStation::Analysed));
  CDAttr[ 1]->setChecked(Station->isAttr(SBStation::Reserved_10));
  CDAttr[ 2]->setChecked(Station->isAttr(SBStation::Reserved_11));
  CDAttr[ 3]->setChecked(Station->isAttr(SBStation::Reserved_12));
  aLayout -> activate();  

  Layout->addWidget(BGw);
  Layout->addStretch(1);
  Layout->activate();

  //-
  Layout = new QVBoxLayout(W, 10);
  Layout-> addWidget(gbox);
  Layout-> activate();
  return W;
};

QWidget* SBStationEditor::wOLoad()
{
  QWidget*	W = new QWidget(this);
  QBoxLayout	*Layout;
  
  Layout = new QVBoxLayout(W, 10, 10);
  
  OLoadEditor = new SBOLoadEditor(Station->oLoad(), FALSE, W);
  Layout -> addWidget(OLoadEditor, 10);
  Layout -> activate();
  return W;
};

void SBStationEditor::delCoordinates()
{
  SBCoordsListItem	*Cli;
  SBCoordinates		*C;
  if ( (Cli = (SBCoordsListItem*)CoordsLV->currentItem()) && (C = Cli->coords()))
    {
      QListViewItem* NextItem = Cli->itemBelow();
      if (!NextItem) 
	NextItem = Cli->itemAbove();

      if (!QMessageBox::warning(this, "Delete?", "Are you sure to delete coordinates entry valid since\n \""
				+ C->toString() + "\"?\n", "Yes, del' them!", "No, let it live.."))
	{
	  if (Station->refPt().remove(C))
	    {
	      delete Cli;
	      if (NextItem) 
		CoordsLV->setSelected(NextItem, TRUE);
	      isModified = TRUE;
	    }
	  else
	    Log->write(SBLog::ERR, SBLog::INTERFACE, ClassName() + 
		       ": `" + C->toString() + "' not found in the list of coordinates");
	};
    };
};

void SBStationEditor::addCoordinates()
{
  SBCoordinates	*C = new SBCoordinates(Station->r_first(), Station->dR_first(), v3Zero, v3Unit, TZero, 
				       "Manualy created data [overwrite me]");
  Station->refPt().inSort(C);
  SBCoordsListItem* Cli = new SBCoordsListItem(CoordsLV, C);
  
  SBCoordsEditor *CE =  new SBCoordsEditor(Cli, Station, this);
  connect (CE, SIGNAL(coordinatesModified(bool)), SLOT(modified(bool)));
  CE->show();
};

void SBStationEditor::editCoordinates(QListViewItem* /*Item*/)
{
  SBCoordsListItem	*Cli = (SBCoordsListItem*)CoordsLV->currentItem();
  if (Cli)
    {
      SBCoordsEditor *CE =  new SBCoordsEditor(Cli, Station, this);
      connect (CE, SIGNAL(coordinatesModified(bool)), SLOT(modified(bool)));
      CE->show();
    };
};

void SBStationEditor::modified(bool IsModified_)
{  
  isModified = isModified || IsModified_;
  CoordsLV->sort();
};

void SBStationEditor::acquireData()
{
  int				d;
  double			f;
  bool				isOK;
  Technique			Tech = TECH_UNKN;
  /*  SBStation::SClassCoord	ClassC = SBStation::Class_Unknown;*/
  SBStation::SMountType		MountType = SBStation::Mnt_Unkn;
 
  if (isNew)
    {
      isModified = TRUE;
      if (RBPtType[0]->isChecked()) Station->setPointType(SBStation::Pt_Antenna);
      else Station->setPointType(SBStation::Pt_Marker);
      
      Str=EDome->text();
      d = Str.toInt(&isOK);
      if (isOK) Station->setDomeMinor(d);
    };

  if ((Str=EName->text())!=Station->name())
    {
      isModified = TRUE;
      Station->setName(Str);
    };

  
  Str=ECDP->text();
  d = Str.toInt(&isOK);
  if (isOK && d!=Station->CDP())
    {
      isModified = TRUE;
      Station->setCDP(d);
    };

  if ((Str=ECharID->text())!=Station->charID())
    {
      isModified = TRUE;
      Station->setCharID(Str);
    };

  if ((Str=EDescr->text())!=Station->description())
    {
      isModified = TRUE;
      Station->setDescription(Str);
    };

  if ((Str=MEComments->text())!=Station->comments())
    {
      isModified = TRUE;
      Station->setComments(Str);
    };


  if (RBTech[ 0]->isChecked()) Tech = TECH_VLBI;
  if (RBTech[ 1]->isChecked()) Tech = TECH_OA;
  if (RBTech[ 2]->isChecked()) Tech = TECH_LLR;
  if (RBTech[ 3]->isChecked()) Tech = TECH_GPS;
  if (RBTech[ 4]->isChecked()) Tech = TECH_SLR;
  if (RBTech[ 5]->isChecked()) Tech = TECH_DORIS;
  if (RBTech[ 6]->isChecked()) Tech = TECH_COMBINED;
  if (RBTech[ 7]->isChecked()) Tech = TECH_CG;
  if (RBTech[ 8]->isChecked()) Tech = TECH_AD;
  if (RBTech[ 9]->isChecked()) Tech = TECH_TIE;
  if (RBTech[10]->isChecked()) Tech = TECH_UNKN;
  if (Tech!=Station->tech())
    {
      isModified = TRUE;
      Station->setTech(Tech);
    };


  for (int i=0; i<3; i++)
    {
      Str=EV[i]->text();
      f = Str.toDouble(&isOK);
      if (isOK && f!=Station->v().at((DIRECTION)i))
	{
	  isModified = TRUE;
	  Station->setV_i((DIRECTION)i, f);
	};
      Str=EdV[i]->text();
      f = Str.toDouble(&isOK);
      if (isOK && f!=Station->v_err().at((DIRECTION)i))
	{
	  isModified = TRUE;
	  Station->setV_err_i((DIRECTION)i, f);
	};
    };

  Str=EAxisOffset->text();
  f = Str.toDouble(&isOK);
  if (isOK && f!=Station->axisOffset())
    {
      isModified = TRUE;
      Station->setAxisOffset(f);
    };
  
  /*
    if (RBClassCoord[ 0]->isChecked()) ClassC = SBStation::Class_A;
    if (RBClassCoord[ 1]->isChecked()) ClassC = SBStation::Class_B;
    if (RBClassCoord[ 2]->isChecked()) ClassC = SBStation::Class_C;
    if (RBClassCoord[ 3]->isChecked()) ClassC = SBStation::Class_D;
    if (RBClassCoord[ 4]->isChecked()) ClassC = SBStation::Class_E;
    if (RBClassCoord[ 5]->isChecked()) ClassC = SBStation::Class_Z;
    if (RBClassCoord[ 6]->isChecked()) ClassC = SBStation::Class_Unknown;
    if (ClassC!=Station->classCoord())
    {
    isModified = TRUE;
    Station->setClassCoord(ClassC);
    };
  */
  
  if (RBMountType[ 0]->isChecked()) MountType = SBStation::Mnt_AZEL;
  if (RBMountType[ 1]->isChecked()) MountType = SBStation::Mnt_EQUA;
  if (RBMountType[ 2]->isChecked()) MountType = SBStation::Mnt_X_YN;
  if (RBMountType[ 3]->isChecked()) MountType = SBStation::Mnt_X_YE;
  if (RBMountType[ 4]->isChecked()) MountType = SBStation::Mnt_Richmond;
  if (RBMountType[ 5]->isChecked()) MountType = SBStation::Mnt_Unkn;
  if (MountType!=Station->mountType())
    {
      isModified = TRUE;
      Station->setMountType(MountType);
    };

  if (OLoadEditor->collectData()) 
    {
      isModified = TRUE;
      if (Station->isAttr(SBStation::hasOwnOLoad)) 
	{
	  if (!OLoadEditor->isDisabled)
	    Station->setOLoad(*(OLoadEditor->OLoad));
	  else
	    Station->deleteOLoad();
	}
      else if (!OLoadEditor->isDisabled)
	Station->createOLoad(*(OLoadEditor->OLoad));
    };
  
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBOLoadEditor..										*/
/*												*/
/*==============================================================================================*/
SBOLoadEditor::SBOLoadEditor(SBOLoad* OLoad_, bool isSite_, 
			     QWidget* parent, const char* name)
  : QWidget(parent, name)
{
  static const QString sOceanWaveName[]=
  {"M2", "S2", "N2", "K2", "K1", "O1", "P1", "Q1", "Mf", "Mm", "Ssa"};
  static const QString sRowName[]=
  {"Up, m", "EW, m", "NS, m", "Up, d", "EW, d", "NS, d"};
  
  QPushButton	*AddButton, *DeleteButton, *ImportButton, *ExportButton;
  QSize		Size;

  QLabel*	label;
  QString	Str;
  
  
  isSite	= isSite_;
  isDisabled	= FALSE;
  isModified	= FALSE;
  
  if (!OLoad_)
    {
      OLoad = new SBOLoad;
      isDisabled = TRUE;
    }
  else 
    if (isSite) OLoad  = OLoad_;
    else
      OLoad = new SBOLoad(*OLoad_);
  
  QBoxLayout* Layout = new QVBoxLayout(this, 10,10);
  Layout->addStretch(1);
  QGroupBox*	gbox = new QGroupBox("Amplitudes and Phases", this);
  QGridLayout*  grid = new QGridLayout(gbox, 7,12, 20,5);
  for (int j=0; j<11; j++)
    {
      label = new QLabel(sOceanWaveName[j], gbox);
      label->setAlignment(AlignCenter);
      label->setMinimumSize(label->sizeHint());
      grid ->addWidget(label, 0,1+j);
    };
  for (int i=0; i<6; i++)
    {
      label = new QLabel(sRowName[i], gbox);
      label -> setMinimumSize(label->sizeHint());
      grid  -> addWidget(label, 1+i,0);
    };
  for (int i=0; i<3; i++)
    for (int j=0; j<11; j++)
      {
	EAmplitude[i][j] = new QLineEdit(gbox);
	EAmplitude[i][j]-> setText(Str.sprintf("%7.5f", OLoad->amplitude(i,j)));
	EAmplitude[i][j]-> setMinimumWidth (EAmplitude[i][j]->fontMetrics().width(Str) + 2);
	EAmplitude[i][j]-> setMinimumHeight(EAmplitude[i][j]->sizeHint().height());
	EAmplitude[i][j]-> setEnabled(!isDisabled);
	grid   -> addWidget(EAmplitude[i][j], 1+i,1+j);
      };
  for (int i=0; i<3; i++)
    for (int j=0; j<11; j++)
      {
	EPhase[i][j] = new QLineEdit(gbox);
	EPhase[i][j]-> setText(Str.sprintf("%6.1f", OLoad->phase(i,j)));
	EPhase[i][j]-> setMinimumWidth (EPhase[i][j]->fontMetrics().width(Str) + 2);
	EPhase[i][j]-> setMinimumHeight(EPhase[i][j]->sizeHint().height());
	EPhase[i][j]-> setEnabled(!isDisabled);
	grid   -> addWidget(EPhase[i][j], 4+i,1+j);
      };
  grid   -> activate();
  Layout -> addWidget(gbox);

  grid = new QGridLayout(3,2, 5);
  Layout ->addLayout(grid);

  label = new QLabel("Comment:", this);
  label -> setMinimumSize(label->sizeHint());
  grid  -> addWidget(label, 0, 0);

  label = new QLabel("Comment:", this);
  label -> setMinimumSize(label->sizeHint());
  grid  -> addWidget(label, 1, 0);

  label = new QLabel("Info:", this);
  label -> setMinimumSize(label->sizeHint());
  grid  -> addWidget(label, 2, 0);

  EComment1 = new QLineEdit(this);
  EComment1 -> setText(OLoad->commentLine1());
  EComment1 -> setMinimumSize(EComment1->sizeHint());
  EComment1 -> setEnabled(!isDisabled);
  grid  -> addWidget(EComment1, 0,1);

  EComment2 = new QLineEdit(this);
  EComment2 -> setText(OLoad->commentLine2());
  EComment2 -> setMinimumSize(EComment2->sizeHint());
  EComment2 -> setEnabled(!isDisabled);
  grid  -> addWidget(EComment2, 1,1);
  
  EInfo = new QLineEdit(this);
  EInfo-> setText(OLoad->info());
  EInfo-> setMinimumSize(EInfo->sizeHint());
  EInfo -> setEnabled(!isDisabled);
  grid -> addWidget(EInfo, 2,1);

  grid -> setColStretch(1,10);

  Layout->addStretch(1);

  if (!isSite)
    {
      QBoxLayout* SubLayout = new QHBoxLayout(10);
      Layout ->addLayout(SubLayout);
      SubLayout->addStretch(1);

      SubLayout->addWidget(AddButton    = new QPushButton("Create", this));
      SubLayout->addWidget(DeleteButton = new QPushButton("Delete", this));
      SubLayout->addWidget(ImportButton = new QPushButton("Import", this));
      SubLayout->addWidget(ExportButton = new QPushButton("Export", this));
      
      DeleteButton->setMinimumSize(Size=DeleteButton->sizeHint());
      AddButton   ->setMinimumSize(Size);
      ImportButton->setMinimumSize(Size);
      ExportButton->setMinimumSize(Size);
      
      connect(AddButton,    SIGNAL(clicked()), SLOT(createOLoad()));
      connect(DeleteButton, SIGNAL(clicked()), SLOT(deleteOLoad()));
      //      connect(ImportButton, SIGNAL(clicked()), SLOT(importData()));
      //      connect(ExportButton, SIGNAL(clicked()), SLOT(exportData()));
    };
  Layout->activate();
};

SBOLoadEditor::~SBOLoadEditor()
{
  if (!isSite && OLoad) delete OLoad;
};

void SBOLoadEditor::createOLoad()
{
  if (!isDisabled) return;

  isDisabled = FALSE;
  isModified = TRUE;
  OLoad->setPickedUp(SBOLoad::P_Manual);

  for (int i=0; i<3; i++)
    for (int j=0; j<11; j++)
      {
	EAmplitude[i][j]-> setEnabled(!isDisabled);
	EPhase[i][j]    -> setEnabled(!isDisabled);
      };
  EComment1 -> setEnabled(!isDisabled);
  EComment2 -> setEnabled(!isDisabled);
  EInfo     -> setEnabled(!isDisabled);
};

void SBOLoadEditor::deleteOLoad()
{
  if (isDisabled) return;

  isDisabled = TRUE;
  isModified = TRUE;
  OLoad->setPickedUp(SBOLoad::P_NA);

  for (int i=0; i<3; i++)
    for (int j=0; j<11; j++)
      {
	EAmplitude[i][j]-> setEnabled(!isDisabled);
	EPhase[i][j]    -> setEnabled(!isDisabled);
      };
  EComment1 -> setEnabled(!isDisabled);
  EComment2 -> setEnabled(!isDisabled);
  EInfo     -> setEnabled(!isDisabled);
};

bool SBOLoadEditor::collectData()
{
  QString	Str;
  double	f;
  bool		isOK;
  
  if (!isDisabled)
    {
      if ((Str=EComment1->text())!=OLoad->commentLine1())
	{
	  isModified = TRUE;
	  OLoad->setCommentLine1(Str);
	};
      if ((Str=EComment2->text())!=OLoad->commentLine2())
	{
	  isModified = TRUE;
	  OLoad->setCommentLine2(Str);
	};
      if ((Str=EInfo->text())!=OLoad->info())
	{
	  isModified = TRUE;
	  OLoad->setInfo(Str);
	};
      
      for (int i=0; i<3; i++)
	for (int j=0; j<11; j++)
	  {
	    Str=EAmplitude[i][j]->text();
	    f = Str.toDouble(&isOK);
	    if (isOK && f!=OLoad->amplitude(i,j))
	      {
		isModified = TRUE;
		OLoad->setAmplitude(i,j, f);
	      };
	    
	    Str=EPhase[i][j]->text();
	    f = Str.toDouble(&isOK);
	    if (isOK && f!=OLoad->phase(i,j))
	      {
		isModified = TRUE;
		OLoad->setPhase(i,j, f);
	      };
	  };
    };
  return isModified;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBCoordsEditor..										*/
/*												*/
/*==============================================================================================*/
SBCoordsEditor::SBCoordsEditor(SBCoordsListItem* Cli_, SBStation* Station_,
			       QWidget* parent, const char* name, bool modal, WFlags f)
  : QDialog(parent, name, modal, f)
{
  isModified = FALSE;
  Station = Station_;
  Coo = (Cooli=Cli_)?Coo = Cooli->coords():NULL;

  setCaption("Coordinates Editor for the " + Station->name() + " [" + Station->id().toString() + "]");

  QLabel	*label;
  QGroupBox	*gbox;
  QBoxLayout	*Layout, *SubLayout;
  QGridLayout	*grid;
  
  int		Yr=0, Mo=0, Dy=0, Hr=0, Mi=0;
  double	Se=0.0;
  Vector3	R, DR;
  Vector3	V, DV;
  QString	Str;
  
  Yr = Coo->year();
  Mo = Coo->month();
  Dy = Coo->day();
  Hr = Coo->hour();
  Mi = Coo->min();
  Se = Coo->sec();
  R  = Coo->coo();
  DR = Coo->cooErrors();
  V  = Coo->vel();
  DV = Coo->velErrors();
  

  Layout = new QVBoxLayout(this, 20, 20);
  Layout->addStretch(1);

  gbox = new QGroupBox("Valid since", this);
  grid = new QGridLayout(gbox, 2, 11,   20, 5);
  //
  label= new QLabel("YYYY", gbox);
  label->setMinimumSize(label->sizeHint());
  grid ->addWidget(label, 0, 0);

  label= new QLabel("/", gbox);
  label->setMinimumSize(label->sizeHint());
  grid ->addWidget(label, 1, 1);

  label= new QLabel("MM", gbox);
  label->setMinimumSize(label->sizeHint());
  grid ->addWidget(label, 0, 2);

  label= new QLabel("/", gbox);
  label->setMinimumSize(label->sizeHint());
  grid ->addWidget(label, 1, 3);

  label= new QLabel("DD", gbox);
  label->setMinimumSize(label->sizeHint());
  grid ->addWidget(label, 0, 4);

  grid->addColSpacing(5, 15);

  label= new QLabel("HH", gbox);
  label->setMinimumSize(label->sizeHint());
  grid ->addWidget(label, 0, 6);

  label= new QLabel(":", gbox);
  label->setMinimumSize(label->sizeHint());
  grid ->addWidget(label, 1, 7);

  label= new QLabel("MM", gbox);
  label->setMinimumSize(label->sizeHint());
  grid ->addWidget(label, 0, 8);

  label= new QLabel(":", gbox);
  label->setMinimumSize(label->sizeHint());
  grid ->addWidget(label, 1, 9);

  label= new QLabel("SS.SS", gbox);
  label->setMinimumSize(label->sizeHint());
  grid ->addWidget(label, 0,10);

  //
  leYr = new QLineEdit(Str.sprintf("%04d", Yr), gbox);
  leYr->setFixedWidth(leYr->fontMetrics().width("YYYY")+10);
  grid->addWidget(leYr, 1, 0);

  leMo = new QLineEdit(Str.sprintf("%02d", Mo), gbox);
  leMo->setFixedWidth(leMo->fontMetrics().width("MM")+10);
  grid->addWidget(leMo, 1, 2);

  leDy = new QLineEdit(Str.sprintf("%02d", Dy), gbox);
  leDy->setFixedWidth(leDy->fontMetrics().width("MM")+10);
  grid->addWidget(leDy, 1, 4);

  leHr = new QLineEdit(Str.sprintf("%02d", Hr), gbox);
  leHr->setFixedWidth(leHr->fontMetrics().width("MM")+10);
  grid->addWidget(leHr, 1, 6);

  leMi = new QLineEdit(Str.sprintf("%02d", Mi), gbox);
  leMi->setFixedWidth(leMi->fontMetrics().width("MM")+10);
  grid->addWidget(leMi, 1, 8);

  leSe = new QLineEdit(Str.sprintf("%5.2f", Se), gbox);
  leSe->setFixedWidth(leSe->fontMetrics().width("SS.SS")+10);
  grid->addWidget(leSe, 1,10);
  grid->activate();

  Layout->addWidget(gbox);
  Layout->addStretch(1);
  

  //
  gbox = new QGroupBox("Coordinates", this);
  grid = new QGridLayout(gbox, 3, 4,   20, 5);

  label= new QLabel("Rx (m)", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, 0, 1);

  label= new QLabel("Ry (m)", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, 0, 2);
  
  label= new QLabel("Rz (m)", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, 0, 3);

  label= new QLabel("Values:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, 1, 0);

  label= new QLabel("Sigmas:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, 2, 0);
  
  for (int i=0; i<3; i++)
    {
      le_R[i] = new QLineEdit(Str.sprintf("%.5f", R.at((DIRECTION)i)), gbox);
      le_R[i]->setMinimumWidth (le_R[i]->fontMetrics().width(Str) + 20);
      le_R[i]->setMinimumHeight(le_R[i]->sizeHint().height());
      grid->addWidget(le_R[i], 1, i+1);
      
      leDR[i] = new QLineEdit(Str.sprintf("%.5f", DR.at((DIRECTION)i)), gbox);
      leDR[i]->setMinimumWidth (leDR[i]->fontMetrics().width(Str) + 20);
      leDR[i]->setMinimumHeight(leDR[i]->sizeHint().height());
      grid->addWidget(leDR[i], 2, i+1);
    };
  setTabOrder(le_R[0], le_R[1]);
  setTabOrder(le_R[1], le_R[2]);
  setTabOrder(le_R[2], leDR[0]);
  setTabOrder(leDR[0], leDR[1]);
  setTabOrder(leDR[1], leDR[2]);

  grid->activate();

  Layout->addWidget(gbox);
  Layout->addStretch(1);

  //
  gbox = new QGroupBox("Velocities", this);
  grid = new QGridLayout(gbox, 4, 4,   20, 5);

  label= new QLabel("Vx (m/y)", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, 0, 1);

  label= new QLabel("Vy (m/y)", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, 0, 2);
  
  label= new QLabel("Vz (m/y)", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, 0, 3);

  label= new QLabel("Values:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, 1, 0);

  label= new QLabel("Sigmas:", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, 2, 0);
  
  for (int i=0; i<3; i++)
    {
      le_V[i] = new QLineEdit(Str.sprintf("%.5f", V.at((DIRECTION)i)), gbox);
      le_V[i]->setMinimumWidth (le_V[i]->fontMetrics().width(Str) + 20);
      le_V[i]->setMinimumHeight(le_V[i]->sizeHint().height());
      grid->addWidget(le_V[i], 1, i+1);
      
      leDV[i] = new QLineEdit(Str.sprintf("%.5f", DV.at((DIRECTION)i)), gbox);
      leDV[i]->setMinimumWidth (leDV[i]->fontMetrics().width(Str) + 20);
      leDV[i]->setMinimumHeight(leDV[i]->sizeHint().height());
      grid->addWidget(leDV[i], 2, i+1);
    };
  setTabOrder(le_V[0], le_V[1]);
  setTabOrder(le_V[1], le_V[2]);
  setTabOrder(le_V[2], leDV[0]);
  setTabOrder(leDV[0], leDV[1]);
  setTabOrder(leDV[1], leDV[2]);

  cbIsUseVels = new QCheckBox("Is this velocity record should be applied?", gbox);
  cbIsUseVels -> setChecked(Coo->isUseVels());
  cbIsUseVels -> setMinimumSize(cbIsUseVels->sizeHint());
  cbIsUseVels -> setFocusPolicy(QWidget::ClickFocus);
  grid -> addMultiCellWidget(cbIsUseVels, 3, 3, 0, 3);

  grid->activate();

  Layout->addWidget(gbox);
  Layout->addStretch(1);
  

  //
  SubLayout = new QHBoxLayout(10);
  Layout->addLayout(SubLayout);
  Layout->addStretch(1);
  
  label= new QLabel("Descr: ", this);
  label->setMinimumSize(label->sizeHint());
  SubLayout->addWidget(label);

  leEventName = new QLineEdit(Coo->eventName(), this);
  leEventName->setMinimumSize(leEventName->sizeHint());
  SubLayout->addWidget(leEventName, 2);


  //--
  SubLayout = new QHBoxLayout(10);
  Layout -> addLayout(SubLayout);
  SubLayout-> addStretch(1);

  QPushButton *Ok	= new QPushButton("OK", this);
  Ok->setDefault(TRUE);
  QPushButton *Cancel	= new QPushButton("Cancel", this);

  QSize	BtnSize;

  Cancel->setMinimumSize((BtnSize=Cancel->sizeHint()));
  Ok->setMinimumSize(BtnSize);

  SubLayout->addWidget(Ok);
  SubLayout->addWidget(Cancel);

  connect(Ok, SIGNAL(clicked()), SLOT(accept()));
  connect(Cancel, SIGNAL(clicked()), SLOT(reject()));
  Layout->activate();
};

void SBCoordsEditor::accept()
{
  acquireData();
  QDialog::accept();
  if (Cooli && isModified)
    Cooli->repaint();
  emit coordinatesModified(isModified);
  delete this;
};

void SBCoordsEditor::reject()
{
  emit coordinatesModified(FALSE);
  QDialog::reject(); 
  delete this;
};

void SBCoordsEditor::acquireData()
{
  if (!Cooli || !Coo)
    return;

  bool		isOK = TRUE, isTmp;
  int		Yr = 0;
  int		Mo = 0;
  int		Dy = 0;
  int		Hr = 0;
  int		Mi = 0;
  double	Se = 0;
  Vector3	A;

  if (isOK)
    Yr = leYr->text().toInt(&isOK);
  if (isOK)
    Mo = leMo->text().toInt(&isOK);
  if (isOK)
    Dy = leDy->text().toInt(&isOK);
  if (isOK)
    Hr = leHr->text().toInt(&isOK);
  if (isOK)
    Mi = leMi->text().toInt(&isOK);
  if (isOK)
    Se = leSe->text().toDouble(&isOK);

  SBMJD t(Yr, Mo, Dy, Hr, Mi, Se);
  if (isOK && t!=*Coo)
    {
      isModified = TRUE;
      *(SBMJD*)Coo = t;
    };
  
  if (isOK)
    A[X_AXIS] = le_R[0]->text().toDouble(&isOK);
  if (isOK)
    A[Y_AXIS] = le_R[1]->text().toDouble(&isOK);
  if (isOK)
    A[Z_AXIS] = le_R[2]->text().toDouble(&isOK);
  if (isOK && A!=Coo->coo())
    {
      isModified = TRUE;
      Coo->setCoo(A);
    };
  if (isOK)
    A[X_AXIS] = leDR[0]->text().toDouble(&isOK);
  if (isOK)
    A[Y_AXIS] = leDR[1]->text().toDouble(&isOK);
  if (isOK)
    A[Z_AXIS] = leDR[2]->text().toDouble(&isOK);
  if (isOK && A!=Coo->cooErrors())
    {
      isModified = TRUE;
      Coo->setCooErrors(A);
    };

  if (isOK)
    A[X_AXIS] = le_V[0]->text().toDouble(&isOK);
  if (isOK)
    A[Y_AXIS] = le_V[1]->text().toDouble(&isOK);
  if (isOK)
    A[Z_AXIS] = le_V[2]->text().toDouble(&isOK);
  if (isOK && A!=Coo->vel())
    {
      isModified = TRUE;
      Coo->setVel(A);
    };
  if (isOK)
    A[X_AXIS] = leDV[0]->text().toDouble(&isOK);
  if (isOK)
    A[Y_AXIS] = leDV[1]->text().toDouble(&isOK);
  if (isOK)
    A[Z_AXIS] = leDV[2]->text().toDouble(&isOK);
  if (isOK && A!=Coo->velErrors())
    {
      isModified = TRUE;
      Coo->setVelErrors(A);
    };

  if(isOK)
    isTmp = cbIsUseVels->isChecked();
  if (isOK && isTmp!=Coo->isUseVels())
    {
      isModified = TRUE;
      Coo->setIsUseVels(isTmp);
    };

  if (isOK && leEventName->text()!=Coo->eventName())
    {
      isModified = TRUE;
      Coo->setEventName(leEventName->text());
    };
};
/*==============================================================================================*/





/*==============================================================================================*/
/*==============================================================================================*/
