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

#include "SbIStuffSource.H"

#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qdir.h>
#include <qfiledialog.h>
#include <qfileinfo.h>
#include <qframe.h>
#include <qgroupbox.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlistbox.h>
#include <qmessagebox.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qtooltip.h>
#include <qvalidator.h>
#include <qvbuttongroup.h>

#include "SbSetup.H"
#include "SbGeoEop.H"


QPixmap* pmSrcStatus[5];
QPixmap* pmSrcHLink;



/*==============================================================================================*/
/*												*/
/* SBSourceListItem..										*/
/*												*/
/*==============================================================================================*/
SBSourceListItem::SBSourceListItem(QListView* parent, SBSource *Source_)
  : QListViewItem(parent)
{
  isFaked = FALSE;
  if (Source_) Source = Source_;
  else 
    {
      Source = new SBSource;
      isFaked = TRUE;
    };
};

SBSourceListItem::~SBSourceListItem()
{
  if (isFaked && Source) delete Source;
  Source=NULL;
};
 
QString SBSourceListItem::text(int col) const
{
  //  static const char* cStatus[4] = {"D", "C", "O", "N", "?"};
  static const char* cType  [6] = {"Q", "G", "L", "l", "O", "?"};
  TmpString = "NULL";

  if (Source)
    switch (col)
      {
      case 0: 
	TmpString = (const char*)Source->name();
	break;
      case 1: 
	//	TmpString = ""; //cStatus[Source->status()];
	TmpString = Source->isAttr(SBSource::StatusSuspected)?"!":""; //cStatus[Source->status()];
	break;
      case 2: 
	TmpString = ""; //Source->isHLink()?"*":" ";
	break;
      case 3: 
	TmpString =cType[Source->type()];
	break;
      case 4: 
	TmpString = (const char*)Source->RA2str();
	break;
      case 5: 
	TmpString = (const char*)Source->DN2str();
	break;
      case 6: 
	TmpString.sprintf("% 9.4f",double(1000.0*RAD2HR*3600.0*Source->ra_err()));
	break;
      case 7: 
	TmpString.sprintf("% 9.4f",double(1000.0*RAD2DEG*3600.0*Source->dn_err()));
	break;
      case 8: 
	TmpString = (Source->mv()>0.0)?(const char*)TmpString.sprintf("% 4.1f", Source->mv()):" ";
	break;
      case 9: 
	TmpString = (Source->lastMJD() - Source->firstMJD() > 0.0)? 
	  (const char*)TmpString.
	  sprintf("% 7.1f", (double)(Source->lastMJD() - Source->firstMJD())):" ";
        break;
      case 10: 
	TmpString.sprintf("% 7d", Source->nObsApr());
	break;
      case 11: 
	TmpString.sprintf("% 5d", Source->nSessApr());
	break;
      };
  return TmpString;
};

QString SBSourceListItem::key(int col, bool) const
{
  static const char* cStatus[5] = {"AD", "BC", "CO", "DN", "E?"};
  static const char* cType  [6] = {"AQ", "BG", "CL", "Dl", "EA", "F?"};
  TmpString = "NULL";

  if (Source)
    switch (col)
      {
      case 0: TmpString = (const char*)Source->name();
	break;
      case 1: 
	TmpString = cStatus[Source->status()];
	if (Source->isAttr(SBSource::StatusSuspected))
	  TmpString += "Z";
	break;
      case 2: TmpString = Source->isHLink()?"*":" ";
	break;
      case 3: TmpString =cType[Source->type()];
	break;
      case 4: TmpString.sprintf("%016.12f", Source->ra());
	break;
      case 5: TmpString.sprintf("%016.12f", double(7.0+Source->dn())); //just for sorting
	break;
      case 6: TmpString.sprintf("%09.4f",   double(1000.0*RAD2HR*3600.0*Source->ra_err()));
	break;
      case 7: TmpString.sprintf("%09.4f",   double(1000.0*RAD2DEG*3600.0*Source->dn_err()));
	break;
      case 8: TmpString.sprintf("%04.1f", Source->mv());
	break;
      case 9: TmpString.sprintf("%07.1f",   (double)(Source->lastMJD()-Source->firstMJD()));
	break;
      case 10:TmpString.sprintf("%07d", Source->nObsApr());
	break;
      case 11:TmpString.sprintf("%05d", Source->nSessApr());
	break;
      };
  return TmpString;
};

