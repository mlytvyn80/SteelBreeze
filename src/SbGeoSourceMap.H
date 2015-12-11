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

#ifndef SB_GEO_SOURCE_MAP
#define SB_GEO_SOURCE_MAP

#include <config.h>
#include <qintdict.h>

#include "SbGeoSources.H"
#include "SbVector.H"



/*==============================================================================================*/
/**Provides a map of brightness distribution for radio source.
 * 
 * 
 */
/*==============================================================================================*/
class SBSrcMap : public SBAttributed
{
public:
  enum Attributes
  {
    One		   	= 1<<0,		//!< test 1
    Two		   	= 2<<0,		//!< test 2
  };

private:
  
  QString		FITSFileName;	//!< name of a FITS file from which the map was imported
  // keys from the FITS file (for curiosity):
  QString		kAuthor;	//!< an author of the FITS file
  QString		kCreator;	//!< program which produced FITS file 
  QString		kCType1;	//!< axis #1 name
  QString		kCType2;	//!< axis #2 name
  QString		kCType3;	//!< axis #3 name
  QString		kCType4;	//!< axis #4 name
  QString		kDateObs;	//!< observation date
  QString		kInstrument;	//!< instrument used
  QString		kObject;	//!< name of observed source (IAU name?)
  QString		kObserver;	//!< VLBA experiment code
  QString		kOrigin;	//!< origin of data
  QString		kProject;	//!< 
  QString		kSegment;	//!< 
  QString		kTelescop;	//!< telescope used
  QString		kBUnit;		//!< unit of measurement
  int			kNIter;		//!< number of model components
  double		kBMaj;		//!< Clean beam major axis diameter (degrees)
  double		kBMin;		//!< Clean beam minor axis diameter (degrees)
  double		kBPA;		//!< Clean beam position angle (degrees)
  double		kCDelt1;	//!< Pixel increment for axis #1
  double		kCDelt2;	//!< Pixel increment for axis #2
  double		kCDelt3;	//!< Pixel increment for axis #3
  double		kCDelt4;	//!< Pixel increment for axis #4
  double		kCRota1;	//!< Axis #1 rotation
  double		kCRota2;	//!< Axis #2 rotation
  double		kCRota3;	//!< Axis #3 rotation
  double		kCRota4;	//!< Axis #4 rotation
  double		kCRPix1;	//!< Reference pixel for axis #1
  double		kCRPix2;	//!< Reference pixel for axis #2
  double		kCRPix3;	//!< Reference pixel for axis #3
  double		kCRPix4;	//!< Reference pixel for axis #4
  double		kCRVal1;	//!< Reference value in right ascens. (deg)
  double		kCRVal2;	//!< Reference value in declination   (deg)
  double		kCRVal3;	//!< Reference value for axis #3
  double		kCRVal4;	//!< Reference value for axis #4
  double		kEpoch;		//!< Equinox of coordinates
  double		kFluxInt;	//!< Total flux density integrated over the map (Jy)
  double		kFluxShr;	//!< Flux density at short baselines (Jy)
  double		kFluxUnr;	//!< Unresolved flux density at long baselines (Jy)
  double		kImageNoise;	//!< Image 1-sigma noise (Jy)
  double		kNoise;		//!< rms of the input visibilities (Jy)
  double		kObsDec;	//!< Antenna pointing Dec
  double		kObsRA;		//!< Antenna pointing RA
  double		kPOS_Err;	//!< 1-sigma error ellipse semi-major axis (rad)
  double		kRA_D_Corr;	//!< Correl. between right ascens. and decl. errors
  double		kDataMax;	//!< Max data
  double		kDataMin;	//!< Min data

  SBMatrix		Image;		//!< a map itself
  QString		Name;		//!< name of a source

public:
  // constructors/destructors:
  //
  /**A default constructor.
   */
  SBSrcMap();
  /**A destructor. */
  virtual ~SBSrcMap();
  /**Refers to a class name (debug info) */
  virtual QString ClassName() const {return "SBSrcMap";};


  // interface:
  //


  // functions:
  //
  bool importFITS(const QString&);


  /* I/O: */
  inline friend QDataStream &operator<<(QDataStream&, const SBSrcMap&);
  inline friend QDataStream &operator>>(QDataStream&, SBSrcMap&);
};
/*==============================================================================================*/



#endif //SB_GEO_SOURCE_MAP
