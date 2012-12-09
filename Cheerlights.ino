
#include <EtherCard.h>
#include <NanodeUNIO.h>

#define BUF_SIZE 512
#define RED 3
#define GREEN 5
#define BLUE 6

byte mac[6];   // Nanode MAC address.
char buf[20];
uint8_t ip[] = { 192, 168, 1, 8 };          // The fallback board address.
uint8_t dns[] = { 192, 168, 1, 20 };        // The DNS server address.
uint8_t gateway[] = { 192, 168, 1, 20 };    // The gateway router address.
uint8_t subnet[] = { 255, 255, 255, 0 };    // The subnet mask.
byte Ethernet::buffer[BUF_SIZE];
byte fixed;                                 // Address fixed, no DHCP
char thingSpeakAddress[] PROGMEM = "api.thingspeak.com";
static uint32_t timer;
String response;
String lastCommand="";
byte oldColors[] = {0,0,0};

static void set_color(byte r, byte g, byte b){
  for(int i = 0;i<255;i++){
    if(oldColors[0]>r){
      oldColors[0]--;
    }
    else if(oldColors[0]<r){
      oldColors[0]++;
    }
    if(oldColors[1]>g){
      oldColors[1]--;
    }
    else if(oldColors[1]<g){
      oldColors[1]++;
    }
    if(oldColors[2]>b){
      oldColors[2]--;
    }
    else if(oldColors[2]<b){
      oldColors[2]++;
    }
    analogWrite(RED,oldColors[0]);
    analogWrite(GREEN,oldColors[1]);
    analogWrite(BLUE,oldColors[2]);
    delay(10);
  }
  return;
}

static void my_callback (byte status, word off, word len) {
  Serial.println(">>>");
  //Ethernet::buffer[off] = 0;
  response = ((const char*) Ethernet::buffer+off);
  if (response.indexOf("white") > 0)
    {  
        lastCommand = "white";
        set_color(255,255,255);
    }
    else if (response.indexOf("black") > 0)
    {  
        lastCommand = "black";
        set_color(0,0,0);
    }
    else if (response.indexOf("red") > 0)
    {  
        lastCommand = "red";
        set_color(255,0,0);   
    }
    else if (response.indexOf("green") > 0)
    {  
        lastCommand = "green";
        set_color(0,255,0);
    }
    else if (response.indexOf("blue") > 0)
    {  
        lastCommand = "blue";
        set_color(0,0,255);
    }
    else if (response.indexOf("cyan") > 0)
    {  
        lastCommand = "cyan";
        set_color(0,139,139);
    }
    else if (response.indexOf("magenta") > 0)
    {  
        lastCommand = "magenta";
        set_color(139,0,139);
    }
    else if (response.indexOf("yellow") > 0)
    {  
        lastCommand = "yellow";
        set_color(255,255,0);
    }
    else if (response.indexOf("purple") > 0)
    {  
        lastCommand = "purple";
        set_color(128,0,128);
    }
    else if (response.indexOf("orange") > 0)
    {  
        lastCommand = "orange";
        set_color(255,165,0);
    }
    else if (response.indexOf("warmwhite") > 0)
    {  
        lastCommand = "warmwhite";
        set_color(250,235,215);
    }
    Serial.println(lastCommand);
    return;
}


void setup(void)
{
    pinMode(RED,OUTPUT);
    pinMode(GREEN,OUTPUT);
    pinMode(BLUE,OUTPUT);

    Serial.begin(38400);
    delay(2000);
    NanodeUNIO unio(NANODE_MAC_DEVICE);
    /* Check that the Ethernet controller exists */
    Serial.println("Initialising the Ethernet controller");
    Serial.println("Let's get MAC Address");
    Serial.print("Reading MAC address... ");
    unio.read(mac,NANODE_MAC_ADDRESS,6);
    sprintf(buf,"%02X:%02X:%02X:%02X:%02X:%02X",
          mac[0],mac[1],mac[2],
          mac[3],mac[4],mac[5]);
    Serial.print("MAC address is ");
    Serial.println(buf);
    if (ether.begin(sizeof Ethernet::buffer, mac, 8) == 0) {
        Serial.println( "Ethernet controller NOT initialised");
        while (true)
            /* MT */ ;
    }

    /* Get a DHCP connection */
    Serial.println("Attempting to get an IP address using DHCP");
    fixed = false;
    if (ether.dhcpSetup()) {
        ether.printIp("Got an IP address using DHCP: ", ether.myip);
    }
    /* If DHCP fails, start with a hard-coded address */
    else {
        ether.staticSetup(ip, gateway, dns);
        ether.printIp("DHCP FAILED, using fixed address: ", ether.myip);
        fixed = true;
    }
    ether.printIp("IP:  ", ether.myip);
    ether.printIp("GW:  ", ether.gwip);  
    ether.printIp("DNS: ", ether.dnsip); 
    if (!ether.dnsLookup(thingSpeakAddress))
    Serial.println("DNS failed");  
    ether.printIp("SRV: ", ether.hisip);
    return;
}


void loop(void)
{
  ether.packetLoop(ether.packetReceive());
  if (millis() > timer) {
    timer = millis() + 16000;
    Serial.println();
    Serial.print("<<< REQ ");
    ether.browseUrl(PSTR("/channels/1417/field/1/last.txt"), "", thingSpeakAddress, my_callback);
  }
}
