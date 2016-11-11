#include<stdio.h>
#include<stdlib.h>
#include<omp.h>
#include<string.h>
#include<time.h>
 
int **cell;
int **temp;
int step;
void output();
int height,width;

int main(int argc,char *argv[]){
	
	if (argc < 3){
		printf("Wrong arguments\n");
		exit(0);
	}
	
	height=atoi(argv[1]);
	width=atoi(argv[2]);
	step=atoi(argv[3]);
	int i,j,k;
	cell=(int**)malloc((height+2)*sizeof(int*));
	temp=(int**)malloc((height+2)*sizeof(int*));	
	for(i=0;i<(height+2);i++){
		cell[i]=(int*)malloc((width+2)*sizeof(int));
		temp[i]=(int*)malloc((width+2)*sizeof(int));
	}

	double tt;
	double start, finish;
	
	for(i=0;i<(height+2);i++)
		cell[i][0]=cell[i][width+1]=0;
	for(i=0;i<(width+2);i++)
		cell[0][i]=cell[height+1][i]=0;	

	srand((unsigned)time(NULL));
	for(i=1;i<=height;i++)
		for(j=1;j<=width;j++){
			tt=rand()/(RAND_MAX+1.0);
			if(tt>0.7)
				cell[i][j]=1;
			else
				cell[i][j]=0;
		}
	int count;
	int numthreads;
	if(argc==4){
		numthreads=atoi(argv[4]);
		omp_set_num_threads(numthreads);
	}
	start = omp_get_wtime();
	for(i=0;i<step;i++)	{
	#pragma omp parallel shared(cell,temp) private(j,k,count)
	{
		
		for(j=1;j<=height;j++){
			for(k=1;k<=width;k++){
				count=0;
				if(cell[j-1][k-1]==1)
					count++;
				if(cell[j-1][k]==1)
					count++;
				if(cell[j-1][k+1]==1)
					count++;
				if(cell[j][k-1]==1)
					count++;
				if(cell[j][k+1]==1)
					count++;
				if(cell[j+1][k-1]==1)
					count++;
				if(cell[j+1][k]==1)
					count++;
				if(cell[j+1][k+1]==1)
					count++;
				
				if(cell[j][k]==1){
					if(count==2||count==3)
						temp[j][k]=1;
					else
						temp[j][k]=0;
				}
				else{
					if(count==3) 
						temp[j][k]=1;
					else
						temp[j][k]=0;
				}
			}
		}
	
		#pragma omp barrier
		for(j=1;j<=height;j++)
			for(k=1;k<=width;k++)
				cell[j][k]=temp[j][k];
	}
	}
	
	finish = omp_get_wtime();
	output();
    printf("%lf\n", finish - start);

	return 0;
}
				
void output(){
	int i,j;
	for(i=1;i<=height;i++){
		for(j=1;j<=width;j++){
			printf("%d ",cell[i][j]);
		}
		printf("\n");
	}
}		

	
			
