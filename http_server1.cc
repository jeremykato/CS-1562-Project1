#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>

#include "minet_socket.h"

#define BUFSIZE 1024
#define FILENAMESIZE 100

int handle_connection(int);

int writenbytes(int, char *, int);
int readnbytes (int, char *, int);


int
main(int argc, char ** argv)
{
    fprintf(stderr, "Starting server...\n");

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

    fprintf(stderr, "Init succeeded\n");

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
    addr.sin_port = htons(server_port);


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

    fprintf(stderr, "Listening on port %d\n", server_port);
    char *pwd = (char *) malloc(50);
    pwd = getcwd(pwd, 50);
    fprintf(stderr, "Current directory: %s\n", pwd);

    /* connection handling loop */
    while(1)
    {
        /* Accept connections */
        int c = minet_accept(sock, &addr);
        fprintf(stderr, "Receieved request!\n");
        rc = handle_connection(c);
        fprintf(stderr, "Request completed!\n");
        minet_close(c);
    }
}


int
handle_connection(int sock2)
{
    char   response_200[]  = "HTTP/1.0 200 OK\r\n"           \
	                      "Content-type: text/plain\r\n"  \
	                      "Content-length: ";
    char   two_nl[]        = " \r\n\r\n";
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


    // Buffer for the requests
    char *buf = (char *) malloc(BUFSIZE);
    int bytesread;
    int i;


    bytesread = minet_read(sock2, buf, BUFSIZE);

    // make everything all-caps
    // no, we don't want to do this.
    /*
    for (i = 0; i < bytesread; i++) {
        if (islower(buf[i])) {
            buf[i] = toupper(buf[i]);
        }
    } */
    
    /* first read loop -- get request and headers*/
    const char *delims = " "; // use space as delimiter

    char *method = strtok(buf, delims);
    char *resource = strtok(NULL, delims);
    //char *version = strtok(buf, delims); // should almost always be HTTP/1.0

    fprintf(stderr, "Method: %s\n", method);
    fprintf(stderr, "Resource: %s\n", resource);
    

    /* parse request to get file name */
    /* Assumption: this is a GET request and filename contains no spaces*/
    if (strcmp(method, "GET") != 0) {
        minet_write(sock2, response_405, sizeof(response_405));
        free(buf);
        return -1;
    }

    // Attempt to open resource
    FILE *fd = fopen(resource, "r");
    if (fd == NULL) {
        minet_write(sock2, response_404, sizeof(response_404));
        free(buf);
        return -1;
    }
    else {
        // Write ok response and write file contents:
        //  Read in file size in bytes for header
        //  Write response string
        //  Write all appropriate headers
        struct stat info;
        int file_desc = fileno(fd);

        fstat(file_desc, &info);
        int file_len = (int) info.st_size; // should be good unless the file is greater than 2^32 bytes
        char size_buf[10]; // if the size is larger then I don't even know what to do
        for (int i = 0; i < 10; i++) {
            size_buf[i] = 0;
        }
        sprintf(size_buf, "%d", file_len);
        
        int size_len = strlen(size_buf);


        fprintf(stderr, response_200);
        fprintf(stderr, size_buf);
        fprintf(stderr, two_nl);


        minet_write(sock2, response_200, sizeof(response_200));
        minet_write(sock2, size_buf, size_len);
        minet_write(sock2, two_nl, sizeof(two_nl));
        
        //  Read file into memory
        //  Write file to minet socket
        char *file_contents = (char *) malloc(file_len);
        fread(file_contents, 1, file_len, fd);

        fprintf(stderr, file_contents);

        minet_write(sock2, file_contents, file_len);
        //free(file_contents);
        free(buf);
        return 1;
    }

    free(buf);
    return -2; // should not be reached
}
