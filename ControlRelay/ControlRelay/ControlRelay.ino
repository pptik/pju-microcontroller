/* 
 *  PLEASE DONT DELETE AND EDIT 
 *  Develop by          : Nurman Hariyanto
 *  Email               : nurman.hariyanto13@gmail.com
 *  Project             : Relay PJU
 *  Version             : 1.0
 *  Description         : This code trigger Relay from message payload rabbitmq
 *  Microcontroller     : Wemos Mini ESP8266
 *                        NodeMCU ESP8266
 *  Device              : Relay Module 1 Channel
 *                        Relay Module 1 Channel                   
 */




/*
 * YOU CAN EDIT NOW
 */

/*
 * Include library 
 */

 #include <PubSubClient.h>
 #include <ESP8266WiFi.h>
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 * Connection parameter
 * -Wifi -> internet connection foor data communication
 * -RabbitMQ/MQTT -> protoocol data communication
 */
const char* wifiSsid              = "LSKK_Lantai2";
const char* wifiPassword          = "lskk12345";
const char* mqttHost              = "rmq-pju.pptik.id";
const char* mqttUserName          = "/pju:pju";
const char* mqttPassword          = "pEj3Uu123";
const char* mqttQueueLog          = "Log";
const char* mqttQueueAktuator     = "Aktuator";
const char* mqttQueueSensor       = "Sensor";
const char* CL                    = "PPTIK-PJU-Relay-001";
int devicePin                     = D1;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*7
 * Device parameter
 * -Guid -> Id Device (unique) you can generate from here (https://www.uuidgenerator.net/version1) 
 * -status device -> save last state from the relay is on or off (1 = on , 0 = off) 
 * -pin microcontroller 
 * -mac device
 * 
 */
const char* device_guid                = "5b0ed6a6-8e02-11eb-8dcd-0242ac130003"; //You can change this guid with your guid 
int loop_count  = 0 ; //loop count loop
String statusDevice = "0";
String output_value;


/*
 * Wifi setup WiFi client and mac address 
 */
WiFiClient espClient;
PubSubClient client(espClient);
byte mac[6]; //array temp mac address
String MACAddress;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/*
 * Watchdog fucntion to force reset esp
 */
void watchdogSetup(void) {
  ESP.wdtDisable();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 * Set up WiFi connection
 */
 void setup_wifi(){
  delay(10);
  //We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to :");
  Serial.println(wifiSsid);
  WiFi.begin(wifiSsid, wifiPassword);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
 }
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/*
 * Function for Get Mac Address from microcontroller
 */
 
String mac2String(byte ar[]) {
  String s;
  for (byte i = 0; i < 6; ++i)
  {
    char buf[3];
    sprintf(buf, "%2X", ar[i]);
    s += buf;
    if (i < 5) s += ':';
  }
  return s;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 * Function for Print Mac Address 
 */
 void printMACAddress() {
  WiFi.macAddress(mac);
  MACAddress = mac2String(mac);
  Serial.println(MACAddress);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



/*
 * Function for Get message payload from MQTT rabbit mq
 */
char sPayload[100];
char message [40] ;
char address[40];
void callback(char* topic, byte* payload, unsigned int length) {
  memcpy(sPayload, payload, length);
  memcpy(address, payload, 36);
  memcpy(message, &payload[37], length - 37);
  Serial.print("Message arrived [");
  Serial.print(sPayload);
  Serial.println("] ");

  Serial.println(device_guid);
  Serial.println(address);
  if (String((char *)address) == String((char *)device_guid))
  {
    Serial.println("address sama");
  }
  else
  {
    Serial.println("address berbeda");
    return;
  }

  Serial.println(message);

  if (message[0] == '1') {
    digitalWrite(devicePin, LOW);
    Serial.println("relay 1 on");
    statusDevice = "1";

  }
  if (message[0] == '0') {
    digitalWrite(devicePin, HIGH);
    Serial.println("relay 1 off");
    statusDevice = "0";
  }

 
  //Serial.print("Publish message: ");
  //String pubmsg = String(device_guid) + "#" + String(statusDevice[0] + statusDevice[1] + statusDevice[2] + statusDevice[3] + statusDevice[4] + statusDevice[5] + statusDevice[6] + statusDevice[7]);
  //Serial.println(pubmsg);
  //client.publish(mqtt_pub_topic, pubmsg.c_str());
  delay(50);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 * Function for Reconnecting to MQTT/RabbitMQ 
 */
void reconnect() {
  // Loop until we're reconnected
  printMACAddress();
  const char* CL;
  CL = MACAddress.c_str();
  Serial.println(CL);
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(CL, mqttUserName, mqttPassword)) {
      Serial.println("connected");
      client.subscribe(mqttQueueSensor);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      ESP.restart();
      delay(5000);

    }
  }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/*
 * Function for Setup Pin Mode,wifi,mqtt,and serial
 */
void setup()
{

  pinMode(devicePin, OUTPUT);

  digitalWrite(devicePin, HIGH);

  //pinMode(input, INPUT);
  Serial.begin(115200);
  setup_wifi();
  printMACAddress();
  client.setServer(mqttHost, 1883);
  client.setCallback(callback);
  watchdogSetup();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



/*
 * This functioon for loop your program
 */
void loop() {
  //if you disconnected from wifi and mqtt
  for (int i = 0; i <= loop_count; i++) {
    if (!client.connected()) {
      reconnect();
    }
    client.loop();
  }
  loop_count++;
  ESP.wdtFeed();
  Serial.print(loop_count);
  Serial.print(". Watchdog fed in approx. ");
  Serial.print(loop_count * 500);
  Serial.println(" milliseconds.");
  delay(500);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////











 
