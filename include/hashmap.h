#include "linkedlist.h"
#include <functional>
#include <string>
#include <stdexcept>
#include <iostream>

typedef unsigned char u8;
typedef std::vector<u8> vec;

class HashMap{ 
  private:
    // The backbone of the hash map. This is an array of Linked List pointers.
    linkedlist::LinkedList** array;
    // The number of buckets in the array
    size_t size; 
    // Generate a prehash for an item with a given size
    unsigned long prehash(std::string key);

  public:
    HashMap(size_t size);
    HashMap();
    ~HashMap();
    unsigned long hash(std::string key);
    bool remove(std::string key, vec value);
    bool contains(std::string key, vec item);
    void resize(size_t new_size);
    size_t len();
    size_t capacity();
    void print();
    bool insert(std::string key, vec value);
    vec get(std::string key);
};

// Generates a prehash for key with a given size
unsigned long HashMap::prehash(std::string key){ 
    int s = sizeof(key);
    int k = 0;
    unsigned long h = 5381;
    char* myptr = (char*)&key;
    while (k<s){
      char* c = (char*)(myptr+k); 
	    h = ((h << 5) + h) + *c;
      k++;
    }
    return h;
}


// Initialize an empty hashmap, where size is the number of buckets in the array
HashMap::HashMap(size_t size){  
  this->array = new linkedlist::LinkedList*[size];
  for(int i = 0; i < size; i++){
    this->array[i] = new linkedlist::LinkedList();
  }
  this->size=size;
}

// Initialize an empty hashmap, with 10 buckets in the array
HashMap::HashMap(){ 
  this->array = new linkedlist::LinkedList*[10];
  for(int i = 0; i < 10; i++){
    this->array[i] = new linkedlist::LinkedList();
  }
  this->size=10;
}

// Frees all memory allocated by the hashmap 
HashMap::~HashMap(){  
  for(int i = 0; i < this->size; i++){
    array[i]->~LinkedList(); 
  }
}

// Hash an unsigned long into an index that fits into a hashmap
unsigned long HashMap::hash(std::string key){  
  unsigned long pre = this->prehash(key); 
  unsigned long s = (unsigned long)(pre%this->size);
  return s;
}

// Inserts the k/v pair into the hash map. Returns true if the key already existed, and replaces the stored value with the supplied value. Returns false if the key did not exist already.
bool HashMap::insert(std::string key, vec value){  
  unsigned long index = this->hash(key); 

  if(this->array[index]->length>0){  // linkedlist at that index has some k/v pairs 
    linkedlist::LinkedList* temp = this->array[index];
    vec valReturned = temp->containsVal(key); // checks if the linkedlist contains value mapped to that key
    if(valReturned.size() == 0){ // key did not exist already because the returned vector has size 0
      temp->insertAtTail(key, value); // inserts new key value pair 
      return false;
    } else {
      temp->replaceValue(key, value); // replaces stored value with supplied value
      return true;
    }
  } else { // linked list at that index is empty 
    this->array[index]->insertAtHead(key,value); // add key value pair as the first element in linked list because the linkedlist is empty
    return false; // key did not exist already
  }
  return true;
}

// returns the file associated with the supplied key. If the key doesn't exist, this function throws an exception
vec HashMap::get(std::string key){  
  unsigned long index = this->hash(key); // index which contains the key
  vec valReturned;
  if(this->array[index]->length>0){ // linkedlist at that index has some k/v pairs  
    linkedlist::LinkedList* temp = this->array[index];
    valReturned = temp->containsVal(key); // checks if the linkedlist contains value mapped to that key
    if(valReturned.size() == 0){ // key did not exist already because the returned vector has size 0
      throw std::invalid_argument("key doesn't exist"); 
    } 
    return valReturned;
  }
  return valReturned;
}

// Removes an item from the hashmap. Returns true if it was removed, false if it wasn't (i.e. it wasn't in the hashmap to begin with)
bool HashMap::remove(std::string key, vec value){  
  if(this->contains(key, value)==false){ // key value pair wasn't in hashmap to begin with 
    return false;
  }
  unsigned long hashed = hash(key); // index 
  vec s = this->array[hashed]->remove(value); // removes item from the hashmap
  if(s.size()==0){ // key did not exist already
    return false;
  }
  return true; 
}

// Returns true if the value exists in the hashmap, false otherwise
bool HashMap::contains(std::string key, vec value){  
  unsigned long hashed = hash(key);
  bool res = this->array[hashed]->containsItem(value);
  return res;
}

// Resizes the underlying hashmap to the given size. Recalculates the load factor after resize
void HashMap::resize(size_t new_size){  
  linkedlist::LinkedList** old = this->array;
  vec element[this->len()]; // stores values 
  std:: string keys[this->len()]; // stores keys 
  size_t limit = this->size;
  size_t currentindex = 0;
  for(size_t i =0;i<limit;i++){
    if(this->array[i]->length>0){
      for(int j = 0;j<this->array[i]->length;j++){
        element[currentindex++] = this->array[i]->itemAtIndex(j);
        keys[currentindex++] = this->array[i]->keyAtIndex(j);
      }
    }
  }
  this->size = new_size;
  this->array = new linkedlist::LinkedList*[new_size];
  for(int i =0;i<new_size;i++){
    this->array[i] = new linkedlist::LinkedList();
  }
  for(int i =0;i<currentindex;i++){
    this->insert(keys[i],element[i]); 
  }
  for(int i = 0;i<limit;i++){
    old[i]->~LinkedList();
  }
}

// returns length of hashmap 
size_t HashMap::len(){ 
  size_t len = 0;
  for(int i =0;i<this->size;i++){
    len += this->array[i]->length;
  }
  return len;
}

// returns capacity of hashmap 
size_t HashMap::capacity(){  
  return (size_t)(0.7*(double)this->size) - 1;
}

// prints hashmap 
void HashMap::print(){  
  for(int i =0;i<this->size;i++){
    printf("List %d: ",i+1);
    this->array[i]->printList();
    printf("\n");
  }
}

