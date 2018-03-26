/*
// Created by Kevin Le�n & Renato Mainieri on 25/03/2018.
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
    void generatesVector(int V[], int n, bool rank);

    void postResults(int M[], int V[], int Q[], int P[], int B[], int n, int numProcesses, int tp, double time,
                     double processTime);

    int productPoint(int row[], int column[], int n_bar);

    string getMatrix(int M[], int n, string name);

    string getVector(int V[], int n, string name);

    void generatesSendCounts(int sendCounts[], int n_bar, int n);

    void generatesSendDespl(int despl[], int n_bar, int n);

private:


    void writeResults(int M[], int V[], int Q[], int P[], int B[], int n, int numProcesses, int tp, double time,
                      double processTime);
};

// Method which generates a vector
void VectorManager::generatesVector(int V[], int n, bool rank) {
    int i;
    srand(time(NULL));
    for (i = 0; i < n; i++) {
        if (rank) {
            V[i] = rand() % 10;
        } else {
            V[i] = rand() % 6;
        }
    }
}

// Method which post the Results, in console or in a file
void
VectorManager::postResults(int M[], int V[], int Q[], int P[], int B[], int n, int numProcesses, int tp,
                           double totalTime, double processTime) {
    cout << "\nRESULTS\n" << endl;
    cout << "-> Value of n: " << n << endl;
    cout << "-> Total of processes: " << numProcesses << endl;
    cout << "-> Total of prime numbers in M (tp): " << tp << endl;
    cout << "-> Total time: " << totalTime << endl;
    cout << "-> Total time, without the times to write results: " << processTime << endl;
    if (n <= 100) {
        cout << getMatrix(M, n, "M") << endl;
        cout << getVector(V, n, "V") << endl;
        cout << getVector(Q, n, "Q") << endl;
        cout << getVector(P, n, "P") << endl;
        cout << getMatrix(B, n, "B") << endl;
        cout << endl;
    } else {
        writeResults(M, V, Q, P, B, n, numProcesses, tp, totalTime, processTime);
    }

}

// Method which returns a product point of two vectors
int VectorManager::productPoint(int row[], int column[], int n_bar) {
    int result = 0;
    for (int i = 0; i < n_bar; i++) {
        result += row[i] * column[i];
    }
    return result;
}

// Method which prints a vector
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

// Method which prints a matrix
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
            matrix += to_string(static_cast<long long int>(M[(i*n) + j]));
        }
    }
    return matrix;
}

// Method which generates a new File, with the respective results
void
VectorManager::writeResults(int M[], int V[], int Q[], int P[], int B[], int n, int numProcesses, int tp, double time,
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

void VectorManager::generatesSendCounts(int sendCounts[], int n, int numProcesses) {
    int n_bar = n / numProcesses;
    for (int i = 0; i < numProcesses; ++i) {
        if (i == 0 || i == numProcesses - 1) {
            sendCounts[i] = (n_bar + 1) * n;
        } else {
            sendCounts[i] = (n_bar + 2) * n;
        }
    }
}

void VectorManager::generatesSendDespl(int sendDespl[], int n, int numProcesses) {
    int n_bar = n / numProcesses;
    for (int i = 0; i < numProcesses; ++i) {
        if (i == 0) {
            sendDespl[i] = 0;
        } else if (i == 1) {
            sendDespl[i] = (n_bar - 1) * n;
        } else if (i == numProcesses - 1) {
            sendDespl[i] = (n - (n_bar + 1)) * n;
        } else {
            sendDespl[i] = (i * n_bar - 1) * n;
        }
    }
}

int main(int argc, char **argv) {
    int numProcesses, n, myId, tp, localTp;
    int n_bar;        /*  Se calculara como  n/p, es decir es el numero de
                           elementos que le corresponde a cada proceso de cada vector */
    double startTime, endTotalTime, endProcessTime; // MPI_Wtime()
    int M[MAX * MAX];
    int localM[MAX * MAX]; //  Aca recibe cada proceso la parte de M que le corresponde
    int V[MAX], Q[MAX], P[MAX];
    int localQ[MAX];
    int localV[MAX];
    int despl[MAX];
    int sendCounts[MAX];

    VectorManager vectorManager;

    MPI_Init(&argc, &argv); //It starts MPI

    MPI_Comm_size(MPI_COMM_WORLD, &numProcesses); //MPI stores in numProcesses the quantity of processes

    MPI_Comm_rank(MPI_COMM_WORLD, &myId); //MPI stores in myId the id of the current process

    if (myId == 0) {
        //It controls the user cannot enter wrong data
        cout << "\nType n, which is the dimension of the matrix" << endl;
        cin >> n; //It is the length of each row and the number of rows
        while (n < numProcesses || n % numProcesses != 0) {
            cout << "\nThe n must be multiple of the number of processes!!" << endl;
            cout << "Type n, which is the dimension of the matrix." << endl;
            cin >> n; //It is the length of each row and the number of rows
        }
        //int m[n * n], v[n]; //It is the n*n matrix(array) and the n array
        vectorManager.generatesVector(M, n * n, true); // It assign values to m
        vectorManager.generatesVector(V, n, false); // It assign values to v

        vectorManager.generatesSendCounts(sendCounts, n, numProcesses);
        vectorManager.generatesSendDespl(despl, n, numProcesses);


        cout << vectorManager.getMatrix(M, n, "M") << endl;
        cout << vectorManager.getVector(V, n, "V") << endl;
    }

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD); // It does a broadcast of n value
    MPI_Bcast(V, n, MPI_INT, 0, MPI_COMM_WORLD); // It does a broadcast of v value

    n_bar = n / numProcesses;

    MPI_Scatter(Q, n_bar, MPI_INT, localQ, n_bar, MPI_INT, 0,
                MPI_COMM_WORLD); // It sends to each process a part of Q vector for the multiplication


    //MPI_ScatterV(M, );

    /*for(int i=0; i<n_bar; ++i){
        vectorManager.productPoint()
    }*/

    //for (int i = 0; i < n; i++)
    // cout << "Proceso " << myId << ", posicion " << i+1 << " con valor de: " << V[i] << endl;

    MPI_Gather(localQ, n_bar, MPI_INT, Q, n_bar, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Reduce(P, P, n, MPI_INT, MPI_SUM, 0,
               MPI_COMM_WORLD); // It completes the P vector, adding to each column the quantity of prime numbers per column
    MPI_Reduce(&tp, &tp, 1, MPI_INT, MPI_SUM, 0,
               MPI_COMM_WORLD); // It completes tp, adding the quantity of primes numbers.

    if (myId == 0) {
        //vectorManager.postResults();
    }

    MPI_Finalize(); //It ends MPI
    return 0;
}