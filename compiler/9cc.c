#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// token types value
enum  {
	TK_NUM = 256, // integer token
	ND_NUM = 256, // integer node type
	TK_EOF,
};

typedef struct {
	int ty; // token type
	int val; // if ty is int, then return the value
	char *input; // token strings(for error message)
} Token;

typedef struct Node {
	int ty; // token type
	struct Node *lhs; // left-hand side
	struct Node *rhs; //right-hand side
	int val; // if ty is int, then return the value
} Node;

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

Node *new_node(int ty, Node *lhs, Node *rhs) {
	Node *node = malloc(sizeof(Node));
	node->ty =ty;
	node->lhs = lhs;
	node->rhs = rhs;
	return node;
}

Node *new_node_num(int val) {
	Node *node =malloc(sizeof(Node));
	node->ty = ND_NUM;
	node->val = val;
	return node;
}

// prototypes
int consume(int ty);
Node *term();
Node *add();
Node *mul();
Node *expr();

int pos = 0;

int consume(int ty) {
	if (tokens[pos].ty != ty)
		return 0;
	pos++;
	return 1;
}

Node *expr() {
	Node *node = mul();

	for (;;) {
		if (consume('+'))
			node = new_node('+', node, mul());
		else if (consume('-'))
			node = new_node('-', node, mul());
		else
			return node;
	}
}

Node *term() {
	if (consume('(')) {
		Node *node =add();
		if (!consume(')'))
			error("not found corresponding parenthesis: %s",
					tokens[pos].input);
		return node;
	}

	if (tokens[pos].ty == TK_NUM)
		return new_node_num(tokens[pos++].val);

	error("token is neither number or open parenthesis: %s",
			tokens[pos].input);
}

Node *mul() {
	Node *node = term();

	for (;;) {
		if (consume('*'))
			node = new_node('*', node, term());
		else if (consume('/'))
			node = new_node('/', node, term());
		else
			return node;
	}
}

Node *add() {
	Node *node = mul();

	for (;;) {
		if (consume('+'))
			node = new_node('+', node, mul());
		else if (consume('-'))
			node = new_node('-', node, mul());
		else
			return node;
	}
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

		if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')') {
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

void gen(Node *node) {
	if(node->ty == ND_NUM) {
		printf("  push %d\n", node->val);
		return;
	}

	gen(node->lhs);
	gen(node->rhs);

	printf("  pop rdi\n");
	printf("  pop rax\n");

	switch (node->ty) {
		case '+':
			printf("  add rax, rdi\n");
			break;
		case '-':
			printf("  sub rax, rdi\n");
			break;
		case '*':
			printf("  mul rdi\n");
			break;
		case '/':
			printf("  mov rdx, 0\n");
			printf("  div rdi\n");
	}

	printf("  push rax\n");
}

int main(int argc, char **argv) {
  if (argc != 2) {
    error("argc must be 2");
    return 1;
  }

  tokenize(argv[1]);
  Node *node = expr();

  // output first half of assembly
  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  // descend AST and generate code
  gen(node);

  printf("  pop rax\n");
  printf("  ret\n");
  return 0;
}
