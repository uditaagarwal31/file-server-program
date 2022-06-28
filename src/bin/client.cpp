#include<string>
#include<iostream>
#include <sstream>
#include <vector>
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <stdlib.h>
#include <vector>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <fstream>


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
struct File {
    std::string name; // The name of the file, excluding the path. e.g. just document.txt
    std::vector<unsigned char> bytes; // The byte contents of the file
};

// request struct 
struct Request {
  std::string name; // The name of the requested file, e.g. document.txt
};

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

// serialises u8 
vec serialize(u8 item) {
  vec bytes;
  bytes.push_back(PACK109_U8);
  bytes.push_back(item);
  return bytes;
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
          for (int j = 0; j < temp.size(); j++) {
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

vec slice(vec& bytes, int vbegin, int vend){
  auto start = bytes.begin() + vbegin; // iterators
  auto end = bytes.begin() + vend + 1;
  vec result(vend - vbegin + 1); // creates a variable result of type vector w this size
  copy(start, end, result.begin()); // copies those bytes as input & puts it in result
  return result;
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
  int startingPoint = 0;
  std::vector<u8> result;
  if(bytes[0] == PACK109_A8){
    startingPoint = 2;
    for(int i = 2; i < bytes.size(); i+=2){
      vec current = slice(bytes,startingPoint, startingPoint + 1);
      result.push_back(deserialize_u8(current));
    }
  } else if(bytes[0] == PACK109_A16){
    startingPoint = 3;
    for(int i = 3; i < bytes.size(); i+=2){
      vec current = slice(bytes,startingPoint, startingPoint + 1);
      result.push_back(deserialize_u8(current));
    }
  }
  for(int i = 2; i < bytes.size(); i+=2){
    vec current = slice(bytes,startingPoint, startingPoint + 1);
    result.push_back(deserialize_u8(current));
  }
  while(bytes[startingPoint] != NULL){
    vec current = slice(bytes,startingPoint, startingPoint + 1);
    result.push_back(deserialize_u8(current));
    startingPoint+=2;
  }

  return result;
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

// deserializes file struct 
struct File deserialize_file(vec bytes) {
  vec file_slice = slice(bytes, 2, 7);
  string file_string = deserialize_string(file_slice);
  if (file_string != "File") {
    throw;
  }
  u8 fileName_length = bytes[17]; // length of name of file depends on the string filename
  vec namev = slice(bytes, 16, 16+fileName_length+1);
  string name = deserialize_string(namev);
  int startingPoint = 16+fileName_length + 7+2; // index for value in map starts
  vec arraybytesv = slice(bytes,startingPoint, bytes.size());
  vec arraybytes = deserialize(arraybytesv); // BUG HERE 
  struct File deserialized_file = {name, arraybytes};
  return deserialized_file;
}

// encrypts message using the key 42
std::vector<u8> encrypt_message(std::vector<u8> message){
  for(int i = 0; i < message.size(); i++){
    message[i] = message[i] ^ 42;
  }
  return message;
}

// serializes request struct 
vec serialize(struct Request fileRequested){
    vec bytes;
    bytes.push_back(PACK109_M8);
    bytes.push_back(0x01);
    // the key is "Request"
    vec requestKey = serialize("Request");
    bytes.insert(end(bytes), begin(requestKey), end(requestKey));
    // The value is an m8
    bytes.push_back(PACK109_M8);
    bytes.push_back(0x01); // 1 k/v pairs
    // k/v 1 is name 
    vec namek = serialize("name");
    bytes.insert(end(bytes), begin(namek), end(namek));
    vec namev = serialize(fileRequested.name);
    bytes.insert(end(bytes), begin(namev), end(namev));
    return bytes;
}


int main(int argc, char *argv[]) { 
   int sockfd, portno, n;
   struct sockaddr_in serv_addr;
   struct hostent *server;
   vec encryptedFile;
   char buffer[256];
   
   if (argc < 3) {
      fprintf(stderr,"usage %s hostname port\n", argv[0]);
      exit(0);
   }

   bool sendFlag = false;
   bool requestFlag = false;
   bool hostnameFlag = false;
   bool sendRequestTogether = false;

    // checks which flag is passed and accordingly sets value of flag bools 
   for(int i = 0; i < argc; i++){   
       if(strcmp(argv[1], "--hostname") == 0){
           hostnameFlag = true;
       }
       if(strcmp(argv[3], "--send") == 0){
           sendFlag = true;
       }
       if(strcmp(argv[3], "--request") == 0){
           requestFlag = true;
       }
       if((strcmp(argv[4], "--request") == 0 || strcmp(argv[4], "--send") == 0) && (sendFlag == true || requestFlag == true)){
           sendRequestTogether = true;
       }
   }

   // if send and request flags are passed, exits code 
   if(sendRequestTogether == true){
       exit(0);
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

    std::string filepath;
    std::string filename;

    // if send flag is passed
    if(sendFlag == true){
      filepath = argv[4];
      filename = filepath.substr(filepath.find_last_of("/\\") + 1); // parses file name 
      char filename_array[filename.length()];
      strcpy(filename_array, filename.c_str()); // converts string filename to char* array filename_array

      // creates instance of File struct and gives the file name of the file struct the filename passed by user 
      struct File file;
      file.name = filename; 

      // reads file 
      std::ifstream myfile(filepath,std::ifstream::binary);
      myfile.seekg(0, std::ios::end);
      size_t myfilesize = myfile.tellg();
      myfile.seekg(0, std::ios::beg);
      vec fileto;
      for(int i = 0;i < myfilesize; i++){
        char temp;
        myfile.get(temp);
        file.bytes.push_back(temp);
      }
      
      vec bytesFile = serialize(file); // serialised bytes of file 

      encryptedFile = encrypt_message(bytesFile); // encrypts the file message 
     
      // Create a socket point 
      sockfd = socket(AF_INET, SOCK_STREAM, 0); 
      
      if (sockfd < 0) {
          perror("ERROR opening socket");
          exit(1);
      }

      char address_array[address.length()]; // converts string address to char* array address_array
      strcpy(address_array, address.c_str());

      server = gethostbyname(address_array); 
      
      if (server == NULL) {
          fprintf(stderr,"ERROR, no such host\n");
          exit(0);
      }
      
      bzero((char *) &serv_addr, sizeof(serv_addr));
      serv_addr.sin_family = AF_INET;
      bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
      serv_addr.sin_port = htons(portno);
      
      /* Now connect to the server */
      if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
          perror("ERROR connecting");
          exit(1);
      }
      
      // sends the encyrpted file to the server 
      n = write(sockfd, encryptedFile.data(), encryptedFile.size());
      
      if (n < 0) {
          perror("ERROR writing to socket");
          exit(1);
      }
     
      // Now read server response
      bzero(buffer,256);
      n = read(sockfd, buffer, 255);
   
      if (n < 0) {
        perror("ERROR reading from socket");
        exit(1);
      }
      printf("%s\n",buffer);
      
   }

    // if the request flag is passed 
   if(requestFlag == true){
     filename = argv[4];

     // reads it in a Request struct 
     struct Request fileRequested;
     fileRequested.name = filename;

     vec bytesRequest = serialize(fileRequested); // serialises request 

     vec encryptedRequest = encrypt_message(bytesRequest); // encrypts the request message 

     // establish socket connection
      sockfd = socket(AF_INET, SOCK_STREAM, 0); 
      
      if (sockfd < 0) {
          perror("ERROR opening socket");
          exit(1);
      }

      char address_array[address.length()]; // converts string address to char* array address_array
      strcpy(address_array, address.c_str());

      server = gethostbyname(address_array); 
      
      if (server == NULL) {
          fprintf(stderr,"ERROR, no such host\n");
          exit(0);
      }
      
      bzero((char *) &serv_addr, sizeof(serv_addr));
      serv_addr.sin_family = AF_INET;
      bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
      serv_addr.sin_port = htons(portno);
      
      /* Now connect to the server */
      if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
          perror("ERROR connecting");
          exit(1);
      }
      // sends encrypted request to server 
      n = write(sockfd, encryptedRequest.data(), encryptedRequest.size());
      
      if (n < 0) {
          perror("ERROR writing to socket");
          exit(1);
      }
     
      /* Now read server response */
      bzero(buffer,256);
      n = read(sockfd, buffer, 255);

      if (n < 0) {
        perror("ERROR reading from socket");
        exit(1);
      }

      // saves response vector in program memory
      char* response = buffer;
      vec serverResponse;

      // decrypts message and pushes it to vector serverResponse 
      for(int i = 0; i < sizeof(buffer) - 1; i++){ 
        serverResponse.push_back(buffer[i] ^ 42);
      }
      
      // deserialises the serverResponse and saves it to a File deserializedFile 
      File deserializedFile = deserialize_file(serverResponse); // deserialize file
          
      char filename_array[deserializedFile.name.length()]; // converts string filename to char* array filename_array
      strcpy(filename_array, deserializedFile.name.c_str());
      system("mkdir -p recieved");
      char path[1000] = "recieved/"; // path of received folder to save file into
      strcat(path, filename_array);
      
      // save in folder called recieved 
      std::ofstream outfile(path); 
      outfile.write((const char*)deserializedFile.bytes.data(), deserializedFile.bytes.size()); // writes it into the folder 
      outfile.flush();
      outfile.close();
   }
   return 0;
}






