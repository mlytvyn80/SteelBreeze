/*
 *
 * This file is part of SteelBreeze.
 *
 * SteelBreeze, the geodetic VLBI data analysing software.
 * Copyright (C) 1998-2004 Sergei Bolotin, MAO NASU, Kiev, Ukraine.
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

#include <qapplication.h>
#include <qdir.h> 
#include <qmessagebox.h>

#include "SbGeoObsVLBI.H"
#include "SbGeoObsVLBI_IO.H"
#include "SbSetup.H"


#include "SbGeoDelay.H"
#include "SbGeoProject.H"

// 4 estimator's interface:
// (with dalay and rate processing the "1" will be replaced by "2")
SBVector		SBObsVLBIEntry::VLBI_Obs2SRIF(1, "SBObsVLBIEntry::VLBI_Obs2SRIF"); 
SBVector		SBObsVLBIEntry::VLBI_Sig2SRIF(1, "SBObsVLBIEntry::VLBI_Sig2SRIF"); 


/*==============================================================================================*/
/*												*/
/* class SBStationInfo implementation								*/
/*												*/
/*==============================================================================================*/
SBStationInfo::SBStationInfo(const SBStationID& ID_, int Idx_, const QString& Aka_)
  : SBStationID(ID_), SBObjectObsInfo(Idx_, Aka_)
{
  for (int i=0; i<10; i++)
    {
      Cl_V[i]=0.0;
      Cl_E[i]=0.0;
    };
  Cl_Order = 3;
  Zw = 0.0;
  ErrZw = 0.0;
  addAttr(EstCoo);

  /*
    addAttr(EstVel);
    addAttr(ConstrCoo);
    addAttr(ConstrVel);
  */
  Clocks = DefaultConfig.p().par(0);
  Zenith = DefaultConfig.p().par(7);
};

void SBStationInfo::dumpUserInfo(QTextStream& ts)
{
  ts << " +" << toString() << " aka " << aka();
  ts << "\n +ATTRIBUTES OF STATION:";
  ts << "\n  NOTVALID: "	<< (isAttr(notValid)?"Y":"N");
  ts << "\n  REFCLOCK: "	<< (isAttr(refClock)?"Y":"N");
  ts << "\n  BADCABLE: "	<< (isAttr(BadCable)?"Y":"N");
  ts << "\n  BADMETEO: "	<< (isAttr(BadMeteo)?"Y":"N");
  ts << "\n  ARTMETEO: "	<< (isAttr(ArtMeteo)?"Y":"N");
  ts << "\n  HASCLOCKBREAK: "	<< (isAttr(HasClockBreak)?"Y":"N");
  ts << "\n  USELOCALCLOCKS: "	<< (isAttr(UseLocalClocks)?"Y":"N");
  ts << "\n  USELOCALZENITH: "	<< (isAttr(UseLocalZenith)?"Y":"N");
  ts << "\n  INVERTCABLE: "	<< (isAttr(InvCable)?"Y":"N");
  ts << "\n -ATTRIBUTES OF STATION\n";

  ts << " MAXCLOCKORDER: " << QString().sprintf("%8d\n", Cl_Order);

  if (ClockBreaks.count())
    {
      ts << " +CLOCK BREAK LIST:\n";
      for (SBParameterBreak *br=ClockBreaks.first(); br; br=ClockBreaks.next()) 
	ts << "  " << br->toString(SBMJD::F_INTERNAL) << " " << QString().sprintf("%.8g\n", br->b());
      ts << " -CLOCK BREAK LIST\n";
    };

  ts << " +PARAMETER CLOCK:\n";
  ts << "  WHITENOISE: " << QString().sprintf("%.8g\n", Clocks.whiteNoise());
  ts << "  BREAKNOISE: " << QString().sprintf("%.8g\n", Clocks.breakNoise());
  ts << " -PARAMETER CLOCK\n";

  ts << " +PARAMETER ZENITH:\n";
  ts << "  WHITENOISE: " << QString().sprintf("%.8g\n", Zenith.whiteNoise());
  ts << "  BREAKNOISE: " << QString().sprintf("%.8g\n", Zenith.breakNoise());
  ts << " -PARAMETER ZENITH\n";
  ts << " -" << toString() << "\n";
};

bool SBStationInfo::restoreUserInfo(QTextStream& ts)
{
  bool		Re = TRUE;
  double	d  = 0.0;
  int		i  = 3;
  QString	Str;
  QString	EOR = " -" + toString();

  while (!ts.atEnd() && Re && (Str = ts.readLine())!= EOR)
    {
      if (Str == " +ATTRIBUTES OF STATION:")	// Attributes:
	{
	  while (!ts.atEnd() && (Str=ts.readLine())!=" -ATTRIBUTES OF STATION")
	    {
	      if (Str.mid(0,2) != "  ") return FALSE;
	      else if (Str.find("  NOTVALID:") == 0)
		{
		  if (Str.mid(Str.length()-1)=="Y") addAttr(notValid);
		  else delAttr(notValid);
		}
	      else if (Str.find("  REFCLOCK:") == 0)
		{
		  if (Str.mid(Str.length()-1)=="Y") addAttr(refClock);
		  else delAttr(refClock);
		}
	      else if (Str.find("  BADCABLE:") == 0)
		{
		  if (Str.mid(Str.length()-1)=="Y") addAttr(BadCable);
		  else delAttr(BadCable);
		}
	      else if (Str.find("  BADMETEO:") == 0)
		{
		  if (Str.mid(Str.length()-1)=="Y") addAttr(BadMeteo);
		  else delAttr(BadMeteo);
		}
	      else if (Str.find("  ARTMETEO:") == 0)
		{
		  if (Str.mid(Str.length()-1)=="Y") addAttr(ArtMeteo);
		  else delAttr(ArtMeteo);
		}
	      else if (Str.find("  HASCLOCKBREAK:") == 0)
		{
		  if (Str.mid(Str.length()-1)=="Y") addAttr(HasClockBreak);
		  else delAttr(HasClockBreak);
		}
	      else if (Str.find("  USELOCALCLOCKS:") == 0)
		{
		  if (Str.mid(Str.length()-1)=="Y") addAttr(UseLocalClocks);
		  else delAttr(UseLocalClocks);
		}
	      else if (Str.find("  USELOCALZENITH:") == 0)
		{
		  if (Str.mid(Str.length()-1)=="Y") addAttr(UseLocalZenith);
		  else delAttr(UseLocalZenith);
		}
	      else if (Str.find("  INVERTCABLE:") == 0)
		{
		  if (Str.mid(Str.length()-1)=="Y") addAttr(InvCable);
		  else delAttr(UseLocalZenith);
		}
	    };
	}
      else if (Str.find(" MAXCLOCKORDER: ") == 0)
	{
	  i = Str.mid(15).toInt(&Re);
	  if (Re)
	    setCl_Order(i);
	}
      else if (Str == " +CLOCK BREAK LIST:")
	{
	  while (!ts.atEnd() && Re && (Str=ts.readLine())!=" -CLOCK BREAK LIST")
	    {
	      SBMJD	t;
	      int	idx;
	      t.setMJD(SBMJD::F_INTERNAL, Str.mid(2, 17));
	      d=Str.mid(20).toDouble(&Re);
	      if (Re)
		{
		  SBParameterBreak Break(t, d), *pBreak=NULL;
		  if ( (idx=ClockBreaks.find(&Break))!=-1 && (pBreak=ClockBreaks.at(idx)) && pBreak->b() != d)
		    pBreak->setB(d);
		  else if (!pBreak)
		    ClockBreaks.inSort(new SBParameterBreak(t, d));
		};
	    }
	}
      else if (Str == " +PARAMETER CLOCK:")
	{
	  while (!ts.atEnd() && Re && (Str=ts.readLine())!=" -PARAMETER CLOCK")
	    {
	      if (Str.mid(0,2) != "  ") return FALSE;
	      else if (Str.find("  WHITENOISE:") == 0)
		{
		  d=Str.mid(14).toDouble(&Re);
		  if (Re)
		    Clocks.setWhiteNoise(d);
		}
	      else if (Str.find("  BREAKNOISE:") == 0)
		{
		  d=Str.mid(14).toDouble(&Re);
		  if (Re)
		    Clocks.setBreakNoise(d);
		}
	    }
	}
      else if (Str == " +PARAMETER ZENITH:")
	{
	  while (!ts.atEnd() && Re && (Str=ts.readLine())!=" -PARAMETER ZENITH")
	    {
	      if (Str.mid(0,2) != "  ") return FALSE;
	      else if (Str.find("  WHITENOISE:") == 0)
		{
		  d=Str.mid(14).toDouble(&Re);
		  if (Re)
		    Zenith.setWhiteNoise(d);
		}
	      else if (Str.find("  BREAKNOISE:") == 0)
		{
		  d=Str.mid(14).toDouble(&Re);
		  if (Re)
		    Zenith.setBreakNoise(d);
		}
	    }
	}
      else
	Log->write(SBLog::DBG, SBLog::DATA, ClassName() + 
		   ": got an unknown string [" + Str + "] in the UI file (STATION)");
    };
  
  return Re;
};

void SBStationInfo::clearClocks()
{
  for (int i=0; i<10; i++)
    {
      Cl_V[i]=0.0;
      Cl_E[i]=0.0;
    };
};

