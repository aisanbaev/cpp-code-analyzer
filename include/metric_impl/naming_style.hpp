#pragma once
#include "metric.hpp"
#include <string>
#include <unistd.h>

namespace analyzer::metric::metric_impl {

struct NamingStyleMetric : IMetric {
    static inline const std::string kName = "Naming style";

protected:
    std::string Name() const override;

    MetricResult::ValueType CalculateImpl(const function::Function &f) const override;
};

}  // namespace analyzer::metric::metric_impl
