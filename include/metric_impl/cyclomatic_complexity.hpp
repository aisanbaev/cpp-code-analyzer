#pragma once

#include "metric.hpp"
#include <string>
#include <unistd.h>

namespace analyzer::metric::metric_impl {

struct CyclomaticComplexityMetric : IMetric {
    static inline const std::string kName = "Cyclomatic Complexity";

protected:
    std::string Name() const override;

    MetricResult::ValueType CalculateImpl(const function::Function &f) const override;
};

}  // namespace analyzer::metric::metric_impl
