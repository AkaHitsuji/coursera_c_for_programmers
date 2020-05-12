// HW1 Submission by Ang Yang

#include <iostream>
#include <vector>

using namespace std;

const int N = 40;

// use call by reference instead of call by value
inline void sum(int& total, const vector<int>& data) {
	total = 0;

	for (int i = 0; i < data.size(); i++) {
		total += data[i];
	}
}

int main() {
	//init variables
	int accum = 0;
	vector<int> data; //vector replaces array

	//populate vector with data
	for (int i = 0; i < N; i++) {
		data.push_back(i); //push_back used to insert element at end of vector
	}

	//function call to sum up variables in vector data
	sum(accum, data);

	//print results to screen
	cout << "sum is " << accum << endl;

	return 0;
}