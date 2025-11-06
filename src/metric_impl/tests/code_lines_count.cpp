#include "metric_impl/code_lines_count.hpp"
#include "file.hpp"
#include "function.hpp"
#include <gtest/gtest.h>

namespace analyzer::metric::metric_impl {

TEST(CodeLinesCountTest, SimpleFunction) {
    CodeLinesCountMetric metric;

    analyzer::file::File file(std::string(TEST_FILES_DIR) + "/simple.py");
    analyzer::function::FunctionExtractor extractor;
    auto functions = extractor.Get(file);

    // Находим тестовую функцию
    auto func_it = std::ranges::find_if(functions, [](const auto &f) { return f.name == "test_simple"; });

    ASSERT_NE(func_it, functions.end());
    auto result = metric.Calculate(*func_it);

    // Ожидаем 5 строк кода в test_simple
    EXPECT_EQ(std::get<int>(result.value), 5);
}

TEST(CodeLinesCountTest, ManyLinesFunction) {
    CodeLinesCountMetric metric;

    analyzer::file::File file(std::string(TEST_FILES_DIR) + "/many_lines.py");
    analyzer::function::FunctionExtractor extractor;
    auto functions = extractor.Get(file);

    auto func_it = std::ranges::find_if(functions, [](const auto &f) { return f.name == "testmultiline"; });

    ASSERT_NE(func_it, functions.end());
    auto result = metric.Calculate(*func_it);

    // Ожидаем 11 строк кода в testmultiline
    EXPECT_EQ(std::get<int>(result.value), 11);
}

TEST(CodeLinesCountTest, IgnoresComments) {
    CodeLinesCountMetric metric;

    analyzer::file::File file(std::string(TEST_FILES_DIR) + "/comments.py");
    analyzer::function::FunctionExtractor extractor;
    auto functions = extractor.Get(file);

    auto func_it = std::ranges::find_if(functions, [](const auto &f) { return f.name == "Func_comments"; });

    ASSERT_NE(func_it, functions.end());
    auto result = metric.Calculate(*func_it);

    // 3 строки кода (a=10, b=20, result=a+b), комментарии игнорируются
    EXPECT_EQ(std::get<int>(result.value), 3);
}

}  // namespace analyzer::metric::metric_impl