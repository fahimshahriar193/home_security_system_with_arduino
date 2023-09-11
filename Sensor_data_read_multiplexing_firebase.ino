/*
  DigitalReadSerial

  Reads a digital input on pin 2, prints the result to the Serial Monitor

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/DigitalReadSerial
*/
#include <WiFi.h>
#include <FirebaseESP32.h>


#define FIREBASE_HOST "https://home-security-system2-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "A9x3BAN4oIxdmSxUxKFj3ggIqqNIC9CIgwAzUeKv"
#define WIFI_SSID "Paperclip"
#define WIFI_PASSWORD "masterofpuppets"

//Define FirebaseESP32 data object
FirebaseData firebaseData;
FirebaseJson json;





// digital pin 2 has a pushbutton attached to it. Give it a name:
int lpg = 12, door = 13, motion = 14;
int door_out, lpg_out, motion_out;

const int s0Pin = 33; // Connect to S0 pin
const int s1Pin = 32; // Connect to S1 pin
const int s2Pin = 25; // Connect to S2 pin

const int digitalPins = 34; // Connect to A, B, C pins of CJMCU-4051

int fireOut[10];


// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  // make the pushbutton's pin an input:
  pinMode(lpg, INPUT);
  pinMode(door, INPUT);
  pinMode(motion, INPUT);

  //multiplexing pins
  pinMode(s0Pin, OUTPUT);
  pinMode(s1Pin, OUTPUT);
  pinMode(s2Pin, OUTPUT);
  pinMode(digitalPins, INPUT);

  

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  //Set database read timeout to 1 minute (max 15 minutes)
  Firebase.setReadTimeout(firebaseData, 1000 * 60);
  //tiny, small, medium, large and unlimited.
  //Size and its write timeout e.g. tiny (1s), small (10s), medium (30s) and large (60s).
  Firebase.setwriteSizeLimit(firebaseData, "tiny");

  /*
  This option allows get and delete functions (PUT and DELETE HTTP requests) works for device connected behind the
  Firewall that allows only GET and POST requests.
  
  Firebase.enableClassicRequest(firebaseData, true);
  */

  //String path = "/data";
  

  Serial.println("------------------------------------");
  Serial.println("Connected...");
  


}

// the loop routine runs over and over again forever:
void loop() {
  // read the input pin:
  lpg_out = digitalRead(lpg);
  motion_out = digitalRead(motion);
  door_out = digitalRead(door);

  if(lpg_out) lpg_out = 0;
  else if(!lpg_out) lpg_out = 1;



  json.set("/MOTION_sensor", motion_out); // Place your sensor data here
  json.set("/GAS_Sensor", lpg_out); 
  json.set("/DOOR_Sensor", door_out);
  
  



  // print out the state of the button:
  Serial.print(lpg_out);
  Serial.print(" ");
  Serial.print(door_out);
  Serial.print(" ");
  Serial.println(motion_out);


  Serial.print("Fire Channel ");

  for (int i = 0; i < 5; i++) {
    // Select the channel using the multiplexer's address pins
    digitalWrite(s0Pin, bitRead(i, 0));
    digitalWrite(s1Pin, bitRead(i, 1));
    digitalWrite(s2Pin, bitRead(i, 2));
    delay(00);

    // Read analog value from the selected channel
    fireOut[i] = digitalRead(digitalPins);
    
    if (i>1){
      if(fireOut[i]) fireOut[i] = 0;
      else if(!fireOut[i]) fireOut[i] = 1;


    }

    // Print the sensor value
    
    Serial.print(fireOut[i]);
    Serial.print(" ");


  }
  Serial.println("\n");

json.set("/Fire Sensor 1", fireOut[0]); // Place your sensor data here
json.set("/Fire Sensor 2", fireOut[1]);
json.set("/Fire Sensor 3", fireOut[2]);
json.set("/Fire Sensor 4", fireOut[3]);
json.set("/Fire Sensor 5", fireOut[4]);


Firebase.updateNode(firebaseData,"/Sensor",json);




  delay(50);  // delay in between reads for stability
}
