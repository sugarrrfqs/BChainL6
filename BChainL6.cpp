#include <iostream>
#include "tbb/tbb.h"
#include <time.h>
#include <chrono>

const static int rowCount = 6000;
const static int rowLength = rowCount + 1;
const static int maxNumbers = 100;
const static int minNumbers = -50;
static int activeProcs = 0;

static double** makeSLAU()
{
	double** m = new double* [rowCount];
	for (int i = 0; i < rowCount; i++)
	{
		m[i] = new double[rowLength];
	}
	for (int i = 0; i < rowCount; i++)
	{
		for (int j = 0; j < rowLength - 1; j++) m[i][j] = rand() % maxNumbers + minNumbers;
	}

	double* x = new double[rowCount];
	for (int i = 0; i < rowCount; i++)
	{
		x[i] = rand() % maxNumbers + minNumbers;
	}
	for (int i = 0; i < rowCount; i++)
	{
		std::cout << "PRAVILNIY X" << i + 1 << " = " << x[i] << "\n";
	}

	for (int i = 0; i < rowCount; i++)
	{
		m[i][rowLength - 1] = 0;
	}

	for (int i = 0; i < rowCount; i++)
	{
		for (int j = 0; j < rowLength - 1; j++) m[i][rowLength - 1] += m[i][j] * x[j];
	}
	//cout << "STARTOVAYA MATRICA\n";
	//for (int i = 0; i < rowCount; i++)
	//{
	//	for (int j = 0; j < rowLength; j++) printf("%10.0f ", m[i][j]);
	//	cout << "\n";
	//}

	return m;
}

int main()
{
	/*
		2x - 2y +  z = -3
		 x + 3y - 2z =  1
		3x -  y -  z =  2
	*/

	setlocale(LC_ALL, "RU");
	double* x = new double[rowCount + 1];
	double sum = 0;
	double** m = makeSLAU();
	//double m[3][4] = { {2, -2, 1, -3}, {1, 3, -2, 1}, {3, -1, -1, 2} };

	for (int i = 0; i < rowCount; i++) x[i] = 0;
	x[rowCount] = -1;


	//unsigned int start_time = clock();
	auto start_time = std::chrono::steady_clock::now();


	for (int i = 0; i < rowCount - 1; i++)
	{
		tbb::parallel_for(tbb::blocked_range<size_t>(i + 1, rowCount), 
			[&m, &i](const tbb::blocked_range<size_t>& r)
			{
				for (size_t j = r.begin(); j != r.end(); j++)
				{
					double k = m[j][i] / m[i][i];
					for (int h = i; h < rowLength; h++)
					{
						m[j][h] -= m[i][h] * k;
					}
				}
			});
		//for (int j = i + 1; j < rowCount; j++)
		//{
		//	double k = m[j][i] / m[i][i];
		//	for (int h = i; h < rowLength; h++)
		//	{
		//		m[j][h] -= m[i][h] * k;
		//	}
		//}
	}
	//for (int i = 0; i < rowCount; i++)
	//{
	//	for (int j = 0; j < rowLength; j++) cout << m[i][j] << " ";
	//	cout << "\n";
	//}

	for (int i = rowCount - 1; i >= 0; i--)
	{
		sum = 0;
		for (int j = 0; j < rowLength; j++)
		{
			if (j != i)  sum += m[i][j] * x[j];
		}

		x[i] = -(sum / m[i][i]);
	}

	//unsigned int end_time = clock();
	//unsigned int time = end_time - start_time;

	auto end_time = std::chrono::steady_clock::now();
	auto time = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);

	for (int i = 0; i < rowCount; i++)
	{
		std::cout << "X" << i + 1 << " = " << x[i] << "\n";
	}
	//std::cout << "Время - " << time << " ms \n";
	std::cout << "Время - " << time.count() << " ms \n";
}

