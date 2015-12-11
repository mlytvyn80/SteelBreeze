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

#include "SbISetup.H"

#include <qapplication.h>
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qgroupbox.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qmessagebox.h> 
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qspinbox.h>
#include <qtabbar.h>
#include <qtooltip.h>
#include <qvalidator.h>

#include "SbSetup.H"





const uint LogFacilities[32]=
{
  SBLog::IO,
  SBLog::DATA,
  SBLog::TIME,
  SBLog::PRECESSION,
  SBLog::NUTATION,
  SBLog::R3,
  SBLog::PMOTION,
  SBLog::PINTERP,
  SBLog::TIDES,
  SBLog::AXISOFF,
  SBLog::PLATES,
  SBLog::REFRACT,
  SBLog::GRAVDELAY,
  SBLog::DELAYALL,
  SBLog::EPHEM,
  SBLog::ESTIMATOR,
  SBLog::CONFIG,
  SBLog::INTERFACE,
  SBLog::SOURCE,
  SBLog::STATION,
  SBLog::MATRIX3D,
  SBLog::MATRIX,
  SBLog::RUN,
  SBLog::STOCHPARS,
  SBLog::SPLINE,
  SBLog::PREPROC,
  SBLog::RES_07,
  SBLog::RES_08,
  SBLog::RES_09,
  SBLog::RES_10,
  SBLog::TEST,
  SBLog::TEMPORARY
};



