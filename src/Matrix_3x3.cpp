#include <cmath>
#include "Matrix_3x3.h"
#include "CpptrajStdio.h"
//#incl ude "vectormath.h"

// CONSTRUCTOR
Matrix_3x3::Matrix_3x3() {
  M_[0] = 0;
  M_[1] = 0;
  M_[2] = 0;
  M_[3] = 0;
  M_[4] = 0;
  M_[5] = 0;
  M_[6] = 0;
  M_[7] = 0;
  M_[8] = 0;
}

// COPY CONSTRUCTOR
Matrix_3x3::Matrix_3x3(const Matrix_3x3& rhs) {
  M_[0] = rhs.M_[0];
  M_[1] = rhs.M_[1];
  M_[2] = rhs.M_[2];
  M_[3] = rhs.M_[3];
  M_[4] = rhs.M_[4];
  M_[5] = rhs.M_[5];
  M_[6] = rhs.M_[6];
  M_[7] = rhs.M_[7];
  M_[8] = rhs.M_[8];
}

/// CONSTRUCTOR - Takes array of 9, row-major
Matrix_3x3::Matrix_3x3(const double *Min) {
  M_[0] = Min[0]; 
  M_[1] = Min[1]; 
  M_[2] = Min[2]; 
  M_[3] = Min[3]; 
  M_[4] = Min[4]; 
  M_[5] = Min[5]; 
  M_[6] = Min[6]; 
  M_[7] = Min[7]; 
  M_[8] = Min[8]; 
}

/// CONSTRUCTOR - Set diagonal
Matrix_3x3::Matrix_3x3(double d1, double d2, double d3) {
  M_[0] = d1;
  M_[1] = 0;
  M_[2] = 0;
  M_[3] = 0;
  M_[4] = d2;
  M_[5] = 0;
  M_[6] = 0;
  M_[7] = 0;
  M_[8] = d3;
}

// Assignment
Matrix_3x3& Matrix_3x3::operator=(const Matrix_3x3& rhs) {
  if (this==&rhs) return *this;
  M_[0] = rhs.M_[0];
  M_[1] = rhs.M_[1];
  M_[2] = rhs.M_[2];
  M_[3] = rhs.M_[3];
  M_[4] = rhs.M_[4];
  M_[5] = rhs.M_[5];
  M_[6] = rhs.M_[6];
  M_[7] = rhs.M_[7];
  M_[8] = rhs.M_[8];
  return *this;
}

/// Max number of iterations to execute Jacobi algorithm
const int Matrix_3x3::MAX_ITERATIONS = 50;

#define ROTATE(ARR,MAJ1,MIN1,MAJ2,MIN2) { \
  dg = ARR[MAJ1 + MIN1]; \
  dh = ARR[MAJ2 + MIN2]; \
  ARR[MAJ1 + MIN1] = dg - ds*(dh+dg*tau); \
  ARR[MAJ2 + MIN2] = dh + ds*(dg-dh*tau); }

// Matrix_3x3::Diagonalize()
/** Diagonalize the matrix using Jacobi method. The eigenvalues and
  * eigenvectors are produced as output.
  * \param vecD Output eigenvalues.
  * \param matrixV Output eigenvectors in columns (V0x V1x, V2x, V0y, ... V2z).
  */
