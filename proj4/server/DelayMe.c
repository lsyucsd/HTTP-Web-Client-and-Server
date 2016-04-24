#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "Practical.h"

#include <gsl/gsl_rng.h>
#include <gsl/gsl_sf_bessel.h>
#include <gsl/gsl_randist.h>
#include <time.h>
#include <fcntl.h>

void *ThreadMain(void *arg); // Main program of a thread

// Structure of arguments to pass to client thread
struct ThreadArgs {
  int clntSock; // Socket descriptor for client
  double y;
};

int main(int argc, char *argv[]) {

  if (argc != 2) // Test for correct number of arguments
    DieWithUserMessage("Parameter(s)", "<Server Port/Service>");

  char *servPort = argv[1]; // First arg:  local port
  int servSock = SetupTCPServerSocket(servPort);
  if (servSock < 0)
    DieWithUserMessage("SetupTCPServerSocket() failed", "unable to establish");

  const gsl_rng_type * T;
  gsl_rng * r;

  r = gsl_rng_alloc(gsl_rng_mt19937);
  gsl_rng_set(r, (unsigned int)time(NULL));

  for (;;) { // Run forever
    int clntSock = AcceptTCPConnection(servSock);

    //fcntl(clntSock, F_SETFL, O_NONBLOCK);
    double mean = 90.0;
    double sigma = 20.0;

    double y = mean + gsl_ran_gaussian(r, sigma);
    printf ("%g\n", y);

    //sleep(1);
    // Create separate memory for client argument
    struct ThreadArgs *threadArgs = (struct ThreadArgs *) malloc(
        sizeof(struct ThreadArgs));
    if (threadArgs == NULL)
      DieWithSystemMessage("malloc() failed");
    threadArgs->clntSock = clntSock;
    threadArgs->y = y;

    // Create client thread
    pthread_t threadID;
    int returnValue = pthread_create(&threadID, NULL, ThreadMain, threadArgs);
    if (returnValue != 0)
      DieWithUserMessage("pthread_create() failed", strerror(returnValue));
    printf("with thread %ld\n", (long int) threadID);
  }
  // NOT REACHED
}

void *ThreadMain(void *threadArgs) {
  // Guarantees that thread resources are deallocated upon return
  pthread_detach(pthread_self());

  // Extract socket file descriptor from argument
  int clntSock = ((struct ThreadArgs *) threadArgs)->clntSock;

  double y = ((struct ThreadArgs *) threadArgs)->y;

  free(threadArgs); // Deallocate memory for argument

  // const gsl_rng_type * T;
  // gsl_rng * r;

  // r = gsl_rng_alloc(gsl_rng_mt19937);
  // gsl_rng_set(r, (unsigned int)time(NULL));

  // double mean = 90.0;
  // double sigma = 20.0;

  // double y = mean + gsl_ran_gaussian(r, sigma);
  // printf ("%g\n", y);

  HandleTCPClient(clntSock, y);

  return (NULL);
}