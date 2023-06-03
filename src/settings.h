#ifndef SETTINGS_H
#define SETTINGS_H

#include <shared_mutex>

#include <json/json.h>

extern Json::Value       g_settings;

extern std::shared_mutex g_mutex_settings;

#endif // SETTINGS_H
