#ifndef USK_CBOR_H
#define USK_CBOR_H

#include "nanocbor/nanocbor.h"

enum CborCmd
{
    // Firmware commands.
    CborCmd_FwWrite,
    // littlefs file commands.
    CborCmd_FsRemove,
    CborCmd_FsOpen,
    CborCmd_FsClose,
    CborCmd_FsStat,
    CborCmd_FsRename,
    CborCmd_FsRead,
    CborCmd_FsWrite,
    CborCmd_FsSeek,
    CborCmd_FsTrunc,
    CborCmd_FsTell,
    // littlefs directory commands.
    CborCmd_FsMkDir,
    CborCmd_FsDirOpen,
    CborCmd_FsDirClose,
    CborCmd_FsDirRead,
    CborCmd_FsDirSeek,
    CborCmd_FsDirTell,
    CborCmd_FsDirRewind,
    // littlefs code to string.
    CborCmd_FsErrMsg,
    // Wren script commands.
    CborCmd_WrenLoad,
    CborCmd_WrenCall,
    CborCmd_WrenKill,
};

bool handle_cbor(uint8_t* data_cbor, uint8_t* len_cbor);

#endif //USK_CBOR_H