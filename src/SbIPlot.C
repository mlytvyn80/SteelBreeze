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

#include <qbuttongroup.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qfile.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlistbox.h>
#include <qpaintdevicemetrics.h>
#include <qprinter.h>
#include <qpushbutton.h>
#include <qslider.h>
#include <qtextstream.h>


#include "SbIPlot.H"
#include "SbSetup.H"




/*==============================================================================================*/
/*												*/
/* SBPlotBranch											*/
/*												*/
/*==============================================================================================*/
SBPlotBranch::SBPlotBranch(unsigned int NR_, unsigned int NCV_, unsigned int NCE_, const QString& Name_)
  : SBNamed(Name_)
{
  NR  = NR_;
  Data = new SBMatrix(NR, NCV_ + NCE_ + 1, "Plotting data for " + Name_);
  IsBrowsable = TRUE;
  Idx = 0;
};

SBPlotBranch::~SBPlotBranch()
{
  if (Data)
    {
      delete Data;
      Data = NULL;
    };
};

void SBPlotBranch::setDataAttr(unsigned int i, unsigned int Attr_)
{
  Data->set(i, Data->nCol()-1, (double) Attr_);
};

unsigned int SBPlotBranch::dataAttr(unsigned int i) const
{
  return (unsigned int) Data->at(i, Data->nCol()-1);
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBPlotCarrier										*/
/*												*/
/*==============================================================================================*/
SBPlotCarrier::SBPlotCarrier(unsigned int NCV_, unsigned int NCE_, const QString& Name_)
  : SBNamed(Name_)
{
  NCV = NCV_;
  NCE = NCE_;

  DatTypes	= new int[NCV];
  DatStdVarIdx  = new unsigned int[NCV];
  DatAttr	= new unsigned int[NCV];
  //set default values:
  for (unsigned int i=0; i<NCV; i++)
    {
      *(DatTypes+i) = AT_DATA;
      *(DatStdVarIdx+i) = 0;
      *(DatAttr+i) = 0;
    };

  ColumnNames	= new QList<QString>;
  ColumnNames->setAutoDelete(TRUE);
  Branches.setAutoDelete(TRUE);
  IsOK = FALSE;
  File4SaveBaseName = SetUp->path2Output() + "unnamed";
};

SBPlotCarrier::~SBPlotCarrier()
{
  if (DatTypes)
    {
      delete[] DatTypes;
      DatTypes = NULL;
    };
  if (DatStdVarIdx)
    {
      delete[] DatStdVarIdx;
      DatStdVarIdx = NULL;
    };
  if (DatAttr)
    {
      delete[] DatAttr;
      DatAttr = NULL;
    };
  if (ColumnNames)
    {
      delete ColumnNames;
      ColumnNames = NULL;
    };
};

void SBPlotCarrier::createBranch(unsigned int NR_, const QString& BranchName)
{
  SBPlotBranch *pb = new SBPlotBranch(NR_, NCV, NCE, BranchName);
  Branches.inSort(pb);
};

bool SBPlotCarrier::selfCheck()
{
  IsOK = FALSE;
  if (!Branches.count())
  {
    Log->write(SBLog::DBG, SBLog::IO, ClassName() + ": SelfCheck: there is no branches, count()==0; nothing to plot");
    return IsOK; // nothing to plot
  };
  for (SBPlotBranch *pb=Branches.first(); pb; pb=Branches.next())
    if (!pb->data()->nRow()) return IsOK;
  return (IsOK=TRUE);
};

void SBPlotCarrier::setFile4SaveBaseName(const QString& File4SaveBaseName_)
{
  File4SaveBaseName = SetUp->path2Output() + File4SaveBaseName_;
};

void SBPlotCarrier::setAbsFile4SaveBaseName(const QString& AbsFile4SaveBaseName_)
{
  File4SaveBaseName = AbsFile4SaveBaseName_;
};

/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBPlotArea											*/
/*												*/
/*==============================================================================================*/
QString SBPlotArea::XLabel4Unknown("Unnamed X axis");
QString SBPlotArea::YLabel4Unknown("Unnamed Y axis");
SBPlotArea::SBPlotArea(SBPlotCarrier* PlotCarrier_, QWidget* parent, const char* name, WFlags f)
  : QWidget(parent, name, f)
{
  PlotCarrier = PlotCarrier_;

  Width =980;
  Height=510;

  Xcol=Ycol=0;
  isXTicsMJD  = FALSE;
  IsPlotPoints= TRUE;
  IsPlotLines = FALSE;
  IsPlotErrBars= FALSE;
  IsPlotImpulses= FALSE;
  IsHasHaveZero = FALSE;

  IsLimitsOnVisible= FALSE;
  IsAllData	   = FALSE;
  IsStdVar	   = TRUE;

  XLabel=&XLabel4Unknown;
  YLabel=&YLabel4Unknown;

  setXCol(0);
  setYCol(1);

  resize(Width, Height);
  setBackgroundMode(PaletteBase);
  FramePen = new QPen(QColor(0,0,0), 1);
  ZeroPen  = new QPen(QColor(200,200,200), 1);
  BarPen  = new QPen(QColor(180,0,0), 1);

  BPSaturation	= 255;
  BPValue	= 240;

  BranchPens = NULL;
  initBranchPens();


  MaxX = 20.0;
  MaxY = 20.0;
  MinX =-20.0;
  MinY =-20.0;

  //
  UpMarg = 10;
  RtMarg = 10;

  XLabelWidth = 0;
  LabelsHeight= 0;
  YLabelWidth = 0;
  YLabelHeight= 0;
  XTicsWidth  = 0;
  YTicsWidth  = 0;

  TitleWidth  = 0;
  // XTics:
  NumOfXTics  = 0;
  XStepP      = 0;
  XTicsStep   = 0.0;
  XTicsBias   = 0.0;
  isXTicsBiased= FALSE;

  // YTics:
  NumOfYTics  = 0;
  YStepP      = 0;
  YTicsStep   = 0.0;
  YTicsBias   = 0.0;
  isYTicsBiased= FALSE;

  XFrameBegin = 0;
  XFrameEnd   = 0;
  YFrameBegin = 0;
  YFrameEnd   = 0;

  XDataBegin  = 0;
  XDataEnd    = 0;
  YDataBegin  = 0;
  YDataEnd    = 0;

  f_Ax = f_Bx = f_Ay = f_By = 0.0;

  XTicsMJD    = 0;
};

SBPlotArea::~SBPlotArea()
{
  if (FramePen) 
    {
      delete FramePen;
      FramePen = NULL;
    };
  if (ZeroPen) 
    {
      delete ZeroPen;
      ZeroPen = NULL;
    };
  if (BarPen) 
    {
      delete BarPen;
      BarPen = NULL;
    };
  if (BranchPens)
    {
      delete[] BranchPens;
      BranchPens = NULL;
    };
};

void SBPlotArea::initBranchPens()
{
  if (BranchPens)
    {
      delete[] BranchPens;
      BranchPens = NULL;
    };

  int nb=PlotCarrier->branches()->count();
//  BranchPens = new QPen[nb](QColor(0,0,0), 1);
  BranchPens = new QPen[nb];
  setBranchColors();
};

void SBPlotArea::setBPSaturation(int Sat_) 
{
  BPSaturation = Sat_;
  setBranchColors();
  repaint(FALSE); // to avoid flicker
};

void SBPlotArea::setBPValue(int Val_)
{
  BPValue      = Val_;
  setBranchColors();
  repaint(FALSE); // to avoid flicker
};

void SBPlotArea::setBranchColors()
{
  int	nb=PlotCarrier->branches()->count();
  QPen	*pen=BranchPens;
  for (int i=0; i<nb; i++, pen++)
    {
	pen->setColor( QColor( (int)(360*i/nb), BPSaturation, BPValue, QColor::Hsv) );
	pen->setWidth(0);
    };
};

void SBPlotArea::setXCol(unsigned int X_)
{
  if (PlotCarrier->isOK() && X_ < PlotCarrier->numOfCols())
    {
      Xcol = X_;
      if (PlotCarrier->type(Xcol) == SBPlotCarrier::AT_MJD) 
	isXTicsMJD = TRUE;
      else 
	isXTicsMJD = FALSE;
      if (Xcol < PlotCarrier->columnNames()->count())
	XLabel=PlotCarrier->columnNames()->at(Xcol);
      else 
	XLabel=&XLabel4Unknown;
    };
};

void SBPlotArea::setYCol(unsigned int Y_)
{
  if (PlotCarrier->isOK() && Y_ < PlotCarrier->numOfCols())
    {
      Ycol = Y_;
      if (Ycol < PlotCarrier->columnNames()->count())
	YLabel=PlotCarrier->columnNames()->at(Ycol);
      else 
	YLabel=&YLabel4Unknown;
    };
};

void SBPlotArea::getLimits()
{
  if (PlotCarrier->isOK())
    {
      SBPlotBranch* pb=PlotCarrier->branches()->first();
      unsigned int ei = IsStdVar?PlotCarrier->stdVarIdx(Ycol):0;
      unsigned int i=0;

      if (IsLimitsOnVisible)
	{
	  while (pb && !pb->isBrowsable())
	    pb = PlotCarrier->branches()->next();
	  if (!pb) // all dead
	    pb=PlotCarrier->branches()->first();
	};

      if (!IsAllData)
	{
	  while (i<pb->data()->nRow() && (pb->dataAttr(i) & SBPlotCarrier::DA_IGN))
	    i++;
	  if (i==pb->data()->nRow()) // all dead
	    i=0;
	};

      MaxX = pb->data()->at(i, Xcol);
      MaxY = ei?pb->data()->at(i, Ycol)+pb->data()->at(i, ei):pb->data()->at(i, Ycol);
      MinX = pb->data()->at(i, Xcol);
      MinY = ei?pb->data()->at(i, Ycol)-pb->data()->at(i, ei):pb->data()->at(i, Ycol);
      
      for (pb=PlotCarrier->branches()->first(); pb; pb=PlotCarrier->branches()->next())
	if (pb->isBrowsable() || !IsLimitsOnVisible)
	  for (i=0; i<pb->data()->nRow(); i++)
	    {
	      if (!(pb->dataAttr(i) & SBPlotCarrier::DA_IGN) || IsAllData)
		{
		  MinX = std::min(MinX, pb->data()->at(i, Xcol));
		  MaxX = std::max(MaxX, pb->data()->at(i, Xcol));
		  MinY = std::min(MinY, (ei?pb->data()->at(i, Ycol)-pb->data()->at(i, ei):pb->data()->at(i, Ycol)));
		  MaxY = std::max(MaxY, (ei?pb->data()->at(i, Ycol)+pb->data()->at(i, ei):pb->data()->at(i, Ycol)));
		};
	    };
      
      if (MaxX == MinX)
	{
	  MinX -= 0.5;
	  MaxX += 0.5;
	  //	  pb=PlotCarrier->branches()->first();
	  //	  MinX = pb->data()->at(0, Xcol) - 1.0;
	  //	  MaxX = pb->data()->at(0, Xcol) + 1.0;
	};
      if (MaxY == MinY)
	{
	  MinY -= 0.5;
	  MaxY += 0.5;
	  //	  pb=PlotCarrier->branches()->first();
	  //	  MinY = pb->data()->at(0, Ycol) - 1.0;
	  //	  MaxY = pb->data()->at(0, Ycol) + 1.0;
	};

      if (IsHasHaveZero)
	{
	  if (MaxY*MinY > 0.0 && PlotCarrier->type(Ycol) != SBPlotCarrier::AT_MJD)
	    {
	      if (MinY>0.0) MinY=0.0;
	      else MaxY = 0.0;
	    };
	  if (MaxX*MinX > 0.0 && PlotCarrier->type(Xcol) != SBPlotCarrier::AT_MJD)
	    {
	      if (MinX>0.0) MinX=0.0;
	      else MaxX = 0.0;
	    };
	};
    };
};

void SBPlotArea::calcTransforms()
{
  f_Bx = (XDataEnd - XDataBegin)/(MaxX - MinX);
  f_Ax = XDataEnd - f_Bx*MaxX;

  f_By = (YDataEnd - YDataBegin)/(MinY - MaxY);
  f_Ay = YDataBegin - f_By*MaxY;
};

void SBPlotArea::defineAreas(QPainter* P)
{
  double dy=0.0;
  isXTicsBiased= FALSE;
  isYTicsBiased= FALSE;


  QFontMetrics	fm(P->fontMetrics());
  LabelsHeight = fm.height();

  XLabelWidth  = fm.width(*XLabel);
  YLabelWidth  = fm.width(*YLabel);
  YLabelHeight = fm.height();
  TitleWidth   = fm.width(PlotCarrier->name());

  // should be smarter a bit
  XTicsWidth = fm.width("800000000");
  if (isXTicsMJD)
    XTicsWidth = fm.width(" " + SBMJD(MinX).toString(SBMJD::F_DateShort) + " ");

  UpMarg      = 3*LabelsHeight/2;
  YFrameBegin = UpMarg;
  YFrameEnd   = Height - 7*LabelsHeight/2;
  // Define Width of Y Tics:
  NumOfYTics=(YFrameEnd - YFrameBegin)/(5*LabelsHeight);
  YTicsStep = (MaxY-MinY)/NumOfYTics;
  YStepP = (int)floor(log10(YTicsStep));


  YTicsStep = trunc( YTicsStep/exp10(YStepP) );
  if (YTicsStep==3)
    YTicsStep=2;
  else if (YTicsStep==3)
    YTicsStep=4;
  else if (YTicsStep==6)
    YTicsStep=5;
  else if (YTicsStep==7)
    YTicsStep=5;
  else if (YTicsStep==8 || YTicsStep==9)
    YTicsStep=10;
  YTicsStep = YTicsStep*exp10(YStepP);
  YTicsWidth = 10;
  //  if (MaxY*MinY<0.0) // here the "0" is the origin
  if ((MaxY+YTicsStep/3.0)*(MinY-YTicsStep/3.0)<0.0) // here the "0" is the origin
    {
      dy=0.0;
      while (dy<=MaxY+YTicsStep/3.0)
	{
	  YTicsWidth = std::max(YTicsWidth, fm.width(QString().sprintf("%.8g", dy)));
	  dy+=YTicsStep;
	};
      dy=0.0;
      while (dy>=MinY-YTicsStep/3.0)
	{
	  YTicsWidth = std::max(YTicsWidth, fm.width(QString().sprintf("%.8g", dy)));
	  dy-=YTicsStep;
	};
    }
  else
    {
      if (log10(fabs(MinY)) - YStepP > 7.0) // need a bias
	{
	  isYTicsBiased = TRUE;
	  YTicsBias = trunc( MinY/exp10(YStepP+1.0) )*exp10(YStepP+1.0);
	  YLabelWidth+=fm.width(QString().sprintf(", %.16g +", YTicsBias));
	}
      else
	{
	  YTicsBias = 0.0;
	  isYTicsBiased = FALSE;
	};
      // define the origin:
      dy = YTicsStep*floor(MaxY/YTicsStep);
      while (dy>MinY) // down from the origin:
	{
	  YTicsWidth = std::max(YTicsWidth, fm.width(QString().sprintf("%.8g", isYTicsBiased?dy-YTicsBias:dy)));
	  dy-=YTicsStep;
	};
    };
  // end of YTics calculations
  
  XFrameBegin = YTicsWidth + 5*YLabelHeight/2 + 10;
  XFrameEnd   = Width - RtMarg;

  // Define X Tics parameters:
  if (isXTicsMJD)
    {
      NumOfXTics=(XFrameEnd - XFrameBegin)/(7*XTicsWidth/4);
      //      NumOfXTics=(XFrameEnd - XFrameBegin)/(2*XTicsWidth);
      XTicsStep = 86400.0*(MaxX-MinX)/NumOfXTics; // here XTicsStep in sec
      if (XTicsStep<=60.0) // sec.s
	{
	  XStepP = (int)floor(log10(XTicsStep));
	  XTicsStep = trunc( XTicsStep/exp10(XStepP) )*exp10(XStepP);
	  XTicsMJD = 0;
	}
      else if (XTicsStep/60.0<=45.0) // min.s
	{
	  XTicsStep/=60.0;
	  XStepP = (int)floor(log10(XTicsStep));
	  XTicsStep = trunc( XTicsStep/exp10(XStepP) )*exp10(XStepP);
	  if (10.0>XTicsStep && XTicsStep>=8.0)
	    XTicsStep=10.0;
	  else if (8.0>XTicsStep && XTicsStep>=5.0)
	    XTicsStep=5.0;
	  else if (5.0>XTicsStep && XTicsStep>=2.0)
	    XTicsStep=2.0;
	  XTicsMJD = 1;
	}
      else if (XTicsStep/3600.0<=18.0) // hrs
	{
	  XTicsStep/=3600.0;
	  XStepP = (int)floor(log10(XTicsStep));
	  XTicsStep = trunc( XTicsStep/exp10(XStepP) )*exp10(XStepP);
	  if (XTicsStep>=10.0) 
	    XTicsStep=12.0;
	  else if (XTicsStep>=8.0) 
	    XTicsStep=8.0;
	  else if (XTicsStep>=6.0) 
	    XTicsStep=6.0;
	  else if (XTicsStep>=4.0) 
	    XTicsStep=4.0;
	  if (XTicsStep<1.0) 
	    XTicsStep=1.0;
	  XTicsMJD = 2;
	}
      else if (XTicsStep/86400.0<=20.0) // days
	{
	  XTicsStep/=86400.0;
	  if (XTicsStep<1.0) XTicsStep=0.5;
	  XStepP = (int)floor(log10(XTicsStep));
	  XTicsStep = trunc( XTicsStep/exp10(XStepP) )*exp10(XStepP);
	  XTicsMJD = 3; //days
	}
      else
      	{
	  NumOfXTics=(XFrameEnd - XFrameBegin)/(2*XTicsWidth);
	  XTicsStep = 86400.0*(MaxX-MinX)/NumOfXTics;

	  XTicsStep/=86400.0;
      	  XStepP = (int)floor(log10(XTicsStep));
      	  XTicsStep = trunc( XTicsStep/exp10(XStepP) )*exp10(XStepP);
      	  XTicsMJD = 4; //days and more
      	};
    }
  else
    {
      NumOfXTics=(XFrameEnd - XFrameBegin)/(2*XTicsWidth);
      XTicsStep = (MaxX-MinX)/NumOfXTics;
      XStepP = (int)floor(log10(XTicsStep));
      XTicsStep = trunc( XTicsStep/exp10(XStepP) );
      if (XTicsStep==3)
	XTicsStep=2;
      else if (XTicsStep==3)
	XTicsStep=4;
      else if (XTicsStep==6)
	XTicsStep=5;
      else if (XTicsStep==7)
	XTicsStep=5;
      else if (XTicsStep==8 || XTicsStep==9)
	XTicsStep=10;
      XTicsStep = XTicsStep*exp10(XStepP);
      if (!isXTicsMJD && log10(fabs(MinX)) - XStepP > 7.0) // need a bias
	{
	  isXTicsBiased = TRUE;
	  XTicsBias = trunc( MinX/exp10(XStepP+1.0) )*exp10(XStepP+1.0);
	  XLabelWidth+=fm.width(QString().sprintf(", %.16g +", XTicsBias));
	}
      else
	{
	  XTicsBias = 0.0;
	  isXTicsBiased = FALSE;
	};
    };
  // end of XTics calculations
  
  XDataBegin = XFrameBegin + 20;
  XDataEnd   = XFrameEnd   - 20;

  YDataBegin = YFrameBegin + 20;
  YDataEnd   = YFrameEnd   - 20;
};

void SBPlotArea::resizeEvent(QResizeEvent* ev)
{
  QWidget::resizeEvent(ev);
  Height=ev->size().height();
  Width =ev->size().width();
};

void SBPlotArea::paintEvent(QPaintEvent *)
{
  QPainter paint(this);
  showData(&paint);
};

void SBPlotArea::output4Print(QPainter* P, int Width_, int Height_)
{
  int	WidthSaved = Width;
  int	HeightSaved= Height;

  Width = Width_;
  Height= Height_;

  showData(P);

  Width = WidthSaved;
  Height= HeightSaved;
};

void SBPlotArea::showData(QPainter* P)
{
  getLimits();
  defineAreas(P);
  calcTransforms();
  drawFrames(P);
  drawYTics(P);

  if (isXTicsMJD)
    drawXmjdTics(P);
  else
    drawXTics(P);
  
  drawData(P);
};

void SBPlotArea::drawFrames(QPainter* P)
{
  // Title:
  P->drawText((XFrameEnd + XFrameBegin - TitleWidth)/2, LabelsHeight, PlotCarrier->name());

  // Labels:
  // X Label:
  if (isXTicsMJD)
    P->drawText((XFrameBegin + XFrameEnd - XLabelWidth)/2, Height-LabelsHeight/2, 
		(XTicsMJD<=2 && MaxX-MinX<1.0)?
		(*XLabel + "; " + SBMJD((int)trunc(MinX), 0.0).toString(SBMJD::F_DateShort))
		:*XLabel);
  else
    P->drawText((XFrameBegin + XFrameEnd - XLabelWidth)/2, Height-LabelsHeight/2, 
		isXTicsBiased?(*XLabel + QString().sprintf(", %.16g +", XTicsBias)):*XLabel);
  // Y Label:
  P->save();
  P->rotate(270);
  P->drawText(-(YFrameEnd+YFrameBegin+YLabelWidth)/2, 3*LabelsHeight/2, 
	      isYTicsBiased?(*YLabel + QString().sprintf(", %.16g +", YTicsBias)):*YLabel);
  P->restore();

  // frames:
  P->setPen(*FramePen);
  P->drawLine(XFrameBegin, YFrameEnd,   XFrameEnd,   YFrameEnd);
  P->drawLine(XFrameEnd,   YFrameEnd,   XFrameEnd,   YFrameBegin);
  P->drawLine(XFrameEnd,   YFrameBegin, XFrameBegin, YFrameBegin);
  P->drawLine(XFrameBegin, YFrameBegin, XFrameBegin, YFrameEnd);
};

void SBPlotArea::drawYTics(QPainter* P)
{
  QString	YTic;
  int		TicAlignFlag = (YStepP<0)?Qt::AlignLeft:Qt::AlignRight;
  double	dy=0.0;
  int		ytic=0, i;
  
  //if (MaxY*MinY<0.0) // there is "0":
  if ((MaxY+YTicsStep/3.0)*(MinY-YTicsStep/3.0)<0.0) // there is "0":
    {
      P->setPen(*ZeroPen);
      dy=0.0;
      ytic=calcY(dy);
      P->drawLine(XFrameBegin+1, ytic, XFrameEnd-1, ytic);
      P->setPen(*FramePen);
      while (dy<=MaxY)
	{
	  if ((ytic=calcY(dy))>YFrameBegin+3 && ytic<YFrameEnd-3)
	    {
	      YTic.sprintf("%.8g", dy);
	      P->drawLine(XFrameEnd,   ytic, XFrameEnd-10,   ytic);
	      P->drawLine(XFrameBegin, ytic, XFrameBegin+10, ytic);
	      P->drawText(XFrameBegin-YTicsWidth-10, ytic-LabelsHeight/2, YTicsWidth, LabelsHeight, 
			  TicAlignFlag, YTic);
	    };
	  for (i=0; i<9; i++)
	    if ((ytic = calcY(dy + YTicsStep*(i+1)/10.0))>YFrameBegin+3 && ytic<YFrameEnd-3)
	      {
		P->drawLine(XFrameEnd,   ytic, XFrameEnd  - ((i!=4)?3:6), ytic);
		P->drawLine(XFrameBegin, ytic, XFrameBegin+ ((i!=4)?4:7), ytic);
	      };
	  dy+=YTicsStep;
	};
      for (i=0; i<9; i++)
	if ((ytic = calcY(-YTicsStep*(i+1)/10.0))<YFrameEnd-3 && ytic>YFrameBegin+3)
	  {
	    P->drawLine(XFrameEnd,   ytic, XFrameEnd  - ((i!=4)?3:6), ytic);
	    P->drawLine(XFrameBegin, ytic, XFrameBegin+ ((i!=4)?4:7), ytic);
	  };
      dy=-YTicsStep;
      while (dy>=MinY)
	{
	  if ((ytic = calcY(dy))<YFrameEnd-3 && ytic>YFrameBegin+3)
	    {
	      YTic.sprintf("%.8g", dy);
	      P->drawLine(XFrameEnd,   ytic, XFrameEnd-10,   ytic);
	      P->drawLine(XFrameBegin, ytic, XFrameBegin+10, ytic);
	      P->drawText(XFrameBegin-YTicsWidth-10, ytic-LabelsHeight/2, YTicsWidth, LabelsHeight, 
			  TicAlignFlag, YTic);
	    };
	  for (i=0; i<9; i++)
	    if ((ytic = calcY(dy - YTicsStep*(i+1)/10.0))<YFrameEnd-3 && ytic>YFrameBegin+3)
	      {
		P->drawLine(XFrameEnd,   ytic, XFrameEnd  - ((i!=4)?3:6), ytic);
		P->drawLine(XFrameBegin, ytic, XFrameBegin+ ((i!=4)?4:7), ytic);
	      };
	  dy-=YTicsStep;
	};
    }
  else
    {
      // define the origin:
      double	y_0=YTicsStep*floor((YFrameBegin+3-f_Ay)/f_By/YTicsStep);
      dy = y_0;
      while (dy>(YFrameEnd-3 -f_Ay)/f_By) // down from the origin:
	{
	  ytic = calcY(dy);
	  YTic.sprintf("%.8g", isYTicsBiased?dy-YTicsBias:dy);
	  P->drawLine(XFrameEnd,   ytic, XFrameEnd-10,   ytic);
	  P->drawLine(XFrameBegin, ytic, XFrameBegin+10, ytic);
	  P->drawText(XFrameBegin-YTicsWidth-10, ytic-LabelsHeight/2, YTicsWidth, LabelsHeight, 
		      TicAlignFlag, YTic);
	  
	  for (i=0; i<9; i++)
	    {
	      if ((ytic = calcY(dy - YTicsStep*(i+1)/10.0))<YFrameEnd-3)
		{
		  P->drawLine(XFrameEnd,   ytic, XFrameEnd  - ((i!=4)?3:6), ytic);
		  P->drawLine(XFrameBegin, ytic, XFrameBegin+ ((i!=4)?4:7), ytic);
		};
	    };
	  dy-=YTicsStep;
	};
      for (i=0; i<9; i++) // draw upward from the origin:
	if ((ytic = calcY(y_0 + YTicsStep*(i+1)/10.0))>YFrameBegin+3)
	  {
	    P->drawLine(XFrameEnd,   ytic, XFrameEnd  - ((i!=4)?3:6), ytic);
	    P->drawLine(XFrameBegin, ytic, XFrameBegin+ ((i!=4)?4:7), ytic);
	  };
    };
};

void SBPlotArea::drawXTics(QPainter* P)
{
  QString	XTic;
  double	dx=0.0;
  int		xtic=0, i;
  int		TicAlignFlag = Qt::AlignCenter;
  //  TicAlignFlag = (XStepP<0)?Qt::AlignLeft:Qt::AlignRight;

  if (MaxX*MinX<0.0) // there is "0":
    {
      dx=0.0;
      P->setPen(*ZeroPen);
      xtic=calcX(dx);
      P->drawLine(xtic, YFrameBegin+1, xtic, YFrameEnd-1);
      P->setPen(*FramePen);
      while (dx<=MaxX)
	{
	  xtic = calcX(dx);
	  XTic.sprintf("%.8g", dx);
	  P->drawLine(xtic, YFrameEnd,   xtic, YFrameEnd  -10);
	  P->drawLine(xtic, YFrameBegin, xtic, YFrameBegin+10);
	  P->drawText(xtic-XTicsWidth/2, YFrameEnd+LabelsHeight/2, XTicsWidth, LabelsHeight, 
		      TicAlignFlag, XTic);
	  
	  for (i=0; i<9; i++)
	    if ((xtic = calcX(dx + XTicsStep*(i+1)/10.0))<XFrameEnd-3)
	      {
		P->drawLine(xtic, YFrameEnd,   xtic, YFrameEnd  - ((i!=4)?3:6));
		P->drawLine(xtic, YFrameBegin, xtic, YFrameBegin+ ((i!=4)?4:7));
	      };
	  dx+=XTicsStep;
	};
      for (i=0; i<9; i++)
	if ((xtic = calcX(-XTicsStep*(i+1)/10.0))>XFrameBegin+3)
	  {
	    P->drawLine(xtic, YFrameEnd,   xtic, YFrameEnd  - ((i!=4)?3:6));
	    P->drawLine(xtic, YFrameBegin, xtic, YFrameBegin+ ((i!=4)?4:7));
	  };
      dx=-XTicsStep;
      while (dx>=MinX)
	{
	  xtic = calcX(dx);
	  XTic.sprintf("%.8g", dx);
	  P->drawLine(xtic, YFrameEnd,   xtic, YFrameEnd  -10);
	  P->drawLine(xtic, YFrameBegin, xtic, YFrameBegin+10);
	  P->drawText(xtic-XTicsWidth/2, YFrameEnd+LabelsHeight/2, XTicsWidth, LabelsHeight, 
		      TicAlignFlag, XTic);
	  
	  for (i=0; i<9; i++)
	    if ((xtic = calcX(dx - XTicsStep*(i+1)/10.0))>XFrameBegin+3)
	      {
		P->drawLine(xtic, YFrameEnd,   xtic, YFrameEnd  - ((i!=4)?3:6));
		P->drawLine(xtic, YFrameBegin, xtic, YFrameBegin+ ((i!=4)?4:7));
	      };
	  dx-=XTicsStep;
	};
    }
  else
    {
      // define the origin:
      double	x_0=XTicsStep*floor((XFrameEnd-3-f_Ax)/f_Bx/XTicsStep);
      dx =x_0;
      while (dx>(XFrameBegin-f_Ax)/f_Bx) // down from the origin:
	{
	  xtic = calcX(dx);
	  XTic.sprintf("%.8g", isXTicsBiased?dx-XTicsBias:dx);
	  P->drawLine(xtic, YFrameEnd,   xtic, YFrameEnd  -10);
	  P->drawLine(xtic, YFrameBegin, xtic, YFrameBegin+10);
	  P->drawText(xtic-XTicsWidth/2, YFrameEnd+LabelsHeight/2, XTicsWidth, LabelsHeight, 
		      TicAlignFlag, XTic);
	  
	  for (i=0; i<9; i++)
	    {
	      if ((xtic = calcX(dx - XTicsStep*(i+1)/10.0))>XFrameBegin+3)
		{
		  P->drawLine(xtic, YFrameEnd,   xtic, YFrameEnd  - ((i!=4)?3:6));
		  P->drawLine(xtic, YFrameBegin, xtic, YFrameBegin+ ((i!=4)?4:7));
		};
	    };
	  dx-=XTicsStep;
	};
      for (i=0; i<9; i++) // draw upward from the origin:
	if ((xtic = calcX(x_0 + XTicsStep*(i+1)/10.0))<XFrameEnd-3)
	  {
	    P->drawLine(xtic, YFrameEnd,   xtic, YFrameEnd  - ((i!=4)?3:6));
	    P->drawLine(xtic, YFrameBegin, xtic, YFrameBegin+ ((i!=4)?4:7));
	  };
    };
};

void SBPlotArea::drawXmjdTics(QPainter* P)
{
  QString	TTic;
  SBMJD		dt;
  SBMJD		D0((int)trunc(MinX)+1, 0.0); // here is the next midnight of the epoch of the MinX
  QFontMetrics	fm(P->fontMetrics());
  double	tmp;
  int		ttic=0, i;
  int		ss=2, tw=0;
  int		TicAlignFlag = Qt::AlignCenter;
  //  TicAlignFlag = (XStepP<0)?Qt::AlignLeft:Qt::AlignRight;
  int		N4SubTic;

  //std::cout << "XTicsStep: " + QString().setNum(XTicsStep) + "; Mode: " + QString().setNum(XTicsMJD)+"\n";

  switch (XTicsMJD)
    {
    case 0: // XTicsStep in seconds:
      D0 = SBMJD((int)trunc(MinX), 0.0);
      D0+= XTicsStep/86400.0*trunc((SBMJD((XFrameBegin+3 -f_Ax)/f_Bx) - D0)/(XTicsStep/86400.0));

      dt = D0;
      ss = 5;
      N4SubTic = 20;
      while ((double)dt < (XFrameEnd-3 -f_Ax)/f_Bx) // upward from the origin:
	{
	  ttic = calcX(dt);
	  if ( modf(dt, &tmp)*24.0 < 0.1)
	    {
	      TTic = SBMJD(dt).toString(SBMJD::F_DateShort);
	      P->setPen(*ZeroPen);
	      P->drawLine(ttic, YFrameBegin+1, ttic, YFrameEnd-1);
	      P->setPen(*FramePen);
	    }
	  else
	    TTic = SBMJD(dt).toString(SBMJD::F_TimeShort);

	  if (ttic > XFrameBegin+3)
	    {
	      tw = fm.width(TTic);
	      P->drawLine(ttic, YFrameEnd,   ttic, YFrameEnd  -10);
	      P->drawLine(ttic, YFrameBegin, ttic, YFrameBegin+10);
	      P->drawText(ttic-tw/2, YFrameEnd+LabelsHeight/2, tw, LabelsHeight, 
			  TicAlignFlag, TTic);
	    };
	  for (i=0; i<N4SubTic-1; i++)
	    if ((ttic = calcX(dt + XTicsStep/86400.0*(i+1)/N4SubTic))<XFrameEnd-3 && ttic>XFrameBegin+3)
	      {
		P->drawLine(ttic, YFrameEnd,   ttic, YFrameEnd  - (((i+1)%ss)?3:6));
		P->drawLine(ttic, YFrameBegin, ttic, YFrameBegin+ (((i+1)%ss)?4:7));
	      };
	  dt+=XTicsStep/86400.0;
	};
      for (i=0; i<N4SubTic-1; i++)
	if ((ttic = calcX(D0 - XTicsStep/86400.0*(i+1)/N4SubTic))>XFrameBegin+3  && 
	    ttic>XFrameBegin+3)
          {
	    P->drawLine(ttic, YFrameEnd,   ttic, YFrameEnd  - (((i+1)%ss)?3:6));
	    P->drawLine(ttic, YFrameBegin, ttic, YFrameBegin+ (((i+1)%ss)?4:7));
	  };
      dt = D0 - XTicsStep/86400.0;
      while ((double)dt > (XFrameBegin+3 -f_Ax)/f_Bx) // downward from the origin:
 	{
	  ttic = calcX(dt);
	  if ( modf(dt, &tmp)*86400.0 < 0.1)
	    {
	      TTic = SBMJD(dt).toString(SBMJD::F_DateShort);
	      P->setPen(*ZeroPen);
	      P->drawLine(ttic, YFrameBegin+1, ttic, YFrameEnd-1);
	      P->setPen(*FramePen);
	    }
	  else
	    TTic = SBMJD(dt).toString(SBMJD::F_TimeShort);
	  
	  tw = fm.width(TTic);
	  P->drawLine(ttic, YFrameEnd,   ttic, YFrameEnd  -10);
	  P->drawLine(ttic, YFrameBegin, ttic, YFrameBegin+10);
	  P->drawText(ttic-tw/2, YFrameEnd+LabelsHeight/2, tw, LabelsHeight, 
		      TicAlignFlag, TTic);
	  for (i=0; i<N4SubTic-1; i++)
	    if ((ttic = calcX(dt - XTicsStep/86400.0*(i+1)/N4SubTic))>XFrameBegin+3 
		&& ttic>XFrameBegin+3)
	      {
		P->drawLine(ttic, YFrameEnd,   ttic, YFrameEnd  - (((i+1)%ss)?3:6));
		P->drawLine(ttic, YFrameBegin, ttic, YFrameBegin+ (((i+1)%ss)?4:7));
	      };
	  dt-=XTicsStep/86400.0;
	};
      break;
      
    case 1: // XTicsStep in minutes:
      D0 = SBMJD((int)trunc(MinX), 0.0);
      D0+= XTicsStep/1440.0*trunc((SBMJD((XFrameBegin+3 -f_Ax)/f_Bx) - D0)/(XTicsStep/1440.0));

      dt = D0;
      ss = 1;
      if (XTicsStep>=50) 
	{
	  N4SubTic = 10;
	  ss = 5;
	}
      else if (XTicsStep>=30)
	{
	  N4SubTic = 20;
	  ss=10;
	}
      else if (XTicsStep==20) 
	{
	  N4SubTic = 20;
	  ss=5;
	}
      else if (XTicsStep==10) 
	{
	  N4SubTic = 10;
	  ss=5;
	}
      else if (XTicsStep>=8) 
	{
	  ss = 2;
	  N4SubTic = (int)(ss*XTicsStep);
	}
      else
	{
	  ss = 5;
	  N4SubTic = (int)(ss*XTicsStep);
	};

      while ((double)dt < (XFrameEnd-3 -f_Ax)/f_Bx) // upward from the origin:
	{
	  ttic = calcX(dt);
	  if ( modf(dt, &tmp)*24.0 < 0.1)
	    {
	      TTic = SBMJD(dt).toString(SBMJD::F_DateShort);
	      P->setPen(*ZeroPen);
	      P->drawLine(ttic, YFrameBegin+1, ttic, YFrameEnd-1);
	      P->setPen(*FramePen);
	    }
	  else
	    TTic = SBMJD(dt).toString(SBMJD::F_TimeVShort);

	  if (ttic > XFrameBegin+3)
	    {
	      tw = fm.width(TTic);
	      P->drawLine(ttic, YFrameEnd,   ttic, YFrameEnd  -10);
	      P->drawLine(ttic, YFrameBegin, ttic, YFrameBegin+10);
	      P->drawText(ttic-tw/2, YFrameEnd+LabelsHeight/2, tw, LabelsHeight, 
			  TicAlignFlag, TTic);
	    };
	  for (i=0; i<N4SubTic-1; i++)
	    if ((ttic = calcX(dt + XTicsStep/1440.0*(i+1)/N4SubTic))<XFrameEnd-3 && ttic>XFrameBegin+3)
	      {
		P->drawLine(ttic, YFrameEnd,   ttic, YFrameEnd  - (((i+1)%ss)?3:6));
		P->drawLine(ttic, YFrameBegin, ttic, YFrameBegin+ (((i+1)%ss)?4:7));
	      };
	  dt+=XTicsStep/1440.0;
	};
      for (i=0; i<N4SubTic-1; i++)
	if ((ttic = calcX(D0 - XTicsStep/1440.0*(i+1)/N4SubTic))>XFrameBegin+3  && 
	    ttic>XFrameBegin+3)
          {
	    P->drawLine(ttic, YFrameEnd,   ttic, YFrameEnd  - (((i+1)%ss)?3:6));
	    P->drawLine(ttic, YFrameBegin, ttic, YFrameBegin+ (((i+1)%ss)?4:7));
	  };
      dt = D0 - XTicsStep/1440.0;
      while ((double)dt > (XFrameBegin+3 -f_Ax)/f_Bx) // downward from the origin:
 	{
	  ttic = calcX(dt);
	  if ( modf(dt, &tmp)*1440.0 < 0.1)
	    {
	      TTic = SBMJD(dt).toString(SBMJD::F_DateShort);
	      P->setPen(*ZeroPen);
	      P->drawLine(ttic, YFrameBegin+1, ttic, YFrameEnd-1);
	      P->setPen(*FramePen);
	    }
	  else
	    TTic = SBMJD(dt).toString(SBMJD::F_TimeVShort);
	  
	  tw = fm.width(TTic);
	  P->drawLine(ttic, YFrameEnd,   ttic, YFrameEnd  -10);
	  P->drawLine(ttic, YFrameBegin, ttic, YFrameBegin+10);
	  P->drawText(ttic-tw/2, YFrameEnd+LabelsHeight/2, tw, LabelsHeight, 
		      TicAlignFlag, TTic);
	  for (i=0; i<N4SubTic-1; i++)
	    if ((ttic = calcX(dt - XTicsStep/1440.0*(i+1)/N4SubTic))>XFrameBegin+3 
		&& ttic>XFrameBegin+3)
	      {
		P->drawLine(ttic, YFrameEnd,   ttic, YFrameEnd  - (((i+1)%ss)?3:6));
		P->drawLine(ttic, YFrameBegin, ttic, YFrameBegin+ (((i+1)%ss)?4:7));
	      };
	  dt-=XTicsStep/1440.0;
	};
      break;
      
    case 2: // XTicsStep in hours:
      dt = D0;
      ss = 2;
      if (XTicsStep<4) ss=3;
      if (XTicsStep<3) ss=4;
      if (XTicsStep<2) ss=6;
      while ((double)dt < (XFrameEnd-3 -f_Ax)/f_Bx) // upward from the origin:
	{
	  ttic = calcX(dt);
	  if ( modf(dt, &tmp)*24.0 < 0.1)
	    {
	      TTic = SBMJD(dt).toString(SBMJD::F_DateShort);
	      P->setPen(*ZeroPen);
	      P->drawLine(ttic, YFrameBegin+1, ttic, YFrameEnd-1);
	      P->setPen(*FramePen);
	    }
	  else
	    TTic = SBMJD(dt).toString(SBMJD::F_TimeVShort);
	  
	  tw = fm.width(TTic);
	  P->drawLine(ttic, YFrameEnd,   ttic, YFrameEnd  -10);
	  P->drawLine(ttic, YFrameBegin, ttic, YFrameBegin+10);
	  P->drawText(ttic-tw/2, YFrameEnd+LabelsHeight/2, tw, LabelsHeight, 
		      TicAlignFlag, TTic);
	  for (i=0; i<ss*XTicsStep-1; i++)
	    if ((ttic = calcX(dt + XTicsStep/24.0*(i+1)/(ss*XTicsStep)))<XFrameEnd-3)
	      {
		P->drawLine(ttic, YFrameEnd,   ttic, YFrameEnd  - (((i+1)%ss)?3:6));
		P->drawLine(ttic, YFrameBegin, ttic, YFrameBegin+ (((i+1)%ss)?4:7));
	      };
	  dt+=XTicsStep/24.0;
	};
      for (i=0; i<ss*XTicsStep-1; i++)
	if ((ttic = calcX(D0 - XTicsStep/24.0*(i+1)/(ss*XTicsStep)))>XFrameBegin+3  && ttic<XFrameEnd-3)
	  {
	    P->drawLine(ttic, YFrameEnd,   ttic, YFrameEnd  - (((i+1)%ss)?3:6));
	    P->drawLine(ttic, YFrameBegin, ttic, YFrameBegin+ (((i+1)%ss)?4:7));
	  };
      dt = D0 - XTicsStep/24.0;
      while ((double)dt > (XFrameBegin+3 -f_Ax)/f_Bx) // downward from the origin:
	{
	  ttic = calcX(dt);
	  if ( modf(dt, &tmp)*24.0 < 0.1)
	    {
	      TTic = SBMJD(dt).toString(SBMJD::F_DateShort);
	      P->setPen(*ZeroPen);
	      P->drawLine(ttic, YFrameBegin+1, ttic, YFrameEnd-1);
	      P->setPen(*FramePen);
	    }
	  else
	    TTic = SBMJD(dt).toString(SBMJD::F_TimeVShort);

	  tw = fm.width(TTic);
	  P->drawLine(ttic, YFrameEnd,   ttic, YFrameEnd  -10);
	  P->drawLine(ttic, YFrameBegin, ttic, YFrameBegin+10);
	  P->drawText(ttic-tw/2, YFrameEnd+LabelsHeight/2, tw, LabelsHeight, 
		      TicAlignFlag, TTic);
	  for (i=0; i<ss*XTicsStep-1; i++)
	    if ((ttic = calcX(dt - XTicsStep/24.0*(i+1)/(ss*XTicsStep)))>XFrameBegin+3 && ttic<XFrameEnd-3)
	      {
		P->drawLine(ttic, YFrameEnd,   ttic, YFrameEnd  - (((i+1)%ss)?3:6));
		P->drawLine(ttic, YFrameBegin, ttic, YFrameBegin+ (((i+1)%ss)?4:7));
	      };
	  dt-=XTicsStep/24.0;
	};
      break;

    case 3: // XTicsStep in days:
      dt = D0;
      ss = 6;
      if (XTicsStep<1.0) ss=24;
      else if (XTicsStep<2) ss=12;
      else if (XTicsStep<4) ss=6;
      else if (XTicsStep<6) ss=4;
      else if (XTicsStep<12) ss=2;
      else ss=1;
      while ((double)dt < (XFrameEnd-3 -f_Ax)/f_Bx) // upward from the origin:
	{
	  ttic = calcX(dt);
	  if (modf(dt, &tmp)*24.0 < 0.1)
	    {
	      TTic = SBMJD(dt).toString(SBMJD::F_DateShort);
	      P->setPen(*ZeroPen);
	      P->drawLine(ttic, YFrameBegin+1, ttic, YFrameEnd-1);
	      P->setPen(*FramePen);
	    }
	  else
	    TTic = SBMJD(dt).toString(SBMJD::F_TimeVShort);
	  
	  tw = fm.width(TTic);
	  P->drawLine(ttic, YFrameEnd,   ttic, YFrameEnd  -10);
	  P->drawLine(ttic, YFrameBegin, ttic, YFrameBegin+10);
	  P->drawText(ttic-tw/2, YFrameEnd+LabelsHeight/2, tw, LabelsHeight, 
		      TicAlignFlag, TTic);
	  for (i=0; i<ss*XTicsStep-1; i++)
	    if ((ttic = calcX(dt + XTicsStep*(i+1)/(ss*XTicsStep)))<XFrameEnd-3)
	      {
		P->drawLine(ttic, YFrameEnd,   ttic, YFrameEnd  - (((i+1)%ss)?3:6));
		P->drawLine(ttic, YFrameBegin, ttic, YFrameBegin+ (((i+1)%ss)?4:7));
	      };
	  dt+=XTicsStep;
	};
      for (i=0; i<ss*XTicsStep-1; i++)
	if ((ttic = calcX(D0 - XTicsStep*(i+1)/(ss*XTicsStep)))>XFrameBegin+3)
	  {
	    P->drawLine(ttic, YFrameEnd,   ttic, YFrameEnd  - (((i+1)%ss)?3:6));
	    P->drawLine(ttic, YFrameBegin, ttic, YFrameBegin+ (((i+1)%ss)?4:7));
	  };
      dt = D0 - XTicsStep;
      while ((double)dt > (XFrameBegin+3 -f_Ax)/f_Bx) // downward from the origin:
	{
	  ttic = calcX(dt);
	  if ( modf(dt, &tmp)*24.0 < 0.1)
	    {
	      TTic = SBMJD(dt).toString(SBMJD::F_DateShort);
	      P->setPen(*ZeroPen);
	      P->drawLine(ttic, YFrameBegin+1, ttic, YFrameEnd-1);
	      P->setPen(*FramePen);
	    }
	  else
	    TTic = SBMJD(dt).toString(SBMJD::F_TimeVShort);
	  
	  tw = fm.width(TTic);
	  P->drawLine(ttic, YFrameEnd,   ttic, YFrameEnd  -10);
	  P->drawLine(ttic, YFrameBegin, ttic, YFrameBegin+10);
	  P->drawText(ttic-tw/2, YFrameEnd+LabelsHeight/2, tw, LabelsHeight, 
		      TicAlignFlag, TTic);
	  for (i=0; i<ss*XTicsStep-1; i++)
	    if ((ttic = calcX(dt - XTicsStep*(i+1)/(ss*XTicsStep)))>XFrameBegin+3)
	      {
		P->drawLine(ttic, YFrameEnd,   ttic, YFrameEnd  - (((i+1)%ss)?3:6));
		P->drawLine(ttic, YFrameBegin, ttic, YFrameBegin+ (((i+1)%ss)?4:7));
	      };
	  dt-=XTicsStep;
	};
      break;

    case 4: // XTicsStep in days:
      dt = D0;
      while ((double)dt < (XFrameEnd-3 -f_Ax)/f_Bx) // upward from the origin:
	{
	  ttic = calcX(dt);
	  TTic = SBMJD(dt).toString(SBMJD::F_DateShort);
	  P->setPen(*ZeroPen);
	  P->drawLine(ttic, YFrameBegin+1, ttic, YFrameEnd-1);
	  P->setPen(*FramePen);
	  
	  tw = fm.width(TTic);
	  P->drawLine(ttic, YFrameEnd,   ttic, YFrameEnd  -10);
	  P->drawLine(ttic, YFrameBegin, ttic, YFrameBegin+10);
	  P->drawText(ttic-tw/2, YFrameEnd+LabelsHeight/2, tw, LabelsHeight, 
		      TicAlignFlag, TTic);
	  for (i=0; i<9; i++)
	    if ((ttic = calcX(dt + XTicsStep*(i+1)/(10)))<XFrameEnd-3)
	      {
		P->drawLine(ttic, YFrameEnd,   ttic, YFrameEnd  - (((i+1)%5)?3:6));
		P->drawLine(ttic, YFrameBegin, ttic, YFrameBegin+ (((i+1)%5)?4:7));
	      };
	  dt+=XTicsStep;
	};
      for (i=0; i<9; i++)
	if ((ttic = calcX(D0 - XTicsStep*(i+1)/10))>XFrameBegin+3)
	  {
	    P->drawLine(ttic, YFrameEnd,   ttic, YFrameEnd  - (((i+1)%5)?3:6));
	    P->drawLine(ttic, YFrameBegin, ttic, YFrameBegin+ (((i+1)%5)?4:7));
	  };
      break;
    }; // end of cases

};

void SBPlotArea::drawData(QPainter* P)
{
  if (PlotCarrier->isOK())
    {
      int		i;
      unsigned int	ei;
      SBPlotBranch	*pb;
      bool		IsTMP;
      
      for (i=0, pb=PlotCarrier->branches()->first(); pb; i++, pb=PlotCarrier->branches()->next())
	if (pb->isBrowsable())
	  {
	    IsTMP=FALSE;
	    P->setPen(*(BranchPens+i));
	    if (IsPlotLines && pb->dataAttr(0) && (!(pb->dataAttr(0) & SBPlotCarrier::DA_IGN) || IsAllData))
	      {
		P->moveTo(calcX(pb->data()->at(0, Xcol)), calcY(pb->data()->at(0, Ycol)));
		IsTMP=TRUE;
	      };
	    for (unsigned int j=0; j<pb->data()->nRow(); j++)
	      if (!(pb->dataAttr(j) & SBPlotCarrier::DA_IGN) || IsAllData)
		{
		  int y_u=0;
		  int y_d=0;
		  int x=calcX(pb->data()->at(j, Xcol)), y=calcY(pb->data()->at(j, Ycol));
		  if (IsPlotLines)
		    {
		      if (!IsTMP)
			{
			  P->moveTo(x, y);
			  IsTMP=TRUE;
			}
		      else
			P->lineTo(x, y);
		    };
		  if (IsPlotPoints)
		    {
		      P->drawPoint(x-1, y);
		      P->drawPoint(x  , y);
		      P->drawPoint(x+1, y);
		      P->drawPoint(x, y-1);
		      P->drawPoint(x, y+1);
		    };
		  if (IsPlotErrBars && (ei=PlotCarrier->stdVarIdx(Ycol)))
		    {
		      y_u = std::max(calcY(pb->data()->at(j, Ycol) + pb->data()->at(j, ei)), YDataBegin);
		      y_d = std::min(calcY(pb->data()->at(j, Ycol) - pb->data()->at(j, ei)), YDataEnd);

		      P->drawLine(x, y_u, x, y_d);
		      if (IsPlotLines)
			P->moveTo(x, y);
		    };
		  if (IsPlotImpulses)
		    {
		      if (pb->data()->at(j, Ycol)>0.0)
			{
			  y_u = calcY(pb->data()->at(j, Ycol));
			  y_d = std::min(calcY(0.0), YDataEnd);
			}
		      else
			{
			  y_u = std::max(calcY(0.0), YDataBegin);
			  y_d = calcY(pb->data()->at(j, Ycol));
			};

		      P->drawLine(x, y_u, x, y_d);
		      if (IsPlotLines)
			P->moveTo(x, y);
		    };

		  // draw bars:
		  if (pb->dataAttr(j) & SBPlotCarrier::DA_BAR)
		    {
		      P->setPen(*BarPen);
		      P->drawLine(x, YDataBegin, x, YDataEnd);
		      P->setPen(*(BranchPens+i));
		      if (IsPlotLines)
			P->moveTo(x, y);
		    };
		};
	  };
    };
};

void SBPlotArea::output4Files()
{
  QString	FileName = PlotCarrier->file4SaveBaseName() + QString().sprintf("[%u:%u]", Xcol, Ycol);
  QString	buff;
  QFile		f;
  QTextStream	s;

  int		i;
  unsigned int	ei;
  SBPlotBranch	*pb;
  for (i=0, pb=PlotCarrier->branches()->first(); pb; i++, pb=PlotCarrier->branches()->next())
    if (pb->isBrowsable())
      {
	f.setName(FileName + QString().sprintf("_%03d.dat", i));
	if (f.open(IO_WriteOnly))
	  {      
	    s.setDevice(&f);
	    s << "# " << pb->name() <<  "\n";
	    s << "# X: [" << *XLabel << "]\n";
	    s << "# Y: [" << *YLabel << "]\n";

	    for (unsigned int j=0; j<pb->data()->nRow(); j++)
	      if (!(pb->dataAttr(j) & SBPlotCarrier::DA_IGN) || IsAllData)
		{
		  s << QString().sprintf("%20.8f  %20.8f ", 
					 pb->data()->at(j, Xcol), pb->data()->at(j, Ycol));
		  if ((ei=PlotCarrier->stdVarIdx(Ycol)))
		    s << QString().sprintf(" %20.8f ", pb->data()->at(j, ei));
		  if (isXTicsMJD)
		    s << SBMJD((long double) pb->data()->at(j, Xcol)).toString(SBMJD::F_YYYYMMDDHHMMSSSS);
		  s << "\n";
		};
	    Log->write(SBLog::DBG, SBLog::IO, ClassName() + ": The data '" + PlotCarrier->name() +
		       "' has been saved to the file " + f.name());
	    f.close();
	    s.unsetDevice();
	  }
	else    
	  Log->write(SBLog::ERR, SBLog::IO, ClassName() + ": could not write " + (QString)f.name());
      };
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBPlot											*/
/*												*/
/*==============================================================================================*/
const double SBPlot::ScaleFactor = M_2_SQRTPI;	// it's a "right" scale factor: 2/sqrt(pi)
SBPlot::SBPlot(SBPlotCarrier* PlotCarrier_, QWidget *parent, unsigned int Modes_, const char * name)
  : QWidget(parent, name)
{
  scaleX = 1.0;
  scaleY = 1.0;
  Modes  = Modes_;
  
  PlotCarrier = PlotCarrier_;
  if (!PlotCarrier->selfCheck())
    Log->write(SBLog::DBG, SBLog::INTERFACE, ClassName() + 
	       ": SelfCheck for [" + PlotCarrier->name() + "] failed");

  QBoxLayout	*Layout = new QVBoxLayout(this, 0, 10);

  Plotter = new QScrollView(this);
  Area	  = new SBPlotArea(PlotCarrier, Plotter->viewport());
  Plotter->addChild(Area);

  if (Modes & PM_HAS_HAVE_ZERO)
    Area->setIsHasHaveZero(TRUE);

  Layout->addWidget(Plotter, 5);
  Layout->addWidget(control());

  Layout->activate();
};

SBPlot::~SBPlot()
{
  if (Plotter) 
    {
      delete Plotter;
      Plotter = NULL;
    };
};

void SBPlot::resizeEvent(QResizeEvent* ev)
{
  QWidget::resizeEvent(ev);
  QSize S=Plotter->size();
  Area->resize((int)(scaleX*(S.width()-4)), (int)(scaleY*(S.height()-4)));
};

QWidget* SBPlot::control()
{
  QGridLayout	*grid;
  QLabel	*label;
  QGroupBox	*gbox;
  QBoxLayout	*Layout, *aLayout;
  QPushButton	*button;
  QSlider	*slider;
  unsigned int	i;


  Controls = new QFrame(this);
  Layout = new QHBoxLayout(Controls, 0, 15);

  // axis selection:
  if (! (Modes & PM_WO_AXIS_NAMES) )
    {
      gbox = new QGroupBox("Axis to plot", Controls);
      grid = new QGridLayout(gbox, 2,2, 10,3);
      
      label	 = new QLabel("X:", gbox);
      label -> setMinimumSize(label->sizeHint());
      grid  -> addWidget(label, 0,0,  Qt::AlignLeft || Qt::AlignVCenter);
      
      label	 = new QLabel("Y:", gbox);
      label -> setMinimumSize(label->sizeHint());
      grid  -> addWidget(label, 1,0,  Qt::AlignLeft || Qt::AlignVCenter);
      
      CBXAxis = new QComboBox(FALSE, gbox);
      CBXAxis->setInsertionPolicy(QComboBox::AtBottom);
      CBYAxis = new QComboBox(FALSE, gbox);
      CBYAxis->setInsertionPolicy(QComboBox::AtBottom);
  
      if (PlotCarrier->nCV()+PlotCarrier->nCE())
	{
	  QString	*s;
	  for (i=0; i<PlotCarrier->nCV(); i++) // columns of values:
	    {
	      s = i<PlotCarrier->columnNames()->count()?PlotCarrier->columnNames()->at(i):NULL;
	      CBXAxis->insertItem(s?*s:"Undesribed data at column #" + QString().setNum(i));
	      CBYAxis->insertItem(s?*s:"Undesribed data at column #" + QString().setNum(i));
	    };
	  for (i=0; i<PlotCarrier->nCE(); i++) // columns of values:
	    {
	      s = (PlotCarrier->nCV()+i<PlotCarrier->columnNames()->count())?
		PlotCarrier->columnNames()->at(PlotCarrier->nCV()+i):NULL;
	      CBXAxis->insertItem(s?*s:"Undesribed data at column #" + 
				  QString().setNum(PlotCarrier->nCV() + i));
	      CBYAxis->insertItem(s?*s:"Undesribed data at column #" + 
				  QString().setNum(PlotCarrier->nCV() + i));
	    };
	  // set defaults:
	  CBXAxis->setCurrentItem(0);
	  CBYAxis->setCurrentItem(PlotCarrier->nCV()+PlotCarrier->nCE()>1?1:0);
	  connect(CBXAxis, SIGNAL(highlighted(int)), SLOT(xAxisChanged(int)));
	  connect(CBYAxis, SIGNAL(highlighted(int)), SLOT(yAxisChanged(int)));
	}
      else 
	{
	  CBXAxis->insertItem("N/A");
	  CBYAxis->insertItem("N/A");
	};
      
      CBXAxis->setMinimumSize(CBXAxis->sizeHint());
      grid->addWidget(CBXAxis, 0,1);
      CBYAxis->setMinimumSize(CBYAxis->sizeHint());
      grid->addWidget(CBYAxis, 1,1);
      
      grid->activate();
      Layout->addWidget(gbox);
    };
  //
  
  
  // branches:
  if (! (Modes & PM_WO_BRANCH_NAMES) )
    {
      LBBranches = new QListBox(Controls);
      if (PlotCarrier->branches()->count())
	{
	  for (SBPlotBranch *pb=PlotCarrier->branches()->first(); pb; pb=PlotCarrier->branches()->next())
	    LBBranches->insertItem(pb->name());
	  connect(LBBranches, SIGNAL(selectionChanged()), SLOT(branchChanged()));
	  LBBranches->setSelectionMode(QListBox::Multi);
	  LBBranches->selectAll(TRUE);
	}
      else
	LBBranches->insertItem("N/A");
      LBBranches->setMinimumWidth(20 + LBBranches->maxItemWidth());
      LBBranches->setMaximumHeight(6*LBBranches->itemHeight());
      Layout->addWidget(LBBranches, 5);
    };
  //
  
  // draw modes:
  QButtonGroup *bgr = new QButtonGroup("Plot data as", Controls);
  aLayout = new QVBoxLayout(bgr, 10,3);
  
  aLayout->addStretch(1);
  CBPoints = new QCheckBox("Points", bgr);
  CBPoints->setMinimumSize(CBPoints->sizeHint());
  // defaults:
  CBPoints->setChecked(TRUE);
  connect(CBPoints, SIGNAL(toggled(bool)), SLOT(DMPointsChanged(bool)));
  if (Modes & PM_WO_DOTS)
    CBPoints->setChecked(FALSE);
  aLayout->addWidget(CBPoints);

  CBLines = new QCheckBox("Lines", bgr);
  CBLines->setMinimumSize(CBLines->sizeHint());
  connect(CBLines, SIGNAL(toggled(bool)), SLOT(DMLinesChanged(bool)));
  if (Modes & PM_LINES)
    CBLines->setChecked(TRUE);
  aLayout->addWidget(CBLines);

  CBErrBars = new QCheckBox("Error bars", bgr);
  CBErrBars->setMinimumSize(CBErrBars->sizeHint());
  connect(CBErrBars, SIGNAL(toggled(bool)), SLOT(DMErrBarsChanged(bool)));
  aLayout->addWidget(CBErrBars);

  CBImpulses = new QCheckBox("Impulses", bgr);
  CBImpulses->setMinimumSize(CBImpulses->sizeHint());
  connect(CBImpulses, SIGNAL(toggled(bool)), SLOT(DMImpulsesChanged(bool)));
  aLayout->addWidget(CBImpulses);
  if (Modes & PM_IMPULSE)
    CBImpulses->setChecked(TRUE);
  aLayout->addStretch(1);

  aLayout->activate();
  Layout->addWidget(bgr);
  //

  // colors:
  gbox = new QGroupBox("Colors (S,V)", Controls);
  grid = new QGridLayout(gbox, 2,2,   12,5);

  grid->addRowSpacing(0, 10);

  slider = new QSlider(0, 255, 50, 255, Qt::Vertical, gbox);
  slider->setTickmarks(QSlider::Left);
  slider->setMinimumSize(slider->sizeHint());
  connect(slider, SIGNAL(valueChanged(int)), SLOT(colorSChanged(int)));
  grid->addWidget(slider, 1,0);

  slider= new QSlider(0, 255, 50, 200, Qt::Vertical, gbox);
  slider->setTickmarks(QSlider::Right);
  slider->setMinimumSize(slider->sizeHint());
  connect(slider, SIGNAL(valueChanged(int)), SLOT(colorVChanged(int)));
  grid->addWidget(slider, 1,1);

  grid->activate();
  Layout->addWidget(gbox);
  //

  // options:
  bgr = new QButtonGroup("Plot Ranges", Controls);
  aLayout = new QVBoxLayout(bgr,  10,3);
  
  aLayout->addStretch(1);
  CBAllData = new QCheckBox("All Data", bgr);
  CBAllData->setMinimumSize(CBAllData->sizeHint());
  connect(CBAllData, SIGNAL(toggled(bool)), SLOT(ODataAllChanged(bool)));
  aLayout->addWidget(CBAllData);

  CBRangeVisible = new QCheckBox("Relaxed", bgr);
  CBRangeVisible->setMinimumSize(CBRangeVisible->sizeHint());
  connect(CBRangeVisible, SIGNAL(toggled(bool)), SLOT(OVisRang(bool)));
  aLayout->addWidget(CBRangeVisible);

  CBWStdVar = new QCheckBox("w/ StdVar", bgr);
  CBWStdVar->setMinimumSize(CBWStdVar->sizeHint());
  CBWStdVar->setChecked(TRUE);
  connect(CBWStdVar, SIGNAL(toggled(bool)), SLOT(OWStdVar(bool)));
  aLayout->addWidget(CBWStdVar);
  aLayout->addStretch(1);

  aLayout->activate();
  Layout->addWidget(bgr);


  // plot size:
  Layout->addStretch(1);
  grid = new QGridLayout(3,3, 1);
  Layout->addLayout(grid, 0);

  button = new QPushButton(">file", Controls);
  button->setMinimumSize(3*button->sizeHint()/4);
  connect (button, SIGNAL(clicked()), SLOT(save2File()));
  grid->addWidget(button, 0,0);

  button = PBZommXOut = new QPushButton("Z'X-", Controls);
  button->setMinimumSize(3*button->sizeHint()/4);
  connect (button, SIGNAL(clicked()), SLOT(zoomXOut()));
  grid->addWidget(button, 1,0);

  button = PBZommOut = new QPushButton("Z'-", Controls);
  button->setMinimumSize(3*button->sizeHint()/4);
  connect (button, SIGNAL(clicked()), SLOT(zoomOut()));
  grid->addWidget(button, 2,0);

  button = PBZommYIn = new QPushButton("Z'Y+", Controls);
  button->setMinimumSize(3*button->sizeHint()/4);
  connect (button, SIGNAL(clicked()), SLOT(zoomYIn()));
  grid->addWidget(button, 0,1);

  button = new QPushButton("O", Controls);
  button->setMinimumSize(3*button->sizeHint()/4);
  connect (button, SIGNAL(clicked()), SLOT(zoomNormalView()));
  grid->addWidget(button, 1,1);

  button = PBZommYOut = new QPushButton("Z'Y-", Controls);
  button->setMinimumSize(3*button->sizeHint()/4);
  connect (button, SIGNAL(clicked()), SLOT(zoomYOut()));
  grid->addWidget(button, 2,1);

  button = PBZommIn = new QPushButton("Z'+", Controls);
  button->setMinimumSize(3*button->sizeHint()/4);
  connect (button, SIGNAL(clicked()), SLOT(zoomIn()));
  grid->addWidget(button, 0,2);

  button = PBZommXIn = new QPushButton("Z'X+", Controls);
  button->setMinimumSize(3*button->sizeHint()/4);
  connect (button, SIGNAL(clicked()), SLOT(zoomXIn()));
  grid->addWidget(button, 1,2);

  button = new QPushButton(">PS", Controls);
  button->setMinimumSize(3*button->sizeHint()/4);
  connect (button, SIGNAL(clicked()), SLOT(save2PS()));
  grid->addWidget(button, 2,2);
  //


  Layout->activate();
  return Controls;
};

void SBPlot::fillAxisNames()
{
  if (Modes & PM_WO_AXIS_NAMES)
    return;

  unsigned int	i;
  int		CurrentX = CBXAxis->currentItem();
  int		CurrentY = CBYAxis->currentItem();
  CBXAxis->clear();
  CBYAxis->clear();
  if (PlotCarrier->nCV()+PlotCarrier->nCE())
    {
      QString	*s;
      for (i=0; i<PlotCarrier->nCV(); i++) // columns of values:
	{
	  s = i<PlotCarrier->columnNames()->count()?PlotCarrier->columnNames()->at(i):NULL;
	  CBXAxis->insertItem(s?*s:"Undesribed data at column #" + QString().setNum(i));
	  CBYAxis->insertItem(s?*s:"Undesribed data at column #" + QString().setNum(i));
	};
      for (i=0; i<PlotCarrier->nCE(); i++) // columns of errors:
	{
	  s = (PlotCarrier->nCV()+i<PlotCarrier->columnNames()->count())?
	    PlotCarrier->columnNames()->at(PlotCarrier->nCV()+i):NULL;
	  CBXAxis->insertItem(s?*s:"Undesribed data at column #" + 
			      QString().setNum(PlotCarrier->nCV() + i));
	  CBYAxis->insertItem(s?*s:"Undesribed data at column #" + 
			      QString().setNum(PlotCarrier->nCV() + i));
	};
      // set defaults:
      // ??? keep setings...
      if (CBXAxis->count()>CurrentX)
	CBXAxis->setCurrentItem(CurrentX);
      else
	CBXAxis->setCurrentItem(0);
      if (CBYAxis->count()>CurrentY)
	CBYAxis->setCurrentItem(CurrentY);
      else
	CBYAxis->setCurrentItem(PlotCarrier->nCV()+PlotCarrier->nCE()>1?1:0);
      //      connect(CBXAxis, SIGNAL(highlighted(int)), SLOT(xAxisChanged(int)));
      //      connect(CBYAxis, SIGNAL(highlighted(int)), SLOT(yAxisChanged(int)));
    }
  else 
    {
      CBXAxis->insertItem("N/A");
      CBYAxis->insertItem("N/A");
    };
  CBXAxis->setMinimumSize(CBXAxis->sizeHint());
  CBYAxis->setMinimumSize(CBYAxis->sizeHint());
};

void SBPlot::fillBranchesNames()
{
  if (Modes & PM_WO_BRANCH_NAMES)
    return;
  
  LBBranches->clear();
  if (PlotCarrier->branches()->count())
    {
      for (SBPlotBranch *pb=PlotCarrier->branches()->first(); pb; pb=PlotCarrier->branches()->next())
	LBBranches->insertItem(pb->name());
      //      connect(LBBranches, SIGNAL(selectionChanged()), SLOT(branchChanged()));
      LBBranches->setSelectionMode(QListBox::Multi);
      LBBranches->selectAll(TRUE);
    }
  else
    LBBranches->insertItem("N/A");
  //  LBBranches->setMinimumWidth(20+LBBranches->maxItemWidth());
  //  ???????????????
  //  LBBranches->setMinimumWidth(LBBranches->maxItemWidth());
  //  LBBranches->setMaximumHeight(6*LBBranches->itemHeight());
};

void SBPlot::dataChanged() 
{
  Area->dataChanged();
  fillAxisNames(); 
  fillBranchesNames(); 
  Area->update();
};

void SBPlot::xAxisChanged(int n)
{
  Area->setXCol(n);
  Area->update();
};

void SBPlot::yAxisChanged(int n)
{
  Area->setYCol(n);
  Area->update();
};

void SBPlot::colorSChanged(int n)
{
  Area->setBPSaturation(n);
};

void SBPlot::colorVChanged(int n)
{
  Area->setBPValue(n);
};

void SBPlot::branchChanged()
{
  for (unsigned int i=0; i<LBBranches->count(); i++)
    {
      if (LBBranches->isSelected(i))
	PlotCarrier->branches()->at(i)->setIsBrowsable(TRUE);
      else
	PlotCarrier->branches()->at(i)->setIsBrowsable(FALSE);
    };
  Area->update();
};

void SBPlot::DMPointsChanged(bool Is)
{
  Area->setIsPlotPoints(Is);
  Area->update();
};

void SBPlot::DMLinesChanged(bool Is)
{
  Area->setIsPlotLines(Is);
  Area->update();
};

void SBPlot::DMErrBarsChanged(bool Is)
{
  Area->setIsPlotErrBars(Is);
  Area->update();
};

void SBPlot::DMImpulsesChanged(bool Is)
{
  Area->setIsPlotImpulses(Is);
  Area->update();
};

void SBPlot::ODataAllChanged(bool Is)
{
  Area->setIsAllData(Is);
  Area->update();
};

void SBPlot::OVisRang(bool Is)
{
  Area->setIsLimitsOnVisible(Is);
  Area->update();
};

void SBPlot::OWStdVar(bool Is)
{
  Area->setIsStdVar(Is);
  Area->update();
};

void SBPlot::zoomNormalView()
{
  scaleX = scaleY = 1.0;
  QSize S=Plotter->size();
  Area->resize((int)(scaleX*(S.width()-4)), (int)(scaleY*(S.height()-4)));

  if (!PBZommOut->isEnabled())
    PBZommOut->setEnabled(TRUE);
  if (!PBZommIn->isEnabled())
    PBZommIn->setEnabled(TRUE);
  if (!PBZommXOut->isEnabled())
    PBZommXOut->setEnabled(TRUE);
  if (!PBZommXIn->isEnabled())
    PBZommXIn->setEnabled(TRUE);
  if (!PBZommYOut->isEnabled())
    PBZommYOut->setEnabled(TRUE);
  if (!PBZommYIn->isEnabled())
    PBZommYIn->setEnabled(TRUE);
};

void SBPlot::zoomIn()
{
  QSize S=Area->size();
  if (S.width()<8000 && S.height()<8000)
    {
      scaleX *= ScaleFactor;
      scaleY *= ScaleFactor;
      int centX = Plotter->contentsX() + Plotter->visibleWidth()/2;
      int centY = Plotter->contentsY() + Plotter->visibleHeight()/2;
      Area->resize((int)(S.width() *ScaleFactor), (int)(S.height()*ScaleFactor));
      Plotter->center((int)(centX*ScaleFactor), (int)(centY*ScaleFactor));
      if (!PBZommOut->isEnabled())
	PBZommOut->setEnabled(TRUE);
    }
  else
    PBZommIn->setDisabled(TRUE);
};

void SBPlot::zoomOut()
{
  QSize S=Area->size();
  if (S.width()>160 && S.height()>100)
    {
      scaleX /= ScaleFactor;
      scaleY /= ScaleFactor;
      int centX = Plotter->contentsX() + Plotter->visibleWidth()/2;
      int centY = Plotter->contentsY() + Plotter->visibleHeight()/2;
      Area->resize((int)(S.width() /ScaleFactor), (int)(S.height()/ScaleFactor));
      Plotter->center((int)(centX/ScaleFactor), (int)(centY/ScaleFactor));
      if (!PBZommIn->isEnabled())
	PBZommIn->setEnabled(TRUE);
    }
  else
    PBZommOut->setDisabled(TRUE);
};

void SBPlot::zoomXIn()
{
  QSize S=Area->size();
  if (S.width()<8000)
    {
      scaleX *= ScaleFactor;
      int centX = Plotter->contentsX() + Plotter->visibleWidth()/2;
      int centY = Plotter->contentsY() + Plotter->visibleHeight()/2;
      Area->resize((int)(S.width() *ScaleFactor), S.height());
      Plotter->center((int)(centX*ScaleFactor), centY);
      if (!PBZommXOut->isEnabled())
	PBZommXOut->setEnabled(TRUE);
    }
  else
    PBZommXIn->setDisabled(TRUE);
};

void SBPlot::zoomXOut()
{
  QSize S=Area->size();
  if (S.width()>160)
    {
      scaleX /= ScaleFactor;
      int centX = Plotter->contentsX() + Plotter->visibleWidth()/2;
      int centY = Plotter->contentsY() + Plotter->visibleHeight()/2;
      Area->resize((int)(S.width()/ScaleFactor), S.height());
      Plotter->center((int)(centX/ScaleFactor), centY);
      if (!PBZommXIn->isEnabled())
	PBZommXIn->setEnabled(TRUE);
    }
  else
    PBZommXOut->setDisabled(TRUE);
};

void SBPlot::zoomYIn()
{
  QSize S=Area->size();
  if (S.height()<8000)
    {
      scaleY *= ScaleFactor;
      int centX = Plotter->contentsX() + Plotter->visibleWidth()/2;
      int centY = Plotter->contentsY() + Plotter->visibleHeight()/2;
      Area->resize(S.width(), (int)(S.height()*ScaleFactor));
      Plotter->center(centX, (int)(centY*ScaleFactor));
      if (!PBZommYOut->isEnabled())
	PBZommYOut->setEnabled(TRUE);
    }
  else
    PBZommYIn->setDisabled(TRUE);
};

void SBPlot::zoomYOut()
{
  QSize S=Area->size();
  if (S.height()>100)
    {
      scaleY /= ScaleFactor;
      int centX = Plotter->contentsX() + Plotter->visibleWidth()/2;
      int centY = Plotter->contentsY() + Plotter->visibleHeight()/2;
      Area->resize(S.width(), (int)(S.height()/ScaleFactor));
      Plotter->center(centX, (int)(centY/ScaleFactor));
      if (!PBZommYIn->isEnabled())
	PBZommYIn->setEnabled(TRUE);
    }
  else
    PBZommYOut->setDisabled(TRUE);
};

void SBPlot::save2File()
{
  Area->output4Files();
};

void SBPlot::save2PS()
{
  QString	FileName = PlotCarrier->file4SaveBaseName() + 
    QString().sprintf("[%u:%u].ps", Area->xCol(), Area->yCol());
  QPrinter	Printer;
  Printer.setOutputFileName(FileName);
  Printer.setOutputToFile(TRUE);
  Printer.setOrientation(QPrinter::Landscape);
  Printer.setFullPage(TRUE);
  Printer.setPageSize(QPrinter::A4);
  Printer.setCreator(Version.selfName() + " " + Version.toString());
  Printer.setFullPage(TRUE);
//Printer.setup();

  QPaintDeviceMetrics Metrics(&Printer);

  QPainter P;
  
  P.begin(&Printer);

  QFont Font = P.font();
  Font.setPointSize(16); // set a fixed size to fit to the A4 format page
  P.setFont(Font);

  Area->output4Print(&P, Metrics.width(), Metrics.height());
  P.end();
  Log->write(SBLog::DBG, SBLog::IO, ClassName() + ": The '" + PlotCarrier->name() +
	     "' has been saved to the file " + FileName);
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBPlotDialog.										*/
/*												*/
/*==============================================================================================*/
SBPlotDialog::SBPlotDialog(SBPlotCarrier* Carrier_, bool IsNeedDeletePC_, 
			   QWidget* parent, const char* name, bool modal, WFlags f)
  : QDialog(parent, name, modal, f)
{
  Carrier = Carrier_;
  IsNeedDeletePC = IsNeedDeletePC_;
  setCaption("Plot(s) of " + Carrier->name());
  
  QPushButton	*button;
  QBoxLayout	*Layout, *SubLayout;

  Layout= new QVBoxLayout(this, 20, 20);
  Plot  = new SBPlot(Carrier, this);
  Layout->addWidget(Plot, 5);

  QFrame	*bar = new QFrame(this);
  bar->setFrameStyle(QFrame::HLine | QFrame::Sunken);
  Layout->addWidget(bar);

  /*
    QPushButton	*button;
    QFrame	*body = new QFrame(this);
    body->setFrameStyle(QFrame::Panel | QFrame::Raised);
    
    QBoxLayout	*Layout, *SubLayout;
    
    Layout= new QVBoxLayout(body, 20, 20);
    Plot  = new SBPlot(Carrier, body);
    Layout->addWidget(Plot);
    Layout->activate();
    
    Layout = new QVBoxLayout(this, 20, 20);
    Layout ->addWidget(body, 5);
  */

  SubLayout = new QHBoxLayout(10);
  Layout -> addLayout(SubLayout);
  SubLayout-> addStretch(1);
  
  SubLayout-> addWidget(button=new QPushButton("Close", this));
  button -> setMinimumSize(button->sizeHint());
  connect(button, SIGNAL(clicked()), SLOT(accept()));
  
  Layout->activate();
};

SBPlotDialog::~SBPlotDialog()
{
  if (Plot)
    {
      delete Plot;
      Plot = NULL;
    };

  if (IsNeedDeletePC && Carrier)
    {
      delete Carrier;
      Carrier = NULL;
    };

  emit windowClosed();
};

void SBPlotDialog::accept()
{
  QDialog::accept();
  delete this;
};
/*==============================================================================================*/
