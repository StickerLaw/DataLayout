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
	int** addrOfAddr;	
};

struct tidAndAddr{
	int ID;
	int* addr0;
	int* addr1;
};

void* DoWork(void* args){
	// access data according the core number
	struct tidAndAddr* p = (struct tidAndAddr*)args;
	int TID = p->ID;
	int* addr0 = p->addr0;
	int* addr1 = p->addr1;
	if (TID<60) {
		for (int i=0; i<1000; i++){
			int tmp = addr1[i];
		}
	}
	else{
		for (int i=0; i<1000; i++){
			int tmp = addr0[i];
		}

	}
	return 0;
}

// this thread create a memory space near one node. 
// and save this memory spaces to a pointer of pointer.
void* assignData(void* args){
	struct coreNumAndAddr* arg = (struct coreNumAndAddr*) args;
	int** addrAddr = arg->addrOfAddr;
	int* addr = (int*)mmap(NULL, 1024*sizeof(int), PROT_READ|PROT_WRITE, MAP_ANON|MAP_SHARED, -1, 0);
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
	int NumThreads = 120;
	pthread_t threads[NumThreads];
	pthread_attr_t attr;
	cpu_set_t cpus;

	// allocate two memory location
	int* closeToNode0;
	int* closeToNode1;

	struct coreNumAndAddr parameter0;
	struct coreNumAndAddr parameter1;

	parameter0.addrOfAddr = &closeToNode0;
	parameter1.addrOfAddr = &closeToNode1;

	// create two threads each allocate memory address close to that core
	pthread_attr_init(&attr);
	CPU_ZERO(&cpus);
	CPU_SET(3, &cpus);
	pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpus);
	pthread_create(&threadOnCore0, &attr, assignData, (void*)&parameter0);

	pthread_attr_init(&attr);
	CPU_ZERO(&cpus);
	CPU_SET(9, &cpus);
	pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpus);
	pthread_create(&threadOnCore1, &attr, assignData, (void*)&parameter1);

	pthread_join(threadOnCore0, NULL);
	pthread_join(threadOnCore1, NULL);

	// check if the memory space is created successfully.
	for (int i =0; i<1000; i++){
		int** tmp = parameter0.addrOfAddr;
		if (tmp ==NULL) cout<<"error in access to the pointer"<<endl;
		cout<<(*tmp)[i]<<endl;
	}

	for (int i =0; i<1000; i++){
		int** tmp = parameter1.addrOfAddr;
		if (tmp ==NULL) cout<<"error in access to the pointer"<<endl;
		cout<<(*tmp)[i]<<endl;
	}

	// create a bunch of threads randomly distributed them on different cores.
	struct tidAndAddr p[NumThreads];


	for (int i=0; i<NumThreads; i++){
		int** tmp = parameter0.addrOfAddr;
		p[i].addr0 = *tmp;
		tmp = parameter1.addrOfAddr;
		p[i].addr1 = *tmp;
		p[i].ID = i;
	}

	for (int i=0; i<NumThreads/2; i++){
		pthread_attr_init(&attr);
		CPU_ZERO(&cpus);
		CPU_SET(i%6, &cpus);
		int threadNum = i;
		pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpus);
		pthread_create(&threads[i], &attr, DoWork, (void*)&p[i]);
	}

	for (int i=NumThreads/2; i<NumThreads; i++){
		pthread_attr_init(&attr);
		CPU_ZERO(&cpus);
		CPU_SET(i%6+6, &cpus);
		int threadNum = i;
		pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpus);
		pthread_create(&threads[i], &attr, DoWork, (void*)&p[i]);
	}

	for (int i=0; i<NumThreads; i++){
		pthread_join(threads[i], NULL);
	}

	return 0;
}
