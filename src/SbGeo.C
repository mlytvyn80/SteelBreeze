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

#include "SbGeo.H"


SBInstitutionList	*Institutions;


/*==============================================================================================*/
/*												*/
/* class SBDated implementation									*/
/*												*/
/*==============================================================================================*/
QString SBDated::date2str()
{
  QString S;
  return S.sprintf("%7.1f", Date);
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBInstitutionList implementation							*/
/*												*/
/*==============================================================================================*/
SBInstitutionList::SBInstitutionList() 
  : SBNList() 
{
  setAutoDelete(TRUE);
  InstByAcronym = new QDict<SBInstitution>(60);
  InstByAcronym->setAutoDelete(FALSE);
  IsChanged = FALSE;
};

SBInstitutionList::~SBInstitutionList()
{
  if (InstByAcronym) delete InstByAcronym;
};

SBInstitutionList& SBInstitutionList::operator=(const SBInstitutionList& L)
{
  clear();
  if (InstByAcronym) delete InstByAcronym;
  InstByAcronym = new QDict<SBInstitution>(L.InstByAcronym->count());
  InstByAcronym->setAutoDelete(FALSE);
  SBNIterator it(L);
  while(it.current())
    {
      insert(it.current()->name(), ((SBInstitution*)it.current())->descr());
      ++it;
    };
  IsChanged = TRUE;
  return *this;
};

bool SBInstitutionList::insert(const QString& A_, const QString& D_)
{
  SBInstitution *W=new SBInstitution(A_, D_); 
  if (!find(A_))
    {
      inSort(W);
      InstByAcronym->insert((const char*)W->name(), W);
    }
  else
    {
      Log->write(SBLog::ERR, SBLog::IO | SBLog::CONFIG, ClassName() +
		 ": duplicated acronym \"" + W->name() + "\"; entry ignored");
      delete W;
      return FALSE;
    };
  return (IsChanged = TRUE);
};

bool SBInstitutionList::insert(const SBInstitution* W)
{
  if (!find(W->name()))
    {
      inSort(W);
      InstByAcronym->insert((const char*)W->name(), W);
    }
  else
    {
      Log->write(SBLog::ERR, SBLog::IO | SBLog::CONFIG, ClassName() +
		 ": duplicated acronym \"" + W->name() + "\"; entry ignored");
      return FALSE;
    };
  return (IsChanged = TRUE);
};

bool SBInstitutionList::remove(const SBInstitution* W)
{
  InstByAcronym->remove((const char*)W->name());
  return (IsChanged = SBNList::remove(W));
};

bool SBInstitutionList::remove(const QString& A)
{
  SBInstitution *W = find(A);
  if (!W) return FALSE;
  InstByAcronym->remove((const char*)W->name());
  return (IsChanged = SBNList::remove(W));
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBInstitutionList's friends implementation						*/
/*												*/
/*==============================================================================================*/
QDataStream &operator<<(QDataStream& s, const SBInstitutionList& L)
{
  return s << (const SBNList&)L;
};

QDataStream &operator>>(QDataStream& s, SBInstitutionList& L)
{
  L.clear();
  s >> (SBNList&)L;
  if (L.InstByAcronym) delete L.InstByAcronym;
  L.InstByAcronym = new QDict<SBInstitution>(L.count() + 5);
  L.InstByAcronym->setAutoDelete(FALSE);
  for (SBInstitution *W=(SBInstitution*)L.first(); W; W=(SBInstitution*)L.next())
    if (!L.InstByAcronym->find(W->name())) 
      L.InstByAcronym->insert((const char*)W->name(), W);
    else
      Log->write(SBLog::INF, SBLog::IO | SBLog::CONFIG, L.ClassName() +
		 ": duplicated acronym \"" + W->name() + "\"; entry ignored");
  L.IsChanged = FALSE; //
  return s;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBSolutionName implementation								*/
/*												*/
/*==============================================================================================*/
const QString SBSolutionName::TypeNames[] = 
{
  "Earth Orientation Parameters",
  "Sets of Station Coordinates",
  "Sets of Station Velocities",
  "Radio Source Coordinates",
  "Atmospheric Angular Momentum",
  "unknown"
};

const QString SBSolutionName::Types[] = 
{"EOP", "SSC", "SSV", "RSC", "AAM", "???"};

const QString SBSolutionName::TechNames[] = 
{
  "Very Long Baseline Interferometry",
  "Optical Astrometry",
  "Lunar Laser Ranging",
  "Global Positioning System",
  "Satellite Laser Ranging",
  "Doppler Orbit determination and Radiopositioning Intergrated on Satellite",
  "Combination of techniques",
  "Conventional Geodesy (for SSC)",
  "Atmospheric data",
  "TIE (I havn't found what it means)",
  "unknown"
};

const QString SBSolutionName::TechAbbrev[] = 
{ "VLBI", "OPTA", "LLR", "GPS", "SLR", "DORIS", "COMB", "CGEOD", "ATM", "TIE", "UNKN"};

const QString SBSolutionName::Techs[] = 
{"R","A","M","P","L","D","C","G","*","T","?"};


QString SBSolutionName::toString() const
{
  QString str;
  str.sprintf("%s(%s)%02d %s %02d",
	      (const char*)Types[Type], (const char*)Acronym, Year<2000?Year-1900:Year-2000, 
	      (const char*)Techs[Tech], Serial);
  return str;
};

void SBSolutionName::fromString(const QString& str)
{
  QTextStream ts((QString &)str, IO_ReadOnly);
  ts >> *this;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBSolutionName friend's implementation							*/
/*												*/
/*==============================================================================================*/
QDataStream &operator<<(QDataStream& s, const SBSolutionName& Sol)
{
  return s <<(int &)Sol.Type<<(int &)Sol.Tech<<Sol.Acronym<<Sol.Year<<Sol.Serial;
};

QDataStream &operator>>(QDataStream& s, SBSolutionName& Sol)
{
  return s >>(int &)Sol.Type>>(int &)Sol.Tech>>Sol.Acronym>>Sol.Year>>Sol.Serial;
};

QTextStream &operator<<(QTextStream& s, const SBSolutionName& Sol)
{
  return s << Sol.toString();
};

QTextStream &operator>>(QTextStream& s, SBSolutionName& Sol)
{
  int		i,j;
  QString	sType, sAcronym, sYear, sTech, sSerial;
  char		c;
  char		buff[]="123456789012345678901234567890";
  
  // init Sol with faked data
  Sol.Type	= SBSolutionName::TYPE_UNKN;
  Sol.Acronym	= "?????";
  Sol.Year	= 1980;
  Sol.Tech	= TECH_UNKN;
  Sol.Serial	= 1;


  // canonical form:
  //  AAM(AAAAA)XX E YY  
  
  //  do s>>c; while (c==' '); QTextStream strips spaces
  s>>buff[0];
  s>>buff[1];
  s>>buff[2];
  buff[3]=0;
  sType = buff;
  //  do s>>c; while (c==' ');
  s>>c;
  if (c!='(')
    {
      Log->write(SBLog::WRN, SBLog::IO,
		 "SBSolutionName: Bad format of a solution label \"(\" missed");
      return s;
    };
  i = 0;
  while ( c!=')' && i<12 ) 
    {
      s>>c;
      buff[i++] = c;
    }
  if (buff[i-1]!=')')
    {
      Log->write(SBLog::WRN, SBLog::IO, 
		 "SBSolutionName: Bad format of a solution label \")\" missed");
      return s;
    };
  buff[i-1] = 0;
  sAcronym = buff;
  //  do s>>c; while (c==' ');
  s>>buff[0];
  s>>buff[1];
  buff[2] = 0;
  sYear = buff;
  //  do s>>c; while (c==' ');
  s>>buff[0];
  buff[1] = 0;
  sTech = buff;
  //  do s>>c; while (c==' ');
  s>>buff[0];
  s>>buff[1];
  buff[2] = 0;
  sSerial = buff;

  Log->write(SBLog::DBG, SBLog::IO,
	     "SBSolutionName: parsing: Type: [" + sType + "], Acronym: [" + sAcronym + 
	     "], Year: [" + sYear + "], Tech: [" + sTech + "], Serial: [" + sSerial + "]");

  // parce a type of the label:
  if (sType == "EOP") Sol.Type = SBSolutionName::TYPE_EOP;
  else if (sType == "SSC") Sol.Type = SBSolutionName::TYPE_SSC;
  else if (sType == "SSV") Sol.Type = SBSolutionName::TYPE_SSV;
  else if (sType == "RSC") Sol.Type = SBSolutionName::TYPE_RSC;
  else if (sType == "AAM") Sol.Type = SBSolutionName::TYPE_AAM;
  else 
    Log->write(SBLog::WRN, SBLog::IO, "SBSolutionName: Wrong format: unknown type: \""  + sType + 
	       "\", forced to UNKN");
  // parce an acronym:
  if (sAcronym.length()<3)
    Log->write(SBLog::WRN, SBLog::IO,
	       "SBSolutionName: Wrong format: length of the acronym less then 3: \""
	       + sAcronym + "\"");
  else 
    {
      if (sAcronym.length()>5)
	{
	  Log->write(SBLog::WRN, SBLog::IO,
		     "SBSolutionName: Wrong format: length of the acronym greater then 5: \"" 
		     + sAcronym + "\"");
	  sAcronym = sAcronym.left(5);
	  Log->write(SBLog::WRN, SBLog::IO, "SBSolutionName: Wrong format: truncated to: \"" 
		     + sAcronym + "\"");
	};
      
      SBInstitution *I = Institutions->find(sAcronym);
      if (!I)
	{
	  Log->write(SBLog::WRN, SBLog::IO, 
		     "SBSolutionName: Wrong format: unknown organization: \"" + sAcronym + "\"");
	  I = Institutions->find(sAcronym.left(sAcronym.length() - 1));
	  if (I)
	    {
	      sAcronym = sAcronym.left(sAcronym.length() - 1);
	      Log->write(SBLog::WRN, SBLog::IO, "SBSolutionName: organization lookup: \"" + 
			 sAcronym + "\": " + I->descr());
	    }
	  else if (sAcronym.length()>4)
	    {
	      I = Institutions->find(sAcronym.left(sAcronym.length() - 2));
	      if (I)
		{
		  sAcronym = sAcronym.left(sAcronym.length() - 2);
		  Log->write(SBLog::INF, SBLog::IO, "SBSolutionName: organization lookup: \"" + 
			     sAcronym + "\": " + I->descr());
		};
	    };
	}
      else
	Log->write(SBLog::DBG, SBLog::IO, 
		   "SBSolutionName: organization lookup: \"" + sAcronym + "\": " + I->descr());
    };
  
  Sol.Acronym = sAcronym;
    
  // parce a year:
  i = sYear.toInt((bool*)&j);
  if (!j)
    Log->write(SBLog::ERR, SBLog::IO, "SBSolutionName: Wrong format: cannot convert \"" + 
	       sYear + "\" to int, forced to 1980");
  else 
    {
      if (i>50)
	Sol.Year = 1900 + i;
      else
	Sol.Year = 2000 + i;
    };

  // parce a tech:
  if (sTech == "R") Sol.Tech = TECH_VLBI;
  else if (sTech == "A") Sol.Tech = TECH_OA;
  else if (sTech == "M") Sol.Tech = TECH_LLR;
  else if (sTech == "P") Sol.Tech = TECH_GPS;
  else if (sTech == "L") Sol.Tech = TECH_SLR;
  else if (sTech == "D") Sol.Tech = TECH_DORIS;
  else if (sTech == "C") Sol.Tech = TECH_COMBINED;
  else if (sTech == "G") Sol.Tech = TECH_CG;
  else if (sTech == "T") Sol.Tech = TECH_TIE;
  else if (sTech == "*") Sol.Tech = TECH_AD;
  else 
    Log->write(SBLog::WRN, SBLog::IO,
	       "SBSolutionName: Wrong format: unknown technique: \"" + sTech + 
	       "\", forced to UNKN");

  // parce a serial number:
  i = sSerial.toInt((bool*)&j);
  if (!j)
    Log->write(SBLog::WRN, SBLog::IO, "SBSolutionName: Wrong format: cannot convert \"" + 
	       sSerial + "\" to int, forced to 1");
  else Sol.Serial = i;
  
  return s;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBCatalog implementation								*/
/*												*/
/*==============================================================================================*/
SBCatalog::SBCatalog() : SBNList()
{
  Label.setAttr(SBSolutionName::TYPE_UNKN, "UNKN", 1970, TECH_UNKN);
  Epoch	 = TZero;
  Comment= "no comment";
  setAutoDelete(TRUE);
  Dict	 = new QDict<SBNamed>(20);
  Dict	 -> setAutoDelete(FALSE);
  Aliases= new QDict<QString>(60);
  Aliases-> setAutoDelete(TRUE);
};

SBCatalog::SBCatalog(const SBCatalog& Catalog) : SBNList(Catalog)
{
  Epoch		= Catalog.Epoch;
  Comment	= Catalog.Comment;
  Label		= Catalog.Label;
  setAutoDelete(TRUE);
  Dict	  = new QDict<SBNamed>(*(Catalog.Dict));
  Dict	  -> setAutoDelete(FALSE);
  Aliases =  new QDict<QString>(*(Catalog.Aliases));
  Aliases -> setAutoDelete(TRUE);
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBCatalog's friends implementation							*/
/*												*/
/*==============================================================================================*/
QDataStream &operator<<(QDataStream& s, const SBCatalog& C)
{
  s << (SBNList&)C << C.Label << C.Epoch << C.Comment << C.Aliases->count();
  QDictIterator<QString> it(*(C.Aliases));
  while (it.current())
    {
      s << (QString)(it.currentKey()) << *it.current();
      ++it;
    };
  return s;
};

QDataStream &operator>>(QDataStream& s, SBCatalog& C)
{      
  uint		n;
  uint		i;
  QString	sKey; 
  QString	sItem;

  s >> (SBNList&)C >> C.Label >> C.Epoch >> C.Comment >> n;
  if (C.Dict) delete C.Dict;
  C.Dict = new QDict<SBNamed>(C.count()+5);
  C.Dict -> setAutoDelete(FALSE);
  SBNIterator it(C);
  while(it.current())
    {
      C.Dict -> insert(it.current()->name(), it.current());
      ++it;
    };

  if (C.Aliases) delete C.Aliases;
  C.Aliases = new QDict<QString>(n+5);
  C.Aliases -> setAutoDelete(TRUE);
  for (i=0; i<n; i++)
    {
      s >> sKey >> sItem;
      if (sKey.length() && sItem.length())
	C.Aliases -> insert((const char*)sKey, new QString((const char*)sItem));
    };
  return s;
};  
/*==============================================================================================*/




/*==============================================================================================*/
void rad2hms(double rad, int& h, int& m, double& s)
{
  s = fmod(rad, 2.0*M_PI)*RAD2HR*3600.0; //secs
  //  s = rad*RAD2HR*3600.0; //secs
  h = (int)(s/3600.0);
  m = (int)((s - 3600.0*h)/60.0);
  s -= 3600.0*h + 60.0*m;
};

void rad2dms(double rad, int& d, int& m, double& s)
{
  s = drem(rad, 2.0*M_PI)*RAD2DEG*3600.0; //secs  
  //  s = rad*RAD2DEG*3600.0; //secs  
  d = (int)(s/3600.0);
  m = (int)((s - 3600.0*d)/60.0);
  s -= 3600.0*d + 60.0*m;
};

void rad2dmsl(double rad, int& d, int& m, double& s)
{
  s = fmod(rad, 2.0*M_PI)*RAD2DEG*3600.0; //secs  
  //  s = rad*RAD2DEG*3600.0; //secs  
  d = (int)(s/3600.0);
  m = (int)((s - 3600.0*d)/60.0);
  s -= 3600.0*d + 60.0*m;
};

QString	rad2dmsStr(double DN_)
{
  int		dn_d, dn_m;
  double	dn_s;
  QString	s;
  rad2dms(fabs(DN_), dn_d, dn_m, dn_s);
  return  s.sprintf("%c%2d %2d' %7.4f\"", DN_<0.0?'-':' ', dn_d, dn_m, dn_s);
};

QString	rad2dmslStr(double DN_)
{
  int		dn_d, dn_m;
  double	dn_s;
  QString	s;
  rad2dmsl(DN_, dn_d, dn_m, dn_s);
  return  s.sprintf("%3d %2d' %7.4f\"", dn_d, dn_m, dn_s);
};

QString	rad2hmsStr(double RA_)
{
  int		ra_h, ra_m;
  double	ra_s;
  QString	s;
  rad2hms(RA_, ra_h, ra_m, ra_s);
  return s.sprintf("%2dh %2dm %7.4fs", ra_h, ra_m, ra_s);
};
/*==============================================================================================*/

