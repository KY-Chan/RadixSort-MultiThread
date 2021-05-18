#include "RadixSortMultiThread2.h"

#include "iostream"
#include "time.h"
#include "algorithm"
#include <thread>

using namespace std;
using std::thread;

#define MAX 20000000
#define THD 60
#define BIT 4
// BIT < 32

void rSort1(int* arr, int size);
void rSort2(int* arr, int size);
void rSort3(int* arr, int size);
void rSortThread(int* arr, int** v, int size, int maxPos, int* vCount, int pos, int td, int tempSize);
void rSortThread2(int* arr, int** v, int size, int* vCount, int pos, int td, int tSize, int bSize);

int tenPow(int pow);

int main() {
	clock_t start, end;
	start = clock();
	int* numbers1;
	int* numbers2;
	numbers1 = new int[MAX];
	numbers2 = new int[MAX];
	srand((unsigned int)time(NULL));
	for (int i = 0; i < MAX; i++) {
		numbers1[i] = numbers2[i] = rand() * rand() + rand();
	}

	end = clock();
	cout << "생성된 숫자 갯수 : " << MAX << "\n"
		<<"숫자 생성 시간 : " << end - start << "ms\n";


	cout << "------------------------------------------\n";
	start = clock();
	rSort2(numbers1, MAX);
	end = clock();
	cout << "기수 정렬2 소모 시간 : " << end - start << "ms\n";

	cout << "------------------------------------------\n";

	start = clock();
	rSort3(numbers2, MAX);
	end = clock();
	cout << "기수 정렬3 소모 시간 : " << end - start << "ms\n";

	cout << "------------------------------------------\n";
	bool equals = true;
	for (int i = 0; i < MAX; i++) {
		if (numbers1[i] != numbers2[i])
			equals = false;
	}

	if (equals)
		cout << "성공적으로 정렬됨\n";
	else
		cout << "정렬 실패\n";

	delete[] numbers1;
	delete[] numbers2;
	return 0;
}

void rSort1(int* arr, int size) {
	int vCount[10];
	int** v;
	v = new int* [10];
	for (int i = 0; i < 10; i++) {
		v[i] = new int[size];
	}

	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < MAX; j++) {
			v[i][j] = 0;
		}
	}

	for (int i = 0; i < 10; i++) {
		vCount[i] = 0;
	}



	int maxNum = 0;
	int maxPos = 0;
	for (int i = 0; i < size; i++) {
		if (maxNum < arr[i]) {
			maxNum = arr[i];
		}
	}
	while (true) {
		if ((maxNum = maxNum / 10) > 0)
			maxPos++;
		else {
			maxPos++;
			break;
		}
	}

	for (int i = 0; i < maxPos; i++) {
		for (int j = 0; j < size; j++) {
			int key = arr[j] % (int)pow(10, i + 1) / (int)pow(10, i);
			v[key][vCount[key]++] = arr[j];
		}
		int count = 0;
		for (int i = 0; i < 10; i++) {
			for (int j = 0; j < vCount[i]; j++) {
				arr[count++] = v[i][j];
			}
		}
		for (int i = 0; i < 10; i++) {
			vCount[i] = 0;
		}
	}

	for (int i = 0; i < 10; i++) {
		delete[] v[i];
	}
	delete[] v;

}

