#include "metric_accumulator_impl/categorical_accumulator.hpp"
#include <gtest/gtest.h>
#include <stdexcept>

namespace analyzer::metric_accumulator::metric_accumulator_impl::test {

TEST(CategoricalAccumulatorTest, BasicCategorical) {
    CategoricalAccumulator accumulator;

    accumulator.Accumulate(metric::MetricResult{"style", "camelCase"});
    accumulator.Accumulate(metric::MetricResult{"style", "snake_case"});
    accumulator.Accumulate(metric::MetricResult{"style", "camelCase"});
    accumulator.Accumulate(metric::MetricResult{"style", "PascalCase"});

    accumulator.Finalize();

    auto result = accumulator.Get();
    EXPECT_EQ(result.size(), 3);  // 3 уникальные категории
    EXPECT_EQ(result["camelCase"], 2);
    EXPECT_EQ(result["snake_case"], 1);
    EXPECT_EQ(result["PascalCase"], 1);
}

TEST(CategoricalAccumulatorTest, SingleCategory) {
    CategoricalAccumulator accumulator;

    accumulator.Accumulate(metric::MetricResult{"type", "function"});
    accumulator.Accumulate(metric::MetricResult{"type", "function"});
    accumulator.Accumulate(metric::MetricResult{"type", "function"});

    accumulator.Finalize();

    auto result = accumulator.Get();
    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result["function"], 3);
}

TEST(CategoricalAccumulatorTest, Reset) {
    CategoricalAccumulator accumulator;

    accumulator.Accumulate(metric::MetricResult{"test", "A"});
    accumulator.Finalize();

    EXPECT_EQ(accumulator.Get().size(), 1);

    accumulator.Reset();
    accumulator.Accumulate(metric::MetricResult{"test", "B"});
    accumulator.Finalize();

    auto result = accumulator.Get();
    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result["B"], 1);
    EXPECT_EQ(result.find("A"), result.end());  // A не должно быть после сброса
}

TEST(CategoricalAccumulatorTest, GetBeforeFinalizeThrows) {
    CategoricalAccumulator accumulator;

    accumulator.Accumulate(metric::MetricResult{"test", "value"});

    EXPECT_THROW(accumulator.Get(), std::runtime_error);
}

TEST(CategoricalAccumulatorTest, EmptyCategories) {
    CategoricalAccumulator accumulator;

    accumulator.Finalize();

    auto result = accumulator.Get();
    EXPECT_TRUE(result.empty());
}

}  // namespace analyzer::metric_accumulator::metric_accumulator_impl::test