#include "metric_accumulator_impl/sum_average_accumulator.hpp"
#include <cmath>
#include <gtest/gtest.h>

namespace analyzer::metric_accumulator::metric_accumulator_impl::test {

TEST(SumAverageAccumulatorTest, BasicSumAverage) {
    SumAverageAccumulator accumulator;

    accumulator.Accumulate(metric::MetricResult{"test", 5});
    accumulator.Accumulate(metric::MetricResult{"test", 10});
    accumulator.Accumulate(metric::MetricResult{"test", 15});

    accumulator.Finalize();

    auto result = accumulator.Get();
    EXPECT_EQ(result.sum, 30);  // 5+10+15
    EXPECT_DOUBLE_EQ(result.average, 10.0);
}

TEST(SumAverageAccumulatorTest, SingleValue) {
    SumAverageAccumulator accumulator;

    accumulator.Accumulate(metric::MetricResult{"test", 42});
    accumulator.Finalize();

    auto result = accumulator.Get();
    EXPECT_EQ(result.sum, 42);
    EXPECT_DOUBLE_EQ(result.average, 42.0);
}

TEST(SumAverageAccumulatorTest, Reset) {
    SumAverageAccumulator accumulator;

    accumulator.Accumulate(metric::MetricResult{"test", 1});
    accumulator.Accumulate(metric::MetricResult{"test", 2});
    accumulator.Finalize();

    auto result1 = accumulator.Get();
    EXPECT_EQ(result1.sum, 3);

    accumulator.Reset();
    accumulator.Accumulate(metric::MetricResult{"test", 100});
    accumulator.Finalize();

    auto result2 = accumulator.Get();
    EXPECT_EQ(result2.sum, 100);
    EXPECT_DOUBLE_EQ(result2.average, 100.0);
}

TEST(SumAverageAccumulatorTest, GetBeforeFinalizeThrows) {
    SumAverageAccumulator accumulator;

    accumulator.Accumulate(metric::MetricResult{"test", 10});

    EXPECT_THROW(accumulator.Get(), std::runtime_error);
}

TEST(SumAverageAccumulatorTest, ZeroValues) {
    SumAverageAccumulator accumulator;

    accumulator.Finalize();

    auto result = accumulator.Get();
    EXPECT_EQ(result.sum, 0);
    EXPECT_TRUE(std::isnan(result.average) || result.average == 0.0);
}

}  // namespace analyzer::metric_accumulator::metric_accumulator_impl::test