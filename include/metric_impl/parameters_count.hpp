#include "metric.hpp"
#include <string>

namespace analyzer::metric::metric_impl {

struct CountParametersMetric final : public IMetric {
    static inline const std::string kName = "Parameters count";

protected:
    std::string Name() const override;

    MetricResult::ValueType CalculateImpl(const function::Function &f) const override;
};

}  // namespace analyzer::metric::metric_impl
