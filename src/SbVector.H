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

#ifndef SB_VECTOR_H
#define SB_VECTOR_H

#include <config.h>
#include <math.h>
#include <string.h>

#include "SbGeneral.H"
#include "SbGeo.H"

//check for mem* functions:
#ifndef HAVE_MEMSET
#error: "need right C-Library: Your C-Library doesn't have memset() function"
#endif //HAVE_MEMSET

#ifndef HAVE_MEMCPY
#error: "need right C-Library: Your C-Library doesn't have memcpy() function"
#endif //HAVE_MEMCPY



class SBMatrix;
class SBUpperMatrix;
class SBSymMatrix;

/*==============================================================================================*/
/**class Vector.
 *  
 *
 */
/*==============================================================================================*/
class SBVector : public SBNamed
{
  friend class SBMatrix;
  friend class SBUpperMatrix;
protected:
  unsigned int	N;
  double	*B;

public:
  // constructors/destructors:
  //
  /**A constructor.
   * Creates an empty vector (for I/O purposes).
   */
  SBVector() : SBNamed("Temporary Vector") {B=NULL; N=0;};
  /**A constructor.
   * Creates a copy of object.
   * \param N number of elements.
   */
  SBVector(unsigned int N_, const QString& Name_, bool IsNeedClear=TRUE)
    : SBNamed(Name_)
    {B=new double[N=N_]; if (IsNeedClear) clear();};
  /**A constructor.
   * Creates a copy of object.
   * \param V values of the new object.
   */
  SBVector(const SBVector& V)
    : SBNamed(V.name())
    {B=new double[N=V.N]; memcpy((void*)B, (const void*)V.B, N*sizeof(double));};
  /**A destructor. */
  virtual ~SBVector(){if (B) delete[] B; B = NULL;};
  /**Refers to a class name (debug info).*/
  virtual QString ClassName() const {return "SBVector";};

  // interface:
  //
  /**Returns number of elements.*/
  inline unsigned int n() const {return N;};
  /**Returns the reference to i-th element.*/
  inline double& operator()(unsigned int);
  /**Sets the i-th element to the double.*/
  inline void set(unsigned int, double);
  /**Returns value of i-th element.*/
  inline double at (unsigned int) const;

  // functions:
  //
  /**Returns vector's module.*/
  inline double module() const;
  /**Unifies vector and returns reference to it.*/
  SBVector& unify(){return *this/=module();};
  /**Returns value of unified vector.*/
  SBVector  unit () const {return SBVector(*this)/module();};
  /**Fills elements with zero.*/
  void clear() {memset((void*)B, 0, sizeof(double)*N);};
  /**Assign the vector to another one.*/
  inline SBVector& operator= (const SBVector&);
  /**Fills the vector with a double.*/
  inline SBVector& operator= (double);
  /**Increments vector by another one.*/
  inline SBVector& operator+=(const SBVector&);
  /**Decrements vector by another one.*/
  inline SBVector& operator-=(const SBVector&);
  /**Multiplies vector by scalar.*/
  inline SBVector& operator*=(double);
  /**Divides vector by scalar.*/
  inline SBVector& operator/=(double);
  /**Comperes vector with another one, returns TRUE if values are equal.*/
  inline bool operator==(const SBVector&) const;
  /**Comperes vector with another one, returns TRUE if values are different.*/
  inline bool operator!=(const SBVector& V) const {return !(*this==V);};
  
  // friends:
  //
  friend inline SBVector operator-(const SBVector& V) {return SBVector(V)*=-1.0;};
  friend inline SBVector operator/(const SBVector& V, double f) {return SBVector(V)/=f;};
  friend inline SBVector operator*(const SBVector& V, double f) {return SBVector(V)*=f;};
  friend inline SBVector operator*(double f, const SBVector& V) {return SBVector(V)*=f;};
  friend inline SBVector operator+(const SBVector&, const SBVector&);
  friend inline SBVector operator-(const SBVector&, const SBVector&);
  friend inline double operator*(const SBVector&, const SBVector&);
  friend SBVector operator*(const SBMatrix&, const SBVector&);
  friend SBVector matT_x_vec(const SBMatrix&, const SBVector&);
  friend SBVector matT_x_vec(const SBUpperMatrix&, const SBVector&);
  friend inline SBVector retVector(unsigned int, const SBVector&);
  friend SBVector& Solve(const SBUpperMatrix&, SBVector&, const SBVector&);
  
