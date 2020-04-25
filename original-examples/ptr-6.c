/* Apparently Accessible Out-of-Range Index */

#include <stddef.h>
#include <stdio.h>

#define COLS 5
#define ROWS 7
static int matrix[ROWS][COLS];
 
void init_matrix(int x) {
   size_t i, j;
   for (i = 0; i < COLS; i++) {
      for (j = 0; j < ROWS; j++) {
	 matrix[i][j] = x;
      }
   }
}

void print_matrix() {
   size_t i, j;
   for (i = 0; i < COLS; i++) {
      for (j = 0; j < ROWS; j++) {
	 printf("%d ",matrix[i][j]);
      }
      printf("\n");
   }
}

int main () {
   init_matrix(3);
   print_matrix();
}

/*
   This noncompliant code example declares matrix to consist of 7 rows and 5
   columns in row-major order. The function init_matrix iterates over all 35
   elements in an attempt to initialize each to the value given by the function
   argument x. However, because multidimensional arrays are declared in C in
   row-major order, the function iterates over the elements in column-major
   order, and when the value of j reaches the value COLS during the first
   iteration of the outer loop, the function attempts to access element
   matrix[0][5]. Because the type of matrix is int[7][5], the j subscript 
   is out of range, and the access has undefined behavior 49.
*/
