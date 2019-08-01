/*******************************************************************************************************/
/* File        : Threadpool.h
/* Author      : Febil Chacko
/* Date        : 23-May-2019
/* Project     : Enttect Technical Test
/* Description : This the source file for class Threadpool implementation.
/******************************************************************************************************/
#include "Threadpool.h"
//#include <pthread.h> // to set thread priority

/*
 Function Name: Threadpool
 Description:   Constructor. This initializes the thread pool
*/
Threadpool::Threadpool( unsigned int nThreadCount )
	       : m_nWorkersCount( nThreadCount ),
			 m_bInitialized( false )
{
	initializeThreadPool();
}

/*
 Function Name: Threadpool
 Description:   Destructor. Releases the threads
*/
Threadpool::~Threadpool()
{
	clearThreads();
}

/*
 Function Name: initializeThreadPool
 Description:   Initializes the threads
*/
void Threadpool::initializeThreadPool()
{
	if( !m_bInitialized )
	{
		// Creates the predefined number of threads
		for( unsigned int nIdx = 0; nIdx < m_nWorkersCount; nIdx++ )
		{
			m_vThWorkers.push_back(std::thread( &Threadpool::workHandler, this));
		}
		m_bInitialized = true;
	}
}

/*
 Function Name: workHandler
 Description:   Thread handler function. This will invoke the task processor function if any new task added to the queue  
*/
void Threadpool::workHandler()
{
	while( false == m_abExit )
	{
		stWorkNode workNode;
		{
			std::unique_lock<std::mutex> uLock( m_syncMutex );
			//The lock is released when the thread starts waiting for the condition and the locked again when the thread is resumed
			m_workCheckCondVar.wait( uLock, [&] { return ( hasAnyWork() || m_abExit ); });
			workNode = getWork(); // This takes the next task to be processed in the queue
		}

		if( true == m_abExit )
			break;
		if( nullptr != workNode.fWorkHandler)
		{
			/************************************************************************************************/
			/**********To set the thread priority (In the compiler I used the lib for this was not added)****/
			//sched_param sch;
			//int policy;
			//pthread_getschedparam(pthread_self(), &policy, &sch);
			//sch.sched_priority = workNode.nTaskPriority;
			//if (pthread_setschedparam(pthread_self(), SCHED_FIFO, &sch)) {
			//	std::cout << "Failed to setschedparam: " << std::strerror(errno) << '\n';
			//}
			/***********************************************************************************************/
			// Invoking the actual task processor function
			workNode.fWorkHandler(workNode.nPrimeCount, std::this_thread::get_id(), workNode.nTaskPriority );
		}
	}
}
/*
 Function Name: hasAnyWork
 Description:   Checks task availablilty in the queue
*/
bool Threadpool::hasAnyWork()
{
	return !m_workQueue.empty();
}

/*
 Function Name: getWork
 Description:   Returns the task from the queue
*/
stWorkNode Threadpool::getWork()
{
	stWorkNode node;
	if( m_workQueue.empty())
	{
		node.fWorkHandler = nullptr;
	}
	else
	{
		node = m_workQueue.top();
		m_workQueue.pop();
	}
	return node;
}

/*
 Function Name: addWork
 Description:   Add task to the queue. This function creates the worknode and pushed to the queue using addWork(stWorkNode workNode) function
*/
void Threadpool::addWork( int nPrimeCount, int nTaskPriority, std::function<void(int, std::thread::id, int)> work )
{
	stWorkNode workNode;
	workNode.nPrimeCount = nPrimeCount;
	workNode.nTaskPriority = nTaskPriority;
	workNode.fWorkHandler = work;
	addWork( workNode );
}

/*
 Function Name: addWork
 Description:   Pushing the task to the queue and notifies 1 worker thread
*/
void Threadpool::addWork(stWorkNode workNode)
{
	{
		std::lock_guard<std::mutex> lGurad( m_syncMutex );
		m_workQueue.push( workNode );
	}
	m_workCheckCondVar.notify_one();
}

/*
 Function Name: clearThreads
 Description:   Releases all the threads
*/
void Threadpool::clearThreads()
{
	m_abExit = true; // atomic varibale set to release the loop in the task processor function
	m_workCheckCondVar.notify_all(); // Notifies all the threads
	// Iterating through the thread list to release 
	for( auto& threadInstance : m_vThWorkers )
	{
		threadInstance.join();
	}
	m_vThWorkers.clear(); // Clears the thread list
}

/*
 Function Name: restart
 Description:   Restarts the thread pool
*/
void Threadpool::restart()
{
	clearThreads();
	m_bInitialized = false;
	m_abExit = false;
	initializeThreadPool();
}