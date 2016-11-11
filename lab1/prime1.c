#include <stdio.h>
#include <omp.h>
#include <math.h>
#include <stdlib.h>

int MIN=2;
//#define MAX 1000000
int MAX;

#define PATH "prime0.txt"

int isPrime(int num) {
        int i,t;
	t=sqrt(num);
        for (i = 2; i <= t; i++) {
                if (num % i == 0) {
                        return 0;
                }
        }
        return 1;
}

int main(int argc, char* argv[]) {

        FILE* fp = NULL;
        int num, s = 0;
        double start, finish;
	
        fp = fopen(PATH, "w");
	MAX=1000000;
        long num_threads = 4;
        if (argc == 2) {
                num_threads = atoi(argv[1]);
        }
	if(argc==3){
		MAX=atoi(argv[2]);
	}
        omp_set_num_threads(num_threads);

        start = omp_get_wtime();
    
        int min = MIN % 2 ? MIN : MIN + 1;
        //int max = MAX % 2 ? MAX : MAX - 1;
	//int min=MIN;
	int max=MAX;
	printf("%d %d\n",max,min);
        #pragma omp parallel for private(num) reduction(+:s)
        for (num = min; num <= max; num+=2) {
                if (isPrime(num)) {
                        fprintf(fp, "%d\n", num);
                        s++;
                }
        }
	s++;
        finish = omp_get_wtime();
        printf("%d\n", s);
        printf("%lf\n", finish - start);

        return 0;
}
