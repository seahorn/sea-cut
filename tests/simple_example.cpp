// RUN: %sea-cut %s --namespace std --class vector  2>/dev/null | grep -v '^//' | OutputCheck %s -d --comment='//'

// 1. In std: same namespace, same class, same function name but different parameter
// 2. lower/upper function names
// 3. different namespace (i.e., not std), same class name, same function name and parameters
// 4. same namespace, different class (i.e., not vector), same function name and parameter
// 5. function signature of the nested class should not be removed, only the function body
// 6. structure body should not be removed
// 7. functions in comments are not affected
// 8. inline function are handled
// 9. nested namespace are handled
// 10.multiple templates are handled

namespace outer {
namespace std {

//  CHECK-L: void foo();
void foo();

// CHECK-NOT-L: void bar(int) ;
void bar(int) {}

template <typename T, class A> class vector {

  class element {
    //  CHECK-L: test(const T&) ;
    void test(const T &) { outer::std::bar(1); }
  };

  //  CHECK-L: structure remain
  struct Books {
    char title[50];
    char author[50];
    char subject[100];
    int book_id;
  } book;

  //  CHECK-L: void bar(int) {}
  // void bar(int) {}

  // CHECK-L: testfunction();
  inline void testfunction(){};

  // CHECK-L: void push_back(const T&) ;
  void push_back(const T &) { outer::std::bar(1); }

  // CHECK-L: void push_back(int a, int b) ;
  void push_back(int a, int b) { outer::std::bar(1); }
  // CHECK-L: void push_back();
  void push_back() { outer::std::bar(1); }

  // CHECK-L: void PUSH_BACK();
  void PUSH_BACK() { outer::std::bar(1); }
};

class test1 {

  // CHECK-L: void push_back(int a) {
  void push_back(int a) { outer::std::bar(1); }
};
}
}

namespace testnamespace{
class vector {

// CHECK-L: void push_back(int a) {
  void push_back(int a) {
    outer::std::bar(1);
  }
};
}
