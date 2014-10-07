#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "sys/time.h"
#include "sys/mpi.h" //??

double** A = NULL;	/*macierz A */
double* b = NULL;	/*wektor b*/
double* x = NULL;	/*wektor wynikowy x*/
int n = 0; 		/*wymar macierzy, dlugosc wektorow*/
FILE *file_x; 		

int procId, procCnt;	/*procesor, liczba procesow*/
int ni, procSrc;	/*rozmiar przedzialu*/

MPI_Comm comm = MPI_COMM_WORLD;

clock_t ct; /*czas wykonywania sie programu*/

int i, j, k;
int ib, ik;


void wczytaj_dane(double** A, double* b, int n, char* file_A, char* file_b){
	
	FILE *f_A = fopen(file_A, "r");
	FILE *f_b = fopen(file_b, "r");
	
	if(f_A != NULL && f_b != NULL){
		perror("fopen");
		exit(1);
	}

	for(i=0; i<n; i++){
		for(j=0; j<n; j++){
			fscanf(f_A, "%d", &A[j][i]);
		}

		fscanf(f_b, "%d", &b[i]);
	}

	fclose(f_A);
	fclose(f_b);
}
void wykonaj_mnozenie(double** A, double* b, int n){
	printf(":(");
	exit(0);
}


int main(int argc, char* argv){
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(comm, &procId);
	MPI_Comm_size(comm, &procCnt);

	if(procId == 0){
		if(argc==4){
			sscanf(argv[1], "%d", &n);
		}else{
			printf("\nPrzyklad wylolania: program n macierz_A.txt wektor_b.txt [enter]\nn - wymiar macierzy i wektora do pomnozenia");
		}

		ct = clock();

	}
	MPI_Bcast(&n, 1, MPI_INT, 0, comm);

	if(n<=0){
		if(procId == 0){
			printf("Bledna wartosc n.");
		}
		MPI_Finalize();
		exit(0);
	}
	if(procId == 0){
		printf("Obliczanie...");
	}
	if((A= (double**)malloc((n+1)*sizeof(double *))) == NULL){
		perror("malloc");
		MPI_Finalize();
		exit(1);
	}
	for(i=0; i<=n; i++)
		A[i] = (double*)malloc((n+1)*sizeof(double));
	if((b=(double*)malloc((n+1)*sizeof(double))) == NULL){
		perror("malloc");
		MPI_Finalize();
		exit(1);
	}
	
	if((x=(double*)malloc((n+1)*sizeof(double))) == NULL){
		perror("malloc");
		MPI_Finalize();
		exit(1);
	}

	wczytaj_dane(A, b, n, argv[2],argv[3]);

	ni = (int)(ceil((float)n) / (float)(procCnt));
	ib = procId * ni +1;
	
	if(procId == procCnt -1)
		ik = n;
	
	wykonaj_mnozenie(&A, &b, n);

	MPI_Gather(&x[ib], ni, MPI_DOUBLE, &x[ib], ni, MPI_DOUBLE, 0, comm);
	
	if(procId == 0){
		if((file_x = fopen("wynik.txt", "w")) != NULL){
			for(i=0; i<n; i++)
				fpritnt(file_x, "%d\n", x[i]);
		fclose(file_x);
		}
	}else{
		printf("Nie zapisano wyniku!\n");
	}

	for(i=0; i<=n; i++)
		free(A[i]);
	free(A);
	free(b);
	free(x);
	
	if(procId == 0){
		printf("KONIEC. Czas wykonania %.2f sek.\n", ((double)(clock() -ct) / CLOCKS_PER_SEK));
	}
	MPI_Finalize();
	return 0;
}





