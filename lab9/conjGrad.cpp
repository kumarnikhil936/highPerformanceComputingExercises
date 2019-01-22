#include <iostream>
#include <cmath>
#include <vector>
#include <numeric>
#include <omp.h>
#include <time.h>

using namespace std;

const double tolerance = 1.0e-9;

using vec    = vector<double>;
using matrix = vector<vec>;

//======================================================================

vec conjGrad( const matrix &A, const vec &B )
{
    int n = A.size();
    vec X( n, 0.0 );
    
    vec R = B;
    vec P = R;
    int k = 0, i = 0;
    
#pragma omp parallel
{
    while ( k < n )
    {
        vec Rold = R;
        vec AP(n);

        #pragma omp parallel for
        for (i = 0; i < A.size(); i++) AP[i] = (inner_product(A[i].begin(), A[i].end(), P.begin(), 0.0));
        
        #pragma omp parallel for
        double alpha = (inner_product(R.begin(), R.end(), R.begin(), 0.0)) / max( (inner_product(R.begin(), R.end(), AP.begin(), 0.0)), tolerance );
        
        #pragma omp parallel for
        for ( i = 0; i < X.size(); i++ ) X[i] = X[i] + alpha * P[i];
        
        #pragma omp parallel for
        for ( i = 0; i < R.size(); i++ ) R[i] = R[i] - alpha * AP[i];
        
        if ( sqrt(inner_product(R.begin(), R.end(), R.begin(), 0.0)) < tolerance ) break;
        
        double beta = (inner_product(R.begin(), R.end(), R.begin(), 0.0)) / max( (inner_product(Rold.begin(), Rold.end(), Rold.begin(), 0.0)), tolerance );
        
        for ( i = 0; i < R.size(); i++ ) P[i] = R[i] + beta * P[i];
        
        k++;
    }
}
    
    return X;
}



//======================================================================


int main(int argc, char *argv[])
{
    double d, sigma, s;
    int i, j, n;
    double temp = 0.0;
    double max_err = 0.0;

    clock_t start, end;

    // Taking values from command line
    sigma = atof(argv[2]);
    n = atoi(argv[1]);
    
    // Declare matrix and vectors
    matrix A(n);
    vec sol(n);
    vec B(n);
    
    // Configuring the values of PSD matrix
    d = 1+ sigma;
    s = -0.5;

    start = clock();

    // Initialize the matrix
    for ( i = 0 ; i < n ; i++ )
        A[i].resize(n);
    
    // Fill the values of the matrix A
    for (i=0; i<n; i++){
        for (j=0; j<n; j++) {
            if(i==j)
                A[i][j] = d;
            else if (i==j+1 || j==i+1 || (j==n-1 && i==0) || (j==0 && i==n-1))
                A[i][j] = s;
            else
                A[i][j] = 0.0;
        }
    }
    
    // Create an initial solution vector 'sol'
    for (i=0; i<n; i++) {
        sol[i] = ((double) rand() / (RAND_MAX));
    }

    // Create the vector B from the equation given in sheet
    #pragma omp parallel for
    for (i=0; i<n; i++) {
        for (j=0; j<n; j++) {
            temp += A[i][j]*sol[j];
        }
        B[i] = temp;
    }
    
    vec X = conjGrad( A, B );
    
    cout << "\nSolving for Ax = B \n\nMatrix A = \n";
    #pragma omp parallel for
    for ( i = 0; i < A.size(); i++ )
    {
        for ( j = 0; j < A[0].size(); j++ )
        {
            cout << A[i][j] << "\t";
        }
        cout << "\n";
    }
    
    // Display vector B
    cout << "\n\nVector B =\n";
    for (i = 0; i < B.size(); i++ ) {
        if ( abs( B[i] ) > tolerance ) cout << B[i] << "\t";
        else cout << "0.0 \t";
    }
    
    // Display
    cout << "\n\nVector X =\n";
    for (i = 0; i < X.size(); i++ ) {
        if ( abs( X[i] ) > tolerance ) cout << X[i] << "\t";
        else cout << "0.0 \t";
    }

    cout << "\n\nChecking if Ax = B  \nAx = ";
    vec B_calc(B.size());
    for (i = 0; i < A.size(); i++) B_calc[i] = (inner_product(A[i].begin(), A[i].end(), X.begin(), 0.0));

    for (i = 0; i < B_calc.size(); i++ ) {
        cout << B_calc[i] << "\t";
    }
    cout << "\n";
    
    end = clock();
    
    // Calculating the max error between X and sol
    for (i=0; i<n; i++) {
        if ((X[i] - sol[i]) > max_err)
            max_err = X[i] - sol[i];
    }
    
    cout << "\nMax error between X and sol is " << max_err << "\n";
    
    cout << "\nTotal Time to solve = %.9f seconds\n" << double(end-start)/CLOCKS_PER_SEC << "\n";
}




