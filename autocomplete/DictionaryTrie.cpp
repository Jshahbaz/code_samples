//Name:       Jared Shahbazian
//Assignment: PA3
//File:       DictionaryTrie.cpp

#include "DictionaryTrie.hpp"
#include "util.hpp"

/* Create a new Dictionary that uses a Trie back end */
DictionaryTrie::DictionaryTrie(){
  root = 0;
}

/* Insert a word with its frequency into the dictionary.
 * Return true if the word was inserted, and false if it
 * was not (i.e. it was already in the dictionary or it was
 * invalid (empty string) 
 */
bool DictionaryTrie::insert(std::string word, unsigned int freq){
  if(word == "" || find(word) == 1)
    return 0;
  
  TrieNode* lastNode = new TrieNode(word.at(word.size()-1),freq);
  lastNode->endOfWord=1;
  root = insertRec(lastNode, root,word,freq,0);

  return 1;
}

/* Recursive helper to insert 
 * Arguments:
 *   endWord: a ptr to the node which we wish to make the end of the word
 *   node: the node we are currently working on
 *   word: the word we wish to insert
 *   freq: the word we want to insert's frequency
 *   idx: an index used to keep place in string while iterating through it
 * Returns:
 *   The TrieNode we inserted (recursive)
 */
TrieNode* DictionaryTrie::insertRec(TrieNode* endWord, TrieNode* node, 
      std::string word, unsigned int freq, int idx){

  char letter = word.at(idx);
  if(!node){
    node = new TrieNode(letter, 0);
    if(idx!=0)
      node->subWords.push_back(endWord);
  }
  
  if(word.length()==1 && word.at(0) == node->letter)
    node->subWords.push_back(endWord);
    
    
  if(letter < node->letter){
    node->left = insertRec(endWord, node->left, word, freq, idx);
    node->left->parent = node;
  }
  else if(letter > node->letter){
    node->right = insertRec(endWord, node->right, word, freq, idx);
    node->right->parent = node;
  }
  else if(idx < (signed)word.length()-1){
    node->mid = insertRec(endWord, node->mid, word, freq, ++idx);
    node->mid->parent = node;
    if(idx != 0)
      node->subWords.push_back(endWord);
  }
  else{    
    endWord->left = node->left;
    if(endWord->left)
      endWord->left->parent = endWord;

    endWord->right = node->right;
    if(endWord->right)
      endWord->right->parent = endWord;

    endWord->mid = node->mid;
    if(endWord->mid)
      endWord->mid->parent = endWord;
    
    endWord->parent = node->parent;
    
    unsigned int i = 0;
    while(i < node->subWords.size()){
      endWord->subWords.push_back(node->subWords[i]);
      i++;
    }
    
    delete node;
    return endWord;
  }
  return node;
}

/* Function that determines if a word exists in our trie 
 * Arguments:
 *   word: the word we wish to find
 * Returns:
 *   True if it exists, false if it doesnt or we have an empty trie
 */
bool DictionaryTrie::find(std::string word) const{
  if(!root || word == "")
    return 0;

  return root->find(word);
}

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
std::vector<std::string> DictionaryTrie::predictCompletions(std::string prefix,
  unsigned int num_completions){
  
  std::vector<std::string> empty;
  if(num_completions <= 0 || prefix == ""){
    return empty;
  }

  if(TrieNode* pre = root->returnPre(prefix)){
    std::vector<std::string> words;
    unsigned int i = 0;
    

    std::priority_queue<TrieNode*,std::vector<TrieNode*>, 
        TrieNodePtrComp> sortedSubWords;

    //Add all subwords to a pq
    for(i=0;i<pre->subWords.size();i++)
    {
      sortedSubWords.push(pre->subWords[i]);
    }    

    i = 0;
    while(i < num_completions && sortedSubWords.size() != 0){      
      words.push_back(writeString(prefix,pre,sortedSubWords.top()));
      sortedSubWords.pop();
      i++;
    }
    return words;
  }
  return empty;
}

/* writeString: Used by predictCompletions to turn a Node ptr into its string 
 * Arguments:
 *   prefix: the string representation of the prefix 
 *   preNode: the node ptr of the last char of the prefix
 *   node: the node of the last char of the word we are concerned with
 * Returns:
 *   The complete string of the word we wanted
 */
std::string DictionaryTrie::writeString(std::string prefix,
  TrieNode* preNode, TrieNode* node){

  std::string suffix = "";
  if(preNode == node)
    return prefix;
  
  TrieNode* curr = node->parent;
  TrieNode* last = node;
  
  
  suffix = node->letter + suffix;  

  while(curr != preNode){
    if(!(last == curr->left || last == curr->right))
      suffix = curr->letter + suffix;

    last = curr;
    curr = curr->parent;
  }
  
  return prefix + suffix;
}

/* Destructor */
DictionaryTrie::~DictionaryTrie(){
  deleteAll(root);
}

/* Recursive helper to destructor */
void DictionaryTrie::deleteAll(TrieNode* n){
  if(!n)
    return;

  deleteAll(n->left);
  deleteAll(n->mid);
  deleteAll(n->right);

  delete n;
}

// TRIENODE METHODS

/* Constructor for node class */
TrieNode::TrieNode(char c, int freq){
  left = 0;
  right = 0;
  mid = 0;
  parent = 0;
  letter = c;
  endOfWord = 0;
  this->freq = freq;
}

/* Function that determines if a word exists in our trie 
 * Arguments:
 *   word: the word we wish to find
 * Returns:
 *   True if it exists, false if it doesnt
 */
bool TrieNode::find(std::string word){
 if(word.at(0)<letter){
   if(left)
    return left->find(word);
 }
 if(word.at(0)>letter){
   if(right)
     return right->find(word);
 }
 if(word.at(0)==letter){
   if(word.size()==1){
     if(!endOfWord)
       return 0;
     return 1;
   }
   else{
     word.erase(0,1);
     if(mid)
       return mid->find(word);
   }
 }
 return 0;
}


/* Function that returns the last node of a word (the prefix word)
 * Arguments:
 *   word: the word we wish to find the last letter of
 * Returns:
 *   A ptr to the node of the last letter in the prefix
 */
TrieNode* TrieNode::returnPre(std::string word){
 if(word.at(0)<letter){
   if(left)
    return left->returnPre(word);
 }
 if(word.at(0)>letter){
   if(right)
     return right->returnPre(word);
 }
 if(word.at(0)==letter){
   if(word.size()==1){
     return this;
   }
   else{
     word.erase(0,1);
     if(mid)
       return mid->returnPre(word);
   }
 }
 return 0;
}


/* Overload function for less than compare
 * Arguments:
 *   other: the rhs of the operator
 * Returns:
 *   True if LHS's freq is less than the RHS's, false if not
 */
bool TrieNode::operator<(const TrieNode& other)
{
  /* If the freq of this node is greater than the freq of the other
  node then return false */
  if(freq != other.freq)
  {
    return freq < other.freq;
  }
  return false;
}
