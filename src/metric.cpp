#include "metric.hpp"
#include "function.hpp"
#include <ranges>
#include <unistd.h>
#include <vector>

namespace analyzer::metric {
void MetricExtractor::RegisterMetric(std::unique_ptr<IMetric> metric) { metrics.push_back(std::move(metric)); }

/**
 * @brief Вычисляет все зарегистрированные метрики для заданной функции.
 *
 * Эта функция применяет каждый метрический объект из контейнера `metrics`
 * к переданной функции `func` и собирает результаты в вектор.
 */
MetricResults MetricExtractor::Get(const function::Function &func) const {
    return metrics |
           std::views::transform([&](const std::unique_ptr<IMetric> &metric) { return metric->Calculate(func); }) |
           std::ranges::to<MetricResults>();
}

}  // namespace analyzer::metric