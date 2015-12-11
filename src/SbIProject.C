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

#include "SbIProject.H"

#include <qapplication.h>
#include <qbuttongroup.h>
#include <qcheckbox.h>
//#include <qcombobox.h>
#include <qdir.h>
//#include <qfiledialog.h>
//#include <qframe.h>
#include <qgroupbox.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qlineedit.h>
//#include <qlistbox.h>
#include <qmessagebox.h>
#include <qmultilinedit.h> 
#include <qpushbutton.h>
#include <qradiobutton.h>
//#include <qstrlist.h>
//#include <qtooltip.h>

#include "SbSetup.H"
#include "SbGeoProject.H"
#include "SbIMainWin.H"




/*==============================================================================================*/
/*												*/
/* SBProjectCreate..										*/
/*												*/
/*==============================================================================================*/
SBProjectCreate::SBProjectCreate(SBProject *Project_, 
				 QWidget* parent, const char* name, bool modal, WFlags f)
  : SBVLBISetViewBase(parent, name, modal, f)
{
  Project = Project_;
  NumObsSel = 0;
  NumSessSel = 0;
  NumPPSessSel = 0;
  IsProcessing = FALSE;
  init();
};

void SBProjectCreate::init()
{
  setCaption("VLBI Dataset");
  
  QGroupBox  *gbox   = new QGroupBox("VLBI Sessions", this);
  QBoxLayout *Layout = new QVBoxLayout(gbox, 20, 10);

  QBoxLayout *SubLayout = new QHBoxLayout(10);
  Layout->addLayout(SubLayout);
  
  EPrjName = new QLineEdit(gbox);
  QLabel *label	= new QLabel(EPrjName, "&Project name:", gbox);
  label	-> setMinimumSize(label->sizeHint());
  EPrjName->setMinimumSize(EPrjName->sizeHint());

  if (Project)
    {
      EPrjName->setText(Project->name());
      EPrjName->setEnabled(FALSE);
    };

  SubLayout->addWidget(label);
  SubLayout->addWidget(EPrjName);
  SubLayout->addStretch(1);

  Layout->addWidget(VLBIList(gbox, BM_PROJECT, Project), 5);

  brwsSes->lvSessions()->setMultiSelection (TRUE);
  if (brwsSes->lvSessions()->firstChild())
    brwsSes->lvSessions()->setSelected(brwsSes->lvSessions()->firstChild(), FALSE);
  connect(brwsSes->lvSessions(), 
	  SIGNAL(selChanged4Item(QListViewItem*, bool)), SLOT(selChanged(QListViewItem*, bool)));
  
  SubLayout = new QHBoxLayout(10);
  Layout->addLayout(SubLayout);
  SubLayout->addWidget(unitsGroup(gbox));
  SubLayout->addWidget(optionsGroup(gbox));
  SubLayout->addWidget(infoGroup(gbox));
  Layout->activate();

  //---
  Layout = new QVBoxLayout(this, 10, 10);
  Layout -> addWidget(gbox, 10);
  SubLayout = new QHBoxLayout(10);
  Layout -> addLayout(SubLayout);
  SubLayout-> addStretch(1);

  QPushButton *Ok	= new QPushButton("OK", this);
  QPushButton *SelAll	= new QPushButton("Select All", this);
  QPushButton *UnsAll	= new QPushButton("Unselect All", this);
  QPushButton *Create	= new QPushButton(Project==NULL?"Create":"Add", this);
  QPushButton *Cancel	= new QPushButton("Cancel", this);
  Ok->setDefault(TRUE);

  QSize	BtnSize;
  UnsAll->setMinimumSize((BtnSize=UnsAll->sizeHint()));
  Ok->setMinimumSize(BtnSize);
  SelAll->setMinimumSize(BtnSize);
  Cancel->setMinimumSize(BtnSize);
  Create->setMinimumSize(BtnSize);

  SubLayout->addWidget(Ok);
  SubLayout->addWidget(SelAll);
  SubLayout->addWidget(UnsAll);
  SubLayout->addWidget(Create);
  SubLayout->addWidget(Cancel);

  connect(Ok, SIGNAL(clicked()), SLOT(accept()));
  connect(SelAll, SIGNAL(clicked()), SLOT(selectAll()));
  connect(UnsAll, SIGNAL(clicked()), SLOT(unselectAll()));
  connect(Create, SIGNAL(clicked()), SLOT(createProject()));
  connect(Cancel, SIGNAL(clicked()), SLOT(reject()));
  //--

  connect(this, SIGNAL(stBarReset()),			MainWin,SLOT(stBarReset()));
  connect(this, SIGNAL(stBarSetTotalSteps(int)),	MainWin,SLOT(stBarSetTotalSteps(int)));
  connect(this, SIGNAL(stBarSetProgress(int)),		MainWin,SLOT(stBarSetProgress(int)));
  connect(this, SIGNAL(message(const QString&)),	MainWin,SLOT(setMessage(const QString&)));

  Layout->activate();
};



QWidget* SBProjectCreate::optionsGroup(QWidget* parent)
{
  QButtonGroup *Options = new QButtonGroup("Options", parent);
  QBoxLayout *aLayout = new QVBoxLayout(Options, 20, 5);

  QCheckBox *CBOpt_1 = new QCheckBox("Option #1", Options);
  CBOpt_1->setMinimumSize(CBOpt_1->sizeHint());
  aLayout->addWidget(CBOpt_1);

  QCheckBox *CBOpt_2 = new QCheckBox("Option #2", Options);
  CBOpt_2->setMinimumSize(CBOpt_2->sizeHint());
  aLayout->addWidget(CBOpt_2);

  aLayout->activate();
  return Options;
};

void SBProjectCreate::drawInfo()
{
  LNumSes->setText(str.sprintf("%d/%d", Wvlbi->count(), NumSessSel));
  LNumPPS->setText(str.sprintf("%d/%d", brwsSes->numPPSess(), NumPPSessSel));
  LNumObs->setText(str.sprintf("%d/%d", brwsSes->numObs(), NumObsSel));
};

void SBProjectCreate::editEntry(QListViewItem* SesItem)
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": editEntry selected");
  SBVLBISessionEditor *SE = new SBVLBISessionEditor((SBVLBISesInfoLI*)SesItem, Wvlbi);
  connect (SE, SIGNAL(sessionModified(bool)), SLOT(modified(bool)));
  connect (SE, SIGNAL(windowClosed()), SLOT(childWindowClosed()));
  numOpenedWindows++;
  SE->show();
};

void SBProjectCreate::selChanged(QListViewItem* Item, bool sel)
{
  if (!Item) return;
  SBVLBISesInfo	*SI = ((SBVLBISesInfoLI*)Item)->sessionInfo();
  if (sel)
    {
      NumObsSel += SI->numObs();
      NumSessSel++;
      if (SI->isAttr(SBVLBISesInfo::preProc)) NumPPSessSel++;
    }
  else
    {
      NumObsSel -= SI->numObs();
      NumSessSel--;
      if (SI->isAttr(SBVLBISesInfo::preProc)) NumPPSessSel--;
    };
  drawInfo();
};

void SBProjectCreate::selectAll()
{
  for (QListViewItem *i=brwsSes->lvSessions()->firstChild(); i; i=i->itemBelow())
    brwsSes->lvSessions()->setSelected(i, TRUE);
};

void SBProjectCreate::unselectAll()
{
  for (QListViewItem *i=brwsSes->lvSessions()->firstChild(); i; i=i->itemBelow())
    brwsSes->lvSessions()->setSelected(i, FALSE);
};

void SBProjectCreate::createProject()
{
  if (IsProcessing) return;
  QString	PrjName;
  if ((PrjName=EPrjName->text()).stripWhiteSpace()=="")
    {
      QMessageBox::warning
	(0, "Empty project name",
	 "Sorry, cannot create a file with the name \"" + PrjName + "\".\n"
	 "Please, type a name of a project in the corresponded window.\n");
      return;
    };
  IsProcessing = TRUE;
  
  bool IsPrjOwner = (Project == NULL);

  if (IsPrjOwner)
    {
      Project = new SBProject(PrjName);
      SetUp->setProject(PrjName);
    };
  
  emit stBarReset();
  emit stBarSetTotalSteps(NumSessSel);
  int i=1;
  SBVLBISesInfo *info=NULL;
  for (QListViewItem *item=brwsSes->lvSessions()->firstChild(); item; item=item->itemBelow())
    if (item->isSelected() && (info=((SBVLBISesInfoLI*)item)->sessionInfo()))
      {
	qApp->processEvents();
	emit message(QString("Processing: ") + info->name());
	emit stBarSetProgress(i++);
	Project->addSession(Wvlbi, info);
	brwsSes->lvSessions()->setSelected(item, FALSE);
      };
  emit message("Saving");
  
  if (IsPrjOwner)
    {
      Project->addHistoryLine("Project has been created.");
      SetUp->saveProject(Project);
    }
  else
    {
      Project->addHistoryLine("List of sessions has been updated.");
    };
  
  emit stBarReset();
  emit message("Ready");
  IsProcessing = FALSE;
  
  if (IsPrjOwner)
    {
      // clear the solution (if exists):
      SBSolution *Solution = new SBSolution(Project, FALSE);
      Solution->deleteSolution();
      delete Solution;
      delete Project;
    }
  else 
    {
      brwsSes->updateList();
      emit sessionsAdded();
    };
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBProjectEdit..										*/
/*												*/
/*==============================================================================================*/
SBProjectEdit::SBProjectEdit(QWidget* parent, const char* name, bool modal, WFlags f)
  : SBTabDialog(parent, name, modal, f)
{
  if (!(Wvlbi=SetUp->loadVLBI())) Wvlbi = new SBVLBISet;
  Project = new SBProject("");
  SetUp->loadProject(Project);

  CRF = SetUp->loadCRF();

  isModified = FALSE;
  isCommentsChanged = FALSE;

  numOpenedWindows = 0;
  setCaption("Project: " + Project->name() + " (Version:" + Str.setNum(Project->version()) + ")");

  addTab(brwsSes = 
	 new SBBrowseSessions(Project->VLBIsessionList(),
			      BM_PROJECT, NULL, this), "VLBI Sessions");
  addTab(brwsSta = new SBBrowseStations(Project->stationList(),
					BM_PROJECT, Project->tStart(), this), "Stations");
  addTab(brwsBas = new SBBrowseBases(Project->baseList(), this), "Bases");
  addTab(brwsSou = new SBBrowseSources(Project->sourceList(), BM_PROJECT, CRF, this), "Sources");
  addTab(brwsHist = new SBBrowseHistory(NULL, Project->history(),
					Project->version(), this), "History");
  addTab(comments(), "Comments");
  setCancelButton();
  setApplyButton();

  setDefaultButton("Add session(s)");
  setHelpButton("Delete session(s)");


  connect(this, SIGNAL(applyButtonPressed()), SLOT(makeApply()));
  connect(this, SIGNAL(helpButtonPressed()), SLOT(delSession()));
  connect(this, SIGNAL(defaultButtonPressed()), SLOT(addSession()));

  connect(brwsHist, SIGNAL(modified(bool)), SLOT(histModified(bool)));
  connect(brwsHist, SIGNAL(versionInced()), SLOT(histVerInced()));
  connect(brwsSes->lvSessions(), 
	  SIGNAL(rightButtonClicked(QListViewItem*, const QPoint&, int)), 
	  SLOT(toggleEntryMarkEnable(QListViewItem*, const QPoint&, int)));
  connect(brwsSes->lvSessions(), 
	  SIGNAL(doubleClicked(QListViewItem*)), SLOT(editEntry(QListViewItem*)));
  connect(brwsSes->lvSessions(), 
	  SIGNAL(returnPressed(QListViewItem*)), SLOT(editEntry(QListViewItem*)));
  connect(brwsSta, SIGNAL(modified(bool)), SLOT(modified(bool)));
  connect(brwsBas, SIGNAL(modified(bool)), SLOT(modified(bool)));
  connect(brwsSou, SIGNAL(modified(bool)), SLOT(modified(bool)));
};

SBProjectEdit::~SBProjectEdit()
{
  if (isModified)
    SetUp->saveProject(Project);

  if (CRF) delete CRF;
  if (Wvlbi) delete Wvlbi;
  if (Project) delete Project;
};

void SBProjectEdit::done(int r)
{
  if (numOpenedWindows) return;
  QDialog::done(r);
  if (r==QDialog::Accepted)
    acquireData();
  delete this;
};

QWidget	*SBProjectEdit::comments()
{
  QWidget	*W	= new QWidget(this);
  QBoxLayout	*Layout = new QVBoxLayout(W, 20, 10);
  mleComments = new QMultiLineEdit(W);
  mleComments->setText(Project->comments());
  Layout->addWidget(mleComments);
  Layout->activate();
  connect(mleComments, SIGNAL(textChanged()), SLOT(commentsChanged()));
  return W;
};

void SBProjectEdit::commentsChanged()
{
  isCommentsChanged = TRUE;
  disconnect(mleComments, SIGNAL(textChanged()), this, SLOT(commentsChanged()));
};

void SBProjectEdit::sessionListChanged()
{
  isCommentsChanged = TRUE;
  isModified = TRUE;
  brwsSes->updateList();
  brwsSta->updateList();
  brwsSou->updateList(CRF);
  brwsBas->updateList();
};

void SBProjectEdit::makeApply()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": applying");
  acquireData();
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": done");
};

