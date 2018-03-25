#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>


int main(int argc, char **argv) {
    int numProcess, n, myId;
    double startTime, endTime;

    // They include the methods
    void generatesMatrix(int m[], int n);
    void printMatrix(int m[], int n, char name);
    void generatesVector(int v[], int n);
    void printVector(int v[], int n, char name);

    MPI_Init(&argc, &argv); //It starts MPI

    MPI_Comm_size(MPI_COMM_WORLD, &numProcess); //MPI stores in numProcess the quantity of processes

    MPI_Comm_rank(MPI_COMM_WORLD, &myId); //MPI stores in myId the id of the current process

    if (myId == 0) {
        //It controls the user cannot enter wrong data
        printf("Type n, which is the dimension of the matrix\n");
        scanf("%d", &n); //It is the length of each row and the number of rows
        while (n < numProcess || n % numProcess != 0) {
            printf("The n must be multiple of the number of processes!!\n");
            printf("Type n, which is the dimension of the matrix\n");
            scanf("%d", &n); //It is the length of each row and the number of rows
        }
        int m[n * n], v[n]; //It is the n*n matrix(array) and the n array
        generatesMatrix(m, n * n);
        generatesVector(v, n);
        printMatrix(m, n, 'M');
        printVector(v, n, 'V');
    } else {

    }

    MPI_Finalize(); //It ends MPI
    return 0;
}


// Method which generates a matrix
void generatesMatrix(int m[], int n) {
    int i;
    for (i = 0; i < n; i++) {
        m[i] = (int) rand() / (int) (((unsigned) RAND_MAX + 1) / 10);
        // printf("Soy de 0 a 9\n%d\n", m[i]);
    }
}

// Method which print a matrix
void printMatrix(int m[], int n, char name) {
    int i;
    int j;
    int counter;
    printf("\n-> The Matrix %c", name);
    for (i = 0; i < n; i++) {
        printf("\nRow %d:", i + 1);
        if (i < 9) {
            printf(" ");
        }
        for (j = 0; j < n; j++) {
            printf(" %d ", m[i + j]);
        }
    }
    printf("\n\n");
}

// Method which generates a vector
void generatesVector(int v[], int n) {
    int i;
    for (i = 0; i < n; i++) {
        v[i] = (int) rand() / (int) (((unsigned) RAND_MAX + 1) / 6);
        // printf("Soy de 0 5\n%d\n", v[i]);
    }
}

// Method which print a vector
void printVector(int v[], int n, char name) {
    int i;
    printf("\n-> The Vector %c\n", name);
    for (i = 0; i < n; i++) {
        printf(" %d ", v[i]);
    }
    printf("\n\n");
}