void rSort2(int* arr, int size) {
	int*** v;
	v = new int** [THD];
	int tSize = size / THD + 1; // 쓰레드 별 계산할 숫자 개수
	for (int i = 0; i < THD; i++) {
		v[i] = new int* [10];
		for (int j = 0; j < 10; j++) {
			v[i][j] = new int[tSize];
		}
	}



	int maxNum = 0;
	int maxPos = 0;
	for (int i = 0; i < size; i++) {
		if (maxNum < arr[i]) {
			maxNum = arr[i];
		}
	}
	do {
		maxPos++;
	} while ((maxNum = maxNum / 10) > 0);

	int vCount[THD][10];
	for (int i = 0; i < THD; i++) {
		for (int j = 0; j < 10; j++) {
			vCount[i][j] = 0;
		}
	}

	
	for (int pos = 0; pos < maxPos; pos++) {
		thread thr[THD];
		for (int td = 0; td < THD; td++) {
			thr[td] = thread(rSortThread, arr, v[td], size, maxPos, vCount[td], pos, td, tSize);
		}
		for (int td = 0; td < THD; td++) {
			thr[td].join();
		}

		int count = 0;
		for (int j = 0; j < 10; j++) {
			for (int td = 0; td < THD; td++) {
				for (int k = 0; k < vCount[td][j]; k++) {
					arr[count++] = v[td][j][k];
				}
			}
		}
		for (int td = 0; td < THD; td++) {
			for (int j = 0; j < 10; j++) {
				vCount[td][j] = 0;
			}
		}
	}

	for (int i = 0; i < THD; i++) {
		for (int j = 0; j < 10; j++) {
			delete[] v[i][j];
		}
		delete[] v[i];
	}
	delete[] v;

}

void rSortThread(int* arr, int** v, int size, int maxPos, int* vCount, int pos, int td, int tempSize) {
	for (int i = td * tempSize; i < min(tempSize * (td + 1), size); i++) {
		int key = arr[i] / tenPow(pos) % 10;
		v[key][vCount[key]++] = arr[i];
	}
}

int tenPow(int pow) {
	int temp = 1;
	for (int i = 0; i < pow; i++) {
		temp = temp * 10;
	}
	return temp;
}

void rSort3(int* arr, int size) {
	int*** v;
	v = new int** [THD];
	int tSize = size / THD + 1; // 쓰레드 별 계산할 숫자 개수
	int bSize = 1; // BIT 갯수별 표현숫자 개수 (비트 4개 : 1111까지 : 16개
	for (int j = 0; j < BIT; j++) {
		bSize *= 2;
	}
	for (int i = 0; i < THD; i++) {
		v[i] = new int* [bSize];
		for (int j = 0; j < bSize; j++) {
			v[i][j] = new int[tSize];
		}
	}
	int maxNum = 0;
	int maxPos = 0;
	for (int i = 0; i < size; i++) {
		if (maxNum < arr[i]) {
			maxNum = arr[i];
		}
	}
	do {
		maxPos++;
	} while ((maxNum = maxNum >> 1) > 0);

	int** vCount;
	vCount = new int* [THD];
	for (int i = 0; i < THD; i++) {
		vCount[i] = new int[bSize];
	}

	for (int i = 0; i < THD; i++) {
		for (int j = 0; j < bSize; j++) {
			vCount[i][j] = 0;
		}
	}

	for (int pos = 0; pos < maxPos; pos+=BIT) {
		thread thr[THD];
		for (int td = 0; td < THD; td++) {
			thr[td] = thread(rSortThread2, arr, v[td], size, vCount[td], pos, td, tSize, bSize);
		}
		for (int td = 0; td < THD; td++) {
			thr[td].join();
		}
		int count = 0;

		for (int j = 0; j < bSize; j++) {
			for (int td = 0; td < THD; td++) {
				for (int k = 0; k < vCount[td][j]; k++) {
					arr[count++] = v[td][j][k];
				}
			}
		}
		for (int td = 0; td < THD; td++) {
			for (int j = 0; j < bSize; j++) {
				vCount[td][j] = 0;
			}
		}
	}



	for (int i = 0; i < THD; i++) {
		for (int j = 0; j < bSize; j++) {
			delete[] v[i][j];
		}
		delete[] v[i];
		delete[] vCount[i];
	}
	delete[] v;
	delete[] vCount;

}

void rSortThread2(int* arr, int** v, int size, int* vCount, int pos, int td, int tSize, int bSize) {
	for (int i = td * tSize; i < min(tSize * (td + 1), size); i++) {
		int key = (arr[i] >> pos) & (bSize - 1);
		v[key][vCount[key]++] = arr[i];
	}
}