void SBProjectEdit::delSession()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": delete a Session");

  SBVLBISesInfoLI*	SIitem = NULL;
  SBVLBISesInfo*	SI = NULL;
  if ( (SIitem=(SBVLBISesInfoLI*)brwsSes->lvSessions()->currentItem()) && (SI=SIitem->sessionInfo()))
    {
      if (!QMessageBox::warning(this, "Delete?", "Are you sure to delete a session\n \""
				+ SI->name() + "\"?\n", "Yes, delete it!", "No, let it live."))
	{
	  QListViewItem* NextItem = SIitem->itemBelow();
	  if (!NextItem) 
	    NextItem = SIitem->itemAbove();
	  if (Project->delSession(Wvlbi, SI))
	    {
	      delete SIitem;
	      SIitem = NULL;
	      SI = NULL;
	      sessionListChanged();
	      //	      if (NextItem) 
	      //		brwsSes->lvSessions()->setSelected(NextItem, TRUE);
	    }
	  else
	    Log->write(SBLog::WRN, SBLog::INTERFACE, ClassName() + ": cannot delete the session [" +
		       SI->name() + "] from the project");
	}
      else
	Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() +  
		   ": relax, `" + SI->name() + "', it's just a joke..");
    };
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": done");
};

void SBProjectEdit::addSession()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": add Session(s)");
  SBProjectCreate	*PrjCreator = new SBProjectCreate(Project, this, "Adding sessions");
  connect(PrjCreator, SIGNAL(sessionsAdded()), SLOT(sessionListChanged()));
  PrjCreator->show();
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": done");
};

void SBProjectEdit::acquireData()
{
  if (isCommentsChanged)
    {
      Project->setComments(mleComments->text());
      isModified = TRUE;
    };
};

void SBProjectEdit::toggleEntryMarkEnable(QListViewItem* SesItem, const QPoint& /*Point*/, int n)
{
  if (SesItem) // may be NULL
    {
      if (n==14) // explicit number of the column
	{
	  ((SBVLBISesInfoLI*)SesItem)->sessionInfo()->xorAttr(SBVLBISesInfo::ConstrEOP);
	  SesItem->repaint();
	  isModified = TRUE;
	};
      if (n==16) // explicit number of the column
	{
	  ((SBVLBISesInfoLI*)SesItem)->sessionInfo()->xorAttr(SBVLBISesInfo::notValid);
	  SesItem->repaint();
	  isModified = TRUE;
	};
    };
};

void SBProjectEdit::editEntry(QListViewItem* SesItem)
{
  SBVLBISessionEditor *SE = new SBVLBISessionEditor((SBVLBISesInfoLI*)SesItem, Wvlbi);
  connect (SE, SIGNAL(sessionModified(bool)), SLOT(modified(bool)));
  connect (SE, SIGNAL(windowClosed()), SLOT(childWindowClosed()));
  numOpenedWindows++;
  SE->show();
};

void SBProjectEdit::modified(bool isModified_)
{
  isModified |= isModified_;
};

void SBProjectEdit::childWindowClosed()
{
  numOpenedWindows--;
};

void SBProjectEdit::histVerInced()
{
  Project->incVersion(); 
  isModified=TRUE;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBProjectSel::ProjectListItem implementation..						*/
/*												*/
/*==============================================================================================*/
SBProjectSel::ProjectListItem::ProjectListItem(QListView* parent, SBProject* P_)
  : QListViewItem(parent)
{
  Name = P_->name().copy();
  TStart = P_->tStart();
  TFinis = P_->tFinis();
  Interval = P_->tInterval();
  NumObs = P_->numObs();
  NumSes = P_->numVLBISessions();
  NumSta = P_->numStations();
  NumBas = P_->numBases();
  NumSou = P_->numSources();
  Ver = P_->version();
};

QString SBProjectSel::ProjectListItem::text(int col) const
{
  TmpString = "NULL";
  switch (col)
    {
    case  0: TmpString = Name.copy();
      break;
    case  1: TmpString = TStart.toString(SBMJD::F_DDMonYYYY).copy();
      break;
    case  2: TmpString = TFinis.toString(SBMJD::F_DDMonYYYY).copy();
      break;
    case  3: TmpString.sprintf("%.3f", Interval);
    break;
    case  4: TmpString.sprintf("%d", NumObs);
      break;
    case  5: TmpString.sprintf("%d", NumSes);
      break;
    case  6: TmpString.sprintf("%d", NumSta);
      break;
    case  7: TmpString.sprintf("%d", NumBas);
      break;
    case  8: TmpString.sprintf("%d", NumSou);
      break;
    case  9: TmpString.sprintf("%d", Ver);
      break;
    };
  return TmpString;
};

QString SBProjectSel::ProjectListItem::key(int col, bool) const
{
  TmpString = "NULL";
  switch (col)
    {
    case  0: TmpString = Name.copy();
      break;
    case  1: TmpString.sprintf("% 12.6f", (double)TStart);
      break;
    case  2: TmpString.sprintf("% 12.6f", (double)TFinis);
      break;
    case  3: TmpString.sprintf("% 9.3f", Interval);
      break;
    case  4: TmpString.sprintf("% 12d", NumObs);
      break;
    case  5: TmpString.sprintf("% 12d", NumSes);
      break;
    case  6: TmpString.sprintf("% 12d", NumSta);
      break;
    case  7: TmpString.sprintf("% 12d", NumBas);
      break;
    case  8: TmpString.sprintf("% 12d", NumSou);
      break;
    case  9: TmpString.sprintf("% 6d", Ver);
      break;
    };
  return TmpString;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBProjectSel..										*/
/*												*/
/*==============================================================================================*/
SBProjectSel::SBProjectSel(QWidget* parent, const char* name, bool modal, WFlags f)
  : QDialog(parent, name, modal, f)
{
  setCaption("Projects");
  
  QGroupBox	*gbox;
  QBoxLayout	*Layout, *SubLayout;

  gbox	 = new QGroupBox("Select:", this);
  Layout = new QVBoxLayout(gbox, 20, 10);

  lvProject = new QListView(gbox);
  lvProject -> addColumn("Name");
  lvProject -> setColumnAlignment( 0, AlignLeft);
  lvProject -> addColumn("First date");
  lvProject -> setColumnAlignment( 1, AlignRight);
  lvProject -> addColumn("Last date");
  lvProject -> setColumnAlignment( 2, AlignRight);
  lvProject -> addColumn("Interval (days)");
  lvProject -> setColumnAlignment( 3, AlignRight);
  lvProject -> addColumn("Obs");
  lvProject -> setColumnAlignment( 4, AlignRight);
  lvProject -> addColumn("Sessions");
  lvProject -> setColumnAlignment( 4, AlignRight);
  lvProject -> addColumn("Stations");
  lvProject -> setColumnAlignment( 5, AlignRight);
  lvProject -> addColumn("Bases");
  lvProject -> setColumnAlignment( 6, AlignRight);
  lvProject -> addColumn("Sources");
  lvProject -> setColumnAlignment( 7, AlignRight);
  lvProject -> addColumn("Version");
  lvProject -> setColumnAlignment( 8, AlignRight);

  QDir D(SetUp->path2Proj());
  D.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
  D.setSorting(QDir::Name | QDir::IgnoreCase);
  
  const QFileInfoList *DirList = D.entryInfoList();
  QFileInfoListIterator it(*DirList);
  QFileInfo *fi;
  while ((fi=it.current()))
    {
      QFile f(fi->filePath());
      if (f.open(IO_ReadOnly))
	{
	  SBProject *P = new SBProject("");
	  QDataStream s(&f);
	  s >> *P;
	  if (f.status()==IO_Ok) (void) new ProjectListItem(lvProject, P);
	  else Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + 
			  ": error reading `" + fi->filePath() + "' as a project file.");
	  f.close();
	  s.unsetDevice();
	  delete P;
	};
      ++it;
    };
  
  lvProject->setAllColumnsShowFocus(TRUE);
  lvProject->setMinimumSize(lvProject->sizeHint());
  lvProject->setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  if (lvProject->firstChild())
    lvProject->setSelected(lvProject->firstChild(), TRUE);
  lvProject->setFocusPolicy(QWidget::StrongFocus);
  Layout->addWidget(lvProject, 5);
  Layout->activate();
  //connect (lvProject, SIGNAL(doubleClicked(QListViewItem*)), SLOT(accept()));
  connect (lvProject, SIGNAL(returnPressed(QListViewItem*)), SLOT(accept()));

  //---
  Layout = new QVBoxLayout(this, 10, 10);
  Layout -> addWidget(gbox, 10);
  SubLayout = new QHBoxLayout(10);
  Layout -> addLayout(SubLayout);
  SubLayout-> addStretch(1);

  QPushButton *Ok	= new QPushButton("OK", this);
  Ok->setDefault(TRUE);
  QPushButton *Remove	= new QPushButton("Remove", this);
  QPushButton *Cancel	= new QPushButton("Cancel", this);

  QSize	BtnSize;
  Cancel->setMinimumSize((BtnSize=Cancel->sizeHint()));
  Ok->setMinimumSize(BtnSize);
  Remove->setMinimumSize(BtnSize);

  SubLayout->addWidget(Ok);
  SubLayout->addWidget(Remove);
  SubLayout->addWidget(Cancel);

  connect(Ok, SIGNAL(clicked()), SLOT(accept()));
  connect(Remove, SIGNAL(clicked()), SLOT(remove()));
  connect(Cancel, SIGNAL(clicked()), SLOT(reject()));
  Layout->activate();
};

void SBProjectSel::accept()
{
  QDialog::accept(); 
  if (lvProject->currentItem())
    SetUp->setProject(((ProjectListItem*)(lvProject->currentItem()))->name());
  delete this;
};

void SBProjectSel::remove()
{
  ProjectListItem *ni, *w;
  if (lvProject->childCount() &&
      !QMessageBox::warning(this, "Delete?", "Are you sure to delete the project\n \"" +
			    ((ni=(ProjectListItem*)lvProject->currentItem()))->name()
			    + "\"?\n", "Yes, del them!", "No, let it live.."))
    {
      if (!(w=(ProjectListItem*)ni->itemBelow())) w=(ProjectListItem*)ni->itemAbove();
      QDir D(SetUp->path2Proj());
      if (!D.remove(ni->name()))
	Log->write(SBLog::DBG, SBLog::IO, ClassName() + 
		   ": error deleting + `" + ni->name() + "' project file.");
      delete ni;
      if (w) lvProject->setSelected(w, TRUE);
    };
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBModelEditor..										*/
/*												*/
/*==============================================================================================*/
SBModelEditor::SBModelEditor(SBConfig* Cfg_,
			     QWidget* parent, const char* name, bool modal, WFlags f)
  : SBTabDialog(parent, name, modal, f)
{
  isModified = FALSE;
  if (Cfg_)
    {
      Cfg = new SBConfig(*Cfg_);
      Project = NULL;
      isGlobal = TRUE;
    }
  else
    {
      //      Project = new SBProject("");
      Project = SetUp->loadProject();
      Cfg = new SBConfig(Project->cfg());
      isGlobal = FALSE;
    };
  setCaption(isGlobal?QString("Global models"):"Models for project: " + Project->name());

  addTab(wDataProcessing(),	"Observables");
  addTab(wSysTransform(),	"CRS<->TRS");
  addTab(wLightPropagation(),	"Grav. delays");
  addTab(wSolidTide(),		"Solid tide");
  addTab(wEOPs(),		"EOPs");
  addTab(wNutation(),		"Nutation");
  addTab(wPlateMotion(),	"Station Motion");
  addTab(wRefraction(),		"Refraction");
  addTab(wSolutionReports(),	"Reports");

  setCancelButton();
  setApplyButton();
  setDefaultButton("Defaults, IERS 2003");
  connect(this, SIGNAL(applyButtonPressed()), SLOT(makeApply()));
  connect(this, SIGNAL(defaultButtonPressed()), SLOT(makeDefault()));
};

SBModelEditor::~SBModelEditor()
{
  if (isModified)
    {
      if (isGlobal) Config = *Cfg;
      else
	{
	  Project->setCfg(*Cfg);
	  SetUp->saveProject(Project);
	};
    };
  if (Cfg) delete Cfg;
  if (Project) delete Project;
};
 
void SBModelEditor::makeDefault()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": defaulting");
  *Cfg = DefaultConfig;

  rbSysTransform[Cfg->sysTransform()]->setChecked(TRUE);
  if (!isGlobal)
    {
      cbUseAltStations->setChecked(Cfg->useAltStations());
      cbUseAltSources ->setChecked(Cfg->useAltSources());
      cbUseAltEOPs    ->setChecked(Cfg->useAltEOPs());
      leAltStations->setText(Cfg->altStations());
      leAltSources ->setText(Cfg->altSources());
      leAltEOPs    ->setText(Cfg->altEOPs());
    };
  
  rbEphem[Cfg->ephemModel()]->setChecked(TRUE);
  cbIsHO-> setChecked(Cfg->isGravDelayHO());
  leHOThreshold->setText(Str.sprintf("%.3f", RAD2DEG*Cfg->gravHOThreshold()));
  for (int i=0; i<11; i++) cbBody[i]-> setChecked(Cfg->isEphBody(i));
  
  rbSTide[Cfg->solidTideLd()]->setChecked(TRUE);
  cbPTide-> setChecked(Cfg->isRemovePermTide());
  rbDegree[Cfg->solidTideDegree()]->setChecked(TRUE);
  cbAnelastic-> setChecked(Cfg->isSolidTideAnelastic());
  cbLatDepend-> setChecked(Cfg->isSolidTideLatDepend());

  rbEOPDiurn[Cfg->eopDiurnal()]->setChecked(TRUE);
  rbEOPDiurnAtm[Cfg->eopDiurnalAtm()]->setChecked(TRUE);
  rbEOPDiurnNonTidal[Cfg->eopDiurnalNonTidal()]->setChecked(TRUE);
  leEOPNInterp->setText(Str.sprintf("%d", Cfg->eopNumIntrplPoints()));
  cbEOP_IAU2K_XY->setChecked(Cfg->isEOP_IAU2K_XY());
  cbEOPResrerved_1->setChecked(Cfg->isEOPResrerved_1());
  
  rbNutModel[Cfg->nutModel()]->setChecked(TRUE);
  rbNutEquinox[Cfg->nutEquinox()]->setChecked(TRUE);
  cbNutGeod-> setChecked(Cfg->isNeedGeodNut());

  rbTPMModel[Cfg->plateMotion()]->setChecked(TRUE);
  cbStatVel->setChecked(Cfg->isUseStationVelocity());
  cbRB2Plates->setChecked(Cfg->isRollBack2Plates());
  cbAplo->setChecked(Cfg->isUseAploEphem());

  rbDryZenith[Cfg->refrDryZenith()]->setChecked(TRUE);
  rbWetZenith[Cfg->refrWetZenith()]->setChecked(TRUE);
  rbDryMap[Cfg->refrDryMapping()]->setChecked(TRUE);
  rbWetMap[Cfg->refrWetMapping()]->setChecked(TRUE);
  cbAxissAlt-> setChecked(Cfg->isAltAxsCorr());
  cbRB2NMF  -> setChecked(Cfg->isRollBack2NMF());

  cbUseDelay-> setChecked(Cfg->isUseDelay());
  cbUseRate -> setChecked(Cfg->isUseRate());
  eDelayQThreshold->setText(QString().setNum(Cfg->qCodeThreshold()));
  eRateQThreshold ->setText(QString().setNum(Cfg->dUFlagThreshold()));

  cbReportMaps		-> setChecked(Cfg->reportMaps());
  cbReportCRF		-> setChecked(Cfg->reportCRF());
  cbReportTRF_SINEX	-> setChecked(Cfg->reportTRF_SINEX());
  cbReportEOP		-> setChecked(Cfg->reportEOP());
  cbReportTechDesc	-> setChecked(Cfg->reportTechDesc());
  cbReportNormalEqs	-> setChecked(Cfg->reportNormalEqs());
  cbReportNormalEqsSta	-> setChecked(Cfg->reportNormalEqsSTCs());
  cbReportNormalEqsSou	-> setChecked(Cfg->reportNormalEqsSRCs());
  cbReportNormalEqsEOP	-> setChecked(Cfg->reportNormalEqsEOPs());
  cbReportTroposhere	-> setChecked(Cfg->reportTroposhere());
  cbReportTRF_Changes	-> setChecked(Cfg->reportTRF_Changes());
  cbReportCRF_Changes	-> setChecked(Cfg->reportCRF_Changes());

  leTropSamplInterval->setText(Str.sprintf("%.1f", Cfg->tropSamplInterval()));
  leTropStartSec->setText(Str.sprintf("%d", Cfg->tropStartSec()));

  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": done");
};

