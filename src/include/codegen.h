#pragma once

#include "utils.h"

using NamedValues_t = std::map<std::string, llvm::Value*>; // TODO: Rename

extern std::unique_ptr<llvm::LLVMContext> TheContext;
extern std::unique_ptr<llvm::Module> TheModule;
extern std::unique_ptr<llvm::IRBuilder<>> Builder;
extern std::list<NamedValues_t> NamedValues;

llvm::Value* LogErrorV(const char *str);

llvm::Type* getType(const std::string &typeStr);
