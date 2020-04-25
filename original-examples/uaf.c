/* Examples of dangling pointer exploitation:
     CVE-2005-4360 - Microsoft IIS 5.1
     CVE-2010-3275 - VLC
     CVE-2010-0425 - Apache HTTP Server
*/

/* Use: uaf with no argument then aaaaaaaaS after input buffer address */
#include <stdio.h>
#include <stdlib.h>

int main(){
   char* data = (char *)malloc(32); /* The 9th char will be critical */
   data[8] = 'L';

   printf("data buffer at address %lx\n", (unsigned long)data);

   /* some complex program code here  */
   free(data); //the data buffer could be accidentially freed.
   /*  some complex program code here  */
   /* Here, the allocator could allocate the chunk of data buffer for 
      the input buffer.  Whether this will happen depends on multiple 
      factors (allocation algorithm, the size of the buffer, etc.). For 
      example, if you set the input buffer with size 64, this might not 
      happen because the allocator will use a new chunk instead of 
      reusing the old one.  */
   char* input = (char *)malloc(32);
   printf("input buffer at address %lx\n", (unsigned long)input);

   /* Assuming that the attacker can control the input. Now, the attacker can 
      type aaaaaaaaS to write char 'S' to the critical space.  */
   scanf("%s", input); 

   /* The dangling pointer is used here to do a critical operation.
      Note that this bug might be hard to find because it will  
      almost never trigger a crash.  */
   if (data[8] == 'S'){ 
      printf("Bad!\n"); // This is the goal of the attacker
   } else {
      printf("All well and good\n");
   }    
   return 0;
}
