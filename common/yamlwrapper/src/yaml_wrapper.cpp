/*
 * @Description: 
 * @Author: an.huang@tusen.ai
 * @Date: 2022-12-05 05:48:44
 * @LastEditors: an.huang
 * @LastEditTime: 2022-12-16 06:08:14
 * @FilePath: /root/adas-monorepo/modules/adaptive_autosar/ara-api/common/yamlwrapper/src/yaml_wrapper.cpp
 * @version: 
 *  
 * Copyright (c) 2022 by Tusen, All Rights Reserved.
 */
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <vector>
#include <errno.h>

#include "yaml_wrapper.h"
#include "yaml_exception.h"



#ifndef MIN_YAML_SUFFIX_LEN
    #define MIN_YAML_SUFFIX_LEN 5    
#endif

#ifndef MIN_YAML_FILE_LEN
    #define MIN_YAML_FILE_LEN 6
#endif

#ifndef DEFAULT_DIR_SIZE
    #define DEFAULT_DIR_SIZE 4
#endif

namespace ara{
namespace yamlwrapper{
    /**
     * @description: Lazy Singleton, precisely Meyer's Singleton.
     * @return {*}
     */
    YamlWrapper& YamlWrapper::get_instance() {
            static YamlWrapper yaml_instance;
            if(!yaml_instance.is_initialized()){
                yaml_instance.initialize();
                yaml_instance.initialized=true;
                info_log()<<"YAML wrapper initialized successfully.";
            }
            return yaml_instance;
    }

    bool YamlWrapper::is_initialized() const{
        return initialized;
    }

    void YamlWrapper::initialize(){
        default_config_dir = "/etc/app_persist";
        default_vehicle_dir = "/etc/app_persist/vehicle";
        default_trailer_dir = "/etc/app_persist/trailer";
        default_calib_dir = "/etc/app_persist/vehicle/calibration";
        mylock = new std::mutex();
    }

    bool YamlWrapper::set_default_dir(std::vector<std::string>& default_dir){
        if(default_dir.size() < DEFAULT_DIR_SIZE)
            return false;
        default_config_dir = default_dir[0];
        default_vehicle_dir = default_dir[1];
        default_trailer_dir = default_dir[2];
        default_calib_dir = default_dir[3];
        return true;
    }

    bool YamlWrapper::is_dir_exist(std::string dir_path){
        string _new_dir = resolve_path(dir_path)[0];
        const char* new_dir = _new_dir.c_str();
        errno = 0;
        DIR* dir = opendir(new_dir);
        if(dir == NULL){
            if( errno != 0 ){
                warn_log()<<"Open directory failed: "<<new_dir;
                switch (errno){
                    case ENOTDIR:{
                        warn_log()<<Semantics(ExceptionSemantics::kInvalidDirException);
                        break;
                    }
                    case ENOENT:{
                        warn_log()<<Semantics(ExceptionSemantics::kInvalidDirException);
                        break;
                    }
                    case EACCES:{
                        warn_log()<<Semantics(ExceptionSemantics::kAccessDeniedException);
                        break;
                    }
                    case EMFILE:{
                        warn_log()<<Semantics(ExceptionSemantics::kMaxProgcessFilesException);
                        break;
                    }
                    case ENFILE:{
                        warn_log()<<Semantics(ExceptionSemantics::kMaxSystemFilesException);
                        break;
                    }
                    case ENOMEM:{
                        warn_log()<<Semantics(ExceptionSemantics::kNoMemoryException);
                        break;
                    }
                }
                return false;
            }
        }
        errno = 0;
        closedir(dir);
        if(errno == EBADF){
            warn_log()<<Semantics(ExceptionSemantics::kBadFileException);
            errno = 0;
            throw BadFileException();
        }
        return true;
    }

