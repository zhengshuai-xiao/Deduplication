#include "include/configMgr.h"
#include <fstream>
#include <nlohmann/json.hpp>

ConfigManager& ConfigManager::Instance() {
    static ConfigManager instance;
    return instance;
}

void ConfigManager::LoadFromJson(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open config file: " + filepath);
    }

    try {
        nlohmann::json jsonData;
        file >> jsonData;
        for (auto& [key, value] : jsonData.items()) {
            if (value.is_number_integer()) {
                m_config[key] = value.get<int>();
            } else if (value.is_number_float()) {
                m_config[key] = value.get<double>();
            } else if (value.is_boolean()) {
                m_config[key] = value.get<bool>();
            } else if (value.is_string()) {
                m_config[key] = value.get<std::string>();
            } else if (value.is_array()) {
                if (!value.empty() && value[0].is_number()) {
                    m_config[key] = value.get<std::vector<int>>();
                } else {
                    m_config[key] = value.get<std::vector<std::string>>();
                }
            }
        }
    } catch (const nlohmann::json::exception& e) {
        throw std::runtime_error("JSON parse error: " + std::string(e.what()));
    }
}

void ConfigManager::SaveToJson(const std::string& filepath) const {
    nlohmann::json jsonData;
    for (const auto& [key, value] : m_config) {
        std::visit([&](auto&& arg) {
            jsonData[key] = arg;
        }, value);
    }

    std::ofstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to save config file: " + filepath);
    }
    file << jsonData.dump(4); 
}

template<typename T>
T ConfigManager::Get(const std::string& key, const T& defaultValue) const {
    auto it = m_config.find(key);
    if (it == m_config.end()) {
        return defaultValue;
    }

    try {
        return std::get<T>(it->second);
    } catch (const std::bad_variant_access&) {
        throw std::runtime_error("Type mismatch for key: " + key);
    }
}

template<typename T>
void ConfigManager::Set(const std::string& key, const T& value) {
    m_config[key] = value;
}

template int ConfigManager::Get<int>(const std::string&, const int&) const;
template void ConfigManager::Set<int>(const std::string&, const int&);
template std::string ConfigManager::Get<std::string>(const std::string&, const std::string&) const;
template void ConfigManager::Set<std::string>(const std::string&, const std::string&);
template bool ConfigManager::Get<bool>(const std::string&, const bool&) const;
template void ConfigManager::Set<bool>(const std::string&, const bool&);
template std::vector<int> ConfigManager::Get<std::vector<int>>(const std::string&, const std::vector<int>&) const;
template std::vector<std::string> ConfigManager::Get<std::vector<std::string>>(const std::string&, const std::vector<std::string>&) const;