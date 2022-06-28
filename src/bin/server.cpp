#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <string>
#include <vector>
#include "hashmap.h"

#define PACK109_HPP

typedef unsigned char u8;
typedef unsigned int u32;
typedef unsigned long u64;
typedef signed char i8;
typedef signed int i32;
typedef signed long i64;
typedef float f32;
typedef double f64;
typedef std::vector<u8> vec;
typedef std::string string;

using std::begin, std::end;
#define X8  256
#define X16 65536

#define PACK109_TRUE  0xa0
#define PACK109_FALSE 0xa1
#define PACK109_U8    0xa2
#define PACK109_U32   0xa3
#define PACK109_U64   0xa4
#define PACK109_I8    0xa5
#define PACK109_I32   0xa6
#define PACK109_I64   0xa7
#define PACK109_F32   0xa8
#define PACK109_F64   0xa9
#define PACK109_S8    0xaa
#define PACK109_S16   0xab
#define PACK109_A8    0xac
#define PACK109_A16   0xad
#define PACK109_M8    0xae
#define PACK109_M16   0xaf



// file struct 
struct File{ 
    std::string name; // The name of the file, excluding the path. e.g. just document.txt
    std::vector<unsigned char> bytes; // The byte contents of the file
};

// request struct 
struct Request{ 
  std::string name; // The name of the requested file, e.g. document.txt
};

// slices vector into subvectors 
vec slice(vec& bytes, int vbegin, int vend){ 
  auto start = bytes.begin() + vbegin; // iterators
  auto end = bytes.begin() + vend + 1;
  vec result(vend - vbegin + 1); // creates a variable result of type vector w this size
  copy(start, end, result.begin()); // copies those bytes as input & puts it in result
  return result;
}

// serialises u8 
vec serialize(u8 item){ 
  vec bytes;
  bytes.push_back(PACK109_U8);
  bytes.push_back(item);
  return bytes;
}

// serialises string 
vec serialize(string item) { 
  vec bytes;
  if (item.size() < 256) {
    bytes.push_back(PACK109_S8);
    bytes.push_back((u8)item.size());
    // Push each byte of the string onto the vector
    for(int i = 0; i < item.size(); i++) {
      bytes.push_back(item[i]);
    }
  } else if (item.size() < 65536) {
    bytes.push_back(PACK109_S16);
    u32 string_length = (u32)item.size();
    // Push the first byte of the length onto the vector
    bytes.push_back((u8)(string_length >> 8));
    // Push the second byte of the length onto the vector
    bytes.push_back((u8)(string_length));
    // Push each byte of the string onto the vector
    for (int i = 0; i < item.size(); i++) {
      bytes.push_back((u8)item[i]);
    }
  } else {
    throw;
  }
  return bytes;
}

// deserialises string 
string deserialize_string(vec bytes) { 
  if(bytes.size() < 3) {
    throw;
  }
  string deserialized_string = "";
  if(bytes[0] == PACK109_S8) {
    // The string length is byte 1
    int string_length = bytes[1];
    // The string starts at byte 2
    for(int i = 2; i < (string_length + 2); i++) {
      deserialized_string += bytes[i];
    }
  }
  else if(bytes[0]==PACK109_S16) {
    // Reconstruct the string length from bytes 1 and 2
    int string_length = (bytes[1]<<8) | bytes[2];
    // The string starts at byte 3
    for(int i = 3; i < (string_length + 3); i++) {
      deserialized_string += bytes[i];
    }
  }
  return deserialized_string;
}


// deserializes u8 
u8 deserialize_u8(vec bytes) {
  // To deserialize the u8, we need at least two bytes in the serialized
  // vector: one for the tag and one for the byte.
  if (bytes.size() < 2) {
    throw;
  }
  // Check for the correct tag
  if (bytes[0] == PACK109_U8) {
    // Directly return the byte. We can do this because the return type of the
    // function is the same as the contents of the vector: u8.
    return bytes[1];
  } else {
    // Throw if the tag is not a u8
    throw;
  }
}

