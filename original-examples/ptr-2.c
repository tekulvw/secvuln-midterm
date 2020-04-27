/* Use after free - zero out sensitive data after free */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

char *save;

void* attacker(void* args) {
   int i;

   char *message = save;
   printf("attacker (%lx): ", (unsigned long)message); fflush(stdout);
   for (i=-100 ; i < 100 ; i++) {
      if (message[i] <= 'z' && message[i] >= ' ') {
	 printf("%c",message[i]);
	 fflush(stdout);
      } else {
	 printf(".");
	 fflush(stdout);
      }
   }
   printf("\n");
   fflush(stdout);
}

void* victim(void* args) {
   int n = 1000;
   save = (char*)malloc(n);
   getline((char**)&save, (size_t*)&n, stdin);
   printf("victim (%lx): %s\n", (unsigned long)&save, save); fflush(stdout);
   free(save);
   return NULL;
}

int main (int argc, char **argv) {
   pthread_t threads[2];

   pthread_create(&threads[0], NULL, victim, NULL);
   printf("Type something> "); fflush(stdout);
   usleep(15000000);
   pthread_create(&threads[1], NULL, attacker, NULL);
   pthread_exit(NULL);
}

/* run ptr-2 then type in a line of text and hit return */
