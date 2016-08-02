//Names:      Jared Shahbazian, Benjamin Nagengast
//Assignment: PA4
//File:       extension.cpp

/** File Description ************************************************
 *  This file runs a program for any actor/actress who wishes to meet 
 *  another actor. It is kind of a matchmaker. It is based on the fact
 *  that actors meet other actors when they work on a movie together.
 *  
 *  Command Line args:
 *  1: a tsv file containing actors and movies they have worked on
 *  2: a tsv with a header, and a single pair of actors, the first 
 *     one is "you" and the second is who you wish to meet
 *  3: the current year. if it is 1995, you can only meet actors who have
 *     already been in movies
 */

#include <iostream>
#include <algorithm>
#include <vector>
#include <fstream>
#include <queue>
#include <string>
#include <sstream>
#include <cstdint> 
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
int DjikstraSearch(std::unordered_map<std::string,ActorNode *> actors,
   ActorNode * actor_one,
   ActorNode * actor_two)
{
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
    //Dequeue node v from front of queue
    ActorNode* next = toExplore.top().first;
    toExplore.pop();
    //If V is not done
    if(next->done == false)
    {
      //set v done = true
      next->done = true;
    }
    //If the destination node has been visited, then the algorithm is done.
    if(next->name == actor_two->name && next->done == true)
    {
      found = 1;
      break;
    }
    //for each of V's neighbors
    for(auto itr = next->connectedActors.begin();
        itr != next->connectedActors.end();itr++)
    {
      ActorNode * neighbor = (*itr).second->associatedActor;
      int c = 0;
      //distance to neighbor through next,
      // C = next.dist + ( edgeWeight between the two )
      int edgeWeight = (*itr).second->weight;
      c = next->dist + edgeWeight;
      //if c is less than w.dist
      if(c < neighbor->dist)
      {
        //neighbor.dist = c
        neighbor->dist = c;
        neighbor->prev = next->name;
        toExplore.push(make_pair(neighbor,c));
      }
    }
    //just in case break doesnt break from while
    if(done)
      break;
  }
  return found;
}


int main(int argc, char* argv[]) {
  if(argc != 4)
  {
    cout << "Wrong number of args! TERMINATING" << endl;
    exit(0);
  }
  ActorGraph * actorsGraph = new ActorGraph();

  actorsGraph->loadFromFile(argv[1], 0);
  
  int year = stoi(argv[3]);

  //assign the edges for each node in the graph
  for(auto it = actorsGraph->movies.begin(); 
      it != actorsGraph->movies.end(); ++it){
    if(it->second->date <= year)
      actorsGraph->assignEdges(it->second, 0);
  }
  
  ifstream infile(argv[2]);
  bool have_header = false;

  string s;
  getline( infile, s );
  getline( infile, s );
  

  istringstream ss( s );
  vector <string> actorPair;

  while (ss) {
    string next;
    if (!getline( ss, next, '\t' ))
      break;
    actorPair.push_back( next );
  }


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
    
  //start
  cout<<"Let's see if we can set you up."<<endl;
  
  // check if youre connected to actor_two, if not no chance
  
  int dsearch = DjikstraSearch(actorsGraph->actors,actor_one,actor_two);
  
  if(dsearch == 0)
  {
    cout<< "Sorry, doesnt look like you have a chance :["<<endl;
    cout<< "Nobody that "<<actor_two->name<<" knows knows anybody that you know."<<endl;
    cout<< "Maybe if you work some more movies, youll meet somebody that can "<<
      "introduce you."<<endl;
    return 0;
  }
  
  ActorNode* lastVisited = actor_one;
  char answer = 'n';
  auto itr = actor_one->connectedActors.begin();
  //start the questionnaire
  while(actor_one != actor_two){
    if(actor_one->connectedActors.find(actor_two->name) != actor_one->connectedActors.end()){
      if(actor_one->name == firstActorsName){
        cout<<"You know them already, dont you remember?! Grow a backbone and ask them!";
        return 0;
      }
      else{
        cout<<"OK, hopefully "<< actor_one->name<<
          " will set you up, they know "<<actor_two->name<<" personally."<<endl;
        cout<<"My job here is done."<<endl;
        return 0;
      }
    }
    for(itr = actor_one->connectedActors.begin();
        itr != actor_one->connectedActors.end();itr++)
      {
        if(DjikstraSearch(actorsGraph->actors,(*itr).second->associatedActor,actor_two) == 0)
          continue;
        
        if(actor_one->name == firstActorsName){
          cout<<"You can ask "<<(*itr).second->associatedActor->name<<endl;
          cout<<"Want to do that? (y/n): ";
        }
        else{
          cout<<"OK, now ask "<< actor_one->name
            <<" to ask "<<(*itr).second->associatedActor->name<<endl;
          cout<<"What did they say to that? (y/n): ";
        }
    
        cin>>answer;
    
        if(answer=='y'){
          break;
        }
      }
    lastVisited = actor_one;
    actor_one = (*itr).second->associatedActor;
  }
  
delete lastVisited;
delete actorsGraph;
infile.close();
return 0;
}
