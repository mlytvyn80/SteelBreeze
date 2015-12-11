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

#ifndef SB_VECTOR3_H
#define SB_VECTOR3_H

#include <config.h>
#include <math.h>

#include "SbGeneral.H"



enum DIRECTION {X_AXIS=0, VERT=0, Y_AXIS=1, EAST=1, Z_AXIS=2, NORT=2};

class Vector3;
class Matrix3;
extern const Vector3 v3Zero;
extern const Vector3 v3Unit;
extern const Matrix3 m3E;



/*==============================================================================================*/
/**3d vector.
 *  
 *
 */
/*==============================================================================================*/
class Vector3
{
protected:
  double	vec[3];
  //  long double	vec[3];

public:
  /** A constructor.
   * Creates a copy of object.
   */
  Vector3(){vec[0]=vec[1]=vec[2]=0.0;};
  Vector3(double v1, double v2, double v3){vec[0]=v1; vec[1]=v2; vec[2]=v3;};
  /** A constructor.
   * Creates a copy of object.
   * \param V values of the new object.
   */
  Vector3(const Vector3& V) {vec[0]=V.vec[0]; vec[1]=V.vec[1]; vec[2]=V.vec[2];};
  /** A destructor. */
  virtual ~Vector3(){};
  /** Refers to a class name (debug info) */
  virtual QString ClassName() const {return "Vector3";};
  double& operator[](DIRECTION i){return vec[i];};
  double at (DIRECTION i) const {return vec[i];};
  double module() const {return sqrt(vec[0]*vec[0] + vec[1]*vec[1] + vec[2]*vec[2]);};
  double phi() const {double f=module(); return f==0.0?0.0:asin(vec[2]/module());};
  double lambda() const 
    {double f = module()==0.0?0.0:atan2(vec[1], vec[0]); return f<0.0?f+2.0*M_PI:f;};
  Vector3& unify(){return *this/=module();};
  Vector3  unit () const {return Vector3(*this)/module();};
  void clear(){vec[0]=vec[1]=vec[2]=0.0;};
  inline Vector3& operator= (const Vector3&);
  inline Vector3& operator+=(const Vector3&);
  inline Vector3& operator-=(const Vector3&);
  inline Vector3& operator*=(double);
  inline Vector3& operator/=(double);
  virtual void	  report2Log(SBLog::LogLevel, uint, const QString&, int, int, double=1.0) const;


  friend inline Vector3 operator-(const Vector3&);
  friend inline Vector3 operator+(const Vector3&, const Vector3&); 
  friend inline Vector3 operator-(const Vector3&, const Vector3&);
  friend inline Vector3 operator/(const Vector3&, double);
  friend inline Vector3 operator*(const Vector3&, double);
  friend inline Vector3 operator*(double, const Vector3&);
  /**  makes a scalar product of two vectors */ 
  friend inline double operator*(const Vector3&, const Vector3&);
  /**  makes a vector product of two vectors (because "%" has a priority of "*") */ 
  friend inline Vector3 operator%(const Vector3&, const Vector3&);
  /**  makes a product of matrix and vector */ 
  friend inline Vector3 operator*(const Matrix3&, const Vector3&);
  bool operator==(const Vector3& v) const 
    {return (vec[0]==v.vec[0]) && (vec[1]==v.vec[1]) && (vec[2]==v.vec[2]);};
  bool operator!=(const Vector3& v) const {return !(*this==v);};
  friend QDataStream &operator<<(QDataStream& s, const Vector3& v)
    {return s<<v.vec[0]<<v.vec[1]<<v.vec[2];};
  friend QDataStream &operator>>(QDataStream& s, Vector3& v)
    {return s>>v.vec[0]>>v.vec[1]>>v.vec[2];};
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class Matrix3.. just a 3-d matrix								*/
/*												*/
/*==============================================================================================*/
class Matrix3
{
protected:
  double	mat[3][3];
  //  long double	mat[3][3];

public:
  inline Matrix3();
  inline Matrix3(const Matrix3&);
  /**Creates object.  
   * Arguments are Vector3, three columns.
   */
  inline Matrix3(const Vector3& col0, const Vector3& col1, const Vector3& col2);
  /**Creates object.  
   * Arguments are doubles: first row, second row and third row.
   */
  inline Matrix3(double a00, double a01, double a02,
		 double a10, double a11, double a12,
		 double a20, double a21, double a22);
  virtual ~Matrix3(){};
  virtual QString ClassName() const {return "Matrix3";};
  virtual double& operator()(DIRECTION i, DIRECTION j) {return mat[i][j];};
  virtual double  at(DIRECTION i, DIRECTION j) const {return mat[i][j];};
  virtual void	  report2Log(SBLog::LogLevel, uint, const QString&) const;
  inline virtual double module() const;  /* determinant */
  inline virtual void   unify() {operator/=(module());};
  /**Returns transposed matrix (original matrix does'n change). */
  inline Matrix3 T() const; /*  transpose: */
  
