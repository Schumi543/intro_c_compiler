#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// token types value
enum  {
	TK_NUM = 256, // integer token
	TK_EOF,
};

typedef struct {
	int ty; // token type
	int val; // if ty is int, then return the value
	char *input; // token strings(for error message)
} Token;

// save tokenize result sequences in this array
// assume token size <=100
Token tokens[100];

// for report error
// the args of this function is same as "printf"
void error(char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	exit(1);
}

// split strings pointed by *p, and save in tokens
void tokenize(char *p) {
	int i = 0;
	while (*p) {
		// skip whitespace
		if (isspace(*p)) {
			p++;
			continue;
		}

		if (*p == '+' || *p == '-') {
			tokens[i].ty = *p;
			tokens[i].input = p;
			i++;
			p++;
			continue;
		}

		if (isdigit(*p)) {
			tokens[i].ty = TK_NUM;
			tokens[i].input = p;
			tokens[i].val = strtol(p, &p, 10);
			i++;
			continue;
		}
	}

	error("failed to tokenize: %s", p);
	exit(1);
}

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "引数の個数が正しくありません\n");
    return 1;
  }

  char *p = argv[1];

  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");
  printf("  mov rax, %ld\n", strtol(p, &p, 10)
);

  while (*p) {
	  if (*p == '+') {
		  p++;
		  printf("	add rax, %ld\n", strtol(p, &p, 10));
		  continue;
	  }

	  if (*p == '-') {
		  p++;
		  printf("	sub rax, %ld\n", strtol(p, &p, 10));
		  continue;
	  }

	  fprintf(stderr, "予期しない文字列です: '%c'\n", *p);
	  return 1;
  }

  printf("	ret\n");
  return 0;
}
