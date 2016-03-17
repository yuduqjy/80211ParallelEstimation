#include "utils.h"

void hermitian(double complex **M,double complex **res){
	for (int r = 0; r < SAMPUTIL; r++)
		for(int c = 0 ; c < SAMPUTIL ; c++ )
			res[c][r] = creal(M[r][c]) - cimag(M[r][c]);
}

void multiply(double complex **M1,double complex **M2,double complex **res){
	double complex sum = 0;
	int num_rows1 = sizeof(M1) / sizeof(M1[0]);
	int num_cols1 = sizeof(M1[0]) / sizeof(M1[0][0]);
	int num_rows2 = sizeof(M2) / sizeof(M2[0]);
	int num_cols2 = sizeof(M2[0]) / sizeof(M2[0][0]);
	if (num_cols1 != num_rows2)
		printf("Matrices dimension missmatch\n");
	else {
		for (int c = 0; c < num_rows1; c++) {
			for (int d = 0; d < num_cols2; d++) {
				for (int k = 0; k < num_rows2; k++)
					sum = sum + M1[c][k]*M2[k][d];

				res[c][d] = sum;
				sum = 0;
			}
		}
	}
}

void identity(double complex **Identity,int size,double scalar){
	for (int c = 0; c < size; c++) {
		for (int r = 0; r < size; r++) {
			if(c==r)
				Identity[r][c] = scalar;
			else
				Identity[r][c] = 0.0;
		}
	}
}

void addition(double complex **M1,double complex **M2,double complex **res){
	int num_rows1 = sizeof(M1) / sizeof(M1[0]);
	int num_cols1 = sizeof(M1[0]) / sizeof(M1[0][0]);
	int num_rows2 = sizeof(M2) / sizeof(M2[0]);
	int num_cols2 = sizeof(M2[0]) / sizeof(M2[0][0]);
	if((num_rows1 != num_rows2) || (num_cols1 != num_cols2))
		printf("Matrices dimension missmatch \n");
	else {
		for (int c = 0; c < num_cols1; c++) {
			for (int r = 0; r < num_rows1; r++) {
				res[r][c] = M1[r][c]+M1[r][c];
			}
		}
	}
}

// matrix inversioon --- https://chi3x10.wordpress.com/2008/05/28/calculate-matrix-inversion-in-c/
// the result is put in Y
void inverse(double complex **A, int order, double complex **Y) {
    // get the determinant of a
    //double complex det = 1.0/CalcDeterminant(A,order);        // Regular Method
    double complex det = 1.0/determinant_impl(A,order);         // Cramer Method
    printf("por aqui \n");

    // memory allocation
    double complex *temp = new double complex [(order-1)*(order-1)];
    double complex **minor = new double complex *[order-1];
    for(int i=0;i<order-1;i++)
        minor[i] = temp+(i*(order-1));
 
    for(int j=0;j<order;j++)
    {
        for(int i=0;i<order;i++)
        {
            // get the co-factor (matrix) of A(j,i)
            GetMinor(A,minor,j,i,order);
            // Y[i][j] = det*CalcDeterminant(minor,order-1);      // Regular Method
            Y[i][j] = det*determinant_impl(minor,order-1);        // Cramer Method
            if( (i+j)%2 == 1)
                Y[i][j] = -Y[i][j];
        }
    }

    // release memory
    //delete [] minor[0];
    delete [] temp;
    delete [] minor;
}
 
// calculate the cofactor of element (row,col)
int GetMinor(double complex **src, double complex **dest, int row, int col, int order) {
    // indicate which col and row is being copied to dest
    int colCount=0,rowCount=0;
 
    for(int i = 0; i < order; i++ ) {
        if( i != row ) {
            colCount = 0;
            for(int j = 0; j < order; j++ ) {
                // when j is not the element
                if( j != col ) {
                    dest[rowCount][colCount] = src[i][j];
                    colCount++;
                }
            }
            rowCount++;
        }
    }
 
    return 1;
}

// Calculate the determinant recursively.
double complex CalcDeterminant( double complex **mat, int order) {
    // order must be >= 0
    // stop the recursion when matrix is a single element
    if(order > 10)
        printf("calcDeterminant - order %d \n",order);
    if( order == 1 )
        return mat[0][0];
 
    // the determinant value
    double complex det = 0.0 + 0.0*I;
 
    // allocate the cofactor matrix
    double complex **minor;
    minor = new double complex *[order-1];
    for(int i=0;i<order-1;i++)
        minor[i] = new double complex [order-1];
 
    for(int i = 0; i < order; i++ ) {
        // get minor of element (0,i)
        GetMinor( mat, minor, 0, i , order);
        // the recusion is here!
 
        det += (i%2==1?-1.0:1.0) * mat[0][i] * CalcDeterminant(minor,order-1);
        //det += pow( -1.0, i ) * mat[0][i] * CalcDeterminant( minor,order-1 );
    }
 
    // release memory
    for(int i=0;i<order-1;i++)
        delete [] minor[i];
    delete [] minor;
 
    return det;
}

void fft_impl(double data[], int nn, int isign)
{
    int n, mmax, m, j, istep, i;
    double wtemp, wr, wpr, wpi, wi, theta;
    double tempr, tempi;

    n = nn << 1;
    j = 1;
    for (i = 1; i < n; i += 2) {
    if (j > i) {
        tempr = data[j];     data[j] = data[i];     data[i] = tempr;
        tempr = data[j+1]; data[j+1] = data[i+1]; data[i+1] = tempr;
    }
    m = n >> 1;
    while (m >= 2 && j > m) {
        j -= m;
        m >>= 1;
    }
    j += m;
    }
    mmax = 2;
    while (n > mmax) {
    istep = 2*mmax;
    theta = TWOPI/(isign*mmax);
    wtemp = sin(0.5*theta);
    wpr = -2.0*wtemp*wtemp;
    wpi = sin(theta);
    wr = 1.0;
    wi = 0.0;
    for (m = 1; m < mmax; m += 2) {
        for (i = m; i <= n; i += istep) {
        j =i + mmax;
        tempr = wr*data[j]   - wi*data[j+1];
        tempi = wr*data[j+1] + wi*data[j];
        data[j]   = data[i]   - tempr;
        data[j+1] = data[i+1] - tempi;
        data[i] += tempr;
        data[i+1] += tempi;
        }
        wr = (wtemp = wr)*wpr - wi*wpi + wr;
        wi = wi*wpr + wtemp*wpi + wi;
    }
    mmax = istep;
    }
}

// Calculate the determinant recursively.
long double complex determinant_impl( double complex **mat, int order) {

    double complex **SubMatrix = new double complex*[order-1];
    long double complex det;
    for (int i = 0; i < order; i++) {
        SubMatrix[i] = new double complex[order-1];
    }
    if( order == 1 ){
        det = mat[0][0];
    } else if(order == 2){
        det = mat[0][0]*mat[1][1] - mat[0][1]*mat[1][0];
    } else{
        for(int i=1 ; i<order ; i++){
            for(int j=1 ; j<order ; j++){
                SubMatrix[i-1][j-1] = mat[i][j] - (mat[i][0] * mat[0][j] / mat[0][0]);
            }
        }
        det = mat[0][0]*determinant_impl(SubMatrix,order-1);
    }

    return det;
}