/*
 *
 * This file is part of SteelBreeze.
 *
 * SteelBreeze, the geodetic VLBI data analysing software.
 * Copyright (C) 2007 Sergei Bolotin, MAO NASU, Kiev, Ukraine.
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

#include "SbGeoSourceMap.H"
#include "SbSetup.H"

#ifdef USECCFITS
#	include <CCfits/CCfits.h>
#	include <CCfits/FITS.h>
#	include <CCfits/FITSUtilT.h>
#	include <CCfits/PHDUT.h>
#endif



/*==============================================================================================*/
/*												*/
/* class SBSrcMap implementation								*/
/*												*/
/*==============================================================================================*/
SBSrcMap::SBSrcMap() 
  : SBAttributed(), Image()
{
  FITSFileName	= "";
  kAuthor	= "";	//!< an author of the FITS file
  kCreator	= "";	//!< program which produced FITS file 
  kCType1	= "";	//!< axis #1 name
  kCType2	= "";	//!< axis #2 name
  kCType3	= "";	//!< axis #3 name
  kCType4	= "";	//!< axis #4 name
  kDateObs	= "";	//!< observation date
  kInstrument	= "";	//!< instrument used
  kObject	= "";	//!< name of observed source (IAU name?)
  kObserver	= "";	//!< VLBA experiment code
  kOrigin	= "";	//!< origin of data
  kProject	= "";	//!< 
  kSegment	= "";	//!< 
  kTelescop	= "";	//!< telescope used
  kBUnit	= "";	//!< unit of measurement
  kNIter	= 0;	//!< number of model components
  kBMaj		= 0.0;	//!< Clean beam major axis diameter (degrees)
  kBMin		= 0.0;	//!< Clean beam minor axis diameter (degrees)
  kBPA		= 0.0;	//!< Clean beam position angle (degrees)
  kCDelt1	= 0.0;	//!< Pixel increment for axis #1
  kCDelt2	= 0.0;	//!< Pixel increment for axis #2
  kCDelt3	= 0.0;	//!< Pixel increment for axis #3
  kCDelt4	= 0.0;	//!< Pixel increment for axis #4
  kCRota1	= 0.0;	//!< Axis #1 rotation
  kCRota2	= 0.0;	//!< Axis #2 rotation
  kCRota3	= 0.0;	//!< Axis #3 rotation
  kCRota4	= 0.0;	//!< Axis #4 rotation
  kCRPix1	= 0.0;	//!< Reference pixel for axis #1
  kCRPix2	= 0.0;	//!< Reference pixel for axis #2
  kCRPix3	= 0.0;	//!< Reference pixel for axis #3
  kCRPix4	= 0.0;	//!< Reference pixel for axis #4
  kCRVal1	= 0.0;	//!< Reference value in right ascens. (deg)
  kCRVal2	= 0.0;	//!< Reference value in declination   (deg)
  kCRVal3	= 0.0;	//!< Reference value for axis #3
  kCRVal4	= 0.0;	//!< Reference value for axis #4
  kEpoch	= 0.0;	//!< Equinox of coordinates
  kFluxInt	= 0.0;	//!< Total flux density integrated over the map (Jy)
  kFluxShr	= 0.0;	//!< Flux density at short baselines (Jy)
  kFluxUnr	= 0.0;	//!< Unresolved flux density at long baselines (Jy)
  kImageNoise	= 0.0;	//!< Image 1-sigma noise (Jy)
  kNoise	= 0.0;	//!< rms of the input visibilities (Jy)
  kObsDec	= 0.0;	//!< Antenna pointing Dec
  kObsRA	= 0.0;	//!< Antenna pointing RA
  kPOS_Err	= 0.0;	//!< 1-sigma error ellipse semi-major axis (rad)
  kRA_D_Corr	= 2.0;	//!< Correl. between right ascens. and decl. errors
  kDataMax	= 0.0;	//!< Max data
  kDataMin	= 0.0;	//!< Min data
  Name		= "unnamed";
};

