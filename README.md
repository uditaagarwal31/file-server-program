# file-server-program
Developed a file server program that hosts files for clients. It receives files that clients want to store, decrypts them, and sends them back when requested. Created a client program that connects to this server, encrypts the files, sends files to it which the server stores. The client also requests files from the file server


# run the program

To build the Server program, in a terminal window, run `make`. Then, run `./build/bin/release/server --hostname 127.0.0.1:8080`. 
To build the Client program, in another terminal window, run `make`. Then, to send a file, run `./build/bin/release/client --hostname 127.0.0.1:8080 --send files/document.txt`. To receive a file, run `./build/bin/release/client --hostname 127.0.0.1:8080 --request document.txt`.
