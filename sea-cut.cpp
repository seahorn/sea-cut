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
#include <fstream>

using namespace llvm;
using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::tooling;

static llvm::cl::OptionCategory SeaCutCategory("SeaCut options");
static llvm::cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);
static llvm::cl::extrahelp MoreHelp("\nMoreHelpText");
cl::opt<std::string> NamespaceName("namespace", cl::cat(SeaCutCategory));
cl::opt<std::string> ClassName("class", cl::cat(SeaCutCategory));
cl::opt<std::string> FileName("fileName", cl::cat(SeaCutCategory));

RefactoringCallback::RefactoringCallback() {}
tooling::Replacements &RefactoringCallback::getReplacements() {
  return Replace;
}

class DeleteBodyConsumer : public ASTConsumer {
  std::map<std::string, tooling::Replacements> &m_replacements;
  std::ofstream newFile;

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

  void ExtractMethod(SmallVector<BoundNodes, 1> Matches, ASTContext &Context) {

    llvm::errs() << "New File:\n";

    newFile.open (FileName);
    auto *MD = selectFirst<CXXMethodDecl>("methods", Matches);
    const DeclContext *parent = MD->getParent();

    // Get all namespace;
    unsigned nameSpaceCount = 0;
    while (parent) {
      if (parent->isNamespace()) {
        ++nameSpaceCount;
        auto *namespaceDecl = cast<NamespaceDecl>(parent);
        errs() << "namespace " << namespaceDecl->getName() << " {\n";
        newFile << "namespace " << namespaceDecl->getName().str() << " {\n";
      }
      parent = parent->getParent();
    }

    for (const BoundNodes &N : Matches) {
      auto *MD = N.getNodeAs<CXXMethodDecl>("methods");
      if (!MD)
        return;
      PrintMethod(MD, Context);
    }

    for (unsigned i = 0; i < nameSpaceCount; i++) {
      errs() << "}\n";
      newFile << "}\n";
    }

    newFile.close();
    llvm::errs() << "\n";
  }

  void PrintMethod(const CXXMethodDecl *MD, ASTContext &Context) {
    PrintTemplateDecl(MD, Context);
    MD->getReturnType().print(errs(), PrintingPolicy(LangOptions()));
    errs() << " ";
    PrintClass(MD);
    errs() << MD->getName();
    PrintParameters(MD);
    MD->getBody()->printPretty(errs(), nullptr, PrintingPolicy(LangOptions()));
    if (MD->isConst()) {
      errs() << " const ";
      newFile << " const ";
    }
  }

  void PrintTemplateDecl(const CXXMethodDecl *MD, ASTContext &Context) {
    const DeclContext *methodParent = MD->getParent();

    while (methodParent) {

      if (!methodParent->isRecord()) {
        methodParent = methodParent->getParent();
        continue;
      }

      auto *record = cast<RecordDecl>(methodParent);

      if (record->isTemplated()) {

        auto *templateParameters = cast<RecordDecl>(methodParent)
                                       ->getDescribedTemplate()
                                       ->getTemplateParameters();
        errs() << getTextFromSourceRange(templateParameters->getSourceRange(),
                                         Context)
               << ">\n";
        newFile << getTextFromSourceRange(templateParameters->getSourceRange(),
                                          Context)
                       .str()
                << ">\n";
      }

      methodParent = methodParent->getParent();
    }

  }

  void PrintParameters(const CXXMethodDecl *MD) {
    errs() << "(";
    newFile << "(";
    for (size_t i = 0; i < MD->param_size(); i++) {
      MD->getParamDecl(i)->print(errs());
      if (i < MD->param_size() - 1) {
        errs() << ", ";
        newFile << ", ";
      }
    }
    errs() << ")";
    newFile << ")";
  }

  void PrintClass(const CXXMethodDecl *MD) {
    const DeclContext *methodParent = MD->getParent();
    while (methodParent) {
      if (methodParent->isRecord()) {
        auto *recordDecl = cast<RecordDecl>(methodParent);
        errs() << recordDecl->getName();
        newFile << recordDecl->getName().str();
        PrintTemplate(recordDecl);
        errs() << "::";
        newFile << "::";
      }
      methodParent = methodParent->getParent();
    }
  }

  void PrintTemplate(const RecordDecl *recordDecl) {
    if (!recordDecl->isTemplated()) return;

    TemplateParameterList *templateParameterList =
        recordDecl->getDescribedTemplate()->getTemplateParameters();
    errs() << "<";
    newFile << "<";
    for (unsigned i = 0; i < templateParameterList->size(); i++) {
      errs() << templateParameterList->getParam(i)->getName();
      newFile << templateParameterList->getParam(i)->getName().str();

      templateParameterList->getParam(i) -> getSourceRange();

      if (i < templateParameterList->size() - 1) {
        errs() << ", ";
        newFile << ", ";
      }

    }
    errs() << ">";
    newFile << ">";
  }

  StringRef getTextFromSourceRange(SourceRange R, ASTContext &Context) {
    return Lexer::getSourceText(CharSourceRange::getCharRange(R), Context.getSourceManager(), LangOptions());
  }

  void HandleTranslationUnit(ASTContext &Context) override {
    auto Matches = findDefinition(Context, "methods");
    if (Matches.size() < 1)
      return;
    ExtractMethod(Matches, Context);

    for (const BoundNodes &N : Matches) {
      auto *MD = N.getNodeAs<CXXMethodDecl>("methods");
      if (!MD)
        return;

      llvm::errs() << "Match found:\n";
      MD->dump();

      SourceRange body = MD->getBody()->getSourceRange();
      StringRef newBody = ";";

      tooling::Replacement replacement(Context.getSourceManager(),
                                       CharSourceRange::getTokenRange(body),
                                       newBody, Context.getLangOpts());
      llvm::errs() << "New replacement: \n" << replacement.toString() <<
      "\n";

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

  llvm::errs() << "File after applying replacements:";
  size_t i = 0;

  for (const auto &file : files) {
    llvm::errs() << "\n#" << (i++) << " " << file << "\n";
    const auto *entry = fileMgr.getFile(file);
    const auto ID = sources.getOrCreateFileID(entry, SrcMgr::C_User);
    rewriter.getEditBuffer(ID).write(llvm::outs());
    llvm::outs() << "\n";
  }

  return 0;
}
