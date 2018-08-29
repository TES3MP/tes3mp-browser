#ifndef COMPONENTS_SETTINGS_H
#define COMPONENTS_SETTINGS_H

#include <set>
#include <map>
#include <string>

namespace Settings
{
    typedef std::pair < std::string, std::string > CategorySetting; 
    typedef std::set< std::pair<std::string, std::string> > CategorySettingVector;
    typedef std::map < CategorySetting, std::string > CategorySettingValueMap;

    ///
    /// \brief Settings management (can change during runtime)
    ///
    class Manager
    {
    public:
        CategorySettingValueMap mDefaultSettings;
        CategorySettingValueMap mUserSettings;

        CategorySettingVector mChangedSettings;
        ///< tracks all the settings that were changed since the last apply() call

        void clear();
        ///< clears all settings and default settings

        void loadDefault (const std::string& file);
        ///< load file as the default settings (can be overridden by user settings)

        void loadUser (const std::string& file);
        ///< load file as user settings

        void saveUser (const std::string& file);
        ///< save user settings to file

        const CategorySettingVector apply();
        ///< returns the list of changed settings and then clears it

        int getInt (const std::string& setting, const std::string& category);
        float getFloat (const std::string& setting, const std::string& category);
        std::string getString (const std::string& setting, const std::string& category);
        bool getBool (const std::string& setting, const std::string& category);

        void setInt (const std::string& setting, const std::string& category, int value);
        void setFloat (const std::string& setting, const std::string& category, float value);
        void setString (const std::string& setting, const std::string& category, const std::string& value);
        void setBool (const std::string& setting, const std::string& category, bool value);
    };

}

#endif // _COMPONENTS_SETTINGS_H
