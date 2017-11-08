#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string> 
#include <sys/mman.h>
#include <sys/types.h>
#include <err.h>
using namespace std;

struct coreNumAndAddr{
	int* addr;
	int** addrOfAddr;	
};

void* DoWork(void* args){
	
	return 0;
}


void* assignData(void* args){
	struct coreNumAndAddr* arg = (struct coreNumAndAddr*) args;
	int** addrAddr = arg->addrOfAddr;
	int* addr = arg->addr;
	cout<<"new thread: "<<endl;
	addr = (int*)mmap(addr, 1024*sizeof(int), PROT_READ|PROT_WRITE, MAP_ANON|MAP_SHARED, -1, 0);
	if (addr == MAP_FAILED){
		cout<<"map failed."<<endl;
		err(1, "mmap");
	}
	for (int i=0; i<1000; i++){
		addr[i] = i;
	}
	*addrAddr = addr;
	return 0;
}






int main(){

	// threads to assign data
	pthread_t threadOnCore0;
	pthread_t threadOnCore1;

	// threads that we want to map	
	int NumThreads = 10;
	pthread_t threads[NumThreads];
	pthread_attr_t attr;
	cpu_set_t cpus;

	// allocate two memory location
	int* closeToNode0;// = new int[1000];
	int* closeToNode1;// = new int[1000];
/*
	for (int i=0; i<1000; i++){
		closeToNode0[i] = i;
		closeToNode1[i] = i+3000;
	}
*/
	struct coreNumAndAddr parameter0;
	struct coreNumAndAddr parameter1;

	parameter0.addrOfAddr = &closeToNode0;
	parameter1.addrOfAddr = &closeToNode1;
	parameter0.addr = closeToNode0;
	parameter1.addr = closeToNode1;

	// create two threads each allocate memory address close to that core
	pthread_attr_init(&attr);
	CPU_ZERO(&cpus);
	CPU_SET(0, &cpus);
	pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpus);
	pthread_create(&threadOnCore0, &attr, assignData, (void*)&parameter0);

	pthread_attr_init(&attr);
	CPU_ZERO(&cpus);
	CPU_SET(6, &cpus);
	pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpus);
	pthread_create(&threadOnCore1, &attr, assignData, (void*)&parameter1);

	pthread_join(threadOnCore0, NULL);
	pthread_join(threadOnCore1, NULL);

	for (int i =0; i<1000; i++){
		int** tmp = parameter0.addrOfAddr;
		if (tmp ==NULL) cout<<"hhhhhh"<<endl;
		cout<<(*tmp)[i]<<endl;
	}


//	for (int i =0; i<1000; i++){
//		cout<<closeToNode1[i]<<endl;
//	}



	// create a bunch of threads randomly assign to different cores.
	for (int i=0; i<NumThreads/2; i++){
		pthread_attr_init(&attr);
		CPU_ZERO(&cpus);
		CPU_SET(0, &cpus);
		pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpus);
		pthread_create(&threads[i], &attr, DoWork, NULL);
	}

	for (int i=NumThreads/2; i<NumThreads; i++){
		pthread_attr_init(&attr);
		CPU_ZERO(&cpus);
		CPU_SET(6, &cpus);
		pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpus);
		pthread_create(&threads[i], &attr, DoWork, NULL);
	}

	for (int i=0; i<NumThreads; i++){
		pthread_join(threads[i], NULL);
	}

	return 0;
}
