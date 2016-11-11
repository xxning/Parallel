#include<stdio.h>
#include<stdlib.h>
#include<omp.h>
#include<time.h>

#define RANDOM(x) (rand() % x)

#define N 1000

int a[N][N];
int b[N][N];
long long c[N][N];

int main(int argc, char* argv[]) {
        int n = N;

        int i, j, k;
        double start, finish;

        srand(time(NULL));

        for (i = 0; i < n; i++) {
                for (j = 0; j < n; j++) {
                        a[i][j] = RANDOM(100);
                        b[i][j] = RANDOM(100);
                }
        }

        long num_threads = 4;
        if (argc == 2) {
                num_threads = atoi(argv[1]);
        }

        omp_set_num_threads(num_threads);

        start = omp_get_wtime();

        #pragma omp parallel for shared(a, b, c) private (i, j, k)
        for (i = 0; i < n; i++) {
                for (j = 0; j < n; j++) {
                        c[i][j] = 0;
                        for (k = 0; k < n; k++) {
                                c[i][j] += a[i][k] * b[k][j];
                        }
                }
        }
        finish = omp_get_wtime();
        printf("%lf\n", finish - start);

        return 0;
}