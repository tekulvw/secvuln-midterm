/* Using past-the-end index */

#include <stdlib.h>
#include <stdio.h>
 
static int *table = NULL;
static size_t size = 0;

/* Name three things wrong with this function */
int insert_in_table(size_t pos, int value) {
   if (size < pos) {  /* if table is too small, add elements */
      int *tmp;
      size = pos + 1;
      tmp = (int*)realloc(table, sizeof(*table) * size);
      if (tmp == NULL) return -1;
      table = tmp;
   }
   
   table[pos] = value;
   return 0;
}

int main (int argc, char **argv) {
   if (argc != 2) {
      printf("Usage: %s [1|2]\n", argv[0]);
      printf("  where 1= first pos request is not equal to 0\n");
      printf("        2= first pos request is equal to 0\n");
      exit(0);
   }
   int choice = atoi(argv[1]);
   int i;
   if (choice == 2) {
      insert_in_table(0, 67);
      for (i=0 ; i < 1 ; i++) printf("%d ", table[i]);
      printf("\n");
      fflush(stdout);
   }
   insert_in_table(10, 67);
   for (i=0 ; i < 11 ; i++) printf("%d ", table[i]);
   printf("\n");
   fflush(stdout);
   insert_in_table(5, 42);
   for (i=0 ; i < 11 ; i++) printf("%d ", table[i]);
   printf("\n");
   fflush(stdout);
   insert_in_table(14, 23);
   for (i=0 ; i < 15 ; i++) printf("%d ", table[i]);
   printf("\n");
}

/*
   Similar to the dereferencing-past-the-end-pointer error, the function
   insert_in_table() in this noncompliant code example uses an otherwise valid
   index to attempt to store a value in an element just past the end of an
   array.

   First, the function incorrectly validates the index pos against the size of
   the buffer.  When pos is initially equal to size, the function attempts to
   store value in a memory location just past the end of the buffer.

   Second, when the index is greater than size, the function modifies size
   before growing the size of the buffer. If the call to realloc() fails to
   increase the size of the buffer, the next call to the function with a value
   of pos equal to or greater than the original value of size will again attempt
   to store value in a memory location just past the end of the buffer or
   beyond.

   Third, the function violates INT30-C. Ensure that unsigned integer operations
   do not wrap, which could lead to wrapping when 1 is added to pos or when size
   is multiplied by the size of int.

   For a discussion of this programming error in the Common Weakness Enumeration
   database, see CWE-122, "Heap-based Buffer Overflow," and CWE-129, "Improper
   Validation of Array Index"
*/
