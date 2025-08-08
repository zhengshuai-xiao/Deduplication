#pragma once
#include <string>
#include <unordered_map>
#include <variant>
#include <memory>
#include <stdexcept>
#include <vector>

#pragma once
#include <string>
#include <unordered_map>
#include <variant>
#include <memory>
#include <stdexcept>

// 支持的数据类型
using ConfigValue = std::variant<
    int, 
    float, 
    double, 
    bool, 
    std::string,
    std::vector<int>,
    std::vector<std::string>
>;

class ConfigManager {
public:

    static ConfigManager& Instance();

    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;

    void LoadFromJson(const std::string& filepath);
    void SaveToJson(const std::string& filepath) const;
    void LoadFromIni(const std::string& filepath); 
    void LoadFromXml(const std::string& filepath); 

    template<typename T>
    T Get(const std::string& key, const T& defaultValue = T()) const;

    template<typename T>
    void Set(const std::string& key, const T& value);

    bool Contains(const std::string& key) const;
    void Clear();

private:
    ConfigManager() = default;
    std::unordered_map<std::string, ConfigValue> m_config;
};