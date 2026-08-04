static void nxSend(const char* cmd)
{
    nexHwSerial.print(cmd);
    nexHwSerial.write(0xff);
    nexHwSerial.write(0xff);
    nexHwSerial.write(0xff);
}

void NextionInit()
{
   nexHwSerial.begin(9600, SERIAL_8N1, NEXTIONHWSERIAL_RX_PIN, NEXTIONHWSERIAL_TX_PIN);
   delay(100);
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
   nxSend("page pgItemTaken");
   nxSend("wQuizSolved.en=1");
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
    nxSend(cmd.c_str());
}
void LeftGenerator(){
    int gen = (int)my["left_generator"];
    if (gen < 1 || gen > 5) return;

    int offset = ((String)(const char *)shift_machine["selected_language"] == "EN") ? 7 : 0;
    int pic = 168 + (5 - gen) + offset;

    nxSend(("pleftDevice.pic=" + String(pic)).c_str());
    Serial.println("left Generator " + String(gen));
}
void BatteryPackSend(){
    int batteryPack = (int)my["battery_pack"];
    if(batteryPack == 0)
    {
        nxSend("tBattery0.en=0");
    }
    else if(batteryPack == 1)
    {
        nxSend("tBattery0.en=1");
    }
    else if(batteryPack == 2)
    {
        nxSend("tBattery1.en=1");
    }
    else if(batteryPack == 3)
    {
        nxSend("tBattery2.en=1");
    }
    else if(batteryPack == 4)
    {
        nxSend("tBattery3.en=1");
    }
    else if(batteryPack == 5)
    {
        nxSend("vBatteryPack.val=5");
    }
}
