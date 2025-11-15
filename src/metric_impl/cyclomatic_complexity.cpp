#include "metric_impl/cyclomatic_complexity.hpp"
#include <algorithm>
#include <array>
#include <ranges>
#include <string>
#include <string_view>
#include <unistd.h>

namespace analyzer::metric::metric_impl {
std::string CyclomaticComplexityMetric::Name() const { return kName; }
MetricResult::ValueType CyclomaticComplexityMetric::CalculateImpl(const function::Function &f) const {
    // Получаем строковое представление AST (абстрактного синтаксического дерева) функции.
    // Это S-выражение, сгенерированное утилитой tree-sitter, например:
    // "(function_definition name: (identifier) ... (if_statement ...) (for_statement ...))"
    std::string_view ast = f.ast;

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

    auto count_node_occurrences = [ast](std::string_view node) -> size_t {
        return std::ranges::distance(std::views::iota(size_t{0}, ast.size() - node.size()) |
                                     std::views::filter([ast, node](size_t i) {
                                         return ast[i] == '(' && ast.substr(i + 1, node.size()) == node;
                                     }));
    };

    auto total = std::ranges::fold_left(complexity_nodes | std::views::transform(count_node_occurrences), size_t{0},
                                        std::plus{});

    return static_cast<int>(total + 1);
}

}  // namespace analyzer::metric::metric_impl
