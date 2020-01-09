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
}

int main(int argc, char **argv) {
  const char *arg = argv[1];

  return 0;
}
```

`./Main Main.c`
```
bar(): 
y
x
foo(): 
s
```
