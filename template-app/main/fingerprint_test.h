
#include "driver/uart.h"
#include "crypto_test.h"

void setupFtdiConsole();
int getFtdiCommand();

int sendHandshake();
int testUartDoubleInstall();
int uartEchoTest();
int builtinEchoTest();
int testLibraryHandshake();
int testLibraryReadParam();
int testLibraryGenImg();
int testLibraryCaptureImage();
int testLibraryGenerateFileFromImg(uint8_t bufferID);
int testLibraryGenerateTemplate();
int testLibraryStoreTemplate(uint16_t pageID);
int testLibraryMatchSearch(uint8_t bufferID);
int testLibraryLoadTemplate(uint8_t bufferID, uint16_t pageID);
int testLibraryClearTemplates();
int testLibraryUploadFile(uint8_t bufferID);
int testLibraryGenerateCryptoKey();
int testLibraryLedCtrl(int state);