/* till the next time...*/
const QPixmap * SBSourceListItem::pixmap (int col) const
{
  QPixmap *pm=0;
  if (Source)
    switch (col)
      {
      case 0: pm=0;
	break;
      case 1:
	pm = pmSrcStatus[Source->status()];
	break;
      case 2: pm = Source->isHLink()?pmSrcHLink:0;
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
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBAliasListItem..										*/
/*												*/
/*==============================================================================================*/
SBAliasListItem::SBAliasListItem(QListView* parent, const char* Ali_, const char* Ref_)
  : QListViewItem(parent)
{
  isFaked = FALSE;
  if (Ali_ && Ref_) 
    {
      Alias = Ali_;
      Reference = Ref_;
    }
  else 
    {
      Alias = "NULL";
      Reference = "NULL";
      isFaked = TRUE;
    };
};

QString SBAliasListItem::text(int col) const
{
  switch (col)
    {
    case 0: return (const char*)Alias;
      break;
    case 1: return (const char*)Reference;
      break;
    };
  return "NULL";
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBStuffSources..										*/
/*												*/
/*==============================================================================================*/
SBStuffSources::SBStuffSources(QWidget* parent, const char* name, bool modal, WFlags f)
  : QDialog(parent, name, modal, f)
{
  numOpenedWindows = 0;
  
  setCaption("Sources");
  
  QPushButton	*AddButton, *DeleteButton, *OKButton, *CancelButton, *ImportButton, *ExportButton;
  QSize		Size;
  QGroupBox	*gbox;
  QBoxLayout	*Layout, *SubLayout, *aLayout, *bLayout;
  QLabel	*label;
  QFrame	*frame;
  
  if (!(Wcrf=SetUp->loadCRF())) Wcrf = new SB_CRF;
  if (!Wcrf->count()) Wcrf->inSort(new SBSource("0000+000", 0.0, 0.0));
  Wcrf->prepareAliases4Output();

  
  gbox	 = new QGroupBox("CRF", this);
  Layout = new QVBoxLayout(gbox, 20, 10);
  
  SubLayout = new QHBoxLayout();
  Layout -> addLayout(SubLayout, 10);
  
  
  //--
  aLayout = new QVBoxLayout();
  SubLayout -> addLayout(aLayout, 5);
  
  
  // set up aliases list box:
  AliasesLV = new QListView(gbox);
  AliasesLV -> addColumn("Sourse's Alias ");
  AliasesLV -> addColumn(" Refers to");
  AliasesLV -> setColumnAlignment( 1, AlignRight);

  QDictIterator<QString> it(*Wcrf->aliasDict());
  while (it.current())
    {
      (void) new SBAliasListItem(AliasesLV, QString(it.currentKey()), *it.current());
      ++it;
    };
  AliasesLV -> setAllColumnsShowFocus(TRUE);
  AliasesLV -> setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  AliasesLV -> setMinimumSize(AliasesLV->sizeHint());
  AliasesLV -> setFocusPolicy(QWidget::StrongFocus);
  if (Wcrf->aliasDict()->count() && AliasesLV->firstChild())
    AliasesLV -> setSelected(AliasesLV->firstChild(), TRUE);

  aLayout   -> addWidget(AliasesLV, 10);

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
  
  //--
  QGridLayout  *grid = new QGridLayout(5,3);
  aLayout -> addLayout(grid);
  grid -> setColStretch(1,5);
  
  frame = new QFrame(gbox);
  frame -> setFrameStyle(QFrame::Sunken | QFrame::HLine);
  frame -> setMinimumHeight(6);
  grid-> addMultiCellWidget(frame, 0, 0, 0, 2);
  
  // info:
  label	= new QLabel("Sources/Aliases:", gbox);
  label	-> setMinimumSize(label->sizeHint());
  grid -> addMultiCellWidget(label, 1,1, 0,1);

  str.sprintf(" %d/%d", Wcrf->count(), Wcrf->aliasDict()->count());
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

  CRFLabel = new QLineEdit(gbox);
  CRFLabel -> setText(Wcrf->label().toString());
  CRFLabel -> setMinimumHeight(CRFLabel->sizeHint().height());
  CRFLabel -> setMinimumWidth(CRFLabel->fontMetrics().width(Wcrf->label().toString())+10);
  grid -> addMultiCellWidget(CRFLabel, 3,3, 1,2);

  label	= new QLabel("Epoch:", gbox);
  label	-> setMinimumSize(label->sizeHint());
  grid -> addWidget(label, 4, 0);

  CRFEpoch = new QLineEdit(gbox);
  CRFEpoch -> setText(Wcrf->epoch().toString(SBMJD::F_Year));
  CRFEpoch -> setValidator(new QDoubleValidator(1900.0, 2200.0, 1, gbox));
  CRFEpoch -> setMinimumHeight(CRFEpoch->sizeHint().height());
  CRFEpoch -> setMinimumWidth(CRFEpoch->fontMetrics().width(Wcrf->label().toString())+10);
  grid -> addMultiCellWidget(CRFEpoch, 4,4, 1,2);

  //  set up sources list box:
  SourcesLV = new QListView(gbox);
  SourcesLV -> addColumn("Source");
  SourcesLV -> setColumnAlignment( 0, AlignLeft);
  SourcesLV -> addColumn("S");
  SourcesLV -> setColumnAlignment( 1, AlignRight);
  SourcesLV -> addColumn("H");
  SourcesLV -> setColumnAlignment( 2, AlignCenter);
  SourcesLV -> addColumn("T");
  SourcesLV -> setColumnAlignment( 3, AlignRight);
  SourcesLV -> addColumn(" Right Ascension ");
  SourcesLV -> setColumnAlignment( 4, AlignRight);
  SourcesLV -> addColumn("Declination North");
  SourcesLV -> setColumnAlignment( 5, AlignRight);
  SourcesLV -> addColumn("uncert RA");
  SourcesLV -> setColumnAlignment( 6, AlignRight);
  SourcesLV -> addColumn("uncert De");
  SourcesLV -> setColumnAlignment( 7, AlignRight);
  SourcesLV -> addColumn("mv");
  SourcesLV -> setColumnAlignment( 8, AlignRight);
  SourcesLV -> addColumn("interval");
  SourcesLV -> setColumnAlignment( 9, AlignRight);
  SourcesLV -> addColumn("NObs");
  SourcesLV -> setColumnAlignment(10, AlignRight);
  SourcesLV -> addColumn("NSess");
  SourcesLV -> setColumnAlignment(11, AlignRight);
  SourcesLV -> setAllColumnsShowFocus(TRUE);

  for (SBSource* W=Wcrf->first(); W; W=Wcrf->next())
    (void) new SBSourceListItem(SourcesLV, W);
  SourcesLV -> setMinimumHeight(SourcesLV->sizeHint().height()*2);
  SourcesLV -> setMinimumWidth(SourcesLV->sizeHint().width());
  SourcesLV -> setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  if (SourcesLV->firstChild())
    SourcesLV -> setSelected(SourcesLV->firstChild(), TRUE);
  SourcesLV -> setFocusPolicy(QWidget::StrongFocus);
  SubLayout -> addWidget(SourcesLV,5);

  // comments:
  Layout -> addLayout(bLayout = new QHBoxLayout(10));
  bLayout-> addWidget(new QLabel("Comment:", gbox));
  bLayout->addWidget(CRFComment=new QLineEdit(Wcrf->comment(), gbox), 1);
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
  connect(AddButton,	SIGNAL(clicked()), SLOT(insertEntryS()));
  connect(DeleteButton, SIGNAL(clicked()), SLOT(deleteEntryS()));
  connect(ImportButton, SIGNAL(clicked()), SLOT(importData()));
  connect(ExportButton, SIGNAL(clicked()), SLOT(exportData()));
  
  connect(SourcesLV, SIGNAL(doubleClicked(QListViewItem*)), SLOT(editEntryS(QListViewItem*)));
  connect(AliasesLV, SIGNAL(doubleClicked(QListViewItem*)), SLOT(editEntryA(QListViewItem*)));
  connect(SourcesLV, SIGNAL(returnPressed(QListViewItem*)), SLOT(editEntryS(QListViewItem*)));
  connect(AliasesLV, SIGNAL(returnPressed(QListViewItem*)), SLOT(editEntryA(QListViewItem*)));

  isModifiedCRF = FALSE;
};

SBStuffSources::~SBStuffSources()
{
  if (Wcrf) 
    {
      Wcrf->releaseAliases4Output();
      delete Wcrf;
    };
};

void SBStuffSources::accept()
{
  if (numOpenedWindows) return; //there are open windows dependent upon us..

  bool		isNeed2Save = isModifiedCRF;
  QString	a;
  SBMJD		date;

  QDialog::accept();

  str = CRFLabel->text();
  SBSolutionName *w = new SBSolutionName(SBSolutionName::TYPE_RSC);
  w->fromString(str);
  if (*w!=Wcrf->label())
    {
      Wcrf->setLabel(*w);
      isNeed2Save = TRUE;
    };
  delete w;

  str = CRFEpoch->text();
  if (Wcrf->epoch().toString(SBMJD::F_Year) != str)
    {
      date.setMJD(SBMJD::F_Year, str);
      Wcrf->setEpoch(date);
      isNeed2Save = TRUE;
    };

  if (Wcrf->comment() != CRFComment->text())
    {
      Wcrf->setComment(CRFComment->text());
      isNeed2Save = TRUE;
    };

  if (isNeed2Save) SetUp->saveCRF(Wcrf);
  delete this;
};

void SBStuffSources::reject() 
{
  if (numOpenedWindows) return;
  
  QDialog::reject();
  delete this;
};

void SBStuffSources::reloadCRF()
{
  AliasesLV -> clear();
  SourcesLV -> clear();
  
  if (Wcrf) delete Wcrf;
  if (!(Wcrf=SetUp->loadCRF())) Wcrf = new SB_CRF;
  //  if (!Wcrf->count()) Wcrf->inSort(new SBSource("0000+000", 0.0, 0.0));
  Wcrf->prepareAliases4Output();

  // redraw aliases:
  QDictIterator<QString> it(*Wcrf->aliasDict());
  while (it.current())
    {
      (void) new SBAliasListItem(AliasesLV, QString(it.currentKey()), *it.current());
      ++it;
    };
  if (Wcrf->aliasDict()->count() && AliasesLV->firstChild())
    AliasesLV->setSelected(AliasesLV->firstChild(), TRUE);

  // redraw sources:
  for (SBSource* W=Wcrf->first(); W; W=Wcrf->next())
    (void) new SBSourceListItem(SourcesLV, W);
  if (SourcesLV->firstChild())
    SourcesLV->setSelected(SourcesLV->firstChild(), TRUE);
    
  drawInfo();
};

void SBStuffSources::drawInfo()
{
  CRFLabel -> setText(Wcrf->label().toString());
  CRFLabel -> setMinimumHeight(CRFLabel->sizeHint().height());
  CRFLabel -> setMinimumWidth(CRFLabel->fontMetrics().width(Wcrf->label().toString())+10);
  CRFEpoch -> setText(Wcrf->epoch().toString(SBMJD::F_Year));
  CRFComment -> setText(Wcrf->comment());

  NumSA	-> setText(str.sprintf(" %d/%d", Wcrf->count(), Wcrf->aliasDict()->count()));
};

void SBStuffSources::importData()
{
  if (numOpenedWindows) return; //there are dependants..
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": SBStuffSourceImport selected");

  if (isModifiedCRF) 
    {
      SetUp->saveCRF(Wcrf);
      isModifiedCRF = FALSE;
    };

  //  SBStuffSourcesImport *importWin = new SBStuffSourcesImport(this, 0, TRUE);
  //  connect (importWin, SIGNAL(SourcesImported()), SLOT(reloadCRF()));
  //  importWin->exec();
  //  delete importWin;
  SBSourceImport *importWin = new SBSourceImport();
  connect (importWin, SIGNAL(sourcesImported()), SLOT(reloadCRF()));
  importWin->show();
};

void SBStuffSources::exportData()
{
  QFile		f;
  QTextStream	*s;
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": SBStuffSourceExport selected");

  f.setName(SetUp->path2Output() + SetUp->sources() + ".rsc");
  if (f.open(IO_WriteOnly))
    {      
      s = new SBTS_rsc;
      s->setDevice(&f);
      *(SBTS_rsc*)s << *Wcrf;
      f.close();
      s->unsetDevice();
      if (s) delete s;
      Log->write(SBLog::DBG, SBLog::IO, ClassName() + ": file " + (QString)f.name() + 
		 " has been saved");
    }
  else    
    Log->write(SBLog::ERR, SBLog::IO, ClassName() + ": could not write " + (QString)f.name());
  

  f.setName(SetUp->path2Output() + SetUp->sources() + ".car");
  if (f.open(IO_WriteOnly))
    {      
      s = new SBTS_car;
      s->setDevice(&f);
      *(SBTS_car*)s << *Wcrf;
      f.close();
      s->unsetDevice();
      if (s) delete s;
      Log->write(SBLog::DBG, SBLog::IO, ClassName() + ": file " + (QString)f.name() + 
		 " has been saved");
    }
  else    
    Log->write(SBLog::ERR, SBLog::IO, ClassName() + ": could not write " + (QString)f.name());

  f.setName(SetUp->path2Output() + SetUp->sources() + ".dico");
  if (f.open(IO_WriteOnly))
    {      
      s = new SBTS_dico;
      s->setDevice(&f);
      *(SBTS_dico*)s << *Wcrf;
      f.close();
      s->unsetDevice();
      if (s) delete s;
      Log->write(SBLog::DBG, SBLog::IO, ClassName() + ": file " + (QString)f.name() + 
		 " has been saved");
    }
  else    
    Log->write(SBLog::ERR, SBLog::IO, ClassName() + ": could not write " + (QString)f.name());

};

void SBStuffSources::editEntryA(QListViewItem* AkaItem)
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": SBStuffSourceEditAlias selected");

  SBAliasEditor *AEditor =  new SBAliasEditor ((SBAliasListItem*)AkaItem, Wcrf);
  connect (AEditor, SIGNAL(AliasModified(bool)), SLOT(CRFModified(bool)));
  numOpenedWindows++;
  AEditor->show();
};

void SBStuffSources::insertEntryA()
{  
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": SBStuffSourceInsertAlias selected");

  SBAliasEditor *AEditor =  new SBAliasEditor (AliasesLV, Wcrf);
  connect (AEditor, SIGNAL(AliasModified(bool)), SLOT(CRFModified(bool)));
  numOpenedWindows++;
  AEditor->show();
};

void SBStuffSources::deleteEntryA()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": SBStuffSourceDeleteAlias selected");
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
	  if (Wcrf->aliasDict()->remove(str.left(8)))
	    {
	      delete AkaItem;
	      if (AkaNextItem) AliasesLV->setSelected(AkaNextItem, TRUE);
	      Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + 
			 ": " + str + ", rest in peace..");
	      isModifiedCRF = TRUE;
	      str.sprintf("%d/%d", Wcrf->count(), Wcrf->aliasDict()->count());
	      NumSA -> setText(str);
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

void SBStuffSources::editEntryS(QListViewItem* SrcItem)
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": SBStuffSourceEditSource selected");

  SBSourceEditor *SEditor =  new SBSourceEditor((SBSourceListItem*)SrcItem);
  connect (SEditor, SIGNAL(SourceModified(bool)), SLOT(CRFModified(bool)));
  numOpenedWindows++;
  SEditor->show();
};

void SBStuffSources::insertEntryS()
{  
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": SBStuffSourceInsertSource selected");

  SBSourceEditor *SEditor =  new SBSourceEditor(SourcesLV, Wcrf);
  connect (SEditor, SIGNAL(SourceModified(bool)), SLOT(CRFModified(bool)));
  numOpenedWindows++;
  SEditor->show();
};

