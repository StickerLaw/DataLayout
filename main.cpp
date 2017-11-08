#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string> 
#include <sys/mman.h>

using namespace std;

struct coreNumAndAddr{
	int cpuNum;
	int* addr;	
};

void* DoWork(void* args){
	//string filename = "a.txt";
	
	//ofstream myfile(filename.c_str());
	//myfile<<"a\n";
	//myfile.close();

	return 0;
}


void* assignData(void* args){
	struct coreNumAndAddr* arg = (struct coreNumAndAddr*) args;
	int cpuIdx = arg->cpuNum;
	int* addr = arg->addr;
	cout<<"new thread: "<<cpuIdx<<endl;
	
	addr = (int*)mmap(addr, 1000*sizeof(int), PROT_READ||PROT_WRITE, MAP_32BIT, 0, 0);

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
	int* closeToNode0 = new int[1000];
	int* closeToNode1 = new int[1000];

	struct coreNumAndAddr parameter0;
	struct coreNumAndAddr parameter1;

	parameter0.cpuNum = 0;
	parameter1.cpuNum = 6;
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
