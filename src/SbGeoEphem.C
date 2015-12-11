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

#include "SbGeoEphem.H"
#include "SbSetup.H"
#include "SbGeoProject.H"

#include <qapplication.h> 
#include <qfileinfo.h> 
#include <qregexp.h> 


const SBEphem::Entry SBEphem::LocAbstract[13] =
{
  {  0,  0,  0},  {  0,  0,  0},  {  0,  0,  0},  {  0,  0,  0},  {  0,  0,  0},  
  {  0,  0,  0},  {  0,  0,  0},  {  0,  0,  0},  {  0,  0,  0},  {  0,  0,  0},  
  {  0,  0,  0},  {  0,  0,  0},  {  0,  0,  0}
};

const SBEphemDE200::Entry SBEphemDE200::LocDE200[13] =
{
  {  3, 12,  4},	// 0, MERCURY
  {147, 12,  1},	// 1, VENUS
  {183, 15,  2},	// 2, EARTH-MOON BARYCENTER
  {273, 10,  1},	// 3, MARS
  {303,  9,  1},	// 4, JUPITER
  {330,  8,  1},	// 5, SATURN
  {354,  8,  1},	// 6, URANUS
  {378,  6,  1},	// 7, NEPTUNE
  {396,  6,  1},	// 8, PLUTO
  {414, 12,  8},	// 9, GEOCENTRIC MOON
  {702, 15,  1},	//10, SUN
  {747, 10,  4},	//11, NUTATIONS IN LONGITUDE AND OBLIQUITY
  {  0,  0,  0} 	//12, LUNAR LIBRATIONS (IF ON FILE)
};

const SBEphemDE403::Entry SBEphemDE403::LocDE403[13] =
{
  {  3, 14,  4},	// 0, MERCURY
  {171, 10,  2},	// 1, VENUS
  {231, 13,  2},	// 2, EARTH-MOON BARYCENTER
  {309, 11,  1},	// 3, MARS
  {342,  8,  1},	// 4, JUPITER
  {366,  7,  1},	// 5, SATURN
  {387,  6,  1},	// 6, URANUS
  {405,  6,  1},	// 7, NEPTUNE
  {423,  6,  1},	// 8, PLUTO
  {441, 13,  8},	// 9, GEOCENTRIC MOON
  {753, 11,  2},	//10, SUN
  {819, 10,  4},	//11, NUTATIONS IN LONGITUDE AND OBLIQUITY
  {899, 10,  4}		//12, LUNAR LIBRATIONS (IF ON FILE)
};



/*==============================================================================================*/
/*												*/
/* class SBEphem implementation									*/
/*												*/
/*==============================================================================================*/
/**A constructor.
 * Creates a copy of object.
 */
SBEphem::SBEphem(const SBConfig *Cfg_, const QString& Name_) : SBNamed(Name_),
  Mercury	("Mercury",   1.0), 
  Venus		("Venus",     1.0), 
  Earth		("Earth",     3.986004418E+14),
  Mars		("Mars",      1.0), 
  Jupiter	("Jupiter",   1.0),
  Saturn	("Saturn",    1.0), 
  Uran		("Uranus",    1.0), 
  Neptun	("Neptune",   1.0), 
  Pluto		("Pluto",     1.0), 
  Moon		("Moon",      1.0),
  Sun		("Sun",       1.327124E+20),
  GeoMoon	("Geocentric Moon",1.0),
  TStart(TZero), TFinis(TZero), TBlStart(TZero), TBlFinis(TZero)
{
  Cfg	     = Cfg_;
  Solary[ 0] = &Mercury;
  Solary[ 1] = &Venus;
  Solary[ 2] = &Earth;
  Solary[ 3] = &Mars;
  Solary[ 4] = &Jupiter;
  Solary[ 5] = &Saturn;
  Solary[ 6] = &Uran;
  Solary[ 7] = &Neptun;
  Solary[ 8] = &Pluto;
  Solary[ 9] = &Moon;
  Solary[10] = &Sun;

  for (int i=0; i<11; i++) 
    if (Cfg->isEphBody(i)) 
      {
	Solary[i]->addAttr(SBCelestBody::IsEligible);
	Solary[i]->addAttr(SBCelestBody::IsGravDelay);
      }
    else
      {	
	Solary[i]->addAttr(SBCelestBody::IsGravDelay);
	if (i!=10 && i!=2 && i!=9)
	  Solary[i]->delAttr(SBCelestBody::IsEligible);
      };
  
  NumRecs = 0;
  NRecCoef = 2;
  DT	= 1.0;
  Loc = LocAbstract;
  FileOffset = 0L;
  AU = 0.0; //<grin>
  CLight = 299792458.0;
  EMRat  = 1.0/0.0123000345;
  IsOK = FALSE;
};

