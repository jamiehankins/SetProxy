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
            false;
    }
    WCharBuff sConnection;
    bool fDetectSettings;
    bool fUseConfigScript;
    WCharBuff sConfigScriptUrl;
    bool fUseProxyServer;
    WCharBuff sProxyServerUrl;
    bool fBypassLocal;
    bool fClear;
} SET_PROXY_OPTIONS, *PSET_PROXY_OPTIONS;