  inline Matrix3& operator = (const Matrix3&);		       
  inline Matrix3& operator+= (const Matrix3&);
  inline Matrix3& operator-= (const Matrix3&);
  inline Matrix3& operator*= (double);
  inline Matrix3& operator/= (double);
  
  friend inline Matrix3  operator+  (const Matrix3&, const Matrix3&);
  friend inline Matrix3  operator-  (const Matrix3&, const Matrix3&);
  friend inline Matrix3  operator/  (const Matrix3&, double);
  friend inline Matrix3  operator*  (const Matrix3&, double);
  friend inline Matrix3  operator*  (double, const Matrix3&);
  friend inline Matrix3  operator-  (const Matrix3&);
  friend inline Vector3  operator*  (const Matrix3&, const Vector3&);
  friend        Matrix3  operator*  (const Matrix3&, const Matrix3&);
  /**Returns inversed matrix: A*~A=~A*A=1 (original matrix does'n change). */
  friend	Matrix3  operator~  (const Matrix3&);
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class RotMatrix.. rotation 3-d matrix							*/
/*												*/
/*==============================================================================================*/
class RotMatrix : public Matrix3
{
protected:
  double		 Angle, sa, ca;
  DIRECTION		 Axis;
public:
  RotMatrix(DIRECTION, double=0.0);
  RotMatrix(const RotMatrix&);
  virtual ~RotMatrix (){};
  virtual QString ClassName() const {return "RotMatrix";};
  virtual const Matrix3& operator()(double);
  virtual double angle() const {return Angle;};
  virtual DIRECTION axis() const {return Axis;};
  //  virtual const Matrix3& setAngle(double A_){return (*this)(A_);};
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class RotDerMatrix.. derivative of rotation 3-d matrix					*/
/*												*/
/*==============================================================================================*/
class RotDerMatrix : public RotMatrix
{
public:
  RotDerMatrix(DIRECTION Axis_, double Angle_=0.0) : RotMatrix(Axis_, Angle_){};
  virtual const Matrix3& operator()(double);
  virtual QString ClassName() const {return "RotDerMatrix";};
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* class Rot2DerMatrix.. derivative of rotation 3-d matrix					*/
/*												*/
/*==============================================================================================*/
class Rot2DerMatrix : public RotMatrix
{
public:
  Rot2DerMatrix(DIRECTION Axis_, double Angle_=0.0) : RotMatrix(Axis_, Angle_){};
  virtual const Matrix3& operator()(double);
  virtual QString ClassName() const {return "Rot2DerMatrix";};
};
/*==============================================================================================*/




/*==============================================================================================*/
/*												*/
/* Vector3's iniline members:									*/
/*												*/
/*==============================================================================================*/
inline Vector3& Vector3::operator=(const Vector3& V)
{
  vec[0]=V.vec[0];   vec[1]=V.vec[1];   vec[2]=V.vec[2]; 
  return *this;
};

inline Vector3& Vector3::operator+= (const Vector3& V)
{
  vec[0]+=V.vec[0];  vec[1]+=V.vec[1];  vec[2]+=V.vec[2];
  return *this;
};  

inline Vector3& Vector3::operator-= (const Vector3& V)
{
  vec[0]-=V.vec[0];  vec[1]-=V.vec[1];  vec[2]-=V.vec[2];
  return *this;
};  

inline Vector3& Vector3::operator*= (double v)
{
  vec[0]*=v;  vec[1]*=v;  vec[2]*=v;
  return *this;
};  

inline Vector3& Vector3::operator/= (double v)
{
  vec[0]/=v;  vec[1]/=v;  vec[2]/=v;
  return *this;
};  
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* Vector3's iniline friends:									*/
/*												*/
/*==============================================================================================*/
/**\relates Vector3
 * 
 */
inline Vector3 operator-(const Vector3& V)
{
  return Vector3(-V.vec[0], -V.vec[1], -V.vec[2]);
};

/**\relates Vector3
 * 
 */
inline Vector3 operator+ (const Vector3& V1, const Vector3& V2)
{
  return Vector3(V1.vec[0]+V2.vec[0], V1.vec[1]+V2.vec[1], V1.vec[2]+V2.vec[2]);
};

/**\relates Vector3
 * 
 */
inline Vector3 operator- (const Vector3& V1, const Vector3& V2)
{
  return Vector3(V1.vec[0]-V2.vec[0], V1.vec[1]-V2.vec[1], V1.vec[2]-V2.vec[2]);
};

/**\relates Vector3
 * 
 */
inline Vector3 operator/ (const Vector3& V1, double v2)
{
  return Vector3(V1.vec[0]/v2, V1.vec[1]/v2, V1.vec[2]/v2);
};

/**\relates Vector3
 * multiplies vector by scalar.
 */
inline Vector3 operator* (const Vector3& V1, double v2)
{
  return Vector3(V1.vec[0]*v2, V1.vec[1]*v2, V1.vec[2]*v2);
};

/**\relates Vector3
 * multiplies vector by scalar.
 */
inline Vector3 operator* (double v1, const Vector3& V2)
{
  return Vector3(v1*V2.vec[0], v1*V2.vec[1], v1*V2.vec[2]);
};

/**\relates Vector3
 * makes scalar product of two vectors.
 */
inline double  operator* (const Vector3& V1, const Vector3& V2)
{
  return (V1.vec[0]*V2.vec[0]+V1.vec[1]*V2.vec[1]+V1.vec[2]*V2.vec[2]);
};

/**\relates Vector3
 * makes vector product of two vectors.
 */
inline Vector3 operator% (const Vector3& V1, const Vector3& V2)
{
  return Vector3(V1.vec[1]*V2.vec[2] - V1.vec[2]*V2.vec[1] , 
		 V1.vec[2]*V2.vec[0] - V1.vec[0]*V2.vec[2] , 
		 V1.vec[0]*V2.vec[1] - V1.vec[1]*V2.vec[0]);
};
/*==============================================================================================*/


/*==============================================================================================*/
/*												*/
/* Matrix3's iniline members:									*/
/*												*/
/*==============================================================================================*/
inline Matrix3::Matrix3()
{
  mat[0][0]=1.0; mat[0][1]=0.0; mat[0][2]=0.0;
  mat[1][0]=0.0; mat[1][1]=1.0; mat[1][2]=0.0;
  mat[2][0]=0.0; mat[2][1]=0.0; mat[2][2]=1.0;      
};

inline Matrix3::Matrix3(const Vector3& col0, const Vector3& col1, const Vector3& col2)
{
  mat[0][0]=col0.at(X_AXIS); mat[0][1]=col1.at(X_AXIS); mat[0][2]=col2.at(X_AXIS);
  mat[1][0]=col0.at(Y_AXIS); mat[1][1]=col1.at(Y_AXIS); mat[1][2]=col2.at(Y_AXIS);
  mat[2][0]=col0.at(Z_AXIS); mat[2][1]=col1.at(Z_AXIS); mat[2][2]=col2.at(Z_AXIS);
};

inline Matrix3::Matrix3(double a00, double a01, double a02,
			double a10, double a11, double a12,
			double a20, double a21, double a22)
{
  mat[0][0]=a00; mat[0][1]=a01; mat[0][2]=a02;
  mat[1][0]=a10; mat[1][1]=a11; mat[1][2]=a12;
  mat[2][0]=a20; mat[2][1]=a21; mat[2][2]=a22;
};

inline Matrix3::Matrix3(const Matrix3& M)
{
  mat[0][0]=M.mat[0][0]; mat[0][1]=M.mat[0][1]; mat[0][2]=M.mat[0][2];
  mat[1][0]=M.mat[1][0]; mat[1][1]=M.mat[1][1]; mat[1][2]=M.mat[1][2];
  mat[2][0]=M.mat[2][0]; mat[2][1]=M.mat[2][1]; mat[2][2]=M.mat[2][2];
};

inline Matrix3& Matrix3::operator=(const Matrix3& M)
{
  mat[0][0]=M.mat[0][0]; mat[0][1]=M.mat[0][1]; mat[0][2]=M.mat[0][2];
  mat[1][0]=M.mat[1][0]; mat[1][1]=M.mat[1][1]; mat[1][2]=M.mat[1][2];
  mat[2][0]=M.mat[2][0]; mat[2][1]=M.mat[2][1]; mat[2][2]=M.mat[2][2];
  return *this;
};

inline Matrix3& Matrix3::operator+= (const Matrix3& M)
{
  mat[0][0]+=M.mat[0][0]; mat[0][1]+=M.mat[0][1]; mat[0][2]+=M.mat[0][2];
  mat[1][0]+=M.mat[1][0]; mat[1][1]+=M.mat[1][1]; mat[1][2]+=M.mat[1][2];
  mat[2][0]+=M.mat[2][0]; mat[2][1]+=M.mat[2][1]; mat[2][2]+=M.mat[2][2];
  return *this;
};  

inline Matrix3& Matrix3::operator-= (const Matrix3& M)
{
  mat[0][0]-=M.mat[0][0]; mat[0][1]-=M.mat[0][1]; mat[0][2]-=M.mat[0][2];
  mat[1][0]-=M.mat[1][0]; mat[1][1]-=M.mat[1][1]; mat[1][2]-=M.mat[1][2];
  mat[2][0]-=M.mat[2][0]; mat[2][1]-=M.mat[2][1]; mat[2][2]-=M.mat[2][2];
  return *this;
};   

inline Matrix3& Matrix3::operator*= (double v)
{
  mat[0][0]*=v; mat[0][1]*=v; mat[0][2]*=v;
  mat[1][0]*=v; mat[1][1]*=v; mat[1][2]*=v;
  mat[2][0]*=v; mat[2][1]*=v; mat[2][2]*=v;
  return *this;
};  

inline Matrix3& Matrix3::operator/= (double v)
{
  mat[0][0]/=v; mat[0][1]/=v; mat[0][2]/=v;
  mat[1][0]/=v; mat[1][1]/=v; mat[1][2]/=v;
  mat[2][0]/=v; mat[2][1]/=v; mat[2][2]/=v;
  return *this;
};  

inline double Matrix3::module() const 
{
  return
    mat[0][0]*(mat[1][1]*mat[2][2] - mat[1][2]*mat[2][1]) -
    mat[0][1]*(mat[1][0]*mat[2][2] - mat[1][2]*mat[2][0]) +
    mat[0][2]*(mat[1][0]*mat[2][1] - mat[1][1]*mat[2][0]) ;
};

inline Matrix3 Matrix3::T() const
{
  return Matrix3(mat[0][0], mat[1][0], mat[2][0],
		 mat[0][1], mat[1][1], mat[2][1],
		 mat[0][2], mat[1][2], mat[2][2]);
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* Matrix3's iniline friends:									*/
/*												*/
/*==============================================================================================*/
inline Matrix3 operator- (const Matrix3& M1)
{
  Matrix3 M;
  M.mat[0][0] = -M1.mat[0][0]; 
  M.mat[0][1] = -M1.mat[0][1]; 
  M.mat[0][2] = -M1.mat[0][2];
  
  M.mat[1][0] = -M1.mat[1][0]; 
  M.mat[1][1] = -M1.mat[1][1]; 
  M.mat[1][2] = -M1.mat[1][2];
  
  M.mat[2][0] = -M1.mat[2][0]; 
  M.mat[2][1] = -M1.mat[2][1]; 
  M.mat[2][2] = -M1.mat[2][2];
  return M;
};

inline Matrix3 operator+ (const Matrix3& M1, const Matrix3& M2)
{
  Matrix3 M(M1);
  M.mat[0][0]+= M2.mat[0][0]; 
  M.mat[0][1]+= M2.mat[0][1]; 
  M.mat[0][2]+= M2.mat[0][2];
    
  M.mat[1][0]+= M2.mat[1][0];
  M.mat[1][1]+= M2.mat[1][1];
  M.mat[1][2]+= M2.mat[1][2];
    
  M.mat[2][0]+= M2.mat[2][0];
  M.mat[2][1]+= M2.mat[2][1];
  M.mat[2][2]+= M2.mat[2][2];
  return M;
};

inline Matrix3 operator- (const Matrix3& M1, const Matrix3& M2)
{
  Matrix3 M(M1);
  M.mat[0][0]-= M2.mat[0][0];
  M.mat[0][1]-= M2.mat[0][1];
  M.mat[0][2]-= M2.mat[0][2];

  M.mat[1][0]-= M2.mat[1][0];
  M.mat[1][1]-= M2.mat[1][1];
  M.mat[1][2]-= M2.mat[1][2];

  M.mat[2][0]-= M2.mat[2][0];
  M.mat[2][1]-= M2.mat[2][1];
  M.mat[2][2]-= M2.mat[2][2];
  return M;
};

inline Matrix3 operator/ (const Matrix3& M1, double v2)
{
  Matrix3 M(M1);
  M.mat[0][0]/=v2;  M.mat[0][1]/=v2;  M.mat[0][2]/=v2;
  M.mat[1][0]/=v2;  M.mat[1][1]/=v2;  M.mat[1][2]/=v2;
  M.mat[2][0]/=v2;  M.mat[2][1]/=v2;  M.mat[2][2]/=v2;
  return M;
};

inline Matrix3 operator* (const Matrix3& M1, double v2)
{
  Matrix3 M(M1);
  M.mat[0][0]*=v2;  M.mat[0][1]*=v2;  M.mat[0][2]*=v2;
  M.mat[1][0]*=v2;  M.mat[1][1]*=v2;  M.mat[1][2]*=v2;
  M.mat[2][0]*=v2;  M.mat[2][1]*=v2;  M.mat[2][2]*=v2;
  return M;
};

inline Matrix3 operator* (double v1, const Matrix3& M2)
{
  Matrix3 M(M2*v1);
  return M;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* Vector3's and Matrix3's iniline friends:							*/
/*												*/
/*==============================================================================================*/
/**\relates Vector3 Matrix3
 * makes product of matrix and vector.
 */
inline Vector3 operator* (const Matrix3& M, const Vector3& V)
{
  return 
    Vector3(M.mat[0][0]*V.vec[0]+M.mat[0][1]*V.vec[1]+M.mat[0][2]*V.vec[2],
	    M.mat[1][0]*V.vec[0]+M.mat[1][1]*V.vec[1]+M.mat[1][2]*V.vec[2],
	    M.mat[2][0]*V.vec[0]+M.mat[2][1]*V.vec[1]+M.mat[2][2]*V.vec[2]);
};
/*==============================================================================================*/



#endif //SB_VECTOR3_H
