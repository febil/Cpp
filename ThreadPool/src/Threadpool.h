/*******************************************************************************************************/
/* File        : Threadpool.h
/* Author      : Febil Chacko
/* Date        : 23-May-2019
/* Project     : Enttect Technical Test
/* Description : This the header file for class Threadpool declaration.
/******************************************************************************************************/
#pragma once

#ifndef _TREADPOOL_H // Header Gurd
#define _TREADPOOL_H

#define THREAD_COUNT 4 // The number of precreated threads are four (Because the PC used for development is 4 core)

#include <thread>
#include <vector> 
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <queue> 

/*This is a worker node specific to a task*/
struct stWorkNode
{
	unsigned int nPrimeCount; // the number for which the prime factors to be calculated
	unsigned int nTaskPriority; // priority for this task (lower number has higher priority)
	std::function<void(int, std::thread::id, int nPriority)> fWorkHandler;  // callable target, parameter specified function 
																			// which processes the number
};

// this is an structure which implements the operator overlading to compare the priority queue for sorting
struct ComparePriority{
	bool operator()(stWorkNode const& work_1, stWorkNode const& work_2)
	{
		return work_1.nTaskPriority > work_2.nTaskPriority; // expecting low number for higher priority
	}
};

/*
	Class Name : ThreadPool
	Description: Allows the execution of any task in a precreated thread. This is singleton class.
				 The number of precreated threads are four (Because the PC used for development is 4 core). 
				 This class has 2 main functionalities. 1) Add Task, 2) Reinitiate thread pool, 
*/
class Threadpool
{
public:

	/*
	 Function Name: getInstance
	 Description:   This is a static function which retruns the class instance
	*/
	static Threadpool& getInstance() 
	{
		static Threadpool sinInstance;
		return sinInstance;
	}
	void addWork( int nPrimeCount, int nTaskPriority, std::function<void(int,std::thread::id,int)> work );
	void addWork( stWorkNode workNode );
	void restart();
	void setThreadCount(int nCount) { m_nWorkersCount = nCount;  } // Inline function to set the thread count. 
																   // This will be reflected only after restart opertion 

private:

	Threadpool(unsigned int nThreadCount = THREAD_COUNT); // constructor with default parameter
	~Threadpool();										  // destructor
	Threadpool(const Threadpool&) = delete;				  //explicit delete function
	Threadpool& operator=(const Threadpool&) = delete;    // explicit delete function

	void initializeThreadPool();
	void workHandler();
	bool hasAnyWork();
	stWorkNode getWork();
	void clearThreads();

private:

	unsigned int				m_nWorkersCount;	// Number of active threads in thread pool 
	bool						m_bInitialized;		// Thread pool initialization flag to control initializeThreadPool() function

	std::vector<std::thread>	m_vThWorkers;		// Thread list
	std::atomic<bool>			m_abExit{ false };  // Interthread synch flag
	std::mutex					m_syncMutex;		// Synchronization object for Task Queue
	std::condition_variable		m_workCheckCondVar; // Task update notification variable 
	std::priority_queue<stWorkNode, std::vector<stWorkNode>, ComparePriority> m_workQueue; // Priority task queue
};

#endif // _TREADPOOL_H