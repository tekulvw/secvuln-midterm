/* pointer de-references past flexible array member */
#include <stdlib.h>
#include <stdio.h>

struct S {
   size_t len;
   char buf[];  /* Flexible array member */
};

const char *find(const struct S *s, int c) {
   const char *first = s->buf;
   const char *last  = s->buf + s->len;
   while (first++ != last) { /* Undefined behavior */
      if (*first == (unsigned char)c) return first;
   }
   return NULL;
}
 
void g(void) {
   struct S *s = (struct S *)malloc(sizeof(struct S));
   if (s == NULL) { /* Handle error */  }
   s->len = 0;
   printf("location: %lx\n",(unsigned long)find(s, 'a'));
}

int main () {
   g();
}

/* 
   The function find() attempts to iterate over the elements of the flexible 
   array member buf, starting with the second element. However, because 
   function g() does not allocate any storage for the member, the expression 
   first++ in find() attempts to form a pointer just past the end of buf 
   when there are no elements. This attempt is undefined behavior 62 
*/
