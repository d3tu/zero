# **DJ Lang, remix your code!**

## \[Status]: rewriting

```
[~] cli
[x] lexer
[x] parser
[!] code-gen (bytecode)
[!] runtime
[~] embedder
[~] plugins
```

## Building
`make`

## Usage
`dj --help`

## Syntax

### Types
`bool` `char` `int` `float`

### Declare/assign
```
bool a = true
bool b
b = a
```

### If/else
```
if a == b {
  // ...
} else {
  // ...
}
```

### Methods
```
int sum(int a, int b) {
  return a + b
}
```

### Call
```
int c = sum(1, 2)
```

## Others

```
[x] type name = ...
[x] type name
[+] if ... { ... } else { ... }
[x] label:
[+] type name(...) { ... }
[+] while ... { ... }
[x] name = ...
[+] name(...)
[x] return ...
[+] continue
[x] break
[-] jump ...
```

```
while -> for
  for { ... }
  for a : b { ... }
  for expr { ... }
  for a; b; c { ... }
```

```
jump -> continue ...
```

```
[+] const type name = ...
[+] const name = ...
[+] let name = ...
```

```
[+] if {
[|]   ... { ... }
[|]   ... { ... }
[|] } else { ... }

[~] if bool {
[|]   ...
[|] } else {
[|]   ...
[|] } 

[+] if ... {
[|]   ... { ... }
[|]   ... { ... }
[|] } else { ... }
```

```
bitwise
  & = and
  | = or
  ^ = xor
  ~ = complement
  << = shift left
  >> = shift right
```

```
ptr & ref
```

```
// comment
```

```
list[]
```

```
struct name {}
```

```
class name {}
```

> Coming soon...  

> Spoiler: new name and logo