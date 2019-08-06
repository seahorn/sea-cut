// RUN: %sea-cut %s --namespace std --class vector --fileName=%t 2>/dev/null| %cat %t | OutputCheck %s -d --comment="//"

// CHECK-L namespace std {
namespace std {

// CHECK-L template <typename T>
template <typename T>
class vector {

  // CHECK-L: void vector<T>::push_back(){
  void push_back() {

  }

  // CHECK-L: void vector<T>::foo(T a){
  void foo(T a) {

  }

  // CHECK-L: inline int vector<T>::inlineFoo(){
  inline int inlineFoo(){
    return 0;
  };

  const bool constFoo(){
    return false;
  };

  class InnerClass{

    // CHECK-NOT-L: void vector<T>::innerClass::innerClassMethod(){
    int innerClassMethod(){
      return 1;
    }
  };
};
}
