/*
// Created by Kevin León & Renato Mainieri on 25/03/2018.
*/
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <fstream>
#include <string>
#include <iostream>

#define MAX 1000

using namespace std;

class VectorManager {
public:
    void generatesVector(int V[], int n);

    void postResults(int M[], int V[], int Q[], int P[], int B[], int n, int numProcess, int tp, double time,
                     double processTime);

    int productPoint(int row[], int column[], int n_bar);

    string getMatrix(int M[], int n, string name);

    string getVector(int V[], int n, string name);

private:


    void writeResults(int M[], int V[], int Q[], int P[], int B[], int n, int numProcess, int tp, double time,
                      double processTime);
};

// Method which generates a matrix
void VectorManager::generatesVector(int V[], int n) {
    int i;
    srand(time(NULL));
    for (i = 0; i < n; i++) {
        V[i] = rand() % 10;
    }

}

// Method which post the Results, in console or in a file
void
VectorManager::postResults(int M[], int V[], int Q[], int P[], int B[], int n, int numProcess, int tp, double totalTime,
                           double processTime) {
    cout << "\nRESULTS\n" << endl;
    cout << "-> Value of n: " << to_string(static_cast<long long int>(n)) << endl;
    cout << "-> Total of processes: " << to_string(static_cast<long long int>(numProcess)) << endl;
    cout << "-> Total of prime numbers in M (tp): " << to_string(static_cast<long long int>(tp)) << endl;
    cout << "-> Total time: " << to_string(static_cast<long long int>(totalTime)) << endl;
    cout << "-> Total time, without the times to write results: " << to_string(static_cast<long long int>(processTime))
         << endl;
    if (n <= 100) {
        cout << getMatrix(M, n, "M") << endl;
        cout << getVector(V, n, "V") << endl;
        cout << getVector(Q, n, "Q") << endl;
        cout << getVector(P, n, "P") << endl;
        cout << getMatrix(B, n, "B") << endl;
        cout << endl;
    } else {
        writeResults(M, V, Q, P, B, n, numProcess, tp, totalTime, processTime);
    }

}

// Method which return a product point of two vectors
int VectorManager::productPoint(int row[], int column[], int n_bar) {
    int result = 0;
    for (int i = 0; i < n_bar; i++) {
        result = result+ row[i] * column[i];
    }
    return result;
}

// Method which print a vector
string VectorManager::getVector(int V[], int n, string name) {
    string vector = "";
    vector += "\n-> VECTOR ";
    vector += name;
    vector += "\n";
    for (int i = 0; i < n; i++) {
        vector += " ";
        vector += to_string(static_cast<long long int>(V[i]));
    }
    return vector;
}

// Method which print a matrix
string VectorManager::getMatrix(int M[], int n, string name) {
    int i, j;
    string matrix = "";
    matrix += "\n-> MATRIX ";
    matrix += name;
    for (i = 0; i < n; i++) {
        matrix += "\nRow ";
        matrix += to_string(static_cast<long long int>(i + 1));
        matrix += ":";
        if (i < 9) {
            matrix += " ";
        }
        for (j = 0; j < n; j++) {
            matrix += " ";
            matrix += to_string(static_cast<long long int>(M[i + j]));
        }
    }
    return matrix;
}

// Method which generates a new File, with the respective results
void
VectorManager::writeResults(int M[], int V[], int Q[], int P[], int B[], int n, int numProcess, int tp, double time,
                            double processTime) {
    string input;
    input = "RESULTS";
    ofstream out("results.txt");
    out << input;
    out << endl;
    input = getMatrix(M, n, "M");
    out << input;
    out << endl;
    input = getVector(V, n, "V");
    out << input;
    out << endl;
    input = getVector(Q, n, "Q");
    out << input;
    out << endl;
    input = getVector(P, n, "P");
    out << input;
    out << endl;
    input = getMatrix(B, n, "B");
    out << input;
    out.close();
}


int main(int argc, char **argv) {
    int numProcess, n, myId, tp, localTp;
    int n_bar;        /*  Se calculara como  n/p, es decir es el numero de
                           elementos que le corresponde a cada proceso de cada vector */
    double startTime, endTotalTime, endProcessTime; // MPI_Wtime()
    int M[MAX * MAX];
    int localM[MAX * MAX]; //  Aca recibe cada proceso la parte de M que le corresponde
    int V[MAX], Q[MAX], P[MAX];
    int localQ[MAX];
    int localV[MAX];

    VectorManager vectorManager;

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
        //int m[n * n], v[n]; //It is the n*n matrix(array) and the n array
        vectorManager.generatesVector(M, n * n); // It assign values to m
        vectorManager.generatesVector(V, n); // It assign values to v
    }

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD); // Se hace un broadcast del valor de n
    MPI_Bcast(V, n, MPI_INT, 0, MPI_COMM_WORLD); // Se hace un broadcast del valor de V

    n_bar = n / numProcess;

    //for (int i = 0; i < n; i++)
    // cout << "Proceso " << myId << ", posicion " << i+1 << " con valor de: " << V[i] << endl;


    MPI_Reduce(P, P, n, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD); // Completa el vector P, sumando todos los resultados
    MPI_Reduce(&tp, &tp, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD); // Suma el total de tp, sumando todos los resultados


    if (myId == 0) {
        //vectorManager.postResults();
    }

    MPI_Finalize(); //It ends MPI
    return 0;
}