#pragma once
#include <unistd.h>

#include <memory>
#include <string>
#include <vector>

#include "function.hpp"

namespace analyzer::metric {

struct MetricResult {
    using ValueType = int;
    // using ValueType = std::variant<int, std::string>; // если захотите реализовывать метрику
    // naming style
    std::string metric_name;  // Название метрики
    ValueType value;          // Значение метрики
};

struct IMetric {
    virtual ~IMetric() = default;
    MetricResult Calculate(const function::Function &f) const {
        return MetricResult{.metric_name = Name(), .value = CalculateImpl(f)};
    }

protected:
    virtual MetricResult::ValueType CalculateImpl(const function::Function &f) const = 0;
    virtual std::string Name() const = 0;
};

using MetricResults = std::vector<MetricResult>;

struct MetricExtractor {
    void RegisterMetric(std::unique_ptr<IMetric> metric);

    MetricResults Get(const function::Function &func) const;
    std::vector<std::unique_ptr<IMetric>> metrics;
};

}  // namespace analyzer::metric
