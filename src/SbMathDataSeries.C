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
 */

#include "SbMathDataSeries.H"

//#include <qfile.h>
//#include <qmessagebox.h>


/*==============================================================================================*/
/*												*/
/* SBDataSeriesEntry's friends implementation							*/
/*												*/
/*==============================================================================================*/
QTextStream &operator<<(QTextStream& s, const SBDataSeriesEntry& entry)
{
  QString	str;
  if (entry.Data && entry.Data->n()>0)
    {
      str.sprintf("%.4f ", entry.T);
      for (unsigned int i=0; i<entry.Data->n(); i++)
	str.sprintf(" %.4f", entry.Data->at(i));
      s << str << "\n";
    }
  return s;
};

QTextStream &operator>>(QTextStream& s, SBDataSeriesEntry& entry)
{
  QString	str;
  bool		IsOK;
  int		i, j, l, m, idx;
  double	d;
  int		NumOfFields=0;

  str = s.readLine().simplifyWhiteSpace();
  if (!(l=str.length()))
    return s;

  if (str.mid(0,1)=="#")	// it's a comment
    return s;

  // determine the number of fields:
  i=0;
  j=0;
  while (i<l)
    {
      j=str.find(" ", i+1);
      if (j!=-1)
	{
	  NumOfFields++;
	  i=j;
	}
      else 
	i=l;
    };
  
  if (!NumOfFields)	// only one field
    return s;
  
  // create the data records:
  entry.Data = new SBVector(NumOfFields, "data record");

  // get the argument:
  j=str.find(" ", 0);
  d=str.mid(0, j).toDouble(&IsOK);
  if (!IsOK)		// arg is not a double
    return s;
  entry.T=d;
  
  // get the fields of data:
  i=j;
  idx=0;
  m=0;
  while ((i!=-1) && IsOK)
    {
      j=str.find(" ", i+1);
      if (j>i)
	{
	  d = str.mid(i+1,j-i-1).toDouble(&IsOK);
	  if (IsOK)
	    {
	      entry.Data->set(idx, d);
	      idx++;
	    };
	  m++;
	}
      else if (m+1==NumOfFields) // last field
	{
	  d = str.mid(i+1).toDouble(&IsOK);
	  if (IsOK)
	    {
	      entry.Data->set(idx, d);
	      idx++;
	    };
	};
      i=j;
    };

  if (IsOK)
    entry.IsReadOK = TRUE;

  return s;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBDataSeries implementation									*/
/*												*/
/*==============================================================================================*/
SBDataSeries::SBDataSeries()
{
  setAutoDelete(TRUE);
  NumOfDataColumns = 0;
  DT = 0.0;
  IsEquiDistant = FALSE;
  IsChecked = FALSE;
};

SBDataSeries::~SBDataSeries()
{

};

void SBDataSeries::checkRecords()
{
  int		Num;

  DT    = 0.0;
  DTMin = 0.0;
  DTMax = 0.0;
  IsChecked = FALSE;
  NumOfDataColumns = 0;

  if ((Num=count()))
    {
      unsigned int	idx=0;
      unsigned int	NoC=0, MaxNoC=0, MinNoC=0, n;
      double		DT_sum=0.0, DT_tmp=0.0;
      double		TPrev =0.0;

      sort();
      IsEquiDistant = TRUE;

      for (SBDataSeriesEntry *E=first(); E; E=next(), idx++)
	{
	  if (idx)
	    {
	      DT_sum+= (DT_tmp = E->t() - TPrev);
	      if (DT==0.0)
		DT = DTMin = DTMax = DT_tmp;
	      else if (DT!=DT_tmp)
		{
		  if (DTMin > DT_tmp)
		    DTMin = DT_tmp;
		  if (DTMax < DT_tmp)
		    DTMax = DT_tmp;
		  IsEquiDistant = FALSE;
		};
	    }
	  else
	    NoC=MaxNoC=MinNoC = E->data()->n();
	  TPrev = E->t();
	  if ((n=E->data()->n()) != NoC)
	    {
	      if (n<MinNoC)
		MinNoC = n;
	      if (MaxNoC<n)
		MaxNoC = n;
	    };
	};

      if (!IsEquiDistant)
	DT = DT_sum/(Num-1);
      
      if (MaxNoC != MinNoC) // well, remove them..
	{
	  int	NRm=0;
	  for (idx=0; idx<count(); idx++)
	    if (at(idx)->data()->n() != MaxNoC)
	      {
		if (remove(idx))
		  {
		    NRm++;
		    idx--;
		  }
		else
		  Log->write(SBLog::ERR, SBLog::DATA, ClassName() + ": cannot remove the record #" + 
			     QString("_").setNum(idx) + " from the series");
	      }
	  Log->write(SBLog::DBG, SBLog::DATA, ClassName() + ": the series is cleaned, " +
		     QString("_").setNum(NRm) + " record freed");
	};
      
      NumOfDataColumns = MaxNoC;
      IsChecked = TRUE;
    };
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBDataSeries's friends implementation							*/
/*												*/
/*==============================================================================================*/
QTextStream &operator<<(QTextStream& s, const SBDataSeries& series)
{
  s << "# The output of the series\n";

  SBDataSeriesEntry *E=NULL;
  QListIterator<SBDataSeriesEntry> it(series);
  for(; (E=it.current()); ++it)
    s << *E;
  return s;
};

QTextStream &operator>>(QTextStream& s, SBDataSeries& series)
{
  SBDataSeriesEntry	*E;

  series.clear();

  while (!s.eof())
    {
      E = new SBDataSeriesEntry;
      s>>*E;
      if (E->isReadOK())
	{
	  series.append(E);
	  //	  std::cout << "t=" << E->t() << " x=" << 
	  //	    E->data()->at(0) << " y=" << E->data()->at(1) << "\n";
	}
      else 
	delete E;
    };
  series.checkRecords();
  return s;
};
/*==============================================================================================*/



/*==============================================================================================*/
