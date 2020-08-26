#define _GNU_SOURCE

#include <assert.h>
#include <err.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include<sys/wait.h>
#include <netdb.h>
#include <unistd.h>

#include "shared_queue.h"

#define BUFFER_SIZE 512

void echo(int fd_in, int fd_out)
{
    ssize_t r;
    char buff[512];
    while ((r=read(fd_in,buff,512))>0)
    {
        int offset=0;
        while(r>0)
        {
            ssize_t w = write(fd_out,buff+offset,r);
            if(w==-1)
                err(1,"An error has occured while writing data.");
            offset+=w;
            r-=w;
        }
    }
    if(r==-1)
        err(1,"An error has occured while reading data.");
}

void* worker(void *arg_) {
  struct shared_queue * q = arg_;
  while(1)
  {
    int fd = shared_queue_pop(q);
    printf("New connection (thread_Id = %li)\n",pthread_self());
    echo(fd,fd);
    printf("Close connection (thread_Id = %li)\n",pthread_self());
    close(fd);
  }
  pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
  struct shared_queue * q = new_shared_queue();
  if (argc != 2)
      errx(EXIT_FAILURE, "Usage:\n"
          "Arg 1 = Port number (e.g. 2048)");

  struct addrinfo hints;
  struct addrinfo *res;

  memset(&hints,0,sizeof(struct addrinfo));
  hints.ai_family=AF_INET;
  hints.ai_socktype=SOCK_STREAM;
  hints.ai_flags=AI_PASSIVE;

  int adr = getaddrinfo(NULL,argv[1],&hints,&res);
  if(adr!=0)
      err(adr,"an error has occured");

  int soc=0;
  while(res!=NULL)
  {
    soc=socket(res->ai_family,res->ai_socktype,res->ai_protocol);
    if(soc!=-1)
    {
      int optVal = 1;
      setsockopt(soc,SOL_SOCKET,SO_REUSEADDR,&optVal,sizeof(int));
      if(bind(soc,res->ai_addr,res->ai_addrlen)==0)
          break;
    }
    close(soc);
    res=res->ai_next;
  }

  int thread_number = 8;
  pthread_t thread_Id[thread_number];

  if(res==NULL ||soc==-1)
      err(1,"an error has occured during socket creation");
  if(listen(soc,thread_number)==-1)
      err(1,"an error has occured");
  printf("Waiting for connections...\n");

  for (int i = 0; i < thread_number; i++) {
    int ptSucess = pthread_create(&thread_Id[i], NULL, worker, q);
    if(ptSucess!=0)
    {
        errno = ptSucess;
        err(EXIT_FAILURE,"Error in thread");
    }
  }
  while(1)
  {
    int connectedSoc=accept(soc,res->ai_addr,&res->ai_addrlen);
    if(connectedSoc==-1)
        err(1,"an error has occured");
    shared_queue_push(q,connectedSoc);
  }
  for (int i = 0; i < thread_number; i++) {
    pthread_join(thread_Id[i], NULL);
  }
  close(soc);
  freeaddrinfo(res);
  return 1;
}
