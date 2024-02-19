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