#pragma once

#include <mpi.h>

#include "thread_worker.h"

void runWithMpi() {
    // количество генераций больших "простых" чисел
    int n = 1'000;

    // Initialize the MPI environment
    int ierr = MPI_Init(nullptr, nullptr);
    if (ierr != MPI_SUCCESS) {
        char *mpiError;
        MPI_Error_string(ierr, mpiError, nullptr);
        string mpiErrorString(mpiError);
        throw invalid_argument("MPI ERROR: " + mpiErrorString);
    }

    int numThread;
    MPI_Comm_size(MPI_COMM_WORLD, &numThread);

    int currentThread;
    MPI_Comm_rank(MPI_COMM_WORLD, &currentThread);

    // Главный поток - выводим число всех потоков только 1 раз
    if (currentThread == 0) cout << "Count of threads: " << numThread << endl;
    clock_t start, end;

    start = clock();
    mt19937 generator((unsigned long long) getRand() >> currentThread);
    for (int j = 0; j < n / numThread + 1; j++) {
        findPrimeNumber(currentThread, generator);
//        cout << "Thread " << currentThread << " complete!" << endl;
    }

    end = clock();

    double executeTime = ((double)end - start) / ((double)CLOCKS_PER_SEC);
    cout << "Executing time: " << executeTime << " second(s) for n = " << n / numThread + 1
         << " (thread "<< currentThread << ")" << endl;

    // Finalize the MPI environment
    MPI_Finalize();
}
