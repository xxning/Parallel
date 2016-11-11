#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <time.h>

#define DEBUG(X) printf(X);
#define DEAD 0
#define BIRTH 1

typedef int bool;

typedef struct Report {
  int row;
  int col;
  int change;
} Report;

void printM ();
void check(int,int,int*);

bool** matrix;
Report* reports;
Report* rBuff;
int  numtasks,rank, len, rc,iter,row,col;
int  size,slide,res,rowm,colm,rowM,colM;

int main(int argc, char *argv[]) {

  //Variables
  int  i,j,k;
  char hostname[MPI_MAX_PROCESSOR_NAME];

  //MPI init
  rc = MPI_Init(&argc,&argv);
  if (rc != MPI_SUCCESS) {
    printf ("Error starting MPI program. Terminating.\n");
    MPI_Abort(MPI_COMM_WORLD, rc);
  }


  //Get mpi info
  MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  MPI_Get_processor_name(hostname, &len);

  //print info message
  if(rank == 0)
    printf ("Number of tasks= %d My rank= %d Running on %s\n",
            numtasks,rank,hostname);

  //Open the input file
  FILE *file;
	if(argc<3){
		printf("Error parameter!\n");
		exit(1);
	}
		
  //file = fopen(argv[1], "r");

  //Read column and row sizes
  //fscanf(file,"%i %i",&row,&col);
	row=atoi(argv[1]);
	col=atoi(argv[2]);
  //Allocate memory for the matrix
  matrix=malloc(row*sizeof(bool*));
  for(i=0; i<row; i++) matrix[i] = malloc(col*sizeof(bool));

  //Fill the matrix with info
	srand((unsigned)time(NULL));
	double tt;
  for(i=0; i<row; i++)
    for(j=0; j<col; j++){
			tt=rand()/(RAND_MAX+1.0);
			if(tt>0.5)
				matrix[i][j]=1;
			else
				matrix[i][j]=0;
		}
      

  //Read iterations
  //fscanf(file,"%i",&iter);
	iter=atoi(argv[3]);
  //Make room for the reports
  reports      = malloc(row*col*sizeof(Report));
  rBuff        = malloc(row*col*sizeof(Report));
  int * counts = malloc(numtasks*sizeof(int));
  int * offs   = malloc(numtasks*sizeof(int));
  //Make the division of the problem
  size = row*col;
  slide = size / numtasks;
  res   = size % numtasks;
  rowm  = slide*rank / col;
  colm  = slide*rank % col;
  if (rank != numtasks){
    rowM  = slide*(rank+1) / col;
    colM  = slide*(rank+1) % col;
  }else{
    rowM = row - 1;
    colM = col;
  }


  /* printf("Am %i size %i\n" */
  /*        "      slide %i\n" */
  /*        "      rowm %i\n" */
  /*        "      colm %i\n" */
  /*        "      rowM %i\n" */
  /*        "      colM %i\n",rank,size,slide,rowm,colm,rowM,colM); */
  int changes, count,off;
	double time = MPI_Wtime();
  for(i = 0; i < iter; i++){
    count   = 0;
    changes = 0;
    /* if(rank == 0) printf("cicle: %i\n", i); */
    for(j=colm;j<col  && rowm != rowM;j++) check(rowm,j,&changes);
    for(j=0   ;j<colM && rowm != rowM;j++) check(rowM,j,&changes);
    for(j=colm;j<colM && rowm == rowM;j++) check(rowm,j,&changes);
    for(j=rowm+1;j<rowM;j++)
      for(k=0;k<col;k++)
        check(j,k,&changes);

    MPI_Allgather(&changes,1,MPI_INT,
                  counts ,1,MPI_INT,
                  MPI_COMM_WORLD);
    off = 0;
    for(j=0;j<numtasks;j++){
      offs[j]=off;
      count += counts[j];
      counts[j] *= 3;
      off  += counts[j];
    }
    MPI_Allgatherv(reports,3*changes, MPI_INT,
                   rBuff  ,counts, offs,
                   MPI_INT ,MPI_COMM_WORLD);

    for(j=0;j<count;j++){
      if(rBuff[j].change == DEAD ) matrix[rBuff[j].row][rBuff[j].col] = DEAD;
      if(rBuff[j].change == BIRTH) matrix[rBuff[j].row][rBuff[j].col] = BIRTH;
    }
  }
	time = MPI_Wtime()-time;
  printf("time= %lf s\n",time);
  //if(rank == 0) printM();
  MPI_Finalize();
}

void check(int r,int c, int * changes){
  int count  = 0;
  int eleft  = !(c == 0);
  int eup    = !(r == 0);
  int eright = !(c == col-1);
  int edown  = !(r == row -1);

  if (eleft)  count += matrix[r][c-1];
  if (eup)    count  += matrix[r-1][c];
  if (eright) count += matrix[r][c+1];
  if (edown)  count += matrix[r+1][c];

  if (eup && eleft)    count += matrix[r-1][c-1];
  if (eup && eright)   count += matrix[r-1][c+1];
  if (edown && eleft)  count += matrix[r+1][c-1];
  if (edown && eright) count += matrix[r+1][c+1];

  if (matrix[r][c] == 1 && (count > 3 || count < 2)){
    /* printf("muere"); */
    reports[*changes].row = r;
    reports[*changes].col = c;
    reports[*changes].change = DEAD;
    (*changes)++;
  }

  if((matrix[r][c] == 0) && (count == 3)){
    /* printf("vive"); */
    reports[*changes].row = r;
    reports[*changes].col = c;
    reports[*changes].change = BIRTH;
    (*changes)++;
  }
  /* printf("%i %i %i\n",r,c,count); */
  /* if((c == 5) && (r == 4)) */
  /*   printf("Up: %i Down: %i Left: %i Right: %i count: %i \n", */
  /*          matrix[r-1][c],matrix[r+1][c], */
  /*          matrix[r][c-1],matrix[r][c+1],count); */

}
void printM(){
  int i,j;

  for(i=0; i<row; i++){
    for(j=0; j<col; j++)
      printf("%i ",matrix[i][j]);
    printf("\n");
  }
}
