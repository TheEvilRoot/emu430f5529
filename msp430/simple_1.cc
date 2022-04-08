
int main() {
  const int a = 3;
  const int b = 2;

  register int *result = (int*)(0x200);

  int c = a + b;
  result[0] = c == 5;
  int d = a - b;
  result[1] = d == 1;
  int e = a * b;
  result[2] = e == 6;
  int o = a / b;
  result[3] = o == 1;
  int g = a & b;
  result[4] = g == 2;
  int h = a | b;
  result[5] = h == 3;
  int j = a ^ b;
  result[6] = j == 1;
  int f = a << b;
  result[7] = f == 12;
  int k = a >> b;
  result[8] = k == 0;
}