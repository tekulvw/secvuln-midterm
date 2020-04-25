/* Use after free - set pointer to null after free */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

char *msg;

void* attacker(void* args) {
   int i;

   char *message = msg;
   printf("attacker (%lx): ",(unsigned long)message); fflush(stdout);
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
   msg = (char*)malloc(n);
   getline((char**)&msg, (size_t*)&n, stdin);
   printf("victim (%lx): %s\n",(unsigned long)&msg,msg); fflush(stdout);
   free(msg);
   return NULL;
}

int main (int argc, char **argv) {
   pthread_t threads[2];

   pthread_create(&threads[0], NULL, victim, NULL);
   printf("type something> "); fflush(stdout); 
   usleep(15000000);
   pthread_create(&threads[1], NULL, attacker, NULL);
   pthread_exit(NULL);
}
