// registry.h
#ifndef REGISTRY_H
#define REGISTRY_H

#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>

// Структура, описывающая один endpoint сервиса
struct Endpoint {
    std::string host;
    uint16_t port;
    bool healthy;
    time_t deadUntil;
};

// Загружает конфигурацию из JSON-файла
void loadConfig(const std::string& filename);

// Возвращает список здоровых endpoint-ов для сервиса
std::vector<Endpoint> getHealthy(const std::string& serviceName);

// Возвращает доступ к общей карте всех сервисов
std::unordered_map<std::string, std::vector<Endpoint>>& getRegistry();

// Возвращает mutex для защиты карты сервисов
std::mutex& getRegistryMutex();

// Запускает фоновую проверку доступности сервисов
void startHealthCheckThread(int intervalSeconds = 10);

#endif // REGISTRY_H