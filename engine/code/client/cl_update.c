#include "client.h"
#include "cl_update.h"

#include <ctype.h>
#include <string.h>

#define CL_UPDATE_URL "https://ladder.q3rally.com/version.txt"
#define CL_UPDATE_BUFFER_SIZE 1024

static cvar_t *cl_updateStateCvar;
static cvar_t *cl_updateRemoteCvar;
static cvar_t *cl_updateDateCvar;
static cvar_t *cl_updateErrorCvar;

typedef struct {
    qboolean running;
    qboolean attempted;
#ifdef USE_CURL
    CURL *easyHandle;
    CURLM *multiHandle;
    char errorText[CURL_ERROR_SIZE];
#endif
    size_t bufferLength;
    char buffer[CL_UPDATE_BUFFER_SIZE];
} clUpdateContext_t;

static clUpdateContext_t cl_updateContext;

static void CL_UpdateVersionCheck_SetState(const char *state) {
    if (cl_updateStateCvar) {
        Cvar_Set(cl_updateStateCvar->name, state);
    }
}

static void CL_UpdateVersionCheck_SetRemote(const char *value) {
    if (cl_updateRemoteCvar) {
        Cvar_Set(cl_updateRemoteCvar->name, value);
    }
}

static void CL_UpdateVersionCheck_SetDate(const char *value) {
    if (cl_updateDateCvar) {
        Cvar_Set(cl_updateDateCvar->name, value);
    }
}

static void CL_UpdateVersionCheck_SetError(const char *value) {
    if (cl_updateErrorCvar) {
        Cvar_Set(cl_updateErrorCvar->name, value);
    }
}

static void CL_UpdateVersionCheck_ResetBuffers(void) {
    cl_updateContext.bufferLength = 0;
    cl_updateContext.buffer[0] = '\0';
#ifdef USE_CURL
    cl_updateContext.errorText[0] = '\0';
#endif
}

#ifdef USE_CURL
static size_t CL_UpdateVersionCheck_WriteCallback(void *ptr, size_t size, size_t nmemb, void *userdata) {
    clUpdateContext_t *ctx = (clUpdateContext_t *)userdata;
    size_t bytes = size * nmemb;
    size_t spaceLeft;
    size_t copyBytes;

    if (!ctx) {
        return bytes;
    }

    spaceLeft = (CL_UPDATE_BUFFER_SIZE - 1) - ctx->bufferLength;
    copyBytes = bytes;
    if (copyBytes > spaceLeft) {
        copyBytes = spaceLeft;
    }

    if (copyBytes > 0) {
        memcpy(ctx->buffer + ctx->bufferLength, ptr, copyBytes);
        ctx->bufferLength += copyBytes;
        ctx->buffer[ctx->bufferLength] = '\0';
    }

    return bytes;
}

static void CL_UpdateVersionCheck_CleanupHandles(void) {
    if (cl_updateContext.multiHandle && cl_updateContext.easyHandle) {
        qcurl_multi_remove_handle(cl_updateContext.multiHandle, cl_updateContext.easyHandle);
    }
    if (cl_updateContext.easyHandle) {
        qcurl_easy_cleanup(cl_updateContext.easyHandle);
        cl_updateContext.easyHandle = NULL;
    }
    if (cl_updateContext.multiHandle) {
        qcurl_multi_cleanup(cl_updateContext.multiHandle);
        cl_updateContext.multiHandle = NULL;
    }
    cl_updateContext.running = qfalse;
}

static void CL_UpdateVersionCheck_Fail(const char *message) {
    CL_UpdateVersionCheck_SetRemote("");
    CL_UpdateVersionCheck_SetDate("");
    CL_UpdateVersionCheck_SetState("failed");
    CL_UpdateVersionCheck_SetError(message && *message ? message : "Download failed");
    CL_UpdateVersionCheck_CleanupHandles();
}

