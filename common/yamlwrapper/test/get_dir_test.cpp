/*
 * @Description: 
 * @Author: an.huang@tusen.ai
 * @Date: 2022-12-07 10:52:48
 * @LastEditors: an.huang
 * @LastEditTime: 2022-12-14 04:34:07
 * @FilePath: /root/adas-monorepo/modules/adaptive_autosar/ara-api/common/yamlwrapper/test/get_dir_test.cpp
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

using namespace ara::yamlwrapper;

TEST(GET_VEHICLE_DIR, no_env_viriable){
    auto yamlwrapper = ara::yamlwrapper::YamlWrapper::get_instance();
    EXPECT_THROW(yamlwrapper.get_vehicle_dir(), InvalidDirException);
}

TEST(GET_VEHICLE_DIR, with_vehicle_dir_only){
    char* config_root = getenv("TEST_SRCDIR");
    std::string tmp = config_root;  
    tmp = tmp + "/adas-monorepo/modules/adaptive_autosar/ara-api/common/yamlwrapper/test/yaml_config_root";
    auto yamlwrapper = ara::yamlwrapper::YamlWrapper::get_instance();
    string result;
    std::string vehicle_config_dir = "";
    try{
        vehicle_config_dir = tmp + "/vehicle";
        setenv("VEHICLE_CONFIG_DIR",vehicle_config_dir.c_str(),1);
        result = yamlwrapper.get_vehicle_dir();
        unsetenv("VEHICLE_CONFIG_DIR");
    }catch(...){
    }
    EXPECT_STREQ(result.c_str(),vehicle_config_dir.c_str());
}

TEST(GET_VEHICLE_DIR, with_root_dir_only){
    char* config_root = getenv("TEST_SRCDIR");
    std::string tmp = config_root;    
    tmp = tmp + "/adas-monorepo/modules/adaptive_autosar/ara-api/common/yamlwrapper/test/yaml_config_root";
    auto yamlwrapper = ara::yamlwrapper::YamlWrapper::get_instance();
    string result;
    try{
        setenv("CONFIG_ROOT_DIR",tmp.c_str(),1);
        result = yamlwrapper.get_vehicle_dir();
        unsetenv("CONFIG_ROOT_DIR");
    }catch(...){
    }
    tmp = tmp + "/vehicle";
    EXPECT_STREQ(result.c_str(),tmp.c_str());
}

TEST(GET_VEHICLE_DIR, with_both_root_and_vehicle){
    char* config_root = getenv("TEST_SRCDIR");
    std::string tmp = config_root;    
    tmp = tmp + "/adas-monorepo/modules/adaptive_autosar/ara-api/common/yamlwrapper/test/yaml_config_root";
    tmp = tmp + "/vehicle";
    auto yamlwrapper = ara::yamlwrapper::YamlWrapper::get_instance();
    string result;
    try{
        setenv("VEHICLE_CONFIG_DIR",tmp.c_str(),1);
        setenv("CONFIG_ROOT_DIR","/fake/path/to/root",1);
        result = yamlwrapper.get_vehicle_dir();
        unsetenv("CONFIG_ROOT_DIR");
        unsetenv("VEHICLE_CONFIG_DIR");
    }catch(...){
    }
    EXPECT_STREQ(result.c_str(),tmp.c_str());
}

TEST(GET_VEHICLE_DIR, multi_threads){
    auto yamlwrapper = ara::yamlwrapper::YamlWrapper::get_instance();
    char* config_root = getenv("TEST_SRCDIR");
    auto read([&yamlwrapper,&config_root](){
        string tmp(config_root);
        tmp = tmp + "/adas-monorepo/modules/adaptive_autosar/ara-api/common/yamlwrapper/test/yaml_config_root";
        tmp = tmp + "/vehicle";
        setenv("VEHICLE_CONFIG_DIR",tmp.c_str(),1);
        // const YAML::Node result;
        for(int i=0;i<3;++i){
            std::string result = yamlwrapper.get_vehicle_dir();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            EXPECT_STREQ(result.c_str(),tmp.c_str());
        }
    });
    vector<thread> threads;
    try{
        for(int i=0;i<15;++i){
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


TEST(SET_DEFAULT_DIR, with_invalid_input){
    auto yamlwrapper = ara::yamlwrapper::YamlWrapper::get_instance();
    std::vector<std::string> default_dir{"/fake/path"};
    bool re = yamlwrapper.set_default_dir(default_dir);
    try{
        std::string tmp = yamlwrapper.get_vehicle_dir();
    }catch(std::exception& e){}
    EXPECT_EQ(re,false);
}

TEST(SET_DEFAULT_DIR, with_valid_input){
    auto yamlwrapper = ara::yamlwrapper::YamlWrapper::get_instance();
    std::vector<std::string> default_dir{"/fake/path1","/fake/path2","/fake/path3","/fake/path4"};
    bool re = yamlwrapper.set_default_dir(default_dir);
    try{
        std::string tmp = yamlwrapper.get_vehicle_dir();
    }catch(std::exception& e){}
    EXPECT_EQ(re,true);
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