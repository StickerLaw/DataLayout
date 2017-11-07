#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string> 

using namespace std;

void* DoWork(void* args){
	printf("x\n");
	string filename = "a.txt";
	
	ofstream myfile(filename.c_str());
	myfile<<"a\n";
	myfile.close();

	return 0;
}


void* DoWorkB(void* args){
	printf("x\n");
	string filename = "b.txt";
	
	ofstream myfile(filename.c_str());
	myfile<<"b\n";
	myfile.close();
	return 0;
}



int main(){
	
	pthread_t threadA;
	pthread_t threadB;

	pthread_attr_t attr;
	cpu_set_t cpus;

	pthread_attr_init(&attr);
	CPU_ZERO(&cpus);
	CPU_SET(0, &cpus);
	pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpus);
	pthread_create(&threadA, &attr, DoWork, NULL);

	CPU_ZERO(&cpus);
	CPU_SET(6, &cpus);
	pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpus);
	pthread_create(&threadB, &attr, DoWorkB, NULL);

	pthread_join(threadA, NULL);
	pthread_join(threadB, NULL);

	return 0;
}
