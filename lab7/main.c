#include<stdio.h>
#include<omp.h>
#include<stdlib.h>

int main(int argc, char *argv[]) {
	int n = atoi(argv[1]);
	int i, j;
	double a[n][n+1];
	double temp;

	printf("\nFilling up the matrix\n");
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

	printf("\nShowing the output matrix\n");
	for(i=0; i<n; i++){
		for(j=0; j<=n; j++){
			printf("%.6f\t", a[i][j]);
		}
		printf("\n");
	}

	return 0;	
}