/*==============================================================================================*/
/*												*/
/* SBFileConvLI..										*/
/*												*/
/*												*/
/*==============================================================================================*/
QString SBFileConvLI::text(int col) const
{
  TmpString = "NULL";
  if (Entry)
    switch (col)
      {
      case  0: TmpString = Entry->name().copy();
	break;
      case  1: TmpString = (Entry->comCompr() + "  ").copy();
	break;
      case  2: TmpString = (Entry->comUncom() + "  ").copy();
	break;
      case  3: TmpString = Entry->ext().copy();
	break;
      };
  return TmpString;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBSetupDialog..										*/
/*												*/
/*==============================================================================================*/
QString SBSetupDialog::SBInstLI::text(int col) const
{
  TmpString = "NULL";
  if (Inst)
    switch (col)
      {
      case  0: TmpString = Inst->name().copy();
	break;
      case  1: TmpString = Inst->descr().copy();
	break;
      };
  return TmpString;
};

SBSetupDialog::SBSetupDialog(QWidget* parent, const char* name, bool modal, WFlags f)
  : SBTabDialog(parent, name, modal, f)
{
  setup = new SBSetUp(*SetUp);

  InstList = new SBInstitutionList;
  *InstList = *Institutions;

  setCaption("Configure");
  
  addTab(pathes(), "Dirs && Files");
  addTab(logger(), "Logger");
  addTab(packer(), "Packers");
  addTab(misc(),   "Misc");
  addTab(institutions(), "Organizations");


  setApplyButton();
  setCancelButton();
  setDefaultButton();  

  connect(this, SIGNAL(applyButtonPressed()), SLOT(makeApply()));
  connect(this, SIGNAL(defaultButtonPressed()), SLOT(makeDefaults()));
  connect(this, SIGNAL(aboutToShow()), SLOT(setData()));
};

QWidget* SBSetupDialog::pathes()
{
  QBoxLayout*	main_layout;
  QLabel*	label;
  QGroupBox*	gbox;
  QGridLayout*	greed;
  QWidget*	wPathes;

  const char*	sLabel[19]=
  {
    "&Home, the main directory:",
    "&Data, where keep the data:",
    "&Compile, the input data files:",
    "&Stuff, databases, models, etc.:", 
    "&Output:", 
    "&Solutions:", 
    "Pro&ject, project files in:", 
    "&Log, the log output:", 
    "&Work, for temporary files:", 
    "&Import, general data:", 
    "&Import EOP:", 
    "&Import TRF:", 
    "&Import CRF:",
    "&Import Ocean Loading coef.s:", 
    "&Import Mater files:", 
    "&Import APLo Ephemerides:", 
    //files:
    "S&tations database:", 
    "S&ources database:", 
    "&EOPs database:"
  };
  
  wPathes	= new QWidget(this);
  main_layout	= new QVBoxLayout(wPathes, 10);
  main_layout -> addStretch(2);
  gbox		= new QGroupBox("Directories", wPathes);
  greed		= new QGridLayout(gbox, 15, 2, 22, 2);
  for (int i=0; i<16; i++)
    {
      PathLE[i]	= new QLineEdit(gbox);
      label	= new QLabel(PathLE[i], sLabel[i], gbox);
      PathLE[i]	-> setMinimumHeight(PathLE[i]->sizeHint().height());
      PathLE[i] -> setMinimumWidth(15*PathLE[i]->fontMetrics().width("WW"));
      label	-> setMinimumSize(label->sizeHint());
      greed-> addWidget(label,     i, 0);
      greed-> addWidget(PathLE[i], i, 1);
    };
  greed	-> activate();
  main_layout -> addWidget(gbox);
  main_layout -> addStretch(1);
  
  gbox		= new QGroupBox("Stuff files", wPathes);
  greed		= new QGridLayout(gbox, 3, 2, 22, 2);
  for (int i=16; i<19; i++)
    {
      PathLE[i]	= new QLineEdit(gbox);
      label	= new QLabel(PathLE[i], sLabel[i], gbox);
      PathLE[i]	-> setMinimumSize(PathLE[i]->sizeHint());
      label	-> setMinimumSize(label->sizeHint());
      greed-> addWidget(label,     i-14, 0);
      greed-> addWidget(PathLE[i], i-14, 1);
    };
  greed	-> activate();
  main_layout -> addWidget(gbox);
  main_layout -> addStretch(3);

  main_layout-> activate();
  return wPathes;
};

QWidget* SBSetupDialog::logger()
{
  QBoxLayout*	main_layout;
  QLabel*	label;
  QGroupBox*	gbox;
  QGridLayout*	greed;
  QWidget*	wLogger;

  const char*	sFacility[32]=
  {
    "Input/Output",
    "Data preprocessing",
    "Time calculations",
    "Precession matrix",
    "Nutation matrix",
    "CRS-to-TRS matrix",
    "Polar motion",
    "Polar interpolation ",
    "Tide calculations",
    "Axis offset",
    "Plate motion",
    "Refraction calcs",
    "Gravitation delay ",
    "Total delay",
    "Ephemerides",
    "Estimation",
    "Configuration",
    "Interface",
    "Source manipulation ",
    "Station manipulatin ",
    "General Matrix",
    "3D Matrix",
    "General Executor",
    "Stochastic Parameters",
    "Spline Interpolation",
    "Pre Processing",
    "Reserved #07",
    "Reserved #08",
    "Reserved #09",
    "Reserved #10",
    "Test",
    "Temporary"
  };
  const char* ttstr[4]= {"on ERROR","on NORMAL","on INFO","on DEBUG"};

  wLogger = new QWidget(this);
  main_layout	= new QVBoxLayout(wLogger, 10);
  main_layout	-> addStretch(1);


  //---
  gbox 	= new QGroupBox("Log Parameters", wLogger);
  greed	= new QGridLayout(gbox, 2, 4, 20, 6);

  eLogFileName	= new QLineEdit(gbox);
  eLogFileName	-> setText("1234567890");
  label		= new QLabel(eLogFileName, "Log &file name:", gbox);
  label		-> setMinimumSize(label->sizeHint());
  eLogFileName	-> setMinimumSize(eLogFileName->sizeHint());

  greed -> addWidget(label, 0, 0);
  greed -> addWidget(eLogFileName, 0, 1);

  sLogCapacity	= new QSpinBox(100,10000,100,gbox);
  sLogCapacity	-> setSuffix(" lines");
  label		= new QLabel(sLogCapacity, "Log &capacity:", gbox);
  label		-> setMinimumSize(label->sizeHint());
  sLogCapacity	-> setMinimumSize(sLogCapacity->sizeHint());
  greed -> addWidget(label, 1, 0);
  greed -> addWidget(sLogCapacity, 1, 1);

  greed -> setColStretch( 2, 1);

  cbLogStoreInFile = new QCheckBox("Save log to the file", gbox);
  cbLogTimeLabel   = new QCheckBox("Put time stamps", gbox);
  cbLogStoreInFile-> setMinimumSize(cbLogStoreInFile->sizeHint());
  cbLogTimeLabel  -> setMinimumSize(cbLogTimeLabel->sizeHint());

  greed -> addWidget(cbLogStoreInFile, 0, 3);
  greed -> addWidget(cbLogTimeLabel  , 1, 3);

  greed	-> activate();

  main_layout	-> addWidget(gbox);
  main_layout	-> addStretch(1);


  //---
  gbox	= new QGroupBox("Log Levels", wLogger);
  greed	= new QGridLayout(gbox, 17, 13, 20, 6);

  label = new QLabel("Er", gbox);
  label -> setMinimumSize(label->sizeHint());
  greed -> addWidget(label, 0, 2);
  label = new QLabel("No", gbox);
  label -> setMinimumSize(label->sizeHint());
  greed -> addWidget(label, 0, 3);
  label = new QLabel("In", gbox);
  label -> setMinimumSize(label->sizeHint());
  greed -> addWidget(label, 0, 4);
  label = new QLabel("Db", gbox);
  label -> setMinimumSize(label->sizeHint());
  greed -> addWidget(label, 0, 5);
  label = new QLabel("Er", gbox);
  label -> setMinimumSize(label->sizeHint());
  greed -> addWidget(label, 0, 8);
  label = new QLabel("No", gbox);
  label -> setMinimumSize(label->sizeHint());
  greed -> addWidget(label, 0, 9);
  label = new QLabel("In", gbox);
  label -> setMinimumSize(label->sizeHint());
  greed -> addWidget(label, 0,10);
  label = new QLabel("Db", gbox);
  label -> setMinimumSize(label->sizeHint());
  greed -> addWidget(label, 0,11);

  greed -> setRowStretch( 0, 2);
  greed -> setColStretch( 0, 2);
  greed -> setColStretch( 1, 1);

  for (int i=0; i<16; i++)
    {      
      label= new QLabel(sFacility[i], gbox);
      label-> setMinimumSize(label->sizeHint());
      greed-> addWidget(label, 1+i, 1);
      for (int j=0; j<4; j++)
	{
	  cbLLevel[j][i] = new QCheckBox(gbox);
	  cbLLevel[j][i]->setMinimumSize(cbLLevel[j][i]->sizeHint());
	  QToolTip::add(cbLLevel[j][i], ttstr[j]);
	  greed -> addWidget(cbLLevel[j][i], 1+i, 2+j);
	};
    };
  greed -> setColStretch( 6, 7);
  greed -> setColStretch( 7, 1);
  for (int i=0; i<16; i++)
    {      
      label= new QLabel(sFacility[16+i], gbox);
      label-> setMinimumSize(label->sizeHint());
      greed-> addWidget(label, 1+i, 7);
      for (int j=0; j<4; j++)
	{
	  cbLLevel[j][16+i] = new QCheckBox(gbox);
	  cbLLevel[j][16+i]->setMinimumSize(cbLLevel[j][16+i]->sizeHint());
	  QToolTip::add(cbLLevel[j][16+i], ttstr[j]);
	  greed -> addWidget(cbLLevel[j][16+i], 1+i, 8+j);
	};
    };
  greed -> setColStretch( 12, 2);
  greed	-> activate();
  
  main_layout	-> addWidget(gbox);
  main_layout	-> addStretch(2);

  main_layout	-> activate();
  return wLogger;
};

QWidget* SBSetupDialog::packer()
{
  QWidget		*W;
  QGridLayout		*grid;
  QPushButton		*button;
  QLabel		*label;

  W = new QWidget(this);

  grid = new QGridLayout(W, 6,6, 20,10);

  label	= new QLabel("Set default compressor (for output a large dataset):", W);
  label	-> setMinimumSize(label->sizeHint());
  grid->addMultiCellWidget(label, 0,0, 0,2);

  //-- 
  FConv = new QListView(W);
  FConv -> addColumn("Name");
  FConv -> setColumnAlignment( 0, AlignLeft);
  FConv -> addColumn("Command for compressing");
  FConv -> setColumnAlignment( 1, AlignLeft);
  FConv -> addColumn("Command for uncompressing");
  FConv -> setColumnAlignment( 2, AlignLeft);
  FConv -> addColumn("Extension");
  FConv -> setColumnAlignment( 3, AlignLeft);

  FCDefault = new QComboBox(FALSE, W);
  FCDefault->insertItem("NONE");
  int i=1;
  for (SBNamed* E=setup->fcList()->first(); E; E=setup->fcList()->next())
    {
      (void) new SBFileConvLI(FConv, (SBFileConv*)E);
      if (((SBFileConv*)E)->comCompr()!="NONE") 
	{
	  FCDefault->insertItem(E->name()); 
	  if (E->name()==setup->fcList()->defaultName()) FCDefault->setCurrentItem(i);
	  i++;
	};
    };
  FCDefault->setMinimumSize(FCDefault->sizeHint());
  grid->addWidget(FCDefault, 0, 4);
  
  FConv -> setAllColumnsShowFocus(TRUE);
  FConv -> setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  if (FConv->firstChild())
    FConv -> setSelected(FConv->firstChild(), TRUE);
  FConv -> setMinimumWidth(FConv->sizeHint().width());
  FConv -> setFocusPolicy(QWidget::StrongFocus);
  grid->addMultiCellWidget(FConv, 1,3, 0,4);

  //--
  button = new QPushButton("Check", W);
  button-> setMinimumSize(button->sizeHint());
  grid->addWidget(button, 1,5);
  connect (button, SIGNAL(clicked()), SLOT(chkPacker()));

  button = new QPushButton("Delete", W);
  button-> setMinimumSize(button->sizeHint());
  grid->addWidget(button, 2,5);
  connect (button, SIGNAL(clicked()), SLOT(delPacker()));

  label	= new QLabel("New:", W);
  label	-> setMinimumSize(label->sizeHint());
  grid  -> addWidget(label, 5,0);

  label	= new QLabel("Name", W);
  label	-> setMinimumSize(label->sizeHint());
  grid->addWidget(label, 4,1);

  FCName = new QLineEdit(W);
  grid   -> addWidget(FCName, 5,1);

  label	= new QLabel("Compress", W);
  label-> setMinimumSize(label->sizeHint());
  grid -> addWidget(label, 4,2);

  FCComp = new QLineEdit(W);
  grid  -> addWidget(FCComp, 5,2);

  label	= new QLabel("Uncompress", W);
  label-> setMinimumSize(label->sizeHint());
  grid -> addWidget(label, 4,3);

  FCExtr = new QLineEdit(W);
  grid  -> addWidget(FCExtr, 5,3);

  label	= new QLabel("Extension", W);
  label-> setMinimumSize(label->sizeHint());
  grid -> addWidget(label, 4,4);

  FCExt = new QLineEdit(W);
  grid  -> addWidget(FCExt, 5,4);

  button = new QPushButton("Submit", W);
  button-> setMinimumSize(button->sizeHint());
  grid  -> addWidget(button, 5,5);
  connect (button, SIGNAL(clicked()), SLOT(addPacker()));

  grid -> setColStretch(1, 1);
  grid -> setColStretch(2, 1);
  grid -> setColStretch(3, 1);
  grid -> setColStretch(4, 1);
  grid -> setRowStretch(3, 1);
  
  grid->activate();
  return W;
};

QWidget* SBSetupDialog::misc()
{
  static const char* unitNames[3] =
  {
    "seconds",
    "meters",
    "inches"
  };
  QBoxLayout	*Layout;
  QBoxLayout	*aLayout;  
  QWidget	*W = new QWidget(this);

  Layout = new QVBoxLayout(W, 20, 10);

  // units:
  QButtonGroup *BGw = new QButtonGroup("Units for Observables", W);
  aLayout = new QVBoxLayout(BGw, 20, 10);
  for (int i=0; i<3; i++)
    {
      rbUnitNames[i] = new QRadioButton(unitNames[i], BGw);
      rbUnitNames[i]-> setMinimumSize(rbUnitNames[i]->sizeHint());
      aLayout -> addWidget(rbUnitNames[i]);
    };
  rbUnitNames[setup->units()]->setChecked(TRUE);
  aLayout -> activate();

  Layout ->addWidget(BGw);
  Layout ->addStretch(1);

  // other miscs here

  Layout->activate();
  return W;
};

QWidget* SBSetupDialog::institutions()
{
  QLabel	*label;
  QWidget	*W = new QWidget(this);
  QGridLayout	*grid = new QGridLayout(W, 4,4, 20,10);
  //--
  lvInst = new QListView(W);
  lvInst -> addColumn("Acronym");
  lvInst -> setColumnAlignment( 0, AlignLeft);
  lvInst -> addColumn("Organization's name:");
  lvInst -> setColumnAlignment( 1, AlignLeft);

  if (InstList)
    for (SBNamed* E=InstList->first(); E; E=InstList->next())
      (void) new SBInstLI(lvInst, (SBInstitution*)E);
  
  lvInst -> setAllColumnsShowFocus(TRUE);
  lvInst -> setMinimumHeight(lvInst->sizeHint().height());
  lvInst -> setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
  if (lvInst->firstChild())
    lvInst -> setSelected(lvInst->firstChild(), TRUE);
  lvInst -> setFocusPolicy(QWidget::StrongFocus);
  grid -> addMultiCellWidget(lvInst, 0,1, 0,2);

  label	= new QLabel("Acronym", W);
  label	-> setMinimumSize(label->sizeHint());
  grid->addWidget(label, 2,1);
  label	= new QLabel("Organization's name", W);
  label	-> setMinimumSize(label->sizeHint());
  grid->addWidget(label, 2,2);

  leAcronym = new QLineEdit(W);
  label	= new QLabel(leAcronym, "&Add:", W);
  label	-> setMinimumSize(label->sizeHint());
  leAcronym -> setMinimumHeight(leAcronym->sizeHint().height());
  leAcronym -> setMinimumWidth(leAcronym->fontMetrics().width("WWWWW") + 10);

  grid->addWidget(label, 3,0);
  grid->addWidget(leAcronym, 3,1);

  leDescr = new QLineEdit(W);
  leDescr -> setMinimumSize(leDescr->sizeHint());
  grid->addWidget(leDescr, 3,2);

  QPushButton *button = new QPushButton("Submit", W);
  button->setMinimumSize(button->sizeHint());
  grid-> addWidget(button, 3,3);
  connect (button, SIGNAL(clicked()), SLOT(addInst()));

  button = new QPushButton("Delete", W);
  button->setMinimumSize(button->sizeHint());
  grid-> addWidget(button, 0,3);
  connect (button, SIGNAL(clicked()), SLOT(delInst()));
  grid -> setRowStretch(1, 1);
  grid -> setColStretch(2, 1);
  grid -> activate();
  return W;
};

SBSetupDialog::~SBSetupDialog()
{
  if (setup) delete setup;
  if (InstList) delete InstList;
};

void SBSetupDialog::addInst()
{
  QString Acronym_ = leAcronym->text();
  QString Descr_ = leDescr->text();
  if (Acronym_.simplifyWhiteSpace()!="" && Descr_.simplifyWhiteSpace()!="")
    {
      if (Acronym_.length()<6)
	if (InstList->find(Acronym_))
	  QMessageBox::warning(this, "Organization exist already", 
			       "The acronym you typed already exist\n"
			       "in the list. Can't insert a duplicate.", "OK");
	else
	  {
	    SBInstitution* Item=new SBInstitution(Acronym_, Descr_);
	    if (InstList->insert(Item))
	      {
		(void) new SBInstLI(lvInst, Item);
		leAcronym->setText("");
		leDescr->setText("");
	      }
	    else delete Item;
	  }
      else
	QMessageBox::warning(this, "Invalid acronym", 
			     "The acronym you typed greater than 5 chars.\n", "OK");
    };
};

void SBSetupDialog::delInst()
{
  SBInstLI* Item = (SBInstLI*)lvInst->currentItem();
  QString Str = Item?Item->inst()->name():QString("*NULL*");
  if (Item)
    {
      QListViewItem* NextItem = Item->itemBelow();
      if (!NextItem) NextItem = Item->itemAbove();
      if (!QMessageBox::warning(this, "Delete?", "Are you sure to delete the entry\n\""
				+ Str + "\"?\n", 
				"Yes, del them!", "No, let it live.."))
	{
	  if (InstList->remove(Item->inst()))
	    {
	      delete Item;
	      if (NextItem) lvInst->setSelected(NextItem, TRUE);
	      Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": \"" + Str + 
			 "\", rest in peace..");
	    }
	  else
	    Log->write(SBLog::ERR, SBLog::INTERFACE, ClassName() + 
		       ": \"" + Str + "\" not found in the lists");
	}
      else
	Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": relax, \"" + Str + 
		   "\", it's just a joke.. ");
    };
};

