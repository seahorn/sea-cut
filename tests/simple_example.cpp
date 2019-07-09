// RUN: %sea-cut %s 2>&1 | grep -v '^//' | OutputCheck %s -d --comment='//'
//1. same namespace, same class, same function number but different parameter
//2. lower/upper function names
//3. different namespace, same class name, same function name and parameters
//4. same namespace, different class, same function name and parameter
//5. function signature of the nested class should not be removed, only the function body
//6. structure body should not be removed
//7. functions in comments are neglected
//8. inline function are neglected

namespace std {


//  CHECK-L: void foo();
void foo();

// CHECK-NOT-L: void bar(int) ;
void bar(int) {}

template <typename T>
class vector {

    class element {
        //  CHECK-L: test(const T&) ;
        void test(const T&) {
            std::bar(1);
        }
    };

    //  CHECK-L: structure remain
    struct Books {
        char  title[50];
        char  author[50];
        char  subject[100];
        int   book_id;
    } book;


    //  CHECK-L: void bar(int) {}
    //void bar(int) {}

    // CHECK-NOT-L: testfunction();
    inline void testfunction(){};

    // CHECK-L: void push_back(const T&) ;
    // CHECK-NOT-L: std::bar(1);
      void push_back(const T&) {
         std::bar(1);
      }

    // CHECK-L: void push_back(int a, int b) ;
        void push_back(int a, int b) {
            std::bar(1);
        }
    // CHECK-L: void push_back();
        void push_back() {
            std::bar(1);
        }

    // CHECK-L: void PUSH_BACK();
        void PUSH_BACK() {
            std::bar(1);
        }
};

class test1 {

// CHECK-L: void push_back(int a) ;
    void push_back(int a) {
        std::bar(1);
    }
};
}

namespace testnamespace{
    class vector {

// CHECK-L: void push_back(int a) ;
        void push_back(int a) {
            std::bar(1);
        }
    };
}

