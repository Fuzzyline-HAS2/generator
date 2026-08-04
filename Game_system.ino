void StarterActivate(){
    // RFID 체크는 200ms마다만 수행 (블로킹으로 인한 루프 지연 방지)
    static bool tagOnReader = false;
    static bool lastTagState = false;
    static bool isPlayerTagged = false;
    static unsigned long lastRfidCheck = 0;
    if (millis() - lastRfidCheck >= 200){
        lastRfidCheck = millis();
        byte pn532_buf[64];
        pn532_buf[0] = 0x00;
        tagOnReader = false;
        if (nfc[MAINPN532].sendCommandCheckAck(pn532_buf, 1)){
            if (nfc[MAINPN532].startPassiveTargetIDDetection(PN532_MIFARE_ISO14443A)){
                tagOnReader = true;
                if (!lastTagState) { // 새 태그가 올라왔을 때만 role 확인
                    uint8_t data[32];
                    if (nfc[MAINPN532].ntag2xx_ReadPage(7, data)){
                        String tagUser = "";
                        for(int i = 0; i < 4; i++) tagUser += (char)data[i];
                        has2wifi.Receive(tagUser);
                        isPlayerTagged = ((String)(const char*)tag["role"] == "player");
                        Serial.println(isPlayerTagged ? "Starter: Player OK" : "Starter: Ghost Blocked");
                    } else {
                        isPlayerTagged = false;
                    }
                }
            }
        }
        lastTagState = tagOnReader;
    }
    // 게이지는 "칸"(엔코더 starterEncoderUnit당 1칸) 단위로만 변하므로
    // 칸 수가 바뀌면 즉시 갱신하고, 그 외에는 500ms마다 재전송만 한다
    // (깨진 프레임 자동 복구용). 전송 횟수를 줄이면 깨질 기회도 줄어든다.
    static unsigned long lastGaugeUpdate = 0;
    static int lastGaugeNeoCnt = -1;
    int gaugeNeoCnt = encoderValue / starterEncoderUnit;
    if (gaugeNeoCnt != lastGaugeNeoCnt || millis() - lastGaugeUpdate >= 500){
        lastGaugeUpdate = millis();
        lastGaugeNeoCnt = gaugeNeoCnt;
        EncoderNeopixelOn(gaugeNeoCnt);
    }

    if (!tagOnReader || !isPlayerTagged){
        EncoderDetach();
        EngineSpeeed(0);
        return;
    }

    EncoderAttach();

    // 디버그 출력은 1초에 한 번이면 충분
    static unsigned long lastRawPrint = 0;
    if (millis() - lastRawPrint >= 1000){
        lastRawPrint = millis();
        Serial.println("raw: " + String(encoderValue));
    }
    EngineSpeeed(gaugeNeoCnt*8);
    if(gaugeNeoCnt >= NumPixels[GAUGE]){
        EncoderDetach();
        // SendCmd("page pgFixed");
        StartFinish();
        BlinkTimer.deleteTimer(blinkTimerId);
        NeoLightColor(STARTER, color[GREEN]);
        GameTimer.deleteTimer(gameTimerId);        //게임 타이머 종료
        BlinkTimer.deleteTimer(blinkTimerId);
        NeoLightColor(CIRCUIT, color[BLUE]);
    }
}