//HW3 Submission by Ang Yang
//This program reads the text file provided within the same folder and constructs a graph from the provided data.
//Using the provided graph, the program will then calculate the MST and print out the cost and MST to the console.

#include <iostream>
#include <iterator>
#include <fstream>
#include <algorithm>
#include <vector>
#include <ctime>
#include <string>
#include <tuple>
#include <queue>

#define INF 10000 // infinite

using namespace std;

class Graph {
public:
	Graph() {};

	// Graph constructor
	Graph(unsigned int v) : _v(v), _e(0), _adjMatrix(v, vector<unsigned int>(v)) {};

	// Graph constructor for density
	Graph(unsigned int v, double density) : _v(v), _e(0) {
		_adjMatrix = vector<vector<unsigned int>>(v, vector<unsigned int>(v));

		srand(time(0)); // seed rand
		const int RANGE = 10; // set range value

		// populate adjMatrix
		for (int i = 0; i < v - 1; i++) {
			for (int j = 1; j < v; j++) {
				double prob = (rand() % 100) / 100.0;

				// condition for creating an edge value
				if (prob < density) {
					int value = rand() % RANGE + 1;
					add_edge(i, j, value);
				}
			}
		}
	}

	// new method to create graph from file
	void createGraph() {
		// vector to store list of tuples in the format <node i, node j, weight>
		vector<tuple<int, int, int>> listTuples;
		int numNodes;

		// fread code from class
		ifstream data_file("sampledata.txt");

		if (data_file.is_open()) {
			istream_iterator<string> start(data_file), end;
			vector<string> data(start, end);

			numNodes = stoi(data[0]);

			// create new vector to read from second line onwards
			vector<string> data_tuples(data.begin() + 1, data.end());
			// store data in each line in listTuples in tuple format for easy access
			for (int n = 0; n < data_tuples.size(); n += 3) {
				int i = stoi(data_tuples[n]);
				int j = stoi(data_tuples[n + 1]);
				int weight = stoi(data_tuples[n + 2]);

				auto edge = make_tuple(i, j, weight);
				listTuples.push_back(edge);
			}
		}
		else {
			cout << "Unable to open file." << endl;
		}

		// set number of nodes
		_v = numNodes;

		// create adjMatrix graph representation from data
		_adjMatrix = vector<vector<unsigned int>>(_v, vector<unsigned int>(_v));

		for (auto t : listTuples) {
			int i = get<0>(t);
			int j = get<1>(t);
			int weight = get<2>(t);

			add_edge(i, j, weight);
		}
	}

	// return number of vertices in Graph
	unsigned int V() { return _v; }

	// return number of edges in Graph
	unsigned int E() { return _e; }

	// return true if there is an edge from node x to node y
	bool adjacent(unsigned int x, unsigned int y) {
		return _adjMatrix[x][y] > 0;
	}

	// lists all nodes y such that there is an edge from x to y
	vector<unsigned int> neighbors(unsigned int x) {
		vector<unsigned int> edgeList;
		for (int i = 0; i < _v; ++i) {
			if (adjacent(x, i))
				edgeList.push_back(i);
		}

		return edgeList;
	}

	// adds to Graph the edge from x to y, if it is not there
	bool add_edge(unsigned int x, unsigned int y, unsigned int weight) {
		if (_adjMatrix[x][y] > 0)
			return false;

		// keep track of edge count by incrementing
		_e++;

		_adjMatrix[x][y] = weight;
		_adjMatrix[y][x] = weight;

		return true;
	}

	// removes edge from x to y if it is there
	bool remove_edge(unsigned int x, unsigned int y) {
		if (_adjMatrix[x][y] > 0) {
			_e--;
			_adjMatrix[x][y] = 0;
			_adjMatrix[y][x] = 0;
			return true;
		}

		return false;
	}

	// returns value associated with edge x,y
	unsigned int get_edge_value(unsigned int x, unsigned int y) {
		return _adjMatrix[x][y];
	}

	// sets the value associated to the edge x,y to val
	void set_edge_value(unsigned int x, unsigned int y, unsigned int val) {
		_adjMatrix[x][y] = val;
		_adjMatrix[y][x] = val;
	}

private:
	// unsigned use to guarantee only positive numbers are used
	unsigned int _v;
	unsigned int _e;
	vector<vector<unsigned int>> _adjMatrix;
};

// defined struct for use in minheap, which sorts by distance
struct nodeDistance {
	int node;
	int distance;
};

// Comparison function to overload comparison operator in priority queue
class CompareDistance {
public:
	bool operator()(nodeDistance& n1, nodeDistance& n2) {
		return n1.distance < n2.distance;
	}
};

class MST {
public:
	MST() {};

	MST(Graph& graph) : _g(graph) {};

	// public call to run prim MST algorithm
	void runPrimMST(Graph& graph, int source) { prim(graph, source); }

	// returns cost of MST
	int cost() { return _sumCost; }

	// prints result of MST to console
	void printMSTresults() {
		cout << "Cost of MST found using Prim algorithm: " << cost() << endl;
		cout << "Minimum Spanning Tree found using Prim algorithm:" << endl;
		
		cout << "parent\t-\tnode\t:\tweight" << endl;
		for (int i=0; i < _trace.size(); ++i) {
			cout << _trace[i] << "\t-\t" << i << "\t:\t" << _cost[i] << endl;
		}
	}

protected:
	// prim algorithm implementation
	void prim(Graph& graph, int source) {
		// get size of graph to create vectors
		const int size = graph.V();

		// init vector to keep track of vertexes in MST
		// init vector distance to keep track of distance to each node
		// _trace vector to store MST
		vector<bool> inMST(size, false);
		_cost = vector<int>(size, INF);
		_trace = vector<int>(size, -1);

		// init priority queue
		priority_queue<nodeDistance, vector<nodeDistance>, CompareDistance> pq;

		// place first node into queue
		nodeDistance sourceNode;
		sourceNode.distance = 0;
		sourceNode.node = source;
		pq.push(sourceNode);
		_cost[source] = 0;

		// loop through till pq is empty
		while (!pq.empty()) {
			nodeDistance minNode = pq.top();
			pq.pop();

			// include node in MST
			inMST[minNode.node] = true;

			// get all neighbors
			vector<unsigned int> neighbors = graph.neighbors(minNode.node);

			// loop through all neighbors for computation
			for (auto i : neighbors) {
				int weight = graph.get_edge_value(minNode.node, i);
				// if neighbor node is not in MST and weight of edge to neighbor node is smaller than the current distance value of the neighbor, update
				if (inMST[i] == false && _cost[i] > weight) {
					_cost[i] = weight;
					nodeDistance newNode;
					newNode.node = i;
					newNode.distance = _cost[i];
					pq.push(newNode);
					_trace[i] = minNode.node;
				}
			}
		}

		// compute cost of MST
		for (auto i : _cost) {
			_sumCost += i;
		}
	}

private:
	Graph _g;
	vector<int> _trace;
	vector<int> _cost;
	int _sumCost;
};

int main() {
	Graph graph;
	MST mst;

	graph.createGraph();
	// test if graph is created properly
	cout << graph.get_edge_value(0, 1) << endl; //value should be 17
	cout << graph.get_edge_value(2, 16) << endl; //value should be 25

	// run prim and print out cost and tree
	mst.runPrimMST(graph, 0);
	mst.printMSTresults(); // cost should be 30

	return 0;
}