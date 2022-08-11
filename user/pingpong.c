#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int pp[2];
  int buf[2];
  char *ping = "x";
  char *pong = "y";

  pipe(pp);

  if (fork() == 0) {
    // child process
    if (read(pp[0], buf, 1) != 1) {
      fprintf(2, "Cannot receive ping.\n");
      exit(1);
    } else {
      close(pp[0]);
      printf("%d: received ping\n", getpid());

      if (write(pp[1], pong, 1) != 1) {
        fprintf(2, "Cannot send pong.\n");
      } else {
        close(pp[1]);
        exit(0);
      }
    }
  } else {
    // parent process
    if (write(pp[1], ping, 1) != 1) {
      fprintf(2, "Cannot send ping.\n");
      exit(1);
    } else {
      close(pp[1]);
      
      wait(0);

      if (read(pp[0], buf, 1) != 1) {
        fprintf(2, "Cannot receive pong.\n");
        exit(1);
      } else {
        close(pp[0]);
        printf("%d: received pong\n", getpid());
        exit(0);
      }
    }
  }
  exit(0);
}
