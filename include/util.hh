#pragma once

#define CONCAT(A, B) A ## B
#define CALL_IMPL_NX(NAME, ID) LOCALS.push(&&ID); goto NAME; ID:
#define CALL_IMPL(NAME, ID) CALL_IMPL_NX(NAME, CONCAT($, ID))
#define CALL(NAME) CALL_IMPL(NAME, __COUNTER__)
#define RET goto *LOCALS.pop();

#define SAVE(PTR) BACKUP.push(PTR);
#define RESTORE(VAR) VAR = *reinterpret_cast<decltype(VAR) *>(BACKUP.pop())

#define PUSH(VALUE) VALUES.push(VALUE);
#define POP VALUES.pop()

namespace DJ {
  namespace Util {
    class Exception {
      protected:
        const char *message;
      
      public:
        Exception(const char *message): message(message) {}

        virtual ~Exception() = default;

        virtual const char *what() const {
          return message;
        }

        Exception(const Exception &) = delete;
        Exception &operator=(const Exception &) = delete;
    };

    template <typename T>
    class LinkedList {
      struct Node {
        T data;
        Node *prev;
        Node *next;
      };

      Node *head = nullptr;
      Node *tail = nullptr;

      public:
        void push(T data) {
          auto node = new Node {
            data, tail, nullptr
          };

          if (!head) head = node;
          else tail->next = node;

          tail = node;
        }

        void pop() {
          if (head) {
            if (head == tail) {
              delete head;
              head = nullptr;
              tail = nullptr;
            } else {
              auto node = tail->prev;
              delete tail;
              tail = node;
              tail->next = nullptr;
            }
          }
        }

        class Iterator {
          Node *curr;

          public:
            Iterator(Node *node): curr(node) {}

            Iterator &operator++() {
              if (curr) {
                curr = curr->next;
              }

              return *this;
            }
            
            Iterator &operator--() {
              if (curr) {
                curr = curr->prev;
              }

              return *this;
            }
            
            T &operator*() {
              return curr->data;
            }

            bool operator!() const {
              return curr == nullptr;
            }

            bool operator!=(const Iterator &other) const {
              return curr != other.curr;
            }
        };

        Iterator begin() {
          return Iterator(head);
        }
        
        Iterator end() {
          return Iterator(nullptr);
        }
    };

    template <typename T> class Stack {
      struct Item {
        T value;
        Item *next;
      };

      Item *top = nullptr;

      public:
        ~Stack() {
          while (top) {
            auto item = top;
            top = item->next;
            delete item;
          }
        }

        bool empty() const {
          return !top;
        }

        void push(T value) {
          top = new Item { value, top };
          if (!top) throw "BadAlloc";
        }

        T pop() {
          if (!top) throw "OutOfRange";
          auto item = top;
          auto value = item->value;
          top = item->next;
          delete item;
          return value;
        }
    };
  }
}