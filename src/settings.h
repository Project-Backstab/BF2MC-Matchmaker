#ifndef SETTINGS_H
#define SETTINGS_H

#include <shared_mutex>

#include <json/json.h>

/**
 * @brief Global settings object.
 * 
 * This global variable holds the settings loaded from a JSON file.
 */
extern Json::Value       g_settings;

/**
 * @brief Mutex for thread-safe access to settings.
 * 
 * This mutex is used to ensure thread safety when accessing or modifying
 * the global settings object.
 */
extern std::shared_mutex g_settings_mutex;

#endif // SETTINGS_H