SBEphem::~SBEphem()
{
  if (buff) delete []buff;
  if (isOK() && fclose(EphemFile))
    Log->write(SBLog::ERR, SBLog::EPHEM | SBLog::IO, ClassName() + ": error while closeing file `" 
	       + fileName() + "'");
};

void SBEphem::initData()
{
  buff = new double[nRecCoef()];
  Log->write(SBLog::DBG, SBLog::EPHEM, ClassName() + ": allocated " + 
	     QString("").setNum(nRecCoef()) + " bytes for buffer");
  openFile();
};

void SBEphem::openFile()
{
  QFileInfo *finfo = new QFileInfo(SetUp->path2Stuff() + fileName());
  if (finfo->size()<nRecCoef() || (finfo->size()%nRecCoef()))
    Log->write(SBLog::ERR, SBLog::EPHEM | SBLog::IO, ClassName() + ": file `" + 
	       finfo->filePath() + "' is broken");
  else
    {
      NumRecs = finfo->size()/(nRecCoef()*sizeof(double));
      EphemFile = fopen(SetUp->path2Stuff() + fileName(), "r");
      if (!EphemFile)
	Log->write(SBLog::ERR, SBLog::EPHEM | SBLog::IO, ClassName() + ": can't read file `" + 
		   fileName() + "'");
      else
	{
	  //last record
	  fseek(EphemFile, -(long)(sizeof(double)*nRecCoef()), SEEK_END);
	  fread(buff, sizeof(double), nRecCoef(), EphemFile);
	  TFinis = *(buff+1);
	  // first record
	  FileOffset = 0L;
	  fseek(EphemFile, FileOffset, SEEK_SET);
	  fread(buff, sizeof(double), nRecCoef(), EphemFile);
	  TBlStart = TStart = *buff;
	  TBlFinis = *(buff+1);
	  checkData();
	};
    };
  if (isOK())
    {
      Log->write(SBLog::WRN, SBLog::EPHEM | SBLog::IO, ClassName() + ": loaded file `" + 
		 fileName() + "'");
      Log->write(SBLog::INF, SBLog::EPHEM, ClassName() + ": " + name() + ": ready from " + 
		 TStart.toString(SBMJD::F_Short) + " to " + TFinis.toString(SBMJD::F_Short));
    };
  delete finfo;
};

