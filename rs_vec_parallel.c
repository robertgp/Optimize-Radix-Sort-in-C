// gcc -O1 -fopenmp rs_vec_parallel.c -lrt -lm -o parallel
// Vectorized and parallel version in c of radix sort

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <omp.h>

#define GIG 1000000000
#define CPG 3.3

#define ARRAY_SIZE 10240
#define ITERS 20
#define DELTA 500
#define BASE 0
#define THREADS 4

#define OPTIONS 1

typedef int data_t;

/* Create abstract data type for vector */
typedef struct {
  long int len;
  data_t *data;
} vec_rec, *vec_ptr;

/* Number of bytes in a vector */
#define VBYTES 32

/* Number of elements in a vector */
#define VSIZE VBYTES/sizeof(data_t)

typedef data_t vec_t __attribute__ ((vector_size(VBYTES)));
typedef union {
  vec_t v;
  data_t d[VSIZE];
} pack_t;

/*****************************************************************************/
int main(int argc, char *argv[]) 
{ 
  int OPTION;
  struct timespec diff(struct timespec start, struct timespec end);
  struct timespec time1, time2;
  struct timespec time_stamp[OPTIONS][ITERS+1];
  int clock_gettime(clockid_t clk_id, struct timespec *tp);
 
  vec_ptr new_vec(long int len);
  int get_vec_element(vec_ptr v, long int index, data_t *dest);
  long int get_vec_length(vec_ptr v);
  int set_vec_length(vec_ptr v, long int index);
  int init_vector(vec_ptr v, long int len);
  data_t *data_holder;

  // declare function to be used
  void radixSort(vec_ptr v);
  long int getMax(vec_ptr v);
  void countSort(vec_ptr v, data_t exp);
  void print_v(vec_ptr v);
  
  //other stuff
  long int i, j, k;
  long long int time_sec, time_ns;
  long int MAXSIZE = BASE+(ITERS+1)*DELTA;

  printf("\n Hello World -- vector radix sort\n");

  // declare and initialize the vector structure
  vec_ptr v0 = new_vec(MAXSIZE);
  init_vector(v0, MAXSIZE);

  // Print vector before sorting to check for correctness
  /*
  printf("The original vector is: \n");
  print_v(v0);
  printf("\n");
  */

  // Execute radix sort
  OPTION = 0;
  for (i = 0; i < ITERS; i++) {
    set_vec_length(v0, BASE+(i+1)*DELTA);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time1);
    radixSort(v0);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time2);
    time_stamp[OPTION][i] = diff(time1,time2);
  }

  // Print sorted vector
  /*
  printf("The sorted vector is: \n");
  print_v(v0);
  printf("\n");
  */


  /*output times */
  printf("\nsize, time");
  for (i = 0; i < ITERS; i++) {
    printf("\n%ld, ", ((int) ( BASE+(i+1)*DELTA) ));
    for (j = 0; j < OPTIONS; j++) {
      if (j != 0) printf(", ");
      printf("%ld", (long int)((double)(CPG)*(double)
		 (GIG * time_stamp[j][i].tv_sec + time_stamp[j][i].tv_nsec)));
    }
  }

  printf("\n");

  free(v0);
  

} /* end main */

/**********************************************/
/* Create vector of specified length */
vec_ptr new_vec(long int len)
{
  long int i;

  /* Allocate and declare header structure */
  vec_ptr result = (vec_ptr) malloc(sizeof(vec_rec));
  if (!result) return NULL;  /* Couldn't allocate storage */
  result->len = len;

  /* Allocate and declare array */
  if (len > 0) {
    data_t *data = (data_t *) calloc(len, sizeof(data_t));
    if (!data) {
	  free((void *) result);
	  return NULL;  /* Couldn't allocate storage */
	}
	result->data = data;
  }
  else result->data = NULL;

  return result;
}

/* Retrieve vector element and store at dest.
   Return 0 (out of bounds) or 1 (successful)
*/
int get_vec_element(vec_ptr v, long int index, data_t *dest)
{
  if (index < 0 || index >= v->len) return 0;
  *dest = v->data[index];
  return 1;
}

/* Return length of vector */
long int get_vec_length(vec_ptr v)
{
  return v->len;
}

