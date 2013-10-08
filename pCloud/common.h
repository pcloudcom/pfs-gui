#ifndef COMMON_H
#define COMMON_H

#define DEFAULT_USE_SSL      1
#define DEFAULT_PCLOUD_DIR   "pCloud"
#define DEFAULT_CACHE_SIZEMB 1024

#ifdef Q_OS_MAC

#define WINDOW_ICON ":/images/images/mac-desktop.png"
#define ONLINE_ICON ":/images/images/mac-systray.png"
#define OFFLINE_ICON ":/images/images/mac-systray.png"

#else

#define WINDOW_ICON ":/images/images/online.png"
#define ONLINE_ICON ":/images/images/online.png"
#define OFFLINE_ICON ":/images/images/offline.png"

#endif

#endif
