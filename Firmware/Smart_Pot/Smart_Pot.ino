#include <SoftwareSerial.h>
#define TX 2
#define RX 3
String AP = "SSID";  //WiFi SSID
String PASS = "PW";  //WiFi PASSWORD
String HOST = "IP"; //Replace it with your server IP Address
String PORT = "PORT"; //Replace it with port number
String getData;
float sensor_val;
boolean notification_timeout = false;

const int leds[5] = {4,5,6,7,8};
const int sensor = A0;
const int arr_size = sizeof(leds)/2;
const int the_range[2] = {400, 700};

int countTrueCommand;
int countTimeCommand; 
boolean found = false;

SoftwareSerial esp8266(RX,TX); 

void setup() {
  for(int idx=0; idx < arr_size; idx++){
    pinMode(leds[idx], OUTPUT);
  }
  Serial.begin(9600);
  esp8266.begin(9600);
  sendCommand("AT",5,"OK");
  sendCommand("AT+CWMODE=1",5,"OK");
  sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");
}

void loop() {
  sensor_val = analogRead(sensor);
  write_led(sensor_val);
  if(sensor_val < the_range[0]+100 && notification_timeout == false){
    send_notification(sensor_val);
    notification_timeout = true;   
  }
  if(sensor_val > the_range[0]+100 && notification_timeout == true){
    notification_timeout = false;
  }
  
}

void write_led(float sensor_val){
  int len = ((sensor_val-the_range[0]) / (the_range[1]-the_range[0]))*5;
  for(int idx=0; idx < len;idx++){
    digitalWrite(leds[idx],1);  
  }  
  delay(100);
  for(int idx=0; idx < arr_size;idx++){
    digitalWrite(leds[idx],0);  
  } 
}

void send_notification(int sensor_val){
   sendCommand("AT+CIPMUX=1",5,"OK");
   sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
   getData = "GET /push_notification/"+ String(sensor_val) + " HTTP/1.1";
   sendCommand("AT+CIPSEND=0," +String(getData.length()+4),4,">");
   esp8266.println(getData);
   esp8266.println();
   delay(1500);
   sendCommand("AT+CIPCLOSE=0",5,"OK");
}

void sendCommand(String command, int maxTime, char readReplay[]) {
  Serial.print(countTrueCommand);
  Serial.print(". at command => ");
  Serial.print(command);
  Serial.print(" ");
  while(countTimeCommand < (maxTime*1))
  {
    esp8266.println(command);//at+cipsend
    if(esp8266.find(readReplay))//ok
    {
      found = true;
      break;
    }
  
    countTimeCommand++;
  }
  
  if(found == true)
  {
    Serial.println("OYI");
    countTrueCommand++;
    countTimeCommand = 0;
  }
  
  if(found == false)
  {
    Serial.println("Fail");
    countTrueCommand = 0;
    countTimeCommand = 0;
  }
  
  found = false;
 }
