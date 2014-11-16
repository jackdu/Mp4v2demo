#include <jni.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "NativeLogcat.h"
#include "mp4v2.h"

#undef LOG_TAG
#define LOG_TAG "Mp4v2demo Jni"

MP4FileHandle mp4File = NULL;
const char *mp4FilePathAndName = "/sdcard/video.mp4";
FILE *h264File = NULL;
const char *h264FilePathAndName = "/sdcard/video.h264";

const char *MY_BUILD_VERSION = "Mp4v2demo 1";

int32_t openFiles() {
    if (mp4File == NULL) {
        mp4File = MP4Read(mp4FilePathAndName);
    }
    if (mp4File == NULL) {
        NL_LOGE("mp4 file read error");
        return -1;
    }

    if (h264File == NULL) {
        h264File = fopen(h264FilePathAndName, "wb+");
    }
    if (h264File == NULL) {
        NL_LOGE("h264 file open error");
        return -1;
    }

    return 0;
}

int32_t closeFiles() {
    if (mp4File != NULL) {
        MP4Close(mp4File);
        mp4File = NULL;
    }
    if (h264File != NULL) {
        fclose(h264File);
        h264File = NULL;
    }
    return 0;
}

/**
 * Class:     "com_djn_mp4v2demo_MainActivity"
 * Method:    "InitNative"
 * Signature: "()I"
 */
JNIEXPORT jint JNICALL InitNative(JNIEnv *env, jobject context)
{
    NL_LOGD("================>Enter");

    NL_LOGW("Build Version:%s", MY_BUILD_VERSION);

    NL_LOGD("Leave================>");
    return 0;
}

/**
 * Class:     "com_djn_mp4v2demo_MainActivity"
 * Method:    "DeinitNative"
 * Signature: "()I"
 */
JNIEXPORT jint JNICALL DeinitNative(JNIEnv *env, jobject context)
{
    NL_LOGD("================>Enter");

    NL_LOGD("Leave================>");
    return 0;
}

/**
 * class:      "com_djn_mp4v2demo_MainActivity"
 * name:       "demuxH264FromMp4"
 * signature:  "()I"
 */
JNIEXPORT jint JNICALL demuxH264FromMp4(JNIEnv* env, jobject thiz)
 {
    openFiles();

    uint32_t trackId = MP4_INVALID_TRACK_ID;
    uint32_t numOfTracks = MP4GetNumberOfTracks(mp4File);

    // find video track
    for (uint32_t tmpTrackId = 1; tmpTrackId < numOfTracks; tmpTrackId++) {
        const char* trackType = MP4GetTrackType(mp4File, tmpTrackId);
        if (MP4_IS_VIDEO_TRACK_TYPE(trackType)) {
            trackId = tmpTrackId;
            break;
        }
    }
    if (trackId == MP4_INVALID_TRACK_ID) {
        NL_LOGE("Can't find video track\n");
        closeFiles();
        return -1;
    }

    uint32_t numSamples = MP4GetTrackNumberOfSamples(mp4File, trackId);
    uint8_t* pSample = NULL;
    uint32_t sampleSize = 0;
    const char nalHeader[] = {0x00, 0x00, 0x00, 0x01};

    // read sps/pps
    uint8_t **seqheader;
    uint8_t **pictheader;
    uint32_t *pictheadersize;
    uint32_t *seqheadersize;
    uint32_t ix;
    MP4GetTrackH264SeqPictHeaders(mp4File, trackId, &seqheader, &seqheadersize, &pictheader,
            &pictheadersize);
    fwrite(nalHeader, 4, 1, h264File);
    for (ix = 0; seqheadersize[ix] != 0; ix++) {
        fwrite(seqheader[ix], seqheadersize[ix], 1, h264File);
        free(seqheader[ix]);
    }
    free(seqheader);
    free(seqheadersize);
    fwrite(nalHeader, 4, 1, h264File);
    for (ix = 0; pictheadersize[ix] != 0; ix++) {
        fwrite(pictheader[ix], pictheadersize[ix], 1, h264File);
        free(pictheader[ix]);
    }
    free(pictheader);
    free(pictheadersize);

    // read samples
    for (uint32_t sampleId = 1; sampleId <= numSamples; sampleId++) {
        if (!MP4ReadSample(mp4File, trackId, sampleId, &pSample, &sampleSize)) {
            NL_LOGE("read sampleId %u error\n", sampleId);
            if (pSample != NULL) {
                free(pSample);
                pSample = NULL;
            }
            break;
        }

        fwrite(nalHeader, 4, 1, h264File);
        fwrite(pSample + 4, sampleSize - 4, 1, h264File);

        if (pSample != NULL) {
            free(pSample);
            pSample = NULL;
        }
    }

    closeFiles();
    return 0;
}

/*****************************************************************************/
/* Copy From This Line
/*****************************************************************************/
static JNINativeMethod methods[] = {
        { "InitNative",       "()I",    (void*)InitNative },
        { "DeinitNative",     "()I",    (void*)DeinitNative },
        { "demuxH264FromMp4", "()I",    (void*)demuxH264FromMp4}
};

/*
 * Register several native methods for one class.
 */
static int registerNativeMethods(JNIEnv* env, const char* className, JNINativeMethod* gMethods,
        int numMethods) {
    jclass clazz;

    clazz = env->FindClass(className);
    if (clazz == NULL) {
        NL_LOGE("Native registration unable to find class '%s'", className);
        return JNI_FALSE;
    }
    if (env->RegisterNatives(clazz, gMethods, numMethods) < 0) {
        NL_LOGE("RegisterNatives failed for '%s'", className);
        return JNI_FALSE;
    }

    return JNI_TRUE;
}

static const char *classPathName = "com/djn/mp4v2demo/MainActivity";


/*****************************************************************************/
/* Don't modify contents under this line
/*****************************************************************************/
JavaVM* JniGlobalVM;
const char *COMPILE_BUILD_TIME = __DATE__ ", " __TIME__; // Compile Time
/*
 * Register native methods for all classes we know about.
 */
static int registerNatives(JNIEnv* env) {
    if (!registerNativeMethods(env, classPathName, methods, sizeof(methods) / sizeof(methods[0]))) {
        return JNI_FALSE;
    }

    return JNI_TRUE;
}

typedef union {
    JNIEnv* env;
    void* vEnv;
} UnionJNIEnvToVoid;

/* This function will be call when the library first be loaded */
jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    UnionJNIEnvToVoid uEnv;
    JNIEnv* env = NULL;
    NL_LOGI("JNI_OnLoad!");
    NL_LOGI("Build Time:%s ", COMPILE_BUILD_TIME);
    JniGlobalVM = vm;
    if (vm->GetEnv((void**) &uEnv.vEnv, JNI_VERSION_1_4) != JNI_OK) {
        NL_LOGE("ERROR: GetEnv failed");
        return -1;
    }

    env = uEnv.env;

    if (registerNatives(env) != JNI_TRUE) {
        NL_LOGE("ERROR: registerNatives failed");
        return -1;
    }

    return JNI_VERSION_1_4;
}
/*****************************************************************************/
/* Don't modify contents before this line
/*****************************************************************************/
