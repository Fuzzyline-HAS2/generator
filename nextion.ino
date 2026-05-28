void NextionInit()
{
   nexInit();
   nexHwSerial.begin(9600, SERIAL_8N1, NEXTIONHWSERIAL_RX_PIN, NEXTIONHWSERIAL_TX_PIN);
}

void DisplayCheck()
{
 while (nexHwSerial.available() > 0)
 {
   String nextion_string = nexHwSerial.readStringUntil(' ');
   NextionReceived(&nextion_string);
 }
}

void NextionReceived(String *nextion_string)
{
 if (*nextion_string == "test")
 {
   sendCommand("page pgItemTaken");
   sendCommand("wQuizSolved.en=1");
 }
}

void SendCmd(String command)
{
    String cmd = "";
    if (command.startsWith("page") && (String)(const char *)shift_machine["selected_language"] == "EN")
    {
        cmd = "page E"+ command.substring(5);
    }
    else
    {
        cmd = command;
    }
    sendCommand(cmd.c_str());
}
void LeftGenerator(){
    int gen = (int)my["left_generator"];
    if (gen < 1 || gen > 5) return;

    int offset = ((String)(const char *)shift_machine["selected_language"] == "EN") ? 7 : 0;
    int pic = 168 + (5 - gen) + offset;

    sendCommand(("pleftDevice.pic=" + String(pic)).c_str());
    Serial.println("left Generator " + String(gen));
}
void BatteryPackSend(){
    if((String)(const char*)my["battery_pack"] == "0")
    {
        sendCommand("tBattery0.en=0");
    }
    else if((String)(const char*)my["battery_pack"] == "1")
    {
        sendCommand("tBattery0.en=1");
    }
    else if((String)(const char*)my["battery_pack"] == "2")
    {
        sendCommand("tBattery1.en=1");
    }
    else if((String)(const char*)my["battery_pack"] == "3")
    {
        sendCommand("tBattery2.en=1");
    }
    else if((String)(const char*)my["battery_pack"] == "4")
    {
        sendCommand("tBattery3.en=1");
    }
    else if((String)(const char*)my["battery_pack"] == "5")
    {
        sendCommand("vBatteryPack.val=5");
    }
}
