#pragma once

#include "thread_worker.h"

#include <omp.h>

using namespace std;

void runWithOpenmp() {
	// количество генераций больших "простых" чисел
	int n = 1'000;

	// количество потоков
	omp_set_num_threads(8);

	clock_t start, end;
	int numThread;
	start = clock();
	#pragma omp parallel private (numThread)
	{
		numThread = omp_get_thread_num();
		mt19937 generator((unsigned long long) getRand() >> numThread);
		// #pragma omp for schedule (static)
		#pragma omp for schedule (dynamic, 10)
		for (int i = 0; i < n; i++) {
			findPrimeNumber(numThread, generator);
			// cout << "Thread " << numThread << " complete!" << endl;
		}
	}
	end = clock();

	double executeTime = ((double)end - start) / ((double)CLOCKS_PER_SEC);
	cout << "Executing time: " << executeTime << " second(s) for n = " << n << endl;
}