int Matrix_3x3::Diagonalize( double *vecD, double* matrixV ) 
{
  // Create identity matrix
  //double matrixV[9];
  matrixV[0] = 1;
  matrixV[1] = 0;
  matrixV[2] = 0;
  matrixV[3] = 0;
  matrixV[4] = 1;
  matrixV[5] = 0;
  matrixV[6] = 0;
  matrixV[7] = 0;
  matrixV[8] = 1;
  // Set vectors B and D equal to diagonal of M_. vector Z is 0.
  double vecB[3], vecZ[3];
  vecB[0] = vecD[0] = M_[0]; 
  vecB[1] = vecD[1] = M_[4]; 
  vecB[2] = vecD[2] = M_[8];
  vecZ[0] = 0; 
  vecZ[1] = 0; 
  vecZ[2] = 0;
  // MAIN LOOP
  double tresh = 0;
  int nrot;
  for (int i = 0; i < MAX_ITERATIONS; ++i) {
    // sm = SUM of UPPER RIGHT TRIANGLE
    double sm = fabs(M_[1]) + fabs(M_[2]) + fabs(M_[5]);
    if (sm == 0) return 0;
    
    if (i < 3)
      tresh = 0.2 * sm / 9;
    else
      tresh = 0;
    // BEGIN INNER LOOP OVER UPPER RIGHT TRIANGLE
    double dt;
    //int p3 = 0;
    int ip, p3;
    for ( ip = p3 = 0; ip < 2; ++ip, p3+=3) {
      for ( int iq = ip + 1; iq < 3; ++iq ) {
        int midx = p3 + iq;
        double dg = 100.0 * fabs(M_[midx]);
        if ( i > 3 && fabs(vecD[ip]) + dg == fabs(vecD[ip]) && 
                      fabs(vecD[iq]) + dg == fabs(vecD[iq]) )
        {
          M_[midx] = 0;
        } else if ( fabs(M_[midx]) > tresh) {
          double dh = vecD[iq] - vecD[ip];
          if (fabs(dh) + dg == fabs(dh))
            dt = M_[p3 + iq] / dh;
          else {
            double theta = 0.5 * dh / M_[midx];
            dt = 1.0 / (fabs(theta)+(double)sqrt(1.0+theta*theta));
            if (theta < 0.0)
              dt = -dt;
          }
          double dc = 1.0 / (double)sqrt(1+dt*dt);
          double ds = dt * dc;
          double tau = ds / (1.0+dc);
          dh = dt * M_[midx];
          vecZ[ip] -= dh;
          vecZ[iq] += dh;
          vecD[ip] -= dh;
          vecD[iq] += dh;
          M_[midx] = 0;
          int j, j3;
          for (j=j3=0; j<=ip-1; j++,j3+=3)
            ROTATE(M_,j3,ip,j3,iq)
          for (int j=ip+1; j<=iq-1; j++)
            ROTATE(M_,p3,j,j*3,iq)
          for (int j=iq+1; j<3; j++)
            ROTATE(M_,p3,j,iq*3,j)

          for (j3=0; j3<9; j3+=3)
            ROTATE(matrixV,j3,ip,j3,iq)

          ++nrot;
        }
      }
    } // END INNER LOOP OVER UPPER RIGHT TRIANGLE
    vecB[0] += vecZ[0];
    vecD[0] = vecB[0];
    vecZ[0] = 0;
    vecB[1] += vecZ[1];
    vecD[1] = vecB[1];
    vecZ[1] = 0;
    vecB[2] += vecZ[2];
    vecD[2] = vecB[2];
    vecZ[2] = 0;
  }
  mprintf("Too many iterations in routine!\n");
  return 1;
}

// Matrix_3x3::Diagonalize_Sort()
/** Diagonalize the matrix and sort eigenvalues/eigenvectors in 
  * descending order.
  * \param EvecOut Output eigenvectors in rows (V0x, V0y, V0z, V1x, ... V2z).
  * \param EvalOut Output eigenvalues.
  */
