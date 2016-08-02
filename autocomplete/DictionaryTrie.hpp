//Name:       Jared Shahbazian
//Assignment: PA3
//File:       DictionaryTrie.hpp


#ifndef DICTIONARY_TRIE_HPP
#define DICTIONARY_TRIE_HPP

#include <vector>
#include <string>
#include <queue>
#include <iostream>

using namespace std;

/** The class for nodes that will be used in a TST
 *  Each node contains a vector subWords which contains nodes which are the 
 *  end of words of which this node is a prefix of.
 *  Each node also has a character and a frequency
 */
class TrieNode {
public:
  TrieNode* left;
  TrieNode* right;
  TrieNode* mid;
  TrieNode* parent;
  std::vector<TrieNode*> subWords;
  bool endOfWord;
  char letter;
  int freq;
   
  /* Constructor */
  TrieNode(char const c, int freq);
   
  /* Function that determines if a word exists in our trie 
   * Arguments:
   *   word: the word we wish to find
   * Returns:
   *   True if it exists, false if it doesnt
   */
  bool find(std::string word);
   
  /* Function that returns the last node of a word (the prefix word)
   * Arguments:
   *   word: the word we wish to find the last letter of
   * Returns:
   *   A ptr to the node of the last letter in the prefix
   */
  TrieNode* returnPre(std::string word);
   
  /* Overload function for less than compare
   * Arguments:
   *   other: the rhs of the operator
   * Returns:
   *   True if LHS's freq is less than the RHS's, false if not
   */
  bool operator<(const TrieNode& other);
};
 
 
/** A 'function class' for use as the Compare class in a
 *  priority_queue<TrieNode*>.
 *  For this to work, operator< must be defined to
 *  do the right thing on TrieNodes.
 */
class TrieNodePtrComp {
public:
   bool operator()(TrieNode*& lhs, TrieNode*& rhs) {
    return *lhs<*rhs;
   }
};


/**
 *  The class for a dictionary ADT, implemented as a trie
 *  You may implement this class as either a mulit-way trie
 *  or a ternary search trie, but you must use one or the other.
 *
 */
class DictionaryTrie
{
public:
  /* Create a new Dictionary that uses a Trie back end */
  DictionaryTrie();

  /* Insert a word with its frequency into the dictionary.
   * Return true if the word was inserted, and false if it
   * was not (i.e. it was already in the dictionary or it was
   * invalid (empty string) 
   */
  bool insert(std::string word, unsigned int freq);

  /* Function that determines if a word exists in our trie 
   * Arguments:
   *   word: the word we wish to find
   * Returns:
   *   True if it exists, false if it doesnt or we have an empty trie
   */
  bool find(std::string word) const;

  /* Return up to num_completions of the most frequent completions
   * of the prefix, such that the completions are words in the dictionary.
   * These completions should be listed from most frequent to least.
   * If there are fewer than num_completions legal completions, this
   * function returns a vector with as many completions as possible.
   * If no completions exist, then the function returns a vector of size 0.
   * The prefix itself might be included in the returned words if the prefix
   * is a word (and is among the num_completions most frequent completions
   * of the prefix)
   */
  std::vector<std::string> predictCompletions(std::string prefix, 
        unsigned int num_completions);

  /* writeString: Used by predictCompletions to turn a Node ptr into its string 
   * Arguments:
   *   prefix: the string representation of the prefix 
   *   preNode: the node ptr of the last char of the prefix
   *   node: the node of the last char of the word we are concerned with
   * Returns:
   *   The complete string of the word we wanted
   */
  std::string writeString(std::string prefix, TrieNode* preNode, TrieNode* node);

  /* Destructor */
  ~DictionaryTrie();

private:
  TrieNode* root;

  /* Recursive helper to insert 
   * Arguments:
   *   endWord: a ptr to the node which we wish to make the end of the word
   *   node: the node we are currently working on
   *   word: the word we wish to insert
   *   freq: the word we want to insert's frequency
   *   idx: an index used to keep place in string while iterating through it
   * Returns:
   *   The TrieNode we inserted (recursive)
   * */
  TrieNode* insertRec(TrieNode* endWord, TrieNode* node,std::string word,unsigned int freq,int idx);

  /* Recursive helper to destructor */
  static void deleteAll(TrieNode* n);

};




#endif // DICTIONARY_TRIE_HPP
