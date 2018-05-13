#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <err.h>

bool shebang_space(char ch) {
  return ch == ' ' || ch == '\t';
}

bool shebang_eol(char ch) {
  return ch == '\n' || ch == '\0';
}

int main(int argc, char *argv[], char *envp[]) {
  if(argc <= 1) {
    fprintf(stderr, "bomshebang: no argument\n");
    return 1;
  }
  FILE *script = fopen(argv[1], "rb");
  if(!script) {
    err(1, "bomshebang: %s", argv[1]);
  }
  size_t bufcap = 1024, buflen = 0;
  char *buf = malloc(bufcap + 1); // +1: workaround to append '\0'
  if(!buf) {
    err(1, "bomshebang: malloc");
  }
  bool done = false;
  while(!done) {
    if(buflen >= bufcap) {
      bufcap += bufcap >> 1;
      buf = realloc(buf, bufcap + 1); // +1: workaround to append '\0'
      if(!buf) {
        err(1, "bomshebang: realloc");
      }
    }
    size_t lastpos = buflen;
    size_t numread = fread(buf + buflen, 1, bufcap - buflen, script);
    buflen += numread;
    if(numread == 0 && ferror(script)) {
      err(1, "bomshebang: %s", argv[1]);
    }
    if(numread < bufcap - buflen) {
      done = true;
      break;
    }
    if(buflen >= 5 && memcmp(buf, "\xEF\xBB\xBF#!", 5) && memcmp(buf, "#!", 2)) {
      done = true;
      break;
    }
    for(size_t i = lastpos; i < buflen; ++i) {
      if(shebang_eol(buf[i])) {
        done = true;
        break;
      }
    }
  }

  size_t eol = 0;
  while(eol < buflen && !shebang_eol(buf[eol])) ++eol;

  size_t execpos = eol;
  if(eol >= 5 && !memcmp(buf, "\xEF\xBB\xBF#!", 5)) {
    execpos = 5;
  } else if(eol >= 2 && !memcmp(buf, "#!", 2)) {
    execpos = 2;
  }
  while(execpos < eol && shebang_space(buf[execpos])) ++execpos;
  size_t execend = execpos;
  while(execend < eol && !shebang_space(buf[execend])) ++execend;
  size_t argpos = execend;
  while(argpos < eol && shebang_space(buf[argpos])) ++argpos;
  size_t argend = eol;
  while(argend > argpos && shebang_space(buf[argend - 1])) --argend;

  // fprintf(stderr, "buf = (maybe broken) %s\n", buf);
  // fprintf(stderr, "buflen = %zu, eol = %zu\n", buflen, eol);
  // fprintf(stderr, "execrange = %zu - %zu\n", execpos, execend);
  // fprintf(stderr, "argrange = %zu - %zu\n", argpos, argend);

  char *execpath, *arg;
  if(execpos == eol) {
    execpath = "/bin/sh";
    arg = NULL;
  } else {
    buf[execend] = '\0';
    execpath = buf + execpos;
    if(argpos == argend) {
      arg = NULL;
    } else {
      buf[argend] = '\0';
      arg = buf + argpos;
    }
  }
  // fprintf(stderr, "execpath = %s\n", execpath);
  // fprintf(stderr, "arg = %s\n", arg);
  char *exec_argv[4] = { execpath };
  if(arg) {
    exec_argv[1] = arg;
    exec_argv[2] = argv[1];
    exec_argv[3] = NULL;
  } else {
    exec_argv[1] = argv[1];
    exec_argv[2] = NULL;
  }
  execve(execpath, exec_argv, envp);
  err(1, "bomshebang: %s", execpath);
}