  // I/O:
  //
  friend QDataStream &operator<<(QDataStream&, const SBVector&);
  friend QDataStream &operator>>(QDataStream&, SBVector&);
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBVector's inlines:										*/
/*												*/
/*==============================================================================================*/
double& SBVector::operator()(unsigned int i)
{
#ifdef DEBUG
  if (N<=i) 
    Log->write(SBLog::ERR, SBLog::MATRIX, ClassName() + "::operator(): index [" + QString().setNum(i) + 
	       "] out of range [0.." + QString().setNum(N-1) + "] for vector " + name());
#endif //DEBUG
  return (i<N)?*(B+i):*B;
};

void SBVector::set(unsigned int i, double d_)
{
#ifdef DEBUG
  if (N<=i) 
    Log->write(SBLog::ERR, SBLog::MATRIX, ClassName() + "::set() index [" + QString().setNum(i) + 
	       "] out of range [0.." + QString().setNum(N-1) + "] for vector " + name());
#endif //DEBUG
  if (i<N)
    *(B+i) = d_;
};

double SBVector::at(unsigned int i) const
{
#ifdef DEBUG
  if (N<=i) 
    Log->write(SBLog::ERR, SBLog::MATRIX, ClassName() + "::at() index [" + QString().setNum(i) + 
	       "] out of range [0.." + QString().setNum(N-1) + "] for vector " + name());
#endif //DEBUG
  return (i<N)?*(B+i):0.0;
};

double SBVector::module() const 
{
  double f=0.0, *w=B;
  for(unsigned int i=0; i<N; i++,w++)f+=*w**w;
  return sqrt(f);
};

SBVector& SBVector::operator= (const SBVector& V)
{
#ifdef DEBUG
  if (N!=V.N) 
    Log->write(SBLog::ERR, SBLog::MATRIX, ClassName() + "::operator=(): ranges of vector " +
	       name() + " and " + V.name() + " are different");
#endif //DEBUG
  clear(); 
  memcpy((void*)B, (const void*)V.B, std::min(N,V.N)*sizeof(double));
  return *this;
};

SBVector& SBVector::operator= (double f)
{
  double *b=B;
  for(unsigned int i=0; i<N; i++)*b++=f;
  return *this;
};

SBVector& SBVector::operator+=(const SBVector& V)
{
#ifdef DEBUG
  if (N!=V.N) 
    Log->write(SBLog::ERR, SBLog::MATRIX, ClassName() + "::operator+=(): ranges of vector " +
	       name() + " and " + V.name() + " are different");
#endif //DEBUG
  double *b=B, *v=V.B;
  unsigned int N_=std::min(N,V.N);
  for(unsigned int i=0; i<N_; i++)*b+++=*v++;
  return *this;
};

SBVector& SBVector::operator-=(const SBVector& V)
{
#ifdef DEBUG
  if (N!=V.N) 
    Log->write(SBLog::ERR, SBLog::MATRIX, ClassName() + "::operator-=(): ranges of vector " +
	       name() + " and " + V.name() + " are different");
#endif //DEBUG
  double *b=B, *v=V.B;
  unsigned int N_=std::min(N,V.N);
  for(unsigned int i=0; i<N_; i++)*b++-=*v++;
  return *this;
};

SBVector& SBVector::operator*=(double f)
{
  double *b=B;
  for(unsigned int i=0; i<N; i++)*b++*=f;
  return *this;
};

SBVector& SBVector::operator/=(double f)
{
  double *b=B;
  for(unsigned int i=0; i<N; i++)*b++/=f;
  return *this;
};

bool SBVector::operator==(const SBVector& V) const
{
  bool	Is = TRUE;
#ifdef DEBUG
  if (N!=V.N) 
    Log->write(SBLog::ERR, SBLog::MATRIX, ClassName() + "::operator==(): ranges of vector" +
	       name() + " and " + V.name() + " are different");
#endif //DEBUG
  if (N!=V.N) return !Is;
  double *b=B, *v=V.B;
  unsigned int i=0;
  while (i++<N && (Is = Is && *b++==*v++)) ;
  return Is;
};
/*==============================================================================================*/



/*==============================================================================================*/
/**class SBMatrix.. just a matrix.
 *
 */
/*==============================================================================================*/
class SBMatrix : public SBNamed
{
  friend class SBUpperMatrix;
protected:
  unsigned int	NRow;
  unsigned int	NCol;
  double	**B;
  double	dTmp;
  
  /**A constructor.
   * Creates an empty matrix (for inheritated objects).
   */
  SBMatrix(const QString& Name_) : SBNamed(Name_) {B=NULL; NCol=NRow=0; dTmp=0.0;};

public:
  // constructors/destructors:
  //
  /**A constructor.
   * Creates an empty matrix (for I/O purposes).
   */
  SBMatrix() : SBNamed("Temporary matrix") {B=NULL; NCol=NRow=0; dTmp=0.0;};
  /**A constructor.
   * Creates a copy of object.
   * \param NRow_ number of rows;
   * \param NCol_ number of columns.
   */
  inline SBMatrix(unsigned int, unsigned int, const QString&, bool = TRUE);
  /**A constructor.
   * Creates a copy of object.
   * \param M values of the new object.
   */
  inline SBMatrix(const SBMatrix&);
  /**A destructor.*/
  inline virtual ~SBMatrix();
  /**Refers to a class name (debug info).*/
  virtual QString ClassName() const {return "SBMatrix";};

  // interface:
  //
  /**Returns number of rows.*/
  virtual unsigned int nRow() const {return NRow;};
  /**Returns number of columns.*/
  virtual unsigned int nCol() const {return NCol;};
  /**Returns reference to the (i,j)-th element.*/
  inline virtual double& operator()(unsigned int, unsigned int);
  /**Returns value of the (i,j)-th element.*/
  inline virtual double at(unsigned int, unsigned int) const;
  /**Sets the (i,j)-th element to the double.*/
  inline virtual void set(unsigned int, unsigned int, double);
  /**Assign the matix to another one.*/
  inline SBMatrix& operator= (const SBMatrix&);
  /**Fills the matix with the double.*/
  inline virtual SBMatrix& operator= (double);
  /**Sets i-th column to a Vector.*/
  inline SBMatrix& setCol(unsigned int, const SBVector&);
  /**Assign to the vector the values of matrix'es column.*/
  inline SBVector& setVector(unsigned int, SBVector&);

  // functions:
  //
  /**Fills matrix with zeros.*/
  inline virtual void clear();
  /**Returns transposed matrix (original matrix doesn't change).*/
  inline virtual SBMatrix T();
  /**Adds a matrix to the matrix.*/
  inline virtual SBMatrix& operator+= (const SBMatrix&);
  /**Substracts a matrix from the matrix.*/
  inline virtual SBMatrix& operator-= (const SBMatrix&);
  /**Multiplies the matrix by a double.*/
  inline SBMatrix& operator*= (double);
  /**Divides the matrix by a double.*/
  inline SBMatrix& operator/= (double);
  /**Changes the size of a matrix.*/
  void resize(unsigned int, unsigned int);

  // friends:
  //
  /**Returns matrix divided by float.*/
  friend SBMatrix operator/ (const SBMatrix& M, double f){return SBMatrix(M)/=f;};
  /**Returns matrix multiplied by float.*/
  friend SBMatrix operator* (const SBMatrix& M, double f){return SBMatrix(M)*=f;};
  /**Returns matrix multiplied by float.*/
  friend SBMatrix operator* (double f, const SBMatrix& M){return SBMatrix(M)*=f;};
  /**Returns matrix with reversed sign.*/
  friend SBMatrix operator- (const SBMatrix& M) {return SBMatrix(M)*=-1.0;};
  
  /**Returns a sum of two matrices.*/
  friend inline SBMatrix  operator+ (const SBMatrix&, const SBMatrix&);
  /**Returns a difference of two matrices.*/
  friend inline SBMatrix  operator- (const SBMatrix&, const SBMatrix&);
  /**Returns product of two matrices.*/
  friend SBMatrix  operator*(const SBMatrix&, const SBMatrix&);

