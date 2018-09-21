#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "minet_socket.h"

#define BUFSIZE      1024
#define FILENAMESIZE 100

int handle_connection(int);

int writenbytes(int, char *, int);
int readnbytes (int, char *, int);


int
main(int argc, char ** argv)
{

    int server_port = -1;
    int sock        =  0;
    int rc          = -1;

    /* parse command line args */
    if (argc != 3) {
	  fprintf(stderr, "usage: http_server1 <k|u> <port>\n");
	  exit(-1);
    }

    server_port = atoi(argv[2]);

    if (server_port < 1500) {
	  fprintf(stderr, "INVALID PORT NUMBER: %d; can't be < 1500\n", server_port);
	  exit(-1);
    }

    /* initialize and make socket */
    if (toupper(*(argv[1])) == 'K') {
	  rc = minet_init(MINET_KERNEL);
    } else if (toupper(*(argv[1])) == 'U') {
	  rc = minet_init(MINET_USER);
    } else {
	  fprintf(stderr, "First argument must be k or u\n");
	  exit(-1);
    }

    if (rc == -1) {
	  fprintf(stderr, "Could not initialize Minet\n");
	  exit(-1);
    }

    sock = minet_socket(SOCK_STREAM);

    if (sock < 0) {
	  minet_perror("couldn't make socket\n");
	  exit(-1);
    }

    /* set server address*/
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY; // Listen on any IP address 
    addr.sin_port = server_port;


    /* bind listening socket */
    if (minet_bind(sock, &addr) < 0) {
        // panic
        exit(-2);
    }

    /* start listening */
    if (minet_listen(sock, 32) < 0) { // I have zero idea what this second parameter is
        // panic
        exit(-3);
    }

    /* connection handling loop */
    while(1)
    {
        /* Accept connections */
        int c = minet_accept(sock, &addr);
        rc = handle_connection(c);
    }
}


int
handle_connection(int sock2)
{
    char   response_200[]  = "HTTP/1.0 200 OK\r\n"           \
	                      "Content-type: text/plain\r\n"  \
	                      "Content-length: ";
    char   two_nl[]        = "\r\n\r\n";
    char   response_404[] = "HTTP/1.0 404 FILE NOT FOUND\r\n" \
	                      "Content-type: text/html\r\n\r\n"		     \
	                      "<html><body bgColor=black text=white>\n"	     \
	                      "<h2>404 FILE NOT FOUND</h2>\n"
	                      "</body></html>\n";
    char   response_405[] = "HTTP/1.0 405 METHOD NOT ALLOWED\r\n" \
                            "Content-type: text/html\r\n\r\n"		     \
                            "<html><body bgColor=black text=white>\n"	     \
                            "<h2>405 METHOD NOT ALLOWED - Only get requests are accepted.</h2>\n"
                            "</body></html>\n";
    
    char *response;

    // Buffer for the requests
    char *buf = (new char[BUFSIZE])[0];
    int bytesread;
    int i;

    bytesread = minet_read(sock2, buf, BUFSIZE);

    // make everything all-caps
    for (i = 0; i < bytesread; i++) {
        if (islower(buf[i])) {
            buf[i] = toupper(buf[i]);
        }
    }
    
    /* first read loop -- get request and headers*/
    const char *delims = " "; // use space as delimiter

    char *method = strtok(&buf, delims);
    char *resource = strtok(&buf, delims);
    char *version = strtok(&buf, delims); // should almost always be HTTP/1.0
    

    /* parse request to get file name */
    /* Assumption: this is a GET request and filename contains no spaces*/
    if (strcmp(method, "GET") != 0) {
        minet_write(sock2, response_405, sizeof(response_405));
        minet_close(sock2);
        return -1;
    }

    // Attempt to open resource
    FILE *fd = fopen(resource, "r");
    if (fd == NULL) {
        minet_write(sock2, response_404, sizeof(response_404));
        minet_close(sock2);
        return -1;
    }
    else {
        // Write ok response and write file contents:
        //  Read in file size in bytes for header
        //  Write response string
        //  Write all appropriate headers
        struct stat info;
        int x = fstat(fileno(fd), &stat);
        int file_len = (int) stat.st_size; // should be good unless the file is greater than 2^32 bytes
        char size_buf[20]; // if the size is larger then I don't even know what to do
        itoa(len, size_buf, 10);
        int size_len = strlen(size_buf);

        minet_write(sock2, response_200, sizeof(response_200));
        minet_write(sock2, size_buf, size_len);
        minet_write(sock2, two_nl, sizeof(two_nl));
        
        //  Read file into memory
        //  Write file to minet socket
        char *file_contents = (char *) malloc(size_len + 1);
        fread(file_contents, 1, file_len, fd);
        minet_write(sock2, file_contents, file_len);
        minet_close(sock2);
        return 1;
    }

    return -2; // should not be reached
}
