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

#include "SbInterface.H"

#include <unistd.h>
#include <qapplication.h> 

QString TmpString("Temporary string");

//#include <qmultilinedit.h>

#include <qtableview.h>


/*==============================================================================================*/
/*												*/
/* SBLogger											*/
/*												*/
/*==============================================================================================*/
SBLogger::SBLogger(QWidget* parent, int Capacity_, bool isStoreInFile_, 
		   const QString& FileName_, const char* name)
  : SBLog(Capacity_,isStoreInFile_,FileName_), QMultiLineEdit(parent,name)
{
  setReadOnly(TRUE);
  setTableFlags(tableFlags() | Tbl_scrollLastVCell | Tbl_vScrollBar);
};
 
void SBLogger::MakeOutput(const QString & s, bool IsAsync)
{
  append(s);
  if (IsAsync)
    qApp->processEvents();

  int n_rows;
  if (!rowIsVisible((n_rows=numRows()-1)))
    {
      setTopCell(n_rows);
      setCursorPosition(n_rows, 0);
      if (n_rows>2) 
	qApp->processEvents();
    };
};
  
void SBLogger::ClearSpool()
{
  SBLog::clearSpool();

  setAutoUpdate(FALSE);
  if ((uint)numRows()>=3*Capacity) 
    for (uint i=0; i<Capacity; i++) removeLine(0);
  setAutoUpdate(TRUE);
  repaint();
  Log->write(DBG, IO, ClassName() + ": spool has been cleared");
};
/*==============================================================================================*/




/*==============================================================================================*/
/*												*/
/* SBListView											*/
/*												*/
/*==============================================================================================*/
SBListView::SBListView(QWidget *parent, const char *name)
  : QListView (parent, name) 
{
  IsMoving	= FALSE;
  MItem		= NULL;
  MCol		= -1;

  connect(this, 
	  SIGNAL(pressed      (QListViewItem*, const QPoint&, int)), 
	  SLOT  (movingStarted(QListViewItem*, const QPoint&, int)));
};

void SBListView::setSelected (QListViewItem *item, bool selected)
{
  if (isMultiSelection()&&(item->isSelected()^selected)) emit selChanged4Item(item, selected);
  QListView::setSelected(item, selected);
};

void SBListView::movingStarted(QListViewItem* item, const QPoint&, int c)
{
  if (item)
    {
      MItem	= item;
      MCol	= c;
      IsMoving	= TRUE;
      emit moveUponItem(MItem, MCol);
    };
};

void SBListView::contentsMouseMoveEvent(QMouseEvent* e)
{
  if (e && IsMoving)
    {
      QPoint vp = contentsToViewport(e->pos());
      QListViewItem *i = itemAt(vp);
      if (i && i!=MItem)
	{
	  MItem=i;
	  emit moveUponItem(MItem, MCol);
	};
    };
  QListView::contentsMouseMoveEvent(e);
};

void SBListView::contentsMouseReleaseEvent(QMouseEvent* e)
{
  MItem		= NULL;
  MCol		= -1;
  IsMoving	= FALSE;
  QListView::contentsMouseReleaseEvent(e);
};
/*==============================================================================================*/


/*==============================================================================================*/
