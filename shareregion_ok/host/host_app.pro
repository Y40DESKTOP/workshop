MCSDK                    = /home/tl/ti
SYSLINK_INSTALL_DIR      = $$MCSDK/syslink_2_21_01_05
IPC_INSTALL_DIR          = $$MCSDK/ipc_1_25_03_15

TARGET       = host_app

TEMPLATE     = app

VPATH       += 

OBJECTS_DIR  = tmp
MOC_DIR      = tmp

CONFIG      += debug

SOURCES     += main.cpp syslink.c resource_sync.c 

HEADERS     += log.h syslink.h resource_sync.h ../shared/protocol.h ../shared/sys_config.h 

INCLUDEPATH += $$SYSLINK_INSTALL_DIR/packages      \
               $$IPC_INSTALL_DIR/packages          \
               ../shared

LIBS        += $$SYSLINK_INSTALL_DIR/packages/ti/syslink/lib/syslink.a_release
