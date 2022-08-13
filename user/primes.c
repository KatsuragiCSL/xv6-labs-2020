#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// handle the numebrs
void primes(int *fd) {
  // the first one comes in is always prime
  int prime;
  int number;

  // close the write end of current pipe
  close(fd[1]);
  if (read(fd[0], (void*) &prime, sizeof(prime)) != sizeof(prime)) {
    fprintf(2, "Cannot read prime %d\n", prime);
    exit(1);
  }
  printf("prime %d\n", prime);
  // for every other numbers coming in, if number is not divisible by prime, pass it to the next pipe.
  if (read(fd[0], (void*) &number, sizeof(number))) {
    int next_pipe[2];
    pipe(next_pipe);

    if (fork() == 0) {
      primes(next_pipe);
    } else {
      // close the read end of the next pipe
      close(next_pipe[0]);
      // keep reading numbers from original pipe and put it in next pipe if not divisible
      do {
        if (number % prime != 0) {
          write(next_pipe[1], (void*) &number, sizeof(number));
        }
      } while (read(fd[0], (void*) &number, sizeof(number)));

      // close the write end of next pipe cuz finished writing
      close(next_pipe[1]);
      // close the read end of original pipe cuz no more numbers to read
      close(fd[0]);

      wait(0);
    }
  }

  exit(0);
}


int
main(int argc, char *argv[])
{
  int initial_pipe[2];
  int start = 2;
  int end = 35;
  pipe(initial_pipe);

  if (fork() == 0) {
    // do primes sieve in child
    primes(initial_pipe);
  } else {
    // pass all number in [2, 35] to child
    close(initial_pipe[0]);

    for (int i = start; i <= end; i++) {
      // write the prime
      if (write(initial_pipe[1], (void*) &i, sizeof(i)) != sizeof(i)) {
        fprintf(2, "Cannot write number %d\n", i);
        exit(1);
      }
    }
    close(initial_pipe[1]);
    wait(0);
  }

  exit(0);
}
