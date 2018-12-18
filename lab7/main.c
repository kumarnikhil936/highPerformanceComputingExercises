#include<stdio.h>
#include<omp.h>
#include<stdlib.h>
#include<time.h>

int main(int argc, char *argv[]) {
    int n = atoi(argv[1]);
    int i, j, k;
    float a[n][n+1];
    float x[n];
    float temp;
    double total_t1, total_t2;
    double gauss_t1, gauss_t2;
    double back_t1, back_t2;
    double max_error;

    total_t1 = omp_get_wtime();

    printf("\n=> Filling up the matrix\n");
    srand(time(NULL));    
    for(i=0; i<n; i++) {
    	for(j=0; j<n; j++) {
    		if (i==j){
    			a[i][j] = 0.1*n;
    			temp += a[i][j];
    		}
    		else{
    			a[i][j] = (double)rand()/(RAND_MAX);
    			temp += a[i][j];
    		}
    	}	
    	a[i][n] = temp;
    	temp = 0;
    }

    /*printf("\nInitial matrix\n");
    for(i=0; i<n; i++){
    	for(j=0; j<=n; j++){
    		printf("%f\t", a[i][j]);
    	}
    	printf("\n");
    }*/
    
    gauss_t1 = omp_get_wtime();
    printf("\n=> Performing Gaussian elimination\n");
    for(i=0; i<n-1; i++){
    #pragma omp parallel for shared(i, temp, n, a) private(j, k) schedule(static, 150)        
        for(j=i+1; j<n; j++){
            temp = (a[j][i])/(a[i][i]);    
            /* Create a upper triangular matrix */
            for(k=0; k<=n; k++){
                a[j][k] = a[j][k] - temp*a[i][k];
            }
        }
    }
    gauss_t2 = omp_get_wtime();

    /*printf("\nAfter Gauss elimination matrix\n");
    for(i=0; i<n; i++){
    	for(j=0; j<=n; j++){
    		printf("%f\t", a[i][j]);
    	}
    	printf("\n");
    }*/
    
    back_t1 = omp_get_wtime();
    printf("\n=> Performing Back substitution\n");
    #pragma omp parallel shared(x, n, a) private(i, j)       
    {
        #pragma omp for schedule(static, 150)
        for(i=n-1; i>=0; i--){
            /* Start from the last row and go upwards */
            x[i] = a[i][n];
            for(j=0; j<n; j++){
                if(j!=i) x[i] -= x[j]*a[i][j];
            }
            x[i] = x[i]/a[i][i];
        }
    }
    back_t2 = omp_get_wtime();

    /*printf("\nAfter back substitution, values of variable array x are\n");
    for(i=0; i<n; i++){
    	printf("%f\t", x[i]);
    } printf("\n");*/

    max_error = x[0] - 1;
    for(i=1; i<n; i++){
        temp = x[i] - 1;
        if(max_error<temp) max_error = temp;
    }

    total_t2 = omp_get_wtime();

    printf("\nMax error in solution = %.9f\n", max_error);
    printf("\nTime for Gaussian elim = %.9f seconds\n", gauss_t2 - gauss_t1); 
    printf("\nTime for back sub = %.9f seconds\n", back_t2 - back_t1); 
    printf("\nTotal Time for solve = %.9f seconds\n", total_t2 - total_t1); 
    return 0;	
}
