// ------------------------------------------------------------------
// Assignment Part II: Trivial Navigation System
// -------------------------------------------------------------------

#include <iostream>
#include <cassert>
#include <fstream>
#include <string>
#include <list>
#include <cstring>

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "wdigraph.h"
#include "dijkstra.h"

#define buffer 1024

struct Point {
    long long lat, lon;
};

// returns the manhattan distance between two points
long long manhattan(const Point& pt1, const Point& pt2) {
  long long dLat = pt1.lat - pt2.lat, dLon = pt1.lon - pt2.lon;
  return abs(dLat) + abs(dLon);
}

// finds the id of the point that is closest to the given point "pt"
int findClosest(const Point& pt, const unordered_map<int, Point>& points) {
  pair<int, Point> best = *points.begin();

  for (const auto& check : points) {
    if (manhattan(pt, check.second) < manhattan(pt, best.second)) {
      best = check;
    }
  }
  return best.first;
}

// read the graph from the file that has the same format as the "Edmonton graph" file
void readGraph(const string& filename, WDigraph& g, unordered_map<int, Point>& points) {
  ifstream fin(filename);
  string line;

  while (getline(fin, line)) {
    // split the string around the commas, there will be 4 substrings either way
    string p[4];
    int at = 0;
    for (auto c : line) {
      if (c == ',') {
        // start new string
        ++at;
      }
      else {
        // append character to the string we are building
        p[at] += c;
      }
    }

    if (at != 3) {
      // empty line
      break;
    }

    if (p[0] == "V") {
      // new Point
      int id = stoi(p[1]);
      assert(id == stoll(p[1])); // sanity check: asserts if some id is not 32-bit
      points[id].lat = static_cast<long long>(stod(p[2])*100000);
      points[id].lon = static_cast<long long>(stod(p[3])*100000);
      g.addVertex(id);
    }
    else {
      // new directed edge
      int u = stoi(p[1]), v = stoi(p[2]);
      g.addEdge(u, v, manhattan(points[u], points[v]));
    }
  }
}

int create_and_open_fifo(const char * pname, int mode) {
  // creating a fifo special file in the current working directory
  // with read-write permissions for communication with the plotter
  // both proecsses must open the fifo before they can perform
  // read and write operations on it
  if (mkfifo(pname, 0666) == -1) {
    cout << "Unable to make a fifo. Ensure that this pipe does not exist already!" << endl;
    exit(-1);
  }

  // opening the fifo for read-only or write-only access
  // a file descriptor that refers to the open file description is
  // returned
  int fd = open(pname, mode);

  if (fd == -1) {
    cout << "Error: failed on opening named pipe." << endl;
    exit(-1);
  }

  return fd;
}

// keep in mind that in part 1, the program should only handle 1 request
// in part 2, you need to listen for a new request the moment you are done
// handling one request
int main() {
  WDigraph graph;
  unordered_map<int, Point> points;

  const char *inpipe = "inpipe";
  const char *outpipe = "outpipe";

  // Open the two pipes
  int in = create_and_open_fifo(inpipe, O_RDONLY);
  cout << "inpipe opened..." << endl;
  int out = create_and_open_fifo(outpipe, O_WRONLY);
  cout << "outpipe opened..." << endl;  

  // build the graph
  readGraph("server/edmonton-roads-2.0.1.txt", graph, points);
  // Create an intake character array to receive from the client
  char line1[buffer]; // One each for the 2 lines of input received
  char line2[buffer];

  while(true){
    memset(line1, 0, buffer);
    memset(line2, 0, buffer);
    char request;
    char temp1;
    int index = 0;
    // Reading start point from inpipe
    // Delimiter based messaging
    while(true){
      int point1 = read(in, &temp1, 1);
      if(temp1 == '\n'){
        break;
      } else{
        line1[index] = temp1;
        index++;
      }
    }
    // If the input from the client is Q, then quit the program and clean up
    if(strcmp("Q", line1) == 0){
      request = 'Q';
      break;
    }
    // Read the second line from the input/read coming from client
    // Intake the starting point and concatenate it
    index = 0;
    while(true){
      int point2 = read(in, &temp1, 1);
      if(temp1 == '\n'){
        break;
      } else{
        line2[index] = temp1;
        index++;
      }
    }
    // Intake the starting point and concatenate it
    vector<string> startVec, endVec;
    string temp;
    for(int i = 0; i < buffer; i++){
      if(line1[i] == ' '){
        startVec.push_back(temp);
        temp.erase();
      } else if(line1[i] == '\0'){
        startVec.push_back(temp);
        break;
      } else{
        temp += line1[i];
      }
    }
    // Reading end point from inpipe - concatenate it as done for the 1st line
    temp = "";
    for(int j = 0; j < buffer; j++){
      if(line2[j] == ' '){
        endVec.push_back(temp);
        temp.erase();
      } else if(line2[j] == '\0'){
        endVec.push_back(temp);
        break;
      } else{
        temp += line2[j];
      }
    }
    // Convert degrees to 100,000-ths of degrees, required for Djikstra's
    long long startLat = static_cast<long long>(stod(startVec[0]) * 100000);
    long long startLon = static_cast<long long>(stod(startVec[1]) * 100000);
    long long endLat = static_cast<long long>(stod(endVec[0]) * 100000);
    long long endLon = static_cast<long long>(stod(endVec[1]) * 100000);
    // Instantiate the starting point and the ending point as a long long
    Point sPoint, ePoint;
    sPoint.lat = startLat;
    sPoint.lon = startLon;
    ePoint.lat = endLat;
    ePoint.lon = endLon;

    // get the points closest to the two points we read
    int start = findClosest(sPoint, points), end = findClosest(ePoint, points);
    // run dijkstra's algorithm, this is the unoptimized version that
    // does not stop when the end is reached but it is still fast enough
    unordered_map<int, PIL> tree;
    dijkstra(graph, start, tree);

    // If no waypoints, print exit message
    if (tree.find(end) == tree.end()) {
      // Must be sent as a character array to the client
      string exitStr = "E\n";
      char exit[exitStr.length()];
      strcpy(exit, exitStr.c_str());
      write(out, exit, sizeof(exit));
      continue;    
    } else{ // If there is a waypoint, continue on
      // read off the path by stepping back through the search tree
      list<int> path;
      while (end != start) {
        path.push_front(end);
        end = tree[end].first;
      }
      if(path.size() > 0){ // Only run this is there is a path
        path.push_front(start);
      }
      // path.push_front(start);
      for (int v : path) {
        // Send the waypoints back to the client to be plotted, undo the conversion
        double outLat = static_cast<double>(points[v].lat) / 100000;
        double outLon = static_cast<double>(points[v].lon) / 100000;
        // Put together the string to be sent back to the client with the waypoints
        string strLat = to_string(outLat); strLat.pop_back(); // Remove trailing zero
        string strLon = to_string(outLon); strLon.pop_back(); // Remove trailing zero
        string outStr = strLat + " " + strLon + "\n"; // final string to be sent
        // Must be sent back to the client as a character array
        char waypoint[outStr.length()];
        // Copy over the final string into a char array
        strcpy(waypoint, outStr.c_str());
        write(out, waypoint, sizeof(waypoint));
      }
    }
    // No more waypoints in this particular path
    string exitStr = "E\n";
    char exit[exitStr.length()];
    strcpy(exit, exitStr.c_str());
    write(out, exit, sizeof(exit));
  }
  // close pipe from the write end 
  close(in);
  close(out);
  // reclaim the backing file 
  unlink(inpipe);
  unlink(outpipe);
  return 0;
}
