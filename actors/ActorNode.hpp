//Names:      Jared Shahbazian, Benjamin Nagengast
//Assignment: PA4
//File:       ActorNode.cpp

#ifndef ACTORNODE_HPP
#define ACTORNODE_HPP

#include <vector>
#include <iostream>
#include <unordered_map>
#include <algorithm>

using namespace std;

class ActorNode;

/** Class: Edge
 *  Each Actor has an edge to another Actor if they were in a movie with eachother
 *  The edge also has the name and date of the movie the actors share
 */
class Edge {
public:
  Edge(ActorNode * actor, string movie, int d, int w) : associatedActor(actor), movieName(movie), date(d), weight(w) { }
  string movieName;
  ActorNode * associatedActor;
  int date;
  int weight;
  /* Add weight after checkpoint */
};


/** Class: ActorNode
 *  Each ActorNode has a map containing every edge of that actor
 */
class ActorNode {
public:
  int i=0;
  string name;
  int minYear;
  unordered_map<std::string, Edge *> connectedActors;
  Edge* unionPointer;
  int dist;
  bool done;
  string prev;

  ActorNode(const string & n, int min) : name(n),minYear(min){
    prev = "";
    unionPointer = 0; 
  }

  virtual ~ActorNode(){
    for(auto itr = connectedActors.begin();itr != connectedActors.end(); itr++){
      delete itr->second;
    }
    delete unionPointer; 
  }

  bool operator==(const ActorNode &anotherActor) const{
    return (name == anotherActor.name);
  }
};

#endif
