#include "metric_impl/cyclomatic_complexity.hpp"
#include "file.hpp"
#include "function.hpp"
#include <gtest/gtest.h>

namespace analyzer::metric::metric_impl {

TEST(CyclomaticComplexityTest, SimpleFunction) {
    CyclomaticComplexityMetric metric;

    analyzer::file::File file(std::string(TEST_FILES_DIR) + "/simple.py");
    analyzer::function::FunctionExtractor extractor;
    auto functions = extractor.Get(file);

    auto func_it = std::ranges::find_if(functions, [](const auto &f) { return f.name == "test_simple"; });

    ASSERT_NE(func_it, functions.end());
    auto result = metric.Calculate(*func_it);
    EXPECT_EQ(result.value, 2);  // 1 (базовая) + 1 (assert)
}

TEST(CyclomaticComplexityTest, IfStatement) {
    CyclomaticComplexityMetric metric;

    analyzer::file::File file(std::string(TEST_FILES_DIR) + "/if.py");
    analyzer::function::FunctionExtractor extractor;
    auto functions = extractor.Get(file);

    auto func_it = std::ranges::find_if(functions, [](const auto &f) { return f.name == "testIf"; });

    ASSERT_NE(func_it, functions.end());
    auto result = metric.Calculate(*func_it);
    EXPECT_EQ(result.value, 2);  // 1 (базовая) + 1 (if)
}

TEST(CyclomaticComplexityTest, NestedIf) {
    CyclomaticComplexityMetric metric;

    analyzer::file::File file(std::string(TEST_FILES_DIR) + "/nested_if.py");
    analyzer::function::FunctionExtractor extractor;
    auto functions = extractor.Get(file);

    auto func_it = std::ranges::find_if(functions, [](const auto &f) { return f.name == "Testnestedif"; });

    ASSERT_NE(func_it, functions.end());
    auto result = metric.Calculate(*func_it);
    EXPECT_EQ(result.value, 4);  // 1 + 1 (if) + 1 (вложенный if) + 1 (elif)
}

TEST(CyclomaticComplexityTest, Loops) {
    CyclomaticComplexityMetric metric;

    analyzer::file::File file(std::string(TEST_FILES_DIR) + "/loops.py");
    analyzer::function::FunctionExtractor extractor;
    auto functions = extractor.Get(file);

    auto func_it = std::ranges::find_if(functions, [](const auto &f) { return f.name == "TestLoops"; });

    ASSERT_NE(func_it, functions.end());
    auto result = metric.Calculate(*func_it);
    EXPECT_EQ(result.value, 4);  // 1 + 1 (for) + 1 (while) + 1 (if)
}

TEST(CyclomaticComplexityTest, MatchCase) {
    CyclomaticComplexityMetric metric;

    analyzer::file::File file(std::string(TEST_FILES_DIR) + "/match_case.py");
    analyzer::function::FunctionExtractor extractor;
    auto functions = extractor.Get(file);

    auto func_it = std::ranges::find_if(functions, [](const auto &f) { return f.name == "test_Match_case"; });

    ASSERT_NE(func_it, functions.end());
    auto result = metric.Calculate(*func_it);
    EXPECT_EQ(result.value, 4);  // 1 + 3 (case)
}

TEST(CyclomaticComplexityTest, Exceptions) {
    CyclomaticComplexityMetric metric;

    analyzer::file::File file(std::string(TEST_FILES_DIR) + "/exceptions.py");
    analyzer::function::FunctionExtractor extractor;
    auto functions = extractor.Get(file);

    auto func_it = std::ranges::find_if(functions, [](const auto &f) { return f.name == "Try_Exceptions"; });

    ASSERT_NE(func_it, functions.end());
    auto result = metric.Calculate(*func_it);
    EXPECT_EQ(result.value, 4);  // 1 + 1 (assert) + 1 (try) + 1 (finally)
}

TEST(CyclomaticComplexityTest, Ternary) {
    CyclomaticComplexityMetric metric;

    analyzer::file::File file(std::string(TEST_FILES_DIR) + "/ternary.py");
    analyzer::function::FunctionExtractor extractor;
    auto functions = extractor.Get(file);

    auto func_it = std::ranges::find_if(functions, [](const auto &f) { return f.name == "teSt_ternary"; });

    ASSERT_NE(func_it, functions.end());
    auto result = metric.Calculate(*func_it);
    EXPECT_EQ(result.value, 3);  // 1 + 2 (два тернарных оператора)
}

}  // namespace analyzer::metric::metric_impl