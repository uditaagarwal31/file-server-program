#include <string>
#include <vector>

typedef unsigned char u8;
typedef std::vector<u8> vec;

namespace linkedlist{

    class Node { 
    public:
        vec value;
        std::string key;
        size_t length;
        linkedlist::Node* next;
        Node();
        Node(std::string key, vec value);
        ~Node();
    };

    // ititialises key to empty string and next node to null 
    Node::Node(){ 
        this->key = "";
        value;
        this->next = NULL;
    }

    // initialises key field at that node with the key parameter passed and value with value parameter passed 
    Node::Node(std::string key, vec value){ 
        this->key = key;
        this->value = value;
    }

    Node::~Node(){ 

    }
}

