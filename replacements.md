1. Go to clang-tools-extra
   cd llvm-project/clang-tools-extra

2. Search for RefactoringTool to see how it's used:
   ag RefactoringTool

```
➜  ~/misc/llvm-project/clang-tools-extra git:(master) 
$ ag RefactoringTool                           
clang-change-namespace/tool/ClangChangeNamespace.cpp
105:  tooling::RefactoringTool Tool(OptionsParser.getCompilations(), Files);

clang-reorder-fields/tool/ClangReorderFields.cpp
57:  tooling::RefactoringTool Tool(OP.getCompilations(), Files);

clang-move/tool/ClangMove.cpp
107:  tooling::RefactoringTool Tool(OptionsParser.getCompilations(),
```

3. Look at ClangReorderFields, as this transformations seems to be the simplest one.
   subl clang-reorder-fields/tool/ClangReorderFields.cpp:57

4. Copy the relevant bits from there. This is a good example of how to use their API to make
   the compiler happy. The file itself doesn't explain how to perfororm replacements -- this is
   implemented in the reorder_fields::ReorderFieldsAction class.

5. ag ReorderFieldsAction

```
➜  ~/misc/llvm-project/clang-tools-extra git:(master) 
$ ag ReorderFieldsAction
clang-reorder-fields/tool/ClangReorderFields.cpp
14:#include "../ReorderFieldsAction.h"
59:  reorder_fields::ReorderFieldsAction Action(RecordName, FieldsOrder,

clang-reorder-fields/ReorderFieldsAction.cpp
1://===-- tools/extra/clang-reorder-fields/ReorderFieldsAction.cpp -*- C++ -*-===//
11:/// ReorderFieldsAction::newASTConsumer method
15:#include "ReorderFieldsAction.h"
304:std::unique_ptr<ASTConsumer> ReorderFieldsAction::newASTConsumer() {

clang-reorder-fields/CMakeLists.txt
4:  ReorderFieldsAction.cpp

clang-reorder-fields/ReorderFieldsAction.h
1://===-- tools/extra/clang-reorder-fields/ReorderFieldsAction.h -*- C++ -*-===//
10:/// This file contains the declarations of the ReorderFieldsAction class and
25:class ReorderFieldsAction {
31:  ReorderFieldsAction(
38:  ReorderFieldsAction(const ReorderFieldsAction &) = delete;
39:  ReorderFieldsAction &operator=(const ReorderFieldsAction &) = delete;
```

The class definition is in clang-reorder-fields/ReorderFieldsAction.h -- it should show us what the API is.

6. See what the API is:
   subl clang-reorder-fields/ReorderFieldsAction.h

7. ReorderFieldsAction does not inherit from any other class. Now it's clear that `newFrontendActionFactory` does not only consume match callbacks. The only requirement is that there is a `newASTConsumer` method.

8. To see what `newASTConsumer()` does open the implementation file:
   subl clang-reorder-fields/ReorderFieldsAction.cpp

9. Copy relevant code from there.

