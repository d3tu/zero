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
    class Node {
      public:
        T data;
        Node<T> *next;
        Node<T> *prev;

        Node(T data): data(data), next(nullptr), prev(nullptr) {} 
    };

    template <typename T>
    class LinkedList {
      Node<T> *head;
      Node<T> *tail;

      public:
        LinkedList(): head(nullptr), tail(nullptr) {}

        void push_back(T data) {
          auto node(new Node<T>(data));

          if (head == nullptr) {
            head = node;
          } else {
            node->prev = tail;
            tail->next = node;
          }

          tail = node;
        }

        class Iterator {
          Node<T> *curr;

          public:
            Iterator(Node<T> *node = nullptr): curr(node) {}

            Iterator &operator++() {
              if (curr != nullptr) {
                curr = curr->next;
              }

              return *this;
            }

            Iterator operator++(int) {
              Iterator copy(*this);
              ++*this;
              return copy;
            }

            Iterator &operator--() {
              if (curr != nullptr) {
                curr = curr->prev;
              }

              return *this;
            }

            Iterator operator--(int) {
              Iterator copy(*this);
              --*this;
              return copy;
            }

            T &operator*() {
              return curr->data;
            }

            bool operator!=(const Iterator &other) const {
              return curr != other.curr;
            }

            bool operator!() const {
              return curr == nullptr;
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