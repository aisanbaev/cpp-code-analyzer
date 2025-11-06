#pragma once

#include "metric.hpp"
#include <string>
#include <unistd.h>

namespace analyzer::metric::metric_impl {

struct CodeLinesCountMetric final : IMetric {
    static inline const std::string kName = "Code lines count";

protected:
    std::string Name() const override;

    MetricResult::ValueType CalculateImpl(const function::Function &f) const override;
};

}  // namespace analyzer::metric::metric_impl
