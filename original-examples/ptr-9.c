/* 
   Double free.
   Allocate and free memory in the same module, at the same level of 
   abstraction 
*/

#include <stdio.h>
#include <stdlib.h>

enum { MIN_SIZE_ALLOWED = 32 };

int verify_size(char *list, size_t size) {
   if (size < MIN_SIZE_ALLOWED) {
      free(list);                /* remove this to eliminate double-free */
      printf("(1) free %lx\n",(unsigned long)list);
      return -1;
   }
   return 0;
}
 
void process_list(size_t number) {
   int i;
   char *list = (char *)malloc(number);
   if (list == NULL) { /* handle error */  }
   
   if (verify_size(list, number) == -1) {
      free(list);
      printf("(2) free %lx\n",(unsigned long)list);
      return;
   }
   for (i = 0 ; i < 9 ; i++) list[i] = 'A'+i;
   list[9] = 0;
   printf("list=%s\n",list);
   free(list);
   printf("(3) free %lx\n",(unsigned long)list);
}

int main () {
   process_list(10);  /* double-free corruption reported at execution */
   printf("got fooled\n");
}

/* 
  verify_size and process_list show a double-free vulnerability resulting 
  from memory being allocated and freed at differing levels of abstraction. 

  Memory for the list array is allocated in the process_list() function. 
  The array is then passed to the verify_size() function that performs 
  error checking on the size of the list.  If the size of the list is 
  below a minimum size, the memory allocated to the list is freed, and 
  the function returns to the caller. The calling function then frees 
  this same memory again, resulting in a double-free and potentially 
  exploitable vulnerability.
*/