void SBModelEditor::makeApply()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": applying");
  acquireData();
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": done");
};

QWidget* SBModelEditor::wSysTransform()
{
  static const QString TransformName[2] = 
  {
    QString("the Equinox (classic)"), 
    QString("the Non Rotating Origin")
  };

  QWidget	*W = new QWidget(this);
  QBoxLayout	*Layout = new QVBoxLayout(W, 20, 10), *aLayout;

  // Sys transform:
  QButtonGroup* bgSysTransform = new QButtonGroup("Coordinate Transformation Referred to: ", W);
  aLayout = new QVBoxLayout(bgSysTransform, 20, 5);
  for (int i=0; i<2; i++)
    {
      rbSysTransform[i] = new QRadioButton(TransformName[i], bgSysTransform);
      rbSysTransform[i]-> setMinimumSize(rbSysTransform[i]->sizeHint());
      aLayout->addWidget(rbSysTransform[i]);
    };
  rbSysTransform[Cfg->sysTransform()]->setChecked(TRUE);
  aLayout->activate();
  Layout->addWidget(bgSysTransform);

  if (!isGlobal)
    {
      // Alts:
      QButtonGroup* bgAltDB = new QButtonGroup("Alternative data bases: ", W);
      QGridLayout *grid = new QGridLayout(bgAltDB, 3,2,  20,5);
      
      cbUseAltStations = new QCheckBox("Use alternative Stations DB:", bgAltDB);
      cbUseAltStations->setMinimumSize(cbUseAltStations->sizeHint());
      cbUseAltStations->setChecked(Cfg->useAltStations());
      grid->addWidget(cbUseAltStations, 0,0);
      
      cbUseAltSources = new QCheckBox("Use alternative Sources DB:", bgAltDB);
      cbUseAltSources->setMinimumSize(cbUseAltSources->sizeHint());
      cbUseAltSources->setChecked(Cfg->useAltSources());
      grid->addWidget(cbUseAltSources, 1,0);
      
      cbUseAltEOPs = new QCheckBox("Use alternative EOP DB:", bgAltDB);
      cbUseAltEOPs->setMinimumSize(cbUseAltEOPs->sizeHint());
      cbUseAltEOPs->setChecked(Cfg->useAltEOPs());
      grid->addWidget(cbUseAltEOPs, 2,0);
      
      leAltStations = new QLineEdit(QString(Cfg->altStations()), bgAltDB);
      leAltStations->setMinimumSize(leAltStations->sizeHint());
      leAltStations->setEnabled(Cfg->useAltStations());
      grid->addWidget(leAltStations, 0,1);
      
      leAltSources = new QLineEdit(QString(Cfg->altSources()), bgAltDB);
      leAltSources->setMinimumSize(leAltSources->sizeHint());
      leAltSources->setEnabled(Cfg->useAltSources());
      grid->addWidget(leAltSources, 1,1);
      
      leAltEOPs = new QLineEdit(QString(Cfg->altEOPs()), bgAltDB);
      leAltEOPs->setMinimumSize(leAltEOPs->sizeHint());
      leAltEOPs->setEnabled(Cfg->useAltEOPs());
      grid->addWidget(leAltEOPs, 2,1);
      
      connect(cbUseAltStations, SIGNAL(toggled(bool)), leAltStations, SLOT(setEnabled(bool)));
      connect(cbUseAltSources,  SIGNAL(toggled(bool)), leAltSources,  SLOT(setEnabled(bool)));
      connect(cbUseAltEOPs,     SIGNAL(toggled(bool)), leAltEOPs,     SLOT(setEnabled(bool)));

      grid->activate();
      Layout->addWidget(bgAltDB);
    };
  
  Layout->addStretch(1);
  Layout->activate();
  return W;
};

QWidget* SBModelEditor::wLightPropagation()
{
  static const QString EphemName[3] = 
  {
    QString("JPL Planetary Ephemeris DE200/LE200"), 
    QString("JPL Planetary Ephemeris DE403/LE403"), 
    QString("JPL Planetary Ephemeris DE405/LE405")
  };
  static const QString CelestBody[11] = 
  {QString("Mercury"), QString("Venus"), QString("Earth"), QString("Mars"), 
   QString("Jupiter"), QString("Saturn"), QString("Uranus"), QString("Neptune"),
   QString("Pluto"), QString("Moon"), QString("Sun")};

  QLabel	*label;
  QWidget	*W	= new QWidget(this);
  QBoxLayout	*Layout = new QHBoxLayout(W, 20, 10), *subLayout = new QVBoxLayout(20), *aLayout;
  
  //  Layout->addStretch(1);
  Layout->addLayout(subLayout);
  //  Layout->addStretch(1);

  QButtonGroup* bgEphem = new QButtonGroup("Ephemerides model", W);
  aLayout = new QVBoxLayout(bgEphem, 20, 5);
  for (int i=0; i<3; i++)
    {
      rbEphem[i] = new QRadioButton(EphemName[i], bgEphem);
      rbEphem[i]-> setMinimumSize(rbEphem[i]->sizeHint());
      aLayout->addWidget(rbEphem[i]);
    };
  rbEphem[Cfg->ephemModel()]->setChecked(TRUE);
  aLayout->activate();
  subLayout->addWidget(bgEphem);
  subLayout->addSpacing(20);
  
  cbIsHO = new QCheckBox("High Order Relativistic Delay (observation close to a body)", W);
  cbIsHO-> setMinimumSize(cbIsHO->sizeHint());
  cbIsHO-> setChecked(Cfg->isGravDelayHO());
  subLayout->addWidget(cbIsHO);
  
  aLayout = new QHBoxLayout(10);
  subLayout->addLayout(aLayout);
  leHOThreshold= new QLineEdit(W);
  leHOThreshold->setText(Str.sprintf("%.3f", RAD2DEG*Cfg->gravHOThreshold()));
  leHOThreshold->setMinimumSize(leHOThreshold->sizeHint());
  
  label = new QLabel(leHOThreshold, "High order threshold (deg): ", W);
  label->setMinimumSize(label->sizeHint());
  aLayout->addWidget(label);
  aLayout->addWidget(leHOThreshold);
  subLayout->addStretch(1);

  subLayout = new QVBoxLayout(10);
  Layout -> addLayout(subLayout);

  QButtonGroup* bgBodies = new QButtonGroup("Gravitation Delay from", W);
  aLayout = new QVBoxLayout(bgBodies, 20, 5);
  for (int i=0; i<11; i++)
    {
      cbBody[i] = new QCheckBox(CelestBody[i], bgBodies);
      cbBody[i]-> setMinimumSize(cbBody[i]->sizeHint());
      cbBody[i]-> setChecked(Cfg->isEphBody(i));
      aLayout->addWidget(cbBody[i]);
    };
  aLayout->activate();
  subLayout->addWidget(bgBodies);
  //  Layout->addStretch(1);

  Layout->activate();
  return W;
};

QWidget* SBModelEditor::wSolidTide()
{
  static const QString STideName[2] = 
  {QString("IERS Standards 1992"), QString("IERS Conventions 1996")};
  static const QString STDegreeName[2] = 
  {QString("Solid tide degree 2"), QString("Solid tide degree 2 and 3")};
  QWidget	*W = new QWidget(this);
  QBoxLayout	*Layout = new QVBoxLayout(W, 20, 10), *subLayout, *aLayout;
  
  QButtonGroup* bgSTide = new QButtonGroup("Solid Tide Model", W);
  aLayout = new QVBoxLayout(bgSTide, 20, 5);
  for (int i=0; i<2; i++)
    {
      rbSTide[i] = new QRadioButton(STideName[i], bgSTide);
      rbSTide[i]-> setMinimumSize(rbSTide[i]->sizeHint());
      aLayout->addWidget(rbSTide[i]);
    };
  rbSTide[Cfg->solidTideLd()]->setChecked(TRUE);
  connect(rbSTide[1], SIGNAL(toggled(bool)), SLOT(solidTideIERS92(bool)));
  aLayout->activate();
  Layout->addWidget(bgSTide);
  Layout->addStretch(1);

  gbSTIERS96 = new QGroupBox("IERS 96 Model Options", W);
  subLayout = new QVBoxLayout(gbSTIERS96, 20, 10);
  
  bgSTOrder = new QButtonGroup("Solid Tide degree order", gbSTIERS96);
  aLayout = new QVBoxLayout(bgSTOrder, 20, 5);
  for (int i=0; i<2; i++)
    {
      rbDegree[i] = new QRadioButton(STDegreeName[i], bgSTOrder);
      rbDegree[i]-> setMinimumSize(rbDegree[i]->sizeHint());
      aLayout->addWidget(rbDegree[i]);
    };
  rbDegree[Cfg->solidTideDegree()]->setChecked(TRUE);
  subLayout->addWidget(bgSTOrder);
  subLayout->activate();

  cbAnelastic = new QCheckBox("Anelastic Earth model", gbSTIERS96);
  cbAnelastic-> setMinimumSize(cbAnelastic->sizeHint());
  cbAnelastic-> setChecked(Cfg->isSolidTideAnelastic());
  subLayout->addWidget(cbAnelastic);

  cbLatDepend = new QCheckBox("Latitude dependence of Love && Shida numbers", gbSTIERS96);
  cbLatDepend-> setMinimumSize(cbLatDepend->sizeHint());
  cbLatDepend-> setChecked(Cfg->isSolidTideLatDepend());
  subLayout->addWidget(cbLatDepend);
  subLayout->activate();
  Layout->addWidget(gbSTIERS96);
  Layout->addStretch(1);

  cbPTide = new QCheckBox("Remove Permanent Deformations", W);
  cbPTide-> setMinimumSize(cbPTide->sizeHint());
  cbPTide-> setChecked(Cfg->isRemovePermTide());
  Layout->addWidget(cbPTide);
  Layout->addStretch(2);

  Layout->activate();
  return W;
};

