
#define OP_PLUS  0
#define OP_MINUS 1
#define OP_MUL   2
#define OP_DIV   3
#define OP_AND   4
#define OP_OR    5

int calculate(int a, int b, int op) {
  if (op == OP_PLUS)
    return a + b;
  if (op == OP_MINUS)
    return a - b;
  if (op == OP_MUL)
    return a * b;
  if (op == OP_DIV)
    return a / b;
  if (op == OP_AND)
    return a & b;
  if (op == OP_OR)
    return a | b;
  return a;
}

int main() {
  volatile int a = calculate(12, 23, OP_PLUS);
  volatile int b = calculate(14, 4, OP_MINUS);
  volatile int c = calculate(a, b, OP_AND);
  volatile int d = calculate(a, 66, OP_OR);
  return d;
}