#define CONCAT(A, B) A ## B
#define CALL_IMPL_NX(NAME, ID) LOCALS.push(&&ID); goto NAME; ID:
#define CALL_IMPL(NAME, ID) CALL_IMPL_NX(NAME, CONCAT($, ID))
#define CALL(NAME) CALL_IMPL(NAME, __COUNTER__)
#define RET goto *LOCALS.pop();

#define SAVE(PTR) BACKUP.push(PTR);
#define RESTORE(VAR) VAR = *reinterpret_cast<decltype(VAR) *>(BACKUP.pop())

#define PUSH(VALUE) VALUES.push(VALUE);
#define POP VALUES.pop()