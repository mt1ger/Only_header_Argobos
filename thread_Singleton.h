#ifndef __SINGLETON_H
#define __SINGLETON_H

#include<cstdlib>
#include<iostream>
#include<abt.h>

using namespace std;

class thread_Singleton 
{

	int num_xstreams = 0;	
	inline static thread_Singleton * tsingleton_ptr = nullptr;
	// static thread_Singleton * tsingleton_ptr;
	ABT_xstream * xstreams;
	ABT_sched* scheds; 

	thread_Singleton()
	{
		Gtid = 0;
		num_xstreams = atoi(getenv("NUM_ES"));
		ABT_init(0, nullptr);
		mem_allocation ();
		pools_scheds_creation ();
		main_xstream ();
		secondary_xstreams ();
	}

	~thread_Singleton()
	{
		join_free_xstream();
		finalize();
	}

	void mem_allocation ()  
	{
		xstreams = (ABT_xstream *)malloc(sizeof(ABT_xstream) * num_xstreams);
		pools = (ABT_pool *)malloc(sizeof(ABT_pool) * num_xstreams);
		scheds = (ABT_sched *)malloc(sizeof(ABT_sched) * num_xstreams);
	}
	void pools_scheds_creation ()
	{
		/* Create pools. */
		for (int i = 0; i < num_xstreams; i++) {
			ABT_pool_create_basic(ABT_POOL_FIFO, ABT_POOL_ACCESS_MPMC, ABT_TRUE,
					&pools[i]);
		}

		/* Create schedulers */
		bool flag = 1;
		for (int i = 0; i < num_xstreams; i++) {
			ABT_pool *tmp = (ABT_pool *)malloc(sizeof(ABT_pool) * num_xstreams);
			for (int j = 0; j < num_xstreams; j++) {
				tmp[j] = pools[(i + j) % num_xstreams];
			}
			flag = ABT_sched_create_basic(ABT_SCHED_DEFAULT, num_xstreams, tmp,
					ABT_SCHED_CONFIG_NULL, &scheds[i]);
			free(tmp);
		}

	}
	void main_xstream () 
	{
		for (int i = 1; i < num_xstreams; i++)
		{
			ABT_xstream_create(scheds[i], &xstreams[i]);
		}

	}
	void secondary_xstreams ()
	{
		for (int i = 1; i < num_xstreams; i++)
		{
			ABT_xstream_create(scheds[i], &xstreams[i]);
		}

	}
	void join_free_xstream () 
	{
		for (int i = 0; i < num_xstreams; i++)
		{
			ABT_xstream_join (xstreams[i]);
			ABT_xstream_free (&xstreams[i]);
		}

	}
	void finalize ()
	{
		/* Finalize Argobots. */
		ABT_finalize();

		/* Free allocated memory. */
		free(xstreams);
		free(pools);
		free(scheds);

	}


	public:
	static thread_Singleton* instance ()
	{
		if (tsingleton_ptr == nullptr)
		{
			tsingleton_ptr = new thread_Singleton();
		}
		return tsingleton_ptr;
	}
	ABT_pool* pools; 
	ABT_thread_id Gtid;
};
#endif