void SBSetupDialog::delPacker()
{
  SBFileConvLI* Item = (SBFileConvLI*)FConv->currentItem();
  QString	Str  = Item?Item->entry()->name():QString("*NULL*");
  if (Item)
    {
      QListViewItem* NextItem = Item->itemBelow();
      if (!NextItem) NextItem = Item->itemAbove();
      if (!QMessageBox::warning(this, "Delete?", "Are you sure to delete the entry\n\""
				+ Str + "\"?\n", 
				"Yes, del them!", "No, let it live.."))
	{
	  if (setup->fcList()->remove(Item->entry()))
	    {
	      delete Item;
	      if (NextItem) FConv->setSelected(NextItem, TRUE);
	      Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": \"" + Str + 
			 "\", rest in peace..");
	    }
	  else
	    Log->write(SBLog::ERR, SBLog::INTERFACE, ClassName() + 
		       ": \"" + Str + "\" not found in the lists");
	}
      else
	Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": relax, \"" + Str + 
		   "\", it's just a joke.. ");
    };
};

void SBSetupDialog::chkPacker()
{
  SBFileConvLI* Item = (SBFileConvLI*)FConv->currentItem();
  if (Item)
    {
      QString str = Item->entry()->comCompr()!="NONE"&&Item->entry()->comUncom()!="NONE"?
	Item->entry()->check()
	?"The packer [" + Item->entry()->name() + "] has been checked, OK."
	:"Checking of the packer [" + Item->entry()->name() + "] failed."
	:"Cannot check the packer [" + Item->entry()->name() + 
	"], because one\nof the commands is \"NONE\". OK assumed.";
      QMessageBox::information(this, "Packer check", str, "OK");
    };
};

