/*
 * @Description: 
 * @Author: an.huang@tusen.ai
 * @Date: 2022-12-05 05:48:33
 * @LastEditors: an.huang
 * @LastEditTime: 2022-12-15 11:16:26
 * @FilePath: /root/adas-monorepo/modules/adaptive_autosar/ara-api/common/yamlwrapper/include/yaml_wrapper.h
 * @version: 
 *  
 * Copyright (c) 2022 by Tusen, All Rights Reserved.
 */

#include <ara/log/logger.h>
#include <iostream>
#include <vector>
#include <mutex>

#include "yaml-cpp/yaml.h"
#include "yaml_logging.h"

#define MIN_YAML_SUFFIX_LEN 5
#define MIN_YAML_FILE_LEN 6
#define DEFAULT_DIR_SIZE 4

namespace ara{
namespace yamlwrapper{

class YamlWrapper{
    public:
        /**
         * @description: Get config root dir, default is /etc/app_persist/, CONFIG_ROOT_DIR
         * @return {std::string}
         */        
        std::string get_config_dir();
        /**
         * @description: Get vehicle config root dir, default is /etc/app_persist/vehicle, VEHICLE_CONFIG_DIR
         * @return {std::string}
         */        
        std::string get_vehicle_dir();
        /**
         * @description: Get trailer config root dir, default is /etc/app_persist/trailer, TRAILER_CONFIG_DIR
         * @return {std::string}
         */        
        std::string get_trailer_dir();
        /**
         * @description: Get calibration root dir, default is /etc/app_persist/vehicle/calibration, CALIB_CONFIG_DIR
         * @return {std::string}
         */        
        std::string get_calibration_dir();
        /**
         * @description: Get dimensions.yaml yaml node, default position is /etc/app_persist/vehicle/dimensions.yaml
         * @return {YAML::Node}
         */        
        const YAML::Node get_vehicle_dimensions();
        /**
         * @description: Get vehicle.yaml yaml node, default position is /etc/app_persist/vehicle/vehicle.yaml
         * @return {YAML::Node}
         */        
        const YAML::Node get_vehicle_config();
        /**
         * @description: Get trailer.yaml yaml node, default position is /etc/app_persist/trailer/trailer.yaml
         * @return {YAML::Node}
         */        
        const YAML::Node get_trailer_config();

        /**
         * @description: Create instance when first called.
         * @return {*}
        */
        static YamlWrapper& get_instance();


        ~YamlWrapper(){};

        /**
         * @description: Set default directory of configs.
         * @return {*}
         */
        bool set_default_dir(std::vector<std::string>& default_dir);

    private:

        /**
         * @description: Initialize default paths.
         * @return {*}
        */
        void initialize();
        
        /**
         * @description: Check if the target has been initialized.
         * @return {*}
         */
        bool is_initialized() const;

        /**
         * @description: Check if the given yaml path exists.
         * @param {string&} yaml_path
         * @return {bool}
         */
        bool is_yaml_exist(std::string yaml_path);         
        /**
         * @description: Check if the given root directory of yaml exists.
         * @param {string&} dir_path
         * @return {*}
         */
        bool is_dir_exist(std::string dir_path);

        /**
         * @description: Resolve path, delete misinputs, such as /root/to//file => /root/to/file
         * @param {string&} path
         * @return {*}
         */
        std::vector<std::string> resolve_path(std::string path);
        
        /**
         * @description: Get yaml node with the given yaml path.
         * @param {string&} yaml_path
         * @return {YAML::Node}
         */
        const YAML::Node get_yaml_node(std::string yaml_path);

        YamlWrapper(){};
        
        std::string default_config_dir;
        std::string default_vehicle_dir;
        std::string default_trailer_dir;
        std::string default_calib_dir;
        
        bool initialized = false;

        const char* delim = "/";

        std::mutex *mylock;
        // Eager Singleton
        // static YamlWrapper* yaml_instance;
        
        // static 
        // class Deletor{
        //     public:
        //         ~Deletor(){
        //             if(YamlWrapper::yaml_instance != NULL)
        //                 delete YamlWrapper::yaml_instance;
        //         }
        // };
};
} // namespace yamlwrapper
} // namespace ara