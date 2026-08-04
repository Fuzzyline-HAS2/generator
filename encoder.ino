// 엔코더 카운팅을 인터럽트(ISR) 대신 ESP32 하드웨어 펄스 카운터(PCNT)로 처리.
// CPU 개입 없이 하드웨어가 A/B 양핀의 모든 엣지를 세므로
// 네오픽셀 전송을 방해하지 않고, loop가 WiFi 등으로 막혀 있어도 펄스가 유실되지 않는다.
#include "driver/pulse_cnt.h"

pcnt_unit_handle_t pcntUnit = NULL;
int lastPcntCount = 0;

void EncoderInit()
{
    pinMode(encoderPinA, INPUT_PULLUP);
    pinMode(encoderPinB, INPUT_PULLUP);

    pcnt_unit_config_t unitConfig = {};
    unitConfig.low_limit = -32768;
    unitConfig.high_limit = 32767;
    esp_err_t err = pcnt_new_unit(&unitConfig, &pcntUnit);
    if (err != ESP_OK) {
        Serial.printf("[PCNT] unit create failed: %s\n", esp_err_to_name(err));
        return;
    }

    pcnt_glitch_filter_config_t filterConfig = {};
    filterConfig.max_glitch_ns = 1000;
    err = pcnt_unit_set_glitch_filter(pcntUnit, &filterConfig);
    if (err != ESP_OK) {
        Serial.printf("[PCNT] glitch filter failed: %s\n", esp_err_to_name(err));
        return;
    }

    pcnt_chan_config_t chanConfig = {};
    chanConfig.edge_gpio_num = encoderPinA;
    chanConfig.level_gpio_num = GPIO_NUM_NC;
    pcnt_channel_handle_t chanA = NULL;
    esp_err_t ret = pcnt_new_channel(pcntUnit, &chanConfig, &chanA);
    if (ret != ESP_OK) {
        Serial.printf("[PCNT] chanA error: %s\n", esp_err_to_name(ret));
        return;
    }
    pcnt_channel_set_edge_action(chanA, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_INCREASE);
    pcnt_channel_set_level_action(chanA, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_KEEP);

    chanConfig.edge_gpio_num = encoderPinB;
    pcnt_channel_handle_t chanB = NULL;
    ret = pcnt_new_channel(pcntUnit, &chanConfig, &chanB);
    if (ret != ESP_OK) {
        Serial.printf("[PCNT] chanB error: %s\n", esp_err_to_name(ret));
        return;
    }
    pcnt_channel_set_edge_action(chanB, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_INCREASE);
    pcnt_channel_set_level_action(chanB, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_KEEP);

    err = pcnt_unit_enable(pcntUnit);
    if (err != ESP_OK) {
        Serial.printf("[PCNT] unit enable failed: %s\n", esp_err_to_name(err));
        return;
    }
    pcnt_unit_clear_count(pcntUnit);
    pcnt_unit_start(pcntUnit);
    encoderAttached = true;
}

void EncoderAttach()
{
    if (!encoderAttached){
        pcnt_unit_clear_count(pcntUnit);
        lastPcntCount = 0;
        pcnt_unit_start(pcntUnit);
        encoderAttached = true;
    }
}

void EncoderDetach()
{
    if (encoderAttached){
        pcnt_unit_stop(pcntUnit);
        encoderAttached = false;
    }
}

// 매 loop마다 호출: 하드웨어 카운터의 증가분을 encoderValue에 반영
void EncoderLoop()
{
    if (!encoderAttached) return;

    int count = 0;
    pcnt_unit_get_count(pcntUnit, &count);
    int delta = count - lastPcntCount;
    lastPcntCount = count;
    // loop가 오래 막힌 사이(WiFi 통신 등) 카운터가 32767을 넘으면
    // 하드웨어가 0으로 랩어라운드해 delta가 음수가 됨 → 한 바퀴만큼 보정
    if (delta < 0) delta += 32767;
    if (delta != 0){
        encoderValue += delta;
        gameTimerCnt = 0; // 회전 감지 → 게이지 감소 타이머 리셋 (기존 ISR과 동일)
    }
    // 하드웨어 한계(32767) 도달 전에 카운터 리베이스
    if (count > 20000){
        pcnt_unit_clear_count(pcntUnit);
        lastPcntCount = 0;
    }
}
