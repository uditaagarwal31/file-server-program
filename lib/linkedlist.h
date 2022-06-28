#include <stdio.h>
#include <stdbool.h>
#include "llnode.h"
#include <stdlib.h>
#include <vector>
#include <iostream>


typedef unsigned char u8;
typedef std::vector<u8> vec;

namespace linkedlist{

class LinkedList{
  private:
    linkedlist::Node* head;
    linkedlist::Node* tail;
  public:
    size_t length;
    LinkedList();
    ~LinkedList();
    size_t insertAtTail(std::string key, vec item);
    size_t insertAtHead(std::string key, vec item);
    size_t insertAtIndex(size_t index, std::string key, vec item);
    vec removeTail();
    void printList();
    vec removeHead();
    vec removeAtIndex(size_t index);
    bool containsItem(vec item);
    vec itemAtIndex(int index);
    vec remove(vec item);
    vec containsVal(std::string key);
    bool replaceValue(std::string key, vec value);
    std::string keyAtIndex(int index);
};


    // Initialises an empty list
    LinkedList::LinkedList(){
        head = new Node();
        tail = new Node();
        this->head=NULL;
        this->tail=NULL;
        this->length=0;
    }

    // frees space allocated for the linkedlist 
    LinkedList::~LinkedList(){ 
        Node* temp = this->head;
        while(temp!=NULL){
            Node* t = temp;
            temp = temp->next;
            free(t);
        }
    }

    // inserts new key value pair at the end of list.
    size_t LinkedList::insertAtTail(std::string key, vec item){ 
        Node* node = new Node(key, item); // creates a new node
        if (node == NULL){ // if the newly created node is null, method fails and returns 1
            return 1;
        }
        //if list is empty.
        if(this->head == NULL){
            this->head = node;
            this->tail = node;
        } else{ // the tail of the list points to the newly created node
            this->tail->next = node;
            this->tail 	= this->tail->next; 
        }
        this->length++;	
        return 0;
    }

    // returns true if list contains that item, false if it doesn't
    bool LinkedList::containsItem(vec item){ 
        Node* current = this->head;
        for(int i = 0; i < this->length; i++){
            if(current->value==item){
                return true;
            }
            current = current->next;
        }
        return false;
    }

    // inserts new key value pair at beginning of the list.
    size_t LinkedList::insertAtHead(std::string key, vec item){ 
        Node* node = new Node(key, item);
        if (node == NULL){
            return 1;
        }
        //if list is empty.
        if(this->head == NULL){
            this->head = node;
            this->tail = node;
        } else{
            node->next 	= this->head; 
            this->head 	= node; // the head of the list points to the newly created node
        }
        this->length++;	
        return 0;
    }

    // add new key value pair at index in the list
    size_t LinkedList::insertAtIndex(size_t index, std::string key, vec item){ 
        if(index == 0){ // list is empty 
            return insertAtHead(key, item); 
        }
        Node* current = this->head;
        Node* items = new Node(key, item);
        if(items == NULL){
            return 1;
        }
        for(int i = 0; i < index-1; i++){
            current = current->next;
        }
        items->next = current->next;
        current->next = items;
        return 0;
    }

    // removes tail from the list
    vec LinkedList::removeTail(){ 
        Node* temp;
        int i = 0;
        vec item;
        if(this->tail == NULL){	
            // List is Empty	
            return item;
        } else{
            temp = this->head;

            // Iterate to the end of the list
            while(temp->next != this->tail){ 
                temp = temp->next;
            }

            item = this->tail->value;

            Node* old_tail = this->tail;
            this->tail = temp;
            this->tail->next = NULL;	
            free(old_tail);	
        }
        this->length--;	
        return item;
    }

    // Removes Node from start of list and returns the value it contains.
    vec LinkedList::removeHead(){ 
        vec item;
        if(this->head == NULL){	
            // List is Empty	
            return item;
        } 
        else{
            item = this->head->value;
            Node* old_head = this->head;
            this->head = this->head->next;	
            free(old_head);	
        }
        this->length--;	
        return item;
    }

    // removes value passed in the function 
    vec LinkedList::remove(vec item){ 
        int index = -1;
        vec temp;
        Node* current = this->head;
        for(int i = 0; i < this->length; i++){
            index++;
            if(current->value == item){ // if the value at that index is the same as value passed, then calls removeAtIndex(index) function to remove item and returns true
                temp = this->removeAtIndex(index);
                return temp;
            }
            current = current->next;
            if(current==NULL){
                break;
            }
        }
        return temp;
    }

    // Removes item at the specified index 
    vec LinkedList::removeAtIndex(size_t index){ 
        vec returnVal;
        if(index < 0 || index >= this->length){
            return returnVal;
        }
        if(index == 0){ // first index 
            return removeHead();
        }
        if(index == length - 1){ // last index 
            return removeTail();
        }
        Node* cN = this->head;
        size_t limit = this->length;
        for (size_t i = 0; i < limit; i++){
            if (i + 1 == index){
                returnVal = cN->next->value;
                free(cN->next);
                cN->next = cN->next->next;
                this->length--;
                return returnVal;
            }
        }
        return returnVal;
    }

    // returns the value at the index passed 
    vec LinkedList::itemAtIndex(int index){ 
        Node* current = this->head;
        for(int i = 0;i < index; i++){
            if(current->next != NULL){
                current = current->next;
            }
            else{
                exit(1);
            }
        }
        return current->value;
    }

    // returns the key at the index passed 
    std::string LinkedList::keyAtIndex(int index){ 
        Node* current = this->head;
        for(int i = 0; i < index; i++){
            if(current->next!=NULL){
                current = current->next;
            }
            else{
                exit(1);
            }
        }
        return current->key;
    }

    
    // prints linked list
    void LinkedList::printList(){ 
        size_t limit = this->length;
        if (!this->length){
            printf("Empty");
        }
        Node* cP = this->head;
        for (size_t i = 0; i < limit; i++){
            for(int i = 0; i < cP->value.size(); i++){
                printf("%d -> ", cP->value[i]);
            }
            cP = cP->next;
        }
    }

    // returns value at a particular key 
    vec LinkedList::containsVal(std::string key){ 
        vec returnVal;
        Node* current = this->head;
        while(current != NULL){
            if(current->key == key){ // if the key at current node is same as the key passed 
                returnVal = current->value;
                return returnVal; // returns value at that key 
            }
            current = current->next;
        }
        return returnVal;
    }

    // replaces value at a particular key with another value 
    bool LinkedList::replaceValue(std::string key, vec value){ 
        vec returnVal;
        Node* current = this->head;
        while(current != NULL){
            if(current->key == key){ // if the key at current node is same as the key passed
                current->value = value; // replaces value at that node with the passed value 
                return true;
            }
            current = current->next;
        }
        return false; 
    }
    
}