  friend SBSymMatrix QuadraticForm(const SBSymMatrix&, const SBMatrix&);

private: // have to reimplement it in future...
  /**Returns inversed matrix.
   * This is time consumed operation, shouldn't use in ordinary operations.
   * M*~M==~M*M==1 (original matrix doesn't change). 
   */
  SBMatrix operator~();
  SBMatrix operator*(const SBSymMatrix&){return SBMatrix();};

public:
  /**Returns a product of a transposed matrix and a matrix.
   * This function is equal to `M1.T()*M2', but implements a more efficient realization.
   */
  friend SBMatrix matT_x_mat(const SBMatrix&, const SBMatrix&);
  friend SBVector operator*(const SBMatrix&, const SBVector&);
  friend SBVector matT_x_vec(const SBMatrix&, const SBVector&);
  //  friend SBSymMatrix matT_x_mat(const SBUpperMatrix&, const SBUpperMatrix&);

  // I/O:
  //
  friend QDataStream &operator<<(QDataStream&, const SBMatrix&);
  friend QDataStream &operator>>(QDataStream&, SBMatrix&);
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBMatrix's inlines:										*/
/*												*/
/*==============================================================================================*/
SBMatrix::SBMatrix(unsigned int NRow_, unsigned int NCol_, const QString& Name_, bool IsNeedClear)
  : SBNamed(Name_)
{
  NRow=NRow_;
  NCol=NCol_;
  dTmp=0.0;
  B=new double*[NCol];
  double **w=B;
  for (unsigned int i=0; i<NCol; i++, w++)
    {
      *w=new double[NRow];
      if (IsNeedClear) memset((void*)(*w), 0, sizeof(double)*NRow);
    };
};

SBMatrix::SBMatrix(const SBMatrix& M)
  : SBNamed(M.name())
{
  NRow=M.NRow;
  dTmp=0.0;
  B=new double*[NCol=M.NCol];
  double **w=B, **q=M.B;
  for (unsigned int i=0; i<NCol; i++,w++,q++)
    {
      *w=new double[NRow];
      memcpy((void*)(*w), (const void*)(*q), NRow*sizeof(double));
    };
};

SBMatrix::~SBMatrix()
{
  if (B) 
    {
      double **w=B;
      for (unsigned int i=0; i<NCol; i++,w++) delete[] *w;
      delete[] B;
      B = NULL;
    };
};

void SBMatrix::clear()
{
  double **w=B;
  for (unsigned int i=0; i<NCol; i++, w++)
    memset((void*)(*w), 0, sizeof(double)*NRow);
};

double& SBMatrix::operator()(unsigned int i, unsigned int j) 
{
#ifdef DEBUG
  if (NRow<=i) 
    Log->write(SBLog::ERR, SBLog::MATRIX, ClassName() + "::operator(): row's index [" + 
	       QString().setNum(i) + "] out of range [0.." + QString().setNum(NRow-1) +
	       "] for " + name());
  if (NCol<=j) 
    Log->write(SBLog::ERR, SBLog::MATRIX, ClassName() + "::operator(): column's index [" + 
	       QString().setNum(j) + "] out of range [0.." + QString().setNum(NCol-1) +
	       "] for " + name());
#endif //DEBUG
  return (i<NRow && j<NCol)?*(*(B+j)+i):dTmp;
};

double SBMatrix::at(unsigned int i, unsigned int j) const 
{
#ifdef DEBUG
  if (NRow<=i) 
    Log->write(SBLog::ERR, SBLog::MATRIX, ClassName() + "::at(): row's index [" + QString().setNum(i) + 
	       "] out of range [0.." + QString().setNum(NRow-1) + "] for " + name());
  if (NCol<=j) 
    Log->write(SBLog::ERR, SBLog::MATRIX, ClassName() + "::at(): column's index [" + QString().setNum(j) + 
	       "] out of range [0.." + QString().setNum(NCol-1) + "] for " + name());
#endif //DEBUG
  return (i<NRow && j<NCol)?*(*(B+j)+i):0.0;
};

void SBMatrix::set(unsigned int i, unsigned int j, double A_)
{
#ifdef DEBUG
  if (NRow<=i) 
    Log->write(SBLog::ERR, SBLog::MATRIX, ClassName() + "::set(): row's index [" + QString().setNum(i) + 
	       "] out of range [0.." + QString().setNum(NRow-1) + "] for " + name());
  if (NCol<=j) 
    Log->write(SBLog::ERR, SBLog::MATRIX, ClassName() + "::set(): column's index [" + QString().setNum(j) +
	       "] out of range [0.." + QString().setNum(NCol-1) + "] for " + name());
#endif //DEBUG
  if (i<NRow && j<NCol)
    *(*(B+j)+i) = A_;
};

SBMatrix& SBMatrix::operator = (const SBMatrix& M)
{
#ifdef DEBUG
  if (NRow!=M.NRow) 
    Log->write(SBLog::ERR, SBLog::MATRIX, ClassName() + "::operator=(): ranges of matrix " +
	       name() + " and " + M.name() + " are different: NRow mismatch");
  if (NCol!=M.NCol) 
    Log->write(SBLog::ERR, SBLog::MATRIX, ClassName() + "::operator=(): ranges of matrix " +
	       name() + " and " + M.name() + " are different: NCol mismatch");
#endif //DEBUG
  double **w=B, **q=M.B;
  unsigned int NCol_=std::min(NCol,M.NCol), NRow_=std::min(NRow,M.NRow);
  for (unsigned int i=0; i<NCol_; i++,w++,q++)
    memcpy((void*)(*w), (const void*)(*q), NRow_*sizeof(double));
  return *this;
};

SBMatrix& SBMatrix::operator = (double f)
{
  double	**w=B, *ww;
  unsigned int	i, j;
  for (i=0; i<NCol; i++,w++)
    for (ww=*w,j=0; i<NRow; j++,w++) *ww++=f;
  return *this;
};

SBMatrix& SBMatrix::operator+= (const SBMatrix& M)
{
#ifdef DEBUG
  if (NRow!=M.NRow) 
    Log->write(SBLog::ERR, SBLog::MATRIX, ClassName() + "::operator+=(): ranges of matrix " +
	       name() + " and " + M.name() + " are different: NRow mismatch");
  if (NCol!=M.NCol) 
    Log->write(SBLog::ERR, SBLog::MATRIX, ClassName() + "::operator+=(): ranges of matrix " +
	       name() + " and " + M.name() + " are different: NCol mismatch");
#endif //DEBUG
  double	**w=B, **q=M.B, *ww, *qq;
  unsigned int	NCol_=std::min(NCol,M.NCol), NRow_=std::min(NRow,M.NRow), i, j;
  for (i=0; i<NCol_; i++,w++,q++)
    for (ww=*w,qq=*q,j=0; j<NRow_; j++) *ww+++=*qq++;
  return *this;
};

SBMatrix& SBMatrix::operator-= (const SBMatrix& M)
{
#ifdef DEBUG
  if (NRow!=M.NRow) 
    Log->write(SBLog::ERR, SBLog::MATRIX, ClassName() + "::operator-=(): ranges of matrix " +
	       name() + " and " + M.name() + " are different: NRow mismatch");
  if (NCol!=M.NCol) 
    Log->write(SBLog::ERR, SBLog::MATRIX, ClassName() + "::operator-=(): ranges of matrix " +
	       name() + " and " + M.name() + " are different: NCol mismatch");
#endif //DEBUG
  double	**w=B, **q=M.B, *ww, *qq;
  unsigned int	NCol_=std::min(NCol,M.NCol), NRow_=std::min(NRow,M.NRow), i, j;
  for (i=0; i<NCol_; i++,w++,q++)
    for (ww=*w,qq=*q,j=0; j<NRow_; j++)*ww++-=*qq++;
  return *this;
};

SBMatrix& SBMatrix::operator*= (double f)
{
  double	**w=B, *ww;
  unsigned int	i, j;
  for (i=0; i<NCol; i++,w++)
    for (ww=*w,j=0; j<NRow; j++)*ww++*=f;
  return *this;
};

SBMatrix& SBMatrix::operator/= (double f)
{
  double	**w=B, *ww;
  unsigned int	i, j;
  for (i=0; i<NCol; i++,w++)
    for (ww=*w,j=0; j<NRow; j++)*ww++/=f;
  return *this;
};

SBMatrix SBMatrix::T()
{
  SBMatrix	M(NCol, NRow, "Transposed " + name(), FALSE);
  double	**a=M.B, *aa;
  unsigned int	i, j;
  for (j=0; j<M.NCol; j++,a++)
    for (aa=*a,i=0; i<M.NRow; i++,aa++) *aa= *(*(B+i)+j);
  return M;
};

SBMatrix& SBMatrix::setCol(unsigned int N_, const SBVector& V)
{
#ifdef DEBUG
  if (NCol>N_) 
    Log->write(SBLog::ERR, SBLog::MATRIX, ClassName() + "::setCol: index out of range for " + name());
  if (NRow!=V.N) 
    Log->write(SBLog::ERR, SBLog::MATRIX, ClassName() + "::setCol: matrix " + name() +
	       " and vector " + V.name() + " have different ranges");
#endif //DEBUG
  double	*ww=*(B+(std::min(NCol,N_))), *v=V.B;
  unsigned int	N=std::min(NRow,V.N), i;
  for (i=0; i<N; i++) *ww++=*v++;
  return *this;
};

SBVector& SBMatrix::setVector(unsigned int N_, SBVector& V)
{
#ifdef DEBUG
  if (NCol>N_) 
    Log->write(SBLog::ERR, SBLog::MATRIX, ClassName() + "::setVector: index out of range for " + name());
  if (NRow!=V.N) 
    Log->write(SBLog::ERR, SBLog::MATRIX, ClassName() + "::setVector: matrix " + name() + 
	       " and vector " + V.name() + " have different ranges");
#endif //DEBUG
  double	*ww=*(B+std::min(NCol,N_)), *v=V.B;
  unsigned int	N=std::min(NRow,V.N), i;
  for (i=0; i<N; i++) *v++=*ww++;
  return V;
};
/*==============================================================================================*/



/*==============================================================================================*/
/**class SBUpperMatrix.
 * Upper triangular matrix.
 *
 */
/*==============================================================================================*/
class SBUpperMatrix : public SBMatrix
{
public:
  // constructors/destructors:
  //
  /**A constructor.
   * Creates an empty matrix (for I/O purposes).
   */
  inline SBUpperMatrix() : SBMatrix(){};
  /**A constructor.
   * Creates a copy of object.
   * \param N number of rows and columns;
   * \param IsNeedClear makes clearing of the allocated memmory;
   */
  inline SBUpperMatrix(unsigned int, const QString&, bool = TRUE);
  /**A constructor.
   * Creates a copy of object.
   * \param M values of the new object.
   */
  inline SBUpperMatrix(const SBUpperMatrix&);
  /**A destructor. */
  inline virtual ~SBUpperMatrix();
  /**Refers to a class name (debug info) */
  virtual QString ClassName() const {return "SBUpperMatrix";};
  
