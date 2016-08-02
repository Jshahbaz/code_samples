//Names:      Jared Shahbazian, Benjamin Nagengast
//Assignment: PA4
//File:       actorconnections.cpp

/** File Description ********************************************************
 *  This file runs a program that determines if a pair of actors/actresses
 *  can be "connected". That is, if they can be connected through movies that
 *  they have worked on to another actor/actress.
 *
 *  Command Line args:
 *  1: a tsv file containing actors and movies they have worked on
 *  2: a tsv with a header, and lines of pairs of actors
 *  3: an output file
 *  4: 'bfs' or 'ufind' which determines which find algorithm to use
 */

#include <iostream>
#include <algorithm>
#include <vector>
#include <fstream>
#include <queue>
#include <string>
#include <cstring>
#include <sstream>
#include <cstdint>
#include <chrono>
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

/** Method: find
 *  Performs a find on a Union-Find data structure
 *  Returns the node's root node
 */
ActorNode* find(ActorNode * actorToFind){
  ActorNode * actorToReturn = actorToFind;
  while(actorToReturn->unionPointer)
  {
      actorToReturn = actorToReturn->unionPointer->associatedActor;
  }
  return actorToReturn;
}

/** Method: merge
 *  Performs a merge on a Union-Find data structure
 *  Makes one of the roots point to the other
 */
void merge(ActorNode * actor1, ActorNode * actor2){
  ActorNode * firstSentinel = find(actor1);
  ActorNode * secondSentinel = find(actor2);
  if(firstSentinel == secondSentinel){
    return;
  }
  secondSentinel->unionPointer = new Edge(0,"",0,0);
  secondSentinel->unionPointer->associatedActor = firstSentinel;

  return;
}

/** Method: DjikstraSearch
 *  This method performs the BFS algorithm
 */
int DjikstraSearch(std::unordered_map<std::string,ActorNode *> actors,
   ActorNode * actor_one, ActorNode * actor_two){
  int found = 0;
  for(auto setNodesItr = actors.begin(); setNodesItr != actors.end(); setNodesItr++){
    //Set each actors dist = infinity
    (*setNodesItr).second->dist = numeric_limits<int>::max();
    //prev fields are -1
    (*setNodesItr).second->prev = -1;
    // done fields to false;
    (*setNodesItr).second->done = false;
  }

  priority_queue<pair<ActorNode*, int>,vector<pair<ActorNode *, int>>, compareDist> toExplore;
  ActorNode* start = actor_one;
  start->dist = 0;
  toExplore.push(make_pair(start,0));
  int currentDist = 0;
  bool done = false;

  // While PQ is not empty
  while(!toExplore.empty()){
    ActorNode* next = toExplore.top().first;
    toExplore.pop();
    if(next->done == false){
      next->done = true;
    }
    if(next->name == actor_two->name && next->done == true){
      found = 1;
      break;
    }
    
    //for each of V's neighbors
    for(auto itr = next->connectedActors.begin();
        itr != next->connectedActors.end();itr++){
      ActorNode * neighbor = (*itr).second->associatedActor;
      int c = 0;
      int edgeWeight = (*itr).second->weight;
      c = next->dist + edgeWeight;
      if(c < neighbor->dist){
        neighbor->dist = c;
        neighbor->prev = next->name;
        toExplore.push(make_pair(neighbor,c));
      }
    }
    if(done)
      break;
  }
  return found;
}


/** Main
 *  The driver firsts reads in input files and compiles vectors of the actors 
 *  and Movies. It then branches into two paths depending on if BFS or UfIND
 *  was chosen.
 */
int main(int argc, char* argv[]) {
  if(argc != 5){
    cout << "Wrong number of args! TERMINATING" << endl;
    exit(0);
  }
  
  ActorGraph * actorsGraph = new ActorGraph();
  actorsGraph->loadFromFile(argv[1], false);

  ofstream * ofs = new ofstream(argv[3]);
  ifstream infile(argv[2]);
  bool have_header = false;
  *ofs << "Actor1" << '\t' << "Actor2" << '\t' << "Year" << endl;

  //while you can, Read in a pair from argv[3]
  while (infile){
    string s;
    if (!getline( infile, s )) break;

    if (!have_header){
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

    auto result = actorsGraph->actors.find(firstActorsName);
    if(result == actorsGraph->actors.end()){
      cout << "Actor " << firstActorsName << " is not in the graph." << endl;
      exit(1);
    }
    else
      actor_one = result->second;

    result = actorsGraph->actors.find(secondActorsName);
    if(result == actorsGraph->actors.end()){
      cout << "Actor " << secondActorsName << " is not in the graph." << endl;
      exit(1);
    }
    else
      actor_two = result->second;

    int minYear = max(actor_one->minYear,actor_two->minYear);
    int currentYear = minYear;

    //BFS
    if(!strcmp("bfs",argv[4])){
      for(auto it = actorsGraph->movies.begin(); it != actorsGraph->movies.end(); ++it){
        if(it->second->date <= minYear){
          actorsGraph->assignEdges(it->second,false);
        }
      }

      while(!DjikstraSearch(actorsGraph->actors, actor_one,actor_two)){
        currentYear++;

        for(auto it = actorsGraph->movies.begin(); it != actorsGraph->movies.end(); ++it){
          if(it->second->date == currentYear){
            actorsGraph->assignEdges(it->second, false);
          }
        }
        if(currentYear > 2016){
          break;
        }
      }
    }
    //UFind
    if(!strcmp("ufind",argv[4])){
      for(auto mv = actorsGraph->movies.begin(); mv != actorsGraph->movies.end();++mv){
        if(mv->second->date <= minYear){
          for(auto actr = mv->second->actors->begin();
              actr != mv->second->actors->end()-1; ++actr){
            merge(*actr,*(actr+1));
          }
        }
      }

      bool result = (find(actor_one) == find(actor_two));

      //if find didnt work, add each year by year
      while(!result){
        currentYear++;
        if(currentYear > 2016){
          break;
        }

        for(auto mv = actorsGraph->movies.begin(); mv != actorsGraph->movies.end(); ++mv){
          if(mv->second->date == currentYear){
            for(auto actr = mv->second->actors->begin();
                actr != mv->second->actors->end()-1; actr++){
              merge(*actr,*(actr+1));
            }
          }
        }

        result = (find(actor_one) == find(actor_two));
      }
    }

    *ofs << actor_one->name << '\t' << actor_two->name << '\t' <<
      currentYear << endl;

    // Reset everything we just set because new query needs fresh start
    for(auto iter = actorsGraph->actors.begin(); iter != actorsGraph->actors.end();iter++ )
    {
      iter->second->connectedActors.erase(iter->second->connectedActors.begin(),
        iter->second->connectedActors.end());
      
      delete iter->second->unionPointer;
      iter->second->unionPointer = 0;
    }
  }

  delete actorsGraph;
  infile.close();
  ofs->close();
  delete ofs;
  return 0;
}
