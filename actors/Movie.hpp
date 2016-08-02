//Names:      Jared Shahbazian, Benjamin Nagengast
//Assignment: PA4
//File:       Movie.cpp

#ifndef MOVIE_HPP
#define MOVIE_HPP

#include <vector>
#include "ActorNode.hpp"
using namespace std;

class Movie {
  
public:
  string name;
  int date;
  string nameanddate;
  vector<ActorNode *> * actors;

  Movie(const string & n, const int & d, 
      const string & nd) : name(n), date(d), nameanddate(nd){
    actors=new vector<ActorNode *>;
  }
  
  virtual ~Movie(){
    delete actors;
  }

  bool operator==(const Movie& otherMovie) const{
    return (name == otherMovie.name && date == otherMovie.date);
  }
};

#endif