// deserialises vector<u8> 
std::vector<u8> deserialize(vec bytes) { 
  int el_size = 2;
  std::vector<u8> result;
  if(bytes[0] == PACK109_A8) {
    int size = el_size * bytes[1];
    for (int i = 2; i < (size + 2); i += el_size) {
      vec sub_vec = slice(bytes, i, i + el_size);
      u64 element = deserialize_u8(sub_vec);
      result.push_back(element);
    }
  } else if(bytes[0] == PACK109_A16) {
    int size = el_size * (((int)bytes[1])<<8 | (int)bytes[2]);
    for(int i = 2; i < (size + 2); i += el_size) {
      vec sub_vec = slice(bytes, i + 1, i + el_size);
      u64 element = deserialize_u8(sub_vec);
      result.push_back(element);
    }
  }
  return result;
}

// deserializes file struct 
struct File deserialize_file(vec bytes){ 
  vec file_slice = slice(bytes, 2, 7);
  string file_string = deserialize_string(file_slice);
  if (file_string != "File") {
    throw;
  }
  u8 fileName_length = bytes[17]; // length of name of file depends on the string filename
  vec namev = slice(bytes, 16, 16 + fileName_length + 1);
  string name = deserialize_string(namev);
  int startingPoint = 16 + fileName_length + 7 + 2; // array starts here 
  vec arraybytesv = slice(bytes,startingPoint, bytes.size());
  vec arraybytes = deserialize(arraybytesv); 
  struct File deserialized_file = {name, arraybytes};
  return deserialized_file;
}

// deserializes request struct 
struct Request deserialize_request(vec bytes){ 
  vec request_slice = slice(bytes, 2, 10);

  string request_string = deserialize_string(request_slice);
  if (request_string != "Request") {
    throw;
  }

  u8 fileName_length = bytes[20]; // length of name of file depends on the string filename
  vec namev = slice(bytes, 19, 19 + fileName_length + 1);
  string name = deserialize_string(namev);
 
  struct Request deserialized_request = {name};
  return deserialized_request;
}

// serialises a vector of unsigned chars 
vec serialize(std::vector<unsigned char> item){ 
    vec bytes;
    if(item.size() < X8){
        // pushes tag 
        bytes.push_back(PACK109_A8);
        u8 size = (u8)item.size();
        // pushes size 
        bytes.push_back(size);
        for (int i = 0; i < item.size(); i++) {
          // serialises each item by calling serialize()
          vec temp = serialize(item[i]);
          for (int j = 0; j < temp.size(); j++){
            // push each byte onto the vector
            bytes.push_back(temp[j]);
          }
        }
    } else if(item.size() < X16){
      // pushes tag 
      bytes.push_back(PACK109_A16);
      // pushes size 
      u32 item_length = (u32)item.size();
      bytes.push_back((u8)(item_length >> 8));
      bytes.push_back((u8)(item_length));
      for (int i = 0; i < item.size(); i++) {
        // serialises each item by calling serialize()
        vec elem = serialize(item[i]);
        bytes.insert(end(bytes), begin(elem), end(elem));
      }
    } else {
      throw;
    }
    return bytes; 
} 

// serialises file struct 
vec serialize(struct File file){ 
    vec bytes;
    bytes.push_back(PACK109_M8);
    bytes.push_back(0x01);
    // the key is "File"
    vec fileKey = serialize("File");
    bytes.insert(end(bytes), begin(fileKey), end(fileKey));
    // The value is an m8
    bytes.push_back(PACK109_M8);
    bytes.push_back(0x02); // 2 k/v pairs
    // k/v 1 is name 
    vec namek = serialize("name");
    bytes.insert(end(bytes), begin(namek), end(namek));
    vec namev = serialize(file.name);
    bytes.insert(end(bytes), begin(namev), end(namev));
    // k/v 2 is "bytes"
    vec bytesk = serialize("bytes");
    bytes.insert(end(bytes), begin(bytesk), end(bytesk));
    vec bytesv = serialize(file.bytes); // serialised file bytes 
    bytes.insert(end(bytes), begin(bytesv), end(bytesv));
    return bytes;
}


