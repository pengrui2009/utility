/*
 * @Description: 
 * @Author: an.huang@tusen.ai
 * @Date: 2022-12-07 02:49:32
 * @LastEditors: an.huang
 * @LastEditTime: 2022-12-08 11:28:48
 * @FilePath: /root/adas-monorepo/modules/adaptive_autosar/ara-api/common/yamlwrapper/include/yaml_logging.h
 * @version: 
 *  
 * Copyright (c) 2022 by Tusen, All Rights Reserved.
 */
#include <ara/log/logger.h>

namespace ara{
namespace yamlwrapper{
class YamlLog{
    public:
        ~YamlLog(){};

        static YamlLog& get_instance();

        const ara::log::Logger* operator->() const {
            return m_logger;
        }

        // Shall not be used! Use is_initialized instead.
        const bool operator==(bool other) const {
            if(this->initialized == other)
                return true;
            return false;
        }

        bool is_initialized() const;

        void initialize();

    private:
    
        YamlLog(){}

        ara::log::Logger* m_logger;

        // Initialization status
        bool initialized = false;

        // Eager Singleton
        // static YamlLog* instance;

        // class Deletor{
        //     public:
        //         ~Deletor(){
        //             if(YamlLog::instance != NULL)
        //                 delete YamlLog::instance;
        //         }
        // };
};


inline auto fatal_log(){
    return YamlLog::get_instance()->LogFatal();
}
inline auto error_log(){
    return YamlLog::get_instance()->LogError();
}
inline auto warn_log(){
    return YamlLog::get_instance()->LogWarn();
}
inline auto info_log(){
    return YamlLog::get_instance()->LogInfo();
}
inline auto debug_log(){
    return YamlLog::get_instance()->LogDebug();
}
}
}