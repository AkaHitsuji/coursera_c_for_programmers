//HW2 Submission by Ang Yang
//
//200 word reflection:
//Implementing Djikstra's algorithm from scratch helped me better practise my fundamentals. Having to do things in classes also helped me practice object oriented programming.
//The process was hard and I had to do a lot of research online to figure out syntax and naming issues. One problem I faced was that my direction array was not initialising with the right values when I used INFINITY.
//Switching to INT_MAX and using a vector instead of an array helped solve the problem. But in solving the problem I figured out how to debug on visual studio so that was a valuable lesson as well.
//I chose to use a matrix for my graph representation as I felt that was easier to visualise for undirected graphs. Although the graphs in this case may not be as dense, for a finite graph vertice number of 50, there weren't any significant losses in using
//a matrix as compared to a list. In all this was a good practice exercise.

#include <iostream>
#include <vector>
#include <queue>
#include <ctime>

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
	unsigned int distance;
};

// Comparison function to overload comparison operator in priority queue
class CompareDistance {
public:
	bool operator()(nodeDistance& n1, nodeDistance& n2) {
		if (n1.distance > n2.distance)
			return true;
		else
			return false;
	}
};

class ShortestPath {
public:
	ShortestPath() {};

	// ShortestPath Constructor
	ShortestPath(Graph& graph) : _g(graph) {};

	// returns the path cost associated with the shortest path
	int pathSize(Graph& graph, int source, int target) { return dijkstra(graph, source, target); }

protected:
	// implementation of dijkstra's algorithm
	int dijkstra(Graph& graph, unsigned int source, unsigned int target) {
		// get size of graph to create visited and distance arrays
		const int size = graph.V();

		// init vector visited to keep track of visited nodes, visited is true, not visted is false
		// init vecor distance to keep track of the minimum distance cost to each node, all nodes initialized to INT_MAX
		vector<bool> visited(size, false);
		vector<int> distance(size, INT_MAX);

		// set source distance to 0
		distance[source] = 0;

		// init priority queue
		priority_queue<nodeDistance, vector<nodeDistance>, CompareDistance> pq;

		// place first node into the queue
		nodeDistance sourceNode = { source, 0 };
		pq.push(sourceNode);

		// while queue is not empty, pick the top node, which is the one with the minimum distance
		// take all neighbors of that node and update the distance and insert into priority queue if it has not yet been visited
		while (!pq.empty()) {
			nodeDistance tmp = pq.top();
			pq.pop();

			// if no path is found
			if (tmp.distance == INT_MAX)
				return INT_MAX;

			// if target is found, return distance
			if (tmp.node == target)
				return tmp.distance;

			// get all neighbors
			int currNode = tmp.node;
			vector<unsigned int> neighbors = graph.neighbors(currNode);

			// loop through all neighbors for computation
			for (int i = 0; i < neighbors.size(); ++i) {
				int n = neighbors[i];

				// update distance if it is smaller than the current distance
				if (distance[n] > distance[currNode] + graph.get_edge_value(currNode, n))
					distance[n] = distance[currNode] + graph.get_edge_value(currNode, n);

				// put it into the queue if the currNode has not been visited before
				if (!visited[n]) {
					nodeDistance newNode;
					newNode.node = n;
					newNode.distance = distance[n];
					pq.push(newNode);
					visited[n] = true;
				}
			}
		}
		return INT_MAX;
	}

private:
	Graph _g;
};

int main() {
	// setup parameters
	int verticeNum = 50;
	double density[4] = { 0.2,0.3,0.4,0.5 };

	ShortestPath sp;

	for (int i = 0; i < 4; i++) {
		int count = 0; // keep track of number of graphs that have been traversed
		int sumPathLength = 0; // keep track of total sum of all path lengths
		Graph graph(verticeNum, density[i]); // create graph with setup parameters

		// calculate shortest path to all vertices in generated graph
		for (int j = 1; j < 50; j++) {
			int pathSize = sp.pathSize(graph, 0, j);

			// if shortest path exists, add to sum total of path lengths and increment number of graphs traversed
			if (pathSize != INT_MAX) {
				count++;
				sumPathLength += pathSize;
			}
		}

		cout << "Graph with density: " << density[i];
		cout << " has average path length of " << (static_cast<double>(sumPathLength) / count) << endl;
	}

	return 0;
}