    std::vector<std::string> YamlWrapper::resolve_path(std::string path){
        std::lock_guard<std::mutex> mylockguard(*mylock);

        info_log()<<"Resolving given path: "<<path;

        if( path[0] != *delim ){
            error_log()<<Semantics(ExceptionSemantics::kRelativeDirException);
            throw RelativeDirException();
        } 

        std::vector<std::string> sub_path;
        const char* p = path.c_str();
        char* new_path = const_cast<char*>(p);
        char *sub = strtok(new_path,delim);
        while(sub){
            std::string tmp = sub;
            sub_path.push_back(tmp);
            sub = strtok(NULL,delim);
        }

        std::string _yaml_dir="";
        std::string _yaml_name="";
        int j=0;
        for( ; j < sub_path.size()-1;++j ){
            if(sub_path[j] != ""){
                _yaml_dir = _yaml_dir+"/";
                _yaml_dir=_yaml_dir+sub_path[j];
            }
        }
        if(sub_path[j].length()<=MIN_YAML_SUFFIX_LEN || sub_path[j].substr(sub_path[j].length()-MIN_YAML_SUFFIX_LEN,MIN_YAML_SUFFIX_LEN) != ".yaml"){
            _yaml_dir = _yaml_dir+"/"+sub_path[j];
        }
        else
            _yaml_name = sub_path[j];
        info_log()<<"Resolved root path: "<<_yaml_dir;
        info_log()<<"Resolved yaml name: "<<_yaml_name;

        return {_yaml_dir,_yaml_name};
    }

    bool YamlWrapper::is_yaml_exist(std::string yaml_path){
        std::lock_guard<std::mutex> mylockguard(*mylock);
        // Log printing
        info_log()<<"Checking given yaml path: "<<yaml_path;

        if(yaml_path.length() <= MIN_YAML_FILE_LEN || yaml_path.substr(yaml_path.length()-MIN_YAML_SUFFIX_LEN,MIN_YAML_SUFFIX_LEN)!=".yaml"){
            error_log()<<Semantics(ExceptionSemantics::kInvalidDirException);
            throw InvalidDirException();
        }
        else if( yaml_path[0] != *delim ){
            error_log()<<Semantics(ExceptionSemantics::kRelativeDirException);
            throw RelativeDirException();
        } 

        vector<string> sub_path;
        
        const char* p = yaml_path.c_str();
        char* path = const_cast<char*>(p);
        char *sub = strtok(path,delim);
        while(sub){
            std::string tmp = sub;
            sub_path.push_back(tmp);
            sub = strtok(NULL,delim);
        }

        string _yaml_dir="";
        int j=0;
        for( ;  j < sub_path.size()-1 ;++j ){
            _yaml_dir = _yaml_dir+"/";
            _yaml_dir=_yaml_dir+sub_path[j];
        }
        info_log()<<"Resolved yaml path: "<<_yaml_dir;
        string yaml_name = sub_path[j];
        info_log()<<"Resolved yaml name: "<<yaml_name;
        const char* yaml_dir=_yaml_dir.c_str();
        errno = 0;
        DIR* root = opendir(yaml_dir);
        if(root == NULL){
            if( errno != 0 ){
                switch (errno){
                    case ENOTDIR:{
                        error_log()<<Semantics(ExceptionSemantics::kInvalidDirException);
                        throw InvalidDirException();
                    }
                    case ENOENT:{
                        error_log()<<Semantics(ExceptionSemantics::kInvalidDirException);
                        throw InvalidDirException();
                    }
                    case EACCES:{
                        error_log()<<Semantics(ExceptionSemantics::kAccessDeniedException);
                        throw AccessDeniedException();
                    }
                    case EMFILE:{
                        error_log()<<Semantics(ExceptionSemantics::kMaxProgcessFilesException);
                        throw MaxProgcessFilesException();
                    }
                    case ENFILE:{
                        error_log()<<Semantics(ExceptionSemantics::kMaxSystemFilesException);
                        throw MaxSystemFilesException();
                    }
                    case ENOMEM:{
                        error_log()<<Semantics(ExceptionSemantics::kNoMemoryException);
                        throw NoMemoryException();
                    }
                }
            }
        }
        errno = 0;
        dirent* dir_file = readdir(root);
        if(errno == EBADF){
            error_log()<<Semantics(ExceptionSemantics::kBadFileException);
            throw BadFileException();
        }
        while(dir_file){
            if(errno == EBADF){
                error_log()<<Semantics(ExceptionSemantics::kBadFileException);
                throw BadFileException();
            }
            if(dir_file->d_name==yaml_name){
                errno = 0;
                closedir(root);
                if(errno == EBADF){
                    error_log()<<Semantics(ExceptionSemantics::kBadFileException);
                    throw BadFileException();
                }
                return true;
                break;
            }
            else
                dir_file = readdir(root);
        }
        errno = 0;
        closedir(root);
        if(errno == EBADF){
            error_log()<<Semantics(ExceptionSemantics::kBadFileException);
            throw BadFileException();
        }
        return false;
    }

