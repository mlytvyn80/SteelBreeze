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

#include "SbVector3.H"

const Vector3 v3Zero(0.0, 0.0, 0.0);
const Vector3 v3Unit(1.0, 1.0, 1.0);
const Matrix3 m3E(Vector3(1.0, 0.0, 0.0), Vector3(0.0, 1.0, 0.0), Vector3(0.0, 0.0, 1.0));





/*==============================================================================================*/
/*												*/
/* Vector3 implementation									*/
/*												*/
/*==============================================================================================*/
void Vector3::report2Log(SBLog::LogLevel Lev, uint Fac, const QString& Pref, 
			 int w_, int d_, double Scale) const
{
  QString tmp("tmp");
  tmp.sprintf("%*.*f, %*.*f, %*.*f [%*.*f]", 
	      w_, d_, (double)(Scale*vec[0]), 
	      w_, d_, (double)(Scale*vec[1]), 
	      w_, d_, (double)(Scale*vec[2]), 
	      w_, d_, (double)(Scale*module()));
  Log->write(Lev, Fac, Pref + tmp);
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* Matrix3 implementation									*/
/*												*/
/*==============================================================================================*/
Matrix3 operator* (const Matrix3& M1, const Matrix3& M2)
{
  Matrix3 M;
  M.mat[0][0] = M1.mat[0][0]*M2.mat[0][0] + M1.mat[0][1]*M2.mat[1][0] + M1.mat[0][2]*M2.mat[2][0];
  M.mat[0][1] = M1.mat[0][0]*M2.mat[0][1] + M1.mat[0][1]*M2.mat[1][1] + M1.mat[0][2]*M2.mat[2][1];
  M.mat[0][2] = M1.mat[0][0]*M2.mat[0][2] + M1.mat[0][1]*M2.mat[1][2] + M1.mat[0][2]*M2.mat[2][2];

  M.mat[1][0] = M1.mat[1][0]*M2.mat[0][0] + M1.mat[1][1]*M2.mat[1][0] + M1.mat[1][2]*M2.mat[2][0];
  M.mat[1][1] = M1.mat[1][0]*M2.mat[0][1] + M1.mat[1][1]*M2.mat[1][1] + M1.mat[1][2]*M2.mat[2][1];
  M.mat[1][2] = M1.mat[1][0]*M2.mat[0][2] + M1.mat[1][1]*M2.mat[1][2] + M1.mat[1][2]*M2.mat[2][2];

  M.mat[2][0] = M1.mat[2][0]*M2.mat[0][0] + M1.mat[2][1]*M2.mat[1][0] + M1.mat[2][2]*M2.mat[2][0];
  M.mat[2][1] = M1.mat[2][0]*M2.mat[0][1] + M1.mat[2][1]*M2.mat[1][1] + M1.mat[2][2]*M2.mat[2][1];
  M.mat[2][2] = M1.mat[2][0]*M2.mat[0][2] + M1.mat[2][1]*M2.mat[1][2] + M1.mat[2][2]*M2.mat[2][2];
  return M;
};

void Matrix3::report2Log(SBLog::LogLevel Lev, uint Fac, const QString& Pref) const
{
  QString tmp("tmp");
  tmp.sprintf("%23.20f  %23.20f  %23.20f", (double)mat[0][0], (double)mat[0][1], (double)mat[0][2]);
  Log->write(Lev, Fac, Pref + tmp);
  tmp.sprintf("%23.20f  %23.20f  %23.20f", (double)mat[1][0], (double)mat[1][1], (double)mat[1][2]);
  Log->write(Lev, Fac, Pref + tmp);
  tmp.sprintf("%23.20f  %23.20f  %23.20f", (double)mat[2][0], (double)mat[2][1], (double)mat[2][2]);
  Log->write(Lev, Fac, Pref + tmp);
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* Matrix3's friends:										*/
/*												*/
/*==============================================================================================*/
Matrix3 operator~ (const Matrix3& M1)
{
  Matrix3 M;
  double d = M1.module();
  if (d==0.0) 
    {
      Log->write(SBLog::ERR, SBLog::MATRIX3D, M1.ClassName() + 
		 ": unable to get inverse matrix, the determinant iz zero");
      return M;
    };
  M.mat[0][0] = (M1.mat[1][1]*M1.mat[2][2] - M1.mat[1][2]*M1.mat[2][1])/d;
  M.mat[0][1] = (M1.mat[2][1]*M1.mat[0][2] - M1.mat[0][1]*M1.mat[2][2])/d;
  M.mat[0][2] = (M1.mat[0][1]*M1.mat[1][2] - M1.mat[1][1]*M1.mat[0][2])/d;
  
  M.mat[1][0] = (M1.mat[2][0]*M1.mat[1][2] - M1.mat[1][0]*M1.mat[2][2])/d;
  M.mat[1][1] = (M1.mat[0][0]*M1.mat[2][2] - M1.mat[2][0]*M1.mat[0][2])/d;
  M.mat[1][2] = (M1.mat[1][0]*M1.mat[0][2] - M1.mat[0][0]*M1.mat[1][2])/d;
  
  M.mat[2][0] = (M1.mat[1][0]*M1.mat[2][1] - M1.mat[2][0]*M1.mat[1][1])/d;
  M.mat[2][1] = (M1.mat[2][0]*M1.mat[0][1] - M1.mat[0][0]*M1.mat[2][1])/d;
  M.mat[2][2] = (M1.mat[0][0]*M1.mat[1][1] - M1.mat[1][0]*M1.mat[0][1])/d;
  return M;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* RotMatrix implementation									*/
/*												*/
/*==============================================================================================*/
RotMatrix::RotMatrix (DIRECTION Axis_, double Angle_) : Matrix3(m3E)
{
  Axis  = Axis_;
  (*this)(Angle_);
};

RotMatrix::RotMatrix(const RotMatrix& RM) : Matrix3(RM)
{
  Axis  = RM.Axis;
  Angle = RM.Angle;
};

const Matrix3& RotMatrix::operator()(double Angle_)
{
  //  sincosl(Angle=Angle_, &sa, &ca);
  Angle=Angle_;
  sa = sinl(Angle);
  ca = cosl(Angle);
  switch (Axis)
    {
    case X_AXIS:
      mat[0][0]= 1.0;	mat[0][1]= 0.0;	mat[0][2]= 0.0;
      mat[1][0]= 0.0;	mat[1][1]= ca;	mat[1][2]= sa;
      mat[2][0]= 0.0;	mat[2][1]=-sa;	mat[2][2]= ca;
      break;
    case Y_AXIS:
      mat[0][0]= ca;	mat[0][1]= 0.0;	mat[0][2]=-sa;
      mat[1][0]= 0.0;	mat[1][1]= 1.0;	mat[1][2]= 0.0;
      mat[2][0]= sa;	mat[2][1]= 0.0;	mat[2][2]= ca;
      break;
    case Z_AXIS:
      mat[0][0]= ca;	mat[0][1]= sa;	mat[0][2]= 0.0;
      mat[1][0]=-sa;	mat[1][1]= ca;	mat[1][2]= 0.0;
      mat[2][0]= 0.0;	mat[2][1]= 0.0;	mat[2][2]= 1.0;
      break;
    };
  return *this;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* RotDerMatrix implementation									*/
/*												*/
/*==============================================================================================*/
const Matrix3& RotDerMatrix::operator()(double Angle_)
{
  sincos(Angle=Angle_, &sa, &ca);
  switch (Axis)
    {
    case X_AXIS:
      mat[0][0]= 0.0;	mat[0][1]= 0.0;	mat[0][2]= 0.0;
      mat[1][0]= 0.0;	mat[1][1]=-sa;	mat[1][2]= ca;
      mat[2][0]= 0.0;	mat[2][1]=-ca;	mat[2][2]=-sa;
      break;
    case Y_AXIS:
      mat[0][0]=-sa;	mat[0][1]= 0.0;	mat[0][2]=-ca;
      mat[1][0]= 0.0;	mat[1][1]= 0.0;	mat[1][2]= 0.0;
      mat[2][0]= ca;	mat[2][1]= 0.0;	mat[2][2]=-sa;
      break;
    case Z_AXIS:
      mat[0][0]=-sa;	mat[0][1]= ca;	mat[0][2]= 0.0;
      mat[1][0]=-ca;	mat[1][1]=-sa;	mat[1][2]= 0.0;
      mat[2][0]= 0.0;	mat[2][1]= 0.0;	mat[2][2]= 0.0;
      break;
    };
  return *this;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* Rot2DerMatrix implementation									*/
/*												*/
/*==============================================================================================*/
const Matrix3& Rot2DerMatrix::operator()(double Angle_)
{
  sincos(Angle=Angle_, &sa, &ca);
  switch (Axis)
    {
    case X_AXIS:
      mat[0][0]= 0.0;
			mat[1][1]=-ca;	mat[1][2]=-sa;
			mat[2][1]= sa;	mat[2][2]=-ca;
      break;
    case Y_AXIS:
      mat[0][0]=-ca;			mat[0][2]= sa;
			mat[1][1]= 0.0;
      mat[2][0]=-sa;			mat[2][2]=-ca;
      break;
    case Z_AXIS:
      mat[0][0]=-ca;	mat[0][1]=-sa;
      mat[1][0]= sa;	mat[1][1]=-ca;
					mat[2][2]= 0.0;
      break;
    };
  return *this;
};
/*==============================================================================================*/
