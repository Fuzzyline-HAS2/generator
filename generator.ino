#include <HAS2_Wifi.h>

#include <HAS2_Wifi.h>

#include <SecureOTA.h>

/**
 * @file Done_Generator_code.ino
 * @author 김병준 (you@domain.com)
 * @brief
 * @version 1.0
 * @date 2022-11-29
 *
 * @copyright Copyright (c) 2022
 *
 */

#define FIRMWARE_VER 6
#define PARTITION_VER 1
#include "generator.h"

void setup() {
    Serial.begin(115200);
    NeopixelInit();
    RfidInit();
    MotorInit();
    EncoderInit();
    NextionInit();
    TimerInit();
    has2wifi.Setup("badland");
    has2wifi.Send((String)(const char*)my["device_name"], "esp_version", String(FIRMWARE_VER));
    ota.setLogStream(Serial);
    ota.setOnSuccess([]() {
        has2wifi.Send((String)(const char*)my["device_name"], "device_state", "setting");
        Serial.println("[OTA] ✅ 업데이트 성공! 재부팅합니다...");
    });
    ota.setOnSkip([]() {
        has2wifi.Send((String)(const char*)my["device_name"], "device_state", "setting");
        Serial.println("[OTA] 이미 최신 버전입니다.");
    });
    ota.setPartitionUpdate(
        "https://raw.githubusercontent.com/Fuzzyline-HAS2/generator/third_store/partitions.bin",
        "https://raw.githubusercontent.com/Fuzzyline-HAS2/generator/third_store/partitions.sig",
        "https://raw.githubusercontent.com/Fuzzyline-HAS2/generator/third_store/partition_version.txt",
        PARTITION_VER
    );
    ptrCurrentMode = WaitFunc;
    ptrRfidMode = WaitFunc;
    DataChanged();
}
void loop() {
    ptrCurrentMode();
    TimerRun();
}
