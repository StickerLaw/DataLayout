#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string> 
#include <sys/mman.h>

using namespace std;

void* DoWork(void* args){
	printf("x\n");
	string filename = "a.txt";
	
	ofstream myfile(filename.c_str());
	myfile<<"a\n";
	myfile.close();

	return 0;
}



int main(){
	
	int NumThreads = 10;

	pthread_t threads[NumThreads];
	pthread_attr_t attr;
	cpu_set_t cpus;

	for (int i=0; i<NumThreads/2; i++){
		pthread_attr_init(&attr);
		CPU_ZERO(&cpus);
		CPU_SET(0, &cpus);
		pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpus);
		pthread_create(&threads[i], &attr, DoWork, NULL);
	}

	for (int i=NumThreads/2+1; i<NumThreads; i++){
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
