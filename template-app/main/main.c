/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "crypto_test.h"
#include "fingerprint_test.h"
#include "uart_setup.h"


void app_main(void)
{
    printf("Hello world!\n");

    /* Print chip information */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("This is ESP32 chip with %d CPU cores, WiFi%s%s, ",
            chip_info.cores,
            (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
            (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    printf("silicon revision %d, ", chip_info.revision);

    printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
            (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    for (int i = 10; i >= 0; i--) {
        printf("Restarting in %d seconds...\n", i);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    encodetest();

    printf("Running test shell\n");
    int cmd = -1;
    const char* const cmdList[] = {
        "0. Exit\n",
        "1. Send Handshake\n",
        "2. Library Handshake\n",
        "3. Library Generate Img\n",
        "4. Library Capture Image\n",
        "5. Library Generate File From Image (Buffer 1)\n",
        "6. Library Generate File From Image (Buffer 2)\n",
        "7. Library Generate Template\n",
        "8. Library Store Template\n",
        "9. Library Match Search\n",
        "10. Library Load Template\n",
        "11. Library Clear All Templates\n",
        "12. Download Fingerprint Image\n",
        "13. Read System Parameters\n",
        "14. Create Crypto Key\n",
        "15. Led Control\n",
        "16. High Level Check Finger Enrolled\n",
        "17. High Level Enroll Finger\n",
        "18. High Level Authenticate Finger\n",
        "19. High Level Get Crypto Key\n",
        "20. High Level Clear All Data\n"
    };
    setupFtdiConsole();
    while (cmd != 0){
        printf("\n\nAvailable commands:\n");
        for(int i = 0; i < 21; i++){
            printf("%s", cmdList[i]);
        }
        printf("Please enter desired sensor command as number...\n");
        cmd = getFtdiCommand();
        printf("\n\n"); 
        switch(cmd){
            case 0:
                printf("Exiting test shell...\n");
                break;
            case 1:
                printf("Running raw handshake command...\n");
                sendHandshake();
                break;
            case 2:
                printf("Running library handshake test...\n");
                testLibraryHandshake();
                break;
            case 3:
                printf("Running library generate image test...\n");
                testLibraryGenImg();
                break;
            case 4:
                printf("Running library capture image test...\n");
                testLibraryCaptureImage();
                break;
            case 5:
                printf("Running library generate file from image (buffer 1) test...\n");
                testLibraryGenerateFileFromImg(0x01);
                break;
            case 6:
                printf("Running library generate file from image (buffer 2) test...\n");
                testLibraryGenerateFileFromImg(0x02);
                break;
            case 7:
                printf("Running library generate template test...\n");
                testLibraryGenerateTemplate();
                break;
            case 8:
                printf("Running library store template test...\n");
                printf("Please enter an ID for storing template...\n");
                uint16_t pageID_store = (uint16_t)getFtdiCommand();
                printf("Storing template under ID: %d...", pageID_store);
                testLibraryStoreTemplate(pageID_store);
                break;
            case 9:
                printf("Running match search test...\n");
                testLibraryMatchSearch(0x01);
                break;
            case 10:
                printf("Running load template test...\n");
                printf("Please enter the template ID to load...\n");
                uint16_t pageID_load = (uint16_t)getFtdiCommand();
                printf("Please enter desired buffer to load to...\n");
                uint8_t bufferID = (uint8_t)getFtdiCommand();
                printf("Attempting to load template %d to buffer %d...\n", pageID_load, bufferID);
                testLibraryLoadTemplate(bufferID, pageID_load);
                break;
            case 11:
                printf("Running clear template library test...\n");
                testLibraryClearTemplates();
                break;
            case 12:
                printf("Running download template file test...\n");
                testLibraryUploadFile(0x01);
                break;
            case 13:
                printf("Running system parameter read test...\n");
                testLibraryReadParam();
                break;
            case 14:
                printf("Running crypto key generation test...\n");
                testLibraryGenerateCryptoKey();
                break;
            case 15:
                printf("Running led control test...\n");
                printf("Please select 1 for led on or 0 for led off...\n");
                int state = getFtdiCommand();
                testLibraryLedCtrl(state);
                break;
            case 16:
                printf("Running high level check finger enrolled...\n");
                testHLcheckFingerEnrolled();
                break;
            case 17:
                printf("Running high level enroll finger...\n");
                testHLenrollFinger();
                break;
            case 18:
                printf("Running high level authenticate finger...\n");
                testHLauthenticateFinger();
                break;
            case 19:
                printf("Running high level get crypto key...\n");
                testHLgetCryptoKey();
                break;
            case 20:
                printf("Running high level clear all data...\n");
                testHLclearAllData();
                break;
            default:
                printf("Unknown command, please select a command from the list\n");
                break;
        }
    }

    
    //testLibraryGenerateTemplate();
    //testLibraryGenerateFileFromImg();
    //testLibraryCaptureImage();
    //testLibraryGenImg();
    //testLibraryReadParam();
    //testLibraryHandshake();
    //testLibraryGenImg();
    //builtinEchoTest();
    //sendHandshake(); 
    //uartEchoTest();
    //printf("Restarting now.\n");
    //fflush(stdout);
    //esp_restart();
}

