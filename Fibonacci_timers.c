#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>

//for portability, just incase...
#include <stdbool.h>


//GLOBAL VARIABLES
int fn1, fn2, fn3;
long int fibresult;

int walltimeSec, walltimeUsec;
int cputimeSec, cputimeUsec;
int userspacetimeSec, userspacetimeUsec;
int kerneltimeSec, kerneltimeUsec;
int counterReal, counterProf, counterVirt;

struct itimerval timer;


//FUNCTION DECLARATIONS
void correctNumberOfArgs(int arraySize, int correctMaxSize);

void lazyExit(void);

void valueChecker(int arraySize, char *array[], int startPoint);
void checkerImplementer(int index, int value);
bool invalidArg(char *ptr);

long int fib( int n);

void allHandler(int sig);
void registerHandlers(void);
void setupTimer(struct itimerval timer);
void profileProcess(void);
void printProfile(void);



//MAIN PROGRAM
int main(int argc, char *argv[])
{
  correctNumberOfArgs(argc, 4);
  valueChecker(argc, argv, 1);
  fflush(stdout); //just incase ig
  
  //parent calculates anytime, but prints its profile after the children.
  //pid_t parentpid = getpid();
  pid_t pid = fork();
  pid_t pid2;
    
  if(pid == 0) {
    //CHILD 1

    //Profiling child 1
    registerHandlers();
    setupTimer(timer);

    fibresult = fib(fn2);
    profileProcess();
    
    printf("\n[Child 1] fibonacci(%d) = %ld\n", fn2, fibresult);
    printProfile();
  }
  else if(pid > 0){
    //PARENT
    //SECTION FOR CREATON OF CHILD2
    pid2 = fork();

    if(pid2 == 0){
      //CHILD 2

      //Profiling child 2
      registerHandlers();
      setupTimer(timer);

      fibresult = fib(fn3);
      profileProcess();
      
      printf("\n[Child 2] fibonacci(%d) = %ld\n", fn3, fibresult);
      printProfile();
    }
    else if(pid2 > 0){
      //PARENT EXECUTION FOR FIB, FINALLY

      //Profiling parent
      registerHandlers();
      setupTimer(timer);

      fibresult = fib(fn1);
      waitpid(pid, NULL, 0);
      waitpid(pid2, NULL, 0);

      profileProcess();
      
      printf("\n[Parent] fibonacci(%d) = %ld\n", fn1, fibresult);
      printProfile();
    }
    else {
      printf("\nFORK ISSUE FOR SOME REASON\n");
      lazyExit();
    }
  }
  else {
    printf("\nFORK ISSUE FOR SOME REASON in first fork\n");
    lazyExit();
  }

}






//FUNCTION IMPLEMENTATIONS
void correctNumberOfArgs(int arraySize, int correctMaxSize) {
  if(arraySize != correctMaxSize){
    printf("Not enough arguments\n");
    lazyExit();
  }
}

void lazyExit(void) {
  exit(EXIT_FAILURE);
}


void valueChecker(int arraySize, char *array[], int startPoint) {
  //This function uses checkerImplementer() and invalidArg()
  char *val;
  int value;
  char *end;
  for(int i = startPoint; i < arraySize; i++){
    val = array[i];
    value = strtol(val, &end, 10);
    //printf("%s = end\n", end);

    if(invalidArg(end)){
      printf("Arg %s was invalid. Process expects all three arguments to be integers\n", val);
      lazyExit();
    }
    else {
      checkerImplementer(i, value);
    }
  }
}

void checkerImplementer(int index, int value) {
  //Sets the global variables for the overall program focus.
  if(index == 1){
    fn1 = value; 
    printf("fn1=%d\n", fn1);
  }
  else if(index == 2){
    fn2 = value;
    printf("fn2=%d\n", fn2);
  }
  else {
    fn3 = value;
    printf("fn3=%d\n", fn3);
  }
  //printf("fn1=%d\nfn2=%d\nfn3=%d\n", fn1, fn2, fn3);
}

bool invalidArg(char *ptr) {
  return *ptr != '\0';
  //want this to be false for value to be accepted
}


//Recursive fibonnaci function
long int fib(int n) {
  if(n < 0){
    printf("Number %d is less than 0(no fib)", n);
    lazyExit();
  }
  else if(n == 0){
    return 0;
  }
  else if(n == 1 || n == 2){
    return 1;
  }
  else {
    return (long)(fib(n-1) + fib(n-2));
  }
}


//PROFILING SECTION
void allHandler(int sig) {
  if(sig == SIGALRM){
    //getitimer(ITIMER_REAL, &parent);
    counterReal++;
  }
  else if(sig == SIGVTALRM){
    counterVirt++;
  }
  else if(sig == SIGPROF){
    counterProf++;
  }
}

void registerHandlers(void) {
  signal(SIGALRM, allHandler);
  signal(SIGVTALRM, allHandler);
  signal(SIGPROF, allHandler);
}

void setupTimer(struct itimerval timer) {
  timer.it_value.tv_sec = 1;
  timer.it_value.tv_usec = 0;
  timer.it_interval.tv_sec = 1;
  timer.it_interval.tv_usec = 0;

  setitimer(ITIMER_REAL, &timer, NULL);
  setitimer(ITIMER_VIRTUAL, &timer, NULL);
  setitimer(ITIMER_PROF, &timer, NULL);
}

void profileProcess() {
  getitimer(ITIMER_REAL, &timer);
  int microsecs = timer.it_value.tv_usec;
  int usecs_remaining = 1000000 - microsecs;
  walltimeSec = counterReal + timer.it_value.tv_sec;
  walltimeUsec = usecs_remaining;

  getitimer(ITIMER_VIRTUAL, &timer);
  microsecs = timer.it_value.tv_usec;
  usecs_remaining = 1000000 - microsecs;
  userspacetimeSec = counterVirt + timer.it_value.tv_sec;
  userspacetimeUsec = usecs_remaining;

  getitimer(ITIMER_PROF, &timer);
  microsecs = timer.it_value.tv_usec;
  usecs_remaining = 1000000 - microsecs;
  cputimeSec = counterProf + timer.it_value.tv_sec;
  cputimeUsec = usecs_remaining;

  kerneltimeSec = cputimeSec - userspacetimeSec; 
  kerneltimeUsec = abs(cputimeUsec - userspacetimeUsec);
}

void printProfile(void) {
  printf("Wall Time Spent Executing: %d seconds, %d microseconds\n", walltimeSec, walltimeUsec);
  printf("Total Time Spent on CPU: %d seconds, %d microseconds\n", cputimeSec, cputimeUsec);
  printf("Time Spent in User-Space: %d seconds, %d microseconds\n", userspacetimeSec, userspacetimeUsec);
  printf("Time Spent in Kernel-Space: %d seconds, %d microseconds\n", kerneltimeSec, kerneltimeUsec);
}
