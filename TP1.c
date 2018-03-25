#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>


int main(int argc,char **argv)
{
    int numProcess, n, myId;
    double startTime, endTime;

    void generatesMatrix(int m[], int n); //It includes the method
    void generatesVector(int v[], int n); //It includes the method

    MPI_Init(&argc, &argv); //It starts MPI

    MPI_Comm_size(MPI_COMM_WORLD, &numProcess); //MPI stores in numProcess the quantity of processes

    MPI_Comm_rank(MPI_COMM_WORLD, &myId); //MPI stores in myId the id of the current process

    if(myId == 0){
        //It controls the user cannot enter wrong data
        do {
            printf("Type n, which is the dimension of the matrix\n");
            scanf("%d", &n); //It is the length of each row and the number of rows
        } while(n<numProcess | n%numProcess!=0);

		int m[n*n], v[n]; //It is the n*n matrix(array) and the n array
        generatesMatrix(m, n*n);
        generatesVector(v, n);

    } else {

    }

    MPI_Finalize(); //It ends MPI
    return 0;
}


// Method wich generates the long matrix
void generatesMatrix(int m[], int n){
    int i;
    for (i = 0; i < n; i++) {
        m[i] = (int) rand() / (int) (((unsigned) RAND_MAX + 1) / 10);
        //printf("Soy de 0 a 9\n%d\n",m[i]);
    }
}

void generatesVector(int v[], int n){
    int i;
    for (i = 0; i < n; i++) {
        v[i] = (int) rand() / (int) (((unsigned) RAND_MAX + 1) / 6);
        //printf("Soy de 0 5\n%d\n",v[i]);
    }
}
