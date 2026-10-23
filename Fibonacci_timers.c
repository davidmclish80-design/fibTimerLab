#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include <wait.h>
#include <stdlib.h>

//GLOBAL VARIABLES
int fn1, fn2, fn3;
long int fibresult;

int walltime, cputime, userspacetime, kernaltime;
int counterReal, counterProf, counterVirt;


//FUNCTION DECLARATIONS
bool correctNumberOfArgs(int arraySize, int correctMaxSize);

void lazyExit(void);

void valueChecker(int arraySize, char *array[], int startPoint);
void checkerImplementer(int index, int value);
bool invalidArg(char *ptr);

long int fib( int n);


int main(int argc, char *argv[])
{
  correctNumberOfArgs(argc, 4);
  valueChecker(argc, argv, 1);
  fflush(stdin); //just incase ig

  //parent calculates anytime, but prints its profile after the children.
  //pid_t parentpid = getpid();
  pid_t pid = fork();
  pid_t pid2;
    
  if(pid == 0) {
    //CHILD 1
    printf("\nfn2 = %d", fn2);
    fibresult = fib(fn2);
    printf("\n[Child 1] fibonacci(%d) = %ld\n", fn2, fibresult);
  }
  else if(pid > 0){
    //PARENT
    //SECTION FOR CREATON OF CHILD2
    pid2 = fork();

    if(pid2 == 0){
      //CHILD 2
      printf("\nfn3 = %d", fn3);
      fibresult = fib(fn3);
      printf("\n[Child 2] fibonacci(%d) = %ld\n", fn3, fibresult);
    }
    else if(pid2 > 0){
      //PARENT EXECUTION FOR FIB, FINALLY
      waitpid(pid, NULL, 0);
      waitpid(pid2, NULL, 0);

      printf("\nfn1 = %d", fn1);
      fibresult = fib(fn1);
      printf("\n[Parent] fibonacci(%d) = %ld\n", fn1, fibresult);
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
bool correctNumberOfArgs(int arraySize, int correctMaxSize) {
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

