#include <algorithm>
#include <ranges>
#include <string>
#include <unistd.h>
#include <vector>

#include "function.hpp"
#include "metric.hpp"
#include "metric_accumulator.hpp"

namespace analyzer {

/**
 * @brief Анализирует список Python-файлов и извлекает метрики для всех функций и методов.
 *
 * Эта функция — центральный "конвейер" обработки:
 * 1. Принимает имена файлов.
 * 2. Для каждого файла создаёт объект `File`, который автоматически парсит его через tree-sitter
 *    и строит AST.
 * 3. Извлекает из AST все функции и методы с помощью `FunctionExtractor`.
 * 4. Объединяет все функции из всех файлов в один плоский список (`join`).
 * 5. Для каждой функции вычисляет набор метрик через переданный `metric_extractor`.
 * 6. Возвращает вектор пар: (функция, результаты её метрик).
 */
auto AnalyseFunctions(const std::vector<std::string> &files,
                      const analyzer::metric::MetricExtractor &metric_extractor) {

    using ResultType = std::vector<std::pair<analyzer::function::Function, analyzer::metric::MetricResults>>;

    // Создаем конвейер обработки с использованием диапазонов
    return files | std::views::transform([](const std::string &filename) {
               // Для каждого файла создаем объект File и извлекаем функции
               analyzer::file::File file(filename);
               analyzer::function::FunctionExtractor extractor;
               return extractor.Get(file);
           }) |
           std::views::join  // Объединяем все векторы функций в один плоский диапазон
           | std::views::transform([&](const analyzer::function::Function &func) {
                 // Для каждой функции вычисляем метрики
                 return std::make_pair(func, metric_extractor.Get(func));
             }) |
           std::ranges::to<ResultType>();  // Собираем в результирующий вектор
}

/**
 *
 * @brief Группирует результаты анализа по классам.
 *
 * Эта функция:
 * 1. Отфильтровывает только те функции, которые являются **методами классов**
 *    (у них `class_name.has_value()` == true).
 * 2. Группирует последовательные элементы с одинаковым именем класса с помощью `chunk_by`.
 *
 * Важно:
 * - `chunk_by` работает только с **последовательными** одинаковыми элементами!
 *   Поэтому предполагается, что входной диапазон уже упорядочен по классам
 *   (например, порядок методов в AST сохраняется как в исходном файле).
 * - Если порядок нарушен, один и тот же класс может быть разбит на несколько групп.
 *
 *  Чтобы убедиться, что фильтрация работает, проверьте, что свободные функции (без class_name)
 * действительно исчезают из результата.
 */
auto SplitByClasses(const auto &analysis) {
    return analysis
           // 1. Фильтруем только методы классов (имеют class_name)
           | std::views::filter([](const auto &item) { return item.first.class_name.has_value(); })
           // 2. Группируем по последовательным одинаковым классам
           |
           std::views::chunk_by([](const auto &a, const auto &b) { return a.first.class_name == b.first.class_name; });
}

/**
 * @brief Группирует результаты анализа по исходным файлам.
 *
 * Эта функция:
 * - Разбивает весь список функций на группы, где каждая группа содержит
 *   только функции из одного и того же файла (`filename`).
 * - Использует `chunk_by`, поэтому **порядок функций в `analysis` должен быть по файлам**.
 */
auto SplitByFiles(const auto &analysis) {
    return analysis
           // Группируем по последовательным одинаковым файлам
           | std::views::chunk_by([](const auto &a, const auto &b) { return a.first.filename == b.first.filename; });
}

/**
 * @brief Агрегирует метрики всех функций с помощью аккумулятора.
 *
 * Эта функция:
 * - Проходит по каждому элементу результата `AnalyseFunctions`
 *   (то есть по каждой функции и её метрикам).
 * - Передаёт результаты метрик (`elem.second`) в аккумулятор через `AccumulateNextFunctionResults`.
 */
void AccumulateFunctionAnalysis(const auto &analysis,
                                const analyzer::metric_accumulator::MetricsAccumulator &accumulator) {
    // Проходим по всем функциям и аккумулируем их метрики
    std::ranges::for_each(analysis | std::views::values, [&](const auto &metric_results) {
        accumulator.AccumulateNextFunctionResults(metric_results);
    });
}

}  // namespace analyzer