void SBSetupDialog::addPacker()
{
  QString Name_ = FCName->text();
  QString Comp_ = FCComp->text();
  QString Extr_ = FCExtr->text();
  QString Ext_  = FCExt ->text();
  if (Name_.simplifyWhiteSpace()!="" && Comp_.simplifyWhiteSpace()!="" &&
      Extr_.simplifyWhiteSpace()!="" && Ext_ .simplifyWhiteSpace()!=""   )
    {
      SBFileConv* Item = new SBFileConv(Name_, Comp_, Extr_, Ext_);
      if (Item->check())
	{
	  if (setup->fcList()->insert(Item))
	    {
	      (void) new SBFileConvLI(FConv, Item);
	      FCName->setText("");
	      FCComp->setText("");
	      FCExtr->setText("");
	      FCExt ->setText("");
	    }
	  else
	    {
	      QMessageBox::warning(this, "Packer exist", 
				   "The file extension you typed already\n"
				   "exist in the list.", 
				   "OK");
	      delete Item;
	    };
	}
      else
	{
	  QMessageBox::warning(this, "Bad commands", 
			       "These pipes are wrong. May be you've\n"
			       "misspelled typing the commands or they\n"
			       "are not exist in your system.", 
			       "OK");
	  delete Item;
	};
    };
};

