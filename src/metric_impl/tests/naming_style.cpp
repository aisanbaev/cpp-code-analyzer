#include "metric_impl/naming_style.hpp"
#include "function.hpp"
#include <gtest/gtest.h>

namespace analyzer::metric::metric_impl {

TEST(NamingStyleTest, SnakeCase) {
    NamingStyleMetric metric;

    function::Function func;
    func.name = "test_snake_case";

    auto result = metric.Calculate(func);
    EXPECT_EQ(std::get<std::string>(result.value), "Snake Case");
}

TEST(NamingStyleTest, CamelCase) {
    NamingStyleMetric metric;

    function::Function func;
    func.name = "testCamelCase";

    auto result = metric.Calculate(func);
    EXPECT_EQ(std::get<std::string>(result.value), "Camel Case");
}

TEST(NamingStyleTest, PascalCase) {
    NamingStyleMetric metric;

    function::Function func;
    func.name = "TestPascalCase";

    auto result = metric.Calculate(func);
    EXPECT_EQ(std::get<std::string>(result.value), "Pascal Case");
}

TEST(NamingStyleTest, LowerCase) {
    NamingStyleMetric metric;

    function::Function func;
    func.name = "testlowercase";

    auto result = metric.Calculate(func);
    EXPECT_EQ(std::get<std::string>(result.value), "Lower Case");
}

}  // namespace analyzer::metric::metric_impl