static void CL_UpdateVersionCheck_ParseBuffer(void) {
    char *versionLine = cl_updateContext.buffer;
    char *dateLine = NULL;
    char *displayVersion;
    const char *localVersion;
    const char *remoteVersion;
    char *cursor;

    cl_updateContext.buffer[cl_updateContext.bufferLength] = '\0';

    cursor = strpbrk(versionLine, "\r\n");
    if (cursor) {
        *cursor = '\0';
        cursor++;
        while (*cursor == '\r' || *cursor == '\n') {
            cursor++;
        }
        if (*cursor) {
            dateLine = cursor;
            cursor = strpbrk(dateLine, "\r\n");
            if (cursor) {
                *cursor = '\0';
            }
        }
    }

    if (versionLine && *versionLine) {
        size_t len;

        if ((unsigned char)versionLine[0] == 0xEF &&
            (unsigned char)versionLine[1] == 0xBB &&
            (unsigned char)versionLine[2] == 0xBF) {
            versionLine += 3;
        }

        while (*versionLine && isspace((unsigned char)*versionLine)) {
            versionLine++;
        }

        len = strlen(versionLine);
        while (len > 0 && isspace((unsigned char)versionLine[len - 1])) {
            versionLine[--len] = '\0';
        }

        if (len >= 2 &&
            ((versionLine[0] == '"' && versionLine[len - 1] == '"') ||
             (versionLine[0] == '\'' && versionLine[len - 1] == '\''))) {
            versionLine[len - 1] = '\0';
            versionLine++;

            while (*versionLine && isspace((unsigned char)*versionLine)) {
                versionLine++;
            }

            len = strlen(versionLine);
            while (len > 0 && isspace((unsigned char)versionLine[len - 1])) {
                versionLine[--len] = '\0';
            }
        }
    }

    if (!versionLine || !*versionLine) {
        CL_UpdateVersionCheck_Fail("Invalid version file");
        return;
    }

    if (dateLine) {
        size_t len = strlen(dateLine);
        while (len > 0 && isspace((unsigned char)dateLine[len - 1])) {
            dateLine[--len] = '\0';
        }
        while (*dateLine && isspace((unsigned char)*dateLine)) {
            dateLine++;
        }
    }

    displayVersion = versionLine;
    CL_UpdateVersionCheck_SetRemote(displayVersion);
    CL_UpdateVersionCheck_SetDate(dateLine ? dateLine : "");
    CL_UpdateVersionCheck_SetError("");

    remoteVersion = displayVersion;
    localVersion = PRODUCT_VERSION;

    if ((remoteVersion[0] == 'v' || remoteVersion[0] == 'V') &&
        isalnum((unsigned char)remoteVersion[1])) {
        remoteVersion++;
    }

    if ((localVersion[0] == 'v' || localVersion[0] == 'V') &&
        isalnum((unsigned char)localVersion[1])) {
        localVersion++;
    }

    if (!Q_stricmp(remoteVersion, localVersion)) {
        CL_UpdateVersionCheck_SetState("current");
    } else {
        CL_UpdateVersionCheck_SetState("outdated");
    }
}
#endif /* USE_CURL */

void CL_UpdateVersionCheck_Register(void) {
    cl_updateStateCvar = Cvar_Get("cl_updateState", "idle", CVAR_TEMP);
    cl_updateRemoteCvar = Cvar_Get("cl_updateRemote", "", CVAR_TEMP);
    cl_updateDateCvar = Cvar_Get("cl_updateDate", "", CVAR_TEMP);
    cl_updateErrorCvar = Cvar_Get("cl_updateError", "", CVAR_TEMP);

    CL_UpdateVersionCheck_SetState("idle");
    CL_UpdateVersionCheck_SetRemote("");
    CL_UpdateVersionCheck_SetDate("");
    CL_UpdateVersionCheck_SetError("");

    cl_updateContext.running = qfalse;
    cl_updateContext.attempted = qfalse;
    CL_UpdateVersionCheck_ResetBuffers();
#ifdef USE_CURL
    cl_updateContext.easyHandle = NULL;
    cl_updateContext.multiHandle = NULL;
#endif
}