void SBSetupDialog::setData()
{
  int i;
  PathLE[ 0]->setText(setup->homeDir());
  PathLE[ 1]->setText(setup->dataDir());
  PathLE[ 2]->setText(setup->compileDir());
  PathLE[ 3]->setText(setup->stuffDir());
  PathLE[ 4]->setText(setup->outputDir());
  PathLE[ 5]->setText(setup->solutionsDir());
  PathLE[ 6]->setText(setup->projectDir());
  PathLE[ 7]->setText(setup->logDir());
  PathLE[ 8]->setText(setup->workDir());
  PathLE[ 9]->setText(setup->importDir());
  PathLE[10]->setText(setup->importEOPDir());
  PathLE[11]->setText(setup->importITRFDir());
  PathLE[12]->setText(setup->importICRFDir());
  PathLE[13]->setText(setup->importOLoadDir());
  PathLE[14]->setText(setup->importMFilesDir());
  PathLE[15]->setText(setup->importAploDir());

  PathLE[16]->setText(setup->stations());
  PathLE[17]->setText(setup->sources());
  PathLE[18]->setText(setup->eops());

  eLogFileName->setText(setup->logFileName());
  sLogCapacity->setValue (setup->logCapacity());

  cbLogStoreInFile->setChecked(setup->logStoreInFile());
  cbLogTimeLabel->setChecked(setup->logTimeLabel());

  uint vasia[4];
  vasia[0] = setup->logFacilities(SBLog::ERR);
  vasia[1] = setup->logFacilities(SBLog::WRN);
  vasia[2] = setup->logFacilities(SBLog::INF);
  vasia[3] = setup->logFacilities(SBLog::DBG);
  for (i=0; i<4; i++ )
    for (int j=0; j<32; j++)
      cbLLevel[i][j]->setChecked(vasia[i] & LogFacilities[j]);

  i=1;
  FConv->clear();
  FCDefault->clear();
  FCDefault->insertItem("NONE");
  for (SBNamed* E=setup->fcList()->first(); E; E=setup->fcList()->next())
    {
      (void) new SBFileConvLI(FConv, (SBFileConv*)E);
      if (((SBFileConv*)E)->comCompr()!="NONE") 
	{
	  FCDefault->insertItem(E->name()); 
	  if (E->name()==setup->fcList()->defaultName()) FCDefault->setCurrentItem(i);
	  i++;
	};
    };
  if (FConv->firstChild())
    FConv->setSelected(FConv->firstChild(), TRUE);


  rbUnitNames[setup->units()]->setChecked(TRUE);

  
  lvInst->clear();
  if (InstList)
    for (SBNamed* E=InstList->first(); E; E=InstList->next())
      (void) new SBInstLI(lvInst, (SBInstitution*)E);
  if (lvInst->firstChild())
    lvInst->setSelected(lvInst->firstChild(), TRUE);
  leAcronym->setText("");
  leDescr->setText("");
};

