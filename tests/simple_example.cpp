// RUN: %sea-cut %s 2>&1 | grep -v '^//' | OutputCheck %s -d --comment='//'


namespace std {

//  CHECK-L: void foo();
void foo();

// CHECK-NOT-L: void bar(int) ;
void bar(int) {}

template <typename T>
class vector {

// CHECK-L: void push_back(const T&) ;
// CHECK-NOT-L: std::bar(1);
  void push_back(const T&) {
     std::bar(1);
  }
};

class test1 {

// CHECK-L: void push_back(const T&) ;
// CHECK-NOT-L: std::bar(1);
    void push_back(const T&) {
        std::bar(1);
    }
};
}

namespace testnamespace{
    class vector {

// CHECK-L: void push_back(const T&) ;
// CHECK-NOT-L: std::bar(1);
        void push_back(const T&) {
            std::bar(1);
        }
    };
}

