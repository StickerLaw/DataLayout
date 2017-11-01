#include <iostream>
#include <omp.h>
using namespace std;
int A[100000], B[100000];
int main() {

	omp_set_num_threads(10);
	#pragma omp parallel for schedule(dynamic, 10)
	for (int i = 0; i < 100; ++i) {
		int num = omp_get_thread_num();
		B[i] = i;
		A[i] = B[i]+1;
		#pragma omp critical
		cout << i<<endl; 
	}
		

	return 0;
}