double SBStationInfo::clocksValue(double dt)
{
  double d	= 1.0;
  double Val	= 0.0;
  for (int i=0; i<Cl_Order; i++)
    {
      Val += Cl_V[i]*d;
      d*=dt;
    };
  return Val*1.0e-9;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBStationInfo's friends implementation							*/
/*												*/
/*==============================================================================================*/
QDataStream &operator<<(QDataStream& s, const SBStationInfo& St)
{
  return s << (const SBStationID&)St << (const SBObjectObsInfo&)St 
	   << St.ClockBreaks << St.Clocks << St.Zenith
	   << St.Zw << St.ErrZw << St.Cl_Order
	   << St.Cl_V[0] << St.Cl_V[1] << St.Cl_V[2] << St.Cl_V[3] << St.Cl_V[4] 
	   << St.Cl_V[5] << St.Cl_V[6] << St.Cl_V[7] << St.Cl_V[8] << St.Cl_V[9]
	   << St.Cl_E[0] << St.Cl_E[1] << St.Cl_E[2] << St.Cl_E[3] << St.Cl_E[4] 
	   << St.Cl_E[5] << St.Cl_E[6] << St.Cl_E[7] << St.Cl_E[8] << St.Cl_E[9];
};

QDataStream &operator>>(QDataStream& s, SBStationInfo& St)
{
  return s >> (SBStationID&)St >> (SBObjectObsInfo&)St 
	   >> St.ClockBreaks >> St.Clocks >> St.Zenith
	   >> St.Zw >> St.ErrZw >> St.Cl_Order
	   >> St.Cl_V[0] >> St.Cl_V[1] >> St.Cl_V[2] >> St.Cl_V[3] >> St.Cl_V[4] 
	   >> St.Cl_V[5] >> St.Cl_V[6] >> St.Cl_V[7] >> St.Cl_V[8] >> St.Cl_V[9]
	   >> St.Cl_E[0] >> St.Cl_E[1] >> St.Cl_E[2] >> St.Cl_E[3] >> St.Cl_E[4] 
	   >> St.Cl_E[5] >> St.Cl_E[6] >> St.Cl_E[7] >> St.Cl_E[8] >> St.Cl_E[9];
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBStationInfoList implementation							*/
/*												*/
/*==============================================================================================*/
bool SBStationInfoList::restoreUserInfo(QTextStream& ts)
{
  bool			Re=TRUE;
  QString		Str;
  SBStationID		id;
  SBStationInfo		*SI=NULL;

  while (!ts.atEnd() && Re && (Str=ts.readLine())!="-STATIONS")
    {
      if (Str.mid(0,1) != " ")
	{
	  Log->write(SBLog::DBG, SBLog::DATA, ClassName() + ": wrong prefix in UI string: [" + Str + "] (STATIONS)");
	  return FALSE;
	};
      // +40440S002 aka HAYSTACK
      id.setID(Str.mid(2, 9));
      if (id.isValidId())
	{
	  if ( (SI = find(&id)) )
	    {
	      Re = Re && SI->restoreUserInfo(ts);
	    }
	  else 
	    Log->write(SBLog::DBG, SBLog::DATA, ClassName() + 
		       ": cannot find the station " + id.toString() + 
		       " in the list of stations; the string is: [" + Str + "]");
	}
      else 
	{
	  Log->write(SBLog::DBG, SBLog::DATA, ClassName() + ": not valid ID: [" + Str + "]");
	  return FALSE;
	};
    };
  return Re;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBBaseInfo implementation								*/
/*												*/
/*==============================================================================================*/
void SBBaseInfo::dumpUserInfo(QTextStream& ts)
{
  ts << " +" << name() << " aka " << aka();
  ts << "\n +ATTRIBUTES OF BASE:";
  ts << "\n  NOTVALID: "	<< (isAttr(notValid)?"Y":"N");
  ts << "\n -ATTRIBUTES OF BASE\n";
  ts << " -" << name() << "\n";
};

bool SBBaseInfo::restoreUserInfo(QTextStream& ts)
{
  bool		Re = TRUE;
  QString	Str;
  QString	EOR = " -" + name();

  while (!ts.atEnd() && Re && (Str = ts.readLine())!= EOR)
    {
      if (Str == " +ATTRIBUTES OF BASE:")	// Attributes:
	{
	  while (!ts.atEnd() && (Str=ts.readLine())!=" -ATTRIBUTES OF BASE")
	    {
	      if (Str.mid(0,2) != "  ") return FALSE;
	      else if (Str.find("  NOTVALID:") == 0)
		{
		  if (Str.mid(Str.length()-1)=="Y") addAttr(notValid);
		  else delAttr(notValid);
		}
	    };
	}
      else
	Log->write(SBLog::DBG, SBLog::DATA, ClassName() + 
		   ": got an unknown string [" + Str + "] in the UI file (BASES)");
    };
  
  return Re;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBBaseInfoList implementation								*/
/*												*/
/*==============================================================================================*/
bool SBBaseInfoList::restoreUserInfo(QTextStream& ts)
{
  bool			Re=TRUE;
  QString		Str;
  SBStationID		id1, id2;
  SBBaseInfo		*BI=NULL;
  SBNamed		BaseName;

  while (!ts.atEnd() && Re && (Str=ts.readLine())!="-BASES")
    {
      if (Str.mid(0,1) != " ")
	{
	  Log->write(SBLog::DBG, SBLog::DATA, ClassName() + ": wrong prefix in UI string: [" + Str + "] (BASES)");
	  return FALSE;
	};
      //0123456789012345678901234567890123456789012
      // +40440S002:40441S001 aka HAYSTACK:NRAO 140
      id1.setID(Str.mid( 2, 9));
      id2.setID(Str.mid(12, 9));
      if (id1.isValidId() && id2.isValidId())
	{
	  BaseName.setName(id1.toString() + ":" + id2.toString());
	  if ( (BI = find(&BaseName)) )
	    {
	      Re = Re && BI->restoreUserInfo(ts);
	    }
	  else 
	    Log->write(SBLog::DBG, SBLog::DATA, ClassName() + 
		       ": cannot find the base " + Str + " in the list of bases");
	}
      else 
	{
	  Log->write(SBLog::DBG, SBLog::DATA, ClassName() + ": not valid IDs: [" + Str + "]");
	  return FALSE;
	};
    };
  return Re;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBBaseInfo implementation								*/
/*												*/
/*==============================================================================================*/
void SBSourceInfo::dumpUserInfo(QTextStream& ts)
{
  ts << " +" << name() << " aka " << aka();
  ts << "\n +ATTRIBUTES OF SOURCE:";
  ts << "\n  NOTVALID: "	<< (isAttr(notValid)?"Y":"N");
  ts << "\n -ATTRIBUTES OF SOURCE\n";
  ts << " -" << name() << "\n";
};

bool SBSourceInfo::restoreUserInfo(QTextStream& ts)
{
  bool		Re = TRUE;
  QString	Str;
  QString	EOR_Name = " -" + name();
  QString	EOR_Aka  = " -" + aka();

  while (!ts.atEnd() && Re && (((Str = ts.readLine())!=EOR_Name) && Str!=EOR_Aka))
    {
      if (Str == " +ATTRIBUTES OF SOURCE:")	// Attributes:
	{
	  while (!ts.atEnd() && (Str=ts.readLine())!=" -ATTRIBUTES OF SOURCE")
	    {
	      if (Str.mid(0,2) != "  ") return FALSE;
	      else if (Str.find("  NOTVALID:") == 0)
		{
		  if (Str.mid(Str.length()-1)=="Y") addAttr(notValid);
		  else delAttr(notValid);
		}
	      else 
		std::cout << "SBSourceInfo::restoreUserInfo: got uknown string: " << Str << "\n";
	    };
	}
      else
	Log->write(SBLog::DBG, SBLog::DATA, ClassName() + 
		   ": got an unknown string [" + Str + "] in the UI file (SOURCES)");
    };

  return Re;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBSourceInfoList implementation							*/
/*												*/
/*==============================================================================================*/
bool SBSourceInfoList::restoreUserInfo(QTextStream& ts)
{
  bool			Re=TRUE;
  QString		Str;
  SBSourceInfo		*SI=NULL;
  SBNamed		SourceName;
  SBNamed		SourceAka;
  int			i;

  while (!ts.atEnd() && Re && (Str=ts.readLine())!="-SOURCES")
    {
      if (Str.mid(0,1) != " ")
	{
	  Log->write(SBLog::DBG, SBLog::DATA, ClassName() + ": wrong prefix in UI string: [" + Str + "] (SOURCES)");
	  return FALSE;
	};
      if ((i=Str.find(" aka "))==-1)
	{
	  Log->write(SBLog::DBG, SBLog::DATA, ClassName() + ": wrong format of UI string: [" + Str + "] (SOURCES)");
	  return FALSE;
	};
      //0123456789012345678901234567890123456789012
      // +0316+413 aka 3C84
      SourceName.setName(Str.mid(2, i-2));
      SourceAka .setName(Str.mid(i+3));
      if ( (SI = find(&SourceName)) )
	Re = Re && SI->restoreUserInfo(ts);
      else if ( (SI = find(&SourceAka)) )
	Re = Re && SI->restoreUserInfo(ts);
      else 
	Log->write(SBLog::DBG, SBLog::DATA, ClassName() + 
		   ": cannot find the source " + Str + " in the list of sources");
    };
  return Re;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBObsVLBIEntry implementation								*/
/*												*/
/*==============================================================================================*/
/**A constructor.
 * Creates a copy of object, fills data with zeros.
 */
SBObsVLBIEntry::SBObsVLBIEntry(SBVLBISession* Owner_) : SBObservation()
{
  setTech(TECH_VLBI);
  Owner				= Owner_;
  //  Idx			=  -1;
  // observables:
  Station_1			=  -1;
  Station_2			=  -1;
  Source			=  -1;
  Delay				= 0.0;
  DelayErr			= 1.0;
  Rate				= 0.0;
  RateErr			= 1.0;
  QualCode[0]			= 'N';
  QualCode[1]			= 'A';
  DUFlag			=   0;
  RUFlag			=   0;
  Correlation			= 0.0;
  FringePhase			= 0.0;
  Cable_1			= 0.0;
  Cable_2			= 0.0;
  AtmTemp_1			= -273.15;
  AtmTemp_2			= -273.15;
  AtmPress_1			= 0.0;
  AtmPress_2			= 0.0;
  AtmHum_1			= -100.0;
  AtmHum_2			= -100.0;
  DelayIon			= 0.0;
  DelayIonErr			= 0.0;
  RateIon			= 0.0;
  RateIonErr			= 0.0;
  RefFreq			= 0.0;
  GrDelayAmbig			= 0.0;
  IonErrorFlag			=   0;
  AmbiguityFactor		=   0;
  // aux data:
  DelayRes			= 0.0;
  RateRes			= 0.0;
  // alternates:
  AltAtmTemp_1			= 10.0;
  AltAtmTemp_2			= 10.0;
  AltAtmPress_1			= 1013.0;
  AltAtmPress_2			= 1013.0;
  AltAtmHum_1			= 50.0;
  AltAtmHum_2			= 50.0;
  // calcables:
  ZenithDelay_1			= 0.0;
  ZenithDelay_2			= 0.0;
};

SBObsVLBIEntry::SBObsVLBIEntry(const SBObsVLBIEntry& Obs_) : SBObservation(Obs_)
{
  setTech(TECH_VLBI);
  Owner		= Obs_.owner();
  //  Idx		= Obs_.idx();
  Station_1	= Obs_.station_1Idx();
  Station_2	= Obs_.station_2Idx();
  Source	= Obs_.sourceIdx();
  Delay		= Obs_.delay();
  DelayErr	= Obs_.delayErr();
  Rate		= Obs_.rate();
  RateErr	= Obs_.rateErr();
  QualCode[0]	= Obs_.QualCode[0];
  QualCode[1]	= Obs_.QualCode[1];
  DUFlag	= Obs_.dUFlag();
  RUFlag	= Obs_.rUFlag();
  Correlation	= Obs_.correlation();
  FringePhase	= Obs_.fringePhase();
  Cable_1	= Obs_.cable_1();
  Cable_2	= Obs_.cable_2();
  AtmTemp_1	= Obs_.atmTemp_1();
  AtmTemp_2	= Obs_.atmTemp_2();
  AtmPress_1	= Obs_.atmPress_1();
  AtmPress_2	= Obs_.atmPress_2();
  AtmHum_1	= Obs_.atmHum_1();
  AtmHum_2	= Obs_.atmHum_2();
  DelayIon	= Obs_.delayIon();
  DelayIonErr	= Obs_.delayIonErr();
  RateIon	= Obs_.rateIon();
  RateIonErr	= Obs_.rateIonErr();
  RefFreq	= Obs_.refFreq();
  GrDelayAmbig	= Obs_.grDelayAmbig();
  IonErrorFlag	= Obs_.ionErrorFlag();
  AmbiguityFactor= Obs_.ambiguityFactor();

  DelayRes	= Obs_.delayRes();
  RateRes	= Obs_.rateRes();

  AltAtmTemp_1	= Obs_.altAtmTemp_1();
  AltAtmTemp_2	= Obs_.altAtmTemp_2();
  AltAtmPress_1	= Obs_.altAtmPress_1();
  AltAtmPress_2	= Obs_.altAtmPress_2();
  AltAtmHum_1	= Obs_.altAtmHum_1();
  AltAtmHum_2	= Obs_.altAtmHum_2();

  ZenithDelay_1	= Obs_.zenithDelay_1();
  ZenithDelay_2	= Obs_.zenithDelay_2();
};

SBObsVLBIEntry& SBObsVLBIEntry::operator= (const SBObsVLBIEntry& Obs_)
{
  SBObservation::operator=(Obs_);
  //  Owner		= Obs_.owner();
  //  Idx		= Obs_.idx();
  setTech(TECH_VLBI);
  Station_1	= Obs_.station_1Idx();
  Station_2	= Obs_.station_2Idx();
  Source	= Obs_.sourceIdx();
  Delay		= Obs_.delay();
  DelayErr	= Obs_.delayErr();
  Rate		= Obs_.rate();
  RateErr	= Obs_.rateErr();
  QualCode[0]	= Obs_.QualCode[0];
  QualCode[1]	= Obs_.QualCode[1];
  DUFlag	= Obs_.dUFlag();
  RUFlag	= Obs_.rUFlag();
  Correlation	= Obs_.correlation();
  FringePhase	= Obs_.fringePhase();
  Cable_1	= Obs_.cable_1();
  Cable_2	= Obs_.cable_2();
  AtmTemp_1	= Obs_.atmTemp_1();
  AtmTemp_2	= Obs_.atmTemp_2();
  AtmPress_1	= Obs_.atmPress_1();
  AtmPress_2	= Obs_.atmPress_2();
  AtmHum_1	= Obs_.atmHum_1();
  AtmHum_2	= Obs_.atmHum_2();
  DelayIon	= Obs_.delayIon();
  DelayIonErr	= Obs_.delayIonErr();
  RateIon	= Obs_.rateIon();
  RateIonErr	= Obs_.rateIonErr();
  RefFreq	= Obs_.refFreq();
  GrDelayAmbig	= Obs_.grDelayAmbig();
  IonErrorFlag	= Obs_.ionErrorFlag();
  AmbiguityFactor= Obs_.ambiguityFactor();

  DelayRes	= Obs_.delayRes();
  RateRes	= Obs_.rateRes();

  AltAtmTemp_1	= Obs_.altAtmTemp_1();
  AltAtmTemp_2	= Obs_.altAtmTemp_2();
  AltAtmPress_1	= Obs_.altAtmPress_1();
  AltAtmPress_2	= Obs_.altAtmPress_2();
  AltAtmHum_1	= Obs_.altAtmHum_1();
  AltAtmHum_2	= Obs_.altAtmHum_2();

  ZenithDelay_1	= Obs_.zenithDelay_1();
  ZenithDelay_2	= Obs_.zenithDelay_2();

  return *this;
};

int SBObsVLBIEntry::quality() const 
{
  int q = 0;
  if (QualCode[0]=='N' && QualCode[1]=='A') // artificials, there are no any qualcode in the database
    q=4;
  else
    switch (QualCode[1])
      {
      default:
      case '0': q=0; break;
      case '1': q=1; break;
      case '2': q=2; break;
      case '3': q=3; break;
      case '4': q=4; break;
      case '5': q=5; break;
      case '6': q=6; break;
      case '7': q=7; break;
      case '8': q=8; break;
      case '9': q=9; break;
      };
  return q;
};

bool SBObsVLBIEntry::isEligible(SBRunManager* Mgr) 
{
  bool		IsEligible = SBObservation::isEligible(Mgr);

  if (Mgr)
    {
      if (quality() <= Mgr->prj()->cfg().qCodeThreshold())
	IsEligible = FALSE;
      if (Mgr->prj()->cfg().dUFlagThreshold()>-1 && DUFlag>Mgr->prj()->cfg().dUFlagThreshold())
	IsEligible = FALSE;
      //      if (RQuality>Mgr->prj()->cfg().rateQThreshold())
      /*IsEligible = FALSE*/;
      if (IonErrorFlag==-1)
	IsEligible = IsEligible && Mgr->prj()->cfg().isUseBadIon();
    };
  
  // check for "notValid" flags:
  if (IsEligible)
    {
      // session's attributes:
      SBStationInfo	*St_1Info = owner()->stationInfo(Station_1);
      if (St_1Info->isAttr(SBStationInfo::notValid))
	IsEligible = FALSE;
      // project's attributes:
      if (Mgr->prj()->stationList()->find(St_1Info)->isAttr(SBStationInfo::notValid))
	IsEligible = FALSE;
      
      // session's attributes:
      SBStationInfo	*St_2Info = owner()->stationInfo(Station_2);
      if (St_2Info->isAttr(SBStationInfo::notValid))
	IsEligible = FALSE;
      // project's attributes:
      if (Mgr->prj()->stationList()->find(St_2Info)->isAttr(SBStationInfo::notValid))
	IsEligible = FALSE;

      SBBaseInfo	*BaseInfo = NULL;
      SBNamed		 baseName(St_1Info->toString() + ":" + St_2Info->toString());
      if ( (BaseInfo = owner()->baseList()->find(&baseName)) )
	{
	  // session's attributes:
	  if (BaseInfo->isAttr(SBBaseInfo::notValid))
	    IsEligible = FALSE;
	  // project's attributes:
	  if (Mgr->prj()->baseList()->find(BaseInfo)->isAttr(SBBaseInfo::notValid))
	    IsEligible = FALSE;
	}
      else
	Log->write(SBLog::ERR, SBLog::DATA, ClassName() + ": cannot find base [" +
		   St_1Info->toString() + ":" + St_2Info->toString() + "] aka " +
		   St_1Info->aka() + ":" + St_2Info->aka() + " in the list of session's bases");
      

      // session's attributes:
      SBSourceInfo	*SrInfo = owner()->sourceInfo(Source);
      if (SrInfo->isAttr(SBSourceInfo::notValid))
	IsEligible = FALSE;
      // project's attributes:
      if (Mgr->prj()->sourceList()->find(SrInfo)->isAttr(SBSourceInfo::notValid))
	IsEligible = FALSE;
    };

  if (!IsEligible)
    {
      DelayRes = 0.0;
      RateRes  = 0.0;
      delAttr(processed);
    };

  return IsEligible;
};

void SBObsVLBIEntry::process(SBRunManager* Mgr)
{
  SBDelay	*CalcedDelay = Mgr->delay();
  const SB_TRF	*TRF	  = Mgr->trf();
  const SB_CRF	*CRF	  = Mgr->crf();
  //
  // station #1:
  SBStationInfo	*St_1Info = NULL;
  SBStation	*St_1	  = NULL;
  // station #2:
  SBStation	*St_2	  = NULL;
  SBStationInfo	*St_2Info = NULL;
  // source:
  SBSource	*Src	  = NULL;
  SBSourceInfo	*SrInfo   = NULL;
  // baseline:
  SBBaseInfo	*BaseInfo = NULL;
  //
  double	O_C = 0.0;
  double	DelayAprS = 15.0e-14;
  double	DelayS    = DelayErr>0.0?DelayErr:200.0e-12;
  double	DelayIonS = DelayIonErr>0.0?DelayIonErr:200.0e-12;

  double	DelaySigma= sqrt(DelayS*DelayS + DelayIonS*DelayIonS + DelayAprS*DelayAprS);
  double	Weight	  = 1.0/DelaySigma;
  
  
  if (!(St_1=station1(TRF, St_1Info)))
    Log->write(SBLog::ERR, SBLog::STATION | SBLog::DELAYALL, 
	       ClassName() + ": cannot find the station #1 [" + (St_1Info?St_1Info->toString():"NULL") + 
	       "] aka [" + (St_1Info?St_1Info->aka():"NULL") + "] in the catalogue");
  else if (!(St_2=station2(TRF, St_2Info)))
    Log->write(SBLog::ERR, SBLog::STATION | SBLog::DELAYALL, 
	       ClassName() + ": cannot find the station #2 [" + (St_2Info?St_2Info->toString():"NULL") + 
	       "] aka [" + (St_2Info?St_2Info->aka():"NULL") + "] in the catalogue");
  else if (!(Src=source(CRF, SrInfo)))
    Log->write(SBLog::ERR, SBLog::SOURCE | SBLog::DELAYALL, 
	       ClassName() + ": cannot find the source [" + (SrInfo?SrInfo->name():"NULL") + 
	       "] aka [" + (SrInfo?SrInfo->aka():"NULL") + "] in the catalogue");
  else 
    {
      St_1->calcDisplacement(Mgr, *this);
      St_2->calcDisplacement(Mgr, *this);
      Src ->updateParameters(Mgr, *this);

      if (isAttr(breakClock1) && (Mgr->prj()->cfg().p().clock0().type()==SBParameterCfg::PT_STH))
	St_1->p_C0()->addAttr(SBDerivation::IsBreak);
      if (isAttr(breakClock2) && (Mgr->prj()->cfg().p().clock0().type()==SBParameterCfg::PT_STH))
	St_2->p_C0()->addAttr(SBDerivation::IsBreak);
      
      double	dt = (*this - Owner->tMean());
      double	c1=0.0, c2=0.0, d, Cabel1=0.9, Cabel2=0.0;


      d = 1.0;
      for (int i=0; i<St_1Info->cl_Order(); i++)
	{
	  c1+=St_1->p_Clock(i)->v()*d;
	  d*=dt;
	};
      d = 1.0;
      for (int i=0; i<St_2Info->cl_Order(); i++)
	{
	  c2+=St_2->p_Clock(i)->v()*d;
	  d*=dt;
	};
      
      Cabel1 = St_1Info->isAttr(SBStationInfo::BadCable)?0.0:
	St_1Info->isAttr(SBStationInfo::InvCable)?-Cable_1:Cable_1;
      Cabel2 = St_2Info->isAttr(SBStationInfo::BadCable)?0.0:
	St_2Info->isAttr(SBStationInfo::InvCable)?-Cable_2:Cable_2;
      if (St_1->p_Cable()->isAttr(SBDerivation::IsInRun))
	St_1->p_Cable()->setD( Cabel1, *this);
      if (St_2->p_Cable()->isAttr(SBDerivation::IsInRun))
	St_2->p_Cable()->setD(-Cabel2, *this);

      if (St_1->p_Cable()->v()!=0.0)
	Cabel1 *= 1.0 + St_1->p_Cable()->v();

      if (St_2->p_Cable()->v()!=0.0)
	Cabel2 *= 1.0 + St_2->p_Cable()->v();

      //      dCable = (St_2Info->isAttr(SBStationInfo::InvCable)?-Cable_2:Cable_2) - 
      //	(St_1Info->isAttr(SBStationInfo::InvCable)?-Cable_1:Cable_1);
      double delayC=0.0;
      /*
	O_C = DelayRes =
 	Delay + (Cabel2 - Cabel1) + AmbiguityFactor*GrDelayAmbig 
	- ( 
	CalcedDelay->calc(Mgr, St_1, St_2, Src, this, St_1Info, St_2Info)
	+ DelayIon 
	+ (St_2Info->clockBreaks().bt(*this) - St_1Info->clockBreaks().bt(*this))*1.0e-12
	+ (St_2Info->clocksValue(dt) - St_1Info->clocksValue(dt))
	+ (c2 - c1)
	);
      */
      delayC = (Cabel1 - Cabel2)
	+ CalcedDelay->calc(Mgr, St_1, St_2, Src, this, St_1Info, St_2Info)
	+ DelayIon 
	+ (St_2Info->clockBreaks().bt(*this) - St_1Info->clockBreaks().bt(*this))*1.0e-12
	+ (St_2Info->clocksValue(dt) - St_1Info->clocksValue(dt))
	+ (c2 - c1);

      O_C = DelayRes = Delay + AmbiguityFactor*GrDelayAmbig - delayC;

      
      /*=*/
      bool		IsTstOutput = TRUE;
      QString		StrTst;
      IsTstOutput = FALSE;
      if (IsTstOutput)
	{
	  double	PhiCorr;
	  double	delTau = delayC;

	  delTau = (Cabel1 - Cabel2)
	    + CalcedDelay->totalDelay()
	    - DelayIon 
	    + (St_2Info->clockBreaks().bt(*this) - St_1Info->clockBreaks().bt(*this))*1.0e-12
	    + (St_2Info->clocksValue(dt) - St_1Info->clocksValue(dt))
	    + (c2 - c1);
	  
	  PhiCorr = 360.0*fmod(refFreq()*1.0e6*delTau, 1.0);
	  StrTst.sprintf("%11.6f  [%-8s]-[%-8s] %8s", 
			 PhiCorr,
			 (const char*)St_1Info->aka(), 
			 (const char*)St_2Info->aka(), 
			 (const char*)SrInfo->name()
			 );
	  std::cout << StrTst << "\n";
	};
      /*=*/

      /*
	static QString LastSessionName;
	static double ddd;
	
	if (LastSessionName != Owner->name())
	{
	LastSessionName = Owner->name();
	ddd = (rand()-RAND_MAX/2.0)/RAND_MAX;
	std::cout << LastSessionName << "\n";
	};
	
	O_C = DelayRes = 
 	1.0e-9*ddd*(station_1Idx() + station_2Idx());
	//	      (Cabel2 - Cabel1);
      */

      
      ZenithDelay_1 = CalcedDelay->totZenithDelay1();
      ZenithDelay_2 = CalcedDelay->totZenithDelay2();
      
      d = 1.0;
      int nCl = Mgr->prj()->cfg().p().clock0().type()==SBParameterCfg::PT_STH?1:
	std::max(St_1Info->cl_Order(), St_2Info->cl_Order());
      for (int i=0; i<nCl; i++)
	{
	  if (St_1->p_Clock(i)->isAttr(SBDerivation::IsInRun))
	    St_1->p_Clock(i)->setD(-d, *this);
	  if (St_2->p_Clock(i)->isAttr(SBDerivation::IsInRun))
	    St_2->p_Clock(i)->setD( d, *this);
	  d*=dt;
	};
      
      SBNamed		 baseName(St_1Info->toString() + ":" + St_2Info->toString());
      if ( !(BaseInfo = owner()->baseList()->find(&baseName)) )
	Log->write(SBLog::ERR, SBLog::DATA, ClassName() + ": cannot find base [" +
		   St_1Info->toString() + ":" + St_2Info->toString() + "] aka " +
		   St_1Info->aka() + ":" + St_2Info->aka() + " in the list of session's bases");
      
      // update wrms info:
      // for the session:
      St_1Info ->addWRMS(O_C, Weight);
      St_2Info ->addWRMS(O_C, Weight);
      SrInfo   ->addWRMS(O_C, Weight);
      if (BaseInfo)
	BaseInfo->addWRMS(O_C, Weight);

      // for the project:
      if ( (St_1Info=Mgr->prj()->stationList()->find(St_1Info)) )
	St_1Info ->addWRMS(O_C, Weight);
      else 
	Log->write(SBLog::ERR, SBLog::STATION | SBLog::DELAYALL,
		   ClassName() + ": cannot find the station_1 [" + St_1->id().toString() + "] aka [" +
		   St_1->name() + "] in the project's list of stations");
      if ( (St_2Info=Mgr->prj()->stationList()->find(St_2Info)) )
	St_2Info ->addWRMS(O_C, Weight);
      else 
	Log->write(SBLog::ERR, SBLog::STATION | SBLog::DELAYALL,
		   ClassName() + ": cannot find the station_2 [" + St_2->id().toString() + "] aka [" +
		   St_2->name() + "] in the project's list of stations");
      if ( (SrInfo=Mgr->prj()->sourceList()->find(SrInfo)) )
	SrInfo ->addWRMS(O_C, Weight);
      else 
	Log->write(SBLog::ERR, SBLog::SOURCE | SBLog::DELAYALL,
		   ClassName() + ": cannot find the source [" + Src->name() + "] " +
		   " in the project's list of sources");

      if (BaseInfo && (BaseInfo=Mgr->prj()->baseList()->find(BaseInfo)) )
	BaseInfo->addWRMS(O_C, Weight);
      else 
	Log->write(SBLog::ERR, SBLog::STATION | SBLog::DELAYALL, 
		   ClassName() + ": cannot find base [" +
		   St_1Info->toString() + ":" + St_2Info->toString() + "] aka " +
		   St_1Info->aka() + ":" + St_2Info->aka() + " in the project's list of bases");
      
      Owner->addDelayRMS(O_C, Weight);
      addAttr(processed);
    };

  VLBI_Obs2SRIF.set(0, O_C);
  //  VLBI_Sig2SRIF.set(0, DelayErr);
  VLBI_Sig2SRIF.set(0, DelaySigma);
  //  VLBI_Obs2SRIF.set(1, rate);
  //  VLBI_Sig2SRIF.set(1, RateErr);
};

const SBVector& SBObsVLBIEntry::O_C()
{
  return VLBI_Obs2SRIF;
};

const SBVector& SBObsVLBIEntry::Sigma()
{
  return VLBI_Sig2SRIF;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBObsVLBIEntry's friends implementation						*/
/*												*/
/*==============================================================================================*/
SBDS_dat &operator<<(SBDS_dat& s, const SBObsVLBIEntry& Entry)
{
  // Warning: does not use SBObservation's friends for IO (even if they are exist)
  return (SBDS_dat&)
    (s << (const SBMJD&)Entry << Entry.Idx << Entry.Station_1 << Entry.Station_2 << Entry.Source <<
     Entry.Delay << Entry.DelayErr << Entry.Rate << Entry.RateErr << 
     (Q_INT8)Entry.QualCode[0] << (Q_INT8)Entry.QualCode[1] << 
     Entry.DUFlag << Entry.RUFlag << 
     Entry.Correlation << Entry.FringePhase <<
     Entry.Cable_1 << Entry.Cable_2 <<
     Entry.AtmTemp_1 << Entry.AtmTemp_2 << Entry.AtmPress_1 <<
     Entry.AtmPress_2 << Entry.AtmHum_1 << Entry.AtmHum_2 <<
     Entry.DelayIon << Entry.DelayIonErr << Entry.RateIon << Entry.RateIonErr <<
     Entry.RefFreq << Entry.GrDelayAmbig << Entry.IonErrorFlag);
};

SBDS_dat &operator>>(SBDS_dat& s, SBObsVLBIEntry& Entry)
{
  // Warning: does not use SBObservation's friends for IO (even if they are exist)
  Entry.setTech(TECH_VLBI);
  return (SBDS_dat&)
    (s >> (SBMJD&)Entry >> Entry.Idx >> Entry.Station_1 >> Entry.Station_2 >> Entry.Source >>
     Entry.Delay >> Entry.DelayErr >> Entry.Rate >> Entry.RateErr >> 
     (Q_INT8&)Entry.QualCode[0] >> (Q_INT8&)Entry.QualCode[1] >>
     Entry.DUFlag >> Entry.RUFlag >>
     Entry.Correlation >> Entry.FringePhase >>
     Entry.Cable_1 >> Entry.Cable_2 >>
     Entry.AtmTemp_1 >> Entry.AtmTemp_2 >> Entry.AtmPress_1 >>
     Entry.AtmPress_2 >> Entry.AtmHum_1 >> Entry.AtmHum_2 >> 
     Entry.DelayIon >> Entry.DelayIonErr >> Entry.RateIon >> Entry.RateIonErr >>
     Entry.RefFreq >> Entry.GrDelayAmbig >> Entry.IonErrorFlag);
};

SBDS_aux &operator<<(SBDS_aux& s, const SBObsVLBIEntry& Entry)
{
  // Warning: does not use SBObservation's friend for IO (even if they are exist)
  return (SBDS_aux&)(s << (const SBAttributed&)Entry << Entry.Idx << Entry.AmbiguityFactor 
		     << Entry.DelayRes << Entry.RateRes
		     << Entry.AltAtmTemp_1 << Entry.AltAtmTemp_2 << Entry.AltAtmPress_1
		     << Entry.AltAtmPress_2 << Entry.AltAtmHum_1 << Entry.AltAtmHum_2
		     << Entry.ZenithDelay_1 << Entry.ZenithDelay_2);
};

SBDS_aux &operator>>(SBDS_aux& s, SBObsVLBIEntry& Entry)
{
  // Warning: does not use SBObservation's friend for IO (even if they are exist)
  Entry.setTech(TECH_VLBI);
  return (SBDS_aux&)(s >> (SBAttributed&)Entry >> Entry.Idx >> Entry.AmbiguityFactor 
		     >> Entry.DelayRes >> Entry.RateRes
		     >> Entry.AltAtmTemp_1 >> Entry.AltAtmTemp_2 >> Entry.AltAtmPress_1
		     >> Entry.AltAtmPress_2 >> Entry.AltAtmHum_1 >> Entry.AltAtmHum_2
		     >> Entry.ZenithDelay_1 >> Entry.ZenithDelay_2);
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBVLBISesInfo										*/
/*												*/
/*==============================================================================================*/
SBVLBISesInfo::SBVLBISesInfo(const QString& Name_) : SBNamed(Name_), SBAttributed()
{
  ImportedFrom	= IF_UNKN;
  TStart	= TZero;
  TFinis	= TZero;
  DateCreat	= TZero;
  TMean		= TZero;
  Version	= -1;
  LocalVersion	= 1;
  NetworkID	= (const char*) "UNDEF";
  SubKey	= (const char*) "_";
  ExpDescr	= (const char*) "UNKN-001 xband, Vasia";
  UserFlag	= "A";
  OfficialName	= "N/A";
  Code		= "N/A";
  CorrelatorName= "N/A";
  DelayType	= DT_UNKN;
  NumSta	= 0;
  NumBas	= 0;
  NumSou	= 0;
  NumObs	= 0;
  DelayRMS2	= 0.0;
  RateRMS2	= 0.0;
  DelaySumW2	= 0.0;
  RateSumW2	= 0.0;
};

SBVLBISesInfo::SBVLBISesInfo(const SBVLBISesInfo& Si) : SBNamed(Si), SBAttributed(Si)
{
  ImportedFrom	= Si.importedFrom();
  TStart	= Si.tStart();
  TFinis	= Si.tFinis();
  TMean		= Si.TMean;
  DateCreat	= Si.dateCreat();
  Version	= Si.version();
  LocalVersion	= Si.localVersion();
  NetworkID	= Si.networkID().copy();
  SubKey	= Si.subKey().copy();
  ExpDescr	= Si.expDescr().copy();
  UserFlag	= Si.userFlag().copy();
  OfficialName	= Si.officialName().copy();
  Code		= Si.code().copy();
  CorrelatorName= Si.correlatorName().copy();
  DelayType	= Si.delayType();
  NumSta	= Si.numSta();
  NumBas	= Si.numBas();
  NumSou	= Si.numSou();
  NumObs	= Si.numObs();
  DelayRMS2	= Si.delayRMS2();
  RateRMS2	= Si.rateRMS2();
  DelaySumW2	= Si.DelaySumW2;
  RateSumW2	= Si.RateSumW2;
};

SBVLBISesInfo& SBVLBISesInfo::operator= (const SBVLBISesInfo& Si)
{
  setName(Si.name());
  (SBAttributed&)*this=(SBAttributed)Si;
  ImportedFrom	= Si.importedFrom();
  DateCreat	= Si.dateCreat();
  TStart	= Si.tStart();
  TFinis	= Si.tFinis();
  TMean		= Si.tMean();
  Version	= Si.version();
  LocalVersion	= Si.localVersion();
  SubKey	= Si.subKey().copy();
  NetworkID	= Si.networkID().copy();
  ExpDescr	= Si.expDescr().copy();
  UserFlag	= Si.userFlag().copy();
  OfficialName	= Si.officialName().copy();
  Code		= Si.code().copy();
  CorrelatorName= Si.correlatorName().copy();
  DelayType	= Si.delayType();
  NumSta	= Si.numSta();
  NumBas	= Si.numBas();
  NumSou	= Si.numSou();
  NumObs	= Si.numObs();
  DelayRMS2	= Si.delayRMS2();
  RateRMS2	= Si.rateRMS2();
  DelaySumW2	= Si.DelaySumW2;
  RateSumW2	= Si.RateSumW2;
  return *this;
};

QString SBVLBISesInfo::fileName(File_Type FType)
{
  QString a;
  return a.sprintf("%s%04d/%s/%s_V%03d.%s", 
		   (const char*)SetUp->path2Data(), TStart.year(), (const char*)SubKey,
		   (const char*)Name, Version, FType==FT_DAT?"dat":"aux");
};

void SBVLBISesInfo::checkPath()
{
  QDir		d;
  QString	a = (const char*)SetUp->path2Data();

  if (!d.exists(a) && !d.mkdir(a))
    Log->write(SBLog::ERR, SBLog::DATA, ClassName() + ": can't create dir [" + a + "]");

  a += QString("1970").setNum(TStart.year());
  if (!d.exists(a) && !d.mkdir(a))
    Log->write(SBLog::ERR, SBLog::DATA, ClassName() + ": can't create dir [" + a + "]");
  
  a += "/" + SubKey;
  if (!d.exists(a) && !d.mkdir(a))
    Log->write(SBLog::ERR, SBLog::DATA, ClassName() + ": can't create dir [" + a + "]");
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBVLBISesInfo's friends implementation							*/
/*												*/
/*==============================================================================================*/
QDataStream &operator<<(QDataStream& s, const SBVLBISesInfo& S)
{
  //  std::cout << QString().sprintf("RMS: %.10e; SW: %.10e \n", S.DelayRMS, S.DelaySumW);
  return s << (const SBNamed&)S << (const SBAttributed&)S << (uint)S.ImportedFrom 
	   << S.TStart << S.TFinis << S.TMean << S.DateCreat
	   << S.Version << S.LocalVersion << S.SubKey << S.NetworkID 
	   << S.ExpDescr << S.UserFlag << S.OfficialName << S.Code << S.CorrelatorName 
	   << (uint)S.DelayType 
	   << S.NumSta << S.NumBas << S.NumSou << S.NumObs << S.DelayRMS2 << S.RateRMS2
	   << S.DelaySumW2 << S.RateSumW2;
};

QDataStream &operator>>(QDataStream& s, SBVLBISesInfo& S)
{
  return s >> (SBNamed&)S >> (SBAttributed&)S >> (uint&)S.ImportedFrom 
    //  s >> (SBNamed&)S >> (SBAttributed&)S >> (uint&)S.ImportedFrom 
	   >> S.TStart >> S.TFinis >> S.TMean >> S.DateCreat
	   >> S.Version >> S.LocalVersion >> S.SubKey >> S.NetworkID 
	   >> S.ExpDescr >> S.UserFlag >> S.OfficialName >> S.Code >> S.CorrelatorName 
	   >> (uint&)S.DelayType 
	   >> S.NumSta >> S.NumBas >> S.NumSou >> S.NumObs >> S.DelayRMS2 >> S.RateRMS2
	   >> S.DelaySumW2 >> S.RateSumW2;
  //  std::cout << QString().sprintf("RMS: %.10e; SW: %.10e \n", S.DelayRMS, S.DelaySumW);
  //  return s;
};

SBDS_dat &operator<<(SBDS_dat& s, const SBVLBISesInfo& S)
{
  return (SBDS_dat&)(s << (const SBNamed&)S << (uint)S.ImportedFrom 
		     << S.TStart << S.TFinis << S.TMean << S.DateCreat 
		     << S.Version << S.SubKey << S.ExpDescr << S.OfficialName << S.Code 
		     << S.CorrelatorName << (uint)S.DelayType);
};

SBDS_dat &operator>>(SBDS_dat& s, SBVLBISesInfo& S)
{
  return (SBDS_dat&)(s >> (SBNamed&)S >> (uint&)S.ImportedFrom 
		     >> S.TStart >> S.TFinis >> S.TMean >> S.DateCreat 
		     >> S.Version >> S.SubKey >> S.ExpDescr >> S.OfficialName >> S.Code
		     >> S.CorrelatorName >> (uint&)S.DelayType);
};

SBDS_aux &operator<<(SBDS_aux& s, const SBVLBISesInfo& S)
{
  return (SBDS_aux&)(s << (const SBAttributed&)S << S.NetworkID << S.UserFlag << S.LocalVersion);
};

SBDS_aux &operator>>(SBDS_aux& s, SBVLBISesInfo& S)
{
  //  S.DelayRMS = S.RateRMS = 0.0;
  return (SBDS_aux&)(s >> (SBAttributed&)S >> S.NetworkID >> S.UserFlag >> S.LocalVersion);
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBVLBISession implementation								*/
/*												*/
/*==============================================================================================*/
/**A constructor. */
SBVLBISession::SBVLBISession() : SBObsVLBIList(), SBVLBISesInfo()
{
  setAutoDelete(TRUE);
  StationList.setAutoDelete(TRUE);
  BaseList   .setAutoDelete(TRUE);
  SourceList .setAutoDelete(TRUE);

  StationByIdx = new QIntDict<SBStationInfo>;
  StationByIdx-> setAutoDelete(FALSE);
  StationByAka = new QDict<SBStationInfo>;
  StationByAka-> setAutoDelete(FALSE);

  BaseByIdx = new QIntDict<SBBaseInfo>;
  BaseByIdx-> setAutoDelete(FALSE);
  BaseByAka = new QDict<SBBaseInfo>;
  BaseByAka-> setAutoDelete(FALSE);

  SourceByIdx  = new QIntDict<SBSourceInfo>;
  SourceByIdx -> setAutoDelete(FALSE);
  SourceByAka  = new QDict<SBSourceInfo>;
  SourceByAka -> setAutoDelete(FALSE);

  RefFreq	= 0.0;
  GrDelayAmbig	= 0.0;
  Dump		= NULL;
};

/**A destructor. */
SBVLBISession::~SBVLBISession()
{
  if (StationByIdx) delete StationByIdx;
  if (StationByAka) delete StationByAka;
  if (BaseByIdx   ) delete BaseByIdx;
  if (BaseByAka   ) delete BaseByAka;
  if (SourceByIdx ) delete SourceByIdx;
  if (SourceByAka ) delete SourceByAka;
};

/**Updates dictionaries.
 * This function have to be called after loading session from a stream to keep
 * the dictionaries (SBVLBISession::StationByIdx, SBVLBISession::StationByAka,
 * SBVLBISession::SourceByIdx and SBVLBISession::SourceByAka) up to date.
 */
void SBVLBISession::fillDicts()
{
  //stations:
  if (StationByIdx) delete StationByIdx;
  StationByIdx = new QIntDict<SBStationInfo>(StationList.count()+5);
  StationByIdx-> setAutoDelete(FALSE);
  if (StationByAka) delete StationByAka;
  StationByAka = new QDict<SBStationInfo>(StationList.count()+5);
  StationByAka-> setAutoDelete(FALSE);
  SBStationInfo *st=NULL;
  for (st=StationList.first(); st; st=StationList.next())
    {
      StationByIdx->insert((long)st->idx(), st);
      if (StationByAka->find((const char*)st->aka()))
	Log->write(SBLog::ERR, SBLog::DATA, ClassName() + ": got a duplicate AKA for station [" +
		   st->aka() + "]");
      else 
	StationByAka->insert((const char*)st->aka(), st);
    };

  //bases:
  if (BaseByIdx) delete BaseByIdx;
  BaseByIdx = new QIntDict<SBBaseInfo>(BaseList.count()+5);
  BaseByIdx-> setAutoDelete(FALSE);
  if (BaseByAka) delete BaseByAka;
  BaseByAka = new QDict<SBBaseInfo>(BaseList.count()+5);
  BaseByAka-> setAutoDelete(FALSE);
  SBBaseInfo *b=NULL;
  for (b=BaseList.first(); b; b=BaseList.next())
    {
      BaseByIdx->insert((long)b->idx(), b);
      if (BaseByAka->find((const char*)b->aka()))
	Log->write(SBLog::ERR, SBLog::DATA, ClassName() + ": got a duplicate AKA for base [" +
		   b->aka() + "]");
      else 
	BaseByAka->insert((const char*)b->aka(), b);
    };

  //sources:
  if (SourceByIdx) delete SourceByIdx;
  SourceByIdx = new QIntDict<SBSourceInfo>(SourceList.count()+5);
  SourceByIdx-> setAutoDelete(FALSE);
  if (SourceByAka) delete SourceByAka;
  SourceByAka = new QDict<SBSourceInfo>(SourceList.count()+5);
  SourceByAka-> setAutoDelete(FALSE);
  SBSourceInfo *so;
  for (so=SourceList.first(); so; so=SourceList.next())
    {
      SourceByIdx->insert((long)so->idx(), so);
      if (SourceByAka->find((const char*)so->aka()))
	Log->write(SBLog::ERR, SBLog::DATA, ClassName() + ": got a duplicate AKA for source [" +
		   so->aka() + "]");
      else
	SourceByAka->insert((const char*)so->aka(), so);
    };
};

void SBVLBISession::calcTMean()
{
  double f=0.0, t=0.0, s2=0.0;
  TStart = *first();
  TFinis = *last();
  for (SBObsVLBIEntry *E=first(); E; E=next())
    if (!E->isAttr(SBObsVLBIEntry::notValid))
      {
	t+=E->mjd()*(s2=1.0/(E->delayErr()*E->delayErr() + E->rateErr()*E->rateErr()));
	f+=s2;
      };
  TMean = t/f;
};

void SBVLBISession::checkArtMeteo()
{
  float			MaxTemp=0.0, MinTemp=0.0;
  float			MaxPres=0.0, MinPres=0.0;
  float			MaxRelH=0.0, MinRelH=0.0;
  bool			IsFirst;
  SBObsVLBIEntry	*E;
  SBStationInfo		*StI;

  for (StI = StationList.first(); StI; StI = StationList.next())
    {
      IsFirst = TRUE;
      for (E=first(); E; E=next())
	{
	  if (E->Station_1==StI->idx())
	    {
	      if (IsFirst)
		{
		  MaxTemp = E->AtmTemp_1; 
		  MinTemp = E->AtmTemp_1;
		  MaxPres = E->AtmPress_1; 
		  MinPres = E->AtmPress_1;
		  MaxRelH = E->AtmHum_1;
		  MinRelH = E->AtmHum_1;
		  IsFirst = FALSE;
		}
	      else
		{
		  MaxTemp = std::max(MaxTemp, E->AtmTemp_1); 
		  MinTemp = std::min(MinTemp, E->AtmTemp_1);
		  MaxPres = std::max(MaxPres, E->AtmPress_1); 
		  MinPres = std::min(MinPres, E->AtmPress_1);
		  MaxRelH = std::max(MaxRelH, E->AtmHum_1);
		  MinRelH = std::min(MinRelH, E->AtmHum_1);
		};
	    }
	  else if (E->Station_2==StI->idx())
	    {
	      if (IsFirst)
		{
		  MaxTemp = E->AtmTemp_2;
		  MinTemp = E->AtmTemp_2;
		  MaxPres = E->AtmPress_2;
		  MinPres = E->AtmPress_2;
		  MaxRelH = E->AtmHum_2;
		  MinRelH = E->AtmHum_2;
		  IsFirst = FALSE;
		}
	      else
		{
		  MaxTemp = std::max(MaxTemp, E->AtmTemp_2);
		  MinTemp = std::min(MinTemp, E->AtmTemp_2);
		  MaxPres = std::max(MaxPres, E->AtmPress_2);
		  MinPres = std::min(MinPres, E->AtmPress_2);
		  MaxRelH = std::max(MaxRelH, E->AtmHum_2);
		  MinRelH = std::min(MinRelH, E->AtmHum_2);
		};
	    };
	};
      if (MaxTemp==MinTemp || MaxPres==MinPres || MaxRelH==MinRelH)
	StI->addAttr(SBStationInfo::ArtMeteo);
    };

  bool IsArtAllMeteos = TRUE;
  bool IsArtMeteo     = FALSE;
  for (StI = StationList.first(); StI; StI = StationList.next())
    {
      if (!StI->isAttr(SBStationInfo::ArtMeteo))
	IsArtAllMeteos = FALSE;
      else
	IsArtMeteo = TRUE;
    };
  if (IsArtAllMeteos)
    {
      addAttr(ArtAllMeteos);
      addHistoryLine("WARNING: All stations have artificial meteo data.");
    };
  if (IsArtMeteo)
    addAttr(ArtMeteo);
};

void SBVLBISession::checkAttributres()
{
  bool			HasBreak;
  SBObsVLBIEntry	*E;
  SBStationInfo		*StI;

  for (StI = StationList.first(); StI; StI = StationList.next())
    {
      HasBreak = FALSE;
      for (E=first(); E; E=next())
	{
	  if (E->Station_1==StI->idx() && E->isAttr(SBObsVLBIEntry::breakClock1))
	    HasBreak = TRUE;
	  else if (E->Station_2==StI->idx() && E->isAttr(SBObsVLBIEntry::breakClock2))
	    HasBreak = TRUE;
	};
      if (HasBreak)
	StI->addAttr(SBStationInfo::HasClockBreak);
      else 
	StI->delAttr(SBStationInfo::HasClockBreak);
    };

  bool IsArtAllMeteos = TRUE;
  bool IsArtMeteo     = FALSE;
  HasBreak = FALSE;
  for (StI = StationList.first(); StI; StI = StationList.next())
    {
      if (!StI->isAttr(SBStationInfo::ArtMeteo))
	IsArtAllMeteos = FALSE;
      else
	IsArtMeteo = TRUE;
      if (StI->isAttr(SBStationInfo::HasClockBreak))
	HasBreak = TRUE;
    };

  if (IsArtAllMeteos)
    addAttr(ArtAllMeteos);
  else 
    delAttr(ArtAllMeteos);

  if (IsArtMeteo)
    addAttr(ArtMeteo);
  else 
    delAttr(ArtMeteo);

  if (HasBreak)
    addAttr(HasClockBreak);
  else
    delAttr(HasClockBreak);
};

void SBVLBISession::dumpUserInfo(const QString& Path)
{
  QFile			file;
  QTextStream		ts;
  FILE			*pipe = 
    SetUp->fcList()->open4OutDflt(Path + "/" + name() + SetUp->fcList()->defaultExt(), file, ts);

  // Info:
  ts << "Dump of UserInfo by " << ::Version.selfName() << "-" << ::Version.toString() 
     << " on " << QDateTime::currentDateTime().toString();

  // Attributes:
  ts << "\n+ATTRIBUTES OF SESSION:";
  ts << "\n PREPROC: "		<< (isAttr(preProc)?"Y":"N");
  ts << "\n NOTVALID: "		<< (isAttr(notValid)?"Y":"N");
  ts << "\n BADCABLE: "		<< (isAttr(BadCable)?"Y":"N");
  ts << "\n BADALLCABLES: "	<< (isAttr(BadAllCables)?"Y":"N");
  ts << "\n BADMETEO: "		<< (isAttr(BadMeteo)?"Y":"N");
  ts << "\n BADALLMETEOS: "	<< (isAttr(BadAllMeteos)?"Y":"N");
  ts << "\n HASCLOCKBREAK: "	<< (isAttr(HasClockBreak)?"Y":"N");
  ts << "\n CONSTREOP: "	<< (isAttr(ConstrEOP)?"Y":"N");
  ts << "\n ARTMETEO: "		<< (isAttr(ArtMeteo)?"Y":"N");
  ts << "\n ARTALLMETEOS: "	<< (isAttr(ArtAllMeteos)?"Y":"N");
  ts << "\n-ATTRIBUTES OF SESSION\n";

  // Network ID:
  ts << "+NETWORK ID:\n " << NetworkID << "\n-NETWORK ID\n";

  // User Flag:
  ts << "+USER FLAG:\n " << UserFlag << "\n-USER FLAG\n";
  
  // Station List:
  ts << "+STATIONS:\n";
  StationList.dumpUserInfo(ts);
  ts << "-STATIONS\n";

  // Source List:
  ts << "+SOURCES:\n";
  SourceList.dumpUserInfo(ts);
  ts << "-SOURCES\n";

  // Base List:
  ts << "+BASES:\n";
  BaseList.dumpUserInfo(ts);
  ts << "-BASES\n";

  // Observations:
  ts << "+OBSERVATIONS:\n";
  for (SBObsVLBIEntry *E=first(); E; E=next())
    {
      ts << " +SCAN: "
	 << QString().setNum(E->idx()) << " " 
	 << E->toString(SBMJD::F_INTERNAL) << " "
	 << stationInfo(E->station_1Idx())->toString() << " " 
	 << stationInfo(E->station_2Idx())->toString() << " "
	 << sourceInfo(E->sourceIdx())->name() << "\n";
      ts << "  NOTVALID: " << (E->isAttr(SBObsVLBIEntry::notValid)?"Y":"N") << "\n";
      ts << "  BREAKCLOCK1: " << (E->isAttr(SBObsVLBIEntry::breakClock1)?"Y":"N") << "\n";
      ts << "  BREAKCLOCK2: " << (E->isAttr(SBObsVLBIEntry::breakClock2)?"Y":"N") << "\n";
      ts << "  AMBIGUITYFACTOR: " << QString().setNum(E->ambiguityFactor()) << "\n";
      ts << " -SCAN\n";
    };
  ts << "-OBSERVATIONS\n";

  SetUp->fcList()->close(file, pipe, ts);
  Log->write(SBLog::DBG, SBLog::DATA, ClassName() + ": 'User Information' of the session " + name() + " have been dumped");
};

bool SBVLBISession::restoreUserInfo(const QString& Path)
{
  bool Re = TRUE;

  QFile			file;
  QTextStream		ts;
  QString		DumpName = Path + "/" + name();

  QDir			dir(Path, name() + ".*", QDir::Time, QDir::Files | QDir::Readable);
  if (dir.count())
    {
      unsigned int	i=0;
      SBFileConv	*FC = NULL;
      while (!(FC = SetUp->fcList()->guessFileType( (DumpName = Path + "/" + dir[i]) )) && i<dir.count())
	i++;

      if (!FC)
	Re = FALSE;
    }
  else
    {
      Log->write(SBLog::DBG, SBLog::DATA, ClassName() + 
		 ": cannot find dump of 'User Information' of the session " + name() + " in the " + Path);
      return FALSE;
    };
  if (!Re)
    {
      Log->write(SBLog::WRN, SBLog::DATA, ClassName() + 
		 ": cannot find file converter for the dump of 'User Information' of the session " + name());
      return FALSE;
    };

  // start:
  FILE		*pipe = SetUp->fcList()->open4In(DumpName, file, ts);
  QString	Str;

  Str = ts.readLine();
  // make here an aux checking...  

  while (!ts.atEnd())
    {
      Str = ts.readLine();
      if (Str == "+ATTRIBUTES OF SESSION:")	// Attributes:
	{
	  while (!ts.atEnd() && (Str=ts.readLine())!="-ATTRIBUTES OF SESSION")
	    {
	      if (Str.mid(0,1) != " ")
		{
		  Log->write(SBLog::WRN, SBLog::DATA, ClassName() + 
			     ": wrong format of UI file " + DumpName + " (ATTRIBUTES); data are ignored");
		  SetUp->fcList()->close(file, pipe, ts);
		  return FALSE;
		};
	      if (Str.find(" PREPROC:") == 0)
		{
		  if (Str.mid(Str.length()-1)=="Y") addAttr(preProc);
		  else delAttr(preProc);
		}
	      else if (Str.find(" NOTVALID:") == 0)
		{
		  if (Str.mid(Str.length()-1)=="Y") addAttr(notValid);
		  else delAttr(notValid);
		}
	      else if (Str.find(" BADCABLE:") == 0)
		{
		  if (Str.mid(Str.length()-1)=="Y") addAttr(BadCable);
		  else delAttr(BadCable);
		}
	      else if (Str.find(" BADALLCABLES:") == 0)
		{
		  if (Str.mid(Str.length()-1)=="Y") addAttr(BadAllCables);
		  else delAttr(BadAllCables);
		}
	      else if (Str.find(" BADMETEO:") == 0)
		{
		  if (Str.mid(Str.length()-1)=="Y") addAttr(BadMeteo);
		  else delAttr(BadMeteo);
		}
	      else if (Str.find(" BADALLMETEOS:") == 0)
		{
		  if (Str.mid(Str.length()-1)=="Y") addAttr(BadAllMeteos);
		  else delAttr(BadAllMeteos);
		}
	      else if (Str.find(" HASCLOCKBREAK:") == 0)
		{
		  if (Str.mid(Str.length()-1)=="Y") addAttr(HasClockBreak);
		  else delAttr(HasClockBreak);
		}
	      else if (Str.find(" CONSTREOP:") == 0)
		{
		  if (Str.mid(Str.length()-1)=="Y") addAttr(ConstrEOP);
		  else delAttr(ConstrEOP);
		}
	      else if (Str.find(" ARTMETEO:") == 0)
		{
		  if (Str.mid(Str.length()-1)=="Y") addAttr(ArtMeteo);
		  else delAttr(ArtMeteo);
		}
	      else if (Str.find(" ARTALLMETEOS:") == 0)
		{
		  if (Str.mid(Str.length()-1)=="Y") addAttr(ArtAllMeteos);
		  else delAttr(ArtAllMeteos);
		};
	    };
	}
      else if (Str == "+NETWORK ID:")		// Network ID:
	{
	  Str=ts.readLine().mid(1);
	  setNetworkID(Str);
	  if ( (Str=ts.readLine()) != "-NETWORK ID")
	    {
	      Log->write(SBLog::WRN, SBLog::DATA, ClassName() + 
			 ": wrong format of UI file " + DumpName + " (NetworkID); data are ignored");
	      SetUp->fcList()->close(file, pipe, ts);
	      return FALSE;
	    };
	}
      else if (Str == "+USER FLAG:")		// User Flag:
	{
	  Str=ts.readLine().mid(1);
	  setUserFlag(Str);
	  if ( (Str=ts.readLine()) != "-USER FLAG")
	    {
	      Log->write(SBLog::WRN, SBLog::DATA, ClassName() + 
			 ": wrong format of UI file " + DumpName + " (User Flag); data are ignored");
	      SetUp->fcList()->close(file, pipe, ts);
	      return FALSE;
	    };
	}
      else if (Str == "+STATIONS:" && !StationList.restoreUserInfo(ts))		// Station List:
	{
	  Log->write(SBLog::WRN, SBLog::DATA, ClassName() + 
		     ": wrong format of UI file " + DumpName + " (STATIONS); data are ignored");
	  SetUp->fcList()->close(file, pipe, ts);
	  return FALSE;
	}
      else if (Str == "+SOURCES:")		// Source List:
	SourceList.restoreUserInfo(ts);
      else if (Str == "+BASES:")		// Base List:
	BaseList.restoreUserInfo(ts);
      else if (Str == "+OBSERVATIONS:")		// Observations:
	{
	  while (!ts.atEnd() && Re && (Str=ts.readLine())!="-OBSERVATIONS")
	    {
	      if (Str.mid(0,1) != " ")
		{
		  Log->write(SBLog::WRN, SBLog::DATA, ClassName() + 
			     ": wrong format of UI file " + DumpName + " (OBSERVATIONS); data are ignored");
		  SetUp->fcList()->close(file, pipe, ts);
		  return FALSE;
		};
	      if (Str.find(" +SCAN:") == 0)	// Scan:
		{
		  SBMJD	t;
		  SBStationInfo*	St_1=NULL;
		  SBStationInfo*	St_2=NULL;
		  SBSourceInfo*		So  =NULL;
		  SBObsVLBIEntry	*E = parseObsDumpString(Str, t, St_1, St_2, So);
		  if (E)
		    while (!ts.atEnd() && Re && (Str=ts.readLine())!=" -SCAN")
		      {
			if (Str.mid(0,2) != "  ")
			  {
			    Log->write(SBLog::DBG, SBLog::DATA, ClassName() + 
				       ": wrong prefix in UI string: [" + Str + "] (SCAN)");
			    return FALSE;
			  }
			else if (Str.find("  NOTVALID:") == 0)
			  {
			    if (Str.mid(Str.length()-1)=="Y") E->addAttr(SBObsVLBIEntry::notValid);
			    else E->delAttr(SBObsVLBIEntry::notValid);
			  }
			else if (Str.find("  BREAKCLOCK1:") == 0)
			  {
			    if (Str.mid(Str.length()-1)=="Y") E->addAttr(SBObsVLBIEntry::breakClock1);
			    else E->delAttr(SBObsVLBIEntry::breakClock1);
			  }
			else if (Str.find("  BREAKCLOCK2:") == 0)
			  {
			    if (Str.mid(Str.length()-1)=="Y") E->addAttr(SBObsVLBIEntry::breakClock2);
			    else E->delAttr(SBObsVLBIEntry::breakClock2);
			  }
			else if (Str.find("  AMBIGUITYFACTOR:") == 0)
			  {
			    int		j;
			    j=Str.mid(18).toInt(&Re);
			    if (Re)
			      E->setAmbiguityFactor(j);
			  };
		      }
		  else
		    {
		      Log->write(SBLog::DBG, SBLog::DATA, ClassName() + ": cannot find a scan [" + 
				 Str + "] in the list of observations; UI file: " + DumpName);
		    };
		}
	    }
	};
    };
  
  SetUp->fcList()->close(file, pipe, ts);
  Log->write(SBLog::DBG, SBLog::DATA, ClassName() + ": 'User Information' of the session " 
	     + name() + " have been restored");
  return Re;
};

SBObsVLBIEntry* SBVLBISession::parseObsDumpString(const QString& Str, SBMJD& t, 
						  SBStationInfo*& St_1, SBStationInfo*& St_2, SBSourceInfo*& So)
{
  bool			Re=TRUE;
  QString		ss;
  SBStationID		id;
  SBObsVLBIEntry*	E = NULL;
  int			idx, i;
  //          1         2         3         4         5
  //012345678901234567890123456789012345678901234567890123456789
  // +SCAN: 523 044091:26340.0000 40440S002 40439S002 1642+690
  //           012345678901234567890123456789012345678901234567890123456789
  
  // idx:
  i = Str.find(" ", 8);
  idx = Str.mid(8,i-8).toInt(&Re);
  if (!Re || idx<0)
    {
      Log->write(SBLog::WRN, SBLog::DATA, ClassName() + ": wrong format of UI string (SCAN::idx)");
      return NULL;
    };

  // time:
  t.setMJD(SBMJD::F_INTERNAL, Str.mid(i+1, 17));
  if (!t.isGood())
    {
      Log->write(SBLog::WRN, SBLog::DATA, ClassName() + ": wrong format of UI string (SCAN::t)");
      return NULL;
    };
  
  // Station_1:
  if (SBStationID::isValidStr((ss = Str.mid(i+19, 9))))
    {
      id.setID(ss);
      if (!(St_1 = StationList.find(&id)))
	{
	  Log->write(SBLog::WRN, SBLog::DATA, ClassName() + ": cannot find station [" + ss + "] in the list");
	  return NULL;
	};
    }
  else
    {
      Log->write(SBLog::WRN, SBLog::DATA, ClassName() + ": wrong format of UI string (SCAN::St_1)");
      return NULL;
    };

  // Station_2:
  if (SBStationID::isValidStr((ss = Str.mid(i+29, 9))))
    {
      id.setID(ss);
      if (!(St_2 = StationList.find(&id)))
	{
	  Log->write(SBLog::WRN, SBLog::DATA, ClassName() + ": cannot find station [" + ss + "] in the list");
	  return NULL;
	};
    }
  else
    {
      Log->write(SBLog::WRN, SBLog::DATA, ClassName() + ": wrong format of UI string (SCAN::St_2)");
      return NULL;
    };

  // Source:
  SBNamed	N((ss = Str.mid(i+39)));
  if (!(So = SourceList.find(&N)))
    {
      Log->write(SBLog::WRN, SBLog::DATA, ClassName() + ": cannot find source [" + ss + "] in the list");
      return NULL;
    };
  
  // guess by idx:
  if (idx<(int)count() && (E=at(idx)) && (fabsl(*E-t)<0.05/86400.0) && 
      E->station_1Idx()==St_1->idx() && 
      E->station_2Idx()==St_2->idx() && 
      E->sourceIdx()==So->idx())
    return E;
  else // if failed, look for proper combination:
    {
      E=first();
      while (E && (fabsl(*E-t)<0.05/86400.0) && 
	     E->station_1Idx()==St_1->idx() && 
	     E->station_2Idx()==St_2->idx() && 
	     E->sourceIdx()==So->idx())
	E=next();
    };
  
  return E;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBVLBISession's friends implementation							*/
/*												*/
/*==============================================================================================*/
SBDS_dat &operator<<(SBDS_dat& s, const SBVLBISession& S)
{
  s << (const SBVLBISesInfo&)S << S.DBHHistory << (const SBObsVLBIList&)S;
  return s;
};

SBDS_dat &operator>>(SBDS_dat& s, SBVLBISession& S)
{
  s >> (SBVLBISesInfo&)S >> S.DBHHistory>> (SBObsVLBIList&)S;
  S.NumObs = S.count();
  return s;
};

SBDS_aux &operator<<(SBDS_aux& s, SBVLBISession& S)
{
  s << (const SBVLBISesInfo&)S << S.LocalHistory << S.StationList << S.BaseList << S.SourceList
    << S.RefFreq << S.GrDelayAmbig;
  for (SBObsVLBIEntry* Entry = S.first(); Entry; Entry = S.next()) s << *Entry;
  return s;
};

SBDS_aux &operator>>(SBDS_aux& s, SBVLBISession& S)
{
  s >> (SBVLBISesInfo&)S >> S.LocalHistory >> S.StationList >> S.BaseList >> S.SourceList
    >> S.RefFreq >> S.GrDelayAmbig;
  S.NumSta = S.StationList.count();
  S.NumBas = S.BaseList.count();
  S.NumSou = S.SourceList .count();
  S.fillDicts();

  SBObsVLBIEntry e(&S);
  for (SBObsVLBIEntry* Entry = S.first(); Entry; Entry = S.next())
    {
      s >> e;
      if (e.Idx!=Entry->Idx)
	Log->write(SBLog::ERR, SBLog::DATA | SBLog::IO, "SBDS_aux: error in database consistency");
      else
	{
	  (SBAttributed&)*Entry=(SBAttributed)e;
	  Entry->AmbiguityFactor = e.AmbiguityFactor;
	  Entry->DelayRes = e.DelayRes;
	  Entry->RateRes = e.RateRes;
	  Entry->AltAtmTemp_1 = e.AltAtmTemp_1;
	  Entry->AltAtmTemp_2 = e.AltAtmTemp_2;
	  Entry->AltAtmPress_1 = e.AltAtmPress_1;
	  Entry->AltAtmPress_2 = e.AltAtmPress_2;
	  Entry->AltAtmHum_1 = e.AltAtmHum_1;
	  Entry->AltAtmHum_2 = e.AltAtmHum_2;
	  Entry->ZenithDelay_1 = e.ZenithDelay_1;
	  Entry->ZenithDelay_2 = e.ZenithDelay_2;
	};
    };
  return s;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBMasterRecord implementation								*/
/*												*/
/*==============================================================================================*/
/**A constructor.
 * Creates a copy of object.
 */
SBMasterRecord::SBMasterRecord()
  : SBNamed("77MBR44AA"), SBAttributed()
{
  Yr	      = 77;
  SessionName = "NoSession";
  SessionCode = "N/A-77";
  SessionDate = "MBR44";
  CorrelatorName = "MOON";
  DBC	      = "AA";
};

SBMasterRecord::SBMasterRecord(int Yr_, const QString& SName_, const QString& SCode_, 
			       const QString& SDate_, const QString& Corr_, 
			       const QString& DBC_)
  : SBNamed("77MBR44AA"), // the forty sixth of Martober, 1977.
    SBAttributed()
{
  Yr	      = Yr_;
  SessionName = SName_.copy();
  SessionCode = SCode_.copy();
  SessionDate = SDate_.copy();
  DBC	      = DBC_.copy();
  CorrelatorName = Corr_.copy();
  setName(QString().sprintf("%02d", Yr) + SessionDate + DBC);
};

SBMasterRecord& SBMasterRecord::operator=(const SBMasterRecord& R)
{
  SBNamed::operator=(R);
  SBAttributed::operator=(R);
  Yr	      = R.Yr;
  SessionName = R.SessionName;
  SessionCode = R.SessionCode;
  SessionDate = R.SessionDate;
  DBC	      = R.DBC;
  CorrelatorName = R.CorrelatorName;
  return *this;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBMasterFile implementation								*/
/*												*/
/*==============================================================================================*/
/**A constructor.
 * Creates a copy of object.
 */
SBMasterFile::SBMasterFile()
  : SBNamed("vasia.txt"), SBAttributed(), QList<SBMasterRecord>()
{
  setAutoDelete(TRUE);
  Yr	      = 77;
  TModified   = TZero;
};

SBMasterFile::SBMasterFile(int Yr_, const QString& FName_, const SBMJD& TModified_) 
  : SBNamed(FName_), SBAttributed(), QList<SBMasterRecord>()
{
  setAutoDelete(TRUE);
  Yr	      = Yr_;
  TModified   = TModified_;
};

SBMasterFile& SBMasterFile::operator=(const SBMasterFile& F)
{
  setAutoDelete(TRUE); 
  SBNamed::operator=(F);
  SBAttributed::operator=(F);
  QList<SBMasterRecord>::operator=(F);

  Yr	      = F.Yr;
  TModified   = F.TModified;

  return *this;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBMasterFile's friends implementation							*/
/*												*/
/*==============================================================================================*/
QTextStream &operator>>(QTextStream& s, SBMasterFile& F)
{
  QString		Str, dbc;
  SBMasterRecord	*rec/*, *tmp*/;
  F.clear();
  while (!s.eof())
    {
      Str = s.readLine();
      if (Str.mid(0,1)=="|")
	{
	  if (Str.length()==112)
	    {
	      //          1         2         3         4         5         6         7         8         9        10        11
	      //0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901
	      //  SESSION  SESSION DATE  DOY TIME  DUR       STATIONS               SKED CORR STATUS  PF   DBC  SUBM  DEL  MK4
	      //    NAME     CODE  mondd ddd hh:mm hr                                         yymondd      CODE       days NUM
	      //|NEOS-A405 |NA405 |JAN30| 30|18:00|24|ApFtWzKkNyGc                 |USNO|WASH|01FEB16|    | XE |NASA|     |    |
	      
	      dbc = Str.mid(91, 4).simplifyWhiteSpace();
	      if (dbc.length()==1)
		dbc +="_";
	      rec = new SBMasterRecord(F.yr(), Str.mid( 1, 10).simplifyWhiteSpace(), 
				       Str.mid(12,  6), Str.mid(19,  5), Str.mid(73, 4), dbc);
	      if (F.isAttr(SBMasterFile::Intensive))
		rec->addAttr(SBMasterRecord::Intensive);

	      F.append(rec);
	    }
	  else if (Str.length()==113) // `INT-02' shtamm of the "## Master file format version 1.0"
	    {
	      //          1         2         3         4         5         6         7         8         9        10        11
	      //0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901
	      //  SESSION      SESSION DATE  DOY TIME DUR        STATIONS                SKED CORR STATUS  PF  DBC SUBM DEL  MK4
	      //    NAME        CODE   mondd ddd hh:mm hr                                          yymondd     CODE     days NUM
	      //|IN102-007     |I02007|JAN07|  7|18:30|1  |KkWz                         |USNO|WASH|       |   |XU|NASA|   0|    |
	      
	      dbc = Str.mid(95, 2).simplifyWhiteSpace();
	      if (dbc.length()==1)
		dbc +="_";
	      rec = new SBMasterRecord(F.yr(), Str.mid( 1, 14).simplifyWhiteSpace(),
				       Str.mid(16,  6), Str.mid(23,  5), Str.mid(78, 4), dbc);
	      if (F.isAttr(SBMasterFile::Intensive))
		rec->addAttr(SBMasterRecord::Intensive);
	      
	      F.append(rec);
	    }
	  else if (Str.length()==114) // `INT-not_02' shtamm of the "## Master file format version 1.0"
	    {
	      //          1         2         3         4         5         6         7         8         9        10        11
	      //0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901
	      //  SESSION      SESSION DATE  DOY TIME DUR        STATIONS                SKED CORR STATUS  PF  DBC SUBM DEL  MK4
	      //    NAME        CODE   mondd ddd hh:mm hr                                          yymondd     CODE     days NUM
	      //|INT00-003     |I00003|JAN03|  3|17:47|1.5|GnWz                         |USNO|WASH|       |    |XU|USNO|   0|    |
	      
	      dbc = Str.mid(96, 2).simplifyWhiteSpace();
	      if (dbc.length()==1)
		dbc +="_";
	      rec = new SBMasterRecord(F.yr(), Str.mid( 1, 14).simplifyWhiteSpace(),
				       Str.mid(16,  6), Str.mid(23,  5), Str.mid(78, 4), dbc);
	      if (F.isAttr(SBMasterFile::Intensive))
		rec->addAttr(SBMasterRecord::Intensive);

	      F.append(rec);
	    }
	  else if (Str.length()==124) // New (-03 and -04) shtamms of the "## Master file format version 1.0"
	    {

	      //          1         2         3         4         5         6         7         8         9        10        11         12
	      //0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123
	      //  SESSION  SESSION DATE  DOY TIME  DUR       STATIONS                           SKED CORR STATUS  PF   DBC  SUBM  DEL  MK4
	      //    NAME     CODE  mondd ddd hh:mm hr                                                     yymondd      CODE       days NUM
	      //----------------------------------------------------------------------------------------------------------------------------
	      //|IVS-R4052 |R4052 |JAN02|  2|18:30|24|ApFtGcKkNyTcWz                           |USNO|WASH|03JAN21|3.5 | XE |USNO|  18 |4052|
	      //|IVS-R1052 |R1052 |JAN06|  6|17:00|24|GcHhNyShTcWfWz                           |NASA|HAYS|03JAN23|2.1 | XA |NASA|  16 |    |

	      dbc = Str.mid(103, 4).simplifyWhiteSpace();
	      if (dbc.length()==1)
		dbc +="_";
	      rec = new SBMasterRecord(F.yr(), Str.mid( 1, 10).simplifyWhiteSpace(),
				       Str.mid(12,  6), Str.mid(19,  5), Str.mid(85, 4), dbc);
	      if (F.isAttr(SBMasterFile::Intensive))
		rec->addAttr(SBMasterRecord::Intensive);

	      F.append(rec);
	    }
	  else if (Str.length()==144) // Another `format version 1.0' shtamm (years: 08, 09)
	    {
	      //          1         2         3         4         5         6         7         8         9        10        11         12        13        14
	      //012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123

	      //  SESSION  SESSION DATE  DOY TIME  DUR       STATIONS                                               SKED CORR STATUS  PF   DBC  SUBM  DEL  MK4
	      //    NAME     CODE  mondd ddd hh:mm hr                                                                         yymondd      CODE       days NUM
	      //------------------------------------------------------------------------------------------------------------------------------------------------
	      //|IVS-R1309 |R1309 |JAN02|  2|17:00|24|FtKkNyShTcWfWz                                               |NASA|BONN|08JAN23|3.0 | XA |NASA|  20 |2150|
	      //|IVS-R4309 |R4309 |JAN03|  3|18:30|24|FtKkMaNyTcWz                                                 |USNO|WASH|08JAN16|1.0 | XE |USNO|  12 |4309|

	      dbc = Str.mid(123, 4).simplifyWhiteSpace();
	      if (dbc.length()==1)
		dbc +="_";
	      rec = new SBMasterRecord(F.yr(), Str.mid( 1, 10).simplifyWhiteSpace(),
				       Str.mid(12,  6), Str.mid(19,  5), Str.mid(105, 4), dbc);
	      if (F.isAttr(SBMasterFile::Intensive))
		rec->addAttr(SBMasterRecord::Intensive);

	      F.append(rec);
	    }
	  else
	    Log->write(SBLog::DBG, SBLog::STATION, F.ClassName() + ": cannot parse a `Master file' record: [" + Str + "]");
	}
      else if (Str.mid(0,2)!="--")
	Log->write(SBLog::DBG, SBLog::STATION, F.ClassName() + ": not a `Master file' record: [" + Str + "]");
    };
  F.sort();
  return s;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBMaster implementation								*/
/*												*/
/*==============================================================================================*/
/**A constructor.
 * Creates a copy of object.
 */
SBMaster::SBMaster() : QList<SBMasterFile>()
{
  setAutoDelete(TRUE);

  RecordByName = new QDict<SBMasterRecord>(17);
  RecordByName->setAutoDelete(FALSE);

  FileByName = new QDict<SBMasterFile>(17);
  FileByName->setAutoDelete(FALSE);
};

SBMaster::~SBMaster()
{
  if (RecordByName)
    {
      delete RecordByName;
      RecordByName = NULL;
    };
  if (FileByName)
    {
      delete FileByName;
      FileByName = NULL;
    };
};

void SBMaster::mapRecords()
{
  if (!count())
    return;

  if (RecordByName) 
    delete RecordByName;

  RecordByName = new QDict<SBMasterRecord>(count() + 10);
  RecordByName->setAutoDelete(FALSE);

  SBMasterFile		*mf=NULL;
  SBMasterRecord	*rec=NULL;

  for (mf=first(); mf; mf=next())
    {
      if (mf->count())
	for (rec=mf->first(); rec; rec=mf->next())
	  {
	    if (!RecordByName->find(rec->name()))
	      RecordByName->insert(rec->name(), rec);
	    else
	      Log->write(SBLog::ERR, SBLog::DATA, ClassName() + ": the `Master file' record for the session [" +
			 rec->name() + "] already inserted into dictionary");
	  }
      else
	Log->write(SBLog::WRN, SBLog::DATA, ClassName() + ": got an empty master file [" + mf->name() + "]");
    };
};

void SBMaster::mapFiles()
{
  if (!count())
    return;

  if (FileByName) 
    delete FileByName;

  FileByName = new QDict<SBMasterFile>(count() + 10);
  FileByName->setAutoDelete(FALSE);

  SBMasterFile		*mf=NULL;

  for (mf=first(); mf; mf=next())
    {
      if (!FileByName->find(mf->name()))
	FileByName->insert(mf->name(), mf);
      else
	Log->write(SBLog::ERR, SBLog::DATA, ClassName() + ": the `Master file' [" +
		   mf->name() + "] already inserted into dictionary");
    };
};

SBMasterRecord* SBMaster::lookupMR(const QString& name_) 
{
  int			i=-1;
  QString		Query = name_;
  SBMasterRecord*	Ret = NULL;
  if (RecordByName && !(Ret = RecordByName->find(Query)))
    {
      if ((i=Query.find("*"))!=-1)
	{
	  Query.replace(i, 1, "M");
	  Ret = RecordByName->find(Query);
	};
    };
  return Ret;
};

void SBMaster::importMF(const QString& FName)
{
  SBMasterFile		*mf = NULL;
  QFile			f(FName);
  QString		Str;
  bool			IsOK;

  if (f.open(IO_ReadOnly))
    {
      QTextStream  s;
      s.setDevice(&f);
      
      Str = s.readLine();
      if (Str.contains("## Master file format version 1.0"))
	{
	  Str = s.readLine();
	  Str = s.readLine().simplifyWhiteSpace();

	  int Year = Str.mid(0, 4).toInt(&IsOK);
	  if (IsOK)
	    {
	      if (Year>1999)
		Year -= 2000;
	      else if (Year>1977)
		Year -= 1900;
	      else
		Log->write(SBLog::WRN, SBLog::IO | SBLog::DATA, ClassName() + 
			   ": got a strange number of year: [" +  Str.mid(0, 4) + "], file: " + FName);
	      
	      bool IsInt = (Str.find("INTENSIVES")!=-1);
	      QFileInfo fi(f);
	      QDateTime mdt = fi.lastModified();
	      SBMJD	md(mdt.date().year(), mdt.date().month(), mdt.date().day(), 
			   mdt.time().hour(), mdt.time().minute(), mdt.time().second() + 
			   mdt.time().msec()/1000.0);
	      

	      SBMasterFile   *currentMF = NULL;
	      mf = new SBMasterFile(Year, fi.fileName(), md);

	      if ( !(currentMF=FileByName->find(mf->name())) || currentMF->tModified()<md)
		{
		  if (currentMF) // clear records to be updated:
		    {
		      FileByName->remove(currentMF->name());
		      currentMF->clear();
		      remove(currentMF);
		      Log->write(SBLog::DBG, SBLog::DATA, ClassName() + ": updateing the `Master file' " + 
				 mf->name());
		    };
		  if (IsInt)
		    mf->addAttr(SBMasterFile::Intensive);

		  s >> *mf;
		}
	      else
		Log->write(SBLog::ERR, SBLog::DATA, ClassName() + ": the `Master file' [" +
			   mf->name() + "] already exist");
	      
	      if (mf->count())
		inSort(mf);
	      else
		delete mf;
	    }
	  else
	    Log->write(SBLog::WRN, SBLog::IO | SBLog::DATA, ClassName() + 
		       ": cannot get a year from the string [" +
		       Str.mid(0, 4) + "], file: " + FName + "; ignored");
	}
      else
	Log->write(SBLog::WRN, SBLog::IO | SBLog::DATA, ClassName() + 
		   ": not a `Master file format', file: " + 
		   FName + "; ignored");
      
      f.close();
      s.unsetDevice();
      Log->write(SBLog::DBG, SBLog::IO | SBLog::DATA, ClassName() + ": file " + FName + " has been loaded");
    }
  else
    Log->write(SBLog::WRN, SBLog::IO | SBLog::DATA, ClassName() + ": could not read " + FName + "; ignored");
};

void SBMaster::checkNewFiles(const QString& DirName)
{
  QDir	dir(DirName);
  
  if (dir.exists() && dir.count())
    {
      dir.setFilter (QDir::Files | QDir::Readable);
      dir.setSorting(QDir::Name);

      for (int i=0; i<(int)dir.count(); i++)
	importMF(DirName + dir[i]);
    };
};
/*==============================================================================================*/






/*==============================================================================================*/
/*												*/
/* class SBVLBISet::Network implementation							*/
/*												*/
/*==============================================================================================*/
/**A constructor.
 * Creates a copy of object.
 */
SBVLBISet::Network::Network(const QString& Name_, const QString& Descr_, QList<QString> *Keys_)
  : SBNamed(Name_) 
{
  Description=Descr_; 
  Keys.setAutoDelete(TRUE);
  if (Keys_ && Keys_->count()) 
    for (QString *a=Keys_->first(); a; a=Keys_->next()) Keys.append(new QString(*a));
};

int SBVLBISet::Network::lookup(const QString& T)
{
  int	n=0;
  for (QString *a=Keys.first(); a; a=Keys.next()) 
    if (*a!="" && T.find(*a, 0, FALSE)>-1)
      n++;
  return n;
};

SBVLBISet::Network& SBVLBISet::Network::operator=(SBVLBISet::Network& N)
{
  (SBNamed &)*this = (SBNamed)N;
  Description = N.Description.copy();
  Keys.setAutoDelete(TRUE);
  Keys.clear();
  for (QString *a=N.Keys.first(); a; a=N.Keys.next())
    Keys.append(new QString(*a));
  return *this;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBVLBISet::NetList implementation							*/
/*												*/
/*==============================================================================================*/
/**Looks for network id key.
 *
 */
QString SBVLBISet::NetList::lookup(const QString& T)
{
  int l=0, j=0;
  QString q("UNDEF");
  for (Network *n=first(); n; n=next())
    if ((j=n->lookup(T))>l) 
      {
	l=j;
	q=n->name().copy();
      };
  return q;
};

SBVLBISet::NetList& SBVLBISet::NetList::operator=(NetList& N_)
{
  if (count()) clear();
  for (Network *n=N_.first(); n; n=N_.next()) 
    inSort(new Network(*n));
  return *this;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBVLBISet implementation								*/
/*												*/
/*==============================================================================================*/
/**A constructor.
 * Creates a copy of object.
 */
SBVLBISet::SBVLBISet()
{
  setAutoDelete(TRUE);
  SessionByName = new QDict<SBVLBISesInfo>;
  SessionByName-> setAutoDelete(FALSE);
  Networks.setAutoDelete(TRUE);
  TotalNumObs = 0;
  Master = NULL;
  if (!Networks.count())
    {
      Network *n;

      //-------------------------------------------------------------------------------------------------------
      //
      // General:
      //
      // undescribed:
      Networks.inSort((n = new Network("ASTROM", "ASTROM Network")));
      n->addKey("astrom-");
      n->addKey("astro ");
      n->addKey("astr-");

      // undescribed:
      Networks.inSort((n = new Network("CORE", "CORE Network")));
      n->addKey("core");
      n->addKey("core-");
      n->addKey("cor-ohg");
      n->addKey("core-ohig");
      n->addKey("core-naps");

      //
      // according to the URL <http://ivscc.gsfc.nasa.gov/program/descrip2002.html>:
      //
      // IVS-CONT02:
      //   
      // A two-week continuous VLBI session is proposed for 2002, called CONT02, similar 
      // in concept to the extremely successful CONT94 and CONT96 sessions. The purpose 
      // of the session is to provide a long series of the highest quality data that VLBI can 
      // provide. The weekly R1 and R4 sessions will be observed plus another 10 CONT02 days to 
      // cover the two week period. Because of the large resource usage (weekend observing, 200 tapes) 
      // a special request will be made to the requested stations for the CONT02 session. The
      // planned time for CONT02 is around September.
      Networks.inSort((n = new Network("CONT-02", "CONT-02 Experiments")));
      n->addKey("cont02");
      n->addKey("cont");

      //
      // according to the URL <http://ivscc.gsfc.nasa.gov/program/cont05/>:
      //
      // IVS-CONT05:
      //   
      // CONT05 is a two-week campaign of continuous VLBI sessions, scheduled for observing during 
      // September 2005. The CONT05 sessions are the follow-on to the spectacularly successful CONT94 
      // observed in January 1994 and the follow-up CONT95 (August 1995), CONT96 (fall 1996), and 
      // CONT02 (October 2002).
      Networks.inSort((n = new Network("CONT-05", "CONT-05 Experiments")));
      n->addKey("cont05");
      n->addKey("cont");

      Networks.inSort((n = new Network("CONT-08", "CONT-08 Experiments")));
      n->addKey("cont08");
      n->addKey("cont");

      Networks.inSort((n = new Network("CONT", "CONT-like Experiments")));
      n->addKey("cont");

      //
      // here are other undescribed CONT-like experiments:
      Networks.inSort((n = new Network("CONT-95", "CONT-95 Experiments")));
      n->addKey("cont95");
      n->addKey("cont");
      //
      Networks.inSort((n = new Network("CONT-96", "CONT-96 Experiments")));
      n->addKey("cont96");
      n->addKey("cont");
      //
      Networks.inSort((n = new Network("CONT-M", "CONT-M Experiments")));
      n->addKey("cont-m");
      n->addKey("cont");
      //
      Networks.inSort((n = new Network("CONT-94", "RD94 Experiments")));
      n->addKey("rd94-");

      //
      // according to the URL <http://ivscc.gsfc.nasa.gov/program/descrip2003.html>:
      //
      // The Celestial Reference Frame (CRF) sessions are intended purely to provide 
      // astrometric observations useful in improving the current CRF, and in extending 
      // and densifying the CRF by observing "new" sources. The CRF sessions are scheduled 
      // 8 times this year.
      Networks.inSort((n = new Network("CRF", "IVS-CRF Network")));
      n->addKey("crf");
      n->addKey("crf-");
      n->addKey("ivs-crf");

      // undescribed:
      Networks.inSort((n = new Network("FRAME", "FRAME Network")));
      n->addKey("frame-");

      // undescribed:
      Networks.inSort((n = new Network("GEOCAT", "Geocat Network")));
      n->addKey("geocat");
      n->addKey("geoct-");

      // undescribed:
      Networks.inSort((n = new Network("GEODETIC", "Geodetic Network")));
      n->addKey("geodetic");

      //
      // according to the URL <ftp://cddisa.gsfc.nasa.gov/pub/vlbigsfc/annual91/text.91.Z>:
      // CRUSTAL DYNAMICS PROJECT DATA ANALYSIS - 1991:
      // 
      // "Global, sessions sponsored by the CDP designed to measure a network spanning the Earth."
      Networks.inSort((n = new Network("GLOBAL", "CDP GLOBAL Network")));
      n->addKey("global-");
      n->addKey("glob");	//?
      n->addKey("globl-trf");	//?
      n->addKey("golden");	//?
      n->addKey("gglb");	//?
      n->addKey("glbl-trf");	//?
      n->addKey("gldn-glbl");	//?
      n->addKey("x-global");
      n->addKey("xglobal");

      //
      // according to the URL <ftp://cddisa.gsfc.nasa.gov/pub/vlbigsfc/annual91/text.91.Z>:
      // CRUSTAL DYNAMICS PROJECT DATA ANALYSIS - 1991:
      // 
      // "IRIS-A and POLARIS, NGS-sponsored sessions designed primarily to monitor Earth rotation.
      // These sessions began in November 1980 with HAYSTACK and HRAS 085 and were scheduled every 
      // 7 days. ONSALA60 participated when possible on a monthly basis. HAYSTACK was replaced by 
      // WESTFORD in June 1981. In August 1983 operations were increased to five-day intervals. 
      // Two new stations, RICHMOND and WETTZELL, were brought on-line in late 1983 and became 
      // fully operational in 1984. HRAS 085 was replaced with MOJAVE12 during the summer of 1989.  
      // Through the end of 1990 IRIS-A undertook one 24-hour session every 5 days with MOJAVE12, 
      // RICHMOND, WESTFORD, and WETTZELL along with the monthly participation of ONSALA60 and 
      // occasional participation by MEDICINA. Whenever possible, ONSALA60 continues to observe 
      // monthly. MEDICINA also participates occasionally."
      //
      // "IRIS-A EUR (European), sessions scheduled by adding a mobile unit at a European site 
      // during a regularly scheduled IRIS-A session."
      Networks.inSort((n = new Network("IRIS-A", "IRIS-A, IRIS-A (EUR) Network")));
      n->addKey("iris-a");
      n->addKey("iris a ");	//?
      n->addKey("iris a");	//?
      n->addKey("irisa");	//?
      // and POLARIS:
      Networks.inSort((n = new Network("POLARIS", "POLARIS Network")));
      n->addKey("POLA-");


      //
      // according to the URL <ftp://cddisa.gsfc.nasa.gov/pub/vlbigsfc/annual91/text.91.Z>:
      // CRUSTAL DYNAMICS PROJECT DATA ANALYSIS - 1991:
      // 
      // "IRIS-P (Pacific), observing sessions carried out by the Japanese NAO Earth 
      // Rotation Division using KASHIMA, NOBEY 6M and stations in the U.S."
      Networks.inSort((n = new Network("IRIS-P", "IRIS-P Network")));
      n->addKey("iris-p");
      n->addKey("iris p ");	//?
      n->addKey("iris p");	//?
      n->addKey("iris-p ");	//?
      n->addKey("iris-p,");	//?
      n->addKey("irisp");	//?
      n->addKey("irisP");	//?


      //
      // according to the URL <ftp://cddisa.gsfc.nasa.gov/pub/vlbigsfc/annual93/text.93.Z>:
      // DATA ANALYSIS BY THE NASA SPACE GEODESY PROGRAM--GSFC:
      // 
      // "IRIS-S (South Africa), observing sessions carried out by IfAG using
      // HARTRAO and/or SANTIA12 and some IRIS-A stations in Europe and the U.S."
      Networks.inSort((n = new Network("IRIS-S", "IRIS-S Network")));
      n->addKey("iris-s");
      n->addKey("iris s ");	//?
      n->addKey("iris s");	//?
      n->addKey("iris-s ");	//?
      n->addKey("iris-s,");	//?
      n->addKey("iriss");	//?
      n->addKey("irisS");	//?

      //
      // according to the URL <http://ivscc.gsfc.nasa.gov/program/descrip2003.html>:
      //
      // The purpose of the IVS-E3 sessions is to provide an additional monthly EOP 
      // monitoring session. These sessions will use S2 technology for recording and 
      // processing. The network will be fixed because of the limited number of stations 
      // currently equipped with geodetic S2 capability. Stations are: Algonquin, Yellowknife, 
      // the Canadian Transportable VLBI Antenna (CTVA), TIGO, and Kokee. When more stations 
      // have geodetic S2 equipment these sessions may be expanded.
      Networks.inSort((n = new Network("IVS-E3", "IVS-E3 Network")));
      n->addKey("ivs-e3");

      //
      // according to the URL <http://ivscc.gsfc.nasa.gov/program/descrip2003.html>:
      //
      // The goal of the R&D sessions, as decided by the IVS Program Committee, is to compare 
      // the results of the IVS-R1 and the IVS-R4 sessions. The IVS-R4 sessions seem to be 
      // achieving better earth orientation results with a lower data rate (56 vs. 256). The R&D 
      // sessions are being scheduled to aide the analysts in understanding the 2002 results of 
      // the IVS-R1 and IVS-R4 sessions by scheduling sessions with a simular IVS-R4 network using 
      // the IVS-R1 observing mode. There are 10 sessions planned for 2003.
      Networks.inSort((n = new Network("R&D: IVS-R&D", "IVS-R&D Network")));
      n->addKey("ivs-r&d");



      //
      // according to the URL <http://ivscc.gsfc.nasa.gov/program/descrip2003.html>:
      //
      // The purpose of of the IVS-R1 and IVS-R4 sessions is to provide twice weekly 
      // EOP results on a timely basis. These sessions provide continuity with the 
      // previous NEOS and CORE series.
      //
      // The "1" and "4" indicate that the sessions are on Mondays and Thursdays, 
      // respectively. Widely separate sessions during the week gives approximately 
      // even spacing between these two sessions for monitoring periodic terms in tides 
      // and nutation.
      Networks.inSort((n = new Network("IVS-R1", "IVS-R1 Network")));
      n->addKey("ivs-r1");

      Networks.inSort((n = new Network("IVS-R4", "IVS-R4 Network")));
      n->addKey("ivs-r4");

      //
      // according to the URL <http://ivscc.gsfc.nasa.gov/program/descrip2003.html>:
      //
      // The Survey sessions are two station networks intended purely to survey for 
      // southeren sources. The SURVEY sessions are scheduled 3 times this year.
      Networks.inSort((n = new Network("IVS-SUR", "IVS-SUR Network")));
      n->addKey("ivs-sur");

      //
      // according to the URL <http://ivscc.gsfc.nasa.gov/program/descrip2003.html>:
      //
      // The purpose of the IVS-T2 sessions is to monitor the TRF via monthly sessions. 
      // All geodetic stations participate in at least two T2 sessions each year. These 
      // sessions replace the IRIS-S sessions observed in previous years until 2002.
      Networks.inSort((n = new Network("IVS-T2", "IVS-T2 Network")));
      n->addKey("ivs-t2");

      //
      // according to the URL <ftp://cddisa.gsfc.nasa.gov/pub/vlbigsfc/annual91/text.91.Z>:
      // CRUSTAL DYNAMICS PROJECT DATA ANALYSIS - 1991:
      // 
      // "MERIT, a series of sessions in 1980 sponsored by the International Association for 
      // Geodesy and the International Union for Geodesy and Geophysics to demonstrate the 
      // efficacy of modern techniques in monitoring Earth rotation."
      Networks.inSort((n = new Network("MERIT", "MERIT Network")));
      n->addKey("merit");

      //
      // according to the URL <ftp://cddisa.gsfc.nasa.gov/pub/vlbigsfc/annual91/text.91.Z>:
      // CRUSTAL DYNAMICS PROJECT DATA ANALYSIS - 1991:
      // 
      // "USNO Nav ex, USNO Navnet sessions with the addition of a European station to the 
      // network to improve the determination of UT1."
      Networks.inSort((n = new Network("NAVex", "USNO Nav ex")));
      n->addKey("navex");
      n->addKey("navyex");
      // and:
      // "Navnet, USNO sessions designed to obtain precise measurement of Earth orientation 
      // and nutation parameters."
      Networks.inSort((n = new Network("NAVNET", "USNO NAVNET Network")));
      n->addKey("navnet");
      // and:
      // "USNO test, early USNO sessions done in preparation for Navnets."
      Networks.inSort((n = new Network("NAVtest", "USNO test Experiments")));
      n->addKey("GNUT-");
      n->addKey("NJ2");




      // undescribed:
      Networks.inSort((n = new Network("NEOS-A", "NEOS-A Network")));
      n->addKey("neos-a");
      n->addKey("neos a");
      n->addKey("neosa");

      // undescribed:
      Networks.inSort((n = new Network("NEOS-B", "NEOS-B Network")));
      n->addKey("neos b");
      n->addKey("neosb");
      n->addKey("neos-b");

      //
      // according to the URL <ftp://cddisa.gsfc.nasa.gov/pub/vlbigsfc/annual91/text.91.Z>:
      // CRUSTAL DYNAMICS PROJECT DATA ANALYSIS - 1991:
      // 
      // "Polar, CDP sessions involving stations in Europe, the conterminous U.S., Alaska, 
      // and Japan.  These sessions link the global VLBI reference frame by using stations 
      // which typically do not observe together in the same network."
      Networks.inSort((n = new Network("POLAR", "CDP POLAR Network")));
      n->addKey("polar");

      // undescribed:
      Networks.inSort((n = new Network("POLAR-N", "POLAR-N Network")));
      n->addKey("polar-n");
      n->addKey("polar n");
      n->addKey("polarn");

      // undescribed:
      Networks.inSort((n = new Network("POLAR-S", "POLAR-S Network")));
      n->addKey("polar-s");
      n->addKey("polar s");
      n->addKey("polars");



      //
      // according to the URL <ftp://cddisa.gsfc.nasa.gov/pub/vlbigsfc/annual91/text.91.Z>:
      // CRUSTAL DYNAMICS PROJECT DATA ANALYSIS - 1991:
      // 
      // "Research and Development, CDP sessions designed to test innovations in hardware 
      // and scheduling techniques."
      Networks.inSort((n = new Network("R&D", "R&D Experiments")));
      n->addKey("r&d-");
      n->addKey("rdv-bf");	//?
      n->addKey("rdvl-");	//?
      n->addKey("bonn-r&d");	//?
      n->addKey("ES-TEST");
      //
      Networks.inSort((n = new Network("R&D: ATD", "R&D Experiments")));
      n->addKey("ATD-");
      //
      Networks.inSort((n = new Network("R&D: EXT", "R&D Experiments")));
      n->addKey("ext-r&d");
      //
      Networks.inSort((n = new Network("R&D: LOW-ELEV", "R&D Low-Elev Experiments")));
      n->addKey("low-elev");
      n->addKey("low-elv");
      //
      Networks.inSort((n = new Network("R&D: PHASED", "R&D PHASED Experiments")));
      n->addKey("phase delay");
      n->addKey("phs-dely");
      n->addKey("phse-dely");
      n->addKey("phsedly-");
      n->addKey("CDP-85M");
      n->addKey("CDP-86PA");
      //
      Networks.inSort((n = new Network("R&D: INT", "R&D INT Experiments")));
      n->addKey("R&DINT");
      //
      Networks.inSort((n = new Network("R&D: GGAO", "R&D: GGAO Experiments")));
      n->addKey("ggao-r");




      // undescribed:
      Networks.inSort((n = new Network("REFRAME", "REFRAME Network")));
      n->addKey("reframe");
      n->addKey("reference");
      n->addKey("reference frame");
      n->addKey("ref-");
      n->addKey("ref-frame");

      //
      // according to the URL <http://ivscc.gsfc.nasa.gov/program/descrip2003.html>:
      //
      // There are six bi-monthly coordinated astrometric/geodetic experiments each year 
      // that use the full 10-station VLBA plus up to 10 geodetic stations.
      // These experiments are being coordinated by the geodetic VLBI programs of three 
      // agencies: 1. USNO will perform repeated imaging and correction for source structure.; 
      // 2. NASA will analyze this data to determine a high accuracy terrestrial reference frame.; 
      // and 3. NRAO will use these sessions to provide a service to users who require high 
      // quality positions for small numbers of sources.
      Networks.inSort((n = new Network("R&D: RDV", "R&D-VLBA Experiments")));
      n->addKey("vlba1");
      n->addKey("vlba2");
      n->addKey("vlba3");
      n->addKey("vlba4");
      n->addKey("vlba5");
      n->addKey("vlba6");
      n->addKey("vlba7");
      n->addKey("vlba8");
      n->addKey("vlba9");
      
      //
      // according to the URL <ftp://cddisa.gsfc.nasa.gov/pub/vlbigsfc/annual93/text.93.Z>:
      // DATA ANALYSIS BY THE NASA SPACE GEODESY PROGRAM--GSFC:
      // 
      // "South Hemisphere Survey, sessions sponsored by the NGS using fixed stations
      // in the Southern Hemisphere."
      Networks.inSort((n = new Network("SHS", "SHS Network")));
      n->addKey("sh-s");
      n->addKey("shs-");
      n->addKey("shs0");

      // undescribed:
      Networks.inSort((n = new Network("SURVEY", "SURVEY Network")));
      n->addKey("survey");
      n->addKey("suvey");
      n->addKey("survy");
      n->addKey("sth");
      n->addKey("surn");
      n->addKey("ss1");

      // undescribed:
      Networks.inSort((n = new Network("TRF", "Global-, South-TRF Networks")));
      n->addKey("strf");
      n->addKey("sth-trf");
      n->addKey("gtrf");

      // undescribed:
      Networks.inSort((n = new Network("WAPS", "WAPS Network")));
      n->addKey("waps");




      //-------------------------------------------------------------------------------------------------------
      //
      // Regional:
      //
      //
      // according to the URL <ftp://cddisa.gsfc.nasa.gov/pub/vlbigsfc/annual91/text.91.Z>:
      // CRUSTAL DYNAMICS PROJECT DATA ANALYSIS - 1991:
      // 
      // "Alaska, CDP sessions to monitor motions at several Alaskan mobile sites including 
      // three sites in seismic gaps near the boundary between the Pacific and North American 
      // plates.  The last observations in this program were in the summer of 1990. There are 
      // currently no plans to continue the program."
      Networks.inSort((n = new Network("Alaska", "Alaska Experiments")));
      n->addKey("CDP-84K");
      n->addKey("CDP-84L");
      n->addKey("CDP-84M");
      n->addKey("CDP-84N");
      n->addKey("CDP-85O");
      n->addKey("CDP-85P");
      n->addKey("CDP-85Q");
      n->addKey("CDP-85R");
      n->addKey("CDP-86L"); // "CDP-86LA", "CDP-86LB"
      n->addKey("CDP-86M");
      n->addKey("CDP-86NA");
      n->addKey("CDP-86NB");
      n->addKey("CDP-86O"); // "CDP-86OA", "CDP-86OB"
      n->addKey("CDP-87N"); // "CDP-87NA", "CDP-87NB"
      n->addKey("CDP-87O");
      n->addKey("CDP-87P"); // "CDP-87P", "CDP-87PB", "CDP-87PC"
      n->addKey("CDP-87Q"); // "CDP-87QA", "CDP-87QB"
      n->addKey("CDP-88J"); // "CDP-88J1", "CDP-88J2", "CDP-88J3"
      n->addKey("CDP-88K"); // "CDP-88K1", "CDP-88K2", "CDP-88K3"
      n->addKey("CDP-88L"); // "CDP-88L1", "CDP-88L2", "CDP-88L3"
      n->addKey("CDP-88M"); // "CDP-88M1", "CDP-88M2", "CDP-88M3"
      n->addKey("CDP-88N"); // "CDP-88N1", "CDP-88N2", "CDP-88N3"
      n->addKey("CDP-89L"); // "CDP-89L1", "CDP-89L2", "CDP-89L3"
      n->addKey("CDP-89M"); // "CDP-89M1", "CDP-89M2", "CDP-89M3"
      n->addKey("CDP-89N"); // "CDP-89N1", "CDP-89N2", "CDP-89N3"
      n->addKey("CDP-89O"); // "CDP-89O1", "CDP-89O2", "CDP-89O3"
      n->addKey("CDP-89P"); // "CDP-89P1", "CDP-89P2", "CDP-89P3"
      n->addKey("CDP-90G"); // "CDP-90G1", "CDP-90G2", "CDP-90G3"
      n->addKey("CDP-90H"); // "CDP-90H1", "CDP-90H2", "CDP-90H3"
      n->addKey("CDP-90I"); // "CDP-90IA1", "CDP-90IA2", "CDP-90IB"
      n->addKey("CDP-90J"); // "CDP-90JA1", "CDP-90JA2", "CDP-90JB"


      // ATLANTIC Ocean:
      //
      // according to the URL <ftp://cddisa.gsfc.nasa.gov/pub/vlbigsfc/annual91/text.91.Z>:
      // CRUSTAL DYNAMICS PROJECT DATA ANALYSIS - 1991:
      // 
      // "Atlantic, U.S. to Europe sessions sponsored by the CDP designed to measure motion 
      // between North America and Europe."
      Networks.inSort((n = new Network("Atlantic", "CDP Atlantic")));
      n->addKey("NORTH-ATL");
      n->addKey("TRANS-ATL");
      n->addKey("TRAS-ATL");
      n->addKey("W.ATL-");
      n->addKey("X-ATLNTIC");
      n->addKey("X-Atlantic");
      n->addKey("XATL");
      //
      // "East Atlantic, U.S. to Europe sessions sponsored by the CDP to measure motion 
      // between North America and Europe with emphasis on European stations."
      Networks.inSort((n = new Network("E-Atlantic", "CDP E. Atln.")));
      n->addKey("e-atl-");
      n->addKey("e.atl-");
      //?
      // undescribed:
      Networks.inSort((n = new Network("S-ATL", "South-Atlantic Network")));
      n->addKey("s.atlantic");
      n->addKey("satl");
      n->addKey("sth-atl");
      n->addKey("satlantc");
      n->addKey("satlantic");



      // PASIFIC Ocean:
      //
      // according to the URL <ftp://cddisa.gsfc.nasa.gov/pub/vlbigsfc/annual91/text.91.Z>:
      // CRUSTAL DYNAMICS PROJECT DATA ANALYSIS - 1991:
      // 
      // "Pacific, CDP sessions designed to measure networks in the Pacific Basin."
      Networks.inSort((n = new Network("Pacific", "CDP Pacific Network")));
      n->addKey("E.PACIFC");
      n->addKey("e.pacif");
      n->addKey("east-pac");
      n->addKey("KASHMOJ");
      n->addKey("KASHIMA");
      n->addKey("north-pac");
      n->addKey("ppm-e");
      n->addKey("ppm-n");
      n->addKey("ppm-s");
      n->addKey("ppm-w");
      n->addKey("trans-pac");
      n->addKey("W.PACIFC");
      n->addKey("west-pac");
      n->addKey("W.PACIFC");
      n->addKey("xpac");
      //?
      n->addKey("n-pac");
      n->addKey("n.pac-");
      n->addKey("npac-");
      n->addKey("w-pac");
      n->addKey("w.pac-");
      n->addKey("w.pacifc");
      n->addKey("wpac-");
      n->addKey("t-pac-");
      n->addKey("t.pac-");
      n->addKey("trns-pac");
      n->addKey("x.pac-");
      n->addKey("x-pac");
      n->addKey("pacific");

      //
      // according to the URL <ftp://cddisa.gsfc.nasa.gov/pub/vlbigsfc/annual91/text.91.Z>:
      // CRUSTAL DYNAMICS PROJECT DATA ANALYSIS - 1991:
      // 
      // "California, mobile sessions sponsored by the CDP carried out to measure regional 
      // deformation and episodic motion in California especially at sites associated with 
      // the San Andreas fault."
      Networks.inSort((n = new Network("California", "California")));
      n->addKey("ARI83A");
      n->addKey("CALIFRNIA");
      n->addKey("CDO83O");
      n->addKey("YUMP");
      // cdp-*
      // 83:
      n->addKey("CDP83H");
      n->addKey("CDP83I");
      n->addKey("CDP83J");
      n->addKey("CDP83K");
      n->addKey("CDP83L");
      n->addKey("CDP83M");
      n->addKey("CDP83N");
      n->addKey("CDP83P");
      n->addKey("CDP83Q");
      n->addKey("CDP83R");
      n->addKey("JPL83F");
      // 84:
      n->addKey("CDP-84A");
      n->addKey("CDP-84B");
      n->addKey("CDP-84C");
      n->addKey("CDP-84D");
      n->addKey("CDP-84E");
      n->addKey("CDP-84F");
      n->addKey("CDP-84G");
      n->addKey("CDP-84H");
      n->addKey("CDP-84I");
      n->addKey("CDP-84J");
      n->addKey("CDP-84O");
      n->addKey("CDP-84P");
      n->addKey("CDP-84Q");
      n->addKey("CDP-84R");
      // 85:
      n->addKey("CDP-85A");
      n->addKey("CDP-85B");
      n->addKey("CDP-85C");
      n->addKey("CDP-85D");
      n->addKey("CDP-85E");
      n->addKey("CDP-85F");
      n->addKey("CDP-85G");
      n->addKey("CDP-85H");
      n->addKey("CDP-85I");
      n->addKey("CDP-85T");
      n->addKey("CDP-85U");
      n->addKey("CDP-85V");
      n->addKey("CDP-85W");
      n->addKey("CDP-85X");
      // 86:
      n->addKey("CDP-86B");
      n->addKey("CDP-86C");
      n->addKey("CDP-86H");
      n->addKey("CDP-86I");
      n->addKey("CDP-86J");
      n->addKey("CDP-86PB");
      n->addKey("CDP-86Q");
      n->addKey("CDP-86R");
      n->addKey("CDP-86T");
      n->addKey("CDP-86U");
      n->addKey("CDP-86W");
      n->addKey("CDP-86X");
      // 87:
      n->addKey("CDP-87A");
      n->addKey("CDP-87B");
      n->addKey("CDP-87C");
      n->addKey("CDP-87D");
      n->addKey("CDP-87E");
      n->addKey("CDP-87F");
      n->addKey("CDP-87G");
      n->addKey("CDP-87H");
      n->addKey("CDP-87K");
      n->addKey("CDP-87L");
      n->addKey("CDP-87M");
      n->addKey("CDP-87R");
      n->addKey("CDP-87S");
      n->addKey("CDP-87T");
      n->addKey("CDP-87U");
      n->addKey("CDP-87V");
      n->addKey("CDP-87W");
      n->addKey("CDP-87X");
      n->addKey("CDP-87Y");
      n->addKey("CDP-87Z");
      // 88:
      n->addKey("CDP-88A");
      n->addKey("CDP-88B");
      n->addKey("CDP-88C");
      n->addKey("CDP-88D"); // "CDP-88D1", "CDP-88D2"
      n->addKey("CDP-88E"); // "CDP-88E1", "CDP-88E2"
      n->addKey("CDP-88H"); // "CDP-88H1", "CDP-88H2"
      n->addKey("CDP-88I");
      n->addKey("CDP-88R"); // "CDP-88R1", "CDP-88R2"
      n->addKey("CDP-88S"); // "CDP-88S1", "CDP-88S2"
      n->addKey("CDP-88T"); // "CDP-88T1", "CDP-88T2"
      // 89:
      n->addKey("CDP-89A"); // "CDP-89A1", "CDP-89A2"
      n->addKey("CDP-89B"); // "CDP-89B1"
      n->addKey("CDP-89C"); // "CDP-89C1", "CDP-89C2"
      n->addKey("CDP-89D"); // "CDP-89D1", "CDP-89D2"
      n->addKey("CDP-89E"); // "CDP-89E1", "CDP-89E2"
      n->addKey("CDP-89F"); // "CDP-89F1", "CDP-89F2"
      n->addKey("CDP-89I"); // "CDP-89I1", "CDP-89I2"
      n->addKey("CDP-89J"); // "CDP-89J1", "CDP-89J2"
      n->addKey("CDP-89K"); // "CDP-89K1", "CDP-89K2"
      n->addKey("CDP-89Q"); // "CDP-89Q1", "CDP-89Q2", "CDP-89Q3A", "CDP-89Q3B", "CDP-89Q[4-6][A-B]"
      // 90:
      n->addKey("CDP-90A"); // "CDP-90A1", "CDP-90A2"
      n->addKey("CDP-90B"); // "CDP-90B1", "CDP-90B2"
      n->addKey("CDP-90C"); // "CDP-90C1", "CDP-90C2"
      n->addKey("CDP-90D"); // "CDP-90D1", "CDP-90D2"
      n->addKey("CDP-90E"); // "CDP-90E1", "CDP-90E2"
      n->addKey("CDP-90F"); // "CDP-90F1", "CDP-90F2"
      n->addKey("CDP-90M"); // "CDP-90M1", "CDP-90M2"
      n->addKey("CDP-90Q"); // "CDP-90Q1", "CDP-90Q2"
      // 91:
      n->addKey("CDP-91A"); // "CDP-91A1", "CDP-91A2"
      n->addKey("CDP-91B"); // "CDP-91B1", "CDP-91B2"
      n->addKey("CDP-91C"); // "CDP-91C1", "CDP-91C2"
      n->addKey("CDP-91D"); // "CDP-91D1", "CDP-91D2"

      


      //
      // according to the URL <ftp://cddisa.gsfc.nasa.gov/pub/vlbigsfc/annual91/text.91.Z>:
      // CRUSTAL DYNAMICS PROJECT DATA ANALYSIS - 1991:
      // 
      // "Western Canada, sessions utilizing mobile units in western Canada to establish 
      // a grid of fiducial points and measure the internal stability of the North American Plate."
      Networks.inSort((n = new Network("W. Canada", "W. Canada Experiments")));
      n->addKey("CDP-90L");
      n->addKey("YELLOW");
      

      //
      // according to the URL <ftp://cddisa.gsfc.nasa.gov/pub/vlbigsfc/annual91/text.91.Z>:
      // CRUSTAL DYNAMICS PROJECT DATA ANALYSIS - 1991:
      // 
      // "Western U.S., mobile sessions sponsored by the CDP to measure deformation across 
      // the Basin and Range Province and in the boundary zone between the North American 
      // and Pacific plates."
      Networks.inSort((n = new Network("Western US", "Western US Experiments")));
      n->addKey("CDP-85J");
      n->addKey("CDP-85K");
      n->addKey("CDP-85L");
      n->addKey("CDP-85N");
      n->addKey("CDP-86D");
      n->addKey("CDP-86E");
      n->addKey("CDP-86F");
      n->addKey("CDP-87I");
      n->addKey("CDP-87J");
      n->addKey("CDP-88F");
      n->addKey("CDP-88G"); // "CDP-88G1", "CDP-88G2"
      n->addKey("CDP-88O"); // "CDP-88O1", "CDP-88O2"
      n->addKey("CDP-88P"); // "CDP-88P1", "CDP-88P2"
      n->addKey("CDP-88Q"); // "CDP-88Q1", "CDP-88Q2"
      n->addKey("CDP-89G"); // "CDP-89G1", "CDP-89G2"
      n->addKey("CDP-89H"); // "CDP-89H1", "CDP-89H2"
      n->addKey("CDP-90N"); // "CDP-90N1", "CDP-90N2"
      n->addKey("CDP-90O"); // "CDP-90O1", "CDP-90O2"
      n->addKey("CDP-90P"); // "CDP-90P1", "CDP-90P2"
      n->addKey("GSFCB");



      // undescribed:
      Networks.inSort((n = new Network("DSN", "JPL DSN Network")));
      n->addKey("dsn ");
      n->addKey("dsn-");
      n->addKey("net-p");
      n->addKey("net-r");
      n->addKey("user-defined skeleton database"); // <grin>

      //
      // according to the URL <http://ivscc.gsfc.nasa.gov/program/descrip2003.html>:
      //
      // The primary purpose of the APSG sessions is to continue monitoring the relative 
      // motions of the plates in the Asia-Pacific region. These sessions involve sites on 
      // the four plates. Tsukuba and Fairbanks are on the North American Plate, Hobart is 
      // on the Australian Plate, Seshan and Urumqi are on the Eurasian Plate, and Kokee is 
      // on the Pacific Plate. Thus, these sessions will produce critical data for the investigation 
      // of the current crustal motions in the Asia-Pacific region combined with the data from 
      // the APSG SLR and GPS campaigns which will also be performed in the same month as the APSG 
      // experiments. Also, these experiments will improve the accuracy of the geocentric coordinates 
      // of the participating stations, especially Urumqi and Tuskuba.
      Networks.inSort((n = new Network("APSG", "Asia-Pacific Network")));
      n->addKey("apsg-");

      //
      // according to the URL <ftp://cddisa.gsfc.nasa.gov/pub/vlbigsfc/annual93/text.93.Z>:
      // DATA ANALYSIS BY THE NASA SPACE GEODESY PROGRAM--GSFC:
      // 
      // "Transasia, SGP sessions using fixed stations in Europe, Africa, Australia, and
      // Asia."
      Networks.inSort((n = new Network("ASIA", "TransAsia Network")));
      n->addKey("xasia");
      n->addKey("c-asia");
      n->addKey("casia");
      n->addKey("trns-asia");

      //
      // according to the URL <http://ivscc.gsfc.nasa.gov/program/descrip2003.html>:
      //
      // The purpose of this experiment is to determine the station coordinates and their 
      // evolution in the European geodetic VLBI network. This process will be done with the 
      // highest precision possible.
      Networks.inSort((n = new Network("EUROPE", "Europe Network")));
      n->addKey("europe");
      n->addKey("ONSALA-2");


      //
      // according to the URL <http://ivscc.gsfc.nasa.gov/program/descrip2003.html>:
      //
      // The purpose of the IVS-OHIG (Southern Terrestrial Reference Frame) sessions is 
      // to tie together optimally the sites in the southern hemisphere. The participating 
      // sites include all the sites in the south - O'Higgins, Fortaleza, Hartebeesthoek, 
      // Hobart, TIGO - as well as Kokee Park. The last two sites are the southern-most of 
      // the northern sites and have been included to make the geometry more robust and to
      // increase the number of observations per scan. Syowa will participate when the station 
      // is available. Because these sessions concentrate on the southern sites and do not use 
      // observing time to accurately tie these sites to far northern sites (as the Global 
      // Sessions do) they should yield a very accurate regional TRF around the South Pole. 
      // Syowa (Japan/Antarctica) are willing to participate in this experiment. The characteristics 
      // of this network have changed slightly due to more weigh of the southern part.
      Networks.inSort((n = new Network("IVS-OHIG", "IVS-OHIG Network")));
      n->addKey("ivs-ohig");

      // undescribed:
      Networks.inSort((n = new Network("SOUTH", "SOUTH Network")));
      n->addKey("south");

      //
      // according to the URL <http://ivscc.gsfc.nasa.gov/program/descrip2003.html>:
      //
      // These experiments were designed to strengthen the reference frame in the southern 
      // hemisphere, and to detect plate motion, in close collaboration with the JARE (Japanese 
      // Antarctic Research Expedition) project.
      Networks.inSort((n = new Network("SYOWA", "SYOWA Network")));
      n->addKey("syowa");


      //
      // according to the URL <ftp://cddisa.gsfc.nasa.gov/pub/vlbigsfc/annual93/text.93.Z>:
      // DATA ANALYSIS BY THE NASA SPACE GEODESY PROGRAM--GSFC:
      // 
      // "Japan CRL, sessions sponsored by the CRL to determine local ties or
      // measure plate motion between Japan and Minami-tori Shima (Marcus Island)."
      Networks.inSort((n = new Network("CRL", "CRL Network")));
      n->addKey("crl-");


      //
      // according to the URL <ftp://cddisa.gsfc.nasa.gov/pub/vlbigsfc/annual91/text.91.Z>:
      // CRUSTAL DYNAMICS PROJECT DATA ANALYSIS - 1991:
      // 
      // "Japan, sessions sponsored by the GSI utilizing fixed stations and a transportable 
      // 5-m antenna to measure regional deformation in and around Japan."
      Networks.inSort((n = new Network("Japan", "GSI Japan Network")));
      n->addKey("gsi-");
      n->addKey("JMS91");

      // undescribed:
      Networks.inSort((n = new Network("JADE", "JADE Network")));
      n->addKey("jade-");

      // undescribed:
      Networks.inSort((n = new Network("KSP", "KSP Network")));
      n->addKey("ksp-");
      n->addKey("ksp0");
      n->addKey("ksp9");



      //
      // according to the URL <ftp://cddisa.gsfc.nasa.gov/pub/vlbigsfc/annual91/text.91.Z>:
      // "CRUSTAL DYNAMICS PROJECT DATA ANALYSIS - 1991":
      //
      // "North American Plate Stability, transcontinental sessions sponsored by the CDP 
      // designed to measure the internal stability of the North American Plate."
      Networks.inSort((n = new Network("NAPS", "CDP North American Plate Stability Experiments")));
      n->addKey("CDP-90KA1");
      n->addKey("CDP-90KA2");
      n->addKey("CDP-90KB");
      n->addKey("NAPS-");


      //
      // according to the URL <ftp://cddisa.gsfc.nasa.gov/pub/vlbigsfc/annual91/text.91.Z>:
      // "CRUSTAL DYNAMICS PROJECT DATA ANALYSIS - 1991":
      //
      // NCMN (National Crustal Motion Network), NGS-sponsored sessions to establish a 
      // grid of fiducial points across the U.S."
      Networks.inSort((n = new Network("NCMN", "NGS National Crustal Motion Network Experiments")));
      n->addKey("ncmn");
      n->addKey("BERM-");
      n->addKey("CDP-86N1");
      n->addKey("SEATLE-90"); // "SEATLE-901", "SEATLE-902"




      //-------------------------------------------------------------------------------------------------------
      //
      // Experiments:
      //
      // according to the URL <ftp://cddisa.gsfc.nasa.gov/pub/vlbigsfc/annual91/text.91.Z>:
      // "CRUSTAL DYNAMICS PROJECT DATA ANALYSIS - 1991":
      //
      // "Trans-U.S., SGP sessions using fixed stations on the east and west coast of the U.S."
      Networks.inSort((n = new Network("X-US", "CDP Trans-U.S.")));
      n->addKey("x-us");
      n->addKey("gsfca");
      n->addKey("jpl83e");

      // undescribed:
      Networks.inSort((n = new Network("GRAV", "GRAV Experiments")));
      n->addKey("grav");
      
      // undescribed:
      Networks.inSort((n = new Network("JUPITER", "Jupiter Experiments")));
      n->addKey("jupiter");

      // undescribed:
      Networks.inSort((n = new Network("MARS", "Mars Experiments")));
      n->addKey("mars");

      //
      // according to the URL <ftp://cddisa.gsfc.nasa.gov/pub/vlbigsfc/annual91/text.91.Z>:
      // CRUSTAL DYNAMICS PROJECT DATA ANALYSIS - 1991:
      // 
      // "Europe mobile, mobile observing sessions carried out by the NGS for various European 
      // agencies at BREST, CARNUSTY, GRASSE, HOHENFRG, METSOHVI, and TROMSONO."
      Networks.inSort((n = new Network("MV-EUR", "Europe mobil")));
      n->addKey("BREST-");
      n->addKey("CARNSTY-");
      n->addKey("GRASSE-");
      n->addKey("HOHNFRG-");
      n->addKey("METSOVI-");
      n->addKey("TROMSO-");
      n->addKey("mveur-");


      //
      // according to the URL <ftp://cddisa.gsfc.nasa.gov/pub/vlbigsfc/annual91/text.91.Z>:
      // CRUSTAL DYNAMICS PROJECT DATA ANALYSIS - 1991:
      // 
      // Local Survey Ties, mobile sessions involving short baselines for the purpose of 
      // establishing local ties between fixed-antenna reference points and ground monuments 
      // used in other (such as satellite laser ranging or Global Positioning System) networks."
      Networks.inSort((n = new Network("Local Survey", "Local Survey Ties")));
      n->addKey("CDP-85M");
      n->addKey("CDP-86PA");
      n->addKey("FD-TIES");
      n->addKey("GORF-");
      n->addKey("HK-TIES");
      n->addKey("MO-TIES");
      n->addKey("MOJ13-TIE");


      //
      // according to the URL <ftp://cddisa.gsfc.nasa.gov/pub/vlbigsfc/annual91/text.91.Z>:
      // CRUSTAL DYNAMICS PROJECT DATA ANALYSIS - 1991:
      // 
      // "German, sessions sponsored by the Geodetic Institute of the University of Bonn, 
      // using stations in Germany, South Africa, China, and Japan."
      Networks.inSort((n = new Network("GERMAN", "German (Univ. of Bonn)")));
      n->addKey("SHAWE90A");
      n->addKey("SHAWE90B");
      

      // undescribed:
      Networks.inSort((n = new Network("POLARIZATION", "POLARIZATION Experiments")));
      n->addKey("polrzaton");

      // undescribed:
      Networks.inSort((n = new Network("RELATIV", "Relativity Experiments")));
      n->addKey("relativity");

      // undescribed:
      Networks.inSort((n = new Network("S2IMAG", "S2IMAG Experiments")));
      n->addKey("s2imag");

      // undescribed:
      Networks.inSort((n = new Network("VEGA", "Vega Experiments")));
      n->addKey("vega");

      // undescribed:
      Networks.inSort((n = new Network("VLBA", "CDP VLBA Experiments")));
      n->addKey("vla-");
      n->addKey("geo-vlba");
      n->addKey("geovlba");
      n->addKey("bb0");
      n->addKey("bf0");
      n->addKey("vlba-");
      n->addKey("TAU28");
      


      //-------------------------------------------------------------------------------------------------------
      //
      // Unknown:
      //
      // undescribed:
      Networks.inSort((n = new Network("APT", "APT Network")));
      n->addKey("apt-");



      //-------------------------------------------------------------------------------------------------------
      //
      // Ints:
      //
      //
      // according to the URL <http://ivscc.gsfc.nasa.gov/program/descrip2003.html>:
      //
      // The INT1 and INT2 sessions are 1-baseline, 1-hour "intensive" sessions to 
      // monitor UT1. These sessions are designed to provide rapid service UT1 results. 
      // Sessions for INT1 are observed four times weekly. Sessions for INT2 are currently 
      // being coordinated and specific details will be posted later.
      Networks.inSort((n = new Network("INT", "INT Network")));
      n->addKey("int ");
      n->addKey("intensive");
      n->addKey("iris int");
      n->addKey("irisa int");
      n->addKey("irisa intensive");
      n->addKey("in102");
      n->addKey("in103");
      n->addKey("in202");
      n->addKey("int0");
      n->addKey("int8");
      n->addKey("int9");
      n->addKey("tswz");

      // undescribed:
      Networks.inSort((n = new Network("NAVINT", "NAVINT Network")));
      n->addKey("navint");




      //-------------------------------------------------------------------------------------------------------
      //
      // Uniques:
      //
      // undescribed:
      Networks.inSort((n = new Network("U-TIES", "Some site ties")));
      n->addKey("gb-ties");
      n->addKey("haw2-ties");
      n->addKey("hawai-tie");
      n->addKey("hh-45tie");
      n->addKey("hw-ties");
      n->addKey("ties");
      n->addKey("japan-tie");
      n->addKey("jpn-ties");
      n->addKey("k4-tie");
      n->addKey("-ties");
      n->addKey(" ties");
      n->addKey("wties");
      n->addKey("johannesburg-dss45 ");

      // undescribed:
      Networks.inSort((n = new Network("UNIQUE", "Unique station experiment")));
      n->addKey("azores");
      n->addKey("brm87a");      
      n->addKey("bermuda");
      n->addKey("crimea");
      n->addKey("daito");
      n->addKey("hawaii");
      n->addKey("hofn");
      n->addKey("hd-survey");
      n->addKey("hohen");
      n->addKey("jeg");
      n->addKey("kash-nobey");
      n->addKey("kirsberg");
      n->addKey("marcus");
      n->addKey("mrcs");
      n->addKey("nj2");
      n->addKey("onsala-tie");
      n->addKey("onsl-wetzl");
      n->addKey("quake");
      n->addKey("sesh");
      n->addKey("seattle");
      n->addKey("seshan");
      n->addKey("sde");
      n->addKey("suwon");
      n->addKey("tau28");
      n->addKey("tja13");
      n->addKey("toulouse");
      n->addKey("ussr");
      n->addKey("ussurisk");
      n->addKey("ussuriisk");
      n->addKey("wesk");
      n->addKey("yellow");
    };
};

/**A destructor. */
SBVLBISet::~SBVLBISet()
{
  if (SessionByName)
    {
      delete SessionByName;
      SessionByName = NULL;
    };
  if (Master)
    {
      delete Master;
      Master = NULL;
    };
};

/**Updates dictionary.
 * 
 */
void SBVLBISet::fillDicts()
{
  if (SessionByName) delete SessionByName;
  SessionByName = new QDict<SBVLBISesInfo>(count()+5);
  SessionByName-> setAutoDelete(FALSE);

  SBVLBISesInfo *si=NULL;
  for (si=first(); si; si=next())
    {
      TotalNumObs+=si->numObs();
      if (SessionByName->find((const char*)si->name()))
	Log->write(SBLog::ERR, SBLog::DATA, ClassName() + ": got a duplicate entry for session " +
		   si->name());
      else 
	SessionByName->insert((const char*)si->name(), si);
    };
};

/**Load Master File info.
 * 
 */
void SBVLBISet::loadMaster()
{
  QFile		f(SetUp->masterFileName());
  QDataStream	s;

  if (f.exists())
    {
      if (Master) delete Master;
      Master = new SBMaster;
      f.open(IO_ReadOnly);
      s.setDevice(&f);
      s >> *Master;
      f.close();
      s.unsetDevice();
    };
};

/**Makes import session from the NGS or MkIII DBH file.
 * \param FName_ file name.
 * \param isNeedDump controls dump;
 */
SBVLBISesInfo* SBVLBISet::import(SBVLBISession::ObsImportedFrom IType, 
				 const QString& FName, bool isNeedDump, int ExistedSessMode)
{
  const char* sImport[]={"NGS", "VBD", "MkIII DBH", "Unknown"};
  SBVLBISession		*W = new SBVLBISession;
  SBVLBISesInfo		*w;
  QTextStream		*dump=NULL;
  QDataStream		*s;
  QFile			f, df;
  QFileInfo		finfo(FName);
  FILE			*pipe=NULL, *pipe_4dump=NULL;
  bool			IsGotNetID;

  QString Q=finfo.isRelative()?SetUp->path2Compil() + "/" + FName:FName;

  if (isNeedDump)
    {
      pipe_4dump = SetUp->fcList()->
	open4OutDflt(SetUp->path2Output() + "/" + finfo.baseName() + ".dump" +
		     SetUp->fcList()->defaultExt(), df, *(dump=new QTextStream));
      W->setDump(dump);
    };
  
  switch (IType)
    {
    case SBVLBISession::IF_NGS:
      {
	QTextStream  *t;
	pipe = SetUp->fcList()->open4In(Q, f, *(t = new SBTS_ngs));
	(SBTS_ngs&)*t >> *W;
	SetUp->fcList()->close(f, pipe, *t);
	delete t;
      };
      break;
    case SBVLBISession::IF_DBH:
      {
	pipe=SetUp->fcList()->open4In(Q, f, *(s = new SBDS_dbh));
	s->setByteOrder(QDataStream::BigEndian); //explicit set of byte order
	(SBDS_dbh&)*s >> *W;
	SetUp->fcList()->close(f, pipe, *s);
	delete s;
      };
      break;
    case SBVLBISession::IF_VBD:
      Log->write(SBLog::WRN, SBLog::IO | SBLog::DATA, ClassName() + 
		 ": sorry, the VBD database importing not implement yet.");
      break;
    default:
    case SBVLBISession::IF_UNKN:
      break;
    };

  if (isNeedDump)
    {
      W->setDump(NULL);
      SetUp->fcList()->close(df, pipe_4dump, *dump);
      delete dump;
    };

  if (!W->count())
    {
      delete W;
      return NULL;
    };
  
  W->addHistoryLine("data imported from the " + QString(sImport[IType])
		    + " file \"" + QString(finfo.filePath()) + "\"");

  IsGotNetID = FALSE;
  SBMasterRecord* rec = NULL;
  if ( Master && (rec=Master->lookupMR(W->name())) )
    {
      W->setNetworkID(Networks.lookup(rec->sessionName()));
      W->setOfficialName(rec->sessionName());
      W->setCode(rec->sessionCode());
      W->setCorrelatorName(rec->correlatorName());
      Log->write(SBLog::DBG, SBLog::IO | SBLog::DATA, ClassName() + ": NetID look up: key [" + 
		 rec->sessionName() + "], returned ID: [" + W->networkID() + "]");
    };
  if (W->networkID()=="UNDEF") // last resort
    W->setNetworkID(Networks.lookup(W->expDescr()));

  static const char* sExclSessions[] =
  {
    "84FEB13X", "84JUL22XA","84JUL27XI","85DEC10X", "86JUL16X", "86SEP17X", "87JUN23X", "87JUL28X",
    "87AUG03XN","87AUG11X", "87AUG15XN","87SEP16X", "87DEC19XI","88FEB25XO","88MAR24XO","88APR27XN",
    "88JUL16XI","88AUG12XO","88SEP28XO","88OCT06XO","88OCT13XP","88OCT30XO","88DEC15XN","88DEC16XN",
    "89JAN05XO","89JAN26XN","89APR03XO","89JUL24XY","89AUG12X", "89DEC13XO","90JAN26X", "90JAN31XO",
    "90JUN30XY","90SEP05X", "90DEC20X", "91JAN06X", "91JUL04XX","91SEP08X", "91DEC01X", "92JAN13XL",
    "92JAN14X", "92FEB13X", "92APR08X", "92MAY12XA","92MAY25XM","92JUN30XO","92JUL07X", "92JUL16X",
    "92NOV03X", "92DEC01X", "93FEB16X", "93APR27XA","93JUN24XX","93AUG18XA","93DEC08XN","93DEC11X",
    "94FEB14XR","94APR27XA","94JUN29X", "94AUG31XA","94OCT26XA","94DEC28XA","95FEB01XC","95APR12XA",
    "95JUN08XA","95AUG31XA","95NOV09XA","95DEC06XA","96FEB07XA","96MAR11XC","96APR25XA","96JUN12XA",
    "97JAN29XA","97MAR17XA","97JUN16XA","97AUG25XA","97OCT30XA","97DEC08XA","98FEB02XA","98APR20XB",
    "98AUG17XA","98OCT12XA","98DEC14XA","99FEB01XA","99APR26XA","99JUN28XA","99AUG16XA","99OCT11XA",
    "99DEC13XA","00JAN27XA","00FEB07XA","00MAR16XA","00MAY15XA","00AUG07XA","00SEP04XA","00DEC07XA"
  };
  
  for (unsigned int i = 0; i< sizeof(sExclSessions)/sizeof(const char*); i++)
    {
      if (W->name().contains(sExclSessions[i]))
	{
	  W->setUserFlag("Q");
	  W->addHistoryLine("User flag has been set up acording to the file `exclude.html'");
	};
    };
  
  if ( (w=SessionByName->find(W->name())) )
    {
      if (ExistedSessMode==1)
	{
	  delete W;
	  return NULL;
	}
      else if (!(ExistedSessMode==-1) &&
	       QMessageBox::warning(0, QString(sImport[IType]) + " importing", 
				    "The session [" + W->name() + "] already exists in the database.\n" +
				    (QString)(w->version()==W->version()?
					      (QString)("Moreover, they have the same version.\n"):
					      "Versions are differ: " + QString().setNum(w->version()) + 
					      " for the database session and " + QString().setNum(W->version()) + 
					      " for the imported one.\n") +
				    "\nAre you going to update the database with this file?\n",
				    "Yes", "No"))
	{
	  delete W;
	  return NULL;
	};
    };

  W->calcTMean();
  W->checkArtMeteo();
  W->checkPath();
  f.setName (W->fileName(SBVLBISesInfo::FT_DAT));
  f.open(IO_WriteOnly);
  s = new SBDS_dat;
  s->setDevice(&f);
  (SBDS_dat&)*s << *W;
  f.close();
  s->unsetDevice();
  delete s;
  
  f.setName (W->fileName(SBVLBISesInfo::FT_AUX));
  f.open(IO_WriteOnly);
  s = new SBDS_aux;
  s->setDevice(&f);
  (SBDS_aux&)*s << *W;
  f.close();
  s->unsetDevice();
  delete s;

  if (w) *w=*W;
  else
    {
      inSort(w=new SBVLBISesInfo(*W));
      SessionByName->insert((const char*)w->name(), w);
      TotalNumObs+=W->numObs();
    };
  delete W;
  return w;
};

/**Loads session.
 * THESE STUPIDNESS HAVE TO BE OVERWRITEN.
 * \param Name_  session ID.
 * \param W points to session;
 * \return loaded session.
 */
SBVLBISession* SBVLBISet::loadSession(const QString& Name_, SBVLBISession* W) const
{
  bool hasCreatedW=FALSE;
  if (!W)
    {
      W=new SBVLBISession;
      hasCreatedW=TRUE;
      //Log->write(SBLog::ERR, SBLog::DATA, ClassName() + ": can't load NULL session");
      //return NULL;
    };
  
  SBVLBISesInfo	*w = SessionByName->find(Name_);
  if (!w)
    {
      Log->write(SBLog::ERR, SBLog::DATA, ClassName() + ": can't load session [" 
		 + Name_ + "]; missed entry");
      if (hasCreatedW) delete W;
      return NULL;
    };
  if (!QFile::exists(w->fileName(SBVLBISesInfo::FT_DAT)))
    {
      Log->write(SBLog::ERR, SBLog::DATA, ClassName() + ": can't load session [" 
		 + Name_ + "]; missed .dat file");
      if (hasCreatedW) delete W;
      return NULL;
    };
  if (!QFile::exists(w->fileName(SBVLBISesInfo::FT_AUX)))
    {
      Log->write(SBLog::ERR, SBLog::DATA, ClassName() + ": can't load session [" 
		 + Name_ + "]; missed .aux file");
      if (hasCreatedW) delete W;
      return NULL;
    };

  QDataStream	*d;
  QFile		f(w->fileName(SBVLBISesInfo::FT_DAT));
  f.open(IO_ReadOnly);
  d = new SBDS_dat;
  d->setDevice(&f);
  *(SBDS_dat*)d >> *W;
  f.close();
  d->unsetDevice();
  delete d;
  
  f.setName (w->fileName(SBVLBISesInfo::FT_AUX));
  f.open(IO_ReadOnly);
  d = new SBDS_aux;
  d->setDevice(&f);
  *(SBDS_aux*)d >> *W;
  f.close();
  d->unsetDevice();
  delete d;

  // exchange info:
  W->setDelayRMS2 (w->delayRMS2 ());
  W->setRateRMS2  (w->rateRMS2  ());
  W->setDelaySumW2(w->DelaySumW2 );
  W->setRateSumW2 (w->RateSumW2 );

  return W;
};

/**Saves session.
 * \param Name_  session ID.
 * \param W session to save;
 * \return saved session.
 * 
 */
//SBVLBISession* SBVLBISet::saveSession(const QString& Name_, SBVLBISession* W) const
SBVLBISession* SBVLBISet::saveSession(SBVLBISession* W) const
{
  //if (!W) W = new SBVLBISession;
  if (!W)
    {
      Log->write(SBLog::ERR, SBLog::DATA, ClassName() + ": nothing to save (ARG==NULL)");
      return NULL;
    };

  SBVLBISesInfo	*w = SessionByName->find(W->name());
  if (!w)
    {
      Log->write(SBLog::ERR, SBLog::DATA, ClassName() + ": can't save session [" 
		 + W->name() + "]; missed entry");
      return NULL;
    };

  // we should not modify the `-dat' file, so
  //  QDataStream	*d;
  //  QFile		f(w->fileName(SBVLBISesInfo::FT_DAT));
  //  f.open(IO_WriteOnly);
  //  d = new SBDS_dat;
  //  d->setDevice(&f);
  //  *(SBDS_dat*)d << *W;
  //  f.close();
  //  d->unsetDevice();
  //  delete d;

  // exchange info:
  W->calcTMean();
  w->setTStart(W->tStart());
  w->setTFinis(W->tFinis());
  w->setTMean (W->tMean());

  w->setDelayRMS2(W->delayRMS2());
  w->setRateRMS2 (W->rateRMS2 ());
  w->setDelaySumW2(W->DelaySumW2);
  w->setRateSumW2 (W->RateSumW2 );

  QFile f(w->fileName(SBVLBISesInfo::FT_AUX));
  f.open(IO_WriteOnly);
  QDataStream *d = new SBDS_aux;
  d->setDevice(&f);
  *(SBDS_aux*)d << *W;
  f.close();
  d->unsetDevice();
  delete d;
  return W;
};

/**Deletes session from the database.
 * \param Name_ session ID.
 *
 * It also removes data files (.dat \& .aux) from the disk.
 */
void SBVLBISet::removeSession(const QString& Name_)
{
  QFile f;
  SBVLBISesInfo	*w = SessionByName->find(Name_);
  if (!w)
    Log->write(SBLog::ERR, SBLog::DATA, ClassName() + ": can't remove session [" 
	       + Name_ + "]; missed entry");
  else
    {
      if (QFile::exists(w->fileName(SBVLBISesInfo::FT_DAT)))
	{
	  f.setName(w->fileName(SBVLBISesInfo::FT_DAT));
	  if (!f.remove())
	    Log->write(SBLog::ERR, SBLog::DATA, ClassName() + ": can't remove \"" 
		       + f.name() + "\" file");
	};
      if (QFile::exists(w->fileName(SBVLBISesInfo::FT_AUX)))
	{
	  f.setName(w->fileName(SBVLBISesInfo::FT_AUX));
	  if (!f.remove())
	    Log->write(SBLog::ERR, SBLog::DATA, ClassName() + ": can't remove \"" 
		       + f.name() + "\" file");
	};
      SessionByName->remove((const char*)w->name());
      remove(w);
      Log->write(SBLog::INF, SBLog::DATA, ClassName() + ": session [" 
		 + Name_ + "] has been removed");
    };
};



//void SBVLBISet::dumpUserInfo(void(*hook)(int))
void SBVLBISet::dumpUserInfo()
{
  SBVLBISession	*Session=NULL;
  int		i=0;
  int		n=count();

  mainMessage("Dumping User Info");
  mainPrBarSetTotalSteps(n);
  mainSetOperation("Processing");
  for (SBVLBISesInfo *si=first(); si; si=next())
    {
      if ( (Session = loadSession(si->name())) )
	{
	  mainSetSessName(Session->name());
	  Session->dumpUserInfo(SetUp->path2UIDump());
	  mainPrBarSetProgress(i++);
	  mainSetProcSes(i, n);
	  qApp->processEvents();
	  //	  (*hook)(i++);
	  delete Session;
	};
    };
  mainSetOperation("Waiting");
  mainSetProcSes(0, 0);
  mainPrBarReset();
  mainMessage("Ready");
  mainSetSessName("<none>");
};

void SBVLBISet::restoreUserInfo()
{
  int		i=0;
  int		n=count();
  QDir		dir(SetUp->path2UIDump(), "*", QDir::Time, QDir::Files | QDir::Readable);
  SBVLBISession	*Session=NULL;

  mainMessage("Restoring User Info");
  mainSetOperation("Processing");
  mainPrBarSetTotalSteps(n);
  for (SBVLBISesInfo *si=first(); si; si=next())
    {
      mainSetSessName(si->name());
      dir.setNameFilter(si->name() + ".*");
      if (dir.count() && (Session = loadSession(si->name())) )
	{
	  if (Session->restoreUserInfo(SetUp->path2UIDump()))
	    saveSession(Session);
	  delete Session;
	};
      mainPrBarSetProgress(i++);
      mainSetProcSes(i, n);
      qApp->processEvents();
    };
  mainSetOperation("Waiting");
  mainSetProcSes(0, 0);
  mainPrBarReset();
  mainMessage("Ready");
  mainSetSessName("<none>");
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBVLBISet's friends implementation							*/
/*												*/
/*==============================================================================================*/
/**\relates SBVLBISet
 * Saves data to the data stream.
 */
QDataStream &operator<<(QDataStream& s, const SBVLBISet& S)
{
  s << (const SBVLBISesInfoList&)S << S.Networks;
  return s;
};

/**\relates SBVLBISet
 * Loads data from the data stream.
 */
QDataStream &operator>>(QDataStream& s, SBVLBISet& S)
{
  if (S.Networks.count()) S.Networks.clear();
  s >>(SBVLBISesInfoList&)S >> S.Networks;
  S.fillDicts();
  return s;
};
/*==============================================================================================*/



/*==============================================================================================*/
