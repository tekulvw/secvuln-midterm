/* null pointer arithmetic */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

char *init_block(size_t block_size, size_t offset,
                 char *data, size_t data_size) {
   char *buffer = (char*)malloc(block_size);  /* no check */
   /* comment this code to see what happens */   
   /****
   if (buffer == NULL) {
      printf("Exiting\n");
      exit(0);
   }
   ****/
   if (data_size > block_size || block_size - data_size < offset) {
      /* Data won't fit in buffer, handle error */
      printf("data won't fit in buffer\n");
      exit(0);
   }
   memcpy(buffer + offset, data, data_size); /* Zowie - if buffer == 0! */
   return buffer;
}

int main () {
   char *data = init_block(1000000000000, 50, "Now is the time", 15);
   printf("%s\n", &data[50]);  /* bytes 0-49 are 0 - Zowie */
}

/* 
   This noncompliant code example is similar to an Adobe Flash Player
   vulnerability that was first exploited in 2008. This code allocates a block
   of memory, and initializes it with some data. The data does not belong at the
   beginning of the block, which is left uninitialized. Instead, it is placed
   offset bytes within the block. The function ensures that the data fits within
   the allocated block.

   This function fails to check if the allocation succeeds, which is a violation
   of ERR33-C. Detect and handle standard library errors. If the allocation
   fails, then malloc() returns a null pointer. The null pointer is added to
   offset and passed as the destination argument to memcpy(). Because a null
   pointer does not point to a valid object, the result of the pointer
   arithmetic is undefined behavior 46.

   An attacker who can supply the arguments to this function can exploit it to
   execute arbitrary code. This can be accomplished by providing an overly large
   value for block_size, which causes malloc() to fail and return a null
   pointer. The offset argument will then serve as the destination address to
   the call to memcpy(). The attacker can specify the data and data_size
   arguments to provide the address and length of the address, respectively,
   that the attacker wishes to write into the memory referenced by offset. The
   overall result is that the call to memcpy() can be exploited by an attacker
   to overwrite an arbitrary memory location with an attacker-supplied address,
   typically resulting in arbitrary code execution.
*/
