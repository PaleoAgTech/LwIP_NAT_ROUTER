#include <EEPROM.h>
void writeString(char add,String data);
String read_String(char add);
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

int i; 
int x;

String ssid;
String password;

String apssid;
String appassword;

#include <ESP8266WiFi.h>

#include "lwip/lwip_napt.h"

#include "lwip/app/dhcpserver.h"

ESP8266WebServer server(80);

// STA
String sta_ssid = "";
String sta_password = "";
//AP
String ap_ssid = "";
String ap_password = "";

void handleRoot() 
   {
   // HERE WILL GO YOUR WEBSITE HTML CODE
   String html ="<!DOCTYPE html> <html> <head><title>ESP NAT</title><meta name='viewport' content='width=device-width, initial-scale=1.0'>";
   html += "<style> .button {  background-color: #73AD21;  border: none;  color: white;  padding: 10px;  text-align: center;  text-decoration: none;  display: inline-block;  font-size: 15px;  margin: 4px 2px;  cursor: pointer;}.button4 {border-radius: 12px;}#rcorners2 { border-radius: 25px; border: 2px solid #73AD21; padding: 20px;  width: width; height: height;  }</style> ";
   html += "</head>";
   html += "<h3><center>ESP NAT ROUTER</center></h3>"; 
   html +="<center><h3></h3></center><center><form action='/action_page'><p id='rcorners2'>Your Router SSID:<br><input type='text' name='ssid' value='";
   html +=sta_ssid;
   html +="'><br>Your router password:<br><input type='text' name='password' value=''><br>New Ap SSID:<br><input type='text' name='assid' value='MyAp'><br>New Ap passowed:<br><input type='text' name='apassword' value=''><br><br><br><input type='submit' class='button button4' value='Submit'></p></form><br>";
   html +="</center><br><br><br><center></center></div> </body> </html>"; 
   //--------------------------------------
  server.send(200, "text/html", html); 
 }

void setup()
{
  delay(1000);
  Serial.begin(9600);
  EEPROM.begin(512); 
  pinMode(2,OUTPUT);
  sta_ssid = read_String(1);
  sta_password = read_String(50);
  ap_ssid = read_String(100);
  ap_password = read_String(150);
  Serial.println("SSID "+ssid);
  Serial.println("PASS "+password);
  Serial.println(apssid);
  Serial.println(appassword); 
  WiFi.softAP("MyAp", "");
  server.on("/", handleRoot);
  server.on("/action_page", handleForm); 
  server.begin();
  while(sta_ssid == "")
  {
    sta_ssid = read_String(1);
    sta_password = read_String(50);
    ap_ssid = read_String(100);
    ap_password = read_String(150);
    server.handleClient(); 
    delay(500);  
  }
   struct dhcps_lease dhcp_lease;
   //HERE IS STAET AND END IP ADDRESS FOR CLIENTS 
   char* start_ip = "192.168.11.10";
   char* end_ip = "192.168.11.100";
   struct ip_info info; 
   //----------------------------------
  WiFi.mode(WIFI_AP_STA);
  Serial.println("Starting NAT demo"); 
  WiFi.begin(sta_ssid, sta_password);
  //WiFi.config(ip, gateway, subnet);
   server.on("/", handleRoot);
   server.on("/action_page", handleForm); 
   server.begin();
  //Wifi connection
    while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(2,LOW); 
    server.handleClient();  
    delay(500);
    Serial.print(".");
  } 
  digitalWrite(2,HIGH);
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(sta_ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("dnsIP address: ");
  Serial.println(WiFi.dnsIP());
  Serial.print("gatewayIP address: ");
  Serial.println(WiFi.gatewayIP());
  Serial.print("subnetMask address: ");
  Serial.println(WiFi.subnetMask());
  Serial.println("");
  Serial.println("Configuring access point..."); 
  WiFi.softAP(ap_ssid, ap_password); 
  wifi_softap_dhcps_stop();
  //THIS VALUES WILL GIVE IP ADDRESS FOR YOUR NAT ROUTER
  IP4_ADDR(&info.ip, 192, 168, 11, 1);     
  IP4_ADDR(&info.gw, 192, 168, 11, 1);
  IP4_ADDR(&info.netmask, 255, 255, 255, 0);
  //------------------------------------------  
  dhcp_lease.start_ip.addr = ipaddr_addr(start_ip);
  dhcp_lease.end_ip.addr = ipaddr_addr(end_ip);
  wifi_set_ip_info(SOFTAP_IF, &info);
  wifi_softap_set_dhcps_lease(&dhcp_lease);
  wifi_softap_dhcps_start();
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  // Initialize the NAT feature
  ip_napt_init(IP_NAPT_MAX, IP_PORTMAP_MAX);
  // Enable NAT on the AP interface
  ip_napt_enable_no(1, 1);
  // Set the DNS server for clients of the AP to the one we also use for the STA interface
  dhcps_set_DNS(WiFi.dnsIP());
  server.on(WiFi.localIP()+"/", handleRoot);
  server.on(WiFi.localIP()+"/action_page", handleForm); 
  server.begin(); 
}

void loop()
{
  while (WiFi.status() != WL_CONNECTED) {
    server.handleClient();
    digitalWrite(2,LOW);     
    delay(500);
    Serial.print(".");
  } 
   x=ESP.getFreeHeap();  
  i = x-i;
  if(i>0 || i<0)    //THIS WILL BLINK ONBOARD LED WHEN HEAP CHANGES
  {
    digitalWrite(2,LOW); 
  }
  else
  {   
    digitalWrite(2,HIGH);
  }  
  i=ESP.getFreeHeap();
   server.handleClient();
  delay(1);
}
void handleForm() {
  digitalWrite(D4,LOW);
  String rssid = server.arg("ssid"); 
  String rpass = server.arg("password"); 
  String assid = server.arg("assid"); 
  String apass = server.arg("apassword"); 
  server.send(200, "text/html", "<!DOCTYPE html><html><head><title>ESP NAT</title><meta name='viewport' content='width=device-width, initial-scale=1.0'></head><body><center>Please wait</center></body>"); 
  writeString(1, rssid);  
  writeString(50, rpass); 
  writeString(100, assid); 
  writeString(150, apass);  
  delay(5000);   
  ssid = read_String(1);
  password = read_String(50);
  apssid = read_String(100);
  appassword = read_String(150);
  delay(10000);
  ESP.restart();
}
void writeString(char add,String data)
{
  int _size = data.length();
  int i;
  for(i=0;i<_size;i++)
  {
    EEPROM.write(add+i,data[i]);
  }
  EEPROM.write(add+_size,'\0');   
  EEPROM.commit();
}
String read_String(char add)
{
  int i;
  char data[100]; 
  int len=0;
  unsigned char k;
  k=EEPROM.read(add);
  while(k != '\0' && len<500)  
  {    
    k=EEPROM.read(add+len);
    data[len]=k;
    len++;
  }
  data[len]='\0';
  return String(data);
}
