#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

void copyArg(char **buf, char *arg) {
  *buf = malloc(strlen(arg) + 1);
  strcpy(*buf, arg);
}

// put arguments from stdin to params, start from index. returns the end of the params
// after filling in new arguments (will be filled with zero).
int readOneLine(char **params, int pos_param) {
  const int LIMIT = 1024;
  char buf[LIMIT];
  int end = 0;

  // read 1 char at a time until seeing a newline
  while (read(0, buf + end, 1) > 0) {
    if (buf[end] == '\n') {
      buf[end] = 0; // null terminated string
      break;
    }

    end++;
    // check input length
    if (end == LIMIT) {
      fprintf(2, "Input length exceeded\n");
      exit(1);
    }
  }

  // no input
  if (end == 0) {
    fprintf(2, "No input in this line\n");
    return -1;
  }

  // seperate arguments by spaces. ignore the spaces
  int cur = 0;
  while (cur < end) {
    // if it is space, ignore, until non-space char, save it as starting postition
    // if it is space again, set buf[cur] as 0 (null terminated string) and copy the string to params
    while (cur < end && buf[cur] == ' ') {
      cur++;
    }

    int start = cur;

    while (cur < end && buf[cur] != ' ') {
      cur++;
    }

    buf[cur++] = 0;
    copyArg(&params[pos_param], buf + start);
    pos_param++;
  }

  return pos_param;
}

int
main(int argc, char *argv[])
{
  char *params[MAXARG];
  // save all args to args[]
  for (int i = 1; i < argc; i++) {
    copyArg(&params[i - 1], argv[i]);
  }
  // read from stdin, 1 newline = 1 new args
  int end;
   while ((end = readOneLine(params, argc - 1)) != -1) {
     params[end] = 0;
     if (fork() == 0) {
       // params[0] is the command. e.g. | xargs grep [arguments for grep]
       exec(params[0], params);
       exit(0);
     } else {
       wait(0);
     }
   }

   exit(0);
}