void SBStuffSources::deleteEntryS()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": SBStuffSourceDeleteSource selected");
  if (numOpenedWindows) return; //to avoid deletion of an entry being edited..

  SBSourceListItem* SrcItem = (SBSourceListItem*)SourcesLV->currentItem();
  if (SrcItem)
    {
      QListViewItem* SrcNextItem = SrcItem->itemBelow();
      if (!SrcNextItem) SrcNextItem = SrcItem->itemAbove();
      str = SrcItem->Source->name().copy();
      if (!QMessageBox::warning(this, "Delete?", "Are you sure to delete source entry\n \""
				+ str + "\"?\n", "Yes, del them!", "No, let it live.."))
	{
	  if (Wcrf->remove(SrcItem->Source))
	    {
	      delete SrcItem;
	      if (SrcNextItem) SourcesLV->setSelected(SrcNextItem, TRUE);
	      Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + 
			 ": " + str + ", rest in peace..");
	      isModifiedCRF = TRUE;
	      str.sprintf("%d/%d", Wcrf->count(), Wcrf->aliasDict()->count());
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

void SBStuffSources::CRFModified(bool Modified)
{
  if (!numOpenedWindows) 
    std::cerr << "your Editor's windows are diyng more often than have been born\n";
  numOpenedWindows--;
  if (Modified) 
    {
      isModifiedCRF = TRUE;
      NumSA->setText(str.sprintf(" %d/%d", Wcrf->count(), Wcrf->aliasDict()->count()));
    };
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBSourceImport..										*/
/*												*/
/*==============================================================================================*/
SBSourceImport::SBSourceImport(QWidget* parent, const char* name, bool modal, WFlags f)
  : QWizard(parent, name, modal, f)
{
  FFormat = F_UNKN;
  Mode    = REPLACE;

  setupPage1();
  setupPage2_1();
  setupPage2_2();
  setupPage2_3();
  setupPage3();
};

void SBSourceImport::formatChanged(int id)
{
  if (page2_1)
    setAppropriate(page2_1, FALSE);
  if (page2_2)
    setAppropriate(page2_2, FALSE);
  if (page2_3)
    setAppropriate(page2_3, FALSE);

  switch (id)
    {
    case 0:   
      FFormat = F_ICRF95;
      if (page2_1)
	setAppropriate(page2_1, TRUE);
      break;
    case 1:   
      FFormat = F_ICRF99;
      if (page2_2)
	setAppropriate(page2_2, TRUE);
      break;
    case 2:
      FFormat = F_ICRF2009;
      if (page2_3)
	setAppropriate(page2_3, TRUE);
      break;
    default:  FFormat = F_UNKN;
      break;
    };
  
  if ( (FFormat != F_UNKN) )
    setNextEnabled(page1, TRUE);
  else
    setNextEnabled(page1, FALSE);
};

void SBSourceImport::modeChanged(int id)
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
    case 4:   Mode = UPD_COO_ONLY;
      break;
    };
};

bool SBSourceImport::checkFileExists(const QString& fil)
{
  QDir dir(fil);
  return dir.exists(fil);
};

void SBSourceImport::setupPage1()
{
  const int NumOfFormat = 3;
  static const QString sFormat[]=
  {
    "Source coordinates in \"RSC(WGRF)95R01-solution\" format",
    "Source coordinates in \"RSC(WGRF)99R01-solution\" format",
    "Source coordinates in \"would to be ICRF2\" format",
  };
  QRadioButton	*rbFormat[NumOfFormat];

  page1	= new QWidget(this);

  QBoxLayout *Layout	= new QVBoxLayout(page1, 20,10);

  QButtonGroup *gbox = new QVButtonGroup("CRF known formats", page1);
  for (int i=0; i<NumOfFormat; i++)
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

void SBSourceImport::setupPage2_1()
{
  bool		IsNextEnabled = TRUE;
  static int	NumOfFiles = 3;
  static const	QString LabelCB[]=
  {
    "RSC file:",
    "CAR file:",
    "DICO file:"
  };
  static const QString ImportFile[] = {"icrf.rsc", "icrf.car", "icrf.dico"};

  page2_1 = new QWidget(this);
  QGridLayout *grid = new QGridLayout(page2_1,  NumOfFiles+2,3,  20, 5);
  
  for (int i=0; i<NumOfFiles; i++)
    {
      cbFileName1995[i] = new QCheckBox(LabelCB[i], page2_1);
      cbFileName1995[i] -> setChecked(TRUE);
      cbFileName1995[i] -> setMinimumSize(cbFileName1995[i]->sizeHint());
      cbFileName1995[i] -> setFocusPolicy(QWidget::ClickFocus);
      grid -> addWidget(cbFileName1995[i], i+1, 0);
      
      sFileName1995[i] = SetUp->path2ImpICRF() + "icrf_rsc/" + ImportFile[i];
      
      leFileName1995[i] = new QLineEdit(page2_1);
      leFileName1995[i] -> setText(sFileName1995[i]);
      leFileName1995[i] -> setMinimumHeight(leFileName1995[i]->sizeHint().height());
      leFileName1995[i] -> setMinimumWidth (leFileName1995[i]->fontMetrics().width(sFileName1995[i]) + 50);
      grid -> addWidget(leFileName1995[i], i+1, 1);
      
      bFile1995[i] = new QPushButton("Select file..", page2_1);
      bFile1995[i] -> setMinimumSize(bFile1995[i]->sizeHint());
      bFile1995[i] -> setFocusPolicy(QWidget::ClickFocus);
      grid -> addWidget(bFile1995[i], i+1, 2);

      IsNextEnabled = IsNextEnabled && checkFileExists(sFileName1995[i]);
    };
  
  connect(cbFileName1995[0],	SIGNAL(toggled(bool)),	SLOT(setRSCstate1995 (bool)));
  connect(cbFileName1995[1],	SIGNAL(toggled(bool)),	SLOT(setCARstate1995 (bool)));
  connect(cbFileName1995[2],	SIGNAL(toggled(bool)),	SLOT(setDICOstate1995(bool)));

  connect(bFile1995[0],		SIGNAL(clicked()),	SLOT(selectRSCfile1995 ()));
  connect(bFile1995[1],		SIGNAL(clicked()),	SLOT(selectCARfile1995 ()));
  connect(bFile1995[2],		SIGNAL(clicked()),	SLOT(selectDICOfile1995()));

  connect(leFileName1995[0],	SIGNAL(textChanged(const QString&)),SLOT(fnRSCChanged1995 (const QString&)));
  connect(leFileName1995[1],	SIGNAL(textChanged(const QString&)),SLOT(fnCARChanged1995 (const QString&)));
  connect(leFileName1995[2],	SIGNAL(textChanged(const QString&)),SLOT(fnDICOChanged1995(const QString&)));

  grid -> setColStretch(1, 5);
  grid -> setRowStretch(0, 1);
  grid -> setRowStretch(NumOfFiles+1, 1);
  grid -> activate();

  addPage(page2_1, "Files to Load");
  setNextEnabled(page2_1, IsNextEnabled);
};

void SBSourceImport::setupPage2_2()
{
  bool		IsNextEnabled=TRUE;
  static int	NumOfFiles = 6;
  static const	QString LabelCB[]=
  {
    "RSC (defining) file:",
    "RSC (candidate) file:",
    "RSC (other) file:",
    "RSC (new) file:",
    "CAR file:",
    "DICO file:"
  };
  static const QString ImportFile[] = 
  {
    "icrf-Ext1.def",
    "icrf-Ext1.can",
    "icrf-Ext1.oth",
    "icrf-Ext1.new",
    "icrf-Ext1.car_phys",
    "icrf.dico"
  };

  page2_2 = new QWidget(this);
  QGridLayout *grid = new QGridLayout(page2_2,  NumOfFiles+2,3,  20, 5);
  
  for (int i=0; i<NumOfFiles; i++)
    {
      cbFileName1999[i] = new QCheckBox(LabelCB[i], page2_2);
      cbFileName1999[i] -> setChecked(TRUE);
      cbFileName1999[i] -> setMinimumSize(cbFileName1999[i]->sizeHint());
      cbFileName1999[i] -> setFocusPolicy(QWidget::ClickFocus);
      grid -> addWidget(cbFileName1999[i], i+1, 0);
      
      sFileName1999[i] = SetUp->path2ImpICRF() + "icrf-Ext.1/" + ImportFile[i];
      
      leFileName1999[i] = new QLineEdit(page2_2);
      leFileName1999[i] -> setText(sFileName1999[i]);
      leFileName1999[i] -> setMinimumHeight(leFileName1999[i]->sizeHint().height());
      leFileName1999[i] -> setMinimumWidth (leFileName1999[i]->fontMetrics().width(sFileName1999[i]) + 50);
      grid -> addWidget(leFileName1999[i], i+1, 1);
      
      bFile1999[i] = new QPushButton("Select file..", page2_2);
      bFile1999[i] -> setMinimumSize(bFile1999[i]->sizeHint());
      bFile1999[i] -> setFocusPolicy(QWidget::ClickFocus);
      grid -> addWidget(bFile1999[i], i+1, 2);

      IsNextEnabled = IsNextEnabled && checkFileExists(sFileName1999[i]);
    };
  
  connect(cbFileName1999[0],	SIGNAL(toggled(bool)),	SLOT(setRSCDstate1999(bool)));
  connect(cbFileName1999[1],	SIGNAL(toggled(bool)),	SLOT(setRSCCstate1999(bool)));
  connect(cbFileName1999[2],	SIGNAL(toggled(bool)),	SLOT(setRSCOstate1999(bool)));
  connect(cbFileName1999[3],	SIGNAL(toggled(bool)),	SLOT(setRSCNstate1999(bool)));
  connect(cbFileName1999[4],	SIGNAL(toggled(bool)),	SLOT(setCARstate1999 (bool)));
  connect(cbFileName1999[5],	SIGNAL(toggled(bool)),	SLOT(setDICOstate1999(bool)));

  connect(bFile1999[0],		SIGNAL(clicked()),	SLOT(selectRSCDfile1999()));
  connect(bFile1999[1],		SIGNAL(clicked()),	SLOT(selectRSCCfile1999()));
  connect(bFile1999[2],		SIGNAL(clicked()),	SLOT(selectRSCOfile1999()));
  connect(bFile1999[3],		SIGNAL(clicked()),	SLOT(selectRSCNfile1999()));
  connect(bFile1999[4],		SIGNAL(clicked()),	SLOT(selectCARfile1999 ()));
  connect(bFile1999[5],		SIGNAL(clicked()),	SLOT(selectDICOfile1999()));

  connect(leFileName1999[0],	SIGNAL(textChanged(const QString&)),SLOT(fnRSCDChanged1999(const QString&)));
  connect(leFileName1999[1],	SIGNAL(textChanged(const QString&)),SLOT(fnRSCCChanged1999(const QString&)));
  connect(leFileName1999[2],	SIGNAL(textChanged(const QString&)),SLOT(fnRSCOChanged1999(const QString&)));
  connect(leFileName1999[3],	SIGNAL(textChanged(const QString&)),SLOT(fnRSCNChanged1999(const QString&)));
  connect(leFileName1999[4],	SIGNAL(textChanged(const QString&)),SLOT(fnCARChanged1999 (const QString&)));
  connect(leFileName1999[5],	SIGNAL(textChanged(const QString&)),SLOT(fnDICOChanged1999(const QString&)));

  grid -> setColStretch(1,5);
  grid -> setRowStretch(0, 1);
  grid -> setRowStretch(NumOfFiles+1, 1);
  grid -> activate();

  addPage(page2_2, "Files to Load");
  setNextEnabled(page2_2, IsNextEnabled);
};

void SBSourceImport::setupPage2_3()
{
  bool		IsNextEnabled=TRUE;
  static int	NumOfFiles = 4;
  static const	QString LabelCB[]=
  {
    "RSC (non-VCS sources) file:",
    "RSC (VCS sources) file:",
    "CAR file:",
    "DICO file:"
  };
  static const QString ImportFile[] = 
  {
    "icrf2-non-vcs.dat",
    "icrf2-vcs-only.dat",
    "icrf-Ext1.car_phys",
    "icrf.dico"
  };

  page2_3 = new QWidget(this);
  QGridLayout *grid = new QGridLayout(page2_3,  NumOfFiles+2,3,  20, 5);
  
  for (int i=0; i<NumOfFiles; i++)
    {
      cbFileName2009[i] = new QCheckBox(LabelCB[i], page2_3);
      cbFileName2009[i] -> setChecked(TRUE);
      cbFileName2009[i] -> setMinimumSize(cbFileName2009[i]->sizeHint());
      cbFileName2009[i] -> setFocusPolicy(QWidget::ClickFocus);
      grid -> addWidget(cbFileName2009[i], i+1, 0);
      
      sFileName2009[i] = SetUp->path2ImpICRF() + "ICRF2/" + ImportFile[i];
      
      leFileName2009[i] = new QLineEdit(page2_3);
      leFileName2009[i] -> setText(sFileName2009[i]);
      leFileName2009[i] -> setMinimumHeight(leFileName2009[i]->sizeHint().height());
      leFileName2009[i] -> setMinimumWidth (leFileName2009[i]->fontMetrics().width(sFileName2009[i]) + 50);
      grid -> addWidget(leFileName2009[i], i+1, 1);
      
      bFile2009[i] = new QPushButton("Select file..", page2_3);
      bFile2009[i] -> setMinimumSize(bFile2009[i]->sizeHint());
      bFile2009[i] -> setFocusPolicy(QWidget::ClickFocus);
      grid -> addWidget(bFile2009[i], i+1, 2);

      IsNextEnabled = IsNextEnabled && checkFileExists(sFileName2009[i]);
    };
  
  connect(cbFileName2009[0],	SIGNAL(toggled(bool)),	SLOT(setRSCNonVCSState2009(bool)));
  connect(cbFileName2009[1],	SIGNAL(toggled(bool)),	SLOT(setRSCVCSState2009(bool)));
  connect(cbFileName2009[2],	SIGNAL(toggled(bool)),	SLOT(setCARState2009(bool)));
  connect(cbFileName2009[3],	SIGNAL(toggled(bool)),	SLOT(setDICOState2009(bool)));

  connect(bFile2009[0],		SIGNAL(clicked()),	SLOT(selectRSCNonVCSFile2009()));
  connect(bFile2009[1],		SIGNAL(clicked()),	SLOT(selectRSCVCSFile2009()));
  connect(bFile2009[2],		SIGNAL(clicked()),	SLOT(selectCARFile2009()));
  connect(bFile2009[3],		SIGNAL(clicked()),	SLOT(selectDICOFile2009()));

  connect(leFileName2009[0],	SIGNAL(textChanged(const QString&)),SLOT(fnRSCNonVCSChanged2009(const QString&)));
  connect(leFileName2009[1],	SIGNAL(textChanged(const QString&)),SLOT(fnRSCVCSChanged2009(const QString&)));
  connect(leFileName2009[2],	SIGNAL(textChanged(const QString&)),SLOT(fnCARChanged2009(const QString&)));
  connect(leFileName2009[3],	SIGNAL(textChanged(const QString&)),SLOT(fnDICOChanged2009(const QString&)));

  grid -> setColStretch(1,5);
  grid -> setRowStretch(0, 1);
  grid -> setRowStretch(NumOfFiles+1, 1);
  grid -> activate();

  addPage(page2_3, "Files to Load");
  setNextEnabled(page2_3, IsNextEnabled);
};

void SBSourceImport::setupPage3()
{
  static const QString sMode[]=
  {
    "Replace current database with imported one",
    "Import only new sources (which are not exist in current database)",
    "Insert new sources and update the present ones",
    "Just update present sources, ignore other records",
    "Just update coordinates of present sources, ignore other info",
  };

  //  page3	= new QButtonGroup("Import Options", this);
  page3	= new QWidget(this);
  QBoxLayout	*Layout=new QVBoxLayout(page3, 20,10);
  QRadioButton	*rbMode[5];

  QButtonGroup *gbox = new QVButtonGroup("Import mode", page3);
  for (int i=0; i<5; i++)
    {
      rbMode[i] = new QRadioButton(sMode[i], gbox);
      rbMode[i] -> setMinimumSize(rbMode[i]->sizeHint());
    };
  rbMode[Mode] -> setChecked(TRUE);
  connect (gbox, SIGNAL(clicked(int)), this, SLOT(modeChanged(int)));

  Layout->addStretch(1);
  Layout->addWidget(gbox);
  Layout->addStretch(1);

  Layout -> activate();
  
  addPage(page3, "Import Options");
  setFinishEnabled(page3, TRUE);
};

void SBSourceImport::selectFile1995(FileType Type)
{
  static const QString filter[] = {"*.rsc", "*.car", "*.dico"};
  QString str = QFileDialog::getOpenFileName(SetUp->path2ImpICRF() + "/icrf_rsc", filter[Type], this);
  if (!str.isNull()) 
    {
      sFileName1995 [Type] = str.copy();
      leFileName1995[Type]->setText(sFileName1995[Type]);
      checkPage2_1State();
    };
};

void SBSourceImport::setState1995(FileType Type, bool T)
{
  leFileName1995[Type]->setEnabled(T);
  bFile1995     [Type]->setEnabled(T);
  checkPage2_1State();
};

void SBSourceImport::fileNameChanged1995(FileType Type, const QString& /*FName*/)
{
  sFileName1995[Type] = leFileName1995[Type]->text();
  checkPage2_1State();
};

void SBSourceImport::selectFile1999(int Type)
{
  static const QString filter[] = {"*.def *.rsc", "*.can *.rsc", "*.oth *.rsc", "*.new *.rsc", "*.car_phys", "*.dico"};
  QString str = QFileDialog::getOpenFileName(SetUp->path2ImpICRF() + "icrf-Ext.1", filter[Type], this);
  if (!str.isNull()) 
    {
      sFileName1999 [Type] = str.copy();
      leFileName1999[Type]->setText(sFileName1999[Type]);
      checkPage2_2State();
    };
};

void SBSourceImport::setState1999(int Type, bool T)
{
  leFileName1999[Type]->setEnabled(T);
  bFile1999     [Type]->setEnabled(T);
  checkPage2_2State();
};

void SBSourceImport::fileNameChanged1999(int Type, const QString& /*FName*/)
{
  sFileName1999[Type] = leFileName1999[Type]->text();
  checkPage2_2State();
};

// ICRF2:
void SBSourceImport::selectFile2009(int Type)
{
  static const QString filter[] = {"*.dat *.rsc", "*.dat *.rsc", "*.car_phys", "*.dico"};
  QString str = QFileDialog::getOpenFileName(SetUp->path2ImpICRF() + "ICRF2", filter[Type], this);
  if (!str.isNull()) 
    {
      sFileName2009 [Type] = str.copy();
      leFileName2009[Type]->setText(sFileName2009[Type]);
      checkPage2_3State();
    };
};

void SBSourceImport::setState2009(int Type, bool T)
{
  leFileName2009[Type]->setEnabled(T);
  bFile2009     [Type]->setEnabled(T);
  checkPage2_3State();
};

void SBSourceImport::fileNameChanged2009(int Type, const QString& /*FName*/)
{
  sFileName2009[Type] = leFileName2009[Type]->text();
  checkPage2_3State();
};
//

void SBSourceImport::checkPage2_1State()
{
  bool IsNextEnabled = TRUE;
  for (int i=0; i<3; i++)
    if (cbFileName1995[i]->isChecked())
      IsNextEnabled = IsNextEnabled && checkFileExists(sFileName1995[i]);
  setNextEnabled(page2_1, IsNextEnabled);
};

void SBSourceImport::checkPage2_2State()
{
  bool IsNextEnabled = TRUE;
  for (int i=0; i<6; i++)
    if (cbFileName1999[i]->isChecked())
      IsNextEnabled = IsNextEnabled && checkFileExists(sFileName1999[i]);
  setNextEnabled(page2_2, IsNextEnabled);
};

void SBSourceImport::checkPage2_3State()
{
  bool IsNextEnabled = TRUE;
  for (int i=0; i<4; i++)
    if (cbFileName2009[i]->isChecked())
      IsNextEnabled = IsNextEnabled && checkFileExists(sFileName2009[i]);
  setNextEnabled(page2_3, IsNextEnabled);
};

void SBSourceImport::accept()
{
  QWizard::accept();
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": accept() selected");

  // importing
  switch (FFormat)
    {
    case F_ICRF95:
      importICRF();
      break;
    case F_ICRF99:
      importICRF();
      break;
    case F_ICRF2009:
      importICRF();
      break;
    default:
      Log->write(SBLog::WRN, SBLog::INTERFACE, ClassName() + ": Unknown type format for ICRF importing");
      break;
    };
  delete this;
};

void SBSourceImport::importICRF()
{
  int		i;
  SB_CRF*	W = new SB_CRF;
  SB_CRF*	OldCRF = new SB_CRF;
  QFileInfo	finfo;
  bool		IsNeedSave = FALSE;
  bool		IsRead = FALSE;
  QString	sFiles = "";
  //  QString	Str;

  static const QString sMode[]= {"REPLACE", "APP_ONLY", "APP_UPD", "UPD_ONLY", "UPD_COO_ONLY"};

  if (FFormat == F_ICRF95)
    {
      for (i=0; i<3; i++)
	{
	  if (cbFileName1995[i]->isChecked())
	    {
	      sFileName1995[i] = leFileName1995[i]->text();
	      finfo.setFile((const char*)sFileName1995[i]);
	      if (finfo.exists() && finfo.isReadable() && !finfo.isDir())
		{
		  loadFile1995(W, sFileName1995[i], (FileType)i);
		  sFiles += " " + finfo.baseName() + "." + finfo.extension();
		  IsRead = TRUE; // hope, we've got something
		}
	      else 
		Log->write(SBLog::WRN, SBLog::INTERFACE, 
			   ClassName() + ": could not acess `" + sFileName1995[i] + "'; ignored");
	    };
	};
    }
  else if (FFormat == F_ICRF99)
    {
      for (i=0; i<6; i++)
	{
	  if (cbFileName1999[i]->isChecked())
	    {
	      sFileName1999[i] = leFileName1999[i]->text();
	      finfo.setFile((const char*)sFileName1999[i]);
	      if (finfo.exists() && finfo.isReadable() && !finfo.isDir())
		{
		  SBSource::SStatus Status = SBSource::STATUS_UNKN;
		  FileType	    t	   = T_RSC;
		  if (i==0)
		    Status = SBSource::STATUS_DEF;
		  else if (i==1)
		    Status = SBSource::STATUS_CAND;
		  else if (i==2)
		    Status = SBSource::STATUS_OTHER;
		  else if (i==3)
		    Status = SBSource::STATUS_NEW;
		  else 
		    t = (FileType)(i-3);

		  loadFile1995(W, sFileName1999[i], t, Status);
		  sFiles += " " + finfo.baseName() + "." + finfo.extension();
		  IsRead = TRUE; // hope, we've got something
		}
	      else 
		Log->write(SBLog::WRN, SBLog::INTERFACE, 
			   ClassName() + ": could not acess `" + sFileName1999[i] + "'; ignored");
	    };
	};
    }
  else if (FFormat == F_ICRF2009)
    {
      for (i=0; i<4; i++)
	{
	  if (cbFileName2009[i]->isChecked())
	    {
	      sFileName2009[i] = leFileName2009[i]->text();
	      finfo.setFile((const char*)sFileName2009[i]);
	      if (finfo.exists() && finfo.isReadable() && !finfo.isDir())
		{
		  FileType    t	   = T_RSC;
		  if (i>1)
		    t = (FileType)(i-1);
		  
		  loadFile1995(W, sFileName2009[i], t);
		  sFiles += " " + finfo.baseName() + "." + finfo.extension();
		  IsRead = TRUE; // hope, we've got something
		}
	      else 
		Log->write(SBLog::WRN, SBLog::INTERFACE, 
			   ClassName() + ": could not acess `" + sFileName2009[i] + "'; ignored");
	    };
	};
    };

  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": Import mode: " + sMode[Mode]);

  // applying a filter
  if (IsRead)
    {
      SetUp->loadCRF(OldCRF);
      
      SBSource *NewS;
      SBSource *OldS;
      switch (Mode)
	{
	case REPLACE:
	  IsNeedSave = IsRead;
	  break;
	case APP_ONLY:
	  if (IsRead)
	    {
	      // sources:
	      for (NewS=W->first(); NewS; NewS=W->next())
		if (!OldCRF->find(*NewS)) 
		  {
		    OldCRF->inSort(new SBSource(*NewS));
		    IsNeedSave = TRUE;
		  };
	      
	      // aliases:
	      QDictIterator<QString> it(*W->aliasDict());
	      while (it.current())
		{
		  if (!OldCRF->aliasDict()->find(it.currentKey()))
		    {
		      OldCRF->aliasDict()->insert(it.currentKey(), new QString(it.current()->data()));
		      IsNeedSave = TRUE;
		    };
		  ++it;
		};
	      delete W;
	      W = OldCRF;
	      OldCRF = NULL;
	    };
	  break;
	case APP_UPD:
	  if (IsRead)
	    {
	      // sources:
	      if (W->count()) IsNeedSave = TRUE;
	      for (NewS=W->first(); NewS; NewS=W->next())
		if (!(OldS=OldCRF->find(*NewS))) 
		  OldCRF->inSort(new SBSource(*NewS));
		else
		  OldS->operator=(*NewS);
	      
	      // aliases:
	      if (W->aliasDict()->count()) IsNeedSave = TRUE;
	      QDictIterator<QString> it(*W->aliasDict());
	      while (it.current())
		{
		  if (!OldCRF->aliasDict()->find(it.currentKey()))
		    OldCRF->aliasDict()->insert(it.currentKey(), new QString(it.current()->data()));
		  else
		    OldCRF->aliasDict()->replace(it.currentKey(), new QString(it.current()->data()));
		  ++it;
		};
	      delete W;
	      W = OldCRF;
	      OldCRF = NULL;
	    };
	  break;
	case UPD_ONLY:
	  if (IsRead)
	    {
	      // sources:
	      for (OldS=OldCRF->first(); OldS; OldS=OldCRF->next())
		if ((NewS=W->find(*OldS)))
		  {
		    *OldS = *NewS;
		    IsNeedSave = TRUE;
		  };
	      
	      // aliases:
	      QDictIterator<QString> it(*OldCRF->aliasDict());
	      while (it.current())
		{
		  QString *StrPtr;
		  if ((StrPtr=W->aliasDict()->find(it.currentKey())))
		    {
		      OldCRF->aliasDict()->replace(it.currentKey(), new QString(StrPtr->data()));
		      IsNeedSave = TRUE;
		    };
		  ++it;
		};
	      delete W;
	      W = OldCRF;
	      OldCRF = NULL;
	    };
	  break;
	case UPD_COO_ONLY:
	  if (IsRead)
	    {
	      // sources:
	      for (OldS=OldCRF->first(); OldS; OldS=OldCRF->next())
		if ((NewS=W->find(*OldS)))
		  {
		    OldS->setRA(NewS->ra());
		    OldS->setDN(NewS->dn());
		    OldS->setRA_err(NewS->ra_err());
		    OldS->setDN_err(NewS->dn_err());

		    IsNeedSave = TRUE;
		  };
	      
	      // aliases:
	      QDictIterator<QString> it(*OldCRF->aliasDict());
	      while (it.current())
		{
		  QString *StrPtr;
		  if ((StrPtr=W->aliasDict()->find(it.currentKey())))
		    {
		      OldCRF->aliasDict()->replace(it.currentKey(), new QString(StrPtr->data()));
		      IsNeedSave = TRUE;
		    };
		  ++it;
		};
	      delete W;
	      W = OldCRF;
	      OldCRF = NULL;
	    };
	  break;
	};
    };
  
  if (IsNeedSave)
    {      
      W -> setComment(W->comment() + "; Imported from:" + sFiles);
      SetUp->saveCRF(W);
      emit sourcesImported();
    };
  
  if (W) delete W;
  if (OldCRF) delete OldCRF;
};