  // interface:
  //
  /**Returns the number of rows/columns of the matrix.*/
  unsigned int n() const {return nRow();};
  /**Returns reference to the (i,j)-th element.*/
  inline virtual double& operator()(unsigned int, unsigned int);
  /**Returns value of the (i,j)-th element.*/
  inline virtual double at(unsigned int, unsigned int) const;
  /**Sets the (i,j)-th element to the double.*/
  inline virtual void set(unsigned int, unsigned int, double);
  /**Assign the matix to another one*/
  inline SBUpperMatrix& operator = (const SBUpperMatrix&);
  /**Sets i-th column to Vector.*/
  inline virtual SBUpperMatrix& setCol(unsigned int, const SBVector&);
  /**Assign to the vector the values of matrix'es column.*/
  inline virtual SBVector& setVector(unsigned int, SBVector&);
  
  // functions:
  //
  /**Fills the matrix with zeros.*/
  inline virtual void clear();
  /**Returns transposed matrix (original matrix doesn't change).*/
  inline virtual SBMatrix T();
  /**Fills the matix with the double.*/
  inline virtual SBUpperMatrix& operator= (double);
  /**Adds a matrix to the matrix.*/
  inline virtual SBUpperMatrix& operator+= (const SBUpperMatrix&);
  /**Substracts a matrix from the matrix.*/
  inline virtual SBUpperMatrix& operator-= (const SBUpperMatrix&);
  /**Multiplies the matrix by a double.*/
  inline virtual SBUpperMatrix& operator*= (double);
  /**Divides the matrix by a double.*/
  inline virtual SBUpperMatrix& operator/= (double);

