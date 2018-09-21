#include "minet_socket.h"
#include <stdlib.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/stat.h>


#define FILENAMESIZE 100
#define BUFSIZE      1024

typedef enum { NEW,
	       READING_HEADERS,
	       WRITING_RESPONSE,
	       READING_FILE,
	       WRITING_FILE,
	       CLOSED } states;


struct connection {
    int      sock;
    int      fd;
    char     filename[FILENAMESIZE + 1];
    char     buf[BUFSIZE + 1];
    char   * endheaders;
    bool     ok;
    long     filelen;
    states   state;
    int      headers_read;
    int      response_written;
    int      file_read;
    int      file_written;
    
    struct connection * next;
};



void read_headers  (struct connection * con);
void write_response(struct connection * con);
void read_file     (struct connection * con);
void write_file    (struct connection * con);

int
main(int argc, char ** argv)
{
  
    int     server_port  = 0;
    int     sock         = 0;  
    int     rc           = 0;


    /* parse command line args */
    if (argc != 3) {
	fprintf(stderr, "Usage: http_server3 <k|u> <port>\n");
	exit(-1);
    }
    
    server_port = atoi(argv[2]);

    if (server_port < 1500) {
	fprintf(stderr,"INVALID PORT NUMBER: %d; can't be < 1500\n", server_port);
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
    
    while(1) {
	/* create read and write lists */

	/* do a select */

	/* process sockets that are ready */
    }
}

void
read_headers(connection *con)
{
 
    /* first read loop -- get request and headers*/
  
    /* parse request to get file name */
    /* Assumption: this is a GET request and filename contains no spaces*/
  
    /* get file name and size, set to non-blocking */
    /* get name */

    
    /* try opening the file */

    /* set to non-blocking, get size */
  

    write_response(con);
}

void
write_response(struct connection * con)
{

    char ok_response_fmt[] = "HTTP/1.0 200 OK\r\n"	               \
	                     "Content-type: text/plain\r\n"            \
	                     "Content-length: %d \r\n\r\n";


    char notok_response[]  = "HTTP/1.0 404 FILE NOT FOUND\r\n"	       \
	                     "Content-type: text/html\r\n\r\n"	       \
	                     "<html><body bgColor=black text=white>\n" \
	                     "<h2>404 FILE NOT FOUND</h2>\n"	       \
	                     "</body></html>\n";

    char ok_response[100];

    /* send response */
    /* send headers */
    /* Send Data */
}

void
read_file(struct connection * con)
{

}

void
write_file(struct connection *con)
{
  
}
