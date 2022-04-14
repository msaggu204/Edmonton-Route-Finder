// ------------------------------------------------------------------
// Name: Manjot Saggu
// CCID: msaggu
// ID: 1693571
// CMPUT 275 Wi22
//
// Assignment Part I: Trivial Navigation System
// -------------------------------------------------------------------

// implementation of the server program

#include <iostream>
#include <unordered_map>
#include <queue>
#include <list>
#include <string>
#include <stack>
#include <fstream>
#include <istream>
#include <map>
#include "wdigraph.h"
#include "dijkstra.h"


using namespace std;

struct Point {
long long lat; // latitude of the point 
long long lon; // longitude of the point
};

long long manhattan(const Point& pt1, const Point& pt2) {
// Return the Manhattan distance between the two given points
	long long lat = abs(pt1.lat - pt2.lat);
	long long lon = abs(pt1.lon - pt2.lon);
	long long manhattanDistance = lat + lon;
	return manhattanDistance;
}
// Auxiliary function for finding the closest vertex in the graph to a given point.
int closest(Point& pt, unordered_map<int, Point>& points){
	int closest = points.begin()->first;
	// Find the absolute distance using manhattan function
	long long distance = manhattan(points[closest], pt);
	// Iterate through each vertex within points to check which is closest
	for(auto i = points.begin(); i != points.end(); i++){
		long long tempDistance = manhattan(i->second, pt);
		if(distance > tempDistance){
			distance = tempDistance;
			closest = i->first;
		}
	}
	return closest;
	// map<long long, int> distance;
	// for(auto it = points.begin(); it != points.end(); it++){
	// 	long long dist = manhattan(pt, it->second);
	// 	distance[dist] = it->first;
	// }
	// int closest = distance.begin()->second;
	// return closest;
}
/*
Read Edmonton map data from the provided file and
load it into the WDigraph object passed to this function.
Store vertex coordinates in Point struct and map each
vertex identifier to its corresponding Point struct variable.
PARAMETERS:
filename: the name of the file that describes a road network
graph: an instance of the weighted directed graph (WDigraph) class
points: a mapping between vertex identifiers and their coordinates
*/
void readGraph(string filename, WDigraph& graph, unordered_map<int, Point>& points){
	ifstream file(filename);
	string input_line;

	while(getline(file, input_line)){
		string command = input_line.substr(0, 1);
		string str = input_line.substr(1, input_line.length());

		if (command == "V"){ // command describing a vertex
			int comma_1 = str.find(","); // find the index value of the first comma
			string Vertex = str.substr(0, comma_1); // Extracting vertex from sting
			// Finding the string that contains the ID and extracting it
			string str_ID = str.substr(comma_1 + 1, str.length() - comma_1 - 1);
			int comma_2 = str_ID.find(",");
			string ID = str_ID.substr(0, comma_2);
			int v = stoi(ID); // Converting the string to int to be added as a vertex
			// Assign latitude and longtitude
			string str_lat = str_ID.substr(comma_2 + 1, str_ID.length() - comma_2 - 1);
			int comma_3 = str_lat.find(",");
			string latitude = str_lat.substr(0, comma_3);
			double coordLat = stod(latitude); // Latitude is taken in as a double

			string str_long = str_lat.substr(comma_3 + 1, str_lat.length() - comma_3 - 1);
			string longtitude = str_long;
			double coordLon = stod(longtitude);
			// Pass along the vertex to be created
			graph.addVertex(v);
			// Convert and store the latitude and longtitude to be stored as long long
			long long lat = static_cast<long long>(coordLat*100000);
			long long lon = static_cast<long long>(coordLon*100000);

			// Instantiate an object of Point to store the lat and lon
			Point coord;
			// Store the coordinates for the unique ID vertex in the map
			// Store lat and lon in the data structure called Point
			coord.lat = lat;
			coord.lon = lon;
			points[v] = coord;
		} else{ // command == "E", describing an edge/street
			int comma_1 = str.find(","); // Don't need to store the string "E"
			// Finding the string that contains the IDs for start/end and extracting it
			string str_start = str.substr(comma_1 + 1, str.length() - comma_1 - 1);
			int comma_2 = str_start.find(",");
			string start = str_start.substr(0, comma_2);
			int u = stoi(start); // ID of start

			string str_end = str_start.substr(comma_2 + 1, str_start.length() - comma_2 - 1);
			int comma_3 = str_end.find(",");
			string end = str_end.substr(0, comma_3);
			int v = stoi(end); // ID of end
			// Assign street name but ignore for this part of the assignment
			string str_name = str_end.substr(comma_3 + 1, str_end.length() - comma_3 - 1);
			string name = str_name;	
			// Pass along the vertices of the edge to determine the weight
			long long w = manhattan(points[u], points[v]);
			// Pass along IDs of 2 vertices connected by the edge, along with calculated edge weight
			graph.addEdge(u, v, w);
			// graph.addEdge(v, u); NOT an undirected graph so we can represent one-way streets
		}
	}
}

int main(){
	WDigraph graph;
	unordered_map<int, Point> points;
	readGraph("edmonton-roads-2.0.1.txt", graph, points);

	string command;
	// string startLat, startLon, endLat, endLon;
	Point startP, endP;
	// First command will be 'R'
	cin >> command >> startP.lat >> startP.lon >> endP.lat >> endP.lon;
	// Finding the closest points to each coordinate provided
	int start = closest(startP, points);
	int end = closest(endP, points);
	// Pass the values through to Dijkstra
	unordered_map<int, PIL> tree;
	dijkstra(graph, start, tree);
	// Traverse the try to ensure that there is a path to be printed out
	if(tree.find(end) == tree.end()){
		cout << "N 0" << endl;
		getline(cin, command);
		if(command == "A"){
			cout << "E" << endl;
		}
	} else{ // Otherwise create the path vector to hold the path values
		vector<int> path;
		int v = end;
		int waypoints = 1;
		// As long as the start and end values aren't the same, traverse the tree
		while(end != start){
			path.push_back(end);
			end = tree[end].first;
			waypoints++;
		}
		// Push one last time to ensure the start coordinate is added
		path.push_back(start);
		// B/c it is a vector, we need to print after reversting order
		reverse(path.begin(), path.end());
		cout << "N " << path.size() << endl;
		// Get a command from the user for each request
		for(int i = 0; i < waypoints; i++){
			bool client = false;
			while(!client){
				getline(cin, command);
				if(command == "A"){
					break;
				}
			}
			// Only print if the user provides the correct command
			cout << "W" << " " << points[path.at(i)].lat << " " << points[path.at(i)].lon << endl;
		}
		// Get the last command line from the user in order to let them know its the end
		getline(cin, command);
		if(command == "A"){
			cout << "E" << endl;
		}
	}
}