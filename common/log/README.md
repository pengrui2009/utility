<!--
 * @Author: renwen.wang renwen.wang@tusen.ai
 * @Date: 2022-06-08 11:41:06
 * @LastEditors: renwen.wang renwen.wang@tusen.ai
 * @LastEditTime: 2023-02-15 14:52:55
 * @FilePath: /aeg-adaptive-autosar/ara-api/log/README.md
 * @Description: 
 * 
 * Copyright (c) 2022 by Tusimple, All Rights Reserved. 
-->
# ap-log使用方法

## 基本概念
### 1. 架构介绍
https://tusimple.feishu.cn/wiki/wikcndbH4iJm95yNBpc5FBDTkSg?sheet=Leo5mn#ZV4PnT
***
### 2. 初始化
#### 2.1 ara::log::Initialize
根据配置文件内容进行初始化。
log的配置可通过ap工具链自动生成，本文档以最终生成的配置文件进行说明。
配置文件路径默认为/usr/local/etc/ap_log/Log_configuration.json。同时支持通过环境变量AP_LOG_CONFIG_FILE指定。若配置文件不存在，或相关配置不满足预设条件（如类型不一致，范围不匹配等），将采用默认配置。
目前支持的配置如下（仅包含了log相关的配置，其他非log相关配置不在此体现）：
```json
{
    /*Application ID. Max up to 4 characters. DFAULT:"XXXX"*/
    "log_app_id": "tuap",
    /*Application Description. DFAULT:""*/
    "log_app_description": "Tu-AP log",
    /*Log level. string:kOff,kFatal,kError,kWarn,kInfo,kDebug,kVerbose DFAULT:"kWarn" */
    "log_default_level": "kVerbose",
    /*Log Mode. string array:[kRemote,kFile,kConsole]. DFAULT:"kConsole" */
    "log_mode": [
        "kConsole",
        "kFile",
        "kRemote"
    ],
    /*Message mode. string:kModeled,kNonModeled DEFAULT:"kNonModeled" */
    "log_message_mode": "kNonModeled",
    /* File save path. Support for both relative and absolute path. DEFAULT:"./"
     * You can also specific the filename with .txt or .dlt sufix, 
     * which represent ascii encode or binary encode 
     */
    "log_file_path": "./log/"
}
```
* `log_app_id`:当前应用的ID，最大4个字符，超过4个的部分会被截取。 
* `log_app_description`:当前应用描述。
* `log_default_level`:当前application默认的loglevel，其生效规则如下：
    * 对初始化之前创建的logger：
        * 若用户通过CreateLogger接口传入loglevel，则使用用户传入的level；
        * 若用户未通过CreateLogger接口传入loglevel，则使用默认等级kWarn；
        * 初始化之后，该logger的等级会被修改为Execution manifest中配置的level；
        * 若用户没有在Execution manifest中配置level，则仍然保持kWarn
    * 对初始化之后创建的logger：
        * 若用户通过CreateLogger接口传入loglevel，则使用用户传入的level；
        * 若用户未通过CreateLogger接口传入loglevel，则使用Execution manifest中配置的level；
        * 若用户没有在Execution manifest中配置配置level，则使用默认等级kWarn
