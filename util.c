
/*CSci4061 F2014 Assignment 5
*date: 12/10/2014
*name: Feng Liu, FNU Huri, Yang Yang
*id:   liux2824,  huri0004, yang3362 
*/


#ifndef _REENTRANT
#define _REENTRANT
#endif

#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>


int keep_alive_flag;
int mz_socket;
int socket_fd;

pthread_mutex_t accept_con_mutex = PTHREAD_MUTEX_INITIALIZER;

struct sigaction act;  
 struct timespec timer;
       
static void action(int sig) {
pthread_mutex_lock(&accept_con_mutex);
        keep_alive_flag=0;
	 printf("5s without request\n");
        pthread_mutex_unlock(&accept_con_mutex);

}

int lookup_host (const char *host)
{
  struct addrinfo hints, *res;
  int errcode;
  char addrstr[100];
  void *ptr;

  memset (&hints, 0, sizeof (hints));
  hints.ai_family = PF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags |= AI_CANONNAME;

  errcode = getaddrinfo (host, NULL, &hints, &res);
  if (errcode != 0)
    {
      perror ("getaddrinfo");
      return -1;
    }

  printf ("Host: %s\n", host);
  while (res)
    {
      inet_ntop (res->ai_family, res->ai_addr->sa_data, addrstr, 100);

      switch (res->ai_family)
        {
        case AF_INET:
          ptr = &((struct sockaddr_in *) res->ai_addr)->sin_addr;
          break;
        case AF_INET6:
          ptr = &((struct sockaddr_in6 *) res->ai_addr)->sin6_addr;
          break;
        }
      inet_ntop (res->ai_family, ptr, addrstr, 100);
      printf ("IPv%d address: %s (%s)\n", res->ai_family == PF_INET6 ? 6 : 4,
              addrstr, res->ai_canonname);
      res = res->ai_next;
    }

  return 0;
}

int makeargv(const char *s, const char *delimiters, char ***argvp) {
   int error;
   int i;
   int numtokens;
   const char *snew;
   char *t;

   if ((s == NULL) || (delimiters == NULL) || (argvp == NULL)) {
      errno = EINVAL;
      return -1;
   }
   *argvp = NULL;
   snew = s + strspn(s, delimiters);
   if ((t = malloc(strlen(snew) + 1)) == NULL)
      return -1;
   strcpy(t,snew);
   numtokens = 0;
   if (strtok(t, delimiters) != NULL)
      for (numtokens = 1; strtok(NULL, delimiters) != NULL; numtokens++) ;

   if ((*argvp = malloc((numtokens + 1)*sizeof(char *))) == NULL) {
      error = errno;
      free(t);
      errno = error;
      return -1;
   }

   if (numtokens == 0)
      free(t);
   else {
      strcpy(t,snew);
      **argvp = strtok(t,delimiters);
      for (i=1; i<numtokens; i++)
         *((*argvp) +i) = strtok(NULL,delimiters);
   }
   *((*argvp) + numtokens) = NULL;
   return numtokens;
}

void freemakeargv(char **argv) {
   if (argv == NULL)
      return;
   if (*argv != NULL)
      free(*argv);
   free(argv);
}

/**********************************************
 * init
   - port is the number of the port you want the server to be
     started on
   - initializes the connection acception/handling system
   - YOU MUST CALL THIS EXACTLY ONCE (not once per thread,
     but exactly one time, in the main thread of your program)
     BEFORE USING ANY OF THE FUNCTIONS BELOW
   - if init encounters any errors, it will call exit().
************************************************/
enum boolean {FALSE, TRUE};
void init(int port) 
{
       

    timer.tv_sec = 5;
     timer.tv_nsec = 0;
     
   act.sa_handler = action;
   act.sa_flags = 0;
  
   sigfillset(&act.sa_mask);
      
   sigaction(SIGALRM, &act, NULL);
      
        mz_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (mz_socket <= 0) {                
                exit(1);
        }
       
        int enable = 1;
        setsockopt(mz_socket, SOL_SOCKET, SO_REUSEADDR,
                           (char*)&enable, sizeof(int));
       
      
        struct sockaddr_in server;
        server.sin_family = AF_INET;
        server.sin_addr.s_addr = htons(INADDR_ANY);
        server.sin_port = htons( port );
       
        if (bind(mz_socket, (struct sockaddr*)&server, sizeof(server)) < 0) {                
                exit(1);
        }
       
        
        if (listen(mz_socket, 5) < 0) {
                               
                exit(1);
        }

}