void SBModelEditor::solidTideIERS92(bool on)
{
  gbSTIERS96->setEnabled(on);
  bgSTOrder->setEnabled(on);
  rbDegree[0]->setEnabled(on);
  rbDegree[1]->setEnabled(on);
  cbAnelastic->setEnabled(on);
  cbLatDepend->setEnabled(on);
};

QWidget* SBModelEditor::wEOPs()
{
  static const QString EOPDiurnName[6] = 
  {QString("None"), QString("IERS Conventions 1996 model, Ray (1995)"), 
   QString("`JPL-92' model, Sovers at al. (1993)"), QString("Herring-Dong 1994 model"), 
   QString("`GSFC-95' model, Ma (1995)"), QString("Gipson (1995) model")};
  static const QString EOPDiurnNameAtm[2] = 
  {QString("None"), QString("Brzezinski, Bizouard and Petrov (2002)")};
  static const QString EOPDiurnNameNonTidal[2] = 
  {QString("None"), QString("de Viron et al. (2002)")};


  QLabel	*label;
  QWidget	*W = new QWidget(this);
  QBoxLayout	*Layout = new QVBoxLayout(W, 20, 10), *aLayout, *bLayout;


  QButtonGroup* bgEOPInterpOpt = new QButtonGroup("EOP Interpolation options", W);
  aLayout = new QVBoxLayout(bgEOPInterpOpt, 20, 5);

  cbEOP_IAU2K_XY = new QCheckBox("Assume EOP nutation angles are dX and dY with respect to IAU-2000", bgEOPInterpOpt);
  cbEOP_IAU2K_XY-> setMinimumSize(cbEOP_IAU2K_XY->sizeHint());
  cbEOP_IAU2K_XY-> setChecked(Cfg->isEOP_IAU2K_XY());
  aLayout->addWidget(cbEOP_IAU2K_XY);

  cbEOPResrerved_1 = new QCheckBox("Reserved #1", bgEOPInterpOpt);
  cbEOPResrerved_1-> setMinimumSize(cbEOPResrerved_1->sizeHint());
  cbEOPResrerved_1-> setChecked(Cfg->isEOPResrerved_1());
  aLayout->addWidget(cbEOPResrerved_1);

  bLayout = new QHBoxLayout(10);
  aLayout->addLayout(bLayout);
  leEOPNInterp = new QLineEdit(bgEOPInterpOpt);
  leEOPNInterp->setText(Str.sprintf("%d", Cfg->eopNumIntrplPoints()));
  leEOPNInterp->setMinimumSize(leEOPNInterp->sizeHint());
  
  label = new QLabel(leEOPNInterp, "Number of points for interpolation: ", bgEOPInterpOpt);
  label->setMinimumSize(label->sizeHint());
  bLayout->addWidget(label);
  bLayout->addWidget(leEOPNInterp);

  aLayout->activate();
  Layout->addWidget(bgEOPInterpOpt);

  QBoxLayout *cLayout = new QHBoxLayout(Layout);

  QButtonGroup* bgEOPDiurn = new QButtonGroup("Tidal Diurnal Variations", W);
  aLayout = new QVBoxLayout(bgEOPDiurn, 20, 5);
  for (int i=0; i<6; i++)
    {
      rbEOPDiurn[i] = new QRadioButton(EOPDiurnName[i], bgEOPDiurn);
      rbEOPDiurn[i]-> setMinimumSize(rbEOPDiurn[i]->sizeHint());
      aLayout->addWidget(rbEOPDiurn[i]);
    };
  rbEOPDiurn[Cfg->eopDiurnal()]->setChecked(TRUE);
  aLayout->activate();
  cLayout->addWidget(bgEOPDiurn);
  
  bLayout = new QVBoxLayout(cLayout);
  bgEOPDiurn = new QButtonGroup("Atmosperic Diurnal Variations", W);
  aLayout = new QVBoxLayout(bgEOPDiurn, 20, 5);
  for (int i=0; i<2; i++)
    {
      rbEOPDiurnAtm[i] = new QRadioButton(EOPDiurnNameAtm[i], bgEOPDiurn);
      rbEOPDiurnAtm[i]-> setMinimumSize(rbEOPDiurnAtm[i]->sizeHint());
      aLayout->addWidget(rbEOPDiurnAtm[i]);
    };
  rbEOPDiurnAtm[Cfg->eopDiurnalAtm()]->setChecked(TRUE);
  aLayout->activate();
  bLayout->addWidget(bgEOPDiurn);

  bgEOPDiurn = new QButtonGroup("Non-Tidal Diurnal Variations", W);
  aLayout = new QVBoxLayout(bgEOPDiurn, 20, 5);
  for (int i=0; i<2; i++)
    {
      rbEOPDiurnNonTidal[i] = new QRadioButton(EOPDiurnNameNonTidal[i], bgEOPDiurn);
      rbEOPDiurnNonTidal[i]-> setMinimumSize(rbEOPDiurnNonTidal[i]->sizeHint());
      aLayout->addWidget(rbEOPDiurnNonTidal[i]);
    };
  rbEOPDiurnNonTidal[Cfg->eopDiurnalNonTidal()]->setChecked(TRUE);
  aLayout->activate();
  bLayout->addWidget(bgEOPDiurn);

  //  Layout->addWidget(bgEOPDiurn);
  Layout->addStretch(1);

  Layout->activate();
  return W;
};

QWidget* SBModelEditor::wNutation()
{
  static const QString NutModelName[3] = 
  {QString("The IAU 1980 Theory of Nutation (IERS Standards 1992)"), 
   QString("The IAU 1996 Theory of Nutation (IERS Conventions 1996)"),
   QString("The IAU 2000 Theory of Nutation (IERS Conventions 2000)")};
  static const QString EqModelName[3] = 
  {QString("According to IERS Standards 1992"),
   QString("According to Sovers and Jacobs, 1996"), 
   QString("According to IERS Conventions 1996")};
  QWidget	*W = new QWidget(this);
  QBoxLayout	*Layout = new QVBoxLayout(W, 20, 10), *aLayout;

  QButtonGroup* bgNutModel = new QButtonGroup("Theory Nutation Model", W);
  aLayout = new QVBoxLayout(bgNutModel, 20, 5);
  for (int i=0; i<3; i++)
    {
      rbNutModel[i] = new QRadioButton(NutModelName[i], bgNutModel);
      rbNutModel[i]-> setMinimumSize(rbNutModel[i]->sizeHint());
      aLayout->addWidget(rbNutModel[i]);
    };
  rbNutModel[Cfg->nutModel()]->setChecked(TRUE);
  aLayout->activate();
  Layout->addWidget(bgNutModel);
  Layout->addStretch(1);

  QButtonGroup* bgEquinoxModel = new QButtonGroup("Equation of Equinoxes", W);
  aLayout = new QVBoxLayout(bgEquinoxModel, 20, 5);
  for (int i=0; i<3; i++)
    {
      rbNutEquinox[i] = new QRadioButton(EqModelName[i], bgEquinoxModel);
      rbNutEquinox[i]-> setMinimumSize(rbNutEquinox[i]->sizeHint());
      aLayout->addWidget(rbNutEquinox[i]);
    };
  rbNutEquinox[Cfg->nutEquinox()]->setChecked(TRUE);
  aLayout->activate();
  Layout->addWidget(bgEquinoxModel);
  Layout->addStretch(1);

  cbNutGeod = new QCheckBox("Take into account Geodetic Nutation", W);
  cbNutGeod-> setMinimumSize(cbNutGeod->sizeHint());
  cbNutGeod-> setChecked(Cfg->isNeedGeodNut());
  Layout->addWidget(cbNutGeod);
  Layout->addStretch(1);

  Layout->activate();
  return W;
};

QWidget* SBModelEditor::wPlateMotion()
{
  static const QString PlateModelName[3] = 
  {QString("NNR Nuvel-1A Model"), 
   QString("Nuvel-1A Model"),
   QString("None")};
  QWidget	*W = new QWidget(this);
  QBoxLayout	*Layout = new QVBoxLayout(W, 20, 10), *aLayout;

  QButtonGroup* bgTPMModel = new QButtonGroup("Tectonic Plate Motion", W);
  aLayout = new QVBoxLayout(bgTPMModel, 20, 5);
  for (int i=0; i<3; i++)
    {
      rbTPMModel[i] = new QRadioButton(PlateModelName[i], bgTPMModel);
      rbTPMModel[i]-> setMinimumSize(rbTPMModel[i]->sizeHint());
      aLayout->addWidget(rbTPMModel[i]);
    };
  rbTPMModel[Cfg->plateMotion()]->setChecked(TRUE);
  aLayout->activate();
  Layout->addWidget(bgTPMModel);
  Layout->addStretch(1);

  cbStatVel = new QCheckBox("Use station velocity according to the Station Catalogue", W);
  cbStatVel-> setMinimumSize(cbStatVel->sizeHint());
  cbStatVel-> setChecked(Cfg->isUseStationVelocity());
  Layout->addWidget(cbStatVel);

  cbRB2Plates = new QCheckBox("Use NUVEL-NNR1A plate motion model if the velocity is equal to zero", W);
  cbRB2Plates-> setMinimumSize(cbRB2Plates->sizeHint());
  cbRB2Plates-> setChecked(Cfg->isRollBack2Plates());
  Layout->addWidget(cbRB2Plates);

  Layout->addStretch(1);

  cbAplo = new QCheckBox("Use Atmosperic Loading Ephemerides", W);
  cbAplo-> setMinimumSize(cbAplo->sizeHint());
  cbAplo-> setChecked(Cfg->isUseAploEphem());
  Layout->addWidget(cbAplo);

  Layout->addStretch(3);

  if (Cfg->plateMotion()!=SBConfig::PM_NONE)
    {
      cbStatVel  ->setEnabled(FALSE);
      cbRB2Plates->setEnabled(FALSE);
    };
  connect(rbTPMModel[2], SIGNAL(toggled(bool)), cbStatVel,   SLOT(setEnabled(bool)));
  connect(rbTPMModel[2], SIGNAL(toggled(bool)), cbRB2Plates, SLOT(setEnabled(bool)));
  
  Layout->activate();
  return W;
};

QWidget* SBModelEditor::wRefraction()
{
  static const QString DryZenithName[3] = 
  {QString("None"), 
   QString("Saastamoinen, 1972"), 
   QString("Hopfield, 1977")};
  static const QString WetZenithName[3] = 
  {QString("None (only estimated values)"), 
   QString("Saastamoinen, 1973"), 
   QString("Hopfield, 1977")};
  static const QString DryMapName[6] = 
  {QString("None"), 
   QString("Chao, 1974"), 
   QString("CfA-2.2, Davis et al., 1985"), 
   QString("Ifadis, 1986"), 
   QString("MTT, Herring, 1992"),
   QString("NMFh2, Niell, 1996")};
  static const QString WetMapName[6] = 
  {QString("None"), 
   QString("Chao, 1974"), 
   QString("Ifadis, 1986"), 
   QString("MTT, Herring, 1992"), 
   QString("NMFw2, Niell, 1996"),
   QString("Reserved")};

  QWidget	*W	= new QWidget(this);
  QBoxLayout	*Layout = new QVBoxLayout(W, 20, 10), *subLayout = new QHBoxLayout;
  QBoxLayout	*aLayout, *layout;
  Layout->addLayout(subLayout);
  aLayout = new QVBoxLayout;
  subLayout->addLayout(aLayout);

  QButtonGroup* bg = new QButtonGroup("Hydrostatic Zenith Delay model", W);
  layout = new QVBoxLayout(bg, 20, 5);
  for (int i=0; i<3; i++)
    {
      rbDryZenith[i] = new QRadioButton(DryZenithName[i], bg);
      rbDryZenith[i]-> setMinimumSize(rbDryZenith[i]->sizeHint());
      layout->addWidget(rbDryZenith[i]);
    };
  rbDryZenith[Cfg->refrDryZenith()]->setChecked(TRUE);
  layout->activate();
  aLayout->addWidget(bg);
  aLayout->addStretch(1);
  
  bg = new QButtonGroup("Hydrostatic Mapping Function model", W);
  layout = new QVBoxLayout(bg, 20, 5);
  for (int i=0; i<6; i++)
    {
      rbDryMap[i] = new QRadioButton(DryMapName[i], bg);
      rbDryMap[i]-> setMinimumSize(rbDryMap[i]->sizeHint());
      layout->addWidget(rbDryMap[i]);
    };
  rbDryMap[Cfg->refrDryMapping()]->setChecked(TRUE);
  layout->activate();
  aLayout->addWidget(bg);


  aLayout = new QVBoxLayout(10);
  subLayout->addLayout(aLayout);

  bg = new QButtonGroup("Wet Zenith Delay model", W);
  layout = new QVBoxLayout(bg, 20, 5);
  for (int i=0; i<3; i++)
    {
      rbWetZenith[i] = new QRadioButton(WetZenithName[i], bg);
      rbWetZenith[i]-> setMinimumSize(rbWetZenith[i]->sizeHint());
      layout->addWidget(rbWetZenith[i]);
    };
  rbWetZenith[Cfg->refrWetZenith()]->setChecked(TRUE);
  layout->activate();
  aLayout->addWidget(bg);
  aLayout->addStretch(1);
  
  bg = new QButtonGroup("Wet Mapping Function model", W);
  layout = new QVBoxLayout(bg, 20, 5);
  for (int i=0; i<6; i++)
    {
      rbWetMap[i] = new QRadioButton(WetMapName[i], bg);
      rbWetMap[i]-> setMinimumSize(rbWetMap[i]->sizeHint());
      layout->addWidget(rbWetMap[i]);
    };
  rbWetMap[5]->setEnabled(FALSE);
  rbWetMap[Cfg->refrWetMapping()]->setChecked(TRUE);
  layout->activate();
  aLayout->addWidget(bg);
  
  Layout->addStretch(1);

  cbAxissAlt = new QCheckBox("Altitude Axiss Offset Correction", W);
  cbAxissAlt-> setMinimumSize(cbAxissAlt->sizeHint());
  cbAxissAlt-> setChecked(Cfg->isAltAxsCorr());
  Layout->addWidget(cbAxissAlt);

  cbRB2NMF = new QCheckBox("Roll back to NMF mapping functions if a station has "
			   "suspicious metheo parameters", W);
  cbRB2NMF-> setMinimumSize(cbRB2NMF->sizeHint());
  cbRB2NMF-> setChecked(Cfg->isRollBack2NMF());
  Layout->addWidget(cbRB2NMF);
  Layout->addStretch(1);

  Layout->activate();
  return W;
};

