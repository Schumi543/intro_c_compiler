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
		error("failed to tokenize: %s", p);
		exit(1);
	}

	tokens[i].ty = TK_EOF;
	tokens[i].input = p;
}

int main(int argc, char **argv) {
  if (argc != 2) {
    error("argc must be 2");
    return 1;
  }

  tokenize(argv[1]);

  // output first half of assembly
  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  if (tokens[0].ty != TK_NUM)
	  error("expression must begin with number");
  printf("  mov rax, %d\n", tokens[0].val);

  // consume `<+ num> or <- num> and output assembly`
  int i =1;
  while (tokens[i].ty != TK_EOF) {
	  if (tokens[i].ty == '+') {
		  i++;
		  if (tokens[0].ty != TK_NUM)
			  error("invalid token: %s", tokens[i].input);
		  printf("  add rax, %d\n", tokens[i].val);
		  i++;
		  continue;
	  }

	  if (tokens[i].ty == '-') {
		  i++;
		  if (tokens[0].ty != TK_NUM)
			  error("invalid token: %s", tokens[i].input);
		  printf("  sub rax, %d\n", tokens[i].val);
		  i++;
		  continue;
	  }

	  error("invalid token: %s", tokens[i].input);
  }

  printf("  ret\n");
  return 0;
}
