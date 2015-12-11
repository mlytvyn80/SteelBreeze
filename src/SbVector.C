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

#include "SbVector.H"



/*==============================================================================================*/
/*												*/
/* SBVector implementation									*/
/*												*/
/*==============================================================================================*/
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBVector's friends:										*/
/*												*/
/*==============================================================================================*/
QDataStream &operator<<(QDataStream& s, const SBVector& V)
{
  s << (const SBNamed&) V;
  return s.writeBytes((const char*)V.B, V.N*sizeof(double));
};

QDataStream &operator>>(QDataStream& s, SBVector& V)
{
  unsigned int n;
  if (V.B) delete[] V.B;
  s >> (SBNamed&) V;
  s.readBytes((char*&)V.B, n);
  V.N = n/sizeof(double);
  return s;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBMatrix implementation									*/
/*												*/
/*==============================================================================================*/
void SBMatrix::resize(unsigned int NRow_, unsigned int NCol_)
{
  if (B) 
    {
      double **w=B;
      for (unsigned int i=0; i<NCol; i++,w++) delete[] *w;
      delete[] B;
    };
  NRow=NRow_;
  dTmp=0.0;
  B=new double*[NCol=NCol_];
  double **w=B;
  for (unsigned int i=0; i<NCol; i++, w++)
    {
      *w=new double[NRow];
      memset((void*)(*w), 0, sizeof(double)*NRow);
    };
};

SBMatrix SBMatrix::operator~()
{
  return *this;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBMatrix's friends:										*/
/*												*/
/*==============================================================================================*/
SBMatrix operator*(const SBMatrix& M1, const SBMatrix& M2)
{
#ifdef DEBUG
  if (M1.NCol!=M2.NRow) 
    Log->write(SBLog::ERR, SBLog::MATRIX, M1.ClassName() + 
	       ": operator*(): incompatible ranges of matrices " + M1.name() + " and " + M2.name());
#endif //DEBUG
  
  SBMatrix		M(M1.NRow, M2.NCol, "Product of " + M1.name() + " and " + M2.name(), FALSE);
  double		**m=M.B, **m1=M1.B, **m2=M2.B, *mm, *mm2;
  unsigned int		N=std::min(M1.NCol,M2.NRow), i, j, l;
  
  for (j=0; j<M.NCol; j++,m++,m2++)
    for (mm=*m,i=0; i<M.NRow; i++,mm++)
      for (mm2=*m2,*mm=0.0,l=0; l<N; l++,mm2++) *mm+=*(*(m1+l)+i)**mm2;
  return M;
};

SBMatrix matT_x_mat(const SBMatrix& M1, const SBMatrix& M2)
{
#ifdef DEBUG
  if (M1.NRow!=M2.NRow) 
    Log->write(SBLog::ERR, SBLog::MATRIX, M1.ClassName() + 
	       ": matT_x_mat: incompatible ranges of matrices " + M1.name() + " and " + M2.name());
#endif //DEBUG

  SBMatrix		M(M1.NCol, M2.NCol, "Product of " + M1.name() + "(Transposed) and " + M2.name(), 
			  FALSE);
  double		**m=M.B, **m1=M1.B, **m2=M2.B, *mm, *mm1, *mm2;
  unsigned int		N=std::min(M1.NRow,M2.NRow), i, j, l;

  for (j=0; j<M.NCol; j++,m++,m2++)
    for (mm=*m,m1=M1.B,i=0; i<M.NRow; i++,mm++,m1++)
      for (mm2=*m2,mm1=*m1,*mm=0.0,l=0; l<N; l++,mm1++,mm2++) *mm+=*mm1**mm2;
  return M;
};

QDataStream &operator<<(QDataStream& s, const SBMatrix& M)
{
  double **w=M.B;
  s << (const SBNamed&)M << M.NCol;
  for (unsigned int i=0; i<M.NCol; i++,w++) s.writeBytes((const char*)*w, M.NRow*sizeof(double));
  return s;
};

QDataStream &operator>>(QDataStream& s, SBMatrix& M)
{
  unsigned int n,NRow=0,i;
  double **w;
  s >> (SBNamed&)M >> n;
  if (M.B) 
    {
      for (w=M.B,i=0; i<M.NCol; i++,w++) delete[] *w;
      delete[] M.B;
    };
  M.B=new double*[M.NCol=n];
  for (w=M.B,i=0; i<M.NCol; i++,w++) 
    {
      s.readBytes((char*&)*w, n);
      if (n%sizeof(double)) 
	Log->write(SBLog::ERR, SBLog::MATRIX | SBLog::IO, 
		   "SBMatrix: operator>>: wrong serialization of the input");
      if (!NRow) NRow=n;
      else if (NRow!=n)
	Log->write(SBLog::ERR, SBLog::MATRIX | SBLog::IO, 
		   "SBMatrix: operator>>: wrong matrix order");
    };
  M.NRow=NRow/sizeof(double);
  return s;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBUpperMatrix's friends:									*/
/*												*/
/*==============================================================================================*/
SBUpperMatrix operator*(const SBUpperMatrix& M1, const SBUpperMatrix& M2)
{
#ifdef DEBUG
  if (M1.NCol!=M2.NRow) 
    Log->write(SBLog::ERR, SBLog::MATRIX, M1.ClassName() + 
	       ": operator*(): incompatible ranges of matrices " + M1.name() + " and " + M2.name());
#endif //DEBUG

  SBUpperMatrix		M(M1.NRow, "Product of " + M1.name() + " and " + M2.name(), FALSE);
  double		**m=M.B, **m1=M1.B, **m2=M2.B, *mm, *mm2;
  unsigned int		i, j, l;
  
  for (j=0; j<M.NCol; j++,m++,m2++)
    for (mm=*m,i=0; i<=j; i++,mm++)
      for (mm2=*m2+i,*mm=0.0,l=i; l<=j; l++,mm2++) *mm+=*(*(m1+l)+i)**mm2;
  return M;
};

SBSymMatrix matT_x_mat(const SBUpperMatrix& M1, const SBUpperMatrix& M2) 
{
#ifdef DEBUG
  if (M1.NRow!=M2.NRow) 
    Log->write(SBLog::ERR, SBLog::MATRIX, M1.ClassName() + 
	       ": matT_x_mat: incompatible ranges of matrices " + M1.name() + " and " + M2.name());
#endif //DEBUG
  
  SBSymMatrix		M(M1.NCol,  "Product of " + M1.name() + "(Transposed) and " + M2.name(), 
			  FALSE);
  double		**m=M.B, **m1=M1.B, **m2=M2.B, *mm, *mm1, *mm2;
  unsigned int		i, j, l;

  for (j=0; j<M.NCol; j++,m++,m2++)
    for (mm=*m,m1=M1.B,i=0; i<M.NRow; i++,mm++,m1++)
      for (mm2=*m2,mm1=*m1,*mm=0.0,l=0; l<=std::min(i,j); l++,mm1++,mm2++) *mm+=*mm1**mm2;
  return M;
};

/**\relates SBVector
 * \relates SBUpperMatrix
 * Returns a product of a transposed upper triangular matrix and a vector.
 * This function is equal to `M.T()*V', but implements a more efficient realization.
 */
SBVector matT_x_vec(const SBUpperMatrix& M, const SBVector& V)
{
#ifdef DEBUG
  if (M.NRow!=V.N) 
    Log->write(SBLog::ERR, SBLog::MATRIX, V.ClassName() + 
	       ": matT_x_vec: incompatible ranges of a matrix " + M.name() + " and a vector " + V.name());
#endif //DEBUG

  SBVector		R(M.NCol, "Product of matrix " + M.name() + "(Transposed) and vector " + V.name(), 
			  FALSE);
  register double	**m=M.B, *r=R.B, *v, *mm;
  unsigned int		i, j;
  //  double		v;

  for (i=0; i<R.N; i++,r++,m++)
    for (*r=0.0,v=V.B,mm=*m,j=0; j<=i; j++,v++,mm++) *r+=*mm**v;
  /*
    for (i=0; i<R.N; i++)
    { 
    for (j=0, v=0.0; j<=i; j++)
    v+=M.at(j,i)*V.at(j);
    R(i)=v;
    };
  */
  return R;
};

SBSymMatrix& RRT(SBSymMatrix& M, const SBUpperMatrix& R)
{
#ifdef DEBUG
  if (M.NRow!=R.NRow) 
    Log->write(SBLog::ERR, SBLog::MATRIX, M.ClassName() + 
	       ": RRT: incompatible ranges of matrices " + M.name() + " and " + R.name());
#endif //DEBUG
  
  //  double		**m=M.B, **r=R.B;
  double		v;
  unsigned int		i, j, l;

  for (i=0; i<M.NRow; i++)
    for (j=i; j<M.NCol; j++)
      {
	for (v=0.0, l=j; l<R.NCol; l++)
	  v+=R.at(i,l)*R.at(j,l);
	M(i,j)=v;
      };
  return M;
};

SBSymMatrix& RTR(SBSymMatrix& M, const SBUpperMatrix& R)
{
#ifdef DEBUG
  if (M.NRow!=R.NRow) 
    Log->write(SBLog::ERR, SBLog::MATRIX, M.ClassName() + 
	       ": RRT: incompatible ranges of matrices " + M.name() + " and " + R.name());
#endif //DEBUG
  
  //  double		**m=M.B, **r=R.B;
  double		v;
  unsigned int		i, j, l;

  for (i=0; i<M.NRow; i++)
    for (j=i; j<M.NCol; j++)
      {
	for (v=0.0, l=0; l<=i; l++)
	  v+=R.at(l,i)*R.at(l,j);
	M(i,j)=v;
      };
  return M;
};

SBVector& Solve(const SBUpperMatrix& R, SBVector& x, const SBVector& z)
{
#ifdef DEBUG
  if (x.N!=z.N)
    Log->write(SBLog::ERR, SBLog::MATRIX, R.ClassName() + 
	       ": Solve: incompatible ranges of vectors " + x.name() + " and " + z.name());
  if (R.NRow!=x.N)
    Log->write(SBLog::ERR, SBLog::MATRIX, R.ClassName() + 
	       ": Solve: incompatible ranges of vector " + x.name() + " and matrix " + R.name());
#endif //DEBUG

  unsigned int		i, j, n=x.N-1;
  double		s;

  if (!x.N) return x;

  if (R.at(n,n)!=0.0)
    x(n)=z.at(n)/R.at(n,n);
  else
    Log->write(SBLog::ERR, SBLog::MATRIX, R.ClassName() + 
	       ": Solve: division by zero caused by R[N,N]=0, for matrix " + R.name());
  for (i=n; i>0; i--) // i ==> (i+1)
    {
      for (j=i, s=0.0; j<x.N; j++) s+=R.at(i-1,j)*x.at(j);
      if (R.at(i-1,i-1)!=0.0)
	x(i-1) = (z.at(i-1) - s)/R.at(i-1,i-1);
      else
	Log->write(SBLog::ERR, SBLog::MATRIX, R.ClassName() + 
		   ": Solve: division by zero caused by R[j,j]=0, for matrix " + R.name());
    };
  return x;
};

SBUpperMatrix operator~(const SBUpperMatrix& R)
{
  SBUpperMatrix U(R.n(), "Inverse of " + R.name(), FALSE);
  double	s;

  if (!R.n()) return U;

  if (R.at(0,0)!=0.0)
    U(0,0) = 1.0/R.at(0,0);
  else 
    Log->write(SBLog::ERR, SBLog::MATRIX, R.ClassName() + 
	       ": operator~: R[1,1] is zero for the matrix " + R.name());
  for (unsigned int j=1; j<R.n(); j++)
    if (R.at(j,j)!=0.0)
      {
        U(j,j) = 1.0/R.at(j,j);
	for (unsigned int k=0; k<j; k++)
	  {
	    s=0.0;
	    for (unsigned int i=k; i<j; i++) s += U.at(k,i)*R.at(i,j);
	    U(k,j) = -s*U.at(j,j);
	  };
      }
    else
      Log->write(SBLog::ERR, SBLog::MATRIX, R.ClassName() + 
		 ": operator~: R[j,j] is zero for the matrix " + R.name());
  return U;
};

QDataStream& operator<<(QDataStream& s, const SBUpperMatrix& M)
{
  double **w=M.B;
  s << (const SBNamed&)M << M.NCol;
  for (unsigned int i=0; i<M.NCol; i++,w++) s.writeBytes((const char*)*w, (i+1)*sizeof(double));
  return s;
};

QDataStream& operator>>(QDataStream& s, SBUpperMatrix& M)
{
  unsigned int n,i;
  double **w;
  s >> (SBNamed&)M >> n;
  if (M.B) 
    {
      for (w=M.B,i=0; i<M.NCol; i++,w++) delete[] *w;
      delete[] M.B;
    };
  M.B=new double*[M.NCol=n];
  for (w=M.B,i=0; i<M.NCol; i++,w++) 
    {
      s.readBytes((char*&)*w, n);
      if (n%sizeof(double)) 
	Log->write(SBLog::ERR, SBLog::MATRIX | SBLog::IO, 
		   "SBUpperMatrix: operator>>: wrong serialization of the input");
      if (n/sizeof(double)!=i+1) 
	Log->write(SBLog::ERR, SBLog::MATRIX | SBLog::IO, 
		   "SBUpperMatrix: operator>>: wrong number of elements in a column");
    };
  M.NRow=M.NCol;
  return s;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* (SBVector && SBMatrix)'s friends.								*/
/*												*/
/*==============================================================================================*/
/**\relates SBVector
 * \relates SBMatrix
 * Returns a product of a matrix and a vector.
 */
SBVector operator*(const SBMatrix& M, const SBVector& V)
{
#ifdef DEBUG
  if (M.NCol!=V.N) 
    Log->write(SBLog::ERR, SBLog::MATRIX, V.ClassName() + 
	       ": operator*(): incompatible ranges of matrix " + M.name() + " and vector " + V.name());
#endif //DEBUG

  SBVector		R(M.NRow, "Product of matrix " + M.name() + " and vector " + V.name(), FALSE);
  double		*v=V.B, *r=R.B;
  unsigned int		N=std::min(M.NCol,V.N), i, l;

  for (i=0; i<R.N; i++,r++)
    for (*r=0.0,v=V.B,l=0; l<N; l++,v++) *r+=*(*(M.B+l)+i)**v;
  return R;
};

/**\relates SBVector
 * \relates SBMatrix
 * Returns a product of a transposed matrix and a vector.
 * This function is equal to `M.T()*V', but implements a more efficient realization.
 */
SBVector matT_x_vec(const SBMatrix& M, const SBVector& V)
{
#ifdef DEBUG
  if (M.NRow!=V.N) 
    Log->write(SBLog::ERR, SBLog::MATRIX, V.ClassName() + 
	       ": matT_x_vec: incompatible ranges of a matrix " + M.name() + " and a vector " + V.name());
#endif //DEBUG

  SBVector		R(M.NCol, "Product of matrix " + M.name() + "(Transposed) and vector " + V.name(), 
			  FALSE);
  register double	**m=M.B, *r=R.B, *v, *mm;
  unsigned int		N=std::min(M.NRow,V.N), i, l;
  
  for (i=0; i<R.N; i++,r++,m++)
    for (*r=0.0,v=V.B,mm=*m,l=0; l<N; l++,v++,mm++) *r+=*mm**v;
  return R;
};
/*==============================================================================================*/



/*==============================================================================================*/
/*												*/
/* SBSymMatrix's friends:									*/
/*												*/
/*==============================================================================================*/
SBSymMatrix operator*(const SBSymMatrix& M1, const SBSymMatrix& M2)
{
#ifdef DEBUG
  if (M1.NCol!=M2.NRow) 
    Log->write(SBLog::ERR, SBLog::MATRIX, M1.ClassName() + 
	       "::operator*(): incompatible ranges of matrices " + M1.name() + " and " + M2.name());
#endif //DEBUG

  SBSymMatrix		M(M1.n(), "Product of " + M1.name() + " and " + M2.name(), FALSE);
  unsigned int		i, j, l;
  double		s;

  for (i=0; i<M.n(); i++)
    for (j=i; j<M.n(); j++)
      {
	for (l=0, s=0.0; l<M.n(); l++) 
	  s+= M1.at(i,l)*M2.at(l,j);
	M(i,j)=s;
      };

  return M;
};

SBSymMatrix QuadraticForm(const SBSymMatrix& P, const SBMatrix& A)
{
#ifdef DEBUG
  if (A.NCol!=P.NRow) 
    Log->write(SBLog::ERR, SBLog::MATRIX, P.ClassName() + 
	       ": QuadraticForm: incompatible ranges of matrices " + A.name() + " and " + P.name());
#endif //DEBUG

  SBSymMatrix S(A.NRow, "[QForm of " + A.name() + " with " + P.name() + "]", FALSE);
  SBMatrix    T(A.NRow, A.NCol, "[Temporary for QForm of " + A.name() + " with " + P.name() + "]");
  unsigned int		i, j, l;
  double		s;
  
  for (i=0; i<T.NRow; i++)
    for (j=0; j<T.NCol; j++)
      {
	for (l=0, s=0.0; l<A.NCol; l++) 
	  s+= A.at(i,l)*P.at(l,j);
	T(i,j)=s;
      };

  for (i=0; i<S.NRow; i++)
    for (j=i; j<S.NCol; j++)
      {
	for (l=0, s=0.0; l<T.NCol; l++) 
	  s+= T.at(i,l)*A.at(j,l);
	S(i,j)=s;
      };

  return S;
};

SBSymMatrix QuadraticForm(const SBSymMatrix& P, const SBUpperMatrix& R)
{
#ifdef DEBUG
  if (R.NCol!=P.NRow) 
    Log->write(SBLog::ERR, SBLog::MATRIX, P.ClassName() + 
	       ": QuadraticForm: incompatible ranges of matrices " + R.name() + " and " + P.name());
#endif //DEBUG

  SBSymMatrix S(R.NRow, "[QForm of " + R.name() + " with " + P.name() + "]", FALSE);
  SBMatrix    T(R.nRow(), R.NCol, "[Temporary for QForm of " + R.name() + " with " + P.name() + "]");
  unsigned int		i, j, l;
  double		s;
  
  for (i=0; i<T.nRow(); i++)
    for (j=0; j<T.nCol(); j++)
      {
	for (l=i, s=0.0; l<R.NCol; l++) 
	  s+= R.at(i,l)*P.at(l,j);
	T(i,j)=s;
      };

  for (i=0; i<S.NRow; i++)
    for (j=i; j<S.NCol; j++)
      {
	for (l=j, s=0.0; l<T.nCol(); l++) 
	  s+= T.at(i,l)*R.at(j,l);
	S(i,j)=s;
      };

  return S;
};

QDataStream& operator<<(QDataStream& s, const SBSymMatrix& M)
{
  return s<<(const SBUpperMatrix&)M;
};

QDataStream& operator>>(QDataStream& s, SBSymMatrix& M)
{
  return s>>(SBUpperMatrix&)M;
};
/*==============================================================================================*/



/*==============================================================================================*/

