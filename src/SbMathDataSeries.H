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
 *
 * Overview:	I/O manipulations of time series;
 *
 */


#ifndef SB_MATH_DATASERIES_H
#define SB_MATH_DATASERIES_H

#include <config.h>
#include <complex>

#include "SbGeneral.H"
#include "SbVector.H"


/*==============================================================================================*/
/*												*/
/* class SBDataSeriesEntry									*/
/*												*/
/*==============================================================================================*/
class SBDataSeriesEntry
{
private:
  double	        T;
  SBVector*		Data;
  bool			IsReadOK;

public:
  SBDataSeriesEntry() {T=0; Data=NULL; IsReadOK=FALSE;};
  SBDataSeriesEntry(const SBDataSeriesEntry& Entry) 
  {
    T=Entry.T;
    Data=new SBVector(*Entry.Data);
    IsReadOK=Entry.IsReadOK;
  };
  virtual ~SBDataSeriesEntry(){if (Data){delete Data; Data=NULL;};};
  virtual QString ClassName() const {return "SBDataSeriesEntry";};

  bool		isReadOK() const {return IsReadOK;};
  double	t() const {return T;};
  SBVector*	data() {return Data;};
  int		n() const {return Data?Data->n():0;};
  
  friend QTextStream &operator<<(QTextStream&, const SBDataSeriesEntry&);
  friend QTextStream &operator>>(QTextStream&, SBDataSeriesEntry&);
};
/*==============================================================================================*/



/*==============================================================================================*/
class SBDataSeries : public QList<SBDataSeriesEntry>
{
protected:
  virtual int compareItems(QCollection::Item item1, QCollection::Item item2)
  {return (((SBDataSeriesEntry*)item1)->t()==((SBDataSeriesEntry*)item2)->t())?0:
      ((((SBDataSeriesEntry*)item1)->t()<((SBDataSeriesEntry*)item2)->t())?-1:1);};
  
  void	checkRecords();

private:
  int		NumOfDataColumns;	//!< the number of columns in the data part (not counting the arg)
  double	DT;			//!< [mean] time spacing interval
  double	DTMin;			//!< min time spacing interval
  double	DTMax;			//!< max time spacing interval
  bool		IsEquiDistant;		//!< true if DT is const
  bool		IsChecked;

public:
  // constructors/destructors:
  //
  SBDataSeries();
  virtual ~SBDataSeries();
  virtual QString ClassName() const {return "SBDataSeries";};
  
  // interface:
  //
  int		numOfDataColumns() const {return NumOfDataColumns;};
  double	dT() const {return DT;};
  double	dTMin() const {return DTMin;};
  double	dTMax() const {return DTMax;};
  bool		isEquiDistant() const {return IsEquiDistant;};
  bool		isChecked() const {return IsChecked;};
  

  // functions:
  //


  // friends:
  //
  
  // I/O:
  //
  friend QTextStream &operator<<(QTextStream&, const SBDataSeries&);
  friend QTextStream &operator>>(QTextStream&, SBDataSeries&);
};
/*==============================================================================================*/



/*==============================================================================================*/
#endif //SB_MATH_DATASERIES_H
