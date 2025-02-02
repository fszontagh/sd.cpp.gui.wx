#ifndef __UI_THREADEVENTSTYPES__
#define __UI_THREADEVENTSTYPES__
namespace sd_gui_utils {
    enum class ThreadEvents {
        QUEUE,
        HASHING_PROGRESS,
        MODEL_LOAD_DONE,
        GENERATION_DONE,
        STANDALONE_HASHING_PROGRESS,
        STANDALONE_HASHING_DONE,
        HASHING_DONE,
        GENERATION_PROGRESS,
        SD_MESSAGE,
        MESSAGE,
        MODEL_INFO_DOWNLOAD_START,
        MODEL_INFO_DOWNLOAD_FAILED,
        MODEL_INFO_DOWNLOAD_FINISHED,
        MODEL_INFO_DOWNLOAD_IMAGES_START,
        MODEL_INFO_DOWNLOAD_IMAGES_PROGRESS,
        MODEL_INFO_DOWNLOAD_IMAGES_DONE,
        MODEL_INFO_DOWNLOAD_IMAGE_FAILED,
        MODEL_MOVE_START,
        MODEL_MOVE_FAILED,
        MODEL_MOVE_DONE,
        MODEL_MOVE_UPDATE,
        SERVER_CONNECTED,
        SERVER_DISCONNECTED,
        SERVER_AUTH_REQUEST,
        SERVER_ERROR,
        SERVER_MODEL_LIST_UPDATE,
        SERVER_AUTH_RESPONSE,
        SERVER_JOBLIST_UPDATE,
        SERVER_JOBLIST_UPDATE_START,
        SERVER_JOBLIST_UPDATE_FINISHED,
        SERVER_JOB_UPDATE,
        SERVER_JOB_DELETE,
        SERVER_JOB_IMAGE_UPDATE
    };
}
#endif  // __UI_THREADEVENTSTYPES__