  // friends:
  //
  /**Returns matrix divided by float.*/
  friend SBUpperMatrix operator/ (const SBUpperMatrix& M, double f){return SBUpperMatrix(M)/=f;};
  /**Returns matrix multiplied by float.*/
  friend SBUpperMatrix operator* (const SBUpperMatrix& M, double f){return SBUpperMatrix(M)*=f;};
  /**Returns matrix multiplied by float.*/
  friend SBUpperMatrix operator* (double f, const SBUpperMatrix& M){return SBUpperMatrix(M)*=f;};
  /**Returns matrix with reversed sign.*/
  friend SBUpperMatrix operator- (const SBUpperMatrix& M) {return SBUpperMatrix(M)*=-1.0;};
  /**Returns a sum of two matrices.*/
  friend inline SBUpperMatrix operator+ (const SBUpperMatrix&, const SBUpperMatrix&);
  /**Returns a difference of two matrices.*/
  friend inline SBUpperMatrix operator- (const SBUpperMatrix&, const SBUpperMatrix&);
  /**Returns product of two matrices.*/
  friend SBUpperMatrix operator*(const SBUpperMatrix&, const SBUpperMatrix&);
  /**Returns inversed matrix.
   * This is time consumed operation, shouldn't use in ordinary operations.
   * M*~M==~M*M==1 (original matrix doesn't change). 
   */
  friend SBUpperMatrix operator~(const SBUpperMatrix&);
  /**Returns a product of a transposed matrix and a matrix.
   * This function is equal to `M1.T()*M2', but implements a more efficient realization.
   */
  friend SBSymMatrix matT_x_mat(const SBUpperMatrix&, const SBUpperMatrix&);
  friend SBVector operator* (const SBUpperMatrix&, const SBVector&);
  friend SBVector matT_x_vec(const SBUpperMatrix&, const SBVector&);

  // usefull operations:
  /**Returns a product of a `R*R.T()' -- a matrix and its transposed one.
   * This function is equal to `M1*M1.T()', but [should] implements a more efficient realization.
   */
  friend SBSymMatrix& RRT(SBSymMatrix&, const SBUpperMatrix&);
  /**Returns a product of a `R.T()*R' -- a transposed matrix and the one.
   * This function is equal to `M1.T()*M1', but [should] implements a more efficient realization.
   */
  friend SBSymMatrix& RTR(SBSymMatrix&, const SBUpperMatrix&);
  /**Solves an equation `R*x=z'. Returns vector x.
   * The order of arguments is the same as in the equation: R, x and z.
   */
  friend SBVector& Solve(const SBUpperMatrix&, SBVector&, const SBVector&);
  /**Calculates the product of `A*P*A.T()'. Returns symmetric matrix.
   */
  friend SBSymMatrix QuadraticForm(const SBSymMatrix&, const SBUpperMatrix&);

