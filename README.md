### BGC
A light weight tool that uses libclang to track various entities, as of now heap allocations.

### Build instructions
`cmake . && make`

### Example
```c
// Main.c
void foo(void) {
  char *s = malloc(sizeof(char) * 10));
}

void bar(void) {
  int *x = malloc(sizeof(int));
  int *y = malloc(sizeof(int));
  int *z = y;

  free(y);
}

int main(int argc, char **argv) {
  const char *arg = argv[1];
  const int *ptr  = malloc(sizeof(int));
  free(ptr);

  return 0;
}

```

`./Main Main.c`
```
foo(): 
s
bar(): 
x

```