* `log_mode`:[LogMode](#5-logmode)
    * 在`log_mode`为`kFile`时，可通过环境变量`AP_LOG_FILE_SAVE_MODE`指定文件保存模式（默认为`kSize`），当前支持模式包括：</br>

        |kSize|kMinutely|kHourly|kDaily|
        |-|-|-|-|
        |按照文件大小保存，其中文件大小通过环境变量`AP_LOG_FILE_SIZE`指定，超过指定的大小后，自动创建新的日志文件|每时钟的整分钟保存一次文件|每时钟的整小时保存一次文件|每时钟零点保存一次文件|
* `log_message_mode`:应用全局的message mode，kModeled和kNonModeled
    * ```kModeled```:该模式下message输出同时支持Modeled类型和Non-Modeled类型，具体取决于调用的接口。
    * ```kNonModeled```:该模式下只支持Non-Modeled类型的message输出。
* `log_file_path`:文件保存路径
    * 如果在`log_file_path`中指定了文件名（即变量以.txt或.dlt结尾），则会将其当作日志文件名，
    并根据文件后缀自动识别要保存的日志编码格式，txt对应ascii，dlt对应binary；如果没有指定文件名，
    默认以app id作为文件名，文件编码为ascii。若指定的路径不存在，则自动创建指定路径（需要具有写权限）。
    * 在`log_mode`为`kFile`时，且文件保存模式为`kSize`时，可通过环境变量`AP_LOG_MAX_FILES`指定最大文件保存数量，默认为5.

在AP 21-11的规范中，Log模块应当通过`ara::core::Initialize()`初始化，该接口可同时初始化多个模块。若仅初始化Log模块，仅调用`ara::log::Initialize()`即可。
***
### 3. Logger
Logger对象对应于一个日志context，每个进程中可存在多个Logger对象，可以为不同的Logger分别设置日志等级。

Logger对象不可拷贝、赋值和移动。

禁止通过Logger的构造函数创建Logger对象。正确的做法是使用`CreateLogger()`来获取对Logger对象的引用。

创建Logger对象时需要指定Context Id，如果传入的Context Id已经存在，则直接返回该Context Id对应的Logger对象。

每个app会自动创建一个context ID为**INTM**的Logger对象。当通过`createLogger()`创建Logger时传入ctxId=INTM或发生内部错误时，会返回该Logger对象。在调用`CreateLogger()`必须指定ctxId，否则会返回内置的Logger对象。
***
### 4. LogStream
LogStream是用于输出日志message的对象。通常情况下，通过调用Logger对象的方法来生成一个临时LogStream对象。LogStream在析构时会自动调用其```Flush()```方法来输出日志内容。也可以将临时生成的LogStream对象保存在另一个变量中，这时如果要刷新日志内容，需要手动调用```Flush()```方法。
***
### 5. LogMode 
LogMode分为三种：```kFile```、```kConsole```和```kRemote```。其中，kFile模式下的文件编码又可以分为两种，分别为```kAscii```和```kBinary```，通过枚举类型```FileEncode```指定。

在```kFile```和```kConsole```模式下，存在无法记录early message的情况。这是因为early message在记录时，kFile和kConsole还没有被初始化。可以通过设置环境变量来默认开启```kFile```或```kConsole```模式，从而使这两种模式下也可以记录early message。

**什么是Early message？**
> 在初始化调用完成之前记录的message。
***
### 6. Non-modeled Message
对应于LT protocol中的verbose message。LT默认使用该模式的message来记录消息。
***
### 7. Modeled Message
对应于LT protocol中的non-verbose message。该模式下不显示attribute的Name和Unit，需要通过dlt-viewer和对应的配置文件重新生成完成的message。
***
### 8. Attribute
适于用Non-modeled message和Modeled message。默认在message中会显示attribute，后续将通过开放接口来控制是否打印。
***
## 关于Thread Safe
kConsole和kFile都是线程安全的，不会产生内容错乱的现象。但存在message生成顺序与日志中顺序不一致的现象。
***
## 日志格式解析
|DATE|TIME|TIMESTAME|MESSAGE COUNTER|ECU ID|APP ID|CTX ID|TYPE|SUB TYPE|VERBOSE/</br>NON-VERBOSE|ARGUMENTS NUMBER|PAYLOAD|
|-|-|-|-|-|-|-|-|-|-|-|-|
|2022/06/22|17:27:46.396979|1996466923|024|ECU1|TSEN|LOG-|log|info|V|1|[0x00000064]|
|2022/06/22|17:27:46.397659|1996466930|031|ECU1|TSEN|LOG-|log|info|V|2|[source_file:/home/tusen/workspace/aeg-adaptive-autosar/ara-api/log/test/test.cpp line_number:130]|

>2022/06/22 17:27:46.396979 1996466923 024 ECU1 TSEN LOG- log info V 1 [0x00000064]</br>
2022/06/22 17:27:46.397659 1996466930 031 ECU1 TSEN LOG- log debug V 2 [source_file:/home/tusen/workspace/aeg-adaptive-autosar/ara-api/log/test/test.cpp line_number:130]</br>

* `MESSAGE COUNTER`:当前context id下产生的message数量，最大255,到达255后从零开始。不同context id分别计数。
* `TIMESTAMP`:从系统启动以便经过的时间，单位是0.1ms，即从系统启动到日志生成时经历了数值×0.1ms。
* `ARGUMENTS NUMBER`:Payload中Arguments的数量。多个Arguments之间使用空格相隔。如果指定的Arguments的Name和Unit，则相应的值会显示在Arguments前面。默认开启Name和Unit显示。
* `PAYLOAD`：包含了所有的Arguments以及额外的arguments信息（仅non-modeled模式下显示）。

## 使用方法
### Step1 初始化
```c++
#include <ara/log/logger.h>

/*仅初始化log:  ara::log::Initialize()*/
/*多次调用该接口时，仅第一次调用生效，第一次之后的调用均将被忽略，不会起任何作用*/
/*若打印初始化配置信息需要设置环境变量：export DLT_LOCAL_PRINT_MODE=FORCE_ON*/
ara::log::InitLogging();
```
### Step2 创建Logger实例
```c++
/**
 * 如果传入重复的ctxid，将会返回已创建logger对象;
 * 如果创建失败，将会返回内置的logger对象;
 * */
ara::log::Logger& m_logger{ara::log::CreateLogger(ctxid, ctxdesc, ara::log::LogLevel::kDebug)};
```
### Step3 打印
```c++
/* LogLevel检查 */
m_logger.IsEnabled(LogLevel::kError);
/*测试用变量 */
uint8_t a1 = 100;
uint16_t a2 = 100;
uint32_t a3 = 100;
uint64_t a4 = 100;
int8_t b1 = 100;
int16_t b2 = -100;
int32_t b3 = 100;
int64_t b4 = -100;
float f1 = 1.23f;
double f2 = 3.14159;
ara::core::StringView sv = "string view";
std::string s = "hello tusimple";
/* LogLevel::kFatal */
m_logger.LogFatal() << a1;      //打印uint8_t类型
m_logger.LogFatal() << a2;      //打印uint16_t类型
m_logger.LogFatal() << a3;      //打印uint32_t类型
m_logger.LogFatal() << a4;      //打印uint64_t类型
/* LogLevel::kError */
m_logger.LogError() << b1;      //打印int8_t类型
m_logger.LogError() << b2;      //打印int16_t类型
m_logger.LogError() << b3;      //打印int32_t类型
m_logger.LogError() << b4;      //打印int64_t类型
/* LogLevel::kWarn */
m_logger.LogWarn() << BinFormat(a1);  //打印LogBin8类型
m_logger.LogWarn() << BinFormat(a2);  //打印LogBin16类型
m_logger.LogWarn() << BinFormat(b3);  //打印LogBin32类型
m_logger.LogWarn() << BinFormat(b4);  //打印LogBin64类型
/* LogLevel::kInfo */
m_logger.LogInfo() << HexFormat(b1);  //打印LogHex8类型
m_logger.LogInfo() << HexFormat(b2);  //打印LogHex16类型
m_logger.LogInfo() << HexFormat(a3);  //打印LogHex32类型
m_logger.LogInfo() << HexFormat(a4);  //打印LogHex64类型
/* LogLevel::kDebug */
m_logger.LogDebug() << true;                            //打印bool类型
m_logger.LogDebug() << f1 << f2;                        //打印float和double类型
m_logger.LogDebug() << sv;                              //打印StringView类型
m_logger.LogDebug() << s;                               //打印string类型
m_logger.LogDebug() << "123";                           //打印const char*类型
m_logger.LogDebug().WithLocation(__FILE__, __LINE__);   //以attribute方式打印文件名、行号
(m_logger.LogDebug() << "Behind WithLocation test").WithLocation(__FILE__, __LINE__) << " Should be no location";   //后接WithLocation将不显示

/* LogLevel::kVerbose */
m_logger.LogVerbose() << &a1;                                               //打印void*类型
m_logger.LogVerbose() << std::chrono::duration<uint8_t, std::micro>(200);   //打印duration ms类型
m_logger.LogVerbose() << std::chrono::duration<uint8_t, std::centi>(200);   //打印duration 10*ms类型
const ara::core::ErrorCode core_errc = ara::core::ErrorCode(ara::core::CoreErrc::connection_aborted);
m_logger.LogVerbose() << core_errc;                                         //打印ErrorCode
const ara::core::Span<ara::core::Byte> span("spantest", 8);
m_logger.LogVerbose() << span;                                              //打印Span
/* 使用WithLevel打印 */
m_logger.WithLevel(LogLevel::kError) << "WithLevel test";   //WithLevel测试
/* 打印Argument */
const ara::log::Argument<uint32_t> arg1{123, "time", "ms"};
const ara::log::Argument<ara::core::String> arg2{"string", "abc"};
m_logger.LogVerbose() << arg1;
m_logger.LogVerbose() << arg2;
```
### 通过LogStream打印
```c++
ara::log::LogStream log{(m_logger.LogInfo())};
log << "logstream builder";
log.Flush();
/* move logstream 原有的log调用不报错但不再可用*/
ara::log::LogStream ls = std::move(log);
ls << "move logStream";
ls.Flush();
```
## 其他须知
* 默认会将attribute的Name和Unit显示出来;
* 对Early message的支持
    |kFile|kRemote|kConsole|
    |-|-|-|
    |暂不支持|支持，在连接上dlt daemon后自动发送缓存的early message|支持，但需要设置环境变量:</br>export DLT_LOCAL_PRINT_MODE=FORCE_ON|
* 指定dlt pipe dir</br> 
dlt-daemon可通过-t指定pipe dir，也可通过`DLT_FIFO_BASE_DIR`环境变量指定。-t会覆盖`DLT_FIFO_BASE_DIR`设置的值。若都没有指定，默认为/tmp
当dlt-daemon改变默认(/tmp/dltpipes)的dlt pipe dir时， 对应log应用也需要通过指定环境变量:export DLT_PIPE_DIR=[path]，且与dlt-daemon中指定的pipe path保持一致。
* 控制台和文件模式换行显示
log默认不支持payload中含有换行符号，若需要开启，可通过配置环境变量:</br>```export DLT_ENABLE_NEW_LINE=True```</br>来开启换行。
* 更改log message payload大小</br>
默认值为1390，超出内容不会显示，若用户需要手动更改该值，可设置环境变量`export DLT_LOG_MSG_BUF_LEN=4000`。