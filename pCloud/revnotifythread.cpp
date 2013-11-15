#include "revnotifythread.h"
#include "pcloudapp.h"

#ifdef Q_OS_WIN
#include <Shlobj.h>

#define  PIPE_NAME L"\\\\.\\pipe\\pfsnotifypipe"

struct notification
{
    quint32 size;
    quint32 event1;
    quint32 event2;
    char message[4*MAX_PATH];
};


RevNotifyThread::RevNotifyThread(PCloudApp *a)
{
    app=a;
    hPipe = CreateNamedPipe(PIPE_NAME, PIPE_ACCESS_INBOUND,
        PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
        1, 0, sizeof(notification), 0, NULL);
}

static void fix_path(wchar_t *res){
  while (*res){
    if (*res == L'/')
      *res = L'\\';
      ++res;
  }
}

static void utf8_to_wchar(const char *src, wchar_t *res, int maxlen){
  if (src==NULL || res==NULL || maxlen==0) return;

  int ln=MultiByteToWideChar(CP_UTF8,0,src,-1,NULL,0);
  if (ln>=maxlen){
    *res=L'\0';
    return;
  }
  MultiByteToWideChar(CP_UTF8,0,src,-1,res,(int)(strlen(src)+1));
  fix_path(res);
}

static void sendNotification(const notification* n, char mount)
{
    wchar_t win_string[2*MAX_PATH];
    win_string[0] = (wchar_t)mount;
    win_string[1] = L':';

    if (!n) return;
    utf8_to_wchar(n->message, win_string+2, 2*MAX_PATH-2);
    if (n->event1){
        SHChangeNotify(n->event1, SHCNF_PATH|SHCNF_FLUSH, win_string, NULL);
    }
    if (n->event2){
        SHChangeNotify(n->event2, SHCNF_PATH|SHCNF_FLUSH, win_string, NULL);
    }
}

void RevNotifyThread::run()
{
    notification n;

    while (app->isLogedIn()){
        DWORD read = 0, total;
        if (hPipe != INVALID_HANDLE_VALUE)
        {
            if (!ConnectNamedPipe(hPipe, NULL) && GetLastError() != ERROR_PIPE_CONNECTED){
                break;
            }
        }
        else
        {
            return;
        }
        n.size = 0;
        if (ReadFile(hPipe, &n, sizeof(n), &read, NULL)){
            if (read > 0 && read < 4) break;
            total = read;
            while (read && total < n.size){
                ReadFile(hPipe, ((char*)&n+total), n.size-total, &read, NULL);
                total += read;
            }
            sendNotification(&n, app->settings->get("path").toStdString().c_str()[0]);
        }
        else{
        }
    }
    if (hPipe != INVALID_HANDLE_VALUE){
        FlushFileBuffers(hPipe);
        DisconnectNamedPipe(hPipe);
    }
}

void RevNotifyThread::killPipe(){
    if (hPipe != INVALID_HANDLE_VALUE){
        CloseHandle(hPipe);
        hPipe = INVALID_HANDLE_VALUE;
    }
}

RevNotifyThread::~RevNotifyThread(){
    killPipe();
}

#endif
