/* Forming out-of-bounds pointer */
#include <stdio.h>

int TABLESIZE = 100;

int *f(unsigned long table, long index) {
   if (index < TABLESIZE) {  /* should have tested index >= 0, now sunk */
      return (int*)(table+index*sizeof(int));
   }
   return NULL;
}

int main () {
   long i;
   int table[TABLESIZE];
   for (i=0 ; i < TABLESIZE ; i++) table[i] = 2*i+1;
   for (i=0 ; i < TABLESIZE ; i++) printf("%d ", *(f((unsigned long)table, i)));
   printf("\n---------------------------------\n");
   for (i=0 ; i > -10 ; i--) {
      printf("%d ", *(f((unsigned long)table, i)));
      fflush(stdout);
   }
   printf("\n");
}
 
