//Names:      Jared Shahbazian, Benjamin Nagengast
//Assignment: PA4
//File:       ActorGraph.cpp

#ifndef ACTORGRAPH_HPP
#define ACTORGRAPH_HPP

#include <iostream>
#include <unordered_map>
#include "ActorNode.hpp"
#include "Movie.hpp"

/** Class: ActorGraph
 *  The data structure for the entire actors and edges
 *  Actors have nodes and edges "connect" them
 */
class ActorGraph {
  
public:
  std::unordered_map<std::string,ActorNode *> actors;
  std::unordered_map<std::string,Movie *> movies;
  
  ActorGraph(void);
  
  virtual ~ActorGraph(){
    /* Delete edges from within actors */
    /* Delete the actors */
    /* Delete The movies */
    for(auto itr = actors.begin();itr != actors.end(); ++itr ){
      delete itr->second;
    }
    for(auto itr = movies.begin();itr != movies.end(); ++itr){
      delete itr->second;
    }
  }
  /** Medthod: loadFromFile
   *  Take a file of actors and movies and create ActorNodes
   *  Make sure every Movie has each of its actors
   */
  bool loadFromFile(const char* in_filename, bool use_weighted_edges);

  /** Medthod: assignEdges
   *  For every movie, link each actor the all of the others
   */
   void assignEdges(Movie * movie, bool weighted);
};
#endif // ACTORGRAPH_HPP
