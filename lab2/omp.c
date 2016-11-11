#include<stdio.h>
#include<stdlib.h>
#include<omp.h>
#include<string.h>
 

int main(int argc, char** argv){

if (argc < 4){
	printf("Wrong arguments\n");
	exit(0);
}

int num_threads, num_steps;
FILE *f_in, *f_out;

f_in = fopen(argv[3],"r");
f_out = fopen(argv[4], "w");
num_threads = atoi(argv[1]);
num_steps = atoi(argv[2]);
omp_set_num_threint ads(num_threads);

char tip_reprezentare;
int W, H, W_HARTA, H_HARTA;
int i, j, k, count, p, q, H_aux, W_aux;

fscanf(f_in, "%c %d %d %d %d", &tip_reprezentare, &W_HARTA, &H_HARTA, &W, &H);

/*harta si harta_aux au dimensiunile H+2 x W+2 deoarece
 se adauga 2 linii si 2 coloane in plus pentru 
 bordare in functie de tipul reprezentarii */
int harta[H + 2][W + 2], harta_aux[H + 2][W + 2];
int harta_in[H_HARTA][W_HARTA];

//citesc in harta_in matricea din fisier
for (i = 0; i < H_HARTA; i++)
	for (j = 0; j < W_HARTA; j++)	
		fscanf(f_in, "%d", &harta_in[i][j]);

/*daca dimensiunile hartii ce trebuie simulata sunt mai
 mici decat ale hartii citite se vor salva in variabila
 harta si harta_aux primele H linii si W coloane din harta_in */
if (H_HARTA > H && W_HARTA > W){
	for (i = 0; i < H; i++)
		for (j = 0; j < W; j++){
			harta[i + 1][j + 1] = harta_in[i][j];
			harta_aux[i + 1][j + 1] = harta_in[i][j];
		}
			
} else
	/*daca dimensiunile hartii simulate sunt mai mari atunci
	 se copiaza elementele din harta_in dupa care se 
	 completeaza matricea cu 0  */ 
	for (i = 1; i < H + 2; i++)
		for(j = 1; j < W + 2; j++){
			if (i <= H_HARTA && j <= W_HARTA){
				harta[i][j] = harta_in[i - 1][j - 1];
				harta_aux[i][j] = harta[i][j];
			}
			else {
				harta[i][j] = 0;
				harta_aux[i][j] = 0;
			}
		}
	

//bordez matricea in functie de tipul reprezentarii
//pentru plan bordez cu zero-uri
if (tip_reprezentare == 'P'){
	for (j = 0; j < W + 2; j++){
		harta[0][j] = 0;
		harta_aux[0][j] = 0;
		harta[H + 1][j] = 0;
		harta_aux[H + 1][j] = 0;
	}
	for ( i = 0; i < H + 2; i++){
		harta[i][0] = 0;
		harta_aux[i][0] = 0;
		harta[i][H + 1] = 0;
		harta[i][H + 1] = 0;
	}
}
//pentru toroid bordez cu ultima si prima linie respectiv coloana
if (tip_reprezentare == 'T'){
	for (j = 1; j < W + 1; j++){
		//copiez prima si ultima linie
		harta[H + 1][j] = harta[1][j];
		harta[0][j] = harta[H][j];
		harta_aux[H + 1][j] = harta_aux[1][j];
		harta_aux[0][j] = harta_aux[H][j];
	}
	//copiez prima si ultima coloana
	for (i = 0; i < H + 2; i++){
		harta[i][0] = harta[i][W];
		harta[i][W + 1] = harta[i][1];
		harta_aux[i][0] = harta_aux[i][W];
		harta_aux[i][W + 1] = harta_aux[i][1];
	}

} 


for (k = 0; k < num_steps; k++){
	#pragma omp parallel for collapse(2) private(i,j,count)
	for (i = 1; i < W + 1; i++){
		for (j = 1; j < H + 1; j++){
			//pentru fiecare element contorizez vecinii
			count = 0;
			if (harta[i - 1][j - 1] == 1)
				count++;
			if (harta[i - 1][j] == 1)
				count++;
			if (harta[i - 1][j + 1] == 1)
				count++;
			if (harta[i][j + 1] == 1)
				count++;
			if (harta[i + 1][j + 1] == 1)
				count++;
			if (harta[i + 1][j] == 1)
				count++;
			if (harta[i + 1][j - 1] == 1)
				count++;
			if (harta[i][j - 1] == 1)
				count++;
			
			//in functie de numarul vecinilor actualizez valoarea harta_aux[i][j]
			if (count == 3)
				harta_aux[i][j] = 1;	
			if (count < 2 || count > 3)
				harta_aux[i][j] = 0;
		}
	}
		//in cazul reprezentarii de toroid voi borda matricea cu noile valori
		if (tip_reprezentare == 'T'){
			for (p = 1; p < W + 1; p++){
				//copiez prima si ultima linie
				harta_aux[H + 1][p] = harta_aux[1][p];
				harta_aux[0][p] = harta_aux[H][p];
			}	
			for (q = 0; q < H + 2; q++){
				//copiez prima si ultima coloana
				harta_aux[q][0] = harta_aux[q][W];
				harta_aux[q][W + 1] = harta_aux[q][1];
			}

		} 
		//actualizez matricea pentru pasul urmator
		memcpy(&harta, &harta_aux, sizeof(harta));

}

//decuparea matricei incepand din coltul dreapta jos
// W_aux si H_aux reprezinta coltul pe a carui linie
// si/sau coloana se afla cea mai departata valoare de 1 

H_aux = 0;
W_aux = 0;
for (i = H; i > 0; i--)
	for (j = W; j > 0; j--){
		if (harta[i][j] == 1){
			if ( i >= H_aux)
				H_aux = i;
			if ( j >= W_aux)
				W_aux = j;	
		}	
	}

//scrierea in fisier a rezultatului
fprintf(f_out, "%c %d %d %d %d\n", tip_reprezentare, W_aux, H_aux, W, H);

for (i = 1; i <= H_aux; i++){
	for (j = 1; j <= W_aux; j++)
		fprintf(f_out, "%d ", harta[i][j]);
	fprintf(f_out, "\n");
}



fclose(f_in);
fclose(f_out);
  

return 0;
}
