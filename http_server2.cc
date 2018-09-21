#include <stdlib.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/stat.h>

#include "minet_socket.h"


#define BUFSIZE      1024
#define FILENAMESIZE 100


int main(int argc, char *argv[])
{
    
    int server_port = 0;
    int sock        = 0;
    int rc          = 0;


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
	minet_init(MINET_KERNEL);
    } else if (toupper(*(argv[1])) == 'U') {
	minet_init(MINET_USER);
    } else {
	fprintf(stderr, "First argument must be 'k' or 'u'\n");
	exit(-1);
    }
    
    sock = minet_socket(SOCK_STREAM);

    if (sock < 0) {
	minet_perror("couldn't make socket\n");
	exit(-1);
    }

    /* set server address*/

    /* bind listening socket */
    
    /* start listening */
    
    /* connection handling loop */

    while (1) {
	/* create read list */

	/* poll with select (or poll/epoll) */

	/* process sockets that are ready */

        /* for the accept socket, add accepted connection to connections */
        /* for a connection socket, handle the connection */
      
        // rc = handle_connection(...);
    }
}

int
handle_connection(int sock2)
{

 
    
    char   ok_response_f[]  = "HTTP/1.0 200 OK\r\n"	                \
	                      "Content-type: text/plain\r\n"            \
	                      "Content-length: %d \r\n\r\n";
    char   notok_response[] = "HTTP/1.0 404 FILE NOT FOUND\r\n"	        \
	                      "Content-type: text/html\r\n\r\n"		\
	                      "<html><body bgColor=black text=white>\n"	\
	                      "<h2>404 FILE NOT FOUND</h2>\n"		\
	                      "</body></html>\n";
    bool   ok = true;

    /* first read loop -- get request and headers*/

    /* parse request to get file name */
    /* Assumption: this is a GET request and filename contains no spaces*/

    /* try opening the file */

    /* send response */
    if (ok) {
	/* send headers */

	/* send file */
	
    } else {
        /* Send Error Response */
    }

    /* Close Socket and free state */
    
    if (ok) {
	return 0;
    } else {
	return -1;
    }
}
