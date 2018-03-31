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

    string getMatrix(int M[], int n, string name);

    string getVector(int V[], int n, string name);

    void generatesSendCounts(int sendCounts[], int n_bar, int n);

    void generatesSendDisplacements(int displs[], int n_bar, int n);
	
	void generateResults(int localM[], int V[], int localQ[], int P[], int tp, int n, int n_bar, bool firstProcess);

private:

	bool isPrime(int value);

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
            matrix += to_string(static_cast<long long int>(M[(i * n) + j]));
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

void VectorManager::generatesSendDisplacements(int sendDisplacements[], int n, int numProcesses) {
    int n_bar = n / numProcesses;
    for (int i = 0; i < numProcesses; ++i) {
        if (i == 0) {
            sendDisplacements[i] = 0;
        } else if (i == numProcesses - 1) {
            sendDisplacements[i] = (n - (n_bar + 1)) * n;
        } else {
            sendDisplacements[i] = (i * n_bar - 1) * n;
        }
    }
}

void VectorManager::generateResults(int localM[], int V[], int localQ[], int P[], int tp, int n, int n_bar, bool firstProcess) {
	int i = 0;
	int counter = 0;
	int tempNBar = 0;
	tempNBar += n_bar;
	if(!firstProcess){
		++i;
		tempNBar += n_bar + 1;
	}
	for(i; i<tempNBar; ++i){
		int column = 0;
		for(int j=0; j<n; ++j){
			if(isPrime(localM[i*n+j])){
				++P[j];
				++tp;
			}
			column += localM[i*n+j] * V[j];
		}
		localQ[counter] = column;
		++counter;
	}
	cout << getVector(P, n, "P") << endl;
	cout << tp << endl;
}

bool VectorManager::isPrime(int value){
	bool isPrime = false;
	if(value==2 || value==3 || value==5 || value==7){
		isPrime = true;
	}
	return isPrime;
}


int main(int argc, char **argv) {
    int numProcesses, n, myId, tp;
    int n_bar;        /*  Se calculara como  n/p, es decir es el numero de
                           elementos que le corresponde a cada proceso de cada vector */
    double startTime, endTotalTime, endProcessTime; // MPI_Wtime()
    int M[MAX], B[MAX];
    int localM[MAX], localB[MAX]; //  Aca recibe cada proceso la parte de M que le corresponde
	int rowsInM;
    int V[MAX], Q[MAX], P[MAX];
    int localQ[MAX]; // It stores the product point in each process
    int displacements[MAX];
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
        vectorManager.generatesSendDisplacements(displacements, n, numProcesses);
		
    }

	tp = 0;
	
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD); // It does a broadcast of n value
    MPI_Bcast(V, n, MPI_INT, 0, MPI_COMM_WORLD); // It does a broadcast of v value

	for(int reni=0; reni<n; ++reni)
		P[reni] = 0;
	
    n_bar = n / numProcesses;

    MPI_Scatter(Q, n_bar, MPI_INT, localQ, n_bar, MPI_INT, 0,
                MPI_COMM_WORLD); // It sends to each process a part of Q vector for the multiplication

    MPI_Scatterv(M, sendCounts, displacements, MPI_INT, localM, (n_bar+2)*n, MPI_INT, 0, MPI_COMM_WORLD);
   
    // Process of getting Q
	bool firstProcess = false;
	if(myId==0 || myId==numProcesses-1){
		if(myId==0) firstProcess = true;
		rowsInM = n_bar+1;
	} else {
		rowsInM = n_bar+2;
	}
	cout << "soy el puto " << myId;
	vectorManager.generateResults(localM, V, localQ, P, tp, n, n_bar, firstProcess);
	cout << endl;
	
    MPI_Gather(localQ, n_bar, MPI_INT, Q, n_bar, MPI_INT, 0, MPI_COMM_WORLD);
	
	//MPI_Gather(localB, n_bar*n, MPI_INT, B, n_bar*n, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Reduce(P, P, n, MPI_INT, MPI_SUM, 0,
               MPI_COMM_WORLD); // It completes the P vector, adding to each column the quantity of prime numbers per column
    MPI_Reduce(&tp, &tp, 1, MPI_INT, MPI_SUM, 0,
               MPI_COMM_WORLD); // It completes tp, adding the quantity of primes numbers.
	
    if (myId == 0) {
		cout << vectorManager.getVector(sendCounts, numProcesses, "S") << endl;
        cout << vectorManager.getVector(displacements, numProcesses, "D") << endl;
		cout << vectorManager.getMatrix(M, n, "M") << endl;
		cout << vectorManager.getVector(V, n, "V") << endl;
		cout << vectorManager.getVector(Q, n, "Q") << endl;
		cout << vectorManager.getVector(P, n, "P") << endl;
		cout << "tp: " << tp << endl;
    }

    MPI_Finalize(); //It ends MPI
    return 0;
}