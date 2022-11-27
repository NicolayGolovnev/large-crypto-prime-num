#pragma once

#include "mr.h"

#include <iostream>
#include <algorithm>
#include <vector>
#include <random>
#include <ctime>
#include <Windows.h>

#define MAX_NUMBER_OF_THREADS 32

using namespace std;

/*
 * Метод получения "псевдо-случайного" 128-битного числа
 */
unsigned long long getRand() {
	return ((((((((uint64_t)rand() << 15) + rand()) << 15) + rand()) << 15) + rand()) << 15) + rand();
}


/*
 * Поиск большого простого числа с помощью теста Миллера-Рабина
 */
void findPrimeNumber(int thread, mt19937& generator) {
	uniform_int_distribution<unsigned long long> distribution(0, 0xFFFFFFFFFFFFFFFF);
	unsigned long long randomNumber, count = 0;
	do {
		randomNumber = distribution(generator);
		count++;
	} while (!miller_rabin(randomNumber, 2));

	cout << "Thread #" << thread << ": Try " << count << ": succeed!" << endl;
	cout << "Prime number is " << randomNumber << endl;
}


/*
 * Работа программы в однопоточном режиме
 * int n - количество запусков метода поиска большого простого числа
 *		По умолчанию равно 100 запускам
 */
void workWithoutThreads(int n = 100) {
	clock_t start, end;
	mt19937 generator(getRand());

	start = clock();
	for (int i = 0; i < n; i++) {
		findPrimeNumber(0, generator);
	}
	end = clock();

	double executeTime = ((double)end - start) / ((double)CLOCKS_PER_SEC);
	cout << "Executing time: " << executeTime << " second(s) for n = " << n << endl;
}


/*
 * Структура параметров, передаваемых в асинхронную функцию
 */
struct Param {
	int numberOfThread;		// номер потока
	int n;					// количество итераций нахождения простого числа на поток
};


/*
 * Асинхронная функция поиска большого простого числа
 */
DWORD WINAPI asyncFindPrime(LPVOID lpParam) {
	Param* param = (Param*)lpParam;
	mt19937 generator((unsigned long long) getRand() >> param->numberOfThread);

	for (int i = 0; i < param->n; i++)
		findPrimeNumber(param->numberOfThread, generator);

	cout << "Thread " << param->numberOfThread << " complete!" << endl;
	return 0;
}


/*
 * Работа программы в многопоточном режиме
 * int threads - количество потоков, которые будут выделены для работы
 *		По умолчанию создается 1 поток
 * int n - количество запусков метода поиска большого простого числа
 * 		По умолчанию равно 100 запускам
 */
void workWithThreads(int threads = 1, int n = 100) {
	clock_t start, end;

	int nForThread = (int)(n / threads + 1);
	HANDLE threadProducer[MAX_NUMBER_OF_THREADS];
	DWORD dwNetThreadId[MAX_NUMBER_OF_THREADS];
	Param paramForThreads[MAX_NUMBER_OF_THREADS];

	for (int i = 0; i < threads; i++)
		paramForThreads[i] = { i + 1, nForThread };

	start = clock();
	// создаем поток с параметрами paramForThreads[i], выполняющую функцию asyncFindPrime
	for (int i = 0; i < threads; i++)
		threadProducer[i] = CreateThread(NULL, 0, asyncFindPrime, &paramForThreads[i], 0, &dwNetThreadId[i]);

	// ожидаем завершение всех потоков
	WaitForMultipleObjects(threads, threadProducer, true, INFINITE);
	end = clock();

	double executeTime = ((double)end - start) / ((double)CLOCKS_PER_SEC);
	cout << "Executing time: " << executeTime << " second(s) for n = " << n << endl;
}


/*
 * Пример работы поиска простых сгенерированных чисел в многопоточном режиме
 */
void execWithThreads() {
	// количество генераций больших "простых" чисел
	int n = 100'000;

	// Пример с потоками
	workWithThreads(8, n);
}


/*
 * Пример работы поиска простых сгенерированных чисел в однопоточном режиме
 */
void exampleWithoutThreads() {
	// количество генераций больших "простых" чисел
	int n = 100'000;

	// Пример без работы с потоками
	workWithoutThreads(n);
}