  // I/O:
  //
  friend QDataStream &operator<<(QDataStream&, const SBUpperMatrix&);
  friend QDataStream &operator>>(QDataStream&, SBUpperMatrix&);
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBUpperMatrix's inlines:									*/
/*												*/
/*==============================================================================================*/
SBUpperMatrix::SBUpperMatrix(unsigned int N_, const QString& Name_, bool IsNeedClear)
  : SBMatrix(Name_)
{
  NRow=N_;
  dTmp=0.0;
  B=new double*[NCol=N_];
  double **w=B;
  for (unsigned int i=0; i<NCol; i++, w++)
    {
      *w=new double[i+1];
      if (IsNeedClear) memset((void*)(*w), 0, sizeof(double)*(i+1));
    };
};

SBUpperMatrix::SBUpperMatrix(const SBUpperMatrix& M)
  : SBMatrix(M.name())
{
  NRow=M.NRow;
  dTmp=0.0;
  B=new double*[NCol=M.NCol];
  double **w=B, **q=M.B;
  for (unsigned int i=0; i<NCol; i++,w++,q++)
    {
      *w=new double[i+1];
      memcpy((void*)(*w), (const void*)(*q), (i+1)*sizeof(double));
    };
};

SBUpperMatrix::~SBUpperMatrix()
{
  if (B) 
    {
      double **w=B;
      for (unsigned int i=0; i<NCol; i++,w++) delete[] *w;
      delete[] B;
      B = NULL;
    };
};

void SBUpperMatrix::clear()
{
  double **w=B;
  for (unsigned int i=0; i<NCol; i++, w++)
    memset((void*)(*w), 0, sizeof(double)*(i+1));
};

double& SBUpperMatrix::operator()(unsigned int i, unsigned int j) 
{
#ifdef DEBUG
  if (i>j||NRow<=i)
    Log->write(SBLog::ERR, SBLog::MATRIX, ClassName() + "::operator(): row's index [" + 
	       QString().setNum(i) + "] out of range [0.." + QString().setNum(NRow-1) + 
	       "] for matrix " + name());
  if (i>j||NCol<=j)
    Log->write(SBLog::ERR, SBLog::MATRIX, ClassName() + "::operator(): column's index ["
	       + QString().setNum(j) + "] out of range [0.." + QString().setNum(NCol-1) +
	       "] for matrix " + name());
#endif //DEBUG
  return (i<=j && j<NCol)?*(*(B+j)+i):dTmp;
};

double SBUpperMatrix::at(unsigned int i, unsigned int j) const 
{
#ifdef DEBUG
  if (NRow<=i) Log->write(SBLog::ERR, SBLog::MATRIX, ClassName() + "::at(): row's index [" 
			  + QString().setNum(i) + "] out of range for matrix " + name());
  if (NCol<=j) Log->write(SBLog::ERR, SBLog::MATRIX, ClassName() + "::at(): column's index [" 
			  + QString().setNum(j) + "] out of range for matrix " + name());
#endif //DEBUG
  return (i<=j && j<NCol)?*(*(B+j)+i):0.0;
};

void SBUpperMatrix::set(unsigned int i, unsigned int j, double A_)
{
#ifdef DEBUG
  if (i>j || NRow<=i) Log->write(SBLog::ERR, SBLog::MATRIX, ClassName() + "::set(): row's index [" 
				 + QString().setNum(i) + "] out of range for matrix " + name());
  if (i>j || NCol<=j) Log->write(SBLog::ERR, SBLog::MATRIX, ClassName() + "::set(): column's index [" 
				 + QString().setNum(j) + "] out of range for matrix " + name());
#endif //DEBUG
  if (i<=j && j<NCol)
    *(*(B+j)+i) = A_;
};

SBMatrix SBUpperMatrix::T()
{
  SBMatrix	M(NCol,NRow, "Transposed " + name(), FALSE);
  double	**a=M.B, *aa;
  unsigned int	i, j;
  for (j=0; j<M.NCol; j++,a++)
    for (aa=*a,i=0; i<M.NRow; i++,aa++) *aa= i<=j?*(*(B+i)+j):0.0;
  return M;
};

SBUpperMatrix& SBUpperMatrix::operator= (const SBUpperMatrix& M)
{
#ifdef DEBUG
  if (NRow!=M.NRow) 
    Log->write(SBLog::ERR, SBLog::MATRIX, ClassName() + "::operator=(): ranges of matrix " + 
	       name() + " and " + M.name() + " are different: NRow mismatch");
  if (NCol!=M.NCol) 
    Log->write(SBLog::ERR, SBLog::MATRIX, ClassName() + "::operator=(): ranges of matrix " +
	       name() + " and " + M.name() + " are different: NCol mismatch");
#endif //DEBUG
  double **w=B, **q=M.B;
  unsigned int NCol_=std::min(NCol,M.NCol);
  for (unsigned int i=0; i<NCol_; i++,w++,q++)
    memcpy((void*)(*w), (const void*)(*q), (i+1)*sizeof(double));
  return *this;
};

SBUpperMatrix& SBUpperMatrix::operator= (double f)
{
  double	**w=B, *ww;
  unsigned int	i, j;
  for (i=0; i<NCol; i++,w++)
    for (ww=*w,j=0; j<=i; j++,w++) *ww++=f;
  return *this;
};

SBUpperMatrix& SBUpperMatrix::operator+= (const SBUpperMatrix& M)
{
#ifdef DEBUG
  if (NRow!=M.NRow) 
    Log->write(SBLog::ERR, SBLog::MATRIX, ClassName() + "::operator+=(): ranges of matrix " +
	       name() + " and " + M.name() + " are different: NRow mismatch");
  if (NCol!=M.NCol) 
    Log->write(SBLog::ERR, SBLog::MATRIX, ClassName() + "::operator+=(): ranges of matrix " + 
	       name() + " and " + M.name() + " are different: NCol mismatch");
#endif //DEBUG
  double	**w=B, **q=M.B, *ww, *qq;
  unsigned int	NCol_=std::min(NCol,M.NCol), i, j;
  for (i=0; i<NCol_; i++,w++,q++)
    for (ww=*w,qq=*q,j=0; j<=i; j++) *ww+++=*qq++;
  return *this;
};

SBUpperMatrix& SBUpperMatrix::operator-= (const SBUpperMatrix& M)
{
#ifdef DEBUG
  if (NRow!=M.NRow) 
    Log->write(SBLog::ERR, SBLog::MATRIX, ClassName() + "::operator-=(): ranges of matrix " +
	       name() + " and " + M.name() + " are different: NRow mismatch");
  if (NCol!=M.NCol) 
    Log->write(SBLog::ERR, SBLog::MATRIX, ClassName() + "::operator-=(): ranges of matrix " +
	       name() + " and " + M.name() + " are different: NCol mismatch");
#endif //DEBUG
  double	**w=B, **q=M.B, *ww, *qq;
  unsigned int	NCol_=std::min(NCol,M.NCol), i, j;
  for (i=0; i<NCol_; i++,w++,q++)
    for (ww=*w,qq=*q,j=0; j<=i; j++)*ww++-=*qq++;
  return *this;
};

SBUpperMatrix& SBUpperMatrix::operator*= (double f)
{
  double	**w=B, *ww;
  unsigned int	i, j;
  for (i=0; i<NCol; i++,w++)
    for (ww=*w,j=0; j<=i; j++)*ww++*=f;
  return *this;
};

SBUpperMatrix& SBUpperMatrix::operator/= (double f)
{
  double	**w=B, *ww;
  unsigned int	i, j;
  for (i=0; i<NCol; i++,w++)
    for (ww=*w,j=0; j<=i; j++)*ww++/=f;
  return *this;
};

SBUpperMatrix& SBUpperMatrix::setCol(unsigned int N_, const SBVector& V)
{
#ifdef DEBUG
  if (NCol<N_) 
    Log->write(SBLog::ERR, SBLog::MATRIX, ClassName() + "::setCol: index out of range for " + name());
  if (NRow!=V.N) 
    Log->write(SBLog::ERR, SBLog::MATRIX, ClassName() + "::setCol: matrix " + name() +
	       " and vector " + V.name() + " have different ranges");
#endif //DEBUG
  double	*ww=*(B+std::min(NCol,N_)), *v=V.B;
  unsigned int	N=std::min((N_+1),V.N), i;
  for (i=0; i<N; i++) *ww++=*v++;
  return *this;
};

SBVector& SBUpperMatrix::setVector(unsigned int N_, SBVector& V)
{
#ifdef DEBUG
  if (NCol<N_) 
    Log->write(SBLog::ERR, SBLog::MATRIX, ClassName() + "::setVector: index out of range for " + name());
  if (NRow!=V.N) 
    Log->write(SBLog::ERR, SBLog::MATRIX, ClassName() + "::setVector: matrix " + name() +
	       " and vector " + V.name() + " have different ranges");
#endif //DEBUG
  double	*ww=*(B+std::min(NCol,N_)), *v=V.B;
  unsigned int	N=std::min((N_+1),V.N), i;
  for (i=0; i<N; i++) *v++=*ww++;
  for (i=N; i<V.N; i++) *v++=0.0;
  return V;
};
/*==============================================================================================*/



/*==============================================================================================*/
/**Class SBSymMatrix.
 * This is the symmetric matrix.
 *
 */
/*==============================================================================================*/
class SBSymMatrix : public SBUpperMatrix
{
public:
  // constructors/destructors:
  //
  /**A constructor.
   * Creates an empty matrix (for I/O purposes).
   */
  SBSymMatrix(){B=NULL; NCol=NRow=0; dTmp=0.0;};
  /**A constructor.
   * Creates a copy of object.
   * \param N_ number of rows and columns;
   * \param IsNeedClear makes clearing of the allocated memmory;
   */
  SBSymMatrix(unsigned int N_, const QString& Name_, bool IsNeedClear=TRUE) 
    : SBUpperMatrix(N_, Name_, IsNeedClear){};
  /**A constructor.
   * Creates a copy of object.
   * \param M values of the new object.
   */
  SBSymMatrix(const SBSymMatrix& M) : SBUpperMatrix(M){};
  /**A destructor.*/
  virtual ~SBSymMatrix(){};
  /**Refers to a class name (debug info).*/
  virtual QString ClassName() const {return "SBSymMatrix";};
  
