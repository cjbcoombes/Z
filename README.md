For information on the bytecode, see the README in the appropriate project folder. The most up-to-date one (which is not very) is in `Z (Attempt 2)`

# The Language

```c
int
<int>

// Comment
int.a
<int>.a
<int.b>.a

@.f = [int.x, int.y]: int {
    x += y;
    y--;
    return x + y;
}
```