QWidget* SBModelEditor::wDataProcessing()
{
  QWidget	*W = new QWidget(this);
  QBoxLayout	*Layout = new QVBoxLayout(W, 20, 10);
  QGridLayout	*grid;
  QLabel	*label;
  QButtonGroup	*bgGrp;

  bgGrp = new QButtonGroup("General options", W);
  grid = new QGridLayout(bgGrp, 2,2,  20,5);

  cbUseDelay = new QCheckBox("Use Time Delay", bgGrp);
  cbUseDelay-> setMinimumSize(cbUseDelay->sizeHint());
  cbUseDelay-> setChecked(Cfg->isUseDelay());
  grid->addWidget(cbUseDelay, 0,0);

  cbUseRate = new QCheckBox("Use Delay Rate", bgGrp);
  cbUseRate-> setMinimumSize(cbUseRate->sizeHint());
  cbUseRate-> setChecked(Cfg->isUseRate());
  grid->addWidget(cbUseRate, 1,0);

  cbUseBadIon = new QCheckBox("Use not good Ionosphere", bgGrp);
  cbUseBadIon-> setMinimumSize(cbUseBadIon->sizeHint());
  cbUseBadIon-> setChecked(Cfg->isUseBadIon());
  grid->addWidget(cbUseBadIon, 0,1);

  grid->activate();
  Layout->addWidget(bgGrp);
  Layout->addStretch(1);

  // Delays:
  bgGrp = new QButtonGroup("VLBI Time Delay", W);
  grid = new QGridLayout(bgGrp, 2,2,   20,5);
  
  label = new QLabel("Quality Code threshold (>=): ", bgGrp);
  label -> setMinimumSize(label->sizeHint());
  grid  -> addWidget(label, 0,0);

  eDelayQThreshold = new QLineEdit(QString().setNum(Cfg->qCodeThreshold()), bgGrp);
  eDelayQThreshold -> setMinimumSize(eDelayQThreshold->sizeHint());
  grid -> addWidget(eDelayQThreshold, 0,1);

  label = new QLabel("Check for DELUFLAG (<=): ", bgGrp);
  label -> setMinimumSize(label->sizeHint());
  grid  -> addWidget(label, 1,0);

  eRateQThreshold = new QLineEdit(QString().setNum(Cfg->dUFlagThreshold()), bgGrp);
  eRateQThreshold -> setMinimumSize(eRateQThreshold->sizeHint());
  grid -> addWidget(eRateQThreshold, 1,1);

  grid  ->activate();
  Layout->addWidget(bgGrp);
  Layout->addStretch(1);

  // Rates:
  bgGrp = new QButtonGroup("VLBI Delay Rate", W);
  grid = new QGridLayout(bgGrp, 1,2,   20,5);
  
  label = new QLabel("Quality index threshold:", bgGrp);
  label -> setMinimumSize(label->sizeHint());
  grid  -> addWidget(label, 0,0);

  QLineEdit *le = new QLineEdit("N/A", bgGrp);
  le -> setMinimumSize(le->sizeHint());
  grid -> addWidget(le, 0,1);

  grid  ->activate();
  Layout->addWidget(bgGrp);
  Layout->addStretch(1);

  // Run-Time options:
  bgGrp = new QButtonGroup("Run-Time options", W);
  QBoxLayout *aLayout = new QVBoxLayout(bgGrp, 20, 10);

  cbGenerateDSINEXes = new QCheckBox("Keep intermediate results for creating Normal Equation System (SINEX)", bgGrp);
  cbGenerateDSINEXes-> setMinimumSize(cbGenerateDSINEXes->sizeHint());
  cbGenerateDSINEXes-> setChecked(Cfg->isGenerateDSINEXes());
  aLayout->addWidget(cbGenerateDSINEXes);

  aLayout->activate();

  Layout->addWidget(bgGrp);
  Layout->addStretch(1);

  Layout->activate();
  return W;
};

QWidget* SBModelEditor::wSolutionReports()
{
  QWidget	*W = new QWidget(this);
  QBoxLayout	*Layout = new QVBoxLayout(W, 20, 10), *aLayout, *bLayout;
  
  QButtonGroup* bgSolutionReports = new QButtonGroup("Report Will Create: ", W);
  aLayout = new QVBoxLayout(bgSolutionReports, 20, 5);
  
  cbReportMaps = new QCheckBox("Scripts for GMT to plot CRF and TRF maps", bgSolutionReports);
  cbReportMaps-> setMinimumSize(cbReportMaps->sizeHint());
  cbReportMaps-> setChecked(Cfg->reportMaps());
  aLayout->addWidget(cbReportMaps);
  
  cbReportCRF = new QCheckBox("CRF (IERS format)", bgSolutionReports);
  cbReportCRF-> setMinimumSize(cbReportCRF->sizeHint());
  cbReportCRF-> setChecked(Cfg->reportCRF());
  aLayout->addWidget(cbReportCRF);
  
  cbReportTRF_SINEX = new QCheckBox("TRF (SINEX format)", bgSolutionReports);
  cbReportTRF_SINEX-> setMinimumSize(cbReportTRF_SINEX->sizeHint());
  cbReportTRF_SINEX-> setChecked(Cfg->reportTRF_SINEX());
  aLayout->addWidget(cbReportTRF_SINEX);
  
  cbReportEOP = new QCheckBox("EOP (IERS and IVS formats)", bgSolutionReports);
  cbReportEOP-> setMinimumSize(cbReportEOP->sizeHint());
  cbReportEOP-> setChecked(Cfg->reportEOP());
  aLayout->addWidget(cbReportEOP);
  
  cbReportTechDesc = new QCheckBox("Technical Description", bgSolutionReports);
  cbReportTechDesc-> setMinimumSize(cbReportTechDesc->sizeHint());
  cbReportTechDesc-> setChecked(Cfg->reportTechDesc());
  aLayout->addWidget(cbReportTechDesc);

  // NEqS:
  bLayout = new QHBoxLayout(aLayout);
  cbReportNormalEqs = new QCheckBox("Normal Equation System (SINEX); Make report for: ", bgSolutionReports);
  cbReportNormalEqs-> setMinimumSize(cbReportNormalEqs->sizeHint());
  cbReportNormalEqs-> setChecked(Cfg->reportNormalEqs());
  bLayout->addWidget(cbReportNormalEqs);

  cbReportNormalEqsSta = new QCheckBox("Stations", bgSolutionReports);
  cbReportNormalEqsSta-> setMinimumSize(cbReportNormalEqsSta->sizeHint());
  cbReportNormalEqsSta-> setChecked(Cfg->reportNormalEqsSTCs());
  bLayout->addWidget(cbReportNormalEqsSta);

  cbReportNormalEqsSou = new QCheckBox("Sources", bgSolutionReports);
  cbReportNormalEqsSou-> setMinimumSize(cbReportNormalEqsSou->sizeHint());
  cbReportNormalEqsSou-> setChecked(Cfg->reportNormalEqsSRCs());
  bLayout->addWidget(cbReportNormalEqsSou);

  cbReportNormalEqsEOP = new QCheckBox("EOPs", bgSolutionReports);
  cbReportNormalEqsEOP-> setMinimumSize(cbReportNormalEqsEOP->sizeHint());
  cbReportNormalEqsEOP-> setChecked(Cfg->reportNormalEqsEOPs());
  bLayout->addWidget(cbReportNormalEqsEOP);
  bLayout->addStretch(1);


  //--
  bLayout = new QHBoxLayout(aLayout);

  cbReportTroposhere = new QCheckBox("Tropospheric parameters", bgSolutionReports);
  cbReportTroposhere-> setMinimumSize(cbReportTroposhere->sizeHint());
  cbReportTroposhere-> setChecked(Cfg->reportTroposhere());
  bLayout->addWidget(cbReportTroposhere);

  QLabel *label = new QLabel("using sampling interval: ", bgSolutionReports);
  label->setMinimumSize(label->sizeHint());
  bLayout->addWidget(label);
  
  leTropSamplInterval = new QLineEdit(QString().sprintf("%.1f", Cfg->tropSamplInterval()), bgSolutionReports);
  leTropSamplInterval->setMinimumWidth(leTropSamplInterval->fontMetrics().width("3600.00") + 10);
  bLayout->addWidget(leTropSamplInterval);

  label = new QLabel("sec, starting on", bgSolutionReports);
  label->setMinimumSize(label->sizeHint());
  bLayout->addWidget(label);

  leTropStartSec = new QLineEdit(QString().sprintf("%d", Cfg->tropStartSec()), bgSolutionReports);
  leTropStartSec->setMinimumWidth(leTropStartSec->fontMetrics().width("3600") + 10);
  bLayout->addWidget(leTropStartSec);

  label = new QLabel("sec", bgSolutionReports);
  label->setMinimumSize(label->sizeHint());
  bLayout->addWidget(label);

  bLayout->addStretch(1);
  //--
  
  
  cbReportTRF_Changes = new QCheckBox("Variations of coordinates of Stations", bgSolutionReports);
  cbReportTRF_Changes-> setMinimumSize(cbReportTRF_Changes->sizeHint());
  cbReportTRF_Changes-> setChecked(Cfg->reportTRF_Changes());
  aLayout->addWidget(cbReportTRF_Changes);

  cbReportCRF_Changes = new QCheckBox("Variations of coordinates of Sources", bgSolutionReports);
  cbReportCRF_Changes-> setMinimumSize(cbReportCRF_Changes->sizeHint());
  cbReportCRF_Changes-> setChecked(Cfg->reportCRF_Changes());
  aLayout->addWidget(cbReportCRF_Changes);

  
  aLayout->activate();
  Layout->addWidget(bgSolutionReports);
  Layout->addStretch(1);

  Layout->activate();
  return W;
};

