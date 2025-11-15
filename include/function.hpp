#pragma once
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

#include "file.hpp"

namespace analyzer::function {

struct Function {
    std::string filename;
    std::optional<std::string> class_name;
    std::string name;
    std::string ast;
};

struct FunctionExtractor {
    std::vector<Function> Get(const analyzer::file::File &file);

private:
    struct Position {
        size_t line;
        size_t col;
    };

    struct FunctionNameLocation {
        Position start;
        Position end;
        std::string name;
    };

    struct ClassInfo {
        std::string name;
        Position start;
        Position end;
    };

    FunctionNameLocation GetNameLocation(const std::string &function_ast);
    std::string GetNameFromSource(const std::string &function_ast, const std::vector<std::string> &lines);
    std::optional<ClassInfo> FindEnclosingClass(const std::string &ast, const FunctionNameLocation &func_loc);
    std::string GetClassNameFromSource(const ClassInfo &class_info, const std::vector<std::string> &lines);
};

}  // namespace analyzer::function
