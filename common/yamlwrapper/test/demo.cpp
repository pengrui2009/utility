/*
 * @Description: 
 * @Author: an.huang@tusen.ai
 * @Date: 2022-12-02 04:05:51
 * @LastEditors: an.huang
 * @LastEditTime: 2022-12-14 02:46:36
 * @FilePath: /root/adas-monorepo/modules/adaptive_autosar/ara-api/common/yamlwrapper/test/demo.cpp
 * @version: 
 *  
 * Copyright (c) 2022 by Tusen, All Rights Reserved.
 */
#include "yaml_wrapper.h"

int main(int argc, char* argv[]){
    auto yamlwrapper = ara::yamlwrapper::YamlWrapper::get_instance();

    std::string config_dir = yamlwrapper.get_config_dir();
    std::string vehicle_dir = yamlwrapper.get_vehicle_dir();
    std::string trailer_dir = yamlwrapper.get_trailer_dir();
    std::string calib_dir = yamlwrapper.get_calibration_dir();

    const YAML::Node dimensions = yamlwrapper.get_vehicle_dimensions();
    const YAML::Node config = yamlwrapper.get_vehicle_config();
    const YAML::Node trailer = yamlwrapper.get_trailer_config();
}