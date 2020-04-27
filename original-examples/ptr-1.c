/* Always set a freed pointer to NULL */
/* Example: attacker can set up a buffer for victim to walk into */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void doSomethingWithMessage(char* message) {
   int i;
   printf("attacker gets: ");
   for (i=0 ; i < 60 ; i++) {
      if (message[i] <= 'z' && message[i] >= ' ')
	 printf("%c",message[i]);
      else
	 printf("x%x",message[i]);
   }
   printf("\n");
}

int main (int argc, char **argv) {
   if (argc != 2) {
      printf("Usage: ptr-1 <message-string>\n");
      exit(0);
   }
   char* message = NULL;
   char wise_guy[] = "        In case you are attacking - too bad";
   int message_type = 1;
   int message_len = strlen(argv[1]);

   message = (char*)malloc(strlen(argv[1])+1);
   strncpy(message, argv[1], strlen(argv[1]));
   printf("victim message:%s\n",message);
   free(message);
   /* message = NULL;*/  /* attack does not succeed - seg fault occurs */
   /* message = wise_guy; */   /* this will stop the attacker too */

   /* lots of stuff */
   /* attacker reads 52 bytes of victim's data */
   doSomethingWithMessage(message);
}
