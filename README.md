# DJ lang, remix your code!

## compile
`g++ -I include -o dj ./main.cc`

## command-line

`./dj --help`

> `./dj build <./main.dj>`

> `./dj run <./main.dj>`

## language-syntax

### types
`boolean: true, false`

`integer: i8, i16, i32, i64`

`unsigned integer: u8, u16, u32, u64`

`float: f32, f64`

`unsigned float: u32, u64`

### include
```
use std // use internal/external library
use .src.util::{ sum } // ./src/util.dj

std::io::printf(sum(1, 2))
```

### variables
```
i8 a = 1 // mutable variable and assignment
i8 a // mutable variable
a = 1 // assignment
const i8 a = 1 // constant variable
const a = 1, b // auto constant variables
let a = 1, b // auto mutable variables
```

### sections
```
i32 i = 0

each:
  if i < 3 {
    goto each
  }
```

### pointers and references
```
i8 n = 1
i8 *p = &n // pointer
i8 &r = n // reference
```

### struct
```
struct Node<T> {
  T data
  Node<T> *next = null, *prev = null
}
```

### class
```
class LinkedList<T> {
  private Node<T> *head, *tail

  () {
    // constructor
  }

  void push(T data) {
    let node = new Node<T> { data }
    if (!head) { head = node }
    else {
      (*node).prev = tail
      (*tail).next = node
    }
    tail = node
  }
}
```

### instance
```
let arr = new i8[5]; // alloc
arr.0 = -1
arr[1] = 1
delete arr.0

LinkedList<i8 *> list
list.push(arr)
delete[] arr
```


### functions
```
i8 sum(i8 a, b) { return a + b }

i8 sum(i8 a, b) return a + b // shorthand function

i8 sum(i8 a, b) // function declaration
sum = (a, b) { // lambda assignment
  return a + b
}
```

### calling function
```
let n = sum(1, 2)
```

### if/else
```
if true { ... } elif { ... } else { ... }
```

### when
```
when value {
  < 3 { ... } // value < 3
  > 1 { ... } // value > 1
  3 { ... } // value == 3
} else { ... } // default

when {
  a > b { ... }
  b > a { ... }
} else { ... } // default
```

### loops
```
// declare; compare; increment
for let i = 1; i <= 10; i++ { continue }
// foreach
for item : iterator { ... }
// while expression
for a > b { ... }
// while true
for { break }
let loop = for { break loop }
goto loop
```