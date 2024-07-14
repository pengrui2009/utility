/*
 * @Description: 
 * @Author: an.huang@tusen.ai
 * @Date: 2022-12-07 04:41:32
 * @LastEditors: an.huang
 * @LastEditTime: 2022-12-08 11:27:45
 * @FilePath: /root/adas-monorepo/modules/adaptive_autosar/ara-api/common/yamlwrapper/src/yaml_logging.cpp
 * @version: 
 *  
 * Copyright (c) 2022 by Tusen, All Rights Reserved.
 */
#include "yaml_logging.h"

namespace ara{
namespace yamlwrapper{

    bool YamlLog::is_initialized() const{
        return initialized;
    }

    /**
     * @description: Lazy Singleton, precisely Meyer's Singleton.
     * @return {*}
     */
    YamlLog& YamlLog::get_instance(){
        static YamlLog instance;
        if(!instance.is_initialized()){
            instance.initialize();
            instance.initialized=true;
            info_log()<<"YAML log initialized successfully.";
        }
        return instance;
    }

    void YamlLog::initialize(){
        ara::log::Initialize();
        ara::core::StringView ctxid = "YAML";
        ara::core::StringView ctxdesc = "log for yaml wrapper";
        m_logger = &(ara::log::CreateLogger(ctxid,ctxdesc,ara::log::LogLevel::kVerbose));
    }
}
}