int Matrix_3x3::Diagonalize_Sort(double *EvecOut, double *EvalOut) 
{
  double Evec[9], Eval[3];

  if ( Diagonalize( Eval, Evec ) ) 
  {
    mprintf("Convergence failed.\n");
    return 1; 
  }
  //printMatrix_3x3("Eigenvector Matrix", Evec);

  if (Eval[0] > Eval[1] && Eval[0] > Eval[2]) { // 0 is max
    if (Eval[1] > Eval[2]) {
      i1_ = 0; i2_ = 1; i3_ = 2;
    } else {
      i1_ = 0; i2_ = 2; i3_ = 1;
    }
  } else if (Eval[1] > Eval[0] && Eval[1] > Eval[2]) { // 1 is max
    if (Eval[0] > Eval[2]) {
      i1_ = 1; i2_ = 0; i3_ = 2;
    } else {
      i1_ = 1; i2_ = 2; i3_ = 0;
    }
  } else if (Eval[0] > Eval[1]) { // 2 is max
    i1_ = 2; i2_ = 0; i3_ = 1;
  } else {
    i1_ = 2; i2_ = 1; i3_ = 0;
  }
  //mprintf("EIGENVALUE ORDER (0=high, 3=med, 6=low): %i %i %i\n",i1_,i2_,i3_);

  // Swap Eigenvectors - place them in rows
  EvecOut[0] = Evec[i1_];
  EvecOut[1] = Evec[i1_+3];
  EvecOut[2] = Evec[i1_+6];

  EvecOut[3] = Evec[i2_];
  EvecOut[4] = Evec[i2_+3];
  EvecOut[5] = Evec[i2_+6];

  EvecOut[6] = Evec[i3_];
  EvecOut[7] = Evec[i3_+3];
  EvecOut[8] = Evec[i3_+6];

  // Swap eigenvalues
  EvalOut[0] = Eval[i1_];
  EvalOut[1] = Eval[i2_];
  EvalOut[2] = Eval[i3_];

  return 0;
}

/**  The jacobi diagonalization procedure can sometimes result
  *  in eigenvectors which when applied to transform the coordinates
  *  result in a a chiral inversion about the Y axis.  This code catches
  *  this case, reversing the offending eigenvectors.
  *  
  *  NOTE: the idea of rotating the coordinate basis vectors came from 
  *  some code posted to the computational chemistry mailing list 
  *  (chemistry@osc) in a summary of methods to perform principal axis 
  *  alignment...
  *
  * It is expected that the eigenvector matrix has eigenvectors in rows.
  */
static int jacobiCheckChirality(double* ev)
{
  Matrix_3x3 points(ev);
  Matrix_3x3 result;
  //points.Print("POINTS"); 

  // rotate vector three into XZ plane
  result.RotationAroundZ( points[2], points[5] ); // Ev0z, Ev1z
  result *= points;
  //result.Print("POINTS1");

  // rotate vector three into Z axis
  points.RotationAroundY( result[2], result[8] ); 
  points *= result;
  //points.Print("POINTS2");

  // rotate vector one into XZ
  result.RotationAroundZ( points[0], points[3] );
  result *= points;
  //result.Print("POINTS3");

  // rotate vector one into X 
  points.RotationAroundY( result[2], result[0] );
  points *= result;
  //points.Print("POINTS4");

  // has Y changed sign? If so, flip Y eigenvector (row 1) 
  if ( points[4] < 0 ) {
    ev[3] = -ev[3];
    ev[4] = -ev[4];
    ev[5] = -ev[5];
    return 1;
  }
  return 0;
}

// Matrix_3x3::Diagonalize_Sort_Chirality()
int Matrix_3x3::Diagonalize_Sort_Chirality(double* EvecOut, double* EvalOut, int debug)
{
  if ( Diagonalize_Sort( EvecOut, EvalOut ) )
    return 1;

  // Invert eigenvector signs based on ordering to avoid reflections
  if (i1_ == 0 && i2_ == 2 && i3_ == 1) {
    EvecOut[3] = -EvecOut[3];
    EvecOut[4] = -EvecOut[4];
    EvecOut[5] = -EvecOut[5];
  } else if (i1_ == 2 && i2_ == 0 && i3_ == 1) {
    EvecOut[0] = -EvecOut[0];
    EvecOut[1] = -EvecOut[1];
    EvecOut[2] = -EvecOut[2];
    EvecOut[3] = -EvecOut[3];
    EvecOut[4] = -EvecOut[4];
    EvecOut[5] = -EvecOut[5];
    EvecOut[6] = -EvecOut[6];
    EvecOut[7] = -EvecOut[7];
    EvecOut[8] = -EvecOut[8];
  }

  // Flip Y-vector if necessary 
  if (jacobiCheckChirality( EvecOut ) && debug>0)
    mprintf("Warning: PRINCIPAL: CHECK CHIRALITY: Y eigenvector sign swapped!\n");
   
  return 0;
}

