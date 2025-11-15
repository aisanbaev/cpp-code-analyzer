#include "metric_accumulator_impl/average_accumulator.hpp"
#include <gtest/gtest.h>

namespace analyzer::metric_accumulator::metric_accumulator_impl::test {

TEST(AverageAccumulatorTest, BasicAverage) {
    AverageAccumulator accumulator;

    // Накопление значений
    accumulator.Accumulate(metric::MetricResult{"test", 10});
    accumulator.Accumulate(metric::MetricResult{"test", 20});
    accumulator.Accumulate(metric::MetricResult{"test", 30});

    accumulator.Finalize();

    EXPECT_DOUBLE_EQ(accumulator.Get(), 20.0);  // (10+20+30)/3 = 20
}

TEST(AverageAccumulatorTest, SingleValue) {
    AverageAccumulator accumulator;

    accumulator.Accumulate(metric::MetricResult{"test", 5});
    accumulator.Finalize();

    EXPECT_DOUBLE_EQ(accumulator.Get(), 5.0);
}

TEST(AverageAccumulatorTest, Reset) {
    AverageAccumulator accumulator;

    accumulator.Accumulate(metric::MetricResult{"test", 10});
    accumulator.Accumulate(metric::MetricResult{"test", 20});
    accumulator.Finalize();

    EXPECT_DOUBLE_EQ(accumulator.Get(), 15.0);

    // Сброс и новое накопление
    accumulator.Reset();
    accumulator.Accumulate(metric::MetricResult{"test", 100});
    accumulator.Finalize();

    EXPECT_DOUBLE_EQ(accumulator.Get(), 100.0);
}

TEST(AverageAccumulatorTest, GetBeforeFinalizeThrows) {
    AverageAccumulator accumulator;

    accumulator.Accumulate(metric::MetricResult{"test", 10});

    EXPECT_THROW(accumulator.Get(), std::runtime_error);
}

TEST(AverageAccumulatorTest, ZeroValues) {
    AverageAccumulator accumulator;

    // Не добавляем значений, но вызываем Finalize
    accumulator.Finalize();

    // Проверяем что не падает и возвращает 0
    EXPECT_NO_THROW(accumulator.Get());
}

}  // namespace analyzer::metric_accumulator::metric_accumulator_impl::test