#include "balancer.hpp"
#include <chrono> // Для инициализации генератора случайных чисел

// --- Конструктор ---
Balancer::Balancer(ServiceRegistry& registry, Algo algorithm)
    : registry_(registry), algo_(algorithm)
{
    seedRandomGenerator(); // Инициализируем генератор случайных чисел
}

// --- Инициализация генератора случайных чисел ---
void Balancer::seedRandomGenerator() {
    // Используем текущее время для более случайного seed'а
    auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    // Блокируем мьютекс на случай, если конструктор вызывается из разных потоков (маловероятно, но безопасно)
    std::lock_guard<std::mutex> lock(rngMutex_);
    rng_.seed(static_cast<std::mt19937::result_type>(seed));
}


// --- Выбор эндпоинта ---
Endpoint Balancer::selectEndpoint(const std::string& serviceName) {
    // Получаем *копию* вектора здоровых эндпоинтов.
    // Блокировки внутри registry_.getHealthy() не распространяются сюда.
    std::vector<Endpoint> healthyEndpoints = registry_.getHealthy(serviceName);

    if (healthyEndpoints.empty()) {
        failedSelections_.fetch_add(1, std::memory_order_relaxed); // Увеличиваем счетчик неудач (атомарно)
        throw NoHealthyEndpoints(serviceName);
    }

    Endpoint selectedEndpoint;
    size_t listSize = healthyEndpoints.size();

    if (algo_ == Algo::RoundRobin) {
        // Получаем или создаем счетчик для этого сервиса
        std::atomic_size_t* indexCounter;
        { // Блок для ограничения области видимости lock_guard
            std::lock_guard<std::mutex> lock(rrIdxMutex_); // Защищаем доступ к карте rrIdx_
            // try_emplace создает новый элемент с 0, только если ключ serviceName не существует
            auto [it, inserted] = rrIdx_.try_emplace(serviceName, 0);
            indexCounter = &(it->second); // Получаем указатель на atomic_size_t
        } // мьютекс rrIdxMutex_ освобождается здесь

        // Атомарно получаем текущее значение и увеличиваем его на 1
        // memory_order_relaxed подходит, т.к. нам не нужны гарантии синхронизации с другими операциями памяти,
        // только атомарность самой операции инкремента.
        size_t idx = indexCounter->fetch_add(1, std::memory_order_relaxed) % listSize;
        selectedEndpoint = healthyEndpoints[idx];

    } else { // Algo::Random
        std::uniform_int_distribution<size_t> distribution(0, listSize - 1);
        size_t idx;
        { // Блок для ограничения области видимости lock_guard
             std::lock_guard<std::mutex> lock(rngMutex_); // Защищаем доступ к генератору
             idx = distribution(rng_); // Генерируем случайный индекс
        } // мьютекс rngMutex_ освобождается здесь

        selectedEndpoint = healthyEndpoints[idx];
    }

    successfulSelections_.fetch_add(1, std::memory_order_relaxed); // Увеличиваем счетчик успехов (атомарно)
    return selectedEndpoint;
}

// --- Опционально: Методы для получения статистики ---

size_t Balancer::getSuccessfulSelections() const {
    // memory_order_relaxed достаточно для чтения значения atomic переменной
    return successfulSelections_.load(std::memory_order_relaxed);
}

size_t Balancer::getFailedSelections() const {
    return failedSelections_.load(std::memory_order_relaxed);
}