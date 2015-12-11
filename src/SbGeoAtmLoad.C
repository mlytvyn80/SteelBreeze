/*
 *
 * This file is part of SteelBreeze.
 *
 * SteelBreeze, the geodetic VLBI data analysing software.
 * Copyright (C) 2003 Sergei Bolotin, MAO NASU, Kiev, Ukraine.
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

#include <math.h>

#include <qfile.h>
#include <qregexp.h>


#include "SbGeoAtmLoad.H"
#include "SbGeoObsVLBI.H"
#include "SbMathSpline.H"
#include "SbSetup.H"




/*==============================================================================================*/
/*												*/
/* class SBAploEntry implementation								*/
/*												*/
/*==============================================================================================*/
SBAploEntry::SBAploEntry(const QString& Name_, const SBStationID& ID_)
  : SBNamed(Name_), QList<SBAploDRec>()
{
  setAutoDelete(TRUE);
  ID = ID_;

};

SBAploEntry::~SBAploEntry()
{
  
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBAploHeader implementation								*/
/*												*/
/*==============================================================================================*/
SBAploHeader::SBAploHeader()
{
  TStart = TZero;
  TFinis = TZero;
  Interval = 0.0;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBAploChunk implementation								*/
/*												*/
/*==============================================================================================*/
SBAploChunk::SBAploChunk()
  : QList<SBAploEntry>(), SBAploHeader()
{
  setAutoDelete(TRUE);
  EntryByID = new QDict<SBAploEntry>;
  EntryByID->setAutoDelete(FALSE);
  EntryByDomes = new QIntDict<SBAploEntry>;
  EntryByDomes->setAutoDelete(FALSE);
};

SBAploChunk::~SBAploChunk()
{
  if (EntryByID)
    {
      delete EntryByID;
      EntryByID = NULL;
    };
  if (EntryByDomes)
    {
      delete EntryByDomes;
      EntryByDomes = NULL;
    };
};

SBAploEntry* SBAploChunk::entryByID(const SBStationID& ID)
{
  SBAploEntry	*E = EntryByID->find(ID.toString());
  if (!E)
    E = EntryByDomes->find(ID.domeMajor());
  return E;
};

void SBAploChunk::fillDict()
{
  EntryByID->clear();
  EntryByDomes->clear();
  for (SBAploEntry *E=first(); E; E=next())
    {
      if (!EntryByID->find(E->id().toString()))
	EntryByID->insert(E->id().toString(), E);
      else
	Log->write(SBLog::DBG, SBLog::TIDES, ClassName() + ": the entry " + E->name() +
		   " [" + E->id().toString() + "] already in the dict 'EntryByID'");

      SBAploEntry *E_prev = NULL;
      if (!(E_prev=EntryByDomes->find(E->id().domeMajor())))
	EntryByDomes->insert(E->id().domeMajor(), E);
      else
	{
	  if (E->count() > E_prev->count())
	    {
	      EntryByDomes->replace(E->id().domeMajor(), E);

	      Log->write(SBLog::WRN, SBLog::TIDES, ClassName() + ": the entry " + E->name() +
			 " [" + E->id().toString() + "] replaced the entry " + E_prev->name() +
			 " {" + E_prev->id().toString() + "] in the dict 'EntryByDomes'");
	      
	    };  
	};
    };
  EntryByID->resize(EntryByID->count() + 5);
  EntryByDomes->resize(EntryByDomes->count() + 5);
};

void SBAploChunk::import(QTextStream& s, SB_TRF* TRF, bool& IsTRFModified)
{
  SBPMNNR_NUVEL1A	Plates;

  QString	Str = s.readLine();
  bool		IsOK = FALSE;
  int		NumOfSRecs = -1;
  int		NumOfERecs = -1;
  int		NumOfDRecs = -1;
  SBMJD		T;
  int		K = 0;
  int		MJDdate = 0;
  double	MJDtime = 0.0;
  double	d = 0.0;

  SBAploEntry	*Entry = NULL;
  SBStation	*Station = NULL;
  SBStationID	ID;
  QString	SiteID;
  Vector3	R(v3Zero);
  SBAploDRec	*DRec = NULL;
  double	ValidD = 1.0; //meters

  clear();


  //# 1) Header record -- the first record should have a signature:
  //#                     EPHEDISP  Format version of 2005.06.30
  //  if (Str.find("EPHEDISP  Format version of 2002.12.12") != 0)
  if (Str.find("EPHEDISP  Format version of 2005.06.30") != 0)
    {
      Log->write(SBLog::ERR, SBLog::TIDES, ClassName() + ": the string [" + Str +
		 "] is not a header of EPHEDISP; file ignored");
      return;
    };

  while (!s.atEnd())
    {
      Str = s.readLine();

      if (Str.find("P")==0)
	{
	  //#     # sites # epochs # points
	  //          1         2         3
	  //01234567890123456789012345678901234567890   : EPHEDISP  Format version of 2005.06.30
	  //P T 3 S        622 E    124 D      77128
	  //P T 3 S        622 E     24 D      14928
	  NumOfSRecs = Str.mid(8,10).toInt(&IsOK);
	  if (!IsOK)
	    {
	      Log->write(SBLog::ERR, SBLog::TIDES, ClassName() + ": wrong format of the `P`-record (S), string [" + 
			 Str + "]; file ignored");
	      return;
	    };
	  NumOfERecs = Str.mid(21,6).toInt(&IsOK);
	  if (!IsOK)
	    {
	      Log->write(SBLog::ERR, SBLog::TIDES, ClassName() + ": wrong format of the `P`-record (E), string [" + 
			 Str + "]; file ignored");
	      return;
	    };
	  NumOfDRecs = Str.mid(30,10).toInt(&IsOK);
	  if (!IsOK)
	    {
	      Log->write(SBLog::ERR, SBLog::TIDES, ClassName() + ": wrong format of the `P`-record (D), string [" + 
			 Str + "]; file ignored");
	      return;
	    };
	}
      else if (Str.find("T")==0)
	{
	  if (Str.find("T begin ")==0)
	    {
	      //0123456789012345678901234567890
	      //T begin   53767     0.0  2006.02.01-00:00:00
	      MJDdate = Str.mid(10,5).toInt(&IsOK);
	      if (!IsOK)
		{
		  Log->write(SBLog::ERR, SBLog::TIDES, ClassName() + 
			     ": wrong format of the `T begin `-record (MJD Date), string [" + 
			     Str + "]; file ignored");
		  return;
		};
	      MJDtime = Str.mid(16,7).toDouble(&IsOK);
	      if (!IsOK)
		{
		  Log->write(SBLog::ERR, SBLog::TIDES, ClassName() + 
			     ": wrong format of the `T begin `-record (MJD Time), string [" + 
			     Str + "]; file ignored");
		  return;
		};
	      T.setDate(MJDdate);
	      T.setTime(MJDtime/86400.0);
	      if (T!=TZero && T.isGood())
		setTStart(T);
	    }
	  else if (Str.find("T end   ")==0)
	    {
	      //0123456789012345678901234567890
	      //T end     53772 64800.0  2006.02.06-18:00:00
	      MJDdate = Str.mid(10,5).toInt(&IsOK);
	      if (!IsOK)
		{
		  Log->write(SBLog::ERR, SBLog::TIDES, ClassName() + 
			     ": wrong format of the `T end   `-record (MJD Date), string [" + 
			     Str + "]; file ignored");
		  return;
		};
	      MJDtime = Str.mid(16,7).toDouble(&IsOK);
	      if (!IsOK)
		{
		  Log->write(SBLog::ERR, SBLog::TIDES, ClassName() + 
			     ": wrong format of the `T end   `-record (MJD Time), string [" + 
			     Str + "]; file ignored");
		  return;
		};
	      T.setDate(MJDdate);
	      T.setTime(MJDtime/86400.0);
	      if (T!=TZero && T.isGood())
		setTFinis(T);
	    }
	  else if (Str.find("T sample")==0)
	    {
	      //0123456789012345678901234567890
	      //T sample     0.25000000000
	      d = Str.mid(10,16).toDouble(&IsOK);
	      if (!IsOK)
		{
		  Log->write(SBLog::ERR, SBLog::TIDES, ClassName() + 
			     ": wrong format of the `T sample`-record, string [" + 
			     Str + "]; file ignored");
		  return;
		};
	      if (d>0.0)
		setInterval(d);
	    }
	  else
	    Log->write(SBLog::WRN, SBLog::TIDES, ClassName() + ": unknown string of the `T`-record, string [" + 
		       Str + "] ignored");
	}
      else if (Str.find("A")==0)
	{
	  //0123456789012345
	  //A    3000.000000
	  ValidD = Str.mid(2,14).toDouble(&IsOK);
	  if (!IsOK)
	    {
	      Log->write(SBLog::ERR, SBLog::TIDES, ClassName() + 
			 ": wrong format of the `A`-record, string [" + 
			 Str + "]; file ignored");
	      return;
	    };
	}
      else if (Str.find("S")==0)
	{
	  //          1         2         3         4         5         6         7         8
	  //012345678901234567890123456789012345678901234567890123456789012345678901234567890
	  //S  AIRA      -3530219.3120  4118797.5720  3344015.8670   31.6516 130.5999  323.1
	  //S  DN_21742  -3530219.3120  4118797.5720  3344015.8670   31.6516 130.5999  323.1
	  //S  ARECIBO    2390486.9000  5564731.4400  1994720.4500   18.2295  66.7527  451.9
	  SiteID = Str.mid(3,8);
	  d = Str.mid(13,13).toDouble(&IsOK);
	  if (IsOK)
	    {
	      R[X_AXIS] = d;
	      d = Str.mid(27,13).toDouble(&IsOK);
	      if (IsOK)
		{
		  R[Y_AXIS] = d;
		  d = Str.mid(41,13).toDouble(&IsOK);
		  if (IsOK)
		    R[Z_AXIS] = d;
		};
	    };
	  if (IsOK)
	    {
	      // well, it's undocumented feature:
	      if ( (Station=TRF->lookupStation(SiteID)) )
		{
		  ID = Station->id();
		  Log->write(SBLog::DBG, SBLog::STATION, ClassName() + ": got an alias '" + SiteID +
			     "' for the station " + Station->name() + " [" + ID.toString() + "]");

		  // should we import coordinates here too???
		  if (Station->r_first()==v3Zero)
		    {
		      if (Station->site()->v() == v3Zero)
			{
			  Station->setCoords(R, v3Unit, v3Zero, v3Unit, TZero, "Initial data");
			  Station->setV(Plates.velocity(*Station));
			  Station->site()->setV(Plates.velocity(*Station));
			}
		      else
			Station->setCoords(R, v3Unit, v3Zero, v3Unit, TZero, "Initial data");
		      Log->write(SBLog::INF, SBLog::DATA, 
				 "SBDS_dbh: set up coordinates for \"" + 
				 Station->name() + "\" [" + Station->id().toString() +
				 "] aka [" + SiteID + "] station");
		      Station->addAttr(SBStation::OrigEdited);
		      IsTRFModified = TRUE;
		    };
		}
	      else
		{
		  Station=TRF->lookupNearest(TRF->epoch(), R, d, TECH_ANY);
		  if (d<=ValidD)
		    ID = Station->id();
		  else
		    {
		      Log->write(SBLog::DBG, SBLog::TIDES | SBLog::STATION, ClassName() + ": cannot find station '" + 
				 SiteID + "' in the TRF; the string: [" + Str + "]");
		      Station=NULL;
		    };
		};
	      if (Station && ID.isValidId())
		{
		  Entry = new SBAploEntry(SiteID, ID);
		  if (find(Entry)==-1)
		    inSort(new SBAploEntry(SiteID, ID));
		  else
		    {
		      Log->write(SBLog::WRN, SBLog::STATION, ClassName() + ": wrong data: duplicate station '" + 
				 SiteID + "` [" + ID.toString() + "], string: " + Str);
		      delete Entry;
		    };
		};
	    }
	  else
	    Log->write(SBLog::WRN, SBLog::TIDES, ClassName() + ": wrong format of the `S`-record, string [" + 
		       Str + "]; entry ignored");
	}
      else if (Str.find("D")==0)
	{
	  //          1         2         3         4         5         6         7         8
	  //012345678901234567890123456789012345678901234567890123456789012345678901234567890
	  //D    19  52279 43200.0  2002.01.05-12:00:00  NYALES20  0.00012  0.00097 -0.00072
	  //D     1  52821     0.0  2003.07.01-00:00:00  DN_21742  0.00054  0.00037 -0.00063
	  //D     1  53767     0.0  2006.02.01-00:00:00  CARROLGA  0.00313 -0.00004 -0.00270
	  SiteID = Str.mid(45,8);
	  K = Str.mid(2,5).toInt(&IsOK);
	  if (IsOK)
	    {
	      d = Str.mid(54,8).toDouble(&IsOK);
	      if (IsOK)
		{
		  R[VERT] = d;
		  d = Str.mid(63,8).toDouble(&IsOK);
		  if (IsOK)
		    {
		      R[EAST] = d;
		      d = Str.mid(72,8).toDouble(&IsOK);
		      if (IsOK)
			R[NORT] = d;
		    };
		};
	    };
	  if (IsOK)
	    {
	      T = tStart() + (K-1)*interval();
	      Entry = new SBAploEntry(SiteID, ID);
	      if ( (K=find(Entry)) != -1)
		{
		  delete Entry;
		  Entry = at(K);
		  DRec = new SBAploDRec(Entry);
		  *(SBMJD*)DRec = T;
		  DRec->setDisplacement(R);
		  Entry->append(DRec);
		}
	      else
		{
		  Log->write(SBLog::DBG, SBLog::TIDES, ClassName() + ": cannot find ephem-entry for the site " +
			     SiteID + ", string [" + Str + "]; entry ignored");
		  delete Entry;
		};
	    }
	  else
	    Log->write(SBLog::WRN, SBLog::TIDES, ClassName() + ": wrong format of the `D`-record, string [" + 
		       Str + "]; entry ignored");
	}
      else if (Str.find("EPHEDISP  Format version of 2005.06.30")==0)
	{
	  
	}
      else if (!(Str.find("#")==0))
	{
	  Log->write(SBLog::WRN, SBLog::TIDES, ClassName() + ": got the string [" + Str +
		     "] of unknown format");
	};
    };

  for (uint i=0; i<count(); i++)
    if ((Entry=at(i))->count())
      Entry->sort();
    else
      if (remove(i))
	i--;
      else
	Log->write(SBLog::ERR, SBLog::TIDES, ClassName() + ": cannot remove the entry [" + 
		   at(i)->name() + "] from the list");
  
  sort();
  fillDict();

  return;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBAploChunk's friends implementation							*/
/*												*/
/*==============================================================================================*/
QTextStream &operator<<(QTextStream& s, const SBAploChunk& /*C*/)
{
  
  return s;
};

QTextStream &operator>>(QTextStream& s, SBAploChunk& /*C*/)
{

  return s;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBAploSiteAmpl implementation								*/
/*												*/
/*==============================================================================================*/
void SBAploSiteAmpl::fillDict()
{
  WavesByName.clear();
  for (SBAploWaveAmpls *W=first(); W; W=next())
    WavesByName.insert(W->name(), W);
  WavesByName.resize(WavesByName.count() + 5);
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBAploEphem implementation								*/
/*												*/
/*==============================================================================================*/
SBAploEphem::SBAploEphem()
  : QList<SBAploHeader>()
{
  setAutoDelete(TRUE);
  TStart = TZero;
  TFinis = TZero;
  TPreparedStart = TZero;
  TPreparedFinis = TZero;
  Interval = -1.0;
  IsOK   = FALSE;
  checkEphem();
  
  Sites.setAutoDelete(TRUE);
  SiteByID.setAutoDelete(FALSE);
  SplineByID.setAutoDelete(TRUE);
  SplineByDomes.setAutoDelete(FALSE);

  Waves.setAutoDelete(TRUE);
  WaveByName.setAutoDelete(FALSE);
  SiteAmplitudes.setAutoDelete(TRUE);
  SiteAmplitudeByDomes.setAutoDelete(FALSE);
};

SBAploEphem::~SBAploEphem()
{
  if (SiteByID.count())
    SiteByID.clear();
  if (SplineByDomes.count())
    SplineByDomes.clear();
  if (SplineByID.count())
    SplineByID.clear();
  if (Sites.count())
    Sites.clear();
  if (WaveByName.count())
    WaveByName.clear();
  if (Waves.count())
    Waves.clear();
  if (SiteAmplitudeByDomes.count())
    SiteAmplitudeByDomes.clear();
  if (SiteAmplitudes.count())
    SiteAmplitudes.clear();
};

void SBAploEphem::checkEphem(bool IsNeedCheckDicts)
{
  for (SBAploHeader *h=first(); h; h=next())
    {
      if (Interval < 0.0)
	Interval = h->interval();
      else
	if (Interval != h->interval())
	  Log->write(SBLog::ERR, SBLog::DATA, ClassName() + ": the ephemerides has different interval in chunks.");

      if (TStart!=TZero)
	{
	  if (TStart>h->tStart())
	    TStart = h->tStart();
	}
      else 
	TStart = h->tStart();

      if (TFinis!=TZero)
	{
	  if (TFinis<h->tFinis())
	    TFinis = h->tFinis();
	}
      else 
	TFinis = h->tFinis();
    };

  if (IsNeedCheckDicts)
    {
      if (WaveByName.count())
	WaveByName.clear();
      for (SBAploWave *Wave=Waves.first(); Wave; Wave=Waves.next())
	WaveByName.insert(Wave->name(), Wave);
      WaveByName.resize(WaveByName.count() + 5);
      
      if (SiteAmplitudeByDomes.count())
	SiteAmplitudeByDomes.clear();
      for (SBAploSiteAmpl *A=SiteAmplitudes.first(); A; A=SiteAmplitudes.next())
	SiteAmplitudeByDomes.insert(A->id().domeMajor(), A);
      SiteAmplitudeByDomes.resize(SiteAmplitudeByDomes.count() + 5);
    };
  
  IsOK   = TRUE;
};

void SBAploEphem::saveChunk(SBAploChunk* Chunk)
{
  if (!Chunk)
    {
      Log->write(SBLog::ERR, SBLog::DATA, ClassName() + ": nothing to save (ARG==NULL)");
      return;
    };

  QDataStream		ds;
  QFile			file;
  FILE			*pipe=NULL;

  pipe = SetUp->fcList()->open4Out(SetUp->path2StuffAplo() + Chunk->name() + ext(), file, ds);
  ds << *Chunk;
  SetUp->fcList()->close(file, pipe, ds);

  return;
};

void SBAploEphem::loadChunk(SBAploHeader* Hdr, SBAploChunk*& Chunk)
{
  if (Chunk)
    delete Chunk;
  
  Chunk = new SBAploChunk;

  QDataStream		ds;
  QFile			file;
  FILE			*pipe=NULL;

  pipe = SetUp->fcList()->open4In(SetUp->path2StuffAplo() + Hdr->name() + ext(), file, ds);
  ds >> *Chunk;
  SetUp->fcList()->close(file, pipe, ds);

  return;
};

bool SBAploEphem::importEphem(const QString& FileName, SB_TRF* TRF, bool& IsTRFmodified)
{
  bool			IsImported = FALSE;
  int			idx;
  QFile			file;
  FILE			*pipe=NULL;
  SBAploChunk		*Chunk = new SBAploChunk;

  // read a chunk of ephemerides:
  QTextStream  ts;
  if ( (pipe=SetUp->fcList()->open4In(FileName, file, ts)) )
    {
      Chunk->import(ts, TRF, IsTRFmodified);
      if (Chunk->count())
	IsImported = TRUE;
    };
  SetUp->fcList()->close(file, pipe, ts);

  if (Chunk->tStart() != TZero && IsImported)
    {
      // put it in the list:
      SBAploHeader		*Header = new SBAploHeader(Chunk->header());
      
      if ( (idx=find(Header))!=-1 )
	remove(idx);
      inSort(Header);
      saveChunk(Chunk);
      checkEphem(FALSE);
    };
  
  delete Chunk;
  return IsImported;
};

void SBAploEphem::importHPS(const QString& FileName, SB_TRF* TRF)
{
  bool			IsOK = FALSE;
  double		d = 0.0;
  double		c_u=0.0, c_e=0.0, c_n=0.0, s_u=0.0, s_e=0.0, s_n=0.0;
  Vector3		R(v3Zero);

  QString		WaveID;
  QString		SiteID;
  SBStation		*Station = NULL;
  SBStationID		ID;
  SBPMNNR_NUVEL1A	Plates;

  SBAploSiteAmpl	*Entry=NULL;
  SBAploWave		*Wave =NULL;

  QFile			f(FileName);
  f.open(IO_ReadOnly);
  QTextStream		s(&f);

  QDict<SBAploSiteAmpl>	SiteByNameAux;


  SiteByNameAux.setAutoDelete(FALSE);
  SiteByNameAux.clear();

  WaveByName.clear();
  Waves.clear();
  SiteAmplitudeByDomes.clear();
  SiteAmplitudes.clear();
  
  QString	Str = s.readLine();

  if (Str.find("HARPOS  Format version of 2002.12.12") != 0)
    {
      Log->write(SBLog::ERR, SBLog::TIDES, ClassName() + ": the string [" + Str +
		 "] is not a header of HARPOS; file ignored");
      return;
    };
  
  while (!s.atEnd())
    {
      Str = s.readLine();

      if (Str.find("H ")==0) // H-Record
	{
	  //          1         2         3         4         5
	  //0123456789012345678901234567890123456789012345678901234567890
	  //H  S1         0.000000D+00   7.272205216643D-05   0.000D+00
	  Wave = new SBAploWave(Str.mid(3,8).simplifyWhiteSpace());

	  d = Str.mid(13,13).replace(QRegExp("D"), "E").toDouble(&IsOK);
	  if (!IsOK)
	    {
	      Log->write(SBLog::ERR, SBLog::TIDES, ClassName() + 
			 ": wrong format of the `H`-record (Phase), string [" + Str + "][" + Str.mid(13,13) + "]; file ignored");
	      return;
	    };
	  Wave->setPhase(d);

	  d = Str.mid(28,19).replace(QRegExp("D"), "E").toDouble(&IsOK);
	  if (!IsOK)
	    {
	      Log->write(SBLog::ERR, SBLog::TIDES, ClassName() + 
			 ": wrong format of the `H`-record (Freq), string [" + Str + "]; file ignored");
	      return;
	    };
	  Wave->setFreq(d);

	  d = Str.mid(49,10).replace(QRegExp("D"), "E").toDouble(&IsOK);
	  if (!IsOK)
	    {
	      Log->write(SBLog::ERR, SBLog::TIDES, ClassName() + 
			 ": wrong format of the `H`-record (Accel), string [" + Str + "]; file ignored");
	      return;
	    };
	  Wave->setAccel(d);

	  if (!WaveByName.find(Wave->name()))
	    {
	      Waves.append(Wave);
	      WaveByName.insert(Wave->name(), Wave);

	    }
	  else
	    Log->write(SBLog::ERR, SBLog::TIDES, ClassName() + 
		       ": got duplicate wave record for " + Wave->name() + "; string [" + Str + "]");
	}
      else if (Str.find("S ")==0)
	{
	  //          1         2         3         4         5         6         7         8
	  //012345678901234567890123456789012345678901234567890123456789012345678901234567890
	  //S  DN_21742  -3530219.3120  4118797.5720  3344015.8670   31.6516 130.5999  323.1
	  SiteID = Str.mid(3,8);
	  d = Str.mid(13,13).toDouble(&IsOK);
	  if (IsOK)
	    {
	      R[X_AXIS] = d;
	      d = Str.mid(27,13).toDouble(&IsOK);
	      if (IsOK)
		{
		  R[Y_AXIS] = d;
		  d = Str.mid(41,13).toDouble(&IsOK);
		  if (IsOK)
		    R[Z_AXIS] = d;
		};
	    };
	  if (IsOK)
	    {
	      // well, it's undocumented feature:
	      if ( (Station=TRF->lookupStation(SiteID)) )
		{
		  ID = Station->id();
		  Log->write(SBLog::DBG, SBLog::STATION, ClassName() + ": got an alias '" + SiteID +
			     "' for the station " + Station->name() + " [" + ID.toString() + "]");

		  // should we import coordinates here too???
		  if (Station->r_first()==v3Zero)
		    {
		      if (Station->site()->v() == v3Zero)
			{
			  Station->setCoords(R, v3Unit, v3Zero, v3Unit, TZero, "Initial data");
			  Station->setV(Plates.velocity(*Station));
			  Station->site()->setV(Plates.velocity(*Station));
			}
		      else
			Station->setCoords(R, v3Unit, v3Zero, v3Unit, TZero, "Initial data");
		      Log->write(SBLog::INF, SBLog::DATA, 
				 "SBDS_dbh: set up coordinates for \"" + 
				 Station->name() + "\" [" + Station->id().toString() +
				 "] aka [" + SiteID + "] station");
		      Station->addAttr(SBStation::OrigEdited);
		      //		      IsTRFModified = TRUE;
		    };
		}
	      else
		{
		  Station=TRF->lookupNearest(TRF->epoch(), R, d, TECH_ANY);
		  if (d<=2000.0) // 
		    ID = Station->id();
		  else
		    {
		      Log->write(SBLog::DBG, SBLog::TIDES || SBLog::STATION, ClassName() + ": cannot find station '" + 
				 SiteID + "' in the TRF; the string: [" + Str + "]");
		      Station=NULL;
		    };
		};
	      if (Station && ID.isValidId())
		{
		  Entry = new SBAploSiteAmpl(SiteID, ID);
		  if (!SiteAmplitudeByDomes.find(Entry->id().domeMajor()))
		    {
		      SiteAmplitudes.append(Entry);
		      SiteAmplitudeByDomes.insert(Entry->id().domeMajor(), Entry);
		      SiteByNameAux.insert(Entry->name(), Entry);
		    }
		  else
		    {
		      Log->write(SBLog::WRN, SBLog::STATION, ClassName() + ": wrong data: duplicate site `" + 
				 SiteID + "' [" + ID.toString() + "], string: " + Str);
		      delete Entry;
		    };
		};
	    }
	  else
	    Log->write(SBLog::WRN, SBLog::TIDES, ClassName() + ": wrong format of the `S`-record, string [" + 
		       Str + "]; entry ignored");
	}
      else if (Str.find("D ")==0)
	{
	  //          1         2         3         4         5         6         7         8
	  //012345678901234567890123456789012345678901234567890123456789012345678901234567890
	  //D  S1        DN_10077   -0.00001 -0.00003  0.00000    0.00004 -0.00002  0.00006
	  //D  S2        DN_10077   -0.00006 -0.00003 -0.00000    0.00026 -0.00002  0.00005
	  WaveID = Str.mid(3,8).simplifyWhiteSpace();
	  SiteID = Str.mid(13,8);
	  
	  c_u = Str.mid(24,8).toDouble(&IsOK);
	  if (IsOK) c_e = Str.mid(33,8).toDouble(&IsOK);
	  if (IsOK) c_n = Str.mid(42,8).toDouble(&IsOK);
	  if (IsOK) s_u = Str.mid(53,8).toDouble(&IsOK);
	  if (IsOK) s_e = Str.mid(62,8).toDouble(&IsOK);
	  if (IsOK) s_n = Str.mid(71,8).toDouble(&IsOK);
	  if (IsOK)
	    {
	      if (!(Wave = WaveByName.find(WaveID)))
		Log->write(SBLog::ERR, SBLog::TIDES, ClassName() + ": cannot find a wave [" + WaveID + 
			   "] in the Wave List; string: [" + Str + "]");
	      if (!(Entry= SiteByNameAux.find(SiteID)))
		Log->write(SBLog::ERR, SBLog::TIDES, ClassName() + ": cannot find a site [" + SiteID + 
			   "] in the Site List; string: [" + Str + "]");

	      if (Wave && Entry)
		{
		  SBAploWaveAmpls *e = new SBAploWaveAmpls(Wave->name());

		  if ((Entry->waveByName(Wave->name())))
		    Log->write(SBLog::ERR, SBLog::TIDES, ClassName() + ": got a duplicate wave entry [" + 
			       WaveID + "] for the site [" + SiteID + "]; string: [" + Str + "]");
		  else
		    {
		      e->setCosU(c_u);
		      e->setCosE(c_e);
		      e->setCosN(c_n);
		      e->setSinU(s_u);
		      e->setSinE(s_e);
		      e->setSinN(s_n);
		      Entry->append(e);
		    }
		}
	    }
	  else
	    Log->write(SBLog::WRN, SBLog::TIDES, ClassName() + ": got a wrong 'D'-record; string: [" + Str + "]");
	}
      else if (Str.find("HARPOS  Format version of 2002.12.12")==0)
	{
	  
	}
      else if (!(Str.find("#")==0))
	{
	  Log->write(SBLog::WRN, SBLog::TIDES, ClassName() + ": got the string [" + Str +
		     "] of unknown format");
	};
    };


  SiteByNameAux.clear();

  f.close();
  s.unsetDevice();
  checkEphem();
};

void SBAploEphem::registerStation(SBStation* Station)
{
  SBStationID	*id=NULL;

  if ( !(id = SiteByID.find(Station->id().toString())) )
    {
      Sites.append( (id = new SBStationID(Station->id())) );
      SiteByID.insert(id->toString(), id);
    }
  else // should not happen
    {
      Log->write(SBLog::WRN, SBLog::STATION, ClassName() + ": Station " + Station->name() + " [" +
		 Station->id().toString() + "] already registered in the aplo");
    };
};

void SBAploEphem::getReady(const SBMJD& Tbegin, const SBMJD& Tend)
{
  if (!Sites.count())
    {
      Log->write(SBLog::WRN, SBLog::STATION, ClassName() + ": got an emty list of sites; ignored");
      return;
    };

  if (Tbegin>=Tend)
    {
      Log->write(SBLog::WRN, SBLog::STATION, ClassName() + ": Tbegin>=Tend; ignored");
      return;
    };

  if (Tbegin>=TFinis-2.1*interval())
    {
      Log->write(SBLog::WRN, SBLog::STATION, ClassName() + ": Tbegin>=TFinis; ignored");
      return;
    };

  if (TStart+2.1*interval()>=Tend)
    {
      Log->write(SBLog::WRN, SBLog::STATION, ClassName() + ": TStart>=Tend; ignored");
      return;
    };

  TPreparedStart = Tbegin;
  TPreparedFinis = Tend;

  if (TPreparedStart<TStart)
    {
      Log->write(SBLog::WRN, SBLog::STATION, ClassName() + ": Date '" + TPreparedStart.toString() + 
		 "' less then the begin of the ephemerides `" + TStart.toString() + "`");
      TPreparedStart = TStart;
    };
  if (TPreparedFinis>TFinis)
    {
      Log->write(SBLog::WRN, SBLog::STATION, ClassName() + ": Date '" + TPreparedFinis.toString() + 
		 "' less then the begin of the ephemerides `" + TFinis.toString() + "`");
      TPreparedFinis = TFinis;
    };

  unsigned int NumRows = 0;
  NumRows = (unsigned int)(ceil((TPreparedFinis-TPreparedStart)/interval()));

  if (NumRows == 1)
    {
      TPreparedStart -= 2.0*interval();
      TPreparedFinis += 2.0*interval();
      NumRows = 5;
    }
  else if (NumRows == 2)
    {
      TPreparedStart -= interval();
      TPreparedFinis += interval();
      NumRows = 4;
    }
  else if (NumRows==3)
    {
      TPreparedStart -= interval();
      TPreparedFinis += interval();
      NumRows = 5;
    };

  SBSpline	*Spline= NULL;
  SBAploChunk	*Chunk = NULL;
  SBAploEntry	*Entry = NULL;
  int		idx;
  //
  for (SBAploHeader *hdr=first(); hdr; hdr=next())
    {
      if ((hdr->tStart()<=TPreparedStart && TPreparedStart<hdr->tFinis()) ||
	  (hdr->tStart()<=TPreparedFinis && TPreparedFinis<hdr->tFinis()) ||
	  (hdr->tStart()>TPreparedStart && hdr->tFinis()<TPreparedFinis)   )
	{
	  loadChunk(hdr, Chunk);
	  for (SBStationID *id=Sites.first(); id; id=Sites.next())
	    if ( (Entry=Chunk->entryByID(*id)) )
	      {
		if ( !(Spline=SplineByID.find(id->toString())) )
		  {
		    Spline = new SBSpline(NumRows, 3);
		    SplineByID.insert(id->toString(), Spline);
		    if (!SplineByDomes[id->domeMajor()])
		      SplineByDomes.insert(id->domeMajor(), Spline);
		  };
		for (SBAploDRec* dr=Entry->first(); dr; dr=Entry->next())
		  {
		    idx = (int)(ceil((*dr-TPreparedStart)/interval()));
		    if (idx>=0 && idx<(int)NumRows)
		      {
			Spline->b()->set(idx, 0, *dr);
			Spline->b()->set(idx, 1, dr->displacement().at(VERT));
			Spline->b()->set(idx, 2, dr->displacement().at(EAST));
			Spline->b()->set(idx, 3, dr->displacement().at(NORT));
		      };
		  };
	      }
	    else
	      Log->write(SBLog::WRN, SBLog::STATION, ClassName() + ": Station [" + id->toString() + 
			 "] not found in the chunk");
	  if (Chunk)
	    {
	      delete Chunk;
	      Chunk = NULL;
	    };

	};
    };
  
  //
  SplineByID.resize(SplineByID.count() + 5);
  SplineByDomes.resize(SplineByDomes.count() + 5);
  QDictIterator<SBSpline> it(SplineByID);
  it.toFirst();
  while ( it.current() )
    {
      it.current()->prepare4Spline();
      ++it;
    };
  
  Log->write(SBLog::DBG, SBLog::STATION, ClassName() + ": the list of [" + 
	     QString().setNum(SplineByDomes.count()) + ";" + QString().setNum(SplineByID.count()) + 
	     "] sites/stations prepared for processing");
  Log->write(SBLog::DBG, SBLog::STATION, ClassName() + ": interval: from [" + 
	     TPreparedStart.toString(SBMJD::F_Short) + "] till [" + 
	     TPreparedFinis.toString(SBMJD::F_Short) + "]");
  
};

void SBAploEphem::dismissed()
{
  TPreparedStart = TZero;
  TPreparedFinis = TZero;
  SiteByID.clear();
  SplineByDomes.clear();
  SplineByID.clear();
  Sites.clear();
};

Vector3 SBAploEphem::displacement(const SBStationID& ID, const SBMJD& T)
{
  SBAploSiteAmpl	*SAmpl = NULL;
  SBAploWave		*Wave  = NULL;
  Vector3		D(v3Zero), D_Harm(v3Zero), D_Eph(v3Zero);
  SBSpline		*spl=SplineByID.find(ID.toString());

  if (!spl && (spl = SplineByDomes.find(ID.domeMajor())))
    Log->write(SBLog::DBG, SBLog::STATION, ClassName() + ": the station [" + 
	       ID.toString() + "] picked up by DOMES Number");
  if (spl)
    {
      if (TPreparedStart-0.5*interval() <= T && T<=TPreparedFinis+0.5*interval())
	{
	  if (spl->isReady())
	    {
	      D_Eph[VERT] = spl->spline(0, T);
	      D_Eph[EAST] = spl->spline(1, T);
	      D_Eph[NORT] = spl->spline(2, T);
	    };

	  if ((SAmpl=SiteAmplitudeByDomes.find(ID.domeMajor())))
	    {
	      for (SBAploWaveAmpls *WaveAmpls=SAmpl->first(); WaveAmpls; WaveAmpls=SAmpl->next())
		{
		  if ((Wave=WaveByName.find(WaveAmpls->name())))
		    {
		      double sinPhi=0.0, cosPhi=0.0;
		      sincos(Wave->angle(T), &sinPhi, &cosPhi);
		      D_Harm[VERT] += WaveAmpls->cosU()*cosPhi + WaveAmpls->sinU()*sinPhi;
		      D_Harm[EAST] += WaveAmpls->cosE()*cosPhi + WaveAmpls->sinE()*sinPhi;
		      D_Harm[NORT] += WaveAmpls->cosN()*cosPhi + WaveAmpls->sinN()*sinPhi;
		    }
		  else
		    Log->write(SBLog::WRN, SBLog::STATION, ClassName() + ": cannot find wave [" + 
			       WaveAmpls->name() + "] in the WaveByName (Harmonic disp.l)");
		}
	    }
	  else
	    Log->write(SBLog::WRN, SBLog::STATION, ClassName() + ": cannot find site [" + 
		       ID.toString() + "] in the SiteAmplitudeByDomes (Harmonic disp.l)");
	  
	  D = D_Eph + D_Harm;

	  if (Log->isEligible(SBLog::DBG, SBLog::TIDES))
	    {
	      Log->write(SBLog::DBG, SBLog::TIDES, ClassName() + ": Arg(TAI)           : " + 
			 T.toString() + "; Station: [" + ID.toString() + "]");
	      D_Eph.report2Log(SBLog::DBG, SBLog::TIDES, ClassName() + ": Ephem.Displ.[VEN,m]: ", 11, 8);
	      D_Harm.report2Log(SBLog::DBG, SBLog::TIDES, ClassName() + ": Harm. Displ.[VEN,m]: ", 11, 8);
	      D.report2Log(SBLog::DBG, SBLog::TIDES, ClassName() + ": Total Displ.[VEN,m]: ", 11, 8);
	    };
	}
      else
	Log->write(SBLog::DBG, SBLog::STATION, ClassName() + ": the argument [" + 
		   T.toString(SBMJD::F_Short) + "] out of range");
    };
  
  return D;
};
/*==============================================================================================*/






/*==============================================================================================*/

/*==============================================================================================*/
