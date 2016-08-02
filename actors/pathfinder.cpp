//Names:      Jared Shahbazian, Benjamin Nagengast
//Assignment: PA4
//File:       Pathfinder.cpp

/** File Description ********************************************************
 *  This file runs a program that finds the shortest path between two actors.
 *  Actors are connected by movies that they worked in.
 *
 *  Command Line args:
 *  1: a tsv file containing actors and movies they have worked on
 *  2: 'u' for unwighted search, 'w' for a weighted search
 *  3: a tsv with a header, and lines of pairs of actors
 *  4: a tsv file to output to
 */

#include <iostream>
#include <algorithm>
#include <vector>
#include <fstream>
#include <queue>
#include <string>
#include <sstream>
#include <cstdint> 
#include "Movie.hpp"
#include "ActorGraph.hpp"

using namespace std;

/** Class: compareDist
 *  This class is used as a helper for the Djikstra Algorithm below
 */
class compareDist {
  public:
  bool operator()(pair<ActorNode *,int> n1,pair<ActorNode *,int> n2) {
      return n1.second>n2.second;
  }
};

/** Method: DjikstraSearch
 *  This method performs the BFS algorithm
 */
void DjikstraSearch(ofstream *ofs,
  std::unordered_map<std::string,ActorNode *> actors, ActorNode * actor_one, 
    ActorNode * actor_two){

  for(auto setNodesItr = actors.begin(); setNodesItr != actors.end(); setNodesItr++){
    //Set each actors dist = infinity
    (*setNodesItr).second->dist = numeric_limits<int>::max();
    //prev fields are -1
    (*setNodesItr).second->prev = -1;
    // done fields to false;
    (*setNodesItr).second->done = false;
  }

  string path = "";
  priority_queue<pair<ActorNode*, int>,vector<pair<ActorNode *, int>>, compareDist> toExplore;
  ActorNode* start = actor_one;
  start->dist = 0;
  toExplore.push(make_pair(start,0));
  int currentDist = 0;
  bool done = false;

  // While PQ is not empty
  while(!toExplore.empty()){
    //Dequeue node v from front of queue
    ActorNode* next = toExplore.top().first;
    toExplore.pop();

    if(next->done == false){
      next->done = true;
    }

    //If the destination node has been visited, then the algorithm is done.
    if(next->name == actor_two->name && next->done == true){
      path = "(" + next->name + ")" + '\n';
      ActorNode * traverseNode = next->connectedActors[next->prev]->associatedActor;
      ActorNode * priorNode = next;
      while(1){
        string sharedMovieName = traverseNode->connectedActors[priorNode->name]->movieName;
        string year = to_string(traverseNode->connectedActors[priorNode->name]->date);
        path = "(" + traverseNode->name + ")" + "--[" + sharedMovieName + "#@" + year + "]-->"  + path;

        if(traverseNode == actor_one){
          break;
        }
        priorNode = traverseNode;
        traverseNode = traverseNode->connectedActors[traverseNode->prev]->associatedActor;
      }
    break;
    }
    
    //for each of V's neighbors
    for(auto itr = next->connectedActors.begin();itr != next->connectedActors.end();itr++){
      ActorNode * neighbor = (*itr).second->associatedActor;
      int c = 0;
      int edgeWeight = (*itr).second->weight;
      c = next->dist + edgeWeight;
      if(c < neighbor->dist){
        //neighbor.dist = c
        neighbor->dist = c;
        neighbor->prev = next->name;
        toExplore.push(make_pair(neighbor,c));
      }
    }
    if(done)
      break;
  }
  *ofs << path;
}


/** Main
 *  The driver firsts reads in input files and compiles vectors of the actors 
 *  and Movies. It then adds edges to our graph of actors. Finally, it performs
 *  Djikstras algorithm to find the path. Djikstras takes care of printing.
 */
int main(int argc, char* argv[]) {
  if(argc != 5){
    cout << "Wrong number of args! TERMINATING" << endl;
    exit(0);
  }
  ActorGraph * actorsGraph = new ActorGraph();
  bool use_weighted_edges;

  if(*argv[2] == 'u'){
    use_weighted_edges = false;
  }
  else if(*argv[2] == 'w'){
    use_weighted_edges = true;
  }

  actorsGraph->loadFromFile(argv[1], use_weighted_edges);

  //assign the edges for each node in the graph
  for(auto it = actorsGraph->movies.begin(); it != actorsGraph->movies.end(); ++it){
    actorsGraph->assignEdges(it->second, use_weighted_edges);
  }
  ofstream * ofs = new ofstream(argv[4]);
  ifstream infile(argv[3]);
  bool have_header = false;

  *ofs << "(actor)--[movie#@year]-->(actor)--..." <<endl;

  //while you can, Read in a pair from argv[3]
  while (infile){
    string s;
    // get the next line
    if (!getline( infile, s )) break;

    if (!have_header){
      // skip the header
      have_header = true;
      continue;
    }

    istringstream ss( s );
    vector <string> actorPair;

    while (ss) {
      string next;
      if (!getline( ss, next, '\t' ))
        break;
      actorPair.push_back( next );
    }
    if (actorPair.size() != 2)
      continue;

    string firstActorsName(actorPair[0]);
    string secondActorsName(actorPair[1]);

    ActorNode * actor_one;
    ActorNode * actor_two;

    //Search for the first actor
    auto result = actorsGraph->actors.find(firstActorsName);
    if(result == actorsGraph->actors.end()){
      cout << "Actor " << firstActorsName << "is not in the graph." << endl;
      exit(1);
    }
    else
      actor_one = result->second;

    //Search for the second actor
    result = actorsGraph->actors.find(secondActorsName);
    if(result == actorsGraph->actors.end()){
      cout << "Actor " << secondActorsName << "is not in the graph." << endl;
      exit(1);
    }
    else
      actor_two = result->second;

    DjikstraSearch(ofs,actorsGraph->actors,actor_one, actor_two);
  }
  
  delete actorsGraph;
  infile.close();
  ofs->close();
  delete ofs;
  return 0;
}