void SBModelEditor::acquireData()
{
  bool				Is;
  double			f;
  int				l;
  SBConfig::MSysTransform	SysTransform;
  SBConfig::MEphModel		Ephem;
  SBConfig::MSolidTideLd	STideLd;
  SBConfig::MSolidTideDegree	STideDeg;
  SBConfig::MEOPDiurnal		EOPdiurn;
  SBConfig::MEOPDiurnalAtm	EOPdiurnAtm;
  SBConfig::MEOPDiurnalNonTidal	EOPdiurnNonTidal;
  SBConfig::MNutModel		NutModel;
  SBConfig::MNutEqEquinox	NutEquinox;
  SBConfig::MPlateMotion	PlateMotion;
  SBConfig::MRefrDryZenith	DryZenith;
  SBConfig::MRefrWetZenith	WetZenith;
  SBConfig::MRefrDryMapping	DryMap;
  SBConfig::MRefrWetMapping	WetMap;

  /* System transformation: */
  if (rbSysTransform[0]->isChecked()) SysTransform = SBConfig::STR_Classic;
  if (rbSysTransform[1]->isChecked()) SysTransform = SBConfig::STR_NRO;
  if (SysTransform!=Cfg->sysTransform())
    {
      Cfg->setSysTransform(SysTransform);
      isModified = TRUE;
    };

  if (!isGlobal)
    {
      if ((Is=cbUseAltStations->isChecked())!=Cfg->useAltStations())
	{
	  Cfg->setUseAltStations(Is);
	  isModified = TRUE;
	};
      if ((Is=cbUseAltSources->isChecked())!=Cfg->useAltSources())
	{
	  Cfg->setUseAltSources(Is);
	  isModified = TRUE;
	};
      if ((Is=cbUseAltEOPs->isChecked())!=Cfg->useAltEOPs())
	{
	  Cfg->setUseAltEOPs(Is);
	  isModified = TRUE;
	};
      
      Str=leAltStations->text().copy();
      if (Str!=Cfg->altStations())
	{
	  isModified = TRUE;
	  Cfg->setAltStations(Str);
	};
      if ((Str=leAltSources->text())!=Cfg->altSources())
	{
	  isModified = TRUE;
	  Cfg->setAltSources(Str);
	};
      if ((Str=leAltEOPs->text())!=Cfg->altEOPs())
	{
	  isModified = TRUE;
	  Cfg->setAltEOPs(Str);
	};
    };
  
  /* Light Propagation: */
  if (rbEphem[0]->isChecked()) Ephem = SBConfig::EM_LEDE200;
  if (rbEphem[1]->isChecked()) Ephem = SBConfig::EM_LEDE403;
  if (rbEphem[2]->isChecked()) Ephem = SBConfig::EM_LEDE405;
  if (Ephem!=Cfg->ephemModel())
    {
      Cfg->setEphemModel(Ephem);
      isModified = TRUE;
    };

  if ((Is=cbIsHO->isChecked())!=Cfg->isGravDelayHO())
    {
      Cfg->setIsGravDelayHO(Is);
      isModified = TRUE;
    };

  Str=leHOThreshold->text();
  f = Str.toDouble(&Is)*DEG2RAD;
  if (Is && f!=Cfg->gravHOThreshold())
    {
      isModified = TRUE;
      Cfg->setGravHOThreshold(f);
    };
  
  for (int i=0; i<11; i++)
    {
      if ((Is=cbBody[i]->isChecked())!=Cfg->isEphBody(i))
	{
	  Cfg->setIsEphBody(i, Is);
	  isModified = TRUE;
	};
    };

  /* Solid tide: */
  if (rbSTide[0]->isChecked()) STideLd = SBConfig::ST_IERS92;
  if (rbSTide[1]->isChecked()) STideLd = SBConfig::ST_IERS96;
  if (STideLd!=Cfg->solidTideLd())
    {
      Cfg->setSolidTideLd(STideLd);
      isModified = TRUE;
    };

  if ((Is=cbPTide->isChecked())!=Cfg->isRemovePermTide())
    {
      Cfg->setIsRemovePermTide(Is);
      isModified = TRUE;
    };

  if (rbDegree[0]->isChecked()) STideDeg = SBConfig::STDEGREE_2;
  if (rbDegree[1]->isChecked()) STideDeg = SBConfig::STDEGREE_3;
  if (STideDeg!=Cfg->solidTideDegree())
    {
      Cfg->setSolidTideDegree(STideDeg);
      isModified = TRUE;
    };

  if ((Is=cbAnelastic->isChecked())!=Cfg->isSolidTideAnelastic())
    {
      Cfg->setIsSolidTideAnelastic(Is);
      isModified = TRUE;
    };

  if ((Is=cbLatDepend->isChecked())!=Cfg->isSolidTideLatDepend())
    {
      Cfg->setIsSolidTideLatDepend(Is);
      isModified = TRUE;
    };

  /* EOPs: */
  if (rbEOPDiurn[0]->isChecked()) EOPdiurn = SBConfig::EDM_NONE;
  if (rbEOPDiurn[1]->isChecked()) EOPdiurn = SBConfig::EDM_Ray1995;
  if (rbEOPDiurn[2]->isChecked()) EOPdiurn = SBConfig::EDM_JPL92;
  if (rbEOPDiurn[3]->isChecked()) EOPdiurn = SBConfig::EDM_HD1994;
  if (rbEOPDiurn[4]->isChecked()) EOPdiurn = SBConfig::EDM_GSFC95;
  if (rbEOPDiurn[5]->isChecked()) EOPdiurn = SBConfig::EDM_Gipson1995;

  if (rbEOPDiurnAtm[0]->isChecked()) EOPdiurnAtm = SBConfig::EDMA_NONE;
  if (rbEOPDiurnAtm[1]->isChecked()) EOPdiurnAtm = SBConfig::EDMA_BBP;

  if (rbEOPDiurnNonTidal[0]->isChecked()) EOPdiurnNonTidal = SBConfig::EDMNT_NONE;
  if (rbEOPDiurnNonTidal[1]->isChecked()) EOPdiurnNonTidal = SBConfig::EDMNT_Viron;

  if (EOPdiurn!=Cfg->eopDiurnal())
    {
      Cfg->setEopDiurnal(EOPdiurn);
      isModified = TRUE;
    };

  if (EOPdiurnAtm!=Cfg->eopDiurnalAtm())
    {
      Cfg->setEopDiurnalAtm(EOPdiurnAtm);
      isModified = TRUE;
    };

  if (EOPdiurnNonTidal!=Cfg->eopDiurnalNonTidal())
    {
      Cfg->setEopDiurnalNonTidal(EOPdiurnNonTidal);
      isModified = TRUE;
    };

  if ((Is=cbEOP_IAU2K_XY->isChecked())!=Cfg->isEOP_IAU2K_XY())
    {
      Cfg->setIsEOP_IAU2K_XY(Is);
      isModified = TRUE;
    };
  if ((Is=cbEOPResrerved_1->isChecked())!=Cfg->isEOPResrerved_1())
    {
      Cfg->setIsEOPResrerved_1(Is);
      isModified = TRUE;
    };
  Str=leEOPNInterp->text();
  l = Str.toInt(&Is);
  if (Is && l!=Cfg->eopNumIntrplPoints())
    {
      isModified = TRUE;
      Cfg->setEOPNumIntrplPoints(l);
    };
  

  /* Nutation: */
  if (rbNutModel[0]->isChecked()) NutModel = SBConfig::NM_IAU1980;
  if (rbNutModel[1]->isChecked()) NutModel = SBConfig::NM_IAU1996;
  if (rbNutModel[2]->isChecked()) NutModel = SBConfig::NM_IAU2000;
  if (NutModel!=Cfg->nutModel())
    {
      Cfg->setNutModel(NutModel);
      isModified = TRUE;
    };

  if (rbNutEquinox[0]->isChecked()) NutEquinox = SBConfig::NEE_IERS92;
  if (rbNutEquinox[1]->isChecked()) NutEquinox = SBConfig::NEE_SOVERS;
  if (rbNutEquinox[2]->isChecked()) NutEquinox = SBConfig::NEE_IERS96;
  if (NutEquinox!=Cfg->nutEquinox())
    {
      Cfg->setNutEquinox(NutEquinox);
      isModified = TRUE;
    };

  if ((Is=cbNutGeod->isChecked())!=Cfg->isNeedGeodNut())
    {
      Cfg->setIsNeedGeodNut(Is);
      isModified = TRUE;
    };

  /* Station Motion: */
  if (rbTPMModel[0]->isChecked()) PlateMotion = SBConfig::PM_NNR_NUVEL1A;
  if (rbTPMModel[1]->isChecked()) PlateMotion = SBConfig::PM_NUVEL1A;
  if (rbTPMModel[2]->isChecked()) PlateMotion = SBConfig::PM_NONE;
  if (PlateMotion!=Cfg->plateMotion())
    {
      Cfg->setPlateMotion(PlateMotion);
      isModified = TRUE;
    };

  if ((Is=cbStatVel->isChecked())!=Cfg->isUseStationVelocity())
    {
      Cfg->setIsUseStationVelocity(Is);
      isModified = TRUE;
    };
  
  if ((Is=cbRB2Plates->isChecked())!=Cfg->isRollBack2Plates())
    {
      Cfg->setIsRollBack2Plates(Is);
      isModified = TRUE;
    };

  if ((Is=cbAplo->isChecked())!=Cfg->isUseAploEphem())
    {
      Cfg->setIsUseAploEphem(Is);
      isModified = TRUE;
    };
  

  /* Refraction: */
  if (rbDryZenith[0]->isChecked()) DryZenith = SBConfig::RDZ_NONE;
  if (rbDryZenith[1]->isChecked()) DryZenith = SBConfig::RDZ_Saastamoinen;
  if (rbDryZenith[2]->isChecked()) DryZenith = SBConfig::RDZ_Hopfield;
  if (DryZenith!=Cfg->refrDryZenith())
    {
      Cfg->setRefrDryZenith(DryZenith);
      isModified = TRUE;
    };
  
  if (rbWetZenith[0]->isChecked()) WetZenith = SBConfig::RWZ_NONE;
  if (rbWetZenith[1]->isChecked()) WetZenith = SBConfig::RWZ_Saastamoinen;
  if (rbWetZenith[2]->isChecked()) WetZenith = SBConfig::RWZ_Hopfield;
  if (WetZenith!=Cfg->refrWetZenith())
    {
      Cfg->setRefrWetZenith(WetZenith);
      isModified = TRUE;
    };

  if (rbDryMap[0]->isChecked()) DryMap = SBConfig::RDM_NONE;
  if (rbDryMap[1]->isChecked()) DryMap = SBConfig::RDM_Chao;
  if (rbDryMap[2]->isChecked()) DryMap = SBConfig::RDM_CfA2p2;
  if (rbDryMap[3]->isChecked()) DryMap = SBConfig::RDM_Ifadis;
  if (rbDryMap[4]->isChecked()) DryMap = SBConfig::RDM_MTT;
  if (rbDryMap[5]->isChecked()) DryMap = SBConfig::RDM_NMFH2;
  if (DryMap!=Cfg->refrDryMapping())
    {
      Cfg->setRefrDryMapping(DryMap);
      isModified = TRUE;
    };
  
  if (rbWetMap[0]->isChecked()) WetMap = SBConfig::RWM_NONE;
  if (rbWetMap[1]->isChecked()) WetMap = SBConfig::RWM_Chao;
  if (rbWetMap[2]->isChecked()) WetMap = SBConfig::RWM_Ifadis;
  if (rbWetMap[3]->isChecked()) WetMap = SBConfig::RWM_MTT;
  if (rbWetMap[4]->isChecked()) WetMap = SBConfig::RDM_NMFW2;
  if (rbWetMap[5]->isChecked()) WetMap = SBConfig::RWM_NONE;
  if (WetMap!=Cfg->refrWetMapping())
    {
      Cfg->setRefrWetMapping(WetMap);
      isModified = TRUE;
    };

  if ((Is=cbAxissAlt->isChecked())!=Cfg->isAltAxsCorr())
    {
      Cfg->setIsAltAxsCorr(Is);
      isModified = TRUE;
    };

  if ((Is=cbRB2NMF->isChecked())!=Cfg->isRollBack2NMF())
    {
      Cfg->setIsRollBack2NMF(Is);
      isModified = TRUE;
    };

  /* Data processing: */
  if ((Is=cbUseDelay->isChecked())!=Cfg->isUseDelay())
    {
      Cfg->setIsUseDelay(Is);
      isModified = TRUE;
    };
  if ((Is=cbUseRate->isChecked())!=Cfg->isUseRate())
    {
      Cfg->setIsUseRate(Is);
      isModified = TRUE;
    };
  if ((Is=cbUseBadIon->isChecked())!=Cfg->isUseBadIon())
    {
      Cfg->setIsUseBadIon(Is);
      isModified = TRUE;
    };
  l = eDelayQThreshold->text().toInt(&Is);
  if (Is && l!=Cfg->qCodeThreshold())
    {
      isModified = TRUE;
      Cfg->setQCodeThreshold(l);
    };
  l = eRateQThreshold->text().toInt(&Is);
  if (Is && l!=Cfg->dUFlagThreshold())
    {
      isModified = TRUE;
      Cfg->setDUFlagThreshold(l);
    };

  /* Run-Time options*/
  if ((Is=cbGenerateDSINEXes->isChecked())!=Cfg->isGenerateDSINEXes())
    {
      Cfg->setIsGenerateDSINEXes(Is);
      isModified = TRUE;
    };

  /* Solution reports: */
  if ((Is=cbReportMaps->isChecked())!=Cfg->reportMaps())
    {
      Cfg->setReportMaps(Is);
      isModified = TRUE;
    };
  if ((Is=cbReportTRF_Changes->isChecked())!=Cfg->reportTRF_Changes())
    {
      Cfg->setReportTRF_Changes(Is);
      isModified = TRUE;
    };
  if ((Is=cbReportCRF_Changes->isChecked())!=Cfg->reportCRF_Changes())
    {
      Cfg->setReportCRF_Changes(Is);
      isModified = TRUE;
    };
  if ((Is=cbReportCRF->isChecked())!=Cfg->reportCRF())
    {
      Cfg->setReportCRF(Is);
      isModified = TRUE;
    };
  if ((Is=cbReportTRF_SINEX->isChecked())!=Cfg->reportTRF_SINEX())
    {
      Cfg->setReportTRF_SINEX(Is);
      isModified = TRUE;
    };
  if ((Is=cbReportEOP->isChecked())!=Cfg->reportEOP())
    {
      Cfg->setReportEOP(Is);
      isModified = TRUE;
    };
  if ((Is=cbReportTechDesc->isChecked())!=Cfg->reportTechDesc())
    {
      Cfg->setReportTechDesc(Is);
      isModified = TRUE;
    };
  if ((Is=cbReportNormalEqs->isChecked())!=Cfg->reportNormalEqs())
    {
      Cfg->setReportNormalEqs(Is);
      isModified = TRUE;
    };
  if ((Is=cbReportNormalEqsSta->isChecked())!=Cfg->reportNormalEqsSTCs())
    {
      Cfg->setReportNormalEqsSTCs(Is);
      isModified = TRUE;
    };
  if ((Is=cbReportNormalEqsSou->isChecked())!=Cfg->reportNormalEqsSRCs())
    {
      Cfg->setReportNormalEqsSRCs(Is);
      isModified = TRUE;
    };
  if ((Is=cbReportNormalEqsEOP->isChecked())!=Cfg->reportNormalEqsEOPs())
    {
      Cfg->setReportNormalEqsEOPs(Is);
      isModified = TRUE;
    };

  if ((Is=cbReportTroposhere->isChecked())!=Cfg->reportTroposhere())
    {
      Cfg->setReportTroposhere(Is);
      isModified = TRUE;
    };

  Str=leTropSamplInterval->text();
  f = Str.toDouble(&Is);
  if (Is && f!=Cfg->tropSamplInterval())
    {
      isModified = TRUE;
      Cfg->setTropSamplInterval(f);
    };
  Str=leTropStartSec->text();
  l = Str.toInt(&Is);
  if (Is && l!=Cfg->tropStartSec())
    {
      isModified = TRUE;
      Cfg->setTropStartSec(l);
    };

};
/*==============================================================================================*/




