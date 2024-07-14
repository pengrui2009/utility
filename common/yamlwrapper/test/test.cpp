/*
 * @Description: 
 * @Author: an.huang@tusen.ai
 * @Date: 2022-12-08 03:53:28
 * @LastEditors: an.huang
 * @LastEditTime: 2022-12-08 09:03:34
 * @FilePath: /root/adas-monorepo/modules/adaptive_autosar/ara-api/common/yamlwrapper/test/test.cpp
 * @version: 
 *  
 * Copyright (c) 2022 by Tusen, All Rights Reserved.
 */
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>

#include "yaml_wrapper.h"

using namespace ara::yamlwrapper;

int main(int argc, char* argv[]){
    setenv("AP_LOG_CONFIG_FILE","/root/adas-monorepo/modules/adaptive_autosar/ara-api/common/yamlwrapper/log_config.json",1);

    auto yamlwrapper = ara::yamlwrapper::YamlWrapper::get_instance();
    std::string config_root = "/root/adas-monorepo/modules/adaptive_autosar/ara-api/common/yamlwrapper/test/yaml_config_root";
    
    // Without environment variables.
    try{
        std::string result = yamlwrapper.get_vehicle_dir();
        warn_log()<<"get_vehicle_dir.no_env_variables: FAIL!";
    }catch(std::exception& e){
        info_log()<<"get_vehicle_dir.no_env_variables: PASS!";
    }
    try{
        YAML::Node node = yamlwrapper.get_vehicle_config();
        info_log()<<"get_vehicle_dir.no_env_variables: FAIL!";
    }catch(std::exception& e){
        info_log()<<"get_vehicle_yaml.no_env_variables: PASS!";
    }

    // With CONFIG_ROOT_DIR only.
    setenv("CONFIG_ROOT_DIR",config_root.c_str(),1);
    try{
        std::string result = yamlwrapper.get_vehicle_dir();
        info_log()<<"get_vehicle_dir.with_root_dir_only: PASS!";
    }catch(std::exception& e){
        info_log()<<"get_vehicle_dir.with_root_dir_only: FAIL!";
    }
    try{
        YAML::Node node = yamlwrapper.get_vehicle_config();
        info_log()<<"get_vehicle_yaml.with_root_dir_only: PASS!";
    }catch(std::exception& e){
        info_log()<<"get_vehicle_yaml.with_root_dir_only: FAIL!";
    }
    unsetenv("CONFIG_ROOT_DIR");

    // With VEHICLE_CONFIG_DIR only.
    std::string vehicle_config_dir = config_root + "/vehicle";
    setenv("VEHICLE_CONFIG_DIR",vehicle_config_dir.c_str(),1);
    try{
        std::string result = yamlwrapper.get_vehicle_dir();
        info_log()<<"get_vehicle_dir.with_vehicle_config_only: PASS!";
    }catch(std::exception& e){
        info_log()<<"get_vehicle_dir.with_vehicle_config_only: FAIL!";
    }
    try{
        YAML::Node node = yamlwrapper.get_vehicle_config();
        info_log()<<"get_vehicle_yaml.with_vehicle_config_only: PASS!";
    }catch(std::exception& e){
        info_log()<<"get_vehicle_yaml.with_vehicle_config_only: FAIL!";
    }
    unsetenv("VEHICLE_CONFIG_DIR");

    // With both CONFIG_ROOT_DIR and VEHICLE_CONFIG_DIR.
    std::string fake_config_root = "/fake/root/path";
    setenv("CONFIG_ROOT_DIR",fake_config_root.c_str(),1);
    setenv("VEHICLE_CONFIG_DIR",vehicle_config_dir.c_str(),1);
    try{
        std::string result = yamlwrapper.get_vehicle_dir();
        info_log()<<"get_vehicle_dir.with_vehicle_config_only: PASS!";
    }catch(std::exception& e){
        info_log()<<"get_vehicle_dir.with_vehicle_config_only: FAIL!";
    }
    try{
        YAML::Node node = yamlwrapper.get_vehicle_config();
        info_log()<<"get_vehicle_yaml.with_vehicle_config_only: PASS!";
    }catch(std::exception& e){
        info_log()<<"get_vehicle_yaml.with_vehicle_config_only: FAIL!";
    }
    unsetenv("VEHICLE_CONFIG_DIR");
    unsetenv("CONFIG_ROOT_DIR");
}