    const YAML::Node YamlWrapper::get_yaml_node(std::string yaml_path){
        bool path_valid = is_yaml_exist(yaml_path);
        if(path_valid){
            YAML::Node node = YAML::LoadFile(yaml_path);
            return node;
        }
    }

    std::string YamlWrapper::get_config_dir(){
        string default_dir = default_config_dir;
        char* config_root_dir = getenv("CONFIG_ROOT_DIR");
        if(config_root_dir!=NULL){
            string tmp(config_root_dir);
            if(is_dir_exist(tmp)){
                return resolve_path(tmp)[0];
            }
        }
        if(is_dir_exist(default_dir))
            return resolve_path(default_dir)[0];
        error_log()<<Semantics(ExceptionSemantics::kInvalidDirException);
        throw InvalidDirException();
    }

    std::string YamlWrapper::get_vehicle_dir(){
        string default_dir = default_vehicle_dir;
        char* vehicle_config_dir = getenv("VEHICLE_CONFIG_DIR"); 
        if(vehicle_config_dir!=NULL){
            string tmp(vehicle_config_dir);
            if(is_dir_exist(tmp))
                return resolve_path(tmp)[0];
        }
        try{
            string config_root_dir = get_config_dir();
            config_root_dir = config_root_dir + "/vehicle";
            const char* t = config_root_dir.c_str();
            char* _tmp = const_cast<char*>(t);
            string tmp(_tmp);
            if(is_dir_exist(tmp))
                return resolve_path(tmp)[0];
        }catch(std::exception& e){
            if(is_dir_exist(default_dir))
                return resolve_path(default_dir)[0];
        }
        error_log()<<Semantics(ExceptionSemantics::kInvalidDirException);
        throw InvalidDirException();
    }

    std::string YamlWrapper::get_trailer_dir(){
        string default_dir = default_trailer_dir;
        char* trailer_config_dir = getenv("TRAILER_CONFIG_DIR"); 
        if(trailer_config_dir!=NULL){
            string tmp(trailer_config_dir);
            if(is_dir_exist(tmp))
                return resolve_path(tmp)[0];
        }
        try{
            string config_root_dir = get_config_dir();
            config_root_dir = config_root_dir + "/trailer";
            const char* t = config_root_dir.c_str();
            char* _tmp = const_cast<char*>(t);
            string tmp(_tmp);
            if(is_dir_exist(tmp))
                return resolve_path(tmp)[0];
        }catch(std::exception& e){
            if(is_dir_exist(default_dir))
                return resolve_path(default_dir)[0];
        }
        error_log()<<Semantics(ExceptionSemantics::kInvalidDirException);
        throw InvalidDirException();
    }

    std::string YamlWrapper::get_calibration_dir(){
        string default_dir = default_calib_dir;
        char* calib_config_dir = getenv("CALIB_CONFIG_DIR"); 
        if(calib_config_dir!=NULL){
            string tmp(calib_config_dir);
            if(is_dir_exist(tmp))
                return resolve_path(tmp)[0];
        }
        try{
            string vehicle_config_dir = get_vehicle_dir();
            vehicle_config_dir = vehicle_config_dir + "/calibration";
            const char* t = vehicle_config_dir.c_str();
            char* _tmp = const_cast<char*>(t);
            string tmp(_tmp);
            if(is_dir_exist(tmp))
                return resolve_path(tmp)[0];
        }catch(std::exception& e){
            if(is_dir_exist(default_dir))
                return resolve_path(default_dir)[0];
        }
        error_log()<<Semantics(ExceptionSemantics::kInvalidDirException);
        throw InvalidDirException();
    }

    const YAML::Node YamlWrapper::get_vehicle_dimensions(){
        string vehicle_config_dir = get_vehicle_dir();
        string vehicle_config_path = vehicle_config_dir + "/dimensions.yaml";
        YAML::Node node = get_yaml_node(vehicle_config_path);
        return node;
    }

    const YAML::Node YamlWrapper::get_vehicle_config(){
        string vehicle_config_dir = get_vehicle_dir();
        string vehicle_config_path = vehicle_config_dir + "/vehicle.yaml";
        YAML::Node node = get_yaml_node(vehicle_config_path);
        return node;
    }

    const YAML::Node YamlWrapper::get_trailer_config(){
        string trailer_config_dir = get_trailer_dir();
        string trailer_config_path = trailer_config_dir + "/trailer.yaml";
        YAML::Node node = get_yaml_node(trailer_config_path);
        return node;
    }
}  // namespace yamlwrpper  
}  // namespace ara