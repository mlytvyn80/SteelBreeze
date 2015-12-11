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

#ifndef SB_GEO_OBS_H
#define SB_GEO_OBS_H

#include <config.h>
#include <qintdict.h>

#include "SbGeo.H"
#include "SbVector.H"



class SBRunManager;
class SBObservationList;
/*==============================================================================================*/
/**Class SBObservation.
 * The virtual class of an abstract observation.
 * 
 */
/*==============================================================================================*/
class SBObservation : public SBMJD, public SBAttributed
{
public:
  /**Observation's attributes.
   * Describes and keeps attributes common for all types of techniques.
   */
  enum Attributes
  {
    notValid			= 1<<0		//!< omit the observations;
  };

protected:
  int				Idx;		//!< an index;
  Technique			Tech;		//!< technique of the observation;

public:
  // constructors/destructors:
  //
  SBObservation();
  SBObservation(const SBObservation&);
  SBObservation& operator= (const SBObservation&);
  /**A destructor.*/
  virtual ~SBObservation(){};
  /**Refers to a class name (debug info).*/
  virtual QString	ClassName() const {return "SBObservation";};

  // interface:
  //
  virtual int		idx() const {return Idx;};
  virtual Technique	tech()const {return Tech;};
  virtual void		setIdx(int Idx_){Idx=Idx_;};
  virtual void		setTech(Technique Tech_){Tech=Tech_;};
  virtual const SBMJD&	mjd() {return *this;};
  
  // functions:
  //
  virtual bool		check() const =0;
  virtual bool		isEligible(SBRunManager*) {return !isAttr(notValid);};
  virtual void		process(SBRunManager*)=0;
  virtual const SBVector& O_C  ()=0;
  virtual const SBVector& Sigma()=0;
  
  virtual bool operator==(const SBObservation& O) const
    {return SBMJD::operator==(O) && idx()==O.idx() && tech()==O.tech();};
  
  virtual bool operator!=(const SBObservation& O) const {return !(O==*this);};
  
  virtual bool operator<(const SBObservation& O) const
    {return SBMJD::operator< (O)?TRUE:
      (SBMJD::operator==(O)?(idx()<O.idx()?TRUE:(idx()==O.idx()?tech()<O.tech():FALSE)):FALSE);};
  virtual int cmp(const SBObservation& O) const {return *this<O?-1:*this==O?0:1;};
  
  // I/O:
  //
};
/*==============================================================================================*/



typedef QListIterator<SBObservation>		SBObsIterator;
/*==============================================================================================*/
/**Class SBObservationList.
 * The virtual class to describe a list of abstract observations.
 * 
 */
/*==============================================================================================*/
class SBObservationList : public QList<SBObservation>
{
protected:
  virtual int compareItems(QCollection::Item item1, QCollection::Item item2)
    {
      SBObservation	*i1, *i2;
      i1=(SBObservation*)item1;
      i2=(SBObservation*)item2;
      return i1->cmp(*i2);
    };
  
public:
  // constructors/destructors:
  //
  /**A constructor.
   * Creates a copy of the object, fills data with zeros.
   */
  SBObservationList() : QList<SBObservation>(){};
  /**A constructor.
   * Creates a copy of the object.
   * \param List_ is value to copy in.
   */
  SBObservationList(const SBObservationList& List_): QList<SBObservation>(List_){};
  SBObservationList& operator= (const SBObservationList&);
  /**A destructor.*/
  virtual ~SBObservationList(){};
  /**Refers to a class name (debug info).*/
  virtual QString	ClassName()	const {return "SBObservationList";};

  // interface:
  //
  /**Returns number of observations (=count()).*/
  virtual int		numObs()	const {return count();};

  // functions:
  //
  
  // I/O:
  //
};
/*==============================================================================================*/


#endif //SB_GEO_OBS_H
