#pragma once
#include "metric.hpp"
#include <string>
#include <unistd.h>
#include <unordered_map>
#include <vector>

namespace analyzer::metric_accumulator {

struct IAccumulator {
    virtual void Accumulate(const metric::MetricResult &metric_result) = 0;
    virtual void Finalize() = 0;
    virtual void Reset() = 0;
    virtual ~IAccumulator() = default;

protected:
    bool is_finalized = false;
};

struct MetricsAccumulator {
    template <typename Accumulator>
    void RegisterAccumulator(const std::string &metric_name, std::unique_ptr<Accumulator> acc) {
        accumulators.emplace(metric_name, std::move(acc));
    }
    template <typename Accumulator>
    const Accumulator &GetFinalizedAccumulator(const std::string &metric_name) const {
        auto metric_accululator = accumulators.at(metric_name);
        metric_accululator->Finalize();
        return dynamic_cast<const Accumulator &>(*metric_accululator);
    }
    void AccumulateNextFunctionResults(const std::vector<metric::MetricResult> &metric_results) const;

    void ResetAccumulators();

private:
    std::unordered_map<std::string, std::shared_ptr<IAccumulator>> accumulators;
};

}  // namespace analyzer::metric_accumulator
