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

#include "SbGeoObs.H"


/*==============================================================================================*/
/*												*/
/* class SBObservation implementation								*/
/*												*/
/*==============================================================================================*/
/**A constructor.
 * Creates a copy of the object, fills data with zeros.
 */
SBObservation::SBObservation()
  : SBMJD(TZero), SBAttributed()
{
  Tech= TECH_UNKN;
  Idx = -1;
};

/**A constructor.
 * Creates a copy of the object.
 * \param Obs_ is value to copy in.
 */
SBObservation::SBObservation(const SBObservation& Obs_) 
  : SBMJD(Obs_), SBAttributed(Obs_)
{
  setTech(Obs_.tech());
  setIdx (Obs_.idx ());
};

/**An operator of assignment.
 * Makes a coping of the object.
 * \param Obs_ is value to copy in.
 */
SBObservation& SBObservation::operator= (const SBObservation& Obs_)
{
  SBMJD::operator=(Obs_);
  SBAttributed::operator=(Obs_);
  setTech(Obs_.tech());
  setIdx (Obs_.idx ());
  return *this;
};
/*==============================================================================================*/




/*==============================================================================================*/
/*												*/
/* class SBObservationList implementation							*/
/*												*/
/*==============================================================================================*/
/**An operator of assignment.
 * Makes a coping of the object (shalow copy).
 * \param Obs_ is value to copy in.
 */
SBObservationList& SBObservationList::operator= (const SBObservationList& List_)
{
  clear();
  if (List_.count())
    {
      SBObsIterator it(List_);
      while (it.current())
	{
	  inSort(it.current());
	  ++it;
	};
    };
  return *this;
};


/*==============================================================================================*/




/*==============================================================================================*/
