static int a = 42;
const int b = 1;

int func(int *ptr) {
  int s = 0;
  static int r[10];

  if (s == 0) {
    r[0] = *ptr + a;
    a++;
  } else {
    r[0] = *ptr + b;
    s = 1;
  }
  return r[0];
}