void SBEphem::calc(const SBMJD& T)
{
  if (!isOK()) 
    {
      Log->write(SBLog::ERR, SBLog::EPHEM | SBLog::IO, ClassName() + 
		 ": trying to access a broken file");
      return;
    }
  if (T<TStart || TFinis<T)
    Log->write(SBLog::ERR, SBLog::EPHEM | SBLog::IO, ClassName() + ": argument [" + 
	       T.toString(SBMJD::F_Short) + "] out of ephemeride's interval: [" +
	       TStart.toString(SBMJD::F_Short) + ".." + TFinis.toString(SBMJD::F_Short) + "]");
  else
    {
      int L = calcOffset(T);
      if (L==-1)
	{
	  Log->write(SBLog::ERR, SBLog::EPHEM | SBLog::IO, ClassName() + 
		     ": error: file offset out of range (==-1)");
	  return;
	}
      else if (FileOffset!=L)
	{
	  fseek(EphemFile, nRecCoef()*sizeof(double)*(FileOffset=L), SEEK_SET);
	  fread(buff, sizeof(double), nRecCoef(), EphemFile);
	  TBlStart = *buff;
	  TBlFinis = *(buff+1);
	};
      double t1 = (T - TBlStart)/dt();
      for (int i=0; i<11; i++) 
	if (Solary[i]->isAttr(SBCelestBody::IsEligible) /*&& T!=Solary[i]->tCurrent()*/)
	  {
	    interp(t1, Loc[i].Offset, Loc[i].NCoef, Loc[i].NSets, *Solary[i]);
	    Solary[i]->setTCurrent(T);
	  };
      //      if (T!=Moon.tCurrent() || T!=Earth.tCurrent() || T!=GeoMoon.tCurrent())
      //	{
      GeoMoon.R = Moon.R;
      GeoMoon.V = Moon.V;
      GeoMoon.A = Moon.A;
      GeoMoon.setTCurrent(T);
      //Earth in SSB:
      Earth.R-= 1.0/(1.0 + emRat())*GeoMoon.R;
      Earth.V-= 1.0/(1.0 + emRat())*GeoMoon.V;
      Earth.A-= 1.0/(1.0 + emRat())*GeoMoon.A;
      //      Earth.setTCurrent(T);
      //Moon in SSB:
      Moon.R = Earth.R + GeoMoon.R;
      Moon.V = Earth.V + GeoMoon.V;
      Moon.A = Earth.A + GeoMoon.A;
      //      Moon.setTCurrent(T);
      //	};
    };
  
  // make a report:
  if (Log->isEligible(SBLog::DBG, SBLog::EPHEM))
    {
      Log->write(SBLog::DBG, SBLog::EPHEM, ClassName() + ": Arg(TAI)         : " + T.toString());
      for (int i=0; i<11; i++) 
	if (Solary[i]->isAttr(SBCelestBody::IsEligible))
	  {
	    Log->write(SBLog::DBG, SBLog::EPHEM, ClassName() + 
		       ": TCurrent         : " + Solary[i]->tCurrent().toString() +
		       ": Body: " + Solary[i]->name());
	    Solary[i]->r().report2Log(SBLog::DBG, SBLog::EPHEM, ClassName() + 
				      ": Cooridinates (km): ", 15,3, 0.001);
	    Solary[i]->v().report2Log(SBLog::DBG, SBLog::EPHEM, ClassName() + 
				      ": Velocities  (m/s): ", 15,3);
	    Solary[i]->a().report2Log(SBLog::DBG, SBLog::EPHEM, ClassName() + 
				      ": Accels   (mm/s/s): ", 15,3, 1000.0);
	  };

      Log->write(SBLog::DBG, SBLog::EPHEM, ClassName() + ": Body             : " + GeoMoon.name());
      GeoMoon.r().report2Log(SBLog::DBG, SBLog::EPHEM, ClassName() + ": Cooridinates (km): ", 15,3, 0.001);
      GeoMoon.v().report2Log(SBLog::DBG, SBLog::EPHEM, ClassName() + ": Velocities  (m/s): ", 15,3);
      GeoMoon.a().report2Log(SBLog::DBG, SBLog::EPHEM, ClassName() + ": Accels   (mm/s/s): ", 15,3, 1000.0);

    };

};

