// Declares clang::SyntaxOnlyAction.
//#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
// Declares llvm::cl::extrahelp.
#include "llvm/Support/CommandLine.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Basic/Diagnostic.h"
//#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"

using namespace clang::tooling;
using namespace llvm;

//// Apply a custom category to all command-line options so that they are the
//// only ones displayed.
//static llvm::cl::OptionCategory MyToolCategory("my-tool options");
//
//// CommonOptionsParser declares HelpMessage with a description of the common
//// command-line options related to the compilation database and input files.
//// It's nice to have this help message in all tools.
//static cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);
//
//// A help message for this specific tool can be added afterwards.
//static cl::extrahelp MoreHelp("\nMore help text...\n");
//
//int main(int argc, const char **argv) {
//  CommonOptionsParser OptionsParser(argc, argv, MyToolCategory);
//  ClangTool Tool(OptionsParser.getCompilations(),
//                 OptionsParser.getSourcePathList());
//  return Tool.run(newFrontendActionFactory<clang::SyntaxOnlyAction>().get());
//}
using namespace clang;
using namespace clang::ast_matchers;



static llvm::cl::OptionCategory MatcherCategory("AST-matcher options");
static llvm::cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);
static llvm::cl::extrahelp MoreHelp("\nMoreHelpText");

struct FunctionDumper : public MatchFinder::MatchCallback {
    virtual void run(const MatchFinder::MatchResult &Result) {
        auto *d = Result.Nodes.getNodeAs<TypeLoc>("stuff");
        SourceLocation locstart = d->getBeginLoc();
        llvm::errs() << "*\n";
        llvm::errs() << " start: " << locstart.printToString(*Result.SourceManager)
                     << "\n";
        SourceLocation locend = d->getEndLoc();
        llvm::errs() << " end: " << locend.printToString(*Result.SourceManager)
                     << "\n";
    }
};

struct VectorHandler : public MatchFinder::MatchCallback {
    virtual void run(const MatchFinder::MatchResult &Result) {
        const VarDecl *lhs = Result.Nodes.getNodeAs<VarDecl>("vector");
        lhs->dump();   // YAY found it!!
    }
};

int main(int argc, const char **argv) {
    CommonOptionsParser OptionsParser(argc, argv, MatcherCategory);
    ClangTool Tool(OptionsParser.getCompilations(),
                   OptionsParser.getSourcePathList());

    MatchFinder Finder;
//    DeclarationMatcher Matcher = functionDecl(
//            hasAnyParameter(hasType(recordDecl(matchesName("std::vector"))));

// Match all explicit casts in the main file (exclude system headers).
//    Finder.addMatcher(
//            cStyleCastExpr(unless(isExpansionInSystemHeader())).bind("cast"), &Alert);

    FunctionDumper HandlerForFunction;
    Finder.addMatcher(functionDecl(),&HandlerForFunction);
    Finder.addMatcher(callExpr(hasParent(compoundStmt()),hasDescendant(implicitCastExpr())).bind("stuff"),&HandlerForFunction);
    Finder.addMatcher(cxxMethodDecl(hasParent(cxxRecordDecl(hasName("vector"))),hasAncestor(namespaceDecl(hasName("std")))).bind("stuff"),&HandlerForFunction);
    Finder.addMatcher(friendDecl(hasParent(cxxRecordDecl(hasName("Box")))).bind("stuff"),&HandlerForFunction);
    Finder.addMatcher(cxxConstructorDecl(hasParent(cxxRecordDecl(hasName("String")))).bind("stuff"),&HandlerForFunction);
    Finder.addMatcher(cxxDestructorDecl(hasParent(cxxRecordDecl(hasName("String")))).bind("stuff"),&HandlerForFunction);

    VectorHandler HandlerForRecords;
    Finder.addMatcher(
            functionDecl(
            hasAnyParameter(hasType(recordDecl(matchesName("std::vector")).bind("vector")))), &HandlerForRecords);

//    Finder.addMatcher(
//            ifStmt(hasCondition(binaryOperator(
//                    hasOperatorName("=="),
//                    hasLHS(ignoringParenImpCasts(declRefExpr(
//                            to(varDecl(hasType(pointsTo(AnyType))).bind("lhs")))))))),
//            &HandlerForIf);

    return Tool.run(newFrontendActionFactory(&Finder).get());
}

