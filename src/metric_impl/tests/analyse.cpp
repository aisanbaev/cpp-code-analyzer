#include "analyse.hpp"
#include "function.hpp"
#include "metric.hpp"
#include "metric_accumulator.hpp"
#include "metric_accumulator_impl/sum_average_accumulator.hpp"
#include "metric_impl/code_lines_count.hpp"
#include "metric_impl/cyclomatic_complexity.hpp"
#include "metric_impl/parameters_count.hpp"
#include <gtest/gtest.h>
#include <vector>

namespace analyzer::test {

TEST(AnalyseFunctionsTest, BasicAnalysis) {
    metric::MetricExtractor metric_extractor;
    metric_extractor.RegisterMetric(std::make_unique<metric::metric_impl::CodeLinesCountMetric>());
    metric_extractor.RegisterMetric(std::make_unique<metric::metric_impl::CyclomaticComplexityMetric>());

    std::vector<std::string> files = {std::string(TEST_FILES_DIR) + "/simple.py"};

    auto analysis = AnalyseFunctions(files, metric_extractor);

    EXPECT_FALSE(analysis.empty());

    // Проверяем что найдена функция test_simple
    auto it = std::ranges::find_if(analysis, [](const auto &item) { return item.first.name == "test_simple"; });
    EXPECT_NE(it, analysis.end());

    // Проверяем что у функции есть метрики
    EXPECT_FALSE(it->second.empty());
}

TEST(AnalyseFunctionsTest, MultipleFiles) {
    metric::MetricExtractor metric_extractor;
    metric_extractor.RegisterMetric(std::make_unique<metric::metric_impl::CodeLinesCountMetric>());

    std::vector<std::string> files = {std::string(TEST_FILES_DIR) + "/simple.py",
                                      std::string(TEST_FILES_DIR) + "/if.py"};

    auto analysis = AnalyseFunctions(files, metric_extractor);

    // Должны найти функции из обоих файлов
    auto simple_it = std::ranges::find_if(analysis, [](const auto &item) { return item.first.name == "test_simple"; });
    auto if_it = std::ranges::find_if(analysis, [](const auto &item) { return item.first.name == "testIf"; });

    EXPECT_NE(simple_it, analysis.end());
    EXPECT_NE(if_it, analysis.end());
}

TEST(SplitByClassesTest, FiltersAndGroupsClasses) {
    std::vector<std::pair<function::Function, metric::MetricResults>> analysis = {
        {function::Function{"file1.py", std::nullopt, "free_func", "ast1"}, {}},
        {function::Function{"file1.py", "ClassA", "method1", "ast2"}, {}},
        {function::Function{"file1.py", "ClassA", "method2", "ast3"}, {}},
        {function::Function{"file1.py", "ClassB", "method1", "ast4"}, {}},
        {function::Function{"file1.py", std::nullopt, "another_free", "ast5"}, {}}};

    auto class_groups = SplitByClasses(analysis);

    auto it = class_groups.begin();
    EXPECT_NE(it, class_groups.end());

    // Первая группа - ClassA с 2 методами
    auto classA_group = *it;
    EXPECT_EQ(std::ranges::distance(classA_group), 2);
    EXPECT_EQ(classA_group.begin()->first.class_name.value(), "ClassA");
    EXPECT_EQ((++classA_group.begin())->first.class_name.value(), "ClassA");

    // Вторая группа - ClassB с 1 методом
    ++it;
    EXPECT_NE(it, class_groups.end());
    auto classB_group = *it;
    EXPECT_EQ(std::ranges::distance(classB_group), 1);
    EXPECT_EQ(classB_group.begin()->first.class_name.value(), "ClassB");

    // Больше групп нет
    ++it;
    EXPECT_EQ(it, class_groups.end());
}

TEST(SplitByFilesTest, GroupsByFiles) {
    std::vector<std::pair<function::Function, metric::MetricResults>> analysis = {
        {function::Function{"file1.py", std::nullopt, "func1", "ast1"}, {}},
        {function::Function{"file1.py", "ClassA", "method1", "ast2"}, {}},
        {function::Function{"file2.py", std::nullopt, "func2", "ast3"}, {}},
        {function::Function{"file2.py", std::nullopt, "func3", "ast4"}, {}},
        {function::Function{"file1.py", "ClassA", "method2", "ast5"}, {}}};

    auto file_groups = SplitByFiles(analysis);
    auto it = file_groups.begin();

    // Первая группа - file1.py с 2 функциями
    EXPECT_NE(it, file_groups.end());
    auto group1 = *it;
    EXPECT_EQ(std::ranges::distance(group1), 2);
    EXPECT_EQ(group1.begin()->first.filename, "file1.py");
    EXPECT_EQ((++group1.begin())->first.filename, "file1.py");

    // Вторая группа - file2.py с 2 функциями
    ++it;
    EXPECT_NE(it, file_groups.end());
    auto group2 = *it;
    EXPECT_EQ(std::ranges::distance(group2), 2);
    EXPECT_EQ(group2.begin()->first.filename, "file2.py");
    EXPECT_EQ((++group2.begin())->first.filename, "file2.py");

    // Третья группа - file1.py с 1 функцией
    ++it;
    EXPECT_NE(it, file_groups.end());
    auto group3 = *it;
    EXPECT_EQ(std::ranges::distance(group3), 1);
    EXPECT_EQ(group3.begin()->first.filename, "file1.py");

    // Больше групп нет
    ++it;
    EXPECT_EQ(it, file_groups.end());
}

TEST(AccumulateFunctionAnalysisTest, AccumulatesMetrics) {
    metric_accumulator::MetricsAccumulator accumulator;
    accumulator.RegisterAccumulator(
        "Code lines count",
        std::make_unique<analyzer::metric_accumulator::metric_accumulator_impl::SumAverageAccumulator>());

    std::vector<std::pair<function::Function, metric::MetricResults>> analysis = {
        {function::Function{"file1.py", std::nullopt, "func1", "ast1"}, {metric::MetricResult{"Code lines count", 10}}},
        {function::Function{"file1.py", std::nullopt, "func2", "ast2"}, {metric::MetricResult{"Code lines count", 20}}},
        {function::Function{"file1.py", std::nullopt, "func3", "ast3"},
         {metric::MetricResult{"Code lines count", 30}}}};

    AccumulateFunctionAnalysis(analysis, accumulator);

    // Получаем результаты аккумуляции
    auto result =
        accumulator.GetFinalizedAccumulator<metric_accumulator::metric_accumulator_impl::SumAverageAccumulator>(
            "Code lines count");
    auto sum_avg = result.Get();

    EXPECT_EQ(sum_avg.sum, 60);  // 10 + 20 + 30
    EXPECT_DOUBLE_EQ(sum_avg.average, 20.0);
}

TEST(AccumulateFunctionAnalysisTest, HandlesEmptyAnalysis) {
    metric_accumulator::MetricsAccumulator accumulator;
    accumulator.RegisterAccumulator(
        "Code lines count", std::make_unique<metric_accumulator::metric_accumulator_impl::SumAverageAccumulator>());

    std::vector<std::pair<function::Function, metric::MetricResults>> empty_analysis;
    EXPECT_NO_THROW(AccumulateFunctionAnalysis(empty_analysis, accumulator));
}

}  // namespace analyzer::test