  // interface:
  //
  /**Assign the matix to another one.*/
  SBSymMatrix& operator= (const SBSymMatrix& M)
    {return (SBSymMatrix&)(SBUpperMatrix::operator=(M));};
  /**Sets i-th column to Vector.*/
  virtual SBSymMatrix& setCol(unsigned int i, const SBVector& V)
    {return (SBSymMatrix&)(SBUpperMatrix::setCol(i,V));};
  
  // functions:
  //
  /**Returns transposed matrix (original matrix doesn't change).*/
  virtual SBMatrix T() {return *this;};
  /**Fills the matix with the double.*/
  virtual SBSymMatrix& operator= (double d)
    {return (SBSymMatrix&)(SBUpperMatrix::operator=(d));};
  /**Adds a matrix to the matrix.*/
  virtual SBSymMatrix& operator+= (const SBSymMatrix& S)
    {return (SBSymMatrix&)(SBUpperMatrix::operator+=(S));};
  /**Substracts a matrix from the matrix.*/
  virtual SBSymMatrix& operator-= (const SBSymMatrix& S)
    {return (SBSymMatrix&)(SBUpperMatrix::operator-=(S));};
  /**Multiplies the matrix by a double.*/
  virtual SBSymMatrix& operator*= (double d) 
    {return (SBSymMatrix&)(SBUpperMatrix::operator*=(d));};
  /**Divides the matrix by a double.*/
  virtual SBSymMatrix& operator/= (double d) 
    {return (SBSymMatrix&)(SBUpperMatrix::operator/=(d));};
  /**Returns value of the (i,j)-th element.*/
  inline virtual double at(unsigned int, unsigned int) const;

  // friends:
  //
  /**Returns matrix divided by float.*/
  friend SBSymMatrix operator/ (const SBSymMatrix& M, double f){return SBSymMatrix(M)/=f;};
  /**Returns matrix multiplied by float.*/
  friend SBSymMatrix operator* (const SBSymMatrix& M, double f){return SBSymMatrix(M)*=f;};
  /**Returns matrix multiplied by float.*/
  friend SBSymMatrix operator* (double f, const SBSymMatrix& M){return SBSymMatrix(M)*=f;};
  /**Returns matrix with reversed sign.*/
  friend SBSymMatrix operator- (const SBSymMatrix& M) {return SBSymMatrix(M)*=-1.0;};
  /**Returns a sum of two matrices.*/
  friend inline SBSymMatrix operator+ (const SBSymMatrix&, const SBSymMatrix&);
  /**Returns a difference of two matrices.*/
  friend inline SBSymMatrix operator- (const SBSymMatrix&, const SBSymMatrix&);
  /**Returns product of two matrices.*/
  friend SBSymMatrix operator*(const SBSymMatrix&, const SBSymMatrix&);

  /**Calculates the product of `A*P*A.T()'. Returns symmetric matrix.
   */
  friend SBSymMatrix QuadraticForm(const SBSymMatrix&, const SBMatrix&);
  friend SBSymMatrix QuadraticForm(const SBSymMatrix&, const SBUpperMatrix&);

private: // have to reimplement it in future... 
  SBSymMatrix operator~()
    {
      Log->write(SBLog::ERR, SBLog::MATRIX, ClassName() + 
		 "::operator~(): called for SBSymMatrix " + name());
      return *this;
    };
  SBMatrix	operator*(const SBMatrix&){return SBMatrix();};
  
public:

  friend SBVector operator*(const SBSymMatrix&, const SBVector&);
  friend SBVector matT_x_vec(const SBSymMatrix&, const SBVector&);
  