void CL_UpdateVersionCheck_Begin(void) {
    if (!cl_updateStateCvar) {
        CL_UpdateVersionCheck_Register();
    }

    if (cl_updateContext.running || cl_updateContext.attempted) {
        return;
    }

    cl_updateContext.attempted = qtrue;

#ifndef USE_CURL
    CL_UpdateVersionCheck_SetState("unavailable");
    CL_UpdateVersionCheck_SetError("cURL support not compiled");
    return;
#else
    if (!CL_cURL_Init()) {
        CL_UpdateVersionCheck_SetState("failed");
        CL_UpdateVersionCheck_SetError("cURL initialization failed");
        return;
    }

    cl_updateContext.easyHandle = qcurl_easy_init();
    if (!cl_updateContext.easyHandle) {
        CL_UpdateVersionCheck_SetState("failed");
        CL_UpdateVersionCheck_SetError("Failed to create cURL easy handle");
        return;
    }

    cl_updateContext.multiHandle = qcurl_multi_init();
    if (!cl_updateContext.multiHandle) {
        qcurl_easy_cleanup(cl_updateContext.easyHandle);
        cl_updateContext.easyHandle = NULL;
        CL_UpdateVersionCheck_SetState("failed");
        CL_UpdateVersionCheck_SetError("Failed to create cURL multi handle");
        return;
    }

    CL_UpdateVersionCheck_ResetBuffers();

    qcurl_easy_setopt(cl_updateContext.easyHandle, CURLOPT_URL, CL_UPDATE_URL);
    qcurl_easy_setopt(cl_updateContext.easyHandle, CURLOPT_WRITEFUNCTION, CL_UpdateVersionCheck_WriteCallback);
    qcurl_easy_setopt(cl_updateContext.easyHandle, CURLOPT_WRITEDATA, &cl_updateContext);
    qcurl_easy_setopt(cl_updateContext.easyHandle, CURLOPT_ERRORBUFFER, cl_updateContext.errorText);
    qcurl_easy_setopt(cl_updateContext.easyHandle, CURLOPT_NOPROGRESS, 1L);
    qcurl_easy_setopt(cl_updateContext.easyHandle, CURLOPT_FOLLOWLOCATION, 1L);
    qcurl_easy_setopt(cl_updateContext.easyHandle, CURLOPT_MAXREDIRS, 4L);
    qcurl_easy_setopt(cl_updateContext.easyHandle, CURLOPT_TIMEOUT, 10L);
#ifdef CURLOPT_USERAGENT
    qcurl_easy_setopt(cl_updateContext.easyHandle, CURLOPT_USERAGENT, va("%s %s", PRODUCT_NAME, PRODUCT_VERSION));
#endif

    if (qcurl_multi_add_handle(cl_updateContext.multiHandle, cl_updateContext.easyHandle) != CURLM_OK) {
        CL_UpdateVersionCheck_SetState("failed");
        CL_UpdateVersionCheck_SetError("Failed to queue version request");
        CL_UpdateVersionCheck_CleanupHandles();
        return;
    }

    cl_updateContext.running = qtrue;
    CL_UpdateVersionCheck_SetState("checking");
    CL_UpdateVersionCheck_SetRemote("");
    CL_UpdateVersionCheck_SetDate("");
    CL_UpdateVersionCheck_SetError("");
#endif
}

void CL_UpdateVersionCheck_Frame(void) {
#ifndef USE_CURL
    return;
#else
    int runningHandles = 0;
    CURLMcode multiResult;

    if (!cl_updateContext.running || !cl_updateContext.multiHandle) {
        return;
    }

    do {
        multiResult = qcurl_multi_perform(cl_updateContext.multiHandle, &runningHandles);
    } while (multiResult == CURLM_CALL_MULTI_PERFORM);

    if (multiResult != CURLM_OK) {
        CL_UpdateVersionCheck_Fail(qcurl_multi_strerror(multiResult));
        return;
    }

    if (runningHandles == 0) {
        int msgsInQueue;
        CURLMsg *msg;

        while ((msg = qcurl_multi_info_read(cl_updateContext.multiHandle, &msgsInQueue)) != NULL) {
            if (msg->easy_handle != cl_updateContext.easyHandle) {
                continue;
            }

            if (msg->msg == CURLMSG_DONE) {
                if (msg->data.result == CURLE_OK) {
                    CL_UpdateVersionCheck_ParseBuffer();
                    CL_UpdateVersionCheck_CleanupHandles();
                } else {
                    const char *err = cl_updateContext.errorText[0] ? cl_updateContext.errorText : qcurl_easy_strerror(msg->data.result);
                    CL_UpdateVersionCheck_Fail(err);
                }
            }
        }
    }
#endif
}

void CL_UpdateVersionCheck_Shutdown(void) {
#ifdef USE_CURL
    if (cl_updateContext.multiHandle || cl_updateContext.easyHandle) {
        CL_UpdateVersionCheck_CleanupHandles();
    }
#endif
    cl_updateContext.running = qfalse;
}