/* Set length of vector */
int set_vec_length(vec_ptr v, long int index)
{
  v->len = index;
  return 1;
}

/* initialize vector */
int init_vector(vec_ptr v, long int len)
{
  long int i;

  if (len > 0) {
    v->len = len;
    for (i = 0; i < len; i++) v->data[i] = (data_t)rand()%1000 + 1; // [1,1000]
    return 1;
  }
  else return 0;
}

/* initialize vector with zeros*/
int init_vector_zero(vec_ptr v, long int len)
{
  long int i;

  if (len > 0) {
    v->len = len;
    for (i = 0; i < len; i++) v->data[i] = 0;
    return 1;
  }
  else return 0;
}

data_t *get_vec_start(vec_ptr v)
{
  return v->data;
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


/**********************************************/
// A utility function to print a vector
void print_v(vec_ptr v) 
{  
  long int i;
  long int get_vec_length(vec_ptr v);
  data_t *get_vec_start(vec_ptr v);
  long int length = get_vec_length(v);
  data_t *data = get_vec_start(v);

  for (i = 0; i < length; i++)
    printf("%d ", data[i]);
} 

// A utility function to get maximum value in v     
long int getMax(vec_ptr v)
{
  long int i;
  long int get_vec_length(vec_ptr v);
  data_t *get_vec_start(vec_ptr v);
  long int length = get_vec_length(v);
  data_t *data = get_vec_start(v);
  long int max = data[0];

  // Unroll by 2
  for (i = 1; i < length - 1; i+=2) {
    if (data[i] > max) max = data[i];
    else if (data[i+1] > max) max = data[i+1];
  }

  // Finish remaining elements
  for (; i < length; i++){
    if (data[i] > max) max = data[i];
  }

  return max;
}

// Actual sorting
void countSort(vec_ptr v, data_t exp)
{
  long int i;
  long int get_vec_length(vec_ptr v);
  data_t *get_vec_start(vec_ptr v);
  long int length = get_vec_length(v);
  data_t *data = get_vec_start(v);

  vec_ptr output = new_vec(ARRAY_SIZE); // output vector
  data_t *data_output = get_vec_start(output);

  vec_ptr count = new_vec(10);
  init_vector_zero(count, 10);
  data_t *data_count = get_vec_start(count);

  //************************* FOR LOOP 1 *********************************
  // Store count of ocurrences in vector count
  int bucket_2d[THREADS][10] = {0};
  int tid;
  omp_set_num_threads(THREADS);

#pragma omp parallel for
  for (i = 0; i < length; i++){
    tid = omp_get_thread_num();
    bucket_2d[tid][(data[i]/exp)%10]++;
  }
  
  for (i=0; i < 10; i++){
  data_count[i] = bucket_2d[0][i] + bucket_2d[1][i] + bucket_2d[2][i] + bucket_2d[3][i];
  }


  //************************* FOR LOOP 2 *********************************
  // Change data_count[i] so that it now contains actual 
  //  position of this digit in output
  // Unroll by 2
  for (i = 1; i < 9; i+=2){ 
    data_count[i] += data_count[i-1];
    data_count[i+1] += data_count[i];
   }

  // Finish remaining elements
  for (; i < 10; i++){
     data_count[i] += data_count[i-1];
  }




  //************************* FOR LOOP 3 *********************************
  // Build the output array
  for (i = length - 1; i >= 0; i--){
    data_output[data_count[ (data[i]/exp)%10 ] - 1] = data[i];
    data_count[ (data[i]/exp)%10 ]--;
  }

  //*************************EXTRA FOR LOOP*********************************
  // Copy output to v with unroll by 2
  for (i = 0; i < length - 1; i+=2){
    data[i] = data_output[i];
    data[i+1] = data_output[i+1];
  }

  for (; i < length; i++){
     data[i] = data_output[i];
  }
  
}
  
// The main function to that sorts arr[] of size n using  
// Radix Sort 
void radixSort(vec_ptr v)
{
  // get the max number in v
  int m = getMax(v);
  int exp;

  // do radix sort on every digit starting
  for (exp = 1; m/exp > 0; exp *= 10)
    countSort(v, exp);

}
