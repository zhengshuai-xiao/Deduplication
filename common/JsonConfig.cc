#include "JsonConfig.h"
#include <fstream>
#include <iostream>

bool JsonConfig::LoadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open config file: " << filepath << std::endl;
        return false;
    }
    try {
        file >> m_data;
        return true;
    } catch (const nlohmann::json::exception& e) {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
        return false;
    }
}

bool JsonConfig::SaveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to save config file: " << filepath << std::endl;
        return false;
    }
    file << m_data.dump(4);
    return true;
}

template<typename T>
T JsonConfig::Get(const std::string& key, const T& defaultValue) const {
    try {
        return m_data.value(key, defaultValue);
    } catch (...) {
        return defaultValue;
    }
}

template<typename T>
void JsonConfig::Set(const std::string& key, const T& value) {
    m_data[key] = value;
}

bool JsonConfig::Contains(const std::string& key) const {
    return m_data.contains(key);
}

template int JsonConfig::Get<int>(const std::string&, const int&) const;
template void JsonConfig::Set<int>(const std::string&, const int&);
template std::string JsonConfig::Get<std::string>(const std::string&, const std::string&) const;
template void JsonConfig::Set<std::string>(const std::string&, const std::string&);