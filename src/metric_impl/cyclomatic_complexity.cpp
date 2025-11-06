#include "metric_impl/cyclomatic_complexity.hpp"
#include <algorithm>
#include <array>
#include <iostream>
#include <ranges>
#include <string>
#include <unistd.h>

namespace analyzer::metric::metric_impl {
std::string CyclomaticComplexityMetric::Name() const { return kName; }
MetricResult::ValueType CyclomaticComplexityMetric::CalculateImpl(const function::Function &f) const {
    // Получаем строковое представление AST (абстрактного синтаксического дерева) функции.
    // Это S-выражение, сгенерированное утилитой tree-sitter, например:
    // "(function_definition name: (identifier) ... (if_statement ...) (for_statement ...))"
    auto &function_ast = f.ast;

    // Список типов узлов AST, каждый из которых увеличивает цикломатическую сложность на 1.
    // Эти узлы соответствуют управляющим конструкциям языка Python:
    // - if / elif
    // - циклы (for, while)
    // - обработка исключений (try, finally)
    // - case в match-выражениях
    // - assert
    // - тернарный оператор (conditional_expression)
    constexpr std::array<std::string_view, 9> complexity_nodes = {
        "if_statement",            // if
        "elif_statement",          // elif
        "for_statement",           // for
        "while_statement",         // while
        "try_statement",           // try
        "finally_clause",          // finally
        "case_clause",             // case
        "assert",                  // assert
        "conditional_expression",  // для тернарного оператора
    };

    // Создаем диапазон с паттернами "(node_type"
    auto patterns = complexity_nodes |
                    std::views::transform([](std::string_view node_type) { return "(" + std::string(node_type); });

    auto total_complexity =
        std::ranges::fold_left(patterns | std::views::transform([&](const std::string &pattern) {
                                   size_t count = 0;
                                   size_t pos = 0;
                                   while ((pos = function_ast.find(pattern, pos)) != std::string::npos) {
                                       ++count;
                                       pos += pattern.length();
                                   }
                                   return count;
                               }),
                               0, std::plus<>());

    return static_cast<MetricResult::ValueType>(total_complexity + 1);
}

}  // namespace analyzer::metric::metric_impl
