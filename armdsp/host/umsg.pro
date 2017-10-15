MCSDK                    = /home/tl/ti
SYSLINK_INSTALL_DIR      = $$MCSDK/syslink_2_21_01_05
IPC_INSTALL_DIR          = $$MCSDK/ipc_1_25_03_15

TARGET       = umsg

TEMPLATE     = app

VPATH       += 

OBJECTS_DIR  = tmp
MOC_DIR      = tmp

CONFIG      += debug

SOURCES     += main.cpp syslink.c resource_sync.c 

HEADERS     += log.h syslink.h resource_sync.h 

INCLUDEPATH += $$SYSLINK_INSTALL_DIR/packages      \
               $$IPC_INSTALL_DIR/packages          

LIBS        += $$SYSLINK_INSTALL_DIR/packages/ti/syslink/lib/syslink.a_release \
               ../shared/umsg/lib/debug/umsg.av5T
