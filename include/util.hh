#pragma once

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
  }
}