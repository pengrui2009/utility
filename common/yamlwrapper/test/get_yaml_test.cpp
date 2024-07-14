/*
 * @Description: 
 * @Author: an.huang@tusen.ai
 * @Date: 2022-12-06 08:16:03
 * @LastEditors: an.huang
 * @LastEditTime: 2022-12-14 04:33:57
 * @FilePath: /root/adas-monorepo/modules/adaptive_autosar/ara-api/common/yamlwrapper/test/get_yaml_test.cpp
 * @version: 
 *  
 * Copyright (c) 2022 by Tusen, All Rights Reserved.
 */
#include <assert.h>
#include <gtest/gtest.h>
#include <exception>
#include <typeinfo>
#include <thread>

#include "yaml_wrapper.h"
#include "yaml_exception.h"


TEST(GET_VEHICLE_CONFIG, no_env_viriable){
    auto yamlwrapper = ara::yamlwrapper::YamlWrapper::get_instance();
    EXPECT_THROW(yamlwrapper.get_vehicle_config(), ara::yamlwrapper::InvalidDirException);
}

TEST(GET_VEHICLE_CONFIG, with_vehicle_dir_only){
    char* config_root = getenv("TEST_SRCDIR");
    string tmp(config_root);
    tmp = tmp + "/adas-monorepo/modules/adaptive_autosar/ara-api/common/yamlwrapper/test/yaml_config_root";
    std::string vehicle_config_dir = tmp + "/vehicle";
    auto yamlwrapper = ara::yamlwrapper::YamlWrapper::get_instance();
    YAML::Node result;
    try{
        setenv("VEHICLE_CONFIG_DIR",vehicle_config_dir.c_str(),1);
        result = yamlwrapper.get_vehicle_config();
        unsetenv("VEHICLE_CONFIG_DIR");
    }catch(...){
        FAIL();
    }
}

TEST(GET_VEHICLE_CONFIG, with_root_dir_only){
    char* config_root = getenv("TEST_SRCDIR");
    string tmp(config_root);
    tmp = tmp + "/adas-monorepo/modules/adaptive_autosar/ara-api/common/yamlwrapper/test/yaml_config_root";
    auto yamlwrapper = ara::yamlwrapper::YamlWrapper::get_instance();
    YAML::Node result;
    try{
        setenv("CONFIG_ROOT_DIR",tmp.c_str(),1);
        result = yamlwrapper.get_vehicle_config();
        unsetenv("CONFIG_ROOT_DIR");
    }catch(...){
        FAIL();
    }
}

TEST(GET_VEHICLE_CONFIG, with_both_root_and_vehicle){
    char* config_root = getenv("TEST_SRCDIR");
    string tmp(config_root);
    tmp = tmp + "/adas-monorepo/modules/adaptive_autosar/ara-api/common/yamlwrapper/test/yaml_config_root";
    tmp = tmp + "/vehicle";
    auto yamlwrapper = ara::yamlwrapper::YamlWrapper::get_instance();
    YAML::Node result;
    try{
        setenv("VEHICLE_CONFIG_DIR",tmp.c_str(),1);
        setenv("CONFIG_ROOT_DIR","/fake/to/root/path",1);
        result = yamlwrapper.get_vehicle_config();
        unsetenv("CONFIG_ROOT_DIR");
        unsetenv("VEHICLE_CONFIG_DIR");
    }catch(...){
        FAIL();
    }
}

TEST(GET_VEHICLE_CONFIG, read_yaml_node){
    char* config_root = getenv("TEST_SRCDIR");
    string tmp(config_root);
    tmp = tmp + "/adas-monorepo/modules/adaptive_autosar/ara-api/common/yamlwrapper/test/yaml_config_root";
    tmp = tmp + "/vehicle";
    auto yamlwrapper = ara::yamlwrapper::YamlWrapper::get_instance();
    YAML::Node result;
    try{
        setenv("VEHICLE_CONFIG_DIR",tmp.c_str(),1);
        result = yamlwrapper.get_vehicle_config();
        if(result["description"]){
            std::cout<<result["description"].as<std::string>()<<std::endl;
            EXPECT_EQ(result["description"].as<std::string>(),"SUCCESS");
        }
        else{
            std::cout<<"Read failed!"<<std::endl;
            FAIL();
        }
        unsetenv("VEHICLE_CONFIG_DIR");
    }
    catch(...){
        FAIL();
    }
}

TEST(SET_VEHICLE_CONFIG, write_yaml_config){
    char* config_root = getenv("TEST_SRCDIR");
    string tmp(config_root);
    tmp = tmp + "/adas-monorepo/modules/adaptive_autosar/ara-api/common/yamlwrapper/test/yaml_config_root";
    tmp = tmp + "/vehicle";
    auto yamlwrapper = ara::yamlwrapper::YamlWrapper::get_instance();
    try{
        setenv("VEHICLE_CONFIG_DIR",tmp.c_str(),1);
        const YAML::Node result = yamlwrapper.get_vehicle_config();
        // result["description"] = "FAIL";
        // if(result["description"] == "FAIL")
        //     FAIL();
        unsetenv("VEHICLE_CONFIG_DIR");
    }
    catch(...){
        FAIL();
    }
}

    
TEST(GET_VEHICLE_CONFIG, read_yaml_node_multi_threads){
    auto yamlwrapper = ara::yamlwrapper::YamlWrapper::get_instance();
    char* config_root = getenv("TEST_SRCDIR");
    auto read([&yamlwrapper,&config_root](){
        string tmp(config_root);
        tmp = tmp + "/adas-monorepo/modules/adaptive_autosar/ara-api/common/yamlwrapper/test/yaml_config_root";
        tmp = tmp + "/vehicle";
        setenv("VEHICLE_CONFIG_DIR",tmp.c_str(),1);
        // const YAML::Node result;
        for(int i=0;i<2;++i){
            const YAML::Node result = yamlwrapper.get_vehicle_config();
            // std::this_thread::sleep_for(std::chrono::milliseconds(400));
            EXPECT_EQ(result["description"].as<std::string>(),"SUCCESS");
        }
    });
    vector<thread> threads;
    try{
        for(int i=0;i<5;++i){
            thread tmp(read);
            threads.push_back(std::move(tmp));
        }
        for(int i=0;i<threads.size();++i){
            threads[i].detach();
        }
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }catch(std::exception& e){
        FAIL();
    }  
}


int main(int argc, char* argv[]){
    try{
        ::testing::InitGoogleTest(&argc, argv);
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
    } catch (...) {
    }
    return RUN_ALL_TESTS();
}