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

/* NGS and MkIII DBH format parsers are coded here						*/

#ifndef SB_GEO_OBS_VLBI_IO_H
#define SB_GEO_OBS_VLBI_IO_H

#include <config.h>
#include <stdlib.h>

#include <qintdict.h>

#include "SbGeoObsVLBI.H"





/*==============================================================================================*/
/*												*/
/* class SBTS_ngs										*/
/*												*/
/* a text stream for i/o NGS format files (observations)					*/
/*==============================================================================================*/
class SBTS_ngs : public QTextStream
{};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBDS_dbh										*/
/*												*/
/* a data stream for i MarkIII DBH format files (observations)					*/
/*==============================================================================================*/
class SBDS_dbh : public QDataStream
{};
/*==============================================================================================*/



/*==============================================================================================*/
/* MarkIII DBH handling:									*/
/*==============================================================================================*/
/*==============================================================================================*/
/*												*/
/* class SBDBHPar										*/
/*												*/
/*==============================================================================================*/
class SBDBHPar
{
  friend class SBDBHTEBlock;
public:
  enum PType {T_R8, T_I2, T_A2, T_D8, T_J4, T_UNKN};
protected:
  QString		Abbrev,	Descr;
  PType			Type;
  short			Dim1, Dim2, Dim3, VerOfMod, NTC, NTE;
  int			Offset;
public:
  SBDBHPar(){Abbrev="Vasia";Descr="Petia";Dim1=Dim2=Dim3=VerOfMod=NTC=NTE=0;Offset=0;Type=T_UNKN;};
  const QString&	abbrev(){return Abbrev;};
  const QString&	descr(){return Descr;};
  short			dim1(){return Dim1;};
  short			dim2(){return Dim2;};
  short			dim3(){return Dim3;};
  short			verOfMod(){return VerOfMod;};
  short			nTC(){return NTC;};
  short			nTE(){return NTE;};
  int			offset(){return Offset;};
  PType			type(){return Type;};
  void			setOffset(int Offset_){Offset=Offset_;};
  void			setType(PType Type_){Type=Type_;};
  void			setNTC(short NTC_){NTC=NTC_;};
  void			setNTE(short NTE_){NTE=NTE_;};
};
/*==============================================================================================*/