/**********************************************
 * accept_connection - takes no parameters
   - returns a file descriptor for further request processing.
     DO NOT use the file descriptor on your own -- use
     get_request() instead.
   - if the return value is negative, the thread calling
     accept_connection must exit by calling pthread_exit().
***********************************************/
int accept_connection(void) 
{
        
        struct sockaddr_in client_addr;
        unsigned int size = sizeof(struct sockaddr);
	

        
	 if (!keep_alive_flag){
	 pthread_mutex_lock(&accept_con_mutex);
        socket_fd = accept(mz_socket, (struct sockaddr*)&client_addr, &size);
        keep_alive_flag=1;
        pthread_mutex_unlock(&accept_con_mutex);
	 if (socket_fd <= 0) {
               
                return -1;
        }}
	 else {

	 setitimer(ITIMER_REAL, &timer, NULL);
	 

	  }
        
        return socket_fd;

}

/**********************************************
 * get_request
   - parameters:
      - fd is the file descriptor obtained by accept_connection()
        from where you wish to get a request
      - filename is the location of a character buf in which
        this function should store the requested filename. (buf
        should be of size 1024 bytes.)
   - returns 0 on success, nonzero on failure. You must account
     for failures because some connections might send faulty
     requests. This is a recoverable error - you must not exit
     inside the thread that called get_request. After an error, you
     must NOT use a return_request or return_error function for that
     specific 'connection'.
************************************************/
int get_request(int fd, char *filename) 
{
        char buf[1024];
        char ** request; 
         
        if (read(fd, buf, 1024) < 0) {
               
                return -1;
        }
       
        int num_of_lines = 0;
        if ((num_of_lines = makeargv(buf, "\n", &request)) < 0) {
                
                return -1;
        }
       
        int i;
	      keep_alive_flag=0;
        for (i = 0; i < num_of_lines; i++) {
               
                char ** row;
             
                if (makeargv(request[i], " ", &row) < 0) {
                       
                        return -1;
                }
              
                if (strcmp(row[0], "GET") == 0) {
                        strncpy(filename, row[1], 1023);
                }
  
                if (strcmp(row[0], "Connection:") == 0) {
                       if (strcmp(row[1], "Keep-Alive") == 0) { 
			  pthread_mutex_lock(&accept_con_mutex);
			  keep_alive_flag = 1; 
			  
			  pthread_mutex_unlock(&accept_con_mutex);}
                      
                }
             
        }
        freemakeargv(request);
        return 0;

}



/**********************************************
 * return_result
   - returns the contents of a file to the requesting client
   - parameters:
      - fd is the file descriptor obtained by accept_connection()
        to where you wish to return the result of a request
      - content_type is a pointer to a string that indicates the
        type of content being returned. possible types include
        "text/html", "text/plain", "image/gif", "image/jpeg" cor-
        responding to .html, .txt, .gif, .jpg files.
      - buf is a pointer to a memory location where the requested
        file has been read into memory (the heap). return_result
        will use this memory location to return the result to the
        user. (remember to use -D_REENTRANT for CFLAGS.) you may
        safely deallocate the memory after the call to
        return_result (if it will not be cached).
      - numbytes is the number of bytes the file takes up in buf
   - returns 0 on success, nonzero on failure.
************************************************/
int return_result(int fd, char *content_type, char *buf, int numbytes) 
{

	     FILE* stream = fdopen(fd, "w");
    fprintf(stream, "HTTP/1.1 200 OK\nContent-Type: %s\nContent-numbytes: %d\nConnection: Close\n\n", content_type, numbytes);
    fflush(stream);
   
    if (write(fd, buf, numbytes) < 0)
        fprintf(stderr, "Fail to write\n");
   
    fclose(stream);
    

    return 0;


}


/**********************************************
 * return_error
   - returns an error message in response to a bad request
   - parameters:
      - fd is the file descriptor obtained by accept_connection()
        to where you wish to return the error
      - buf is a pointer to the location of the error text
   - returns 0 on success, nonzero on failure.
************************************************/
int return_error(int fd, char *buf) 
{

 
  FILE* stream = fdopen(fd, "w");
    
    fprintf(stream, "HTTP/1.1 401 Not Found\nContent-Type: text/html\nContent-numbytes: %zu\nConnection: Close\n\n", strlen(buf));
    fflush(stream);
    
    if (write(fd, buf, strlen(buf)) == -1)
        fprintf(stderr, "Fail to write\n");
    
    fclose(stream);
    
    
    return 0;

}