  // I/O:
  //
  friend QDataStream &operator<<(QDataStream&, const SBSymMatrix&);
  friend QDataStream &operator>>(QDataStream&, SBSymMatrix&);
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBSymMatrix's inlines:									*/
/*												*/
/*==============================================================================================*/
double SBSymMatrix::at(unsigned int i, unsigned int j) const 
{
#ifdef DEBUG
  if (NRow<=i) 
    Log->write(SBLog::ERR, SBLog::MATRIX, ClassName() + "::at(): row's index [" 
	       + QString().setNum(i) + "] out of range [0.." + QString().setNum(NRow-1) +
	       "] for matrix " + name());
  if (NCol<=j) 
    Log->write(SBLog::ERR, SBLog::MATRIX, ClassName() + "::at(): column's index [" 
	       + QString().setNum(j) + "] out of range [0.." + QString().setNum(NCol-1) +
	       "] for matrix " + name());
#endif //DEBUG

  return (i<NRow && j<NCol)?*(*(B+std::max(j,i))+std::min(i,j)):0.0;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBVector's inlined friends.									*/
/*												*/
/*==============================================================================================*/
/**\relates SBVector
 * Returns a sum of two vectors
 */
SBVector operator+(const SBVector& V1, const SBVector& V2)
{
  if (!V1.N) return V2; // here V1 iz zero, just return V2
  if (!V2.N) return V1; // here V2 iz zero, just return V1
#ifdef DEBUG
  if (V1.N!=V2.N) 
    Log->write(SBLog::ERR, SBLog::MATRIX, V1.ClassName() + "::operator+(): ranges of vector " +
	       V1.name() + " and " + V2.name() + " are different");
#endif //DEBUG
  return SBVector(V1)+=V2;
};

/**\relates SBVector
 * Returns a difference of two vectors
 */
SBVector operator-(const SBVector& V1, const SBVector& V2)
{
  if (!V1.N) return V2; // here V1 iz zero, just return V2
  if (!V2.N) return V1; // here V2 iz zero, just return V1
#ifdef DEBUG
  if (V1.N!=V2.N) 
    Log->write(SBLog::ERR, SBLog::MATRIX, V1.ClassName() + "::operator-(): ranges of vector " +
	       V1.name() + " and " + V2.name() + " are different");
#endif //DEBUG
  return SBVector(V1)-=V2;
};

/**\relates SBVector
 * Returns a scalar product of two vectors
 */
double operator*(const SBVector& V1, const SBVector& V2)
{
#ifdef DEBUG
  if (V1.N!=V2.N) 
    Log->write(SBLog::ERR, SBLog::MATRIX, V1.ClassName() + "::operator*(): ranges of vector " +
	       V1.name() + " and " + V2.name() + " are different");
#endif //DEBUG
  double *v1=V1.B, *v2=V2.B, f=0.0;
  unsigned int N_=std::min(V1.N,V2.N);
  for(unsigned int i=0; i<N_; i++) f+=*v1++**v2++;
  return f;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBMatrix's inlined friends.									*/
/*												*/
/*==============================================================================================*/
SBMatrix operator+ (const SBMatrix& M1, const SBMatrix& M2)
{
#ifdef DEBUG
  if (M1.NRow!=M2.NRow) 
    Log->write(SBLog::ERR, SBLog::MATRIX, M1.ClassName() + "::operator+(): ranges of matrix " +
	       M1.name() + " and " + M2.name() + " are different: NRow mismatch");
  if (M1.NCol!=M2.NCol) 
    Log->write(SBLog::ERR, SBLog::MATRIX, M1.ClassName() + "::operator+(): ranges of matrix " +
	       M1.name() + " and " + M2.name() + " are different: NCol mismatch");
#endif //DEBUG
  return SBMatrix (M1)+=M2;
};

SBMatrix operator- (const SBMatrix& M1, const SBMatrix& M2)
{
#ifdef DEBUG
  if (M1.NRow!=M2.NRow) 
    Log->write(SBLog::ERR, SBLog::MATRIX, M1.ClassName() + "::operator-(): ranges of matrix " +
	       M1.name() + " and " + M2.name() + " are different: NRow mismatch");
  if (M1.NCol!=M2.NCol) 
    Log->write(SBLog::ERR, SBLog::MATRIX, M1.ClassName() + "::operator-(): ranges of matrix " +
	       M1.name() + " and " + M2.name() + " are different: NCol mismatch");
#endif //DEBUG
  return SBMatrix (M1)-=M2;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBUpperMatrix's inlined friends.								*/
/*												*/
/*==============================================================================================*/
SBUpperMatrix operator+ (const SBUpperMatrix& M1, const SBUpperMatrix& M2)
{
#ifdef DEBUG
  if (M1.NRow!=M2.NRow) 
    Log->write(SBLog::ERR, SBLog::MATRIX, M1.ClassName() + "::operator+(): ranges of matrix " +
	       M1.name() + " and " + M2.name() + " are different: NRow mismatch");
  if (M1.NCol!=M2.NCol) 
    Log->write(SBLog::ERR, SBLog::MATRIX, M1.ClassName() + "::operator+(): ranges of matrix " +
	       M1.name() + " and " + M2.name() + " are different: NCol mismatch");
#endif //DEBUG
  return SBUpperMatrix (M1)+=M2;
};

SBUpperMatrix operator- (const SBUpperMatrix& M1, const SBUpperMatrix& M2)
{
#ifdef DEBUG
  if (M1.NRow!=M2.NRow) 
    Log->write(SBLog::ERR, SBLog::MATRIX, M1.ClassName() + "::operator-(): ranges of matrix " +
	       M1.name() + " and " + M2.name() + " are different: NRow mismatch");
  if (M1.NCol!=M2.NCol) 
    Log->write(SBLog::ERR, SBLog::MATRIX, M1.ClassName() + "::operator-(): ranges of matrix " +
	       M1.name() + " and " + M2.name() + " are different: NCol mismatch");
#endif //DEBUG
  return SBUpperMatrix (M1)-=M2;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBSymMatrix's inlined friends.								*/
/*												*/
/*==============================================================================================*/
SBSymMatrix operator+ (const SBSymMatrix& M1, const SBSymMatrix& M2)
{
#ifdef DEBUG
  if (M1.NRow!=M2.NRow) 
    Log->write(SBLog::ERR, SBLog::MATRIX, M1.ClassName() + "::operator+(): ranges of matrix " +
	       M1.name() + " and " + M2.name() + " are different: NRow mismatch");
  if (M1.NCol!=M2.NCol) 
    Log->write(SBLog::ERR, SBLog::MATRIX, M1.ClassName() + "::operator+(): ranges of matrix " +
	       M1.name() + " and " + M2.name() + " are different: NCol mismatch");
#endif //DEBUG
  return SBSymMatrix(M1)+=M2;
};

SBSymMatrix operator- (const SBSymMatrix& M1, const SBSymMatrix& M2)
{
#ifdef DEBUG
  if (M1.NRow!=M2.NRow) 
    Log->write(SBLog::ERR, SBLog::MATRIX, M1.ClassName() + "::operator-(): ranges of matrix " +
	       M1.name() + " and " + M2.name() + " are different: NRow mismatch");
  if (M1.NCol!=M2.NCol) 
    Log->write(SBLog::ERR, SBLog::MATRIX, M1.ClassName() + "::operator-(): ranges of matrix " +
	       M1.name() + " and " + M2.name() + " are different: NCol mismatch");
#endif //DEBUG
  return SBSymMatrix (M1)-=M2;
};
/*==============================================================================================*/




#endif //SB_VECTOR_H
