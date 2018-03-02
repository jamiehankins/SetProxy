#pragma once

typedef struct _SET_PROXY_OPTIONS
{
    _SET_PROXY_OPTIONS()
    {
        fDetectSettings =
            fUseConfigScript =
            fUseProxyServer =
            fBypassLocal =
            fClear =
            fEnum =
            false;
    }
    WCharBuff sConnection;
    std::string connRegEx;
    bool fDetectSettings;
    bool fUseConfigScript;
    WCharBuff sConfigScriptUrl;
    bool fUseProxyServer;
    WCharBuff sProxyServerUrl;
    bool fBypassLocal;
    bool fClear;
    bool fEnum;
} SET_PROXY_OPTIONS, *PSET_PROXY_OPTIONS;