void SBSourceImport::loadFile1995(SB_CRF* W, QString FileName, FileType Type, SBSource::SStatus Status)
{
  QTextStream	*s;
  QFile	f(FileName);
  if (f.open(IO_ReadOnly))
    {
      switch (Type)
	{
	case T_RSC:
	  s = new SBTS_rsc;
	  ((SBTS_rsc*)s)->setAssignedStatus(Status);
	  s->setDevice(&f);
	  *(SBTS_rsc*)s >> *W;
	  break;
	case T_CAR:
	  s = new SBTS_car;
	  s->setDevice(&f);
	  *(SBTS_car*)s >> *W;
	  break;
	case T_DICO:
	  s = new SBTS_dico;
	  s->setDevice(&f);
	  *(SBTS_dico*)s >> *W;
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

void SBSourceImport::loadFile1999(SB_CRF* /*W*/, QString /*FileName*/, FileType /*Type*/)
{
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBSourceEditor..										*/
/*												*/
/*==============================================================================================*/
SBSourceEditor::SBSourceEditor(SBSourceListItem* SrcItem_, QWidget* parent,
			       const char* name, bool modal, WFlags f)
  : SBTabDialog(parent, name, modal, f)
{
  SrcItem	= SrcItem_;
  SourcesLV	= SrcItem->listView();
  Source	= SrcItem->Source;
  Wcrf		= NULL;
  
  isNew		= FALSE;
  init();
};

SBSourceEditor::SBSourceEditor(QListView* SourcesLV_, SB_CRF* Wcrf_, QWidget* parent,
			       const char* name, bool modal, WFlags f)
  : SBTabDialog(parent, name, modal, f)
{
  SrcItem	= NULL;
  SourcesLV	= SourcesLV_;
  Source	= new SBSource;
  Wcrf		= Wcrf_;

  isNew		= TRUE;
  init();
};

void SBSourceEditor::init()
{
  isModified	= FALSE;
  setCaption("Source Editor");

  addTab(wCoordinates(), "Coordinates");
  addTab(wPhysChars(), "Character");
  addTab(wStats(), "Status");

  setCancelButton();
  setApplyButton();
  connect(this, SIGNAL(applyButtonPressed()), SLOT(makeApply()));
};

SBSourceEditor::~SBSourceEditor()
{
  emit SourceModified(isModified);
  if (isNew && !isModified && Source) delete Source;
};
 
void SBSourceEditor::makeApply()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": applying");
  acquireData();
  if (isModified) 
    {
      Source->addAttr(SBSource::OrigEdited);
      if (isNew) 
	{
	  if (Wcrf->find(*Source))
	    {
	      QMessageBox::warning(this, "Source already defined", 
				   "The source \"" + Source->name() + "\" already present "
				   "in database.\n\n"
				   "It's impossible to add an object with the same ID,\n"
				   "but you can adit that entry (make a double click or\n"
				   "press Enter when a keyboard focus in a list)\n", 
				   "OK");
	      return;
	    };
	  
	  
	  Wcrf->inSort(Source);
	  SrcItem = new SBSourceListItem(SourcesLV, Source);
	  SourcesLV->setSelected(SrcItem, TRUE);
	  // not works... ;-(
	  // SourcesLV->ensureItemVisible(SrcItem);
	  QListViewItem *item = SrcItem;
	  QListViewItem *firstItem = SourcesLV->firstChild();
	  int h=0;
	  do 
	    {
	      h += item->totalHeight();
	      item=item->itemAbove();
	    } 
	  while (item && item!=firstItem);
	  SourcesLV->ensureVisible(0, h + SrcItem->totalHeight()/2);
	  isNew = FALSE;
	  EName->setEnabled(FALSE);
	  NameGBox->setTitle(" \"" + Source->name() + "\" Coordinates");
	}
      else SrcItem->repaint();
    };
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": done");
};

QWidget* SBSourceEditor::wCoordinates()
{
  QWidget*	W = new QWidget(this);

  int		hr;
  int		min;
  double	sec;
  QString	Str;
  QBoxLayout*	Layout;
  QBoxLayout*	aLayout;  
  QBoxLayout*	bLayout;
  QGridLayout*	grid;
  QLabel*	label;
  QGroupBox*	gbox;

  static const char* cStatus[5] = 
  {
    "Defining",
    "Candidate",
    "Other",
    "New",
    "Unknown"
  };

  if (!isNew) Str = " \"" + Source->name() + "\" Coordinates";
  else Str = "New Source Coordinates";

  NameGBox = gbox = new QGroupBox((const char*)Str, W);
  Layout = new QVBoxLayout(gbox, 20, 10);

  aLayout = new QHBoxLayout(20);
  Layout -> addStretch(1);
  Layout -> addLayout(aLayout);


  // Names & akas:
  grid    = new QGridLayout(5, 2, 5);
  aLayout-> addLayout(grid, 1);

  label   = new QLabel("Name: ", gbox);
  label	 -> setMinimumSize(label->sizeHint());
  grid	 -> addWidget(label, 1,0);

  EName    = new QLineEdit(gbox);
  EName   -> setText(Source->name());
  EName   -> setMinimumWidth (EName->fontMetrics().width(Source->name()) + 2);
  EName   -> setMinimumHeight(EName->sizeHint().height());
  grid    -> addWidget(EName, 1,1);
  EName	  -> setEnabled(isNew);


  label    = new QLabel("ICRF: ", gbox);
  label	  -> setMinimumSize(label->sizeHint());
  grid    -> addWidget(label, 2,0);

  EICRF	   = new QLineEdit(gbox);
  EICRF	  -> setText(Source->ICRFName());
  EICRF   -> setMinimumWidth (EICRF->fontMetrics().width(Source->ICRFName()) + 2);
  EICRF   -> setMinimumHeight(EICRF->sizeHint().height());
  grid    -> addWidget(EICRF, 2,1);

  label	   = new QLabel("Aliases: ", gbox);
  label	  -> setMinimumSize(label->sizeHint());
  grid    -> addWidget(label, 3,0);

  CBAlias  = new QComboBox(FALSE, gbox);
  CBAlias -> setInsertionPolicy(QComboBox::AtBottom);
  CBAlias -> setFocusPolicy(QWidget::StrongFocus);
  QList<QString>* aliases=Source->aliasesList();
  if (aliases && aliases->count())
    {
      QListIterator<QString> it(*aliases);
      while (it.current())
	{
	  CBAlias -> insertItem(*it.current());
	  ++it;
	};
    }
  else
    CBAlias -> insertItem("=NO ALIASES=");
  
  CBAlias -> setMinimumSize(CBAlias->sizeHint());
  grid    -> addWidget(CBAlias, 3,1);
  grid    -> setRowStretch(0,1);
  grid    -> setRowStretch(4,1);
  //--

  // Status:
  QButtonGroup* BGStatus = new QButtonGroup(" Category ", gbox);
  bLayout = new QVBoxLayout(BGStatus, 20, 5);
  for (int i=0; i<5; i++)
    {
      RBStatus[i] = new QRadioButton(cStatus[i], BGStatus);
      RBStatus[i]-> setMinimumSize(RBStatus[i]->sizeHint());
      bLayout -> addWidget(RBStatus[i]);
    };
  RBStatus[Source->status()]->setChecked(TRUE);
  bLayout -> activate();  
  //--
  
  aLayout-> addWidget(BGStatus, 2);
  Layout-> addStretch(1);
  //--

  QGroupBox* coo = new QGroupBox("Coordinates", gbox);
  grid   = new QGridLayout(coo, 5,9, 20,5);

  //-- RA:
  rad2hms(Source->ra(), hr, min, sec);  
  label	 = new QLabel("Right Ascension: ", coo);
  label -> setMinimumSize(label->sizeHint());
  grid  -> addWidget(label, 0,1);

  Str.sprintf("%3d", hr);
  ERAhr  = new QLineEdit(coo);
  ERAhr -> setText(Str);
  ERAhr -> setMinimumHeight(ERAhr->sizeHint().height());
  ERAhr -> setMinimumWidth (ERAhr->fontMetrics().width("-00") + 8);
  grid  -> addWidget(ERAhr, 0,2);

  label	 = new QLabel("hr", coo);
  label -> setMinimumSize(label->sizeHint());
  grid  -> addWidget(label, 0,3);

  Str.sprintf("%3d", min);
  ERAmin = new QLineEdit(coo);
  ERAmin-> setText(Str);
  ERAmin-> setMinimumWidth (ERAmin->fontMetrics().width("-00") + 8);
  grid  -> addWidget(ERAmin, 0,4);

  label	 = new QLabel("min", coo);
  label -> setMinimumSize(label->sizeHint());
  grid  -> addWidget(label, 0,5);

  Str.sprintf("%9.6f", sec);
  ERAsec = new QLineEdit(coo);
  ERAsec-> setText(Str);
  ERAsec-> setMinimumWidth (ERAsec->fontMetrics().width("-00.123456") + 8);
  grid  -> addWidget(ERAsec, 0,6);

  label	 = new QLabel("sec", coo);
  label -> setMinimumSize(label->sizeHint());
  grid  -> addWidget(label, 0,7);

  label	 = new QLabel("uncertainties: ", coo);
  label	-> setMinimumSize(label->sizeHint());
  grid	-> addWidget(label, 1,1);

  rad2hms(Source->ra_err(), hr, min, sec);  
  Str.sprintf("%9.6f", sec + 60.0*min + 3600.0*hr);
  ERAunc = new QLineEdit(coo);
  ERAunc-> setText(Str);
  ERAunc-> setMinimumHeight(ERAunc->sizeHint().height());
  grid  -> addWidget(ERAunc, 1,6);

  label	 = new QLabel("sec", coo);
  label -> setMinimumSize(label->sizeHint());
  grid  -> addWidget(label, 1,7);


  //-- DN:
  rad2dms(Source->dn(), hr, min, sec);  
  label	 = new QLabel("Declination: ", coo);
  label	-> setMinimumSize(label->sizeHint());
  grid	-> addWidget(label, 2,1);

  Str.sprintf("%3d", hr);
  EDNdg  = new QLineEdit(coo);
  EDNdg -> setText(Str);
  EDNdg -> setMinimumHeight(EDNdg->sizeHint().height());
  grid  -> addWidget(EDNdg, 2,2);

  label	 = new QLabel("deg", coo);
  label -> setMinimumSize(label->sizeHint());
  grid  -> addWidget(label, 2,3);

  Str.sprintf("%3d", min);
  EDNmin = new QLineEdit(coo);
  EDNmin-> setText(Str);
  grid  -> addWidget(EDNmin, 2,4);

  label	 = new QLabel("min", coo);
  label -> setMinimumSize(label->sizeHint());
  grid  -> addWidget(label, 2,5);

  Str.sprintf("%8.5f", sec);
  EDNsec = new QLineEdit(coo);
  EDNsec-> setText(Str);
  grid  -> addWidget(EDNsec, 2,6);

  label	 = new QLabel("sec", coo);
  label -> setMinimumSize(label->sizeHint());
  grid  -> addWidget(label, 2,7);

  label	 = new QLabel("uncertainties: ", coo);
  label	-> setMinimumSize(label->sizeHint());
  grid	-> addWidget(label, 3,1);

  rad2dms(Source->dn_err(), hr, min, sec);  
  Str.sprintf("%8.5f", sec + 60.0*min + 3600.0*hr);
  EDNunc = new QLineEdit(coo);
  EDNunc-> setText(Str);
  EDNunc-> setMinimumHeight(EDNunc->sizeHint().height());
  grid  -> addWidget(EDNunc, 3,6);

  label	 = new QLabel("sec", coo);
  label -> setMinimumSize(label->sizeHint());
  grid  -> addWidget(label, 3,7);


  label	 = new QLabel("Correlation (RA,De): ", coo);
  label	-> setMinimumSize(label->sizeHint());
  grid	-> addWidget(label, 4,1);

  Str.sprintf("%8.3f", Source->corrRA_DN());
  ECorr  = new QLineEdit(coo);
  ECorr -> setText(Str);
  ECorr -> setMinimumHeight(ECorr->sizeHint().height());
  grid  -> addWidget(ECorr, 4,6);

  grid	-> setColStretch(8, 3);
  grid	-> setColStretch(0, 1);
  grid  -> activate();
  //-
  Layout-> addWidget(coo);

  Layout-> addStretch(1);

  CBHipp = new QCheckBox("serves to link the Hipparcos stellar reference frame to the ICRS", 
			 gbox);
  CBHipp-> setChecked(Source->isHLink());
  CBHipp-> setMinimumSize(CBHipp->sizeHint());
  Layout-> addWidget(CBHipp);
  Layout-> addStretch(1);
  //--


  Layout-> activate();


  Layout  = new QVBoxLayout(W, 10);
  Layout  -> addWidget(gbox);
  Layout  -> activate();
  
  return W;
};

QWidget* SBSourceEditor::wPhysChars()
{
  QWidget*	W = new QWidget(this);
  
  QBoxLayout*	Layout, *aLayout;
  QGridLayout*	grid;
  QLabel*	label;
  QGroupBox*	gbox;
  QButtonGroup* BGw;
  QString	Str;

  static const char* cType[6] =
  {
    "Quasar",
    "Galaxy",
    "BL Lac",
    "BL Lac cand.",
    "Other",
    "Unknown"
  };

  static const char* cSpClass[8] =
  {
    "Seyfert spectrum",
    "Seyfert 1 spectrum",
    "Seyfert 1-2 spectrum",
    "Seyfert 2 spectrum",
    "Seyfert 3 or liner",
    "High opt. polariz.",
    "Other",
    "Unknown"
  };


  gbox	  = new QGroupBox("Physical characteristics", W);

  Layout = new QHBoxLayout(gbox, 20, 20);

  BGw     = new QButtonGroup("Source type", gbox);
  aLayout = new QVBoxLayout(BGw, 20, 5);
  for (int i=0; i<6; i++)
    {
      RBType[i] = new QRadioButton(cType[i], BGw);
      RBType[i]-> setMinimumSize(RBType[i]->sizeHint());
      aLayout -> addWidget(RBType[i]);
    };
  RBType[Source->type()]->setChecked(TRUE);
  aLayout -> activate();

  aLayout = new QVBoxLayout(10);
  Layout -> addLayout(aLayout);
  aLayout-> addStretch(1);
  aLayout-> addWidget(BGw);
  aLayout-> addStretch(1);

  grid    = new QGridLayout(3, 2, 5);
  aLayout-> addLayout(grid);
  
  label   = new QLabel("Mv: ", gbox);
  label	 -> setMinimumSize(label->sizeHint());
  grid	 -> addWidget(label, 0,0);

  Str.sprintf("%6.2f", Source->mv());
  EMv     = new QLineEdit(gbox);
  EMv    -> setText(Str);
  EMv    -> setMinimumHeight(EMv->sizeHint().height());
  EMv    -> setMinimumWidth (EMv->fontMetrics().width(Str) + 8);
  grid   -> addWidget(EMv, 0,1);

  label   = new QLabel("Flux_1: ", gbox);
  label	 -> setMinimumSize(label->sizeHint());
  grid	 -> addWidget(label, 1,0);

  Str.sprintf("%7.3f", Source->flux1());
  EFlux1  = new QLineEdit(gbox);
  EFlux1 -> setText(Str);
  EFlux1 -> setMinimumHeight(EFlux1->sizeHint().height());
  EFlux1 -> setMinimumWidth (EFlux1->fontMetrics().width(Str) + 8);
  grid   -> addWidget(EFlux1, 1,1);

  label   = new QLabel("Flux_2: ", gbox);
  label	 -> setMinimumSize(label->sizeHint());
  grid	 -> addWidget(label, 2,0);

  Str.sprintf("%7.3f", Source->flux2());
  EFlux2  = new QLineEdit(gbox);
  EFlux2 -> setText(Str);
  EFlux2 -> setMinimumHeight(EFlux2->sizeHint().height());
  EFlux2 -> setMinimumWidth (EFlux2->fontMetrics().width(Str) + 8);
  grid   -> addWidget(EFlux2, 2,1);

  aLayout-> addStretch(1);

  //-
  BGw     = new QButtonGroup("Spectrum Class", gbox);
  aLayout = new QVBoxLayout(BGw, 20, 5);
  for (int i=0; i<8; i++)
    {
      RBSpClass[i] = new QRadioButton(cSpClass[i], BGw);
      RBSpClass[i]-> setMinimumSize(RBSpClass[i]->sizeHint());
      aLayout -> addWidget(RBSpClass[i]);
    };
  RBSpClass[Source->spectrum()]->setChecked(TRUE);
  aLayout -> activate();  

  aLayout = new QVBoxLayout(10);
  Layout -> addLayout(aLayout);
  aLayout-> addStretch(1);

  aLayout-> addWidget(BGw);
  aLayout-> addStretch(1);

  grid    = new QGridLayout(2, 2, 5);
  aLayout-> addLayout(grid);
  
  label   = new QLabel("Spectral index: ", gbox);
  label	 -> setMinimumSize(label->sizeHint());
  grid	 -> addWidget(label, 0,0);

  Str.sprintf("%6.2f", Source->spIdx());
  ESpIdx  = new QLineEdit(gbox);
  ESpIdx -> setText(Str);
  ESpIdx -> setMinimumHeight(ESpIdx->sizeHint().height());
  ESpIdx -> setMinimumWidth (ESpIdx->fontMetrics().width(Str) + 8);
  grid   -> addWidget(ESpIdx, 0,1);

  label   = new QLabel("Red shift: ", gbox);
  label	 -> setMinimumSize(label->sizeHint());
  grid	 -> addWidget(label, 1,0);

  Str.sprintf("%7.4f", Source->redShift());
  ERedShift= new QLineEdit(gbox);
  ERedShift-> setText(Str);
  ERedShift-> setMinimumHeight(ERedShift->sizeHint().height());
  ERedShift-> setMinimumWidth (ERedShift->fontMetrics().width(Str) + 8);
  grid   -> addWidget(ERedShift, 1,1);
  aLayout-> addStretch(1);

  Layout-> activate();
  //-

  Layout = new QVBoxLayout(W, 10);
  Layout-> addWidget(gbox);
  Layout-> activate();
  return W;
};

QWidget* SBSourceEditor::wStats()
{
  static const char* cOrig[4] =
  {
    "has been imported",
    "has been edited",
    "from data files",
    "has been estimated"
  };

  static const char* cAttr[14] =
  {
    "Suspected in instability of RSC",
    "Attribute #02",
    "Attribute #03",
    "Attribute #04",
    "Attribute #05",
    "Attribute #06",
    "Attribute #07",
    "Attribute #08",
    "Attribute #09",
    "Attribute #10",
    "Attribute #11",
    "Attribute #12",
    "Attribute #13",
    "Attribute #14"
  };

  QWidget*	W = new QWidget(this);
  
  QBoxLayout*	Layout;
  QBoxLayout*	aLayout;
  QBoxLayout*	bLayout;
  QButtonGroup* BGw;
  QGridLayout*	grid;
  QLabel*	label;
  QGroupBox*	gbox;
  QString	Str;

  gbox	  = new QGroupBox("Source status", W);

  Layout  = new QHBoxLayout(gbox, 20, 20);
  aLayout = new QVBoxLayout(20);
  Layout -> addLayout(aLayout);
  //  aLayout -> addStretch(1);

  //--
  QGroupBox* stat = new QGroupBox("Statistics", gbox);
  grid    = new QGridLayout(stat, 5,3, 20,5);

  label   = new QLabel("Num of Obs: ", stat);
  label	 -> setMinimumSize(label->sizeHint());
  grid	 -> addWidget(label, 0,0);

  Str.sprintf("%d", Source->nObsApr());
  ENofObs = new QLineEdit(stat);
  ENofObs-> setText(Str);
  ENofObs-> setMinimumHeight(ENofObs->sizeHint().height());
  grid   -> addWidget(ENofObs, 0,2);

  label    = new QLabel("Num of Sessions.: ", stat);
  label	  -> setMinimumSize(label->sizeHint());
  grid    -> addWidget(label, 1,0);

  Str.sprintf("%d", Source->nSessApr());
  ENofSes  = new QLineEdit(stat);
  ENofSes -> setText(Str);
  ENofSes -> setMinimumHeight(ENofSes->sizeHint().height());
  grid    -> addWidget(ENofSes, 1,2);

  label	   = new QLabel("First date of Obs: ", stat);
  label	  -> setMinimumSize(label->sizeHint());
  grid    -> addWidget(label, 2,0);

  EFMJD    = new QLineEdit(stat);
  EFMJD   -> setText(Source->firstMJD().toString(SBMJD::F_Short));
  EFMJD   -> setMinimumHeight(EFMJD->sizeHint().height());
  EFMJD   -> setMinimumWidth (EFMJD->fontMetrics().width("10 Feb, 1976; 18:00:00.000") + 8);
  grid    -> addWidget(EFMJD, 2,2);

  label	   = new QLabel("Mean date of Obs: ", stat);
  label	  -> setMinimumSize(label->sizeHint());
  grid    -> addWidget(label, 3,0);

  EMMJD    = new QLineEdit(stat);
  EMMJD   -> setText(Source->meanMJD().toString(SBMJD::F_Short));
  EMMJD   -> setMinimumHeight(EMMJD->sizeHint().height());
  grid    -> addWidget(EMMJD, 3,2);

  label	   = new QLabel("Last date of Obs: ", stat);
  label	  -> setMinimumSize(label->sizeHint());
  grid    -> addWidget(label, 4,0);

  ELMJD    = new QLineEdit(stat);
  ELMJD   -> setText(Source->lastMJD().toString(SBMJD::F_Short));
  ELMJD   -> setMinimumHeight(ELMJD->sizeHint().height());
  grid    -> addWidget(ELMJD, 4,2);
  grid    -> activate();

  aLayout-> addWidget(stat);

  BGw = new QButtonGroup("Origin", gbox);
  bLayout = new QVBoxLayout(BGw, 20, 5);
  for (int i=0; i<4; i++)
    {
      CDOrigin[i] = new QCheckBox(cOrig[i], BGw);
      CDOrigin[i]-> setMinimumSize(CDOrigin[i]->sizeHint());
      bLayout -> addWidget(CDOrigin[i]);
    };
  CDOrigin[0]->setChecked(Source->isAttr(SBSource::OrigImported));
  CDOrigin[1]->setChecked(Source->isAttr(SBSource::OrigEdited));
  CDOrigin[2]->setChecked(Source->isAttr(SBSource::OrigObsData));
  CDOrigin[3]->setChecked(Source->isAttr(SBSource::OrigEstimated));
  bLayout -> activate();  

  aLayout -> addStretch(1);
  aLayout -> addWidget(BGw);
  aLayout -> addStretch(1);

  aLayout = new QVBoxLayout(20);
  Layout -> addLayout(aLayout);
  //  aLayout -> addStretch(1);

  BGw = new QButtonGroup("Attributes", gbox);
  bLayout = new QVBoxLayout(BGw, 20, 5);
  for (int i=0; i<13; i++)
    {
      CDAttr[i] = new QCheckBox(cAttr[i], BGw);
      CDAttr[i]-> setMinimumSize(CDAttr[i]->sizeHint());
      bLayout -> addWidget(CDAttr[i]);
    };
  CDAttr[ 0]->setChecked(Source->isAttr(SBSource::StatusSuspected));
  CDAttr[ 1]->setChecked(Source->isAttr(SBSource::Analysed));
  CDAttr[ 2]->setChecked(Source->isAttr(SBSource::AnalysConstr));
  CDAttr[ 3]->setChecked(Source->isAttr(SBSource::Reserved_08));
  CDAttr[ 4]->setChecked(Source->isAttr(SBSource::Reserved_09));
  CDAttr[ 5]->setChecked(Source->isAttr(SBSource::Reserved_10));
  CDAttr[ 6]->setChecked(Source->isAttr(SBSource::Reserved_11));
  CDAttr[ 7]->setChecked(Source->isAttr(SBSource::Reserved_12));
  CDAttr[ 8]->setChecked(Source->isAttr(SBSource::Reserved_13));
  CDAttr[ 9]->setChecked(Source->isAttr(SBSource::Reserved_14));
  CDAttr[10]->setChecked(Source->isAttr(SBSource::Reserved_15));
  CDAttr[11]->setChecked(Source->isAttr(SBSource::Reserved_16));
  CDAttr[12]->setChecked(Source->isAttr(SBSource::Reserved_17));
  bLayout -> activate();  

  aLayout -> addWidget(BGw);
  aLayout -> addStretch(1);

  Layout  -> activate();

  //-
  Layout = new QVBoxLayout(W, 10);
  Layout-> addWidget(gbox);
  Layout-> activate();
  return W;
};

void SBSourceEditor::acquireData()
{
  int				hr;
  int				min;
  double			sec;
  double			d;
  float				f;
  bool				isOK;
  QString			Str;
  SBSource::SStatus		Status=SBSource::STATUS_UNKN;
  SBSource::SType		Type=SBSource::TYPE_UNKN;
  SBSource::SSpectrumClass	SpectrumClass=SBSource::SP_UNKN;
  SBMJD				date;
  

  if (isNew && ((Str=EName->text())!=Source->name()))
    {
      isModified = TRUE;
      Source->setName(Str.leftJustify(8, ' ', TRUE));
    };

  if (((Str=EICRF->text())!=Source->ICRFName()) && SBSource::isValidICRFName(Str))
    {
      isModified = TRUE;
      Source->setICRFName(Str);
    };
  
  Str = ERAhr->text();
  hr  = Str.toInt(&isOK);
  if (isOK)
    {
      Str = ERAmin->text();
      min = Str.toInt(&isOK);
      if (isOK)
	{
	  Str = ERAsec->text();
	  sec = Str.toDouble(&isOK);
	  if (isOK && ((d=hms2rad(hr, min, sec)) != Source->ra()))
	    {
	      isModified = TRUE;
	      Source->setRA(d);
	    };	  
	};
    };

  Str = EDNdg->text();
  hr  = Str.toInt(&isOK);
  if (isOK)
    {
      Str = EDNmin->text();
      min = Str.toInt(&isOK);
      if (isOK)
	{
	  Str = EDNsec->text();
	  sec = Str.toDouble(&isOK);
	  if (isOK && ((d=dms2rad(hr, min, sec)) != Source->dn()))
	    {
	      isModified = TRUE;
	      Source->setDN(d);
	    };
	};
    };

  Str = ERAunc->text();
  sec = Str.toDouble(&isOK);
  if (isOK && ((d=hms2rad(0, 0, sec)) != Source->ra_err()))
    {
      isModified = TRUE;
      Source->setRA_err(d);
    };

  Str = EDNunc->text();
  sec = Str.toDouble(&isOK);
  if (isOK && ((d=dms2rad(0, 0, sec)) != Source->dn_err()))
    {
      isModified = TRUE;
      Source->setDN_err(d);
    };

  Str = ECorr->text();
  d   = Str.toDouble(&isOK);
  if (isOK && (d != Source->corrRA_DN()))
    {
      isModified = TRUE;
      Source->setCorrRA_DN(d);
    };

  isOK = CBHipp->isChecked();
  if (isOK!=Source->isHLink())
    {
      isModified = TRUE;
      Source->setIsHLink(isOK);
    };

  if (RBStatus[0]->isChecked()) Status=SBSource::STATUS_DEF;
  if (RBStatus[1]->isChecked()) Status=SBSource::STATUS_CAND;
  if (RBStatus[2]->isChecked()) Status=SBSource::STATUS_OTHER;
  if (RBStatus[3]->isChecked()) Status=SBSource::STATUS_NEW;
  if (RBStatus[4]->isChecked()) Status=SBSource::STATUS_UNKN;
  if (Status!=Source->status())
    {
      isModified = TRUE;
      Source->setStatus(Status);
    };

  Str = ENofObs->text();
  min = Str.toInt(&isOK);
  if (isOK && (min != Source->nObsApr()))
    {
      isModified = TRUE;
      Source->setNObsApr(min);
    };

  Str = ENofSes->text();
  min = Str.toInt(&isOK);
  if (isOK && (min != Source->nSessApr()))
    {
      isModified = TRUE;
      Source->setNSessApr(min);
    };

  if (Source->firstMJD().toString(SBMJD::F_Short) != (Str=EFMJD->text()))
    {
      date.setMJD(SBMJD::F_Short, Str);
      if (date.isGood())
	{
	  isModified = TRUE;
	  Source->setFirstMJD(date);
	};
    };

  if (Source->meanMJD().toString(SBMJD::F_Short) != (Str=EMMJD->text()))
    {
      date.setMJD(SBMJD::F_Short, Str);
      if (date.isGood())
	{
	  isModified = TRUE;
	  Source->setMeanMJD(date);
	};
    };


  if (Source->lastMJD().toString(SBMJD::F_Short) != (Str=ELMJD->text()))
    {
      date.setMJD(SBMJD::F_Short, Str);
      if (date.isGood())
	{
	  isModified = TRUE;
	  Source->setLastMJD(date);
	};
    };

  if (RBType[0]->isChecked()) Type=SBSource::TYPE_QUASAR;
  if (RBType[1]->isChecked()) Type=SBSource::TYPE_GALAXY;
  if (RBType[2]->isChecked()) Type=SBSource::TYPE_BL_LAC;
  if (RBType[3]->isChecked()) Type=SBSource::TYPE_BL_LAC_C;
  if (RBType[4]->isChecked()) Type=SBSource::TYPE_OTHER;
  if (RBType[5]->isChecked()) Type=SBSource::TYPE_UNKN;
  if (Type!=Source->type())
    {
      isModified = TRUE;
      Source->setType(Type);
    };


  if (RBSpClass[0]->isChecked()) SpectrumClass=SBSource::SP_SEYFERT;
  if (RBSpClass[1]->isChecked()) SpectrumClass=SBSource::SP_SEYFERT1;
  if (RBSpClass[2]->isChecked()) SpectrumClass=SBSource::SP_SEYFERT1_2;
  if (RBSpClass[3]->isChecked()) SpectrumClass=SBSource::SP_SEYFERT2;
  if (RBSpClass[4]->isChecked()) SpectrumClass=SBSource::SP_SEYFERT3;
  if (RBSpClass[5]->isChecked()) SpectrumClass=SBSource::SP_HP;
  if (RBSpClass[6]->isChecked()) SpectrumClass=SBSource::SP_OTHER;
  if (RBSpClass[7]->isChecked()) SpectrumClass=SBSource::SP_UNKN;
  if (SpectrumClass!=Source->spectrum())
    {
      isModified = TRUE;
      Source->setSpectrum(SpectrumClass);
    };


  Str = EMv->text();
  f   = Str.toFloat(&isOK);
  if (isOK && (f != Source->mv()))
    {
      isModified = TRUE;
      Source->setMv(f);
    };

  Str = EFlux1->text();
  f   = Str.toFloat(&isOK);
  if (isOK && (f != Source->flux1()))
    {
      isModified = TRUE;
      Source->setFlux1(f);
    };

  Str = EFlux2->text();
  f   = Str.toFloat(&isOK);
  if (isOK && (f != Source->flux2()))
    {
      isModified = TRUE;
      Source->setFlux2(f);
    };

  Str = ESpIdx->text();
  f   = Str.toFloat(&isOK);
  if (isOK && (f != Source->spIdx()))
    {
      isModified = TRUE;
      Source->setSpIdx(f);
    };

  Str = ERedShift->text();
  f   = Str.toFloat(&isOK);
  if (isOK && (f != Source->redShift()))
    {
      isModified = TRUE;
      Source->setRedShift(f);
    };
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBAliasEditor..										*/
/*												*/
/*==============================================================================================*/
SBAliasEditor::SBAliasEditor(SBAliasListItem* AkaItem_, SBCatalog* Wcrf_,
			     QWidget* parent, const char* name, bool modal, WFlags f)
  : QDialog(parent, name, modal, f)
{
  AkaItem	= AkaItem_;
  AliasesLV	= AkaItem->listView();
  Alias		= AkaItem->Alias.copy();
  Reference	= AkaItem->Reference.copy();
  Wcrf		= Wcrf_;

  isNew		= FALSE;
  OldReference	= Reference.copy();
  init();
};

SBAliasEditor::SBAliasEditor(QListView* AliasesLV_, SBCatalog* Wcrf_,
			     QWidget* parent, const char* name, bool modal, WFlags f)
  : QDialog(parent, name, modal, f)
{
  AkaItem	= NULL;
  AliasesLV	= AliasesLV_;
  Alias		= "ALIAS-01";
  Reference	= "_NONAME_";
  Wcrf		= Wcrf_;

  isNew		= TRUE;
  OldReference	= "";
  init();
};

void SBAliasEditor::init()
{
  static const char* description =
    "\nSelect source name corresponding to the alias.\n"
    "This text should be modified some time..\n"
    "and so on...\n";
  
  setCaption("Alias Editor");
  
  QPushButton	*button;
  QLabel	*label;
  QBoxLayout	*Layout;
  QBoxLayout	*SubLayout;
  
  isModified	= FALSE;

  QGroupBox	*gbox = new QGroupBox("Aliases", this);
  Layout = new QVBoxLayout(gbox, 20, 20);
  Layout -> addStretch(1);
  
  SubLayout = new QHBoxLayout(20);
  Layout -> addLayout(SubLayout);
  SubLayout -> addStretch(1);

  label	= new QLabel("alias: ", gbox);
  label	-> setMinimumSize(label->sizeHint());
  SubLayout -> addWidget(label);

  if (isNew)
    {
      EAlias  = new QLineEdit(gbox);
      EAlias -> setText(Alias);
      EAlias -> setMinimumHeight(EAlias->sizeHint().height());
      EAlias -> setMinimumWidth (EAlias->fontMetrics().width("WWWWWWWW") + 8);
      SubLayout -> addWidget(EAlias);
    }
  else
    {
      EAlias	= NULL;
      label	= new QLabel(Alias, gbox);
      label	-> setMinimumSize(label->sizeHint());
      SubLayout -> addWidget(label);
    };

  label	= new QLabel(" stands for: ", gbox);
  label	-> setMinimumSize(label->sizeHint());
  SubLayout -> addWidget(label);


  CBReference  = new QComboBox(FALSE, gbox);
  CBReference -> setInsertionPolicy(QComboBox::AtBottom);

  Wcrf->prepareReferences();
  int i=0;
  if (Wcrf->count() && Wcrf->referenceList()->count())
    {
      SBNIterator it(*Wcrf->referenceList());
      while (it.current())
	{
	  CBReference -> insertItem(it.current()->name());
	  if (Reference==it.current()->name()) CBReference -> setCurrentItem(i);
	  ++it;
	  ++i;
	};
    }
  else
    CBReference -> insertItem("=Not available=");

  CBReference -> setMinimumSize(CBReference->sizeHint());
  SubLayout-> addWidget(CBReference);
  SubLayout -> addStretch(1);

  Layout -> addStretch(1);

  label	= new QLabel(gbox);
  label -> setFrameStyle(QFrame::Panel | QFrame::Raised);
  label -> setAlignment(AlignCenter);
  label -> setText(description);
  label	-> setMinimumSize(label->sizeHint());
  Layout-> addWidget(label);

  Layout -> addStretch(1);

  Layout -> activate();



  //-
  Layout = new QVBoxLayout(this, 10, 10);
  Layout -> addWidget(gbox, 10);


  // buttons:
  SubLayout = new QHBoxLayout(10);
  Layout -> addLayout(SubLayout);

  button = new QPushButton("OK", this);
  button->setMinimumSize(button->sizeHint());
  SubLayout-> addWidget(button);
  connect (button, SIGNAL(clicked()), SLOT(accept()));

  button = new QPushButton("Cancel", this);
  button->setMinimumSize(button->sizeHint());
  SubLayout-> addWidget(button);
  connect (button, SIGNAL(clicked()), SLOT(reject()));
  //--

  Layout -> activate(); 
};

SBAliasEditor::~SBAliasEditor()
{
  emit AliasModified(isModified);
};
 
void SBAliasEditor::accept()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": applying");
  acquireData();
  if (isModified) 
    {
      if (isNew) 
	{
	  if (Wcrf->aliasDict()->find((const char*)Alias))
	    {
	      QMessageBox::warning(this, "Alias already defined", 
				   "The alias \"" + Alias + "\" already defined "
				   "for source \"" + Reference + "\"\n\n"
				   "It's impossible to add an object with the same ID,\n"
				   "but you can adit that entry (make a double click or\n"
				   "press Enter when a keyboard focus in a list)\n",
				   "OK");
	      return;
	    };

	  Wcrf->aliasDict()->insert((const char*)Alias, new QString(Reference));
	  Wcrf->collectObjAliases(Reference);

	  AkaItem = new SBAliasListItem(AliasesLV, Alias, Reference);
	  AliasesLV->setSelected(AkaItem, TRUE);
	  // not works... ;-(
	  // AliasesLV->ensureItemVisible(AkaItem);
	  QListViewItem *item = AkaItem;
	  QListViewItem *firstItem = AliasesLV->firstChild();
	  int h=0;
	  do 
	    {
	      h += item->totalHeight();
	      item=item->itemAbove();
	    } 
	  while (item && item!=firstItem);
	  AliasesLV->ensureVisible(0, h + AkaItem->totalHeight()/2);
	  isNew = FALSE;
	}
      else
	{
	  Wcrf->aliasDict()->replace((const char*)Alias, new QString(Reference));
	  AkaItem->Reference= Reference.copy();
	  Wcrf->collectObjAliases(Reference);
	  if (Reference!=OldReference)
	    Wcrf->collectObjAliases(OldReference);
	  AkaItem->repaint();
	};
    };
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": done");

  QDialog::accept();
  delete this;
};

void SBAliasEditor::reject()
{
  QDialog::reject();
  delete this;
};

void SBAliasEditor::acquireData()
{
  QString	Str;
  if (isNew)
    {
      Str = EAlias->text();
      Str=Str.leftJustify(8, ' ', TRUE);
      if (Alias != Str)
	{
	  isModified = TRUE;
	  Alias = Str;
	};
    };

  if (Reference != (Str=CBReference->currentText()))
    {
      isModified = TRUE;
      Reference = Str;
    };
};
/*==============================================================================================*/
