#pragma once

namespace DJ {
  namespace Util {
    class Exception {
      protected: const char *message;
      public:
        Exception(const char *message): message(message) {}
        virtual ~Exception() = default;
        virtual const char *what() const { return message; }
        Exception(const Exception &) = delete;
        Exception &operator=(const Exception &) = delete;
    };

    template <typename T> struct Node {
      T data;
      Node<T> *next, *prev;
    };

    template <typename T> class LinkedList {
      Node<T> *head = nullptr;
      Node<T> *tail = nullptr;

      public:
        void push_back(T data) {
          auto node = new Node<T> { data, nullptr, nullptr };

          if (!head) head = node;
          else {
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
              if (curr) curr = curr->next;
              return *this;
            }

            Iterator operator++(int) {
              Iterator copy(*this);
              if (curr) curr = curr->next;
              return copy;
            }

            Iterator &operator--() {
              if (curr) curr = curr->prev;
              return *this;
            }

            Iterator operator--(int) {
              Iterator copy(*this);
              if (curr) curr = curr->prev;
              return copy;
            }

            T &operator*() { return curr->data; }
            bool operator!=(const Iterator &other) const { return curr != other.curr; }
            bool operator!() const { return !curr; }
        };

        Iterator begin() { return Iterator(head); }
        Iterator end() { return Iterator(nullptr); }
    };
  }
}