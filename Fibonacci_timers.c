#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include <wait.h>
#include <stdlib.h>

//GLOBAL VARIABLES
int fn1, fn2, fn3;


//FUNCTION DECLARATIONS
bool correctNumberOfArgs(int arraySize, int correctMaxSize);

void lazyExit(void);

void valueChecker(int arraySize, char *array[], int startPoint);
void checkerImplementer(int index, int value);
bool invalidArg(char *ptr);


int main(int argc, char *argv[])
{
  correctNumberOfArgs(argc, 4);
  valueChecker(argc, argv, 1);
  

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
    printf("%s = end\n", end);

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
  if(index == 0){
    fn1 = value; 
  }
  else if(index == 1){
    fn2 = value;
  }
  else {
    fn3 = value;
  }
}

bool invalidArg(char *ptr) {
  return *ptr != '\0';
  //want this to be false for value to be accepted
}

