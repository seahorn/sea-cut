#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Frontend/TextDiagnosticPrinter.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Core/Replacement.h"
#include "clang/Tooling/RefactoringCallbacks.h"
#include "clang/Tooling/Tooling.h"

#include "llvm/Support/CommandLine.h"

using namespace llvm;
using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::tooling;

static llvm::cl::OptionCategory SeaCutCategory("SeaCut options");
static llvm::cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);
static llvm::cl::extrahelp MoreHelp("\nMoreHelpText");
cl::opt<std::string> NamespaceName("namespace", cl::cat(SeaCutCategory));
cl::opt<std::string> ClassName("class", cl::cat(SeaCutCategory));



RefactoringCallback::RefactoringCallback() {}
tooling::Replacements &RefactoringCallback::getReplacements() {
  return Replace;
}

class DeleteBodyConsumer : public ASTConsumer {
  std::map<std::string, tooling::Replacements> &m_replacements;

public:
  DeleteBodyConsumer(std::map<std::string, tooling::Replacements> &replacements)
      : m_replacements(replacements) {}

  DeleteBodyConsumer(const DeleteBodyConsumer &) = delete;
  DeleteBodyConsumer &operator=(const DeleteBodyConsumer &) = delete;

  static const SmallVector<BoundNodes, 1> findDefinition(ASTContext &Context,
                                             StringRef BindName) {

      StringRef parentName;
      StringRef AncestorName;
      parentName = ClassName;
      AncestorName = NamespaceName;

      SmallVector<BoundNodes, 1> Results =
        match(cxxMethodDecl(hasParent(cxxRecordDecl(hasName(parentName))),
                            hasAncestor(namespaceDecl(hasName(AncestorName))))
                  .bind(BindName),
              Context);

    return Results;
  }

  void ExtractMethod(SmallVector<BoundNodes, 1> Matches){
      const CXXMethodDecl* MD = selectFirst<CXXMethodDecl>("stuff", Matches);
      const DeclContext *parent = MD->getParent();

      //Get all namespace;
      int nameSpaceCount = 0;
      while (parent) {
          if (parent->isNamespace()) {
              nameSpaceCount++;
              const NamespaceDecl *namespaceDecl = cast<NamespaceDecl>(parent);
              llvm::errs() << "namespace " << namespaceDecl->getName() << " {\n";
          }
          parent = parent->getParent();
      }


      for (const BoundNodes &N : Matches) {
          const CXXMethodDecl *MD = N.getNodeAs<CXXMethodDecl>("stuff");
          if (!MD) return;
          PrintMethod(MD, nameSpaceCount);
      }

      for (int i = 0; i < nameSpaceCount; i++) {
          llvm::errs() << "}\n";
      }
  }

  void PrintMethod(const CXXMethodDecl *MD, int nameSpaceCount) {

      const DeclContext *methodParent = MD->getParent();
      while (methodParent) {
          if (methodParent->isRecord()) {
              const RecordDecl* recordDecl = cast<RecordDecl>(methodParent);
              llvm::errs() << recordDecl->getName() << "::";
          }
          methodParent = methodParent->getParent();
      }

      MD->print(llvm::errs(), nameSpaceCount, false);
  }

  void HandleTranslationUnit(ASTContext &Context) override {
    auto Matches = findDefinition(Context, "stuff");
    if (Matches.size() < 1) return;
      ExtractMethod(Matches);

      for (const BoundNodes &N : Matches) {
          const CXXMethodDecl *MD = N.getNodeAs<CXXMethodDecl>("stuff");
          if (!MD)
              return;

          //llvm::errs() << "Match found:\n";
          //MD->dump();

          SourceRange body = MD->getBody()->getSourceRange();
          StringRef newBody = ";";

          tooling::Replacement replacement(Context.getSourceManager(),
                                           CharSourceRange::getTokenRange(body),
                                           newBody, Context.getLangOpts());
          //llvm::errs() << "New replacement: \n" << replacement.toString() << "\n";

          consumeError(m_replacements[replacement.getFilePath()].add(replacement));
      }

  }
};

class DeleteBodyAction {
  std::map<std::string, tooling::Replacements> &m_replacements;

public:
  DeleteBodyAction(std::map<std::string, tooling::Replacements> &replacements)
      : m_replacements(replacements) {}

  DeleteBodyAction(const DeleteBodyAction &) = delete;
  DeleteBodyAction &operator=(const DeleteBodyAction &) = delete;

  std::unique_ptr<ASTConsumer> newASTConsumer() {
    return llvm::make_unique<DeleteBodyConsumer>(m_replacements);
  }
};

int main(int argc, const char **argv) {
  CommonOptionsParser parser(argc, argv, SeaCutCategory);

  auto files = parser.getSourcePathList();
  RefactoringTool reTool(parser.getCompilations(), parser.getSourcePathList());

  DeleteBodyAction action(reTool.getReplacements());

  auto factory = tooling::newFrontendActionFactory(&action);

  if (int result = reTool.run(factory.get()))
    return result;

  auto &fileMgr = reTool.getFiles();
  IntrusiveRefCntPtr<DiagnosticOptions> DiagOpts(new DiagnosticOptions());
  clang::TextDiagnosticPrinter DiagnosticPrinter(llvm::errs(), &*DiagOpts);
  DiagnosticsEngine Diagnostics(
      IntrusiveRefCntPtr<DiagnosticIDs>(new DiagnosticIDs()), &*DiagOpts,
      &DiagnosticPrinter, false);
  SourceManager sources(Diagnostics, fileMgr);
  Rewriter rewriter(sources, LangOptions());
  reTool.applyAllReplacements(rewriter);

  //llvm::errs() << "File after applying replacements:";
  size_t i = 0;

  /*
  for (const auto &file : files) {
    llvm::errs() << "\n#" << (i++) << " " << file << "\n";
    const auto *entry = fileMgr.getFile(file);
    const auto ID = sources.getOrCreateFileID(entry, SrcMgr::C_User);
    rewriter.getEditBuffer(ID).write(llvm::outs());
    llvm::outs() << "\n";
  }
   */

  return 0;
}
