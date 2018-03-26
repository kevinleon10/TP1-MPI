#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
    int numProcess, n, myId;
    double startTime, endTime;

    // They include the methods
    void generatesMatrix(int m[], int n);
    char *getMatrix(int m[], int n, char name[1]);
    void generatesVector(int v[], int n);
    char *getVector(int v[], int n, char name[1]);
    void postResults(int m[], int v[], int q[], int p[], int b[], int n, int numProcess, int tp, double time, double processTime);
    void writeResults(int m[], int v[], int q[], int p[], int b[], int n, int numProcess, int tp, double time, double processTime);

    MPI_Init(&argc, &argv); //It starts MPI

    MPI_Comm_size(MPI_COMM_WORLD, &numProcess); //MPI stores in numProcess the quantity of processes

    MPI_Comm_rank(MPI_COMM_WORLD, &myId); //MPI stores in myId the id of the current process

    if (myId == 0) {
        //It controls the user cannot enter wrong data
        printf("\nType n, which is the dimension of the matrix.\n");
        scanf("%d", &n); //It is the length of each row and the number of rows
        while (n < numProcess || n % numProcess != 0) {
            printf("\nThe n must be multiple of the number of processes!!\n");
            printf("Type n, which is the dimension of the matrix.\n");
            scanf("%d", &n); //It is the length of each row and the number of rows
        }
        int m[n * n], v[n]; //It is the n*n matrix(array) and the n array
        generatesMatrix(m, n * n);
        generatesVector(v, n);

        postResults(m, v, m, v, m, n, numProcess, 0, 0, 0);

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
char *getMatrix(int m[], int n, char name[1]) {
    int i, j, counter;
    char *matrix = malloc(1000);
    strcpy(matrix, "\n-> Matrix ");
    strcat(matrix, name);
    for (i = 0; i < n; i++) {
        strcat(matrix, "\nRow ");
        char number[4];
        snprintf(number, 4, "%d", i + 1);
        strcat(matrix, number);
        strcat(matrix, ":");
        //printf("\nRow %d:", i+1);
        if (i < 9) {
            strcat(matrix, " ");
            //printf(" ");
        }
        for (j = 0; j < n; j++) {
            strcat(matrix, " ");
            char str[2];
            snprintf(str, 2, "%d", m[i + j]);
            strcat(matrix, str);
            //printf(" %d ", m[i + j]);
        }
    }
    strcat(matrix, "\n");
    return matrix;
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
char *getVector(int v[], int n, char name[1]) {
    int i;
    char *vector = malloc(300);
    strcpy(vector, "\n-> Vector ");
    strcat(vector, name);
    strcat(vector, "\n");
    //printf("\n-> The Vector %c\n", name);
    char number[2];
    for (i = 0; i < n; i++) {
        strcat(vector, " ");
        snprintf(number, 2, "%d", v[i]);
        strcat(vector, number);
        // printf(" %d ", v[i]);
    }
    strcat(vector, "\n");
    return vector;
    //printf("\n");
}

// Method which generates a new File, with the respective results
void writeResults(int m[], int v[], int q[], int p[], int b[], int n, int numProcess, int tp, double time,double processTime) {
    FILE *f = fopen("results.txt", "w");
    char *text = malloc(2000);
    strcpy(text, "RESULTS\n");
    fprintf(f, "%s", text);
    strcpy(text, getMatrix(m,n,"M"));
    fprintf(f, "%s", text);
    strcpy(text, getVector(v,n,"V"));
    fprintf(f, "%s", text);
    strcpy(text, getVector(q,n,"Q"));
    fprintf(f, "%s", text);
    strcpy(text, getVector(p,n,"P"));
    fprintf(f, "%s", text);
    strcpy(text, getMatrix(b,n,"B"));
    fprintf(f, "%s", text);
    fclose(f);
}

// Method which post the Results, in console or in a file
void postResults(int m[], int v[], int q[], int p[], int b[], int n, int numProcess, int tp, double totalTime, double processTime) {
    printf("\nRESULTS\n\n");
    printf("->Value of n: %d\n", n);
    printf("->Total of processes: %d\n", numProcess);
    printf("->Total of prime numbers in M (tp): %d\n", tp);
    printf("->Total time: %d\n", totalTime);
    printf("->Total time, without the times to write results: %d\n", processTime);
    if (n <= 100) {
        printf(getMatrix(m, n, "M"));
        printf(getVector(v, n, "V"));
        printf(getVector(q, n, "Q"));
        printf(getVector(p, n, "P"));
        printf(getMatrix(b, n, "B"));
    } else {
        writeResults(m, v, q, p, b, n, numProcess, tp, totalTime, processTime);
    }
}



