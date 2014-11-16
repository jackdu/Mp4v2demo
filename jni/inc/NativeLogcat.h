#pragma once

#ifndef MY_LOGCAT_H_
#define MY_LOGCAT_H_

#include <android/log.h>
#include <string.h>

#ifdef LOG_TAG
    #undef LOG_TAG
#endif
#define LOG_TAG "Native Logcat"

// 将长文件名改为短文件名
#ifdef COMPART
    #undef COMPART
#endif

#ifdef _WIN32
    #define COMPART '\\'
#else
    #define COMPART '/'
#endif

static char* ConvertTOShortFileName(const char* pszFileName)
{
    char *pszPret = const_cast<char *>(pszFileName);
    char *pszNext = NULL;

    if(NULL == pszFileName)
    {
        return NULL;
    }

    if ((pszNext = strrchr(pszPret, COMPART)) != NULL)
    {
        pszPret = pszNext + 1;
    }

    return pszPret;
}



#define NL_LOGV(fmt, ...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, "[%s:%s](%d): " fmt, ConvertTOShortFileName(__FILE__), __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define NL_LOGD(fmt, ...) __android_log_print(ANDROID_LOG_DEBUG,   LOG_TAG, "[%s:%s](%d): " fmt, ConvertTOShortFileName(__FILE__), __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define NL_LOGI(fmt, ...) __android_log_print(ANDROID_LOG_INFO,    LOG_TAG, "[%s:%s](%d): " fmt, ConvertTOShortFileName(__FILE__), __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define NL_LOGW(fmt, ...) __android_log_print(ANDROID_LOG_WARN,    LOG_TAG, "[%s:%s](%d): " fmt, ConvertTOShortFileName(__FILE__), __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define NL_LOGE(fmt, ...) __android_log_print(ANDROID_LOG_ERROR,   LOG_TAG, "[%s:%s](%d): " fmt, ConvertTOShortFileName(__FILE__), __FUNCTION__, __LINE__, ##__VA_ARGS__)

#endif // MY_LOGCAT_H_
