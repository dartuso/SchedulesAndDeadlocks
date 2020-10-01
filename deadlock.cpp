/*
 * 457 Assignment 4: Q6 deadlock
 * L1 T04
 * Daniel Artuso
 * 30029826

Command line
Your program will take no command line arguments. Your program will read the standard input
to get state descriptions.
Input
Your program will read the descriptions of each system state from standard input. The input will
be line-based, with 3 types of lines: one represeting an assignment edge, one representing a
request edge and one representing an end of state description.
• a line N -> M represents a request edge, i.e. process N is requesting resource M;
• a line N <- M represents an assignment edge, i.e. process N holds resource M;
• a line that starts with # represents the end of state description.
• “N” and “M” above will be non-negative integers.
As an example, consider the following 4 system states:
 The 4 system states above would be represented by the input below, and the output your program
should produce on this input is shown on the right.
 Notice there is no explicit end of state line # at the end of the input above, as it is implied by
the end-of-file. You may assume the following limits on input:
• process numbers will be in range [0 … 100000];
• resource numbers will be in range [0 … 100000];
• number of edges per state description will be in range [1 … 100000];
• number of states per input will be in range [1 … 20].
Your solution must be efficient enough to be able run on any input within the above limits in less
than 10 seconds.
Output
Your program will write the results to standard output. For each state it reads, it will print out
how the list of processes involved in the deadlock, sorted in ascending order. If there is no
deadlock, it should print out the word ‘None’. Your output should match the sample output above
exactly.
 */

#include <iostream>
#include<bits/stdc++.h>

#define MAXN 100001
#define TOTAL 2*MAXN

using namespace std;

class Graph {
	int V = TOTAL;
	list<int> *adj;
	list<int> found;
	bool isCyclicHelper(int v, bool *visited, bool *recStack);

public:
	virtual ~Graph();

	explicit Graph();

	void addEdge(int v, int w);

	bool isCyclic();

	void printFound();
};

Graph::Graph() {
	adj = new list<int>[V];
}

Graph::~Graph() {
	adj->clear();
}

//Credit for DFS cycle detection: geeksforgeeks.org/detect-cycle-in-a-graph/
bool Graph::isCyclic() {
	bool *visited = new bool[V];
	bool *recStack = new bool[V];
	for (int i = 0; i < V; i++) {
		visited[i] = false;
		recStack[i] = false;
	}
	for (int i = 0; i < V; i++)
		if (isCyclicHelper(i, visited, recStack)) {
			return true;
		}
	return false;
}

bool Graph::isCyclicHelper(int v, bool visited[], bool *recStack) {
	if (!visited[v]) {
		// Mark the current node as visited and part of recursion stack
		visited[v] = true;
		recStack[v] = true;

		// Recur for all the vertices adjacent to this vertex
		list<int>::iterator i;
		for (i = adj[v].begin(); i != adj[v].end(); ++i) {
			if (!visited[*i] && isCyclicHelper(*i, visited, recStack)) {
				if (v < MAXN){
					found.push_front(v);
				}
				return true;
			} else if (recStack[*i]) {
				if (v < MAXN){
					found.push_front(v);
				}

				return true;
			}
		}

	}
	recStack[v] = false;  // remove the vertex from recursion stack
	return false;
}

void Graph::addEdge(int v, int w) {
	adj[v].push_back(w);
}

void Graph::printFound() {
	//Print like this due to recursion order (and to replicate sample output)
	cout << found.front() << " ";
	found.pop_front();
	while (!found.empty()){
		cout << found.back() << ' ';
		found.pop_back();
	}
	cout << endl;
}


int main() {
	bool notEnd = true;
	while (notEnd) {
		Graph graph;
		while (true) {
			string input, input1, input2, input3;
			getline(cin, input);
			notEnd = !input.empty();
			if (input.find('#') != string::npos || !notEnd) {
				//compute
				cout << "Deadlocked processes: ";
				if (!graph.isCyclic()) {
					cout << "none" << endl;
				} else {
					graph.printFound();
				}
				break;
			} else {
				//parse edge and add to graph
				input1 = input.substr(0,input.find_first_of(' '));
				input3 = input.substr(input.find_last_of(' '),string::npos);
				input2 = input.substr(input.find_first_of(' ') + 1,2);
				size_t found = input2.find("->");
				if (found != string::npos) {
					int process = stoi(input1);
					int resource = stoi(input3) + MAXN;
					graph.addEdge(process, resource);
				} else {
					int process = stoi(input1);
					int resource = stoi(input3) + MAXN;
					graph.addEdge(resource, process);
				}
			}
		}
	}
	return 0;
}