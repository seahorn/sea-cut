// Declares clang::SyntaxOnlyAction.
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
// Declares llvm::cl::extrahelp.
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"

#include "clang/Tooling/Core/Replacement.h"
#include "clang/Tooling/RefactoringCallbacks.h"
using namespace clang::tooling;
using namespace llvm;

//// Apply a custom category to all command-line options so that they are the
//// only ones displayed.
// static llvm::cl::OptionCategory MyToolCategory("my-tool options");
//
//// CommonOptionsParser declares HelpMessage with a description of the common
//// command-line options related to the compilation database and input files.
//// It's nice to have this help message in all tools.
// static cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);
//
//// A help message for this specific tool can be added afterwards.
// static cl::extrahelp MoreHelp("\nMore help text...\n");
//
// int main(int argc, const char **argv) {
//  CommonOptionsParser OptionsParser(argc, argv, MyToolCategory);
//  ClangTool Tool(OptionsParser.getCompilations(),
//                 OptionsParser.getSourcePathList());
//  return Tool.run(newFrontendActionFactory<clang::SyntaxOnlyAction>().get());
//}
using namespace clang;
using namespace clang::ast_matchers;

RefactoringCallback::RefactoringCallback() {}
tooling::Replacements &RefactoringCallback::getReplacements() {
  return Replace;
}

static llvm::cl::OptionCategory MatcherCategory("AST-matcher options");
static llvm::cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);
static llvm::cl::extrahelp MoreHelp("\nMoreHelpText");

struct FunctionDumper : public clang::ast_matchers::MatchFinder::MatchCallback {
  //    void printToken(StringRef token) {
  //        size_t tokenlen = token.size();
  //        if ((tokenlen == 0) || (tokenlen > 128))
  //            return;
  //        llvm::outs() << "\"" + token + "\"" << "\n";
  //    }

  virtual void run(const MatchFinder::MatchResult &Result) {
    //        const FunctionDecl *lhs =
    //        Result.Nodes.getNodeAs<FunctionDecl>("stuff"); const CallExpr *lhs
    //        = Result.Nodes.getNodeAs<CallExpr>("stuff"); lhs->dump(); const
    //        CXXMethodDecl *lhs =
    //        Result.Nodes.getNodeAs<CXXMethodDecl>("stuff"); lhs->dump();   //
    //        YAY found it!!
  }
};

// struct VectorHandler : public MatchFinder::MatchCallback {
//    virtual void run(const MatchFinder::MatchResult &Result) {
//        const VarDecl *lhs = Result.Nodes.getNodeAs<VarDecl>("vector");
//        lhs->dump();   // YAY found it!!
//    }
//};

class stringDumper : public MatchFinder::MatchCallback {
private:
  Replacements *Replace;

public:
  stringDumper(Replacements *Replace) : Replace(Replace) {}

  virtual void run(const MatchFinder::MatchResult &Result) {
    // The matched 'if' statement was bound to 'ifStmt'.
    llvm::outs() << "herrjhjerere"
              << "\n";
    const CXXMethodDecl *lhs = Result.Nodes.getNodeAs<CXXMethodDecl>("stuff");
    lhs->dump();
    llvm::outs() << "herrjhjerere"
              << "\n";

    Replacement Rep(*(Result.SourceManager), lhs->getBeginLoc(), 0,
                    "// the 'if' part\n");
    llvm::outs() << "Rep"
              << "\n";
    Replace->add(Rep);
    //        lhs->dump();
  }
};

int main(int argc, const char **argv) {
  CommonOptionsParser OptionsParser(argc, argv, MatcherCategory);
  static llvm::cl::OptionCategory ToolingSampleCategory("Tooling Sample");
  CommonOptionsParser OptionsParser2(argc, argv, ToolingSampleCategory);

  //    ClangTool Tool(OptionsParser.getCompilations(),
  //                   OptionsParser.getSourcePathList());

  RefactoringTool ReTool(OptionsParser2.getCompilations(),
                         OptionsParser2.getSourcePathList());
  MatchFinder Finder;
  //    DeclarationMatcher Matcher = functionDecl(
  //            hasAnyParameter(hasType(recordDecl(matchesName("std::vector"))));

  // Match all explicit casts in the main file (exclude system headers).
  //    Finder.addMatcher(
  //            cStyleCastExpr(unless(isExpansionInSystemHeader())).bind("cast"),
  //            &Alert);
  //    FunctionDumper HandlerForFunction;
  stringDumper HandlerForFunction2(
      reinterpret_cast<Replacements *>(&ReTool.getReplacements()));
  //    Finder.addMatcher(functionDecl().bind("stuff"),&HandlerForFunction);
  //    Finder.addMatcher(callExpr(hasParent(compoundStmt()),hasDescendant(implicitCastExpr())).bind("stuff"),&HandlerForFunction);
  Finder.addMatcher(cxxMethodDecl(hasParent(cxxRecordDecl(hasName("vector"))),
                                  hasAncestor(namespaceDecl(hasName("std"))))
                        .bind("stuff"),
                    &HandlerForFunction2);
  //    Finder.addMatcher(friendDecl(hasParent(cxxRecordDecl(hasName("Box")))).bind("stuff"),&HandlerForFunction);
  //    Finder.addMatcher(cxxConstructorDecl(hasParent(cxxRecordDecl(hasName("String")))).bind("stuff"),&HandlerForFunction);
  //    Finder.addMatcher(cxxDestructorDecl(hasParent(cxxRecordDecl(hasName("String")))).bind("stuff"),&HandlerForFunction);

  //    VectorHandler HandlerForRecords;
  //    Finder.addMatcher(
  //            functionDecl(
  //            hasAnyParameter(hasType(recordDecl(matchesName("std::vector")).bind("vector")))),
  //            &HandlerForRecords);

  //    Finder.addMatcher(
  //            ifStmt(hasCondition(binaryOperator(
  //                    hasOperatorName("=="),
  //                    hasLHS(ignoringParenImpCasts(declRefExpr(
  //                            to(varDecl(hasType(pointsTo(AnyType))).bind("lhs")))))))),
  //            &HandlerForIf);

  return ReTool.run(newFrontendActionFactory(&Finder).get());
}
