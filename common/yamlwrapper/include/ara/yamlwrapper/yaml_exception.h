/*
 * @Description: 
 * @Author: an.huang@tusen.ai
 * @Date: 2022-12-06 03:39:39
 * @LastEditors: an.huang
 * @LastEditTime: 2022-12-07 07:28:34
 * @FilePath: /root/adas-monorepo/modules/adaptive_autosar/ara-api/common/yamlwrapper/include/yaml_exception.h
 * @version: 
 *  
 * Copyright (c) 2022 by Tusen, All Rights Reserved.
 */
#include <iostream>
#include <exception>
using namespace std;
namespace ara{
namespace yamlwrapper{

enum class ExceptionSemantics{
    kInvalidDirException,
    kRelativeDirException,
    kAccessDeniedException,
    kMaxProgcessFilesException,
    kMaxSystemFilesException,
    kNoMemoryException,
    kBadFileException
};

const char* Semantics(ExceptionSemantics i){
    switch(i){
        case ExceptionSemantics::kInvalidDirException:
            return "Invalid directory of YAML file! Please check the input path or environment variables!";
        case ExceptionSemantics::kRelativeDirException:
            return "Relative path of YAML file is not supported! Please check the input path or environment variables!";
        case ExceptionSemantics::kAccessDeniedException:
            return "Access denied while opening given YAML file! Please make sure you have rights to access it!";
        case ExceptionSemantics::kMaxProgcessFilesException:
            return "Open given YAML file failed! Reach the maximum number of files in the process!";
        case ExceptionSemantics::kMaxSystemFilesException:
            return "Open given YAML file failed! Reach the maximum number of files in the system!";
        case ExceptionSemantics::kNoMemoryException:
            return "No memory error occurs!";
        case ExceptionSemantics::kBadFileException:
            return "Invalid file handler, error occurs!";
    }
}

// errno: ENOTDIR, ENOENT
class InvalidDirException : public exception{
    // ExceptionSemantics tmp = ExceptionSemantics::InvalidDirExceptionSemantic;
    const char* what() const noexcept{
        return Semantics(ExceptionSemantics::kInvalidDirException);
    }
};
class RelativeDirException : public exception{
    const char* what() const noexcept{
        return Semantics(ExceptionSemantics::kRelativeDirException);
    }
};
// errno: EACCES
class AccessDeniedException : public exception{
        const char* what() const noexcept{
        return Semantics(ExceptionSemantics::kAccessDeniedException);
    }
};
// errno: EMFILE
class MaxProgcessFilesException : public exception{
        const char* what() const noexcept{
        return Semantics(ExceptionSemantics::kMaxProgcessFilesException);
    }
};
// errno: ENFILE
class MaxSystemFilesException : public exception{
        const char* what() const noexcept{
        return Semantics(ExceptionSemantics::kMaxSystemFilesException);
    }
};
// errno: ENOMEM
class NoMemoryException : public exception{
        const char* what() const noexcept{
            return Semantics(ExceptionSemantics::kNoMemoryException);
        }
};
// errno: EBADF
class BadFileException : public exception{
        const char* what() const noexcept{
            return Semantics(ExceptionSemantics::kBadFileException);
        }
};

}
}