void SBSetupDialog::makeApply()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": apply pressed");
  SBTabDialog::makeApply();
  if (SetUp && setup)
    {
      *SetUp = *setup;
      mainSetPrjName(SetUp->project());
      mainSetTRFName(SetUp->stations());
      mainSetCRFName(SetUp->sources());
      mainSetEOPName(SetUp->eops());
    };

  *Institutions = *InstList;
  SetUp->UpdateLogger();
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": done");
};

void SBSetupDialog::makeDefaults()
{
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": setting defaults");
  setup->SetDefaults();
  if (setup)
    {
      mainSetPrjName(setup->project());
      mainSetTRFName(setup->stations());
      mainSetCRFName(setup->sources());
      mainSetEOPName(setup->eops());
    };

  setData();
  Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + ": done");
};

void SBSetupDialog::acquireData()
{
  setup->setHomeDir		(PathLE[ 0] -> text());
  setup->setDataDir		(PathLE[ 1] -> text());
  setup->setCompileDir		(PathLE[ 2] -> text());
  setup->setStuffDir		(PathLE[ 3] -> text());
  setup->setOutputDir		(PathLE[ 4] -> text());
  setup->setSolutionsDir	(PathLE[ 5] -> text());
  setup->setProjectDir		(PathLE[ 6] -> text());
  setup->setLogDir		(PathLE[ 7] -> text());
  setup->setWorkDir		(PathLE[ 8] -> text());
  setup->setImportDir		(PathLE[ 9] -> text());
  setup->setImportEOPDir	(PathLE[10] -> text());
  setup->setImportITRFDir	(PathLE[11] -> text());
  setup->setImportICRFDir	(PathLE[12] -> text());
  setup->setImportOLoadDir	(PathLE[13] -> text());
  setup->setImportMFilesDir	(PathLE[14] -> text());
  setup->setImportAploDir	(PathLE[15] -> text());
  
  setup->setStations		(PathLE[16] -> text());
  setup->setSources		(PathLE[17] -> text());
  setup->setEOPs		(PathLE[18] -> text());

  setup->setLogFileName		(eLogFileName->text());
  setup->setLogCapacity		(sLogCapacity->value());
  setup->setLogStoreInFile	(cbLogStoreInFile->isChecked());
  setup->setLogTimeLabel	(cbLogTimeLabel->isChecked());

  uint vasia[4] = {0,0,0,0};
  for (int i=0; i<4; i++ )
    for (int j=0; j<32; j++)
      if (cbLLevel[i][j]->isChecked()) vasia[i] |= LogFacilities[j];
  
  setup -> setLogFacilities(SBLog::ERR, vasia[0]);
  setup -> setLogFacilities(SBLog::WRN,	vasia[1]);
  setup -> setLogFacilities(SBLog::INF,	vasia[2]);
  setup -> setLogFacilities(SBLog::DBG, vasia[3]);

  setup->fcList()->setDefault(FCDefault->currentText());

  SBSetUp::DUnits	units=SBSetUp::U_SEC;
  if (rbUnitNames[0]->isChecked()) units=SBSetUp::U_SEC;
  if (rbUnitNames[1]->isChecked()) units=SBSetUp::U_METER;
  if (rbUnitNames[2]->isChecked()) units=SBSetUp::U_INCH;
  setup->setUnits(units);
};
/*==============================================================================================*/