/*==============================================================================================*/
/*												*/
/* SBParametersEditor..										*/
/*												*/
/*==============================================================================================*/
SBParametersEditor::SBParametersEditor(SBParametersDesc* P_,
				       QWidget* parent, const char* name, bool modal, WFlags f)
  : SBTabDialog(parent, name, modal, f)
{
  numOpenedWindows = 0;
  isModified = FALSE;
  if (P_)
    {
      Project = NULL;
      P = new SBParametersDesc(*P_);
      isGlobal = TRUE;
    }
  else
    {
      Project = SetUp->loadProject();
      P = new SBParametersDesc(Project->p());
      isGlobal = FALSE;
    };
  
  setCaption(isGlobal?QString("Global set"):"Estimates for the project: " + Project->name());
  
  addTab(wStationParameters(),	"Station");
  addTab(wSourceParameters(),	"Sources");
  addTab(wEOPParameters(),	"EOPs");
  addTab(wOtherParameters(),	"Other");
  addTab(wTestParameters(),	"Test");

  setCancelButton();
  setApplyButton();
  setDefaultButton("Defaults");
  connect(this, SIGNAL(applyButtonPressed()), SLOT(makeApply()));
  connect(this, SIGNAL(defaultButtonPressed()), SLOT(makeDefault()));
};

SBParametersEditor::~SBParametersEditor()
{
  if (isModified)
    {
      if (isGlobal) Config.setP(*P);
      else
	{
	  Project->setP(*P);
	  SetUp->saveProject(Project);
	};
    };
  if (P) delete P;
  if (Project) delete Project;
};

void SBParametersEditor::makeApply()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": applying");
  acquireData();
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": done");
};

void SBParametersEditor::makeDefault()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": defaulting");
  *P = DefaultConfig.p();
  isModified = TRUE;
  browseData();
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": done");
};

void SBParametersEditor::browseData()
{
  for (int i=0; i<SBParametersDesc::n(); i++)
    RB[i][(int)P->par(i).type()]->setChecked(TRUE);
};

void SBParametersEditor::acquireData()
{
  SBParameterCfg	p;
  for (int i=0; i<SBParametersDesc::n(); i++)
    {
      if (RB[i][0]->isChecked()) p.setType(SBParameterCfg::PT_NONE);
      if (RB[i][1]->isChecked()) p.setType(SBParameterCfg::PT_GLB);
      if (RB[i][2]->isChecked()) p.setType(SBParameterCfg::PT_ARC);
      if (RB[i][3]->isChecked()) p.setType(SBParameterCfg::PT_LOC);
      if (RB[i][4]->isChecked()) p.setType(SBParameterCfg::PT_STH);
      if (p.type()!=P->par(i).type())
	{
	  P->setType(i, p.type());
	  isModified = TRUE;
	};
    };
};

QWidget* SBParametersEditor::wStationParameters()
{
  const int NumOfStPars = 12;

  QGroupBox	*gbox;
  QGridLayout	*grid;
  QBoxLayout	*Layout;
  QLabel*	label;

  QWidget	*W = new QWidget(this);
  Layout = new QVBoxLayout(W, 20, 10);

  gbox = new QGroupBox("Station Parameters:", W);
  grid = new QGridLayout(gbox,  NumOfStPars+1/*for alignment*/+1,8,  20,10);
  
  label = new QLabel("Parameter", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, 0, 0);

  label = new QLabel("N", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, 0, 1);

  label = new QLabel("G", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, 0, 2);

  label = new QLabel("A", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, 0, 3);

  label = new QLabel("L", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, 0, 4);

  label = new QLabel("S", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, 0, 5);

  label = new QLabel("Options", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, 0, 7);
  
  
  //
  QPushButton* btn[NumOfStPars];
  const int StParIdx[NumOfStPars] = {0,1,2,3,5,6,7,8,9,15,16,17};

  for (int i=0; i<NumOfStPars; i++)
    {
      label = new QLabel(P->par(StParIdx[i]).name().copy(), gbox);
      label->setMinimumSize(label->sizeHint());
      grid->addWidget(label, i+1, 0);

      QButtonGroup* bg = new QButtonGroup(gbox);
      QBoxLayout* aLayout = new QHBoxLayout(bg, 0, 5);
      for (int j=0; j<5; j++)
	{
	  RB[StParIdx[i]][j] = new QRadioButton(bg);
	  RB[StParIdx[i]][j]-> setMinimumSize(RB[StParIdx[i]][j]->sizeHint());
	  aLayout->addWidget(RB[StParIdx[i]][j]);
	};
      RB[StParIdx[i]][(int)P->par(StParIdx[i]).type()]->setChecked(TRUE);
      
      aLayout->activate();
      bg->setFrameStyle(QFrame::NoFrame);
      grid->addMultiCellWidget(bg, i+1,i+1, 1,5);
      
      btn[i] = new QPushButton("Edit", gbox);
      btn[i]->setMinimumSize(btn[i]->sizeHint());
      grid->addWidget(btn[i], i+1, 7);
    };

  connect(btn[ 0], SIGNAL(clicked()), SLOT(editPar__0()));
  connect(btn[ 1], SIGNAL(clicked()), SLOT(editPar__1()));
  connect(btn[ 2], SIGNAL(clicked()), SLOT(editPar__2()));
  connect(btn[ 3], SIGNAL(clicked()), SLOT(editPar__3()));
  connect(btn[ 4], SIGNAL(clicked()), SLOT(editPar__5()));
  connect(btn[ 5], SIGNAL(clicked()), SLOT(editPar__6()));
  connect(btn[ 6], SIGNAL(clicked()), SLOT(editPar__7()));
  connect(btn[ 7], SIGNAL(clicked()), SLOT(editPar__8()));
  connect(btn[ 8], SIGNAL(clicked()), SLOT(editPar__9()));
  connect(btn[ 9], SIGNAL(clicked()), SLOT(editPar_15()));
  connect(btn[10], SIGNAL(clicked()), SLOT(editPar_16()));
  connect(btn[11], SIGNAL(clicked()), SLOT(editPar_17()));

  grid->setColStretch(0, 1);
  grid->setRowStretch(NumOfStPars+1, 1);
  grid->addColSpacing(6, btn[0]->sizeHint().width()/2);
  grid->addRowSpacing(0, label->sizeHint().height());
  grid->activate();

  Layout->addWidget(gbox);
  Layout->activate();
  
  return W;
};

QWidget* SBParametersEditor::wSourceParameters()
{
  const int NumOfSoPars = 1;

  QGroupBox	*gbox;
  QGridLayout	*grid;
  QBoxLayout	*Layout;
  QLabel*	label;

  QWidget	*W = new QWidget(this);
  Layout = new QVBoxLayout(W, 20, 10);

  gbox = new QGroupBox("Source Parameters:", W);
  grid = new QGridLayout(gbox,  NumOfSoPars+1/*for alignment*/+1,8,  20,10);
  
  label = new QLabel("Parameter", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, 0, 0);

  label = new QLabel("N", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, 0, 1);

  label = new QLabel("G", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, 0, 2);

  label = new QLabel("A", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, 0, 3);

  label = new QLabel("L", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, 0, 4);

  label = new QLabel("S", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, 0, 5);

  label = new QLabel("Options", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, 0, 7);
  
  
  //
  QPushButton* btn[NumOfSoPars];
  const int SoParIdx[NumOfSoPars] = {4};

  for (int i=0; i<NumOfSoPars; i++)
    {
      label = new QLabel(P->par(SoParIdx[i]).name().copy(), gbox);
      label->setMinimumSize(label->sizeHint());
      grid->addWidget(label, i+1, 0);

      QButtonGroup* bg = new QButtonGroup(gbox);
      QBoxLayout* aLayout = new QHBoxLayout(bg, 0, 5);
      for (int j=0; j<5; j++)
	{
	  RB[SoParIdx[i]][j] = new QRadioButton(bg);
	  RB[SoParIdx[i]][j]-> setMinimumSize(RB[SoParIdx[i]][j]->sizeHint());
	  aLayout->addWidget(RB[SoParIdx[i]][j]);
	};
      RB[SoParIdx[i]][(int)P->par(SoParIdx[i]).type()]->setChecked(TRUE);
      
      aLayout->activate();
      bg->setFrameStyle(QFrame::NoFrame);
      grid->addMultiCellWidget(bg, i+1,i+1, 1,5);
      
      btn[i] = new QPushButton("Edit", gbox);
      btn[i]->setMinimumSize(btn[i]->sizeHint());
      grid->addWidget(btn[i], i+1, 7);
    };

  connect(btn[ 0], SIGNAL(clicked()), SLOT(editPar__4()));

  grid->setColStretch(0, 1);
  grid->setRowStretch(NumOfSoPars+1, 2);
  grid->addColSpacing(6, btn[0]->sizeHint().width()/2);
  grid->addRowSpacing(0, label->sizeHint().height());
  grid->activate();

  Layout->addWidget(gbox);
  Layout->activate();
  
  return W;
};

QWidget* SBParametersEditor::wEOPParameters()
{
  const int NumOfEOPPars = 3;

  QGroupBox	*gbox;
  QGridLayout	*grid;
  QBoxLayout	*Layout;
  QLabel*	label;

  QWidget	*W = new QWidget(this);
  Layout = new QVBoxLayout(W, 20, 10);

  gbox = new QGroupBox("EOP Parameters:", W);
  grid = new QGridLayout(gbox,  NumOfEOPPars+1/*for alignment*/+1,8,  20,10);
  
  label = new QLabel("Parameter", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, 0, 0);

  label = new QLabel("N", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, 0, 1);

  label = new QLabel("G", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, 0, 2);

  label = new QLabel("A", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, 0, 3);

  label = new QLabel("L", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, 0, 4);

  label = new QLabel("S", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, 0, 5);

  label = new QLabel("Options", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, 0, 7);
  
  
  //
  QPushButton* btn[NumOfEOPPars];
  const int EOPParIdx[NumOfEOPPars] = {10,11,12};

  for (int i=0; i<NumOfEOPPars; i++)
    {
      label = new QLabel(P->par(EOPParIdx[i]).name().copy(), gbox);
      label->setMinimumSize(label->sizeHint());
      grid->addWidget(label, i+1, 0);

      QButtonGroup* bg = new QButtonGroup(gbox);
      QBoxLayout* aLayout = new QHBoxLayout(bg, 0, 5);
      for (int j=0; j<5; j++)
	{
	  RB[EOPParIdx[i]][j] = new QRadioButton(bg);
	  RB[EOPParIdx[i]][j]-> setMinimumSize(RB[EOPParIdx[i]][j]->sizeHint());
	  aLayout->addWidget(RB[EOPParIdx[i]][j]);
	};
      RB[EOPParIdx[i]][(int)P->par(EOPParIdx[i]).type()]->setChecked(TRUE);
      
      aLayout->activate();
      bg->setFrameStyle(QFrame::NoFrame);
      grid->addMultiCellWidget(bg, i+1,i+1, 1,5);
      
      btn[i] = new QPushButton("Edit", gbox);
      btn[i]->setMinimumSize(btn[i]->sizeHint());
      grid->addWidget(btn[i], i+1, 7);
    };

  connect(btn[ 0], SIGNAL(clicked()), SLOT(editPar_10()));
  connect(btn[ 1], SIGNAL(clicked()), SLOT(editPar_11()));
  connect(btn[ 2], SIGNAL(clicked()), SLOT(editPar_12()));

  grid->setColStretch(0, 1);
  grid->setRowStretch(NumOfEOPPars+1, 2);
  grid->addColSpacing(6, btn[0]->sizeHint().width()/2);
  grid->addRowSpacing(0, label->sizeHint().height());
  grid->activate();

  Layout->addWidget(gbox);
  Layout->activate();
  
  return W;
};

