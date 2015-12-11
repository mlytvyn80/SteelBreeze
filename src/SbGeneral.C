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

#include "SbGeneral.H"

#include <qdatetime.h>
#include <qfile.h>
#include <qtextstream.h>


SBLog		*Log;


/*==============================================================================================*/
/*												*/
/* SBLog implementation										*/
/*												*/
/*==============================================================================================*/
SBLog::SBLog(int Capacity_, bool IsStoreInFile_, const QString& FileName_)
{
  Capacity	= Capacity_;
  IsStoreInFile	= IsStoreInFile_;
  FileName	= FileName_;

  LogFacilities[0] = 0xFFFFFFFFL;
  LogFacilities[1] = 0xFFFFFFFFL;
  LogFacilities[2] = 0xFFFFFFFFL;
  LogFacilities[3] = 0xFFFFFFFFL;
  IsTimeLabel	   = TRUE;
  Spool.setAutoDelete(TRUE);
};

SBLog::~SBLog() 
{
  clearSpool(); 
};

void SBLog::startUp()
{
  //just explain:
  QString Tmp;
  Tmp.sprintf ("Capacity: %d; log file name: \"", Capacity);
  if (IsStoreInFile) 
    Tmp += FileName;
    Tmp += "\"";
  write(DBG, IO, ClassName() + ": started with parameters: " + Tmp);
};

//all objects should use SBLog::Write(const String)
//to log something
void SBLog::write(LogLevel Level, uint Facility, const QString &s, bool IsAsync)
{
  if (LogFacilities[Level] & Facility)
    {
      if (IsTimeLabel) 
	{
	  QTime time = QTime::currentTime();
	  TmpStr = time.toString() + " ";
	}
      else 
	TmpStr = "";
      TmpStr += s;
      
      Spool.append( new QString(TmpStr) );
      MakeOutput(TmpStr, IsAsync);
      if (Spool.count()==Capacity) clearSpool();
    };
};

void SBLog::clearSpool()
{
  if (IsStoreInFile)
    {
      QFile LogFile(FileName);
      if (LogFile.open(IO_WriteOnly | IO_Append))
	{
	  QTextStream ts (&LogFile);
	  QListIterator<QString> i(Spool);
	  for (; i.current(); ++i) ts << *i.current() << "\n";
	}
      LogFile.close();
    }
  Spool.clear();
};
/*==============================================================================================*/