SBSrcMap::~SBSrcMap()
{
};

bool SBSrcMap::importFITS(const QString& FITSInputFileName)
{
  bool			IsOK = FALSE;
#ifdef USECCFITS
  std::string		InputFileName((const char*)FITSInputFileName), str;
  CCfits::FITS*		FITSFile = NULL;
  int			j=0;
  double		f=0.0;
  
  try 
    {
      FITSFile = new CCfits::FITS(InputFileName, CCfits::Read, TRUE);
    }
  catch (CCfits::FITS::CantOpen)
    {
      Log->write(SBLog::WRN, SBLog::IO, ClassName() + ": cannot open FITS file " + FITSInputFileName);
      return FALSE;
    };

  CCfits::PHDU& phdu     = FITSFile->pHDU();

  try
    {
      phdu.readAllKeys();
    }
  catch (CCfits::FitsException&)
    {
      Log->write(SBLog::WRN, SBLog::IO, ClassName() + 
		 ": error in reading all keys from the FITS file " + FITSInputFileName);
      return FALSE;
    };
  
  FITSFileName = FITSInputFileName;
      
  // dancing around weak CCFITS design:
  // std::string:
  try {phdu.readKey("AUTHOR", str);}
  catch (CCfits::FitsException&)
    {
      Log->write(SBLog::WRN, SBLog::IO, ClassName() + 
		 ": error in reading AUTHOR key from the FITS file " + FITSInputFileName);
      str = "";
    };
  kAuthor = str.data();

  try {phdu.readKey("CREATOR", str);}
  catch (CCfits::FitsException&)
    {
      Log->write(SBLog::WRN, SBLog::IO, ClassName() + 
		 ": error in reading CREATOR key from the FITS file " + FITSInputFileName);
      str = "";
    };
  kCreator = str.data();

  try {phdu.readKey("CTYPE1", str);}
  catch (CCfits::FitsException&)
    {
      Log->write(SBLog::WRN, SBLog::IO, ClassName() + 
		 ": error in reading CTYPE1 key from the FITS file " + FITSInputFileName);
      str = "";
    };
  kCType1 = str.data();

  try {phdu.readKey("CTYPE2", str);}
  catch (CCfits::FitsException&)
    {
      Log->write(SBLog::WRN, SBLog::IO, ClassName() + 
		 ": error in reading CTYPE2 key from the FITS file " + FITSInputFileName);
      str = "";
    };
  kCType2 = str.data();

  try {phdu.readKey("CTYPE3", str);}
  catch (CCfits::FitsException&)
    {
      Log->write(SBLog::WRN, SBLog::IO, ClassName() + 
		 ": error in reading CTYPE3 key from the FITS file " + FITSInputFileName);
      str = "";
    };
  kCType3 = str.data();

  try {phdu.readKey("CTYPE4", str);}
  catch (CCfits::FitsException&)
    {
      Log->write(SBLog::WRN, SBLog::IO, ClassName() + 
		 ": error in reading CTYPE4 key from the FITS file " + FITSInputFileName);
      str = "";
    };
  kCType4 = str.data();

  try {phdu.readKey("DATE-OBS", str);}
  catch (CCfits::FitsException&)
    {
      Log->write(SBLog::WRN, SBLog::IO, ClassName() + 
		 ": error in reading DATE-OBS key from the FITS file " + FITSInputFileName);
      str = "";
    };
  kDateObs = str.data();

  try {phdu.readKey("INSTRUME", str);}
  catch (CCfits::FitsException&)
    {
      Log->write(SBLog::WRN, SBLog::IO, ClassName() + 
		 ": error in reading INSTRUME key from the FITS file " + FITSInputFileName);
      str = "";
    };
  kInstrument = str.data();

  try {phdu.readKey("OBJECT", str);}
  catch (CCfits::FitsException&)
    {
      Log->write(SBLog::WRN, SBLog::IO, ClassName() + 
		 ": error in reading OBJECT key from the FITS file " + FITSInputFileName);
      str = "";
    };
  kObject = str.data();

  try {phdu.readKey("OBSERVER", str);}
  catch (CCfits::FitsException&)
    {
      Log->write(SBLog::WRN, SBLog::IO, ClassName() + 
		 ": error in reading OBSERVER key from the FITS file " + FITSInputFileName);
      str = "";
    };
  kObserver = str.data();

  try {phdu.readKey("ORIGIN", str);}
  catch (CCfits::FitsException&)
    {
      Log->write(SBLog::WRN, SBLog::IO, ClassName() + 
		 ": error in reading ORIGIN key from the FITS file " + FITSInputFileName);
      str = "";
    };
  kOrigin = str.data();

  try {phdu.readKey("PROJECT", str);}
  catch (CCfits::FitsException&)
    {
      Log->write(SBLog::WRN, SBLog::IO, ClassName() + 
		 ": error in reading PROJECT key from the FITS file " + FITSInputFileName);
      str = "";
    };
  kProject = str.data();

  try {phdu.readKey("SEGMENT", str);}
  catch (CCfits::FitsException&)
    {
      Log->write(SBLog::WRN, SBLog::IO, ClassName() + 
		 ": error in reading SEGMENT key from the FITS file " + FITSInputFileName);
      str = "";
    };
  kSegment = str.data();

  try {phdu.readKey("TELESCOP", str);}
  catch (CCfits::FitsException&)
    {
      Log->write(SBLog::WRN, SBLog::IO, ClassName() + 
		 ": error in reading TELESCOP key from the FITS file " + FITSInputFileName);
      str = "";
    };
  kTelescop = str.data();

  try {phdu.readKey("BUNIT", str);}
  catch (CCfits::FitsException&)
    {
      Log->write(SBLog::WRN, SBLog::IO, ClassName() + 
		 ": error in reading BUNIT key from the FITS file " + FITSInputFileName);
      str = "";
    };
  kBUnit = str.data();

  // int:
  try {phdu.readKey("NITER", j);}
  catch (CCfits::FitsException&)
    {
      Log->write(SBLog::WRN, SBLog::IO, ClassName() + 
		 ": error in reading NITER key from the FITS file " + FITSInputFileName);
      j = 0;
    };
  kNIter = j;

  // double:
  try {phdu.readKey("BMAJ", f);}
  catch (CCfits::FitsException&)
    {
      Log->write(SBLog::WRN, SBLog::IO, ClassName() + 
		 ": error in reading BMAJ key from the FITS file " + FITSInputFileName);
      f = 0.0;
    };
  kBMaj = f;

  try {phdu.readKey("BMIN", f);}
  catch (CCfits::FitsException&)
    {
      Log->write(SBLog::WRN, SBLog::IO, ClassName() + 
		 ": error in reading BMIN key from the FITS file " + FITSInputFileName);
      f = 0.0;
    };
  kBMin = f;

  try {phdu.readKey("BPA", f);}
  catch (CCfits::FitsException&)
    {
      Log->write(SBLog::WRN, SBLog::IO, ClassName() + 
		 ": error in reading BPA key from the FITS file " + FITSInputFileName);
      f = 0.0;
    };
  kBPA = f;

  try {phdu.readKey("CDELT1", f);}
  catch (CCfits::FitsException&)
    {
      Log->write(SBLog::WRN, SBLog::IO, ClassName() + 
		 ": error in reading CDELT1 key from the FITS file " + FITSInputFileName);
      f = 0.0;
    };
  kCDelt1 = f;

  try {phdu.readKey("CDELT2", f);}
  catch (CCfits::FitsException&)
    {
      Log->write(SBLog::WRN, SBLog::IO, ClassName() + 
		 ": error in reading CDELT2 key from the FITS file " + FITSInputFileName);
      f = 0.0;
    };
  kCDelt2 = f;

  try {phdu.readKey("CDELT3", f);}
  catch (CCfits::FitsException&)
    {
      Log->write(SBLog::WRN, SBLog::IO, ClassName() + 
		 ": error in reading CDELT3 key from the FITS file " + FITSInputFileName);
      f = 0.0;
    };
  kCDelt3 = f;

  try {phdu.readKey("CDELT4", f);}
  catch (CCfits::FitsException&)
    {
      Log->write(SBLog::WRN, SBLog::IO, ClassName() + 
		 ": error in reading CDELT4 key from the FITS file " + FITSInputFileName);
      f = 0.0;
    };
  kCDelt4 = f;

  try {phdu.readKey("CROTA1", f);}
  catch (CCfits::FitsException&)
    {
      Log->write(SBLog::WRN, SBLog::IO, ClassName() + 
		 ": error in reading CROTA1 key from the FITS file " + FITSInputFileName);
      f = 0.0;
    };
  kCRota1 = f;

  try {phdu.readKey("CROTA2", f);}
  catch (CCfits::FitsException&)
    {
      Log->write(SBLog::WRN, SBLog::IO, ClassName() + 
		 ": error in reading CROTA2 key from the FITS file " + FITSInputFileName);
      f = 0.0;
    };
  kCRota2 = f;

  try {phdu.readKey("CROTA3", f);}
  catch (CCfits::FitsException&)
    {
      Log->write(SBLog::WRN, SBLog::IO, ClassName() + 
		 ": error in reading CROTA3 key from the FITS file " + FITSInputFileName);
      f = 0.0;
    };
  kCRota3 = f;

  try {phdu.readKey("CROTA4", f);}
  catch (CCfits::FitsException&)
    {
      Log->write(SBLog::WRN, SBLog::IO, ClassName() + 
		 ": error in reading CROTA4 key from the FITS file " + FITSInputFileName);
      f = 0.0;
    };
  kCRota4 = f;

  try {phdu.readKey("CRPIX1", f);}
  catch (CCfits::FitsException&)
    {
      Log->write(SBLog::WRN, SBLog::IO, ClassName() + 
		 ": error in reading CRPIX1 key from the FITS file " + FITSInputFileName);
      f = 0.0;
    };
  kCRPix1 = f;

  try {phdu.readKey("CRPIX2", f);}
  catch (CCfits::FitsException&)
    {
      Log->write(SBLog::WRN, SBLog::IO, ClassName() + 
		 ": error in reading CRPIX2 key from the FITS file " + FITSInputFileName);
      f = 0.0;
    };
  kCRPix2 = f;

  try {phdu.readKey("CRPIX3", f);}
  catch (CCfits::FitsException&)
    {
      Log->write(SBLog::WRN, SBLog::IO, ClassName() + 
		 ": error in reading CRPIX3 key from the FITS file " + FITSInputFileName);
      f = 0.0;
    };
  kCRPix3 = f;

  try {phdu.readKey("CRPIX4", f);}
  catch (CCfits::FitsException&)
    {
      Log->write(SBLog::WRN, SBLog::IO, ClassName() + 
		 ": error in reading CRPIX4 key from the FITS file " + FITSInputFileName);
      f = 0.0;
    };
  kCRPix4 = f;

  try {phdu.readKey("CRVAL1", f);}
  catch (CCfits::FitsException&)
    {
      Log->write(SBLog::WRN, SBLog::IO, ClassName() + 
		 ": error in reading CRVAL1 key from the FITS file " + FITSInputFileName);
      f = 0.0;
    };
  kCRVal1 = f;

  try {phdu.readKey("CRVAL2", f);}
  catch (CCfits::FitsException&)
    {
      Log->write(SBLog::WRN, SBLog::IO, ClassName() + 
		 ": error in reading CRVAL2 key from the FITS file " + FITSInputFileName);
      f = 0.0;
    };
  kCRVal2 = f;

  try {phdu.readKey("CRVAL3", f);}
  catch (CCfits::FitsException&)
    {
      Log->write(SBLog::WRN, SBLog::IO, ClassName() + 
		 ": error in reading CRVAL3 key from the FITS file " + FITSInputFileName);
      f = 0.0;
    };
  kCRVal3 = f;

  try {phdu.readKey("CRVAL4", f);}
  catch (CCfits::FitsException&)
    {
      Log->write(SBLog::WRN, SBLog::IO, ClassName() + 
		 ": error in reading CRVAL4 key from the FITS file " + FITSInputFileName);
      f = 0.0;
    };
  kCRVal4 = f;

  try {phdu.readKey("EPOCH", f);}
  catch (CCfits::FitsException&)
    {
      Log->write(SBLog::WRN, SBLog::IO, ClassName() + 
		 ": error in reading EPOCH key from the FITS file " + FITSInputFileName);
      f = 0.0;
    };
  kEpoch = f;

  try {phdu.readKey("FLUX_INT", f);}
  catch (CCfits::FitsException&)
    {
      Log->write(SBLog::WRN, SBLog::IO, ClassName() + 
		 ": error in reading FLUX_INT key from the FITS file " + FITSInputFileName);
      f = 0.0;
    };
  kFluxInt = f;

  try {phdu.readKey("FLUX_SHR", f);}
  catch (CCfits::FitsException&)
    {
      Log->write(SBLog::WRN, SBLog::IO, ClassName() + 
		 ": error in reading FLUX_SHR key from the FITS file " + FITSInputFileName);
      f = 0.0;
    };
  kFluxShr = f;

  try {phdu.readKey("FLUX_UNR", f);}
  catch (CCfits::FitsException&)
    {
      Log->write(SBLog::WRN, SBLog::IO, ClassName() + 
		 ": error in reading FLUX_UNR key from the FITS file " + FITSInputFileName);
      f = 0.0;
    };
  kFluxUnr = f;

  try {phdu.readKey("IM_NOISE", f);}
  catch (CCfits::FitsException&)
    {
      Log->write(SBLog::WRN, SBLog::IO, ClassName() + 
		 ": error in reading IM_NOISE key from the FITS file " + FITSInputFileName);
      f = 0.0;
    };
  kImageNoise = f;

  try {phdu.readKey("NOISE", f);}
  catch (CCfits::FitsException&)
    {
      Log->write(SBLog::WRN, SBLog::IO, ClassName() + 
		 ": error in reading NOISE key from the FITS file " + FITSInputFileName);
      f = 0.0;
    };
  kNoise = f;

  try {phdu.readKey("OBSDEC", f);}
  catch (CCfits::FitsException&)
    {
      Log->write(SBLog::WRN, SBLog::IO, ClassName() + 
		 ": error in reading OBSDEC key from the FITS file " + FITSInputFileName);
      f = 0.0;
    };
  kObsDec = f;

  try {phdu.readKey("OBSRA", f);}
  catch (CCfits::FitsException&)
    {
      Log->write(SBLog::WRN, SBLog::IO, ClassName() + 
		 ": error in reading OBSRA key from the FITS file " + FITSInputFileName);
      f = 0.0;
    };
  kObsRA = f;

  try {phdu.readKey("POS_ERR", f);}
  catch (CCfits::FitsException&)
    {
      Log->write(SBLog::WRN, SBLog::IO, ClassName() + 
		 ": error in reading POS_ERR key from the FITS file " + FITSInputFileName);
      f = 0.0;
    };
  kPOS_Err = f;

  try {phdu.readKey("RA_D_COR", f);}
  catch (CCfits::FitsException&)
    {
      Log->write(SBLog::WRN, SBLog::IO, ClassName() + 
		 ": error in reading RA_D_COR key from the FITS file " + FITSInputFileName);
      f = 2.0;
    };
  kRA_D_Corr = f;

  try {phdu.readKey("DATAMAX", f);}
  catch (CCfits::FitsException&)
    {
      Log->write(SBLog::WRN, SBLog::IO, ClassName() + 
		 ": error in reading DATAMAX key from the FITS file " + FITSInputFileName);
      f = 0.0;
    };
  kDataMax = f;

  try {phdu.readKey("DATAMIN", f);}
  catch (CCfits::FitsException&)
    {
      Log->write(SBLog::WRN, SBLog::IO, ClassName() + 
		 ": error in reading DATAMIN key from the FITS file " + FITSInputFileName);
      f = 0.0;
    };
  kDataMin = f;


  delete FITSFile;
#endif
  return IsOK;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class SBSrcMap's friends implementation							*/
/*												*/
/*==============================================================================================*/
/**\relates SBSrcMap
 * Output to the data stream.
 */
QDataStream &operator<<(QDataStream& s, const SBSrcMap& M)
{
  return s 
    << (const SBAttributed&)M 
    << M.Image
    << M.FITSFileName 
    << M.kAuthor 
    << M.kCreator
    << M.kCType1
    << M.kCType2
    << M.kCType3
    << M.kCType4
    << M.kDateObs
    << M.kInstrument
    << M.kObject
    << M.kObserver
    << M.kOrigin
    << M.kProject
    << M.kSegment
    << M.kTelescop
    << M.kBUnit
    << M.kNIter
    << M.kBMaj
    << M.kBMin
    << M.kBPA
    << M.kCDelt1
    << M.kCDelt2
    << M.kCDelt3
    << M.kCDelt4
    << M.kCRota1
    << M.kCRota2
    << M.kCRota3
    << M.kCRota4
    << M.kCRPix1
    << M.kCRPix2
    << M.kCRPix3
    << M.kCRPix4
    << M.kCRVal1
    << M.kCRVal2
    << M.kCRVal3
    << M.kCRVal4
    << M.kEpoch
    << M.kFluxInt
    << M.kFluxShr
    << M.kFluxUnr
    << M.kImageNoise
    << M.kNoise
    << M.kObsDec
    << M.kObsRA
    << M.kPOS_Err
    << M.kRA_D_Corr
    << M.kDataMax
    << M.kDataMin

    << M.Name

    ;

};

/**\relates SBSrcMap
 * Input from the data stream.
 */
QDataStream &operator>>(QDataStream& s, SBSrcMap& M)
{
  s >> (SBAttributed&)M
    >> M.Image
    >> M.FITSFileName 
    >> M.kAuthor 
    >> M.kCreator
    >> M.kCType1
    >> M.kCType2
    >> M.kCType3
    >> M.kCType4
    >> M.kDateObs
    >> M.kInstrument
    >> M.kObject
    >> M.kObserver
    >> M.kOrigin
    >> M.kProject
    >> M.kSegment
    >> M.kTelescop
    >> M.kBUnit
    >> M.kNIter
    >> M.kBMaj
    >> M.kBMin
    >> M.kBPA
    >> M.kCDelt1
    >> M.kCDelt2
    >> M.kCDelt3
    >> M.kCDelt4
    >> M.kCRota1
    >> M.kCRota2
    >> M.kCRota3
    >> M.kCRota4
    >> M.kCRPix1
    >> M.kCRPix2
    >> M.kCRPix3
    >> M.kCRPix4
    >> M.kCRVal1
    >> M.kCRVal2
    >> M.kCRVal3
    >> M.kCRVal4
    >> M.kEpoch
    >> M.kFluxInt
    >> M.kFluxShr
    >> M.kFluxUnr
    >> M.kImageNoise
    >> M.kNoise
    >> M.kObsDec
    >> M.kObsRA
    >> M.kPOS_Err
    >> M.kRA_D_Corr
    >> M.kDataMax
    >> M.kDataMin

    >> M.Name
    
    ;

  return s;
};
/*==============================================================================================*/



/*==============================================================================================*/