int main(int argc, char *argv[]) {

   int sockfd, newsockfd, portno, clilen; 
   char buffer[256];
   struct sockaddr_in serv_addr, cli_addr;
   int n;

   bool hostnameFlag = false;
   
   // checks if hostname flag has been passed 
   for(int i = 0; i < argc; i++){   
       if(strcmp(argv[1], "--hostname") == 0){
           hostnameFlag = true;
       }
   }

   // if hostname isn't passed, exits code 
   if(hostnameFlag == false){ 
       exit(0);
   }
    
    // parses address & port number 
    std:: string addressPort = std::string(argv[2]); 
    std:: string delimitter = ":";
    std::string address = addressPort.substr(0, addressPort.find(delimitter));
    std::string portnum = addressPort.erase(0, addressPort.find(delimitter) + delimitter.length()); 

    char portno_array[portnum.length()]; 
    strcpy(portno_array, portnum.c_str()); // converts string portnum to char* array portno_array
    portno = atoi(portno_array); // converts char portno_array to int portno

    // binds socket to port 
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
   
    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }
   
    bzero((char *) &serv_addr, sizeof(serv_addr)); 
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) { 
        perror("ERROR on binding");
        exit(1);
    }

    HashMap hashmap;
    //listens for connection 
    listen(sockfd,5); 
    clilen = sizeof(cli_addr);

    // accepts connection 
    newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, (unsigned int *)&clilen); 
	
    if (newsockfd < 0) {
        perror("ERROR on accept");
        exit(1);
    } 
  
    // enters infinite loop
    while(1){
        bzero(buffer,256); 
        // reads message to a buffer 
        n = read(newsockfd,buffer,255); 

        if (n < 0) {
            perror("ERROR reading from socket");
            exit(1);
        }

        vec response;

        // decrypts message and pushes it to vector response 
        for(int i = 0; i < sizeof(buffer) - 1; i++){ 
            response.push_back(buffer[i] ^ 42);
        }
        
        bool request = false;
        bool file = false;

        // checks if message response is sending a file or requesting a file 
        if(response[4] == 'R'){ // if the element at the 4th index of response vector is R, that means its requesting a file 
            request = true;
        } else{
            file = true; // sending file
        }

        struct File deserializedFile;
        struct Request deserializedRequest;

        if(file == true){ // message is a file which is being sent 
            deserializedFile = deserialize_file(response); // deserializes file message vector sent 
            hashmap.insert(deserializedFile.name, deserializedFile.bytes); // inserts the deserialised file into the hashmap by using the filename as key and file's bytes as value 
             std::cout << "Bytes Reading: " << std::endl;
             std::cout << deserializedFile.bytes.size() << std::endl;
             std::cout << "Received File:" << std::endl;
             std::cout << deserializedFile.name << std::endl;
             std::cout << "File stored." << std::endl;
             for(int i = 0 ; i < deserializedFile.bytes.size(); i++){
               std::cout << deserializedFile.bytes[i] << std::endl;
             }
        } else if(request == true){ // message is requesting a file 
            deserializedRequest = deserialize_request(response); // deserializes the request 
            vec returnedBytes = hashmap.get(deserializedRequest.name); // looks for the requested file in hash map and returns its file bytes
            std::cout << "name of deserialized request" << std::endl;
            std::cout << deserializedRequest.name << std::endl;
            std::cout << "returnedBytes.size()" << std::endl;
            std::cout << returnedBytes.size() << std::endl;
            
            if(returnedBytes.size() > 0){ // requested file exists 
                // serialises the returned bytes from the hashmap into a File struct 
                struct File message;
                message.name = deserializedRequest.name;
                message.bytes = returnedBytes;
                vec fileMessage = serialize(message); // serialises the file message 
                vec encryptedMessage;
                // encrypts the file message 
                for(int i = 0; i < fileMessage.size(); i++){
                    encryptedMessage[i] = fileMessage[i] ^ 42;
                }
                // sends message to the client
                n = write(newsockfd,encryptedMessage.data(),encryptedMessage.size());
                // returns the number of bytes actually written to the file associated with newsockfd
                if (n < 0) {
                    perror("ERROR writing to socket");
                    exit(1);
                }
            }
        }
   }
   return 0;
}

