#pragma once
#include "metric_accumulator.hpp"
#include <unistd.h>

namespace analyzer::metric_accumulator::metric_accumulator_impl {

struct SumAverageAccumulator : public IAccumulator {
    struct SumAverage {
        int sum;
        double average;
        auto operator<=>(const SumAverage &) const = default;
    };
    void Accumulate(const metric::MetricResult &metric_result) override;

    virtual void Finalize() override;

    virtual void Reset() override;

    SumAverage Get() const;

private:
    int sum = 0;
    int count = 0;
    double average = 0;
};

}  // namespace analyzer::metric_accumulator::metric_accumulator_impl
