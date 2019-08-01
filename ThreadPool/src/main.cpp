/*******************************************************************************************************/
/* File        : main.cpp
/* Author      : Febil Chacko
/* Date        : 23-May-2019
/* Project     : Enttect Technical Test
/* Description : This file contains the 'main' function. Program execution begins and ends there.
/******************************************************************************************************/
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include "Threadpool.h"

std::ofstream threadOutputfile; // Output file
std::mutex	m_syncFunctMutex;   // Output thread syncronization object

/*
 Function Name: printPrimeFactors
 Description  : Function to print all prime factors of a given number 
*/
void printPrimeFactors( int nVal, 	std::thread::id nThreadId, int nPriority )
{
	std::vector<int> vPrimeNumbers;
	int nIpNo = nVal;

	// Print the number of 2s that divide n  
	while( nVal % 2 == 0 )
	{
		vPrimeNumbers.push_back(2);
		nVal /= 2;
	}

	// n must be odd at this point. So we can skip  
	// one element (Note i = i +2)  
	for (int nIdx = 3; nIdx <= sqrt(nVal); nIdx = nIdx + 2)
	{
		// While i divides n, print i and divide n  
		while (nVal % nIdx == 0)
		{
			vPrimeNumbers.push_back( nIdx );
			nVal /= nIdx;
		}
	}

	// This condition is to handle the case when n  
	// is a prime number greater than 2  
	if (nVal > 2)
		vPrimeNumbers.push_back(nVal);

	std::ostringstream stringStream;
	stringStream << "\nTask priority : " << nPriority << ",\t Thread ID " << nThreadId << "\t : Prime number factors for "<< nIpNo<<" are ";
	
	// Display the list of numbers neing added to the queue by the thread function
	for( unsigned nIdx = 0; nIdx < vPrimeNumbers.size(); nIdx++)
	{
		stringStream << ' ' << vPrimeNumbers[nIdx];
	}
	stringStream << "\n";
	if( threadOutputfile.is_open())
	{
		std::lock_guard<std::mutex> lGurad(m_syncFunctMutex);
		threadOutputfile << stringStream.str();
		std::cout<< stringStream.str();
	}
}

/*
 Function Name: printCommandOptions
 Description  : Print the user options
*/
void printCommandOptions()
{
	std::lock_guard<std::mutex> lGurad(m_syncFunctMutex);
	std::cout << "\ncommand line options : \n";
	std::cout << " \t0. for Exit\n";
	std::cout << " \t1. for Restart \n";
	std::cout << " \t2. for Add Task(Enter two numbers : \n";
	std::cout << " \t   First number N ( to get the prime numbers upto N ), Second Number P for task priority\n";
	std::cout << " \t	(Lowest number has highest priority). \n";
	std::cout << " \t   Input format for Add task : 2,200,3\n  Please enter the commands : \n";
}

/*
 Function Name: main
 Description  : program entry point
*/
int main()
{
	std::string command;
	time_t tCurrTime = time(0);   // get time now
	struct tm  tmNow;
	localtime_s( &tmNow, &tCurrTime );

	char buffer[80];
	strftime( buffer, 80, "Threadpool_out_%Y-%m-%d_%H-%M-%S.txt", &tmNow );


	threadOutputfile.open( buffer, std::ofstream::out | std::ofstream::app );

	std::cout << "\n";
	std::cout << "|----------------------------------Threadpool solution[ command line ]-------------------------------|\n";
	std::cout << "|----------------This is a program which prints the prime factors for the given number---------------|\n";
	std::cout << "|----------------prime factors for a number is the factors of it which are prime numbers-------------|\n";
	std::cout << "|----------------and multiplying all those together will give the original number--------------------| \n";
	std::cout << "|----------------for example prime factors for the number 20 is 2, 2, 5 (20 = 2 * 2 * 5)-------------| \n";
	bool bLoop = true;
	// print the command line options for the user
	printCommandOptions();
	while (true == bLoop)
	{

		std::getline(std::cin, command);
		if (command == "0")
		{
			std::lock_guard<std::mutex> lGurad(m_syncFunctMutex); // to prevent simultaneous writing to the console by more than 1 thread 
			bLoop = false;
			std::cout << "\nExiting ..";
			continue;
		}
		else if (command == "1")
		{
			Threadpool::getInstance().restart();// Reinitiates the thread
			std::lock_guard<std::mutex> lGurad(m_syncFunctMutex); // to prevent simultaneous writing to the console by more than 1 thread  
			std::cout << "\nThreadpool.restarted.\n";
		}
		else
		{
			// Parse the command to separte the tokens (option, number and priority)
			std::stringstream sAddTaskCmd(command);
			int nVal;
			std::vector<int> vect;

			while (sAddTaskCmd >> nVal)
			{
				vect.push_back(nVal);

				if (sAddTaskCmd.peek() == ',')
					sAddTaskCmd.ignore();
			}
			int nTokens = vect.size();
			// Token validation. maximum expected tokens are expected delimted by comma (option,number,priority)
			if (3 != nTokens)
			{
				std::lock_guard<std::mutex> lGurad(m_syncFunctMutex); // to 
				std::cout << "\nInvalid input format.";
				continue;
			}
			int nInputNum = vect.at(1); // The input number
			int nPriority = vect.at(2); // priority for task
			
			// Add Task to the task queue
			Threadpool::getInstance().addWork(nInputNum, nPriority, printPrimeFactors);
			
			// For testing purpose. Enable this to see the actual parallel execution of threads 

			/*Threadpool::getInstance().addWork(50, 5, printPrimeFactors);
			Threadpool::getInstance().addWork(60, 6, printPrimeFactors);
			Threadpool::getInstance().addWork(70, 7, printPrimeFactors);
			Threadpool::getInstance().addWork(80, 8, printPrimeFactors);
			Threadpool::getInstance().addWork(90, 9, printPrimeFactors);
			Threadpool::getInstance().addWork(100, 10, printPrimeFactors);
			Threadpool::getInstance().addWork(99, 9, printPrimeFactors);
			Threadpool::getInstance().addWork(88, 8, printPrimeFactors);
			Threadpool::getInstance().addWork(77, 7, printPrimeFactors);
			Threadpool::getInstance().addWork(1984, 1001, printPrimeFactors);
			Threadpool::getInstance().addWork(5, 100, printPrimeFactors);
			Threadpool::getInstance().addWork(8, 200, printPrimeFactors);
			Threadpool::getInstance().addWork(7, 50, printPrimeFactors);*/
		}
	}
	threadOutputfile << "\n End of execution.";
	threadOutputfile.close();
}