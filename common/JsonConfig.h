#pragma once
#include <string>
#include <nlohmann/json.hpp>
#include "logger.h"

class JsonConfig {
public:
    
    bool LoadFromFile(const std::string& filepath);

    bool SaveToFile(const std::string& filepath) const;

    template<typename T>
    T Get(const std::string& key, const T& defaultValue = T()) const;

    template<typename T>
    void Set(const std::string& key, const T& value);

    bool Contains(const std::string& key) const;

private:
    nlohmann::json m_data;
};