QWidget* SBParametersEditor::wOtherParameters()
{
  const int NumOfOthPars = 1;

  QGroupBox	*gbox;
  QGridLayout	*grid;
  QBoxLayout	*Layout;
  QLabel*	label;

  QWidget	*W = new QWidget(this);
  Layout = new QVBoxLayout(W, 20, 10);

  gbox = new QGroupBox("Test Parameters:", W);
  grid = new QGridLayout(gbox,  NumOfOthPars+1/*for alignment*/+1,8,  20,10);
  
  label = new QLabel("Parameter", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, 0, 0);

  label = new QLabel("N", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, 0, 1);

  label = new QLabel("G", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, 0, 2);

  label = new QLabel("A", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, 0, 3);

  label = new QLabel("L", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, 0, 4);

  label = new QLabel("S", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, 0, 5);

  label = new QLabel("Options", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, 0, 7);
  
  
  //
  QPushButton* btn[NumOfOthPars];
  const int OthParIdx[NumOfOthPars] = {14};

  for (int i=0; i<NumOfOthPars; i++)
    {
      label = new QLabel(P->par(OthParIdx[i]).name().copy(), gbox);
      label->setMinimumSize(label->sizeHint());
      grid->addWidget(label, i+1, 0);

      QButtonGroup* bg = new QButtonGroup(gbox);
      QBoxLayout* aLayout = new QHBoxLayout(bg, 0, 5);
      for (int j=0; j<5; j++)
	{
	  RB[OthParIdx[i]][j] = new QRadioButton(bg);
	  RB[OthParIdx[i]][j]-> setMinimumSize(RB[OthParIdx[i]][j]->sizeHint());
	  aLayout->addWidget(RB[OthParIdx[i]][j]);
	};
      RB[OthParIdx[i]][(int)P->par(OthParIdx[i]).type()]->setChecked(TRUE);
      
      aLayout->activate();
      bg->setFrameStyle(QFrame::NoFrame);
      grid->addMultiCellWidget(bg, i+1,i+1, 1,5);
      
      btn[i] = new QPushButton("Edit", gbox);
      btn[i]->setMinimumSize(btn[i]->sizeHint());
      grid->addWidget(btn[i], i+1, 7);
    };

  connect(btn[ 0], SIGNAL(clicked()), SLOT(editPar_14()));

  grid->setColStretch(0, 1);
  grid->setRowStretch(NumOfOthPars+1, 2);
  grid->addColSpacing(6, btn[0]->sizeHint().width()/2);
  grid->addRowSpacing(0, label->sizeHint().height());
  grid->activate();

  Layout->addWidget(gbox);
  Layout->activate();
  
  return W;
};

QWidget* SBParametersEditor::wTestParameters()
{
  const int NumOfTstPars = 1;

  QGroupBox	*gbox;
  QGridLayout	*grid;
  QBoxLayout	*Layout;
  QLabel*	label;

  QWidget	*W = new QWidget(this);
  Layout = new QVBoxLayout(W, 20, 10);

  gbox = new QGroupBox("Test Parameters:", W);
  grid = new QGridLayout(gbox,  NumOfTstPars+1/*for alignment*/+1,8,  20,10);
  
  label = new QLabel("Parameter", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, 0, 0);

  label = new QLabel("N", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, 0, 1);

  label = new QLabel("G", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, 0, 2);

  label = new QLabel("A", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, 0, 3);

  label = new QLabel("L", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, 0, 4);

  label = new QLabel("S", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, 0, 5);

  label = new QLabel("Options", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, 0, 7);
  
  
  //
  QPushButton* btn[NumOfTstPars];
  const int TstParIdx[NumOfTstPars] = {13};

  for (int i=0; i<NumOfTstPars; i++)
    {
      label = new QLabel(P->par(TstParIdx[i]).name().copy(), gbox);
      label->setMinimumSize(label->sizeHint());
      grid->addWidget(label, i+1, 0);

      QButtonGroup* bg = new QButtonGroup(gbox);
      QBoxLayout* aLayout = new QHBoxLayout(bg, 0, 5);
      for (int j=0; j<5; j++)
	{
	  RB[TstParIdx[i]][j] = new QRadioButton(bg);
	  RB[TstParIdx[i]][j]-> setMinimumSize(RB[TstParIdx[i]][j]->sizeHint());
	  aLayout->addWidget(RB[TstParIdx[i]][j]);
	};
      RB[TstParIdx[i]][(int)P->par(TstParIdx[i]).type()]->setChecked(TRUE);
      
      aLayout->activate();
      bg->setFrameStyle(QFrame::NoFrame);
      grid->addMultiCellWidget(bg, i+1,i+1, 1,5);
      
      btn[i] = new QPushButton("Edit", gbox);
      btn[i]->setMinimumSize(btn[i]->sizeHint());
      grid->addWidget(btn[i], i+1, 7);
    };

  connect(btn[ 0], SIGNAL(clicked()), SLOT(editPar_13()));

  grid->setColStretch(0, 1);
  grid->setRowStretch(NumOfTstPars+1, 2);
  grid->addColSpacing(6, btn[0]->sizeHint().width()/2);
  grid->addRowSpacing(0, label->sizeHint().height());
  grid->activate();

  Layout->addWidget(gbox);
  Layout->activate();
  
  return W;
};

void SBParametersEditor::ParameterModified(bool Modified)
{
  if (!numOpenedWindows) 
    std::cerr << "error in windows accounting...\n";
  numOpenedWindows--;
  if (Modified) 
    isModified = TRUE;
};

void SBParametersEditor::editParameter(int N_)
{
  SBParEditor *e =  new SBParEditor(&(SBParameterCfg&)(P->par(N_)), P->par(N_).name(), N_);
  connect (e, SIGNAL(valueModified(bool)), SLOT(ParameterModified(bool)));
  numOpenedWindows++;
  e->show();
};
/*==============================================================================================*/




/*==============================================================================================*/
/*												*/
/* SBParEditor..										*/
/*												*/
/*==============================================================================================*/
SBParEditor::SBParEditor(SBParameterCfg* P_, const QString& ParName_, int Idx_,
			 QWidget* parent, const char* name, bool modal, WFlags f)
  : QDialog(parent, name, modal, f)
{
  static const QString sText[] = 
  {
    QString("Unassigned"),
    QString("White noise"),
    QString("2-nd order Markov process"),
    QString("Random Walk"),
    QString("Unknown")
  };

  QString	Q("");
  isModified = FALSE;
  P = P_;
  Idx	= 0<=Idx_&&Idx_<SBParametersDesc::n()?Idx_:0;
  ParName=ParName_;
  setCaption(ParName_);
  
  QGroupBox	*gbox;
  QBoxLayout	*Layout = new QVBoxLayout(this, 20, 10), *SubLayout;
  QGridLayout	*grid;
  QLabel*	label;
  
  
  // Convention properties:
  gbox = new QGroupBox("Conventional parameter", this);
  grid = new QGridLayout(gbox,  2,2,  20,10);
  
  label = new QLabel("A priori (initial) sigma (" + P->scaleName() + "):", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, 0, 0);
  
  ConvApriori = new QLineEdit(gbox);
  ConvApriori->setText(Q.sprintf("%.4f",P->convAPriori()));
  ConvApriori->setMinimumSize(ConvApriori->sizeHint());
  grid->addWidget(ConvApriori, 0, 1);

  Propagate = new QCheckBox("Propagate parameter in time", gbox);
  Propagate-> setChecked(P->isPropagated());
  Propagate-> setMinimumSize(Propagate->sizeHint());
  grid-> addMultiCellWidget(Propagate, 1,1, 0,1);
  
  grid->setColStretch(0,1);
  grid->activate();
  Layout->addWidget(gbox);

  // Stochastic properties:
  gbox = new QGroupBox("Stochastic parameter", this);
  grid = new QGridLayout(gbox,  5,2,  20,10);
  
  label = new QLabel("A priori (initial) sigma (" + P->scaleName() + "):", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, 0, 0);

  label = new QLabel("Ruled White Noise (" + P->scaleName() + "/sqrt(hr)):", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, 1, 0);

  label = new QLabel("Break Noise (" + P->scaleName() + "/sqrt(hr)):", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, 2, 0);

  label = new QLabel("Time of relaxation (hr):", gbox);
  label->setMinimumSize(label->sizeHint());
  grid->addWidget(label, 3, 0);
  
  StocApriori = new QLineEdit(gbox);
  StocApriori->setText(Q.sprintf("%.4f",P->stocAPriori()));
  StocApriori->setMinimumSize(StocApriori->sizeHint());
  grid->addWidget(StocApriori, 0, 1);

  RuledWNoise = new QLineEdit(gbox);
  RuledWNoise->setText(Q.sprintf("%.4f",P->whiteNoise()));
  RuledWNoise->setMinimumSize(RuledWNoise->sizeHint());
  grid->addWidget(RuledWNoise, 1, 1);

  BreaksNoise = new QLineEdit(gbox);
  BreaksNoise->setText(Q.sprintf("%.4f",P->breakNoise()));
  BreaksNoise->setMinimumSize(BreaksNoise->sizeHint());
  grid->addWidget(BreaksNoise, 2, 1);

  Tau = new QLineEdit(gbox);
  Tau->setText(Q.sprintf("%.4f",P->tau()));
  Tau->setMinimumSize(Tau->sizeHint());
  grid->addWidget(Tau, 3, 1);

  QButtonGroup* bg = new QButtonGroup("Type of stochastic modelling", gbox);
  SubLayout = new QVBoxLayout(bg, 20, 5);
  for (int i=0; i<5; i++)
    {
      Type[i]=new QRadioButton(sText[i], bg);
      Type[i]->setMinimumSize(Type[i]->sizeHint());
      SubLayout->addWidget(Type[i]);
    };
  Type[(int)P->stochasticType()]->setChecked(TRUE);
  SubLayout->activate();
  grid->addMultiCellWidget(bg, 4,4, 0,1);

  grid->setColStretch(0,1);
  grid->activate();
  Layout->addWidget(gbox, 10);

  
  //---
  SubLayout = new QHBoxLayout(10);
  Layout -> addLayout(SubLayout);
  SubLayout-> addStretch(1);

  QPushButton *Ok	= new QPushButton("OK", this);
  Ok->setDefault(TRUE);
  QPushButton *Default	= new QPushButton("Default", this);
  QPushButton *Cancel	= new QPushButton("Cancel", this);

  QSize	BtnSize;
  Cancel->setMinimumSize((BtnSize=Cancel->sizeHint()));
  Ok->setMinimumSize(BtnSize);
  Default->setMinimumSize(BtnSize);

  SubLayout->addWidget(Ok);
  SubLayout->addWidget(Default);
  SubLayout->addWidget(Cancel);

  connect(Ok, SIGNAL(clicked()), SLOT(accept()));
  connect(Default, SIGNAL(clicked()), SLOT(makeDefault()));
  connect(Cancel, SIGNAL(clicked()), SLOT(reject()));
  Layout->activate();
};

SBParEditor::~SBParEditor()
{
  //  emit valueModified(isModified);
};
 
void SBParEditor::accept()
{
  acquireData();
  emit valueModified(isModified);
  QDialog::accept();
  delete this;
};

void SBParEditor::makeDefault()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": defaulting");
  *P = DefaultConfig.p().par(Idx);
  isModified = TRUE;
  browseData();
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": done");
};

void SBParEditor::browseData()
{
  QString	Q("");

  ConvApriori->setText(Q.sprintf("%.4f",P->convAPriori()));
  StocApriori->setText(Q.sprintf("%.4f",P->stocAPriori()));
  RuledWNoise->setText(Q.sprintf("%.4f",P->whiteNoise()));
  BreaksNoise->setText(Q.sprintf("%.4f",P->breakNoise()));
  Tau->setText(Q.sprintf("%.4f",P->tau()));
  Propagate-> setChecked(P->isPropagated());
  Type[(int)P->stochasticType()]->setChecked(TRUE);
};

void SBParEditor::acquireData()
{
  bool				isOK;
  double			d;
  QString			Q;
  SBParameterCfg::StochType	type=SBParameterCfg::ST_UNKN;
  
  Q=ConvApriori->text();
  d=Q.toDouble(&isOK);
  if (isOK && d!=P->convAPriori())
    {
      isModified=TRUE;
      P->setConvAPriori(d);
    };

  Q=StocApriori->text();
  d=Q.toDouble(&isOK);
  if (isOK && d!=P->stocAPriori())
    {
      isModified=TRUE;
      P->setStocAPriori(d);
    };

  Q=RuledWNoise->text();
  d=Q.toDouble(&isOK);
  if (isOK && d!=P->whiteNoise())
    {
      isModified=TRUE;
      P->setWhiteNoise(d);
    };

  Q=BreaksNoise->text();
  d=Q.toDouble(&isOK);
  if (isOK && d!=P->breakNoise())
    {
      isModified=TRUE;
      P->setBreakNoise(d);
    };

  Q=Tau->text();
  d=Q.toDouble(&isOK);
  if (isOK && d!=P->tau())
    {
      isModified=TRUE;
      P->setTau(d);
    };
  
  if ((isOK=Propagate->isChecked())!=P->isPropagated())
    {
      isModified=TRUE;
      P->setPropagated(isOK);
    };

  if (Type[0]->isChecked()) type=SBParameterCfg::ST_NONE;
  if (Type[1]->isChecked()) type=SBParameterCfg::ST_WHITENOISE;
  if (Type[2]->isChecked()) type=SBParameterCfg::ST_MARKOVPROCESS;
  if (Type[3]->isChecked()) type=SBParameterCfg::ST_RANDWALK;
  if (Type[4]->isChecked()) type=SBParameterCfg::ST_UNKN;
  if (type!=P->stochasticType())
    {
      isModified=TRUE;
      P->setStochasticType(type);
    };    
};
/*==============================================================================================*/
/*==============================================================================================*/



