#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct proc_mode_t {
  int b;
  int e;
  int n;
  int s;
  int t;
  int v;
} proc_mode_t;

enum input_mode_t { FLG, FLE, ERR };

int set_short_flag(char flag, proc_mode_t* mode) {
  int error = 0;
  switch (flag) {
    case 'b':
      mode->b = 1;
      break;
    case 'e':
      mode->e = 1;
      mode->v = 1;
      break;
    case 'E':
      mode->e = 1;
      break;
    case 'n':
      mode->n = 1;
      break;
    case 's':
      mode->s = 1;
      break;
    case 't':
      mode->t = 1;
      mode->v = 1;
      break;
    case 'T':
      mode->t = 1;
      break;
    case 'v':
      mode->v = 1;
      break;
    default:
      error = 1;
      break;
  }
  return error;
}

int set_long_flag(char* flag, proc_mode_t* mode) {
  int error = 0;
  if (!strcmp(flag, "--number-nonblank")) {
    mode->b = 1;
  } else if (!strcmp(flag, "--number")) {
    mode->n = 1;
  } else if (!strcmp(flag, "--squeeze-blank")) {
    mode->s = 1;
  } else {
    error = 1;
  }
  return error;
}

int proc_flag(char* arg, proc_mode_t* proc_mode) {
  int error = 0;
  int len = strlen(arg);
  if (arg[0] == '-') {
    if (arg[1] == '-') {
      error += set_long_flag(arg, proc_mode);
    } else {
      for (int i = 1; i < len; i++) {
        error += set_short_flag(arg[i], proc_mode);
      }
    }
  } else {
    error++;
  }
  return error;
}

int proc_line(char* line, size_t* line_num, size_t* line_len,
              proc_mode_t* proc_mode) {
  int error = 0;
  size_t i = 0;
  if (proc_mode->n && !proc_mode->b) {
    printf("%lu ", *line_num);
    (*line_num)++;
  } else if (proc_mode->b && (*line_len) != 1) {
    printf("%lu ", *line_num);
    (*line_num)++;
  }
  for (; i < (*line_len) - 1; i++) {
    if (proc_mode->t && line[i] == '\t') {
      printf("^I");
    } else {
      printf("%c", line[i]);
    }
  }
  if (proc_mode->e && line[i] == '\n') {
    printf("$");
  }
  if (proc_mode->t && line[i] == '\t') {
    printf("^I");
  } else {
    printf("%c", line[i]);
  }

  return error;
}

int proc_file(char* arg, proc_mode_t* proc_mode) {
  int error = 0;
  FILE* file = fopen(arg, "r");
  if (file == NULL) {
    printf("can't find file: %s", arg);
    error = 1;
  } else {
    char* line = 0;
    size_t line_num = 1;
    size_t line_len = 0;
    size_t file_len = 0;
    int is_empty = 0;
    while (getline(&line, &file_len, file) != -1) {
      line_len = strlen(line);
      // printf("%lu %d | ", line_len, is_empty);
      if (line_len == 1 && !is_empty && proc_mode->s) {
        proc_line(line, &line_num, &line_len, proc_mode);
        is_empty = 1;
      } else if (line_len == 1 && !proc_mode->s) {
        proc_line(line, &line_num, &line_len, proc_mode);
        is_empty = 1;
      } else if (line_len == 1) {
        is_empty = 1;
      } else {
        proc_line(line, &line_num, &line_len, proc_mode);
        is_empty = 0;
      }
    }
  }
  return error;
}

int parse(int argc, char* argv[], proc_mode_t* proc_mode) {
  int error = 0;
  enum input_mode_t inpt_mode = FLG;
  for (int n = 1; n < argc && inpt_mode != ERR;) {
    switch (inpt_mode) {
      case FLG:
        if (!proc_flag(argv[n], proc_mode)) {
          n++;
        } else {
          inpt_mode = FLE;
        }
        break;
      case FLE:
        if (!proc_file(argv[n], proc_mode)) {
          n++;
        } else {
          inpt_mode = ERR;
        }
        break;
      case ERR:
        error = 1;
        break;
    }
  }
  return error;
}

int main(int argc, char* argv[]) {
  proc_mode_t proc_mode = {0, 0, 0, 0, 0, 0};
  parse(argc, argv, &proc_mode);
  // printf("START\n");
  printf("\nb %d\n", proc_mode.b);
  printf("e %d\n", proc_mode.e);
  printf("n %d\n", proc_mode.n);
  printf("s %d\n", proc_mode.s);
  printf("t %d\n", proc_mode.t);
  printf("v %d\n", proc_mode.v);
  // printf("END\n");
  return 0;
}