// Matrix_3x3::Print()
void Matrix_3x3::Print(const char* Title) 
{
  mprintf("    %s\n",Title);
  mprintf("     %8.4lf %8.4lf %8.4lf\n", M_[0], M_[1], M_[2]);
  mprintf("     %8.4lf %8.4lf %8.4lf\n", M_[3], M_[4], M_[5]);
  mprintf("     %8.4lf %8.4lf %8.4lf\n", M_[6], M_[7], M_[8]);
}

// Matrix_3x3::operator*=()
Matrix_3x3& Matrix_3x3::operator*=(const Matrix_3x3& rhs) {
  double Row[9];
  Row[0] = M_[0];
  Row[1] = M_[1];
  Row[2] = M_[2];
  Row[3] = M_[3];
  Row[4] = M_[4];
  Row[5] = M_[5];
  Row[6] = M_[6];
  Row[7] = M_[7];
  Row[8] = M_[8];
  M_[0] = (Row[0] * rhs.M_[0]) + (Row[1] * rhs.M_[3]) + (Row[2] * rhs.M_[6]);
  M_[1] = (Row[0] * rhs.M_[1]) + (Row[1] * rhs.M_[4]) + (Row[2] * rhs.M_[7]);
  M_[2] = (Row[0] * rhs.M_[2]) + (Row[1] * rhs.M_[5]) + (Row[2] * rhs.M_[8]);
  M_[3] = (Row[3] * rhs.M_[0]) + (Row[4] * rhs.M_[3]) + (Row[5] * rhs.M_[6]);
  M_[4] = (Row[3] * rhs.M_[1]) + (Row[4] * rhs.M_[4]) + (Row[5] * rhs.M_[7]);
  M_[5] = (Row[3] * rhs.M_[2]) + (Row[4] * rhs.M_[5]) + (Row[5] * rhs.M_[8]);
  M_[6] = (Row[6] * rhs.M_[0]) + (Row[7] * rhs.M_[3]) + (Row[8] * rhs.M_[6]);
  M_[7] = (Row[6] * rhs.M_[1]) + (Row[7] * rhs.M_[4]) + (Row[8] * rhs.M_[7]);
  M_[8] = (Row[6] * rhs.M_[2]) + (Row[7] * rhs.M_[5]) + (Row[8] * rhs.M_[8]);
  return *this;
}
 
// Matrix_3x3::RotationAroundZ()
void Matrix_3x3::RotationAroundZ(double a1, double a2) {
  double r = sqrt( a1*a1 + a2*a2 );
  M_[0] = a1 / r; //  cos t
  M_[1] = a2 / r; // -sin t
  M_[2] = 0;
  M_[3] = -M_[1]; //  sin t
  M_[4] = M_[0];  //  cos t
  M_[5] = 0;
  M_[6] = 0;
  M_[7] = 0;
  M_[8] = 1;
}

// Matrix_3x3::RotationAroundY()
void Matrix_3x3::RotationAroundY(double a1, double a2) {
  double r = sqrt( a1*a1 + a2*a2 );
  M_[0] = a2 / r;  //  cos t
  M_[1] = 0;
  M_[2] = -a1 / r; //  sin t
  M_[3] = 0;
  M_[4] = 1;
  M_[5] = 0;
  M_[6] = -M_[2];  // -sin t
  M_[7] = 0;
  M_[8] = M_[0];   //  cos t
}

Vec3 Matrix_3x3::operator*(const Vec3& rhs) const {
  double x = rhs[0]; 
  double y = rhs[1]; 
  double z = rhs[2];
  return Vec3( ((M_[0]*x) + (M_[1]*y) + (M_[2]*z)),
               ((M_[3]*x) + (M_[4]*y) + (M_[5]*z)),
               ((M_[6]*x) + (M_[7]*y) + (M_[8]*z))  );
}

Vec3 Matrix_3x3::TransposeMult(const Vec3& rhs) const {
  double x = rhs[0];
  double y = rhs[1];
  double z = rhs[2];
  return Vec3( ((M_[0]*x) + (M_[3]*y) + (M_[6]*z)),
               ((M_[1]*x) + (M_[4]*y) + (M_[7]*z)),
               ((M_[2]*x) + (M_[5]*y) + (M_[8]*z))  );
}
