// gcc -std=c99 radix_sort_serial.c -o serial
// C serial implementation of Radix Sort 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

#define ARRAY_SIZE 1024
#define GIG 1000000000
#define CPG 3.3
#define ITERS 1
#define OPTIONS 1

int main(int argc, char *argv[]) 
{ 
  // timing
  struct timespec diff(struct timespec start, struct timespec end);
  struct timespec time1, time2;
  struct timespec time_stamp[OPTIONS][ITERS+1];
  int clock_gettime(clockid_t clk_id, struct timespec *tp);
  long int i, j, k;
  long long int time_sec, time_ns;

  // Declare function to be used
  int getMax(int arr[], int n);
  void countSort(int arr[], int n, int exp) ;
  void radixsort(int arr[], int n);
  void print(int arr[], int n);
  int init_array(int *array1, int n);
  
  printf("\n Hello World -- vector radix sort\n");

  // Array init
  int *array1 = malloc(ARRAY_SIZE * sizeof(int));
  int n = ARRAY_SIZE;
  init_array(array1,n);

  printf("\n Done with inits\n");

  /*
  // Print old array
  printf("Old array: ");
  print(array1,n);
  printf("\n");
  */

  // Do radix sort
  int OPTION = 0;
  for (i = 0; i < ITERS; i++) {
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time1);
  radixsort(array1,n);
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time2);
  time_stamp[OPTION][i] = diff(time1,time2);
  }

  /*
  // Print new array
  printf("New Array: ");
  print(array1,n);
  printf("\n");
  */

  /*output times */
  printf("\nsize, time");
  for (i = 0; i < ITERS; i++) {
    printf("\n%ld, ", ARRAY_SIZE);
    for (j = 0; j < OPTIONS; j++) {
      if (j != 0) printf(", ");
      printf("%ld", (long int)((double)(CPG)*(double)
		 (GIG * time_stamp[j][i].tv_sec + time_stamp[j][i].tv_nsec)));
    }
  }

  printf("\n");


  return 0;

} 
  
// A utility function to get maximum value in arr[] 
int getMax(int arr[], int n) 
{ 
    int mx = arr[0]; 
    int i;
    for (i = 1; i < n; i++) 
        if (arr[i] > mx) 
            mx = arr[i]; 
    return mx; 
} 
  
// A function to do counting sort of arr[] according to 
// the digit represented by exp. 
void countSort(int arr[], int n, int exp) 
{ 
    int output[n]; // output array 
    int i, count[10] = {0}; 
  
    // Store count of occurrences in count[] 
    for (i = 0; i < n; i++) 
        count[ (arr[i]/exp)%10 ]++; 
  
    // Change count[i] so that count[i] now contains actual 
    //  position of this digit in output[] 
    for (i = 1; i < 10; i++) 
        count[i] += count[i - 1]; 
  
    // Build the output array 
    for (i = n - 1; i >= 0; i--) 
    { 
        output[count[ (arr[i]/exp)%10 ] - 1] = arr[i]; 
        count[ (arr[i]/exp)%10 ]--; 
    } 
  
    // Copy the output array to arr[], so that arr[] now 
    // contains sorted numbers according to current digit 
    for (i = 0; i < n; i++) 
        arr[i] = output[i]; 
} 
  
// The main function to that sorts arr[] of size n using  
// Radix Sort 
void radixsort(int arr[], int n) 
{ 
    // Find the maximum number to know number of digits 
    int m = getMax(arr, n); 
    int exp;
  
    // Do counting sort for every digit. Note that instead 
    // of passing digit number, exp is passed. exp is 10^i 
    // where i is current digit number 
    for (exp = 1; m/exp > 0; exp *= 10) 
        countSort(arr, n, exp); 
} 
  
// A utility function to print an array 
void print(int arr[], int n) 
{ 
  int i;
    for (i = 0; i < n; i++) 
      printf("%d ", arr[i]); 
} 
  
// A utility function to initialize an array
int init_array(int *array1, int n){
  int i;
  for (i = 0; i < n; i++){
    array1[i] = rand()%1000+1; // [1,1000]
  }
  return *array1;
}

/*************************************************/
struct timespec diff(struct timespec start, struct timespec end)
{
  struct timespec temp;
  if ((end.tv_nsec-start.tv_nsec)<0) {
    temp.tv_sec = end.tv_sec-start.tv_sec-1;
    temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
  } else {
    temp.tv_sec = end.tv_sec-start.tv_sec;
    temp.tv_nsec = end.tv_nsec-start.tv_nsec;
  }
  return temp;
} 

