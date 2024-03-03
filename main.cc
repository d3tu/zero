int main(int argc, char **argv) {
  return 0;
}















// type name = ...
// type name

// {
//   op: +,
//   left: {
//     op: +,
//     left: 0,
//     right: 1
//   },
//   right: 2
// }

// mov rax, 0
// add rax, 1
// add rax, 2

// {
//   op: &&,
//   left: {
//     op: >,
//     left: 1,
//     right: 0
//   }, 
//   right: {
//     op: <,
//     left: 0,
//     right: 1
//   }
// }

// ; if
// cmp 1, 0
// jle else
// cmp 0, 1
// jg else
// ; ...
// jmp end
// else:
// ; ...
// end:

// expr op, l, r
//
// cmp l, r
// j!op end

// if ... { ... } else { ... }
//
// cmp a, b
// jne else
// ...
// jmp end
// else:
// ...
// end:

// name:
//
// name:

// type name(...) { ... }
//
// name:
//   ...
//   ret

// for { ... }
//
// forID:
//   ...
//   jmp forID

// for ... { ... }
//
// forID:
//   cmp a, b
//   jne endID
//   ...
//   jmp forID
// endID:

// name = ...
//
// mov name, value

// name(...)
//
// mov a, 0
// mov b, 0
// call name

// return
//
// ret

// return ...
//
// mov %0, value
// ret

// continue
//
// jmp forID

// continue ...
//
// jmp name

// break
//
// jmp endID