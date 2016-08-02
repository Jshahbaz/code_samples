//Names:      Jared Shahbazian, Benjamin Nagengast
//Assignment: PA4
//File:       ActorGraph.cpp

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include "Movie.hpp"
#include "ActorGraph.hpp"
#include <vector>

using namespace std;

ActorGraph::ActorGraph(void) {}

/** Medthod: loadFromFile
 *  Take a file of actors and movies and create ActorNodes
 *  Make sure every Movie has each of its actors
 */
bool ActorGraph::loadFromFile(const char* in_filename, bool use_weighted_edges){
  // Initialize the file stream
  ifstream infile(in_filename);

  bool have_header = false;

  // keep reading lines until the end of file is reached
  while (infile){
    string s;
    if (!getline( infile, s )) break;

    if (!have_header){
      have_header = true;
      continue;
    }

    istringstream ss( s );
    vector <string> record;
    while (ss){
      string next;
      // get the next string before hitting a tab character and put it in 'next'
      if (!getline( ss, next, '\t' )) break;

      record.push_back( next );
    }
    if (record.size() != 3)
    {
      // we should have exactly 3 columns
      continue;
    }

    /* Store the actors in an unordered map */

    string actor_name(record[0]);
    string movie_title(record[1]);
    int movie_year = stoi(record[2]);

    auto result = actors.find(actor_name);

    ActorNode * currentActor;
    if(result == actors.end()){
      //if there is not already an actor node then create one
      currentActor = new ActorNode(actor_name, numeric_limits<int>::max());
      actors.insert(make_pair(actor_name,currentActor));
    }
    else
      currentActor = result->second;

    Movie * currentMovie;
    string nameanddate = movie_title + to_string(movie_year);
    auto movieResult = movies.find(nameanddate);
    if(movieResult == movies.end()){
      currentMovie = new Movie(movie_title, movie_year, nameanddate);
      movies.insert(make_pair(nameanddate,currentMovie));
    }
    else
      currentMovie = movieResult->second;

    if(currentMovie->date < currentActor->minYear){
      currentActor->minYear = currentMovie->date;
    }
    currentMovie->actors->push_back(currentActor);
  }

  if (!infile.eof())
  {
    cerr << "Failed to read " << in_filename << "!\n";
    return false;
  }
  infile.close();

  return true;
}

/** Medthod: assignEdges
 *  For every movie, link each actor the all of the others
 */
void ActorGraph::assignEdges(Movie * movie, bool weighted) {
    ActorNode * currentActor;
    ActorNode * connectingActor;
    for(auto itr = movie->actors->begin();itr != movie->actors->end() ; ++itr ){
      currentActor = *itr;

      //start the iterator at the next actornode in the map
      auto iter = itr+1;
      for(; iter != movie->actors->end(); ++iter){
        connectingActor = *iter;
        auto checkMap = currentActor->connectedActors.find(connectingActor->name);

        int edgeWeight = 1;
        if(weighted){
          edgeWeight = 1+(2015-(movie->date));
        }
        //if the currentActor isn't connected
        if(checkMap == currentActor->connectedActors.end()){
          Edge * newEdge = new Edge(connectingActor,movie->name, movie->date, edgeWeight);
          Edge * reverseEdge = new Edge(currentActor,movie->name,movie->date, edgeWeight);
          string name = connectingActor->name;
          string name2 = currentActor->name;
          currentActor->connectedActors.insert(make_pair(name,newEdge));
          connectingActor->connectedActors.insert(make_pair(name2,reverseEdge));
        }
        if(checkMap != currentActor->connectedActors.end()){
          if(checkMap->second->weight > edgeWeight){
            Edge * currentToConnecting = currentActor->connectedActors.find(connectingActor->name)->second;
            Edge * connectingToCurrent = connectingActor->connectedActors.find(currentActor->name)->second;

            currentToConnecting->movieName = movie->name;
            currentToConnecting->weight=edgeWeight;
            currentToConnecting->date = movie->date;

            connectingToCurrent->movieName = movie->name;
            connectingToCurrent->weight = edgeWeight;
            connectingToCurrent->date = movie->date;
          }
        }
      }
  }
}
