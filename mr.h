#pragma once

// Алгоритм был взят с сайта e-maxx
// https://e-maxx.ru/algo/bpsw

//! Модуль 64-битного числа
unsigned long long abs(unsigned long long n)
{
	return n;
}

//! Возвращает true, если n четное
template <class T>
bool even(const T& n)
{
	// return n % 2 == 0;
	return (n & 1) == 0;
}

//! Делит число на 2
template <class T>
void bisect(T& n)
{
	// n /= 2;
	n >>= 1;
}

//! Умножает число на 2
template <class T>
void redouble(T& n)
{
	// n *= 2;
	n <<= 1;
}

//! Умножает a *= b (mod n)
template <class T>
void mulmod(T& a, T b, const T& n)
{
	// наивная версия, годится только для длинной арифметики
	a *= b;
	a %= n;
}

template <>
void mulmod(int& a, int b, const int& n)
{
	a = int((((long long)a) * b) % n);
}

template <>
void mulmod(unsigned& a, unsigned b, const unsigned& n)
{
	a = unsigned((((unsigned long long)a) * b) % n);
}

template <>
void mulmod(unsigned long long& a, unsigned long long b, const unsigned long long& n)
{
	// сложная версия, основанная на бинарном разложении произведения в сумму
	if (a >= n)
		a %= n;
	if (b >= n)
		b %= n;
	unsigned long long res = 0;
	while (b)
		if (!even(b))
		{
			res += a;
			while (res >= n)
				res -= n;
			--b;
		}
		else
		{
			redouble(a);
			while (a >= n)
				a -= n;
			bisect(b);
		}
	a = res;
}

template <>
void mulmod(long long& a, long long b, const long long& n)
{
	bool neg = false;
	if (a < 0)
	{
		neg = !neg;
		a = -a;
	}
	if (b < 0)
	{
		neg = !neg;
		b = -b;
	}
	unsigned long long aa = a;
	mulmod<unsigned long long>(aa, (unsigned long long)b, (unsigned long long)n);
	a = (long long)aa * (neg ? -1 : 1);
}

//! Вычисляет a^k (mod n). Использует бинарное возведение в степень
template <class T, class T2>
T powmod(T a, T2 k, const T& n)
{
	T res = 1;
	while (k)
		if (!even(k))
		{
			mulmod(res, a, n);
			--k;
		}
		else
		{
			mulmod(a, a, n);
			bisect(k);
		}
	return res;
}

//! Переводит число n в форму q*2^p
template <class T>
void transform_num(T n, T& p, T& q)
{
	T p_res = 0;
	while (even(n))
	{
		++p_res;
		bisect(n);
	}
	p = p_res;
	q = n;
}

//! Алгоритм Евклида
template <class T, class T2>
T gcd(const T& a, const T2& b)
{
	return (a == 0) ? b : gcd(b % a, a);
}

//! Усиленный алгоритм Миллера-Рабина проверки n на простоту по основанию b
template <class T, class T2>
bool miller_rabin(T n, T2 b)
{
	// сначала проверяем тривиальные случаи
	if (n == 2)
		return true;
	if (n < 2 || even(n))
		return false;

	// проверяем, что n и b взаимно просты (иначе это приведет к ошибке)
	// если они не взаимно просты, то либо n не просто, либо нужно увеличить b
	if (b < 2)
		b = 2;
	for (T g; (g = gcd(n, b)) != 1; ++b)
		if (n > g)
			return false;

	// разлагаем n-1 = q*2^p
	T n_1 = n;
	--n_1;
	T p, q;
	transform_num(n_1, p, q);

	// вычисляем b^q mod n, если оно равно 1 или n-1, то n, вероятно, простое
	T rem = powmod(T(b), q, n);
	if (rem == 1 || rem == n_1)
		return true;

	// теперь вычисляем b^2q, b^4q, ... , b^((n-1)/2)
	// если какое-либо из них равно n-1, то n, вероятно, простое
	for (T i = 1; i < p; i++)
	{
		mulmod(rem, rem, n);
		if (rem == n_1)
			return true;
	}

	return false;
}