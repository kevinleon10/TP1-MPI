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

using namespace std;

class VectorManager {
public:
    void generatesVector(int v[], int n);

    void postResults(int m[], int v[], int q[], int p[], int b[], int n, int numProcess, int tp, double time,
                     double processTime);

private:

    string getVector(int v[], int n, string name);

    string getMatrix(int m[], int n, string name);

    void writeResults(int m[], int v[], int q[], int p[], int b[], int n, int numProcess, int tp, double time,
                      double processTime);
};

// Method which generates a matrix
void VectorManager::generatesVector(int *v, int n) {
    int i;
    srand(time(NULL));
    for (i = 0; i < n; i++) {
        v[i] = rand() % 10;
    }
}

// Method which print a vector
string VectorManager::getVector(int v[], int n, string name) {
    string vector = "";
    vector += "\n-> VECTOR ";
    vector += name;
    vector += "\n";
    for (int i = 0; i < n; i++) {
        vector += " ";
        vector += to_string(static_cast<long long int>(v[i]));
    }
    return vector;
}

// Method which print a matrix
string VectorManager::getMatrix(int m[], int n, string name) {
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
            matrix += to_string(static_cast<long long int>(m[i + j]));
        }
    }
    return matrix;
}

// Method which generates a new File, with the respective results
void
VectorManager::writeResults(int m[], int v[], int q[], int p[], int b[], int n, int numProcess, int tp, double time,
                            double processTime) {
    string input;
    input = "RESULTS";
    ofstream out("results.txt");
    out << input;
    out << endl;
    input = getMatrix(m,n,"M");
    out << input;
    out << endl;
    input = getVector(v,n,"V");
    out << input;
    out << endl;
    input = getVector(q,n,"Q");
    out << input;
    out << endl;
    input = getVector(p,n,"P");
    out << input;
    out << endl;
    input = getMatrix(b,n,"B");
    out << input;
    out.close();
}

// Method which post the Results, in console or in a file
void
VectorManager::postResults(int m[], int v[], int q[], int p[], int b[], int n, int numProcess, int tp, double totalTime,
                           double processTime) {
    cout << "\nRESULTS\n" << endl;
    cout << "-> Value of n: " << to_string(static_cast<long long int>(n)) << endl;
    cout << "-> Total of processes: " << to_string(static_cast<long long int>(numProcess)) << endl;
    cout << "->Total of prime numbers in M (tp): " << to_string(static_cast<long long int>(tp)) << endl;
    cout << "-> Total time: " << to_string(static_cast<long long int>(totalTime)) << endl;
    cout << "-> Total time, without the times to write results: " << to_string(static_cast<long long int>(totalTime))
         << endl;
    if (n <= 100) {
        cout << getMatrix(m, n, "M") << endl;
        cout << getVector(v, n, "V") << endl;
        cout << getVector(q, n, "Q") << endl;
        cout << getVector(p, n, "P") << endl;
        cout << getMatrix(b, n, "B") << endl;
    } else {
        writeResults(m, v, q, p, b, n, numProcess, tp, totalTime, processTime);
    }

}


int main(int argc, char **argv) {
    int numProcess, n, myId;
    double startTime, endTime;
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
        int m[n * n], v[n]; //It is the n*n matrix(array) and the n array
        vectorManager.generatesVector(m, n * n); // It assign values to m
        vectorManager.generatesVector(v, n); // It assign values to v
        vectorManager.postResults(m, v, v, v, m, n, 1, 2, 3, 4);


    } else {

    }

    MPI_Finalize(); //It ends MPI
    return 0;
}