void SBEphem::calcI(int I, const SBMJD& T)
{
  if (!isOK()) 
    {
      Log->write(SBLog::ERR, SBLog::EPHEM | SBLog::IO, ClassName() + 
		 ": trying to access a broken file");
      return;
    };
  if (T<TStart || TFinis<T)
    Log->write(SBLog::ERR, SBLog::EPHEM | SBLog::IO, ClassName() + ": argument [" + 
	       T.toString(SBMJD::F_Short) + "] out of ephemeride's interval: [" +
	       TStart.toString(SBMJD::F_Short) + ".." + TFinis.toString(SBMJD::F_Short) + "]");
  if (I<0 || I>10)
    Log->write(SBLog::ERR, SBLog::EPHEM, ClassName() + ": index [" + 
	       QString().setNum(I) + "] out of ephemeride's range: [0..10]");
  else
    {
      long L = calcOffset(T);
      if (L==-1L)
	{
	  Log->write(SBLog::ERR, SBLog::EPHEM | SBLog::IO, ClassName() + 
		     ": error: file offset out of range (==-1)");
	  return;
	}
      else if (FileOffset!=L)
	{
	  fseek(EphemFile, nRecCoef()*sizeof(double)*(FileOffset=L), SEEK_SET);
	  fread(buff, sizeof(double), nRecCoef(), EphemFile);
	  TBlStart = *buff;
	  TBlFinis = *(buff+1);
	};
      double t1 = (T - TBlStart)/dt();
      //      if (T!=Solary[I]->tCurrent())
      //	{
      interp(t1, Loc[I].Offset, Loc[I].NCoef, Loc[I].NSets, *Solary[I]);
      Solary[I]->setTCurrent(T);

	  //	};
      if (I==2 || I==9) //Earth or Moon:
	{
	  if (I==2)
	    {
	      interp(t1, Loc[9].Offset, Loc[9].NCoef, Loc[9].NSets, Moon);
	      Moon.setTCurrent(T);
	    }
	  else
	    {
	      interp(t1, Loc[2].Offset, Loc[2].NCoef, Loc[2].NSets, Earth);
	      Earth.setTCurrent(T);
	    };
	  GeoMoon.R = Moon.R;
	  GeoMoon.V = Moon.V;
	  GeoMoon.A = Moon.A;
	  GeoMoon.setTCurrent(T);
	  //Earth in SSB:
	  Earth.R-= 1.0/(1.0 + emRat())*GeoMoon.R;
	  Earth.V-= 1.0/(1.0 + emRat())*GeoMoon.V;
	  Earth.A-= 1.0/(1.0 + emRat())*GeoMoon.A;

	  //Moon in SSB:
	  Moon.R = Earth.R + GeoMoon.R;
	  Moon.V = Earth.V + GeoMoon.V;
	  Moon.A = Earth.A + GeoMoon.A;
	  
	};
    };
};

void SBEphem::interp(double T, int Offset, int NCo, int NSets, SBCelestBody& B)
{
  // T is (T-T_0)/DT
  int		i,j,l;
  double	bma, bma2, tc, twot, f;
  double	pc[18], vc[18], ac[18];

  pc[0] = 1.0;
  vc[1] = 1.0;
  ac[2] = 4.0;

  bma   = 2.0*NSets/dt();
  bma2  = bma*bma;

  l     = (int)floor(T*NSets - floor(T)); //index of coeff's cluster
  tc    = 2.0* (fmod(T*NSets, 1.0) + floor(T)) - 1.0;
  twot  = 2.0*tc;

  pc[1] = tc;
  vc[2] = 2.0*twot;
  ac[3] = 24.0*pc[1];

  for (i=2; i<NCo; i++) pc[i] = twot*pc[i-1] - pc[i-2];
  for (i=3; i<NCo; i++) vc[i] = twot*vc[i-1] + 2.0*pc[i-1] - vc[i-2];
  for (i=4; i<NCo; i++) ac[i] = twot*ac[i-1] + 4.0*vc[i-1] - ac[i-2];

  for (i=0; i<3; i++)
    {
      f=0.0;
      for (j=0; j<NCo; j++)f += pc[j]**(buff + Offset-1 + j + i*NCo + l*NCo*3);
      B.R[(DIRECTION)i]=f;
      f=0.0;
      for (j=1; j<NCo; j++)f += vc[j]**(buff + Offset-1 + j + i*NCo + l*NCo*3);
      B.V[(DIRECTION)i]=f;
      f=0.0;
      for (j=2; j<NCo; j++)f += ac[j]**(buff + Offset-1 + j + i*NCo + l*NCo*3);
      B.A[(DIRECTION)i]=f;
    };
  B.R*=1000.0;
  B.V*=bma*1000.0/DAY2SEC;
  B.A*=bma2*1000.0/DAY2SEC/DAY2SEC;
};

