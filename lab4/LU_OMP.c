#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <time.h>

int size;
double **l;
double **u;
double **mat;
double **matcpy;
int s;
double begin, end;
FILE *fp;

void printMatrix(double **m) {
    printf("\n \n");

	int i =0, j=0;
    for (i=0;i<size;i++) {
        for(j=0;j<size;j++) {
            printf("%.0f ",m[i][j]);
        }
        printf("\n");
    }
}

void freeMatrix(double **matrix)
{
	int i =0;
    for(i = 0; i < size; i++)
        free(matrix[i]);
    free(matrix);
}


int decompose(double **m) {
	
	int i=0, j=0, k=0;
	double factor;
	int tid;
#pragma omp parallel shared(j) private(i,k,tid)
		for(j =0; j < size-1; j++) {
tid=omp_get_thread_num();
	
	//printf("%d\n",tid);
        for(i = j+tid+1; i < size; i=i+s) {
            factor = m[i][j]/m[j][j];
            for(k = 0; k < size; k++) {
            	u[i][k]= m[i][k] - (m[j][k] * factor);
            }
            l[i][j] = factor;
        }
		
        //matrix copy m = u
        for(i =0; i< size; i++) {
        	for(k=0; k < size; k++) {
        		m[i][k] = u[i][k];
        	}
        }
        //copy end
		
    }
	//freeMatrix(mat);
    for(i = 0; i < size; i++) {
        l[i][i] =1;
    }
 
    return 1;
}

int generateMatrix(int size)
{
    srand(time(NULL));
    mat = malloc(size * (sizeof *mat));
    u = malloc(size * (sizeof *u));
    l = malloc(size * (sizeof *l));
    //matcpy = malloc(size * (sizeof *matcpy));

    int i =0;
    for(i = 0; i<size;i++)
    {
    	u[i] = malloc((sizeof *u[i]) * size);
    	l[i] = malloc((sizeof *l[i]) * size);
    	mat[i] = malloc((sizeof *mat[i]) * size);
    }
    for( i=0 ; i < size ; i++) {
    	u[i] = malloc((sizeof *u[i]) * size);
    	l[i] = malloc((sizeof *l[i]) * size);
        mat[i] = malloc((sizeof *mat[i]) * size);
        //matcpy[i] = malloc((sizeof *matcpy[i]) * size);
		
        int j = i;
        for( j=i; j< size; j++) {
            mat[i][j] = i+1;
            mat[j][i] = i+1;
            u[i][j] = mat[i][j];
            u[j][i] = mat[j][i];
        }
		
		
    }

    return 1;
}


int main(int argc, char * argv[])
{
    if(argc < 2) {
    	printf("Matrix Size missing in the arguments \n");
    	return -1;
    }

    size = strtol(argv[1],(char **)NULL,10);
    fp = fopen("output.txt","a+");

    generateMatrix(size);
  	//setting number of threads
  	int nthreads = omp_get_max_threads();
	//nthreads=2;
  	 s = atoi(argv[2]);

  	//for(s=1 ; s <= nthreads; s= s*2)
  	{
  		omp_set_num_threads(s);
  		//omp_set_nested(1);
begin =omp_get_wtime();
		decompose(mat);
   end = omp_get_wtime();
  		int current_threads = omp_get_num_threads();
		
  		fprintf(fp,"%d ", size);
  		fprintf(fp,"%d ",s);
  		fprintf(fp,"%f\n", end-begin);

  	}
	//printMatrix(mat);
	//printMatrix(l);
	//printMatrix(u);

  	fclose(fp);
    return 0;
}