/*==============================================================================================*/
/*												*/
/* class SBDBHPhysRec										*/
/*												*/
/*==============================================================================================*/
class SBDBHPhysRec
{
protected:
  int		Length;
  char*		LogRec;
  bool		isOK;
public:
  SBDBHPhysRec(){Length=0;LogRec=NULL;isOK=TRUE;};
  virtual ~SBDBHPhysRec(){if (LogRec) delete []LogRec;};
  friend SBDS_dbh &operator>>(SBDS_dbh&, SBDBHPhysRec&);
  bool	isOk(){return isOK;};
  int	length(){return Length;};
  virtual SBDBHPar::PType type(){return SBDBHPar::T_UNKN;};
  virtual int parseLR(SBDS_dbh& s)
    {if (LogRec) delete []LogRec; LogRec = new char[Length+1]; s.readRawBytes(LogRec, Length); 
    *(LogRec+Length)=0; return Length;};
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBDBHLRecPrefixed									*/
/* Service record, abstract									*/
/*==============================================================================================*/
class SBDBHLRecPrefixed : public SBDBHPhysRec
{
protected:
  char		Prefix[2];
  char		P[3];
public:
  SBDBHLRecPrefixed(char P_[2]){Prefix[0]=Prefix[1]='1';P[0]=P_[0];P[1]=P_[1];P[2]=0;};
  bool isP(){return Prefix[0]==P[0]&&Prefix[1]==P[1];};
  virtual bool isAlter(){return FALSE;};
  virtual bool isPrefixParsed(SBDS_dbh&);
  virtual int parseLR(SBDS_dbh& s){if (!isPrefixParsed(s)) return 2; else return 2;};
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBDBHLRecHS1										*/
/* Service record, HS1										*/
/* (we just ignore HS2 record)									*/
/*==============================================================================================*/
class SBDBHLRecHS1 : public SBDBHLRecPrefixed
{
protected:
  short		HistLength;
  short		Date[5];
  short		VerNum;
public:
  SBDBHLRecHS1() : SBDBHLRecPrefixed("HS")
    {HistLength=0;Date[0]=Date[1]=Date[2]=Date[3]=Date[4]=0;VerNum=0;};
  int histLength(){return HistLength;};
  int verNum(){return VerNum;};
  SBMJD date(){return SBMJD(Date);};
  bool isZ1(){return Prefix[0]=='Z'&&Prefix[1]=='Z';};
  virtual bool isAlter(){return isZ1();};
  virtual int parseLR(SBDS_dbh& s)
    {
      HistLength=0;Date[0]=Date[1]=Date[2]=Date[3]=Date[4]=0;VerNum=0;
      if (!isPrefixParsed(s)) return 2; 
      else s>>HistLength>>Date[0]>>Date[1]>>Date[2]>>Date[3]>>Date[4]>>VerNum;
      return 16;
    };
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBDBHLRecTE										*/
/* Service record, TE										*/
/*==============================================================================================*/
class SBDBHLRecTE : public SBDBHLRecPrefixed
{
  friend class SBDBHTEBlock;
protected:
  short		TEBlockNum, GroupNum;
  short		OffsetI, OffsetA, OffsetD, OffsetJ;
  short		R8Num, I2Num, A2Num, D8Num, J4Num;
public:
  SBDBHLRecTE() : SBDBHLRecPrefixed("TE")
    {TEBlockNum=GroupNum=OffsetI=OffsetA=OffsetD=OffsetJ=R8Num=I2Num=A2Num=D8Num=J4Num=0;};
  void dump(QTextStream& s)
    {
      s << "TE record: TE block number: "<<TEBlockNum<<"; number of groups: " <<GroupNum
	<< "\nOffsets: I2:"<<OffsetI<<" A2:"<<OffsetA<<" D8:"<<OffsetD<<" J4:"<<OffsetJ
	<< " (bytes)\nNumbers: I2:"<< I2Num <<" A2:"<< A2Num <<" D8:"<< D8Num <<" J4:"<<J4Num
	<<" R8:"<<R8Num<<"\n";
    };
  virtual int parseLR(SBDS_dbh& s)
    {
      TEBlockNum=GroupNum=OffsetI=OffsetA=OffsetD=OffsetJ=R8Num=I2Num=A2Num=D8Num=J4Num=0;
      if (!isPrefixParsed(s)) return 2; 
      else s>>TEBlockNum>>OffsetI>>OffsetA>>OffsetD>>R8Num>>I2Num>>A2Num
	    >>OffsetJ>>GroupNum>>D8Num>>J4Num;
      return 24;
    };
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBDBHLRecTC										*/
/* Service record, TC										*/
/*==============================================================================================*/
class SBDBHLRecTC : public SBDBHLRecPrefixed
{
protected:
  short		TOCType, NumTEBlocks;
  bool		IsZ2;
public:
  SBDBHLRecTC() : SBDBHLRecPrefixed("TC")
    {TOCType=NumTEBlocks=0;IsZ2=FALSE;};
  bool isZ2(){return IsZ2;};
  short	numTEBlocks(){return NumTEBlocks;};
  short	tocType(){return TOCType;};
  virtual bool isAlter(){return Prefix[0]=='Z'&&Prefix[1]=='Z';};
  void dump(QTextStream& s)
    {s<<"TC record: TOC Type: "<<TOCType<<"; number of TE blocks: "<<NumTEBlocks<<"\n";};
  virtual int parseLR(SBDS_dbh&);
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBDBHLRecDR										*/
/* Service record, DR										*/
/*==============================================================================================*/
class SBDBHLRecDR : public SBDBHLRecPrefixed
{
protected:
  short		P1, P2;
public:
  SBDBHLRecDR() : SBDBHLRecPrefixed("DR") {P1=P2=0;};
  int	p1(){return P1;};
  int	p2(){return P2;};
  virtual int parseLR(SBDS_dbh& s)
    {
      P1=P2=0;
      if (!isPrefixParsed(s)) return 2; 
      else s>>P1>>P2;
      return 6;
    };
  void	dump(QTextStream& s)
    {s<<"DR record: P1: "<<P1<<"; P2:"<<P2<<"; Prefix: " << Prefix[0] << Prefix[1] << "\n";};
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBDBHLRecDE										*/
/* Service record, DE										*/
/*==============================================================================================*/
class SBDBHLRecDE : public SBDBHLRecPrefixed
{
protected:
  short		P1, P2, P3, P4, P5, P6, P7;
  bool		IsZ3;
public:
  SBDBHLRecDE() : SBDBHLRecPrefixed("DE") {P1=P2=P3=P4=P5=P6=P7=0;IsZ3=FALSE;};
  int	p1(){return P1;};
  int	p2(){return P2;};
  int	p3(){return P3;};
  int	p4(){return P4;};
  int	p5(){return P5;};
  int	p6(){return P6;};
  int	p7(){return P7;};
  virtual bool	isAlter(){return Prefix[0]=='Z'&&Prefix[1]=='Z';};
  bool		isZ3(){return IsZ3;};
  virtual int	parseLR(SBDS_dbh&);
  void	dump(QTextStream& s) {s<<"DE record: P1: "<<P1<<"; P2:"<<P2<<"; P3:"<<P3<<"; P4:"
			       <<P4<<"; P5:"<<P5<<"; P6:"<<P6<<"; P7:"<<P7<<"\n";};
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBDBHLRecZ3										*/
/* Service record, Z3										*/
/*==============================================================================================*/
class SBDBHLRecZ3 : public SBDBHLRecPrefixed
{
protected:
  short		P1;
public:
  SBDBHLRecZ3() : SBDBHLRecPrefixed("ZZ") {P1=0;};
  int	p1(){return P1;};
  virtual int parseLR(SBDS_dbh& s)
    {
      P1=0;
      if (!isPrefixParsed(s)) return 2; 
      else s>>P1;
      return 4;
    };
  void	dump(QTextStream& s){s<<"Z3 record: P1: " << P1 << "\n";};
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBDBHDRecString									*/
/* Data record: N x A2										*/
/* WARNING: this class describes two kind of strings, it has to be used by either text() call	*/
/*	    or val(...), nor the both methods.							*/
/*==============================================================================================*/
class SBDBHDRecString : public SBDBHPhysRec
{
protected:
  QString	Text;
public:
  SBDBHDRecString(){Text="Is there anybody out there?";};
  const QString& text(){return Text;};
  virtual const QString val(SBDBHPar*, int, int);
  virtual SBDBHPar::PType type(){return SBDBHPar::T_A2;};
  virtual int parseLR(SBDS_dbh& s)
    {int i=SBDBHPhysRec::parseLR(s); if (isOK) Text=(const char*)LogRec; return i;};
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBDBHDRecT<Class>									*/
/* Data record: N x Class (all other kinds of input data)					*/
/*==============================================================================================*/
template<class Class> class SBDBHDRecT : public SBDBHPhysRec
{
protected:
  Class*	nD;
  int		Num;
public:
  SBDBHDRecT(){nD=NULL; Num=0;};
  virtual ~SBDBHDRecT(){if (nD) delete []nD;};
  virtual Class operator[](int i){return nD&&-1<i&&i<Num?*(nD+i):(Class)0;};
  virtual Class val(SBDBHPar* Par, int dim1, int dim2, int dim3) // zero based indexes
    {
      return (dim1<Par->dim1()&&dim2<Par->dim2()&&dim3<Par->dim3()&&dim1>=0&&dim2>=0&&dim3>=0)?
	operator[](Par->offset() + dim1 + Par->dim1()*dim2 + Par->dim1()*Par->dim2()*dim3)
	:(Class)0;
    };
  int num(){return Num;};
  virtual int parseLR(SBDS_dbh& s)
    {
      if (nD) delete []nD;
      nD=new Class[(Num=Length/sizeof(Class))];  
      Class *p=nD; 
      for(int i=0; i<Num; i++,p++) s>>*p;
      return Length;
    };
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBDBHHistTriplet									*/
/* History entry										*/
/*==============================================================================================*/
class SBDBHHistTriplet
{
protected:
  SBDBHLRecHS1		Rec1;
  SBDBHPhysRec		Rec2;
  SBDBHDRecString	Rec3;
  bool			isOK;
public:
  SBDBHHistTriplet(){isOK=TRUE;};
  bool	isHistLine(){return Rec1.isP();};
  bool	isLast(){return Rec1.isZ1();};
  int	verNum(){return Rec1.verNum();};
  SBMJD date(){return Rec1.date();};
  const QString& text(){return Rec3.text();};
  bool	isOk(){return isOK;};
  void	dump(QTextStream& s)
    {
      s<< Rec1.date().toString(SBMJD::F_Short) << " (" << Rec1.verNum() << "): \""
       << Rec3.text() << "\"\n";
    };
  friend SBDS_dbh &operator>>(SBDS_dbh&, SBDBHHistTriplet&);
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBDBHTEBlock										*/
/*												*/
/*==============================================================================================*/
class SBDBHTEBlock
{
  friend class SBDBHFormat;
protected:
  class SBDBHLRecP3 : public SBDBHPhysRec
  {
  public:
    SBDBHTEBlock	*Owner;
    SBDBHLRecP3(SBDBHTEBlock* Owner_){Owner=Owner_;};
    virtual int parseLR(SBDS_dbh& s){return Owner->parseP3Rec(s);};
  };
  class SBDBHLRecP4 : public SBDBHPhysRec
  {
  public:
    SBDBHTEBlock	*Owner;
    SBDBHLRecP4(SBDBHTEBlock* Owner_){Owner=Owner_;};
    virtual int parseLR(SBDS_dbh& s){return Owner->parseP4Rec(s);};
  };
  SBDBHLRecTE		Rec1;
  SBDBHLRecP3		Rec2;
  SBDBHLRecP4		Rec3;
  bool			isOK;
  QList<SBDBHPar>	ListParms;
  QList<SBDBHPhysRec>	ListRecs;
  QIntDict<SBDBHPhysRec> *RecByType;
public:
  SBDBHTEBlock() : Rec2(this), Rec3(this)
    {ListParms.setAutoDelete(TRUE); ListRecs.setAutoDelete(TRUE); RecByType=NULL;isOK=TRUE;};
  ~SBDBHTEBlock(){if (RecByType) delete RecByType;};
  friend SBDS_dbh &operator>>(SBDS_dbh& s, SBDBHTEBlock& B)
    {
      s>>B.Rec1>>B.Rec2>>B.Rec3; 
      B.isOK = B.isOK && B.Rec1.isOk() && B.Rec2.isOk() && B.Rec3.isOk();
      return s;
    };
  int	parseP3Rec(SBDS_dbh&);
  int	parseP4Rec(SBDS_dbh&);
  void	dump(QTextStream&);
  bool	isOk(){return isOK;};
  QList<SBDBHPar>	*parms(){return &ListParms;};
  QList<SBDBHPhysRec>	*recs(){return &ListRecs;};
  QIntDict<SBDBHPhysRec> *recByType(){return RecByType;};
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBDBHTCBlock										*/
/*												*/
/*==============================================================================================*/
class SBDBHTCBlock
{
  friend class SBDBHFormat;
protected:
  SBDBHLRecTC		Rec;
  QList<SBDBHTEBlock>	ListTEBlock;
  bool			isOK;
  QDict<SBDBHPar>	*ParByAbbrev;
public:
  SBDBHTCBlock() {ListTEBlock.setAutoDelete(TRUE);isOK=TRUE;
  ParByAbbrev=new QDict<SBDBHPar>(20); ParByAbbrev->setAutoDelete(FALSE);};
  ~SBDBHTCBlock(){if (ParByAbbrev) delete ParByAbbrev;};
  bool	isLast(){return Rec.isZ2();};
  QList<SBDBHTEBlock>	*teBlocks(){return &ListTEBlock;};
  QDict<SBDBHPar>	*parByAbbrev(){return ParByAbbrev;};
  short	numTEBlocks(){return Rec.numTEBlocks();};
  short	tocType(){return Rec.tocType();};
  bool	isOk(){return isOK;};
  void	dump(QTextStream& s)
    {
      s << "== TC[" << tocType() << "] Block dump: ==\n";
      Rec.dump(s);
      for (SBDBHTEBlock* te=ListTEBlock.first(); te; te=ListTEBlock.next()) te->dump(s);
      s << "==\n";
    };
  friend SBDS_dbh &operator>>(SBDS_dbh&, SBDBHTCBlock&);
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBDBHStartBlock									*/
/*												*/
/*==============================================================================================*/
class SBDBHStartBlock
{
protected:
  SBDBHDRecString	DbName, ExpDescr, SessID, PrevDb, Prev;
  SBDBHDRecT<short>	Date, Ver;
  bool			isOK;
  short			DateVal[5];
public:
  SBDBHStartBlock(){isOK=FALSE;DateVal[0]=DateVal[1]=DateVal[2]=DateVal[3]=DateVal[4]=0;};
  bool	isOk(){return isOK;};
  SBMJD	date(){return SBMJD(DateVal);};
  int	version(){return Ver[0];};
  const QString& dbName(){return DbName.text();};
  const QString& expDescr(){return ExpDescr.text();};
  const QString& sessID(){return SessID.text();};
  const QString& prevDb(){return PrevDb.text();};
  const QString& prevDescr(){return Prev.text();};
  void	dump(QTextStream&);
  friend SBDS_dbh &operator>>(SBDS_dbh&, SBDBHStartBlock&);
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBDBHFormat										*/
/*												*/
/*==============================================================================================*/
class SBDBHFormat
{
protected:
  QList<SBDBHTCBlock>	ListTCBlock;
  //  QDict<SBDBHPar>	*ParByAbbrev;
  QTextStream		*Dump;
  bool			isOK;
  short			TCcurrent;
public:
  SBDBHFormat() {ListTCBlock.setAutoDelete(TRUE); TCcurrent=-1; Dump=NULL;isOK=TRUE;};
  QList<SBDBHTCBlock>	*tcBlocks(){return &ListTCBlock;};
  bool	isOk(){return isOK;};
  short	tcNum(){return TCcurrent;};
  void	postRead();
  void	setDump(QTextStream* Dump_){Dump=Dump_;};
  void	dump(QTextStream& s)
    {
      s << "== Format Block dump: ==\n";
      for(SBDBHTCBlock *tc=ListTCBlock.first(); tc; tc=ListTCBlock.next())tc->dump(s);
      s << "==\n";
    };
  friend SBDS_dbh &operator>>(SBDS_dbh&, SBDBHFormat&);
  void getBlock(SBDS_dbh&);
  SBDBHTCBlock	*currentTC(){return TCcurrent>-1?ListTCBlock.at(TCcurrent):NULL;};
  SBDBHPhysRec	*parRecord(SBDBHPar *Par)
    {
      SBDBHTEBlock		*te;
      te = currentTC()?currentTC()->teBlocks()->at(Par->nTE()):NULL;
      return te?te->recByType()->find(Par->type()):NULL;
    };
  SBDBHPar	*lookupPar(const char*);
  short		i2(SBDBHPar *Par, int i, int j, int k)
    {return Par?((SBDBHDRecT<short>*)parRecord(Par))->val(Par, i,j,k):0;};
  double	r8(SBDBHPar *Par, int i, int j, int k)
    {return Par?((SBDBHDRecT<double>*)parRecord(Par))->val(Par, i,j,k):0.0;};
  double	d8(SBDBHPar *Par, int i, int j, int k)
    {return Par?((SBDBHDRecT<double>*)parRecord(Par))->val(Par, i,j,k):0.0;};
  int		j4(SBDBHPar *Par, int i, int j, int k)
    {return Par?((SBDBHDRecT<int>*)parRecord(Par))->val(Par, i,j,k):0;};
  QString	str(SBDBHPar *Par, int i, int j)
    {return Par?((SBDBHDRecString*)parRecord(Par))->val(Par, i,j):QString("UNPARSED");};
};
/*==============================================================================================*/






#endif //SB_GEO_OBS_VLBI_IO_H