//bad ugly code.. have to be rewritten.. latter..
bool SBEphem::import(const QString& ImpFName)
{
  bool		IsImportedOK = TRUE, IsOK=FALSE;
  SBMJD		tSt(TZero), tFi(TZero);
  unsigned int	n=0, m=0;
  QFile		file;
  QTextStream	*ts;
  FILE*		pipe=NULL;
  double	*b = new double[nRecCoef()], fOld=0.0;
  QString	Str,s;
  int		i;
  int		NumOfRec=0, Num2Write=0;
 
  
  //determine start/stop dates of the importing file, check for reading:
  mainMessage("Preprocessing: " + ImpFName + s.sprintf(" (%d records)", NumOfRec));
  pipe=SetUp->fcList()->open4In(ImpFName, file, *(ts=new QTextStream));
  while (!ts->eof() && IsImportedOK)
    {
      Str = ts->readLine().simplifyWhiteSpace();
      if ((i=Str.find(' '))!=-1)
	{
	  n = Str.left((i=Str.find(' '))).toInt(&IsOK);
	  if (!IsOK) IsImportedOK=FALSE;
	  m = Str.right(Str.length()-i-1).toInt(&IsOK);
	  if (!IsOK) IsImportedOK=FALSE;
	  if (!IsImportedOK)
	    Log->write(SBLog::ERR, SBLog::EPHEM | SBLog::IO, ClassName() + ": file `" + ImpFName
		       +"': error reading number of coefficients");
	}
      else 
	{
	  Log->write(SBLog::ERR, SBLog::EPHEM | SBLog::IO, ClassName() + ": file `" + ImpFName
		     +"': cannot get number of elements in a record");
	  IsImportedOK=FALSE;
	};
      if (m==nRecCoef() && IsImportedOK)
	{
	  memset((void*)b, 0, sizeof(double)*nRecCoef());
	  for (n=0; n<nRecCoef()-3; n+=3)
	    if (IsImportedOK)
	      {
		//well, easy and stupid solution.. hope, JPL uses fixed format for ASCII files..
		Str = ts->readLine();
		Str = Str.replace(QRegExp("D"), "E");
		*(b+n)  = Str.mid(0,  26).toDouble(&IsOK);
		if (!IsOK) IsImportedOK=FALSE;
		*(b+n+1)= Str.mid(26, 26).toDouble(&IsOK);
		if (!IsOK) IsImportedOK=FALSE;
		*(b+n+2)= Str.mid(52, 26).toDouble(&IsOK);
		if (!IsOK) IsImportedOK=FALSE;
		if (!IsImportedOK)
		  Log->write(SBLog::ERR, SBLog::EPHEM | SBLog::IO, ClassName() + ": file `" + 
			     ImpFName +"': error reading a data record");
	      };
	  if (IsImportedOK)
	    {
	      Str = ts->readLine();
	      Str = Str.replace(QRegExp("D"), "E");
	      for (m=0; m<nRecCoef()%3; m++) 
		{
		  *(b+n+m)  = Str.mid(26*m,  26).toDouble(&IsOK);
		  if (!IsOK) IsImportedOK=FALSE;
		  if (!IsImportedOK)
		    Log->write(SBLog::ERR, SBLog::EPHEM | SBLog::IO, ClassName() + ": file `"
			       + ImpFName + "': error reading last string of a data record");
		};
	    };
	  *b    -= 2400000.5; //they are in JD
	  *(b+1)-= 2400000.5; //they are in JD
	  if (tSt==TZero) tSt = (SBMJD)*b;
	  tFi = (SBMJD)*(b+1);
	  if (*(b+1)-*b!=dt())
	    {
	      Log->write(SBLog::ERR, SBLog::EPHEM | SBLog::IO, ClassName()+": file `" + ImpFName
			 + "': a record's interval not equal to the ephemeride's one");
	      IsImportedOK = FALSE;
	    };
	  if (*b!=fOld && NumOfRec)
	    {
	      Log->write(SBLog::ERR, SBLog::EPHEM | SBLog::IO, ClassName()+": file `" + ImpFName
			 + "': there is a gap in the importing data");
	      IsImportedOK = FALSE;
	    }
	  else fOld=*(b+1);
	  NumOfRec++;
	  if (!(NumOfRec%5))
	    {
	      mainMessage("Preprocessing: " + ImpFName + s.sprintf(" (%d records)", NumOfRec));
	      qApp->processEvents();
	    };
	}
      else
	{
	  Log->write(SBLog::ERR, SBLog::EPHEM | SBLog::IO, ClassName() + ": file `" + ImpFName
		     + "' contain the num of coeff that differs from ephemeride's one");
	  IsImportedOK = FALSE;
	};
    };
  SetUp->fcList()->close(file, pipe, *ts);
  delete ts;
  Num2Write = NumOfRec;

  if (!IsImportedOK)
    Log->write(SBLog::ERR, SBLog::EPHEM | SBLog::IO, ClassName() + ": file `" + ImpFName + 
	       "' seems is not a proper one");
  else
    {
      //check for continiousity
      if (isOK() && !(tFi==tStart() || tSt==tFinis()))
	{
	  if (tFi<tStart()) 
	    Str = "there is a gap in a data (TEnd(new)<TSart(present))";
	  else if (tFinis()<tSt) 
	    Str = "there is a gap in a data (TEnd(present)<TSart(new))";
	  else 
	    Str = "data overlaped";
	  Log->write(SBLog::ERR, SBLog::EPHEM | SBLog::IO, ClassName() + 
		     ": unable to import the file `" + ImpFName + "': " + Str);
	  Log->write(SBLog::DBG, SBLog::EPHEM | SBLog::IO, ClassName() + ": new [" + 
		     tSt.toString(SBMJD::F_Short) + ".." + tFi.toString(SBMJD::F_Short) + 
		     "] vs present [" + tStart().toString(SBMJD::F_Short) + ".." + 
		     tFinis().toString(SBMJD::F_Short) + "]");
	}
      else
	{
	  pipe=SetUp->fcList()->open4In(ImpFName, file, *(ts=new QTextStream));
	  FILE* tmp = tmpfile();
	  if (!tmp)
	    Log->write(SBLog::ERR, SBLog::IO, ClassName() + ": unable to create temporary file");
	  else
	    {
	      if (!isOK() || tFi==tStart())
		{
		  mainMessage("Processing: " + ImpFName);
		  mainPrBarSetTotalSteps(NumOfRec);
		  NumOfRec=0;
		  while (!ts->eof())
		    {
		      memset((void*)b, 0, sizeof(double)*nRecCoef());
		      ts->readLine();
		      if (!ts->eof())
			{
			  for (n=0; n<nRecCoef()-3; n+=3)
			    {
			      Str = ts->readLine();
			      Str = Str.replace(QRegExp("D"), "E");
			      *(b+n)  = Str.mid(0,  26).toDouble();
			      *(b+n+1)= Str.mid(26, 26).toDouble();
			      *(b+n+2)= Str.mid(52, 26).toDouble();
			    };
			  Str = ts->readLine();
			  Str = Str.replace(QRegExp("D"), "E");
			  for (m=0; m<nRecCoef()%3; m++) *(b+n+m) = Str.mid(26*m, 26).toDouble();
			  *b    -= 2400000.5; //they are in JD
			  *(b+1)-= 2400000.5; //they are in JD
			  fwrite(b, sizeof(double), nRecCoef(), tmp);
			  mainPrBarSetProgress(++NumOfRec);
			  qApp->processEvents();
			};
		    };
		  if (isOK())
		    {
		      fseek(EphemFile, 0L, SEEK_SET);
		      for (int i=0; i<numRecs(); Num2Write++, i++)
			if (!feof(EphemFile))
			  {
			    memset((void*)b, 0, sizeof(double)*nRecCoef());
			    fread(b, sizeof(double), nRecCoef(), EphemFile);
			    fwrite(b, sizeof(double), nRecCoef(), tmp);
			  };
		    };
		}
	      else
		{
		  fseek(EphemFile, 0L, SEEK_SET);
		  for (int i=0; i<numRecs(); Num2Write++, i++)
		    if (!feof(EphemFile))
		      {
			memset((void*)b, 0, sizeof(double)*nRecCoef());
			fread(b, sizeof(double), nRecCoef(), EphemFile);
			fwrite(b, sizeof(double), nRecCoef(), tmp);
		      };
		  mainMessage("Processing: " + ImpFName);
		  mainPrBarSetTotalSteps(NumOfRec);
		  NumOfRec=0;
		  while (!ts->eof())
		    {
		      memset((void*)b, 0, sizeof(double)*nRecCoef());
		      ts->readLine();
		      if (!ts->eof())
			{
			  for (n=0; n<nRecCoef()-3; n+=3)
			    {
			      Str = ts->readLine();
			      Str = Str.replace(QRegExp("D"), "E");
			      *(b+n)  = Str.mid(0,  26).toDouble();
			      *(b+n+1)= Str.mid(26, 26).toDouble();
			      *(b+n+2)= Str.mid(52, 26).toDouble();
			    };
			  Str = ts->readLine();
			  Str = Str.replace(QRegExp("D"), "E");
			  for (m=0; m<nRecCoef()%3; m++) *(b+n+m) = Str.mid(26*m, 26).toDouble();
			  *b    -= 2400000.5; //they are in JD
			  *(b+1)-= 2400000.5; //they are in JD
			  fwrite(b, sizeof(double), nRecCoef(), tmp);
			  mainPrBarSetProgress(++NumOfRec);
			  qApp->processEvents();
			};
		    };
		};
	      fseek(tmp, 0L, SEEK_SET);
	      if (isOK()) fclose(EphemFile);
	      EphemFile = fopen(SetUp->path2Stuff() + fileName(), "w");
	      fseek(EphemFile, 0L, SEEK_SET);
	      for (int i=0; i<Num2Write; i++)
		if (!feof(tmp))
		  {		  
		    memset((void*)b, 0, sizeof(double)*nRecCoef());
		    fread(b, sizeof(double), nRecCoef(), tmp);
		    fwrite(b, sizeof(double), nRecCoef(), EphemFile);
		  };
	      fclose(EphemFile);
	      //reopen
	      openFile();
	    };
	  SetUp->fcList()->close(file, pipe, *ts);
	  delete ts;
	  fclose (tmp);
	};
    };
  mainPrBarReset();
  mainMessage("");
  if (b) delete []b;
  return IsImportedOK;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBEphemDE200 implementation								*/
/*												*/
/*==============================================================================================*/
/**A constructor.
 * Creates a copy of object.
 */
SBEphemDE200::SBEphemDE200(const SBConfig *Cfg_) 
  : SBEphem(Cfg_, "JPL Planetary Ephemeris DE200/LE200") 
{
  Loc=LocDE200; 
  DT=32.0; 
  NRecCoef=826; 
  AU=0.149597870659999996E+12; //meters
  CLight = 299792457.999999984;//meters/sec
  EMRat  = 81.3005869999999931;
  double	scale = AU*AU*AU/DAY2SEC/DAY2SEC;
  Mercury.setGM(0.491254745145081187E-10*scale);
  Venus  .setGM(0.724345620963276551E-09*scale); 
  //(GM for the Earth-Moon Barycenter)/(1+1/EMRAT):
  Earth  .setGM(0.899701165855730777E-09/(1.0 + 1.0/EMRat)*scale);
  Mars   .setGM(0.954952894222405808E-10*scale);
  Jupiter.setGM(0.282534210344592645E-06*scale);
  Saturn .setGM(0.845946850483065864E-07*scale);
  Uran   .setGM(0.128881623813803495E-07*scale);
  Neptun .setGM(0.153211248128427619E-07*scale);
  Pluto  .setGM(0.227624775186369935E-11*scale);
  //(GM for the Earth-Moon Barycenter)/(1+EMRAT):
  Moon   .setGM(0.899701165855730777E-09/(1.0 + EMRat)*scale);
  GeoMoon.setGM(Moon.gm());
  Sun    .setGM(0.295912208285591095E-03*scale);
  initData();
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBEphemDE403 implementation								*/
/*												*/
/*==============================================================================================*/
/**A constructor.
 * Creates a copy of object.
 */
SBEphemDE403::SBEphemDE403(const SBConfig *Cfg_) 
  : SBEphem(Cfg_, "JPL Planetary Ephemeris DE403/LE403") 
{
  Loc=LocDE403; 
  DT=32.0; 
  NRecCoef=1018; 
  AU=0.149597870691000015E+12; //meters
  CLight = 299792457.999999984;//meters/sec
  EMRat  = 81.3005849999999981;
  double	scale = AU*AU*AU/DAY2SEC/DAY2SEC;
  Mercury.setGM(0.491254745145081187E-10*scale);
  Venus  .setGM(0.724345248616270270E-09*scale); 
  //(GM for the Earth-Moon Barycenter)/(1+1/EMRAT):
  Earth  .setGM(0.899701137429187710E-09/(1.0 + 1.0/EMRat)*scale);
  Mars   .setGM(0.954953510577925806E-10*scale);
  Jupiter.setGM(0.282534590952422643E-06*scale);
  Saturn .setGM(0.845971518568065874E-07*scale);
  Uran   .setGM(0.129202491678196939E-07*scale);
  Neptun .setGM(0.152435890078427628E-07*scale);
  Pluto  .setGM(0.218869976542596968E-11*scale);
  //(GM for the Earth-Moon Barycenter)/(1+EMRAT):
  Moon   .setGM(0.899701137429187710E-09/(1.0 + EMRat)*scale);
  GeoMoon.setGM(Moon.gm());
  Sun    .setGM(0.295912208285591095E-03*scale);
  initData();
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBEphemDE405 implementation								*/
/*												*/
/*==============================================================================================*/
/**A constructor.
 * Creates a copy of object.
 */
SBEphemDE405::SBEphemDE405(const SBConfig *Cfg_) 
  : SBEphem(Cfg_, "JPL Planetary Ephemeris DE405/LE405") 
{
  Loc=SBEphemDE403::LocDE403; 
  DT=32.0; 
  NRecCoef=1018; 
  AU=0.149597870691000015E+12; //meters
  CLight = 299792457.999999984;//meters/sec
  EMRat  = 81.3005600000000044;
  //0.899701134671249882E-09 // GM(Earth-Moon Barycenter)

  double scale = AU*AU*AU/DAY2SEC/DAY2SEC;
  Mercury.setGM(0.491254745145081187E-10*scale);
  Venus  .setGM(0.724345248616270270E-09*scale); 
  //(GM for the Earth-Moon Barycenter)/(1+1/EMRAT):
  Earth  .setGM(0.899701134671249882E-09/(1.0 + 1.0/EMRat)*scale);
  Mars   .setGM(0.954953510577925806E-10*scale);
  Jupiter.setGM(0.282534590952422643E-06*scale);
  Saturn .setGM(0.845971518568065874E-07*scale);
  Uran   .setGM(0.129202491678196939E-07*scale);
  Neptun .setGM(0.152435890078427628E-07*scale);
  Pluto  .setGM(0.218869976542596968E-11*scale);
  //(GM for the Earth-Moon Barycenter)/(1+EMRAT):
  Moon   .setGM(0.899701134671249882E-09/(1.0 + EMRat)*scale);
  GeoMoon.setGM(Moon.gm());
  Sun    .setGM(0.295912208285591095E-03*scale);
  initData();
};
/*==============================================================================================*/





/*==============================================================================================*/

