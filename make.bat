@echo off
nmake CC_ARGS="-std=c17 -lws2_32" CLIENT_BIN="client.exe" SERVER_BIN="server.exe" RM_CMD="del" RUN_PREFIX="" %*
