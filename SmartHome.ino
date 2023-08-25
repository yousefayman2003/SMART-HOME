 // Used modules
#include <ESP32Servo.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <FirebaseESP32.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>
#include <time.h>

#define WIFI_SSID "mywifi"
#define WIFI_PASSWORD "m123598"
#define API_KEY "AIzaSyAzkKax2O0i-2b2r8U13HjayCipQmChB3E"
#define DATABASE_URL "https://esp-firebase-641b5-default-rtdb.firebaseio.com/"

#define USER_EMAIL "test@gmail.com"
#define USER_PASSWORD "123456"

FirebaseData stream;
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;


// Firebase paths
const String PARENT_PATH = "reading/";
const String INVALID_PASSWORD_PATH = "emergency/invalid_password";
const String THEIF_PATH = "emergency/theif";
const String SERVO_PATH = "utils/door";
const String LED_PATH = "utils/led";
String LDR_PATH;
String GAS_PATH;
String FLAME_PATH;
String ULTRASONIC_PATH;
String PIR_PATH;

// variables
const char* ntpServer = "pool.ntp.org";
int timestamp;
bool ultrasonicValue = false;
//bool fireValue = false;
bool pirValue = false;
int ldrValue;
int gasValue;
bool invalidPassword = false;
char key;
bool initial = true;
bool ledCheck = false;
bool doorOpened = false;
bool motionDectected = false;

// Function Declartion
unsigned long getTime();
void writeDataInt(String path, int data);
void writeDataBool(String path, bool data);
void initWiFi();
void setPinsMode();
bool valid_password();
bool is_there_someone();
void emergency();
void streamCallback(StreamData data);
void streamTimeoutCallback(bool timeout);
void handleStream(String path, bool value);
void theif();

// Used variables
const byte pir_pin = 34;
const byte flame_pin = 5;
const byte rows = 4;
const byte cols = 4;
const byte gas_pin = 35;
const byte echo_pin = 23, trig_pin = 18;
const byte motor_pin = 15;
const byte leds_pin = 19;
const byte doorLedPin = 16;
const byte ldr_pin = 4;
const byte buzzer_pin1 = 2, buzzer_pin2 = 17;

const String password = "123";
byte row_pins[] = {32, 33, 25, 26}, col_pins[] = {27, 14, 12, 13};
char keys[4][4] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'},
};

// Objects
LiquidCrystal_I2C lcd(0x27, 16, 2);
Keypad keypad = Keypad(makeKeymap(keys), row_pins, col_pins, rows, cols);
Servo motor;

void setup() {
    Serial.begin(115200);
    
    setPinsMode();

    //Initialzing LCD
    lcd.init();
    lcd.backlight();

    // Connecting to WiFi
    initWiFi();
  
    //Configure times
    configTime(0, 0, ntpServer);
    
    // Assign the api key (required) 
    config.api_key = API_KEY;
    // Assign the RTDB URL 
    config.database_url = DATABASE_URL;
    // Assign the callback function for the long running token generation task 
    config.token_status_callback = tokenStatusCallback;
    config.max_token_generation_retry = 5;
  
    // Assign the user sign in credentials 
    auth.user.email = USER_EMAIL;
    auth.user.password = USER_PASSWORD;
    
    // Connecting to FireBase
    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);
    //Set the size of HTTP responses buffers to work with large amount of data
    stream.setResponseSize(1024);
      

    //Using TCP KeepAlive For more reliable stream operation and tracking the server connection status
    stream.keepAlive(5, 5, 1);
    if (!Firebase.beginStream(stream, "utils/"))
    {
      // Could not begin stream connection, then print out the error detail
      Serial.println(stream.errorReason());
    }
    Firebase.RTDB.setStreamCallback(&stream, streamCallback, streamTimeoutCallback);
}


void loop()
{
  LDR_PATH = "sensor/ldr";
  GAS_PATH = "sensor/gas";
  FLAME_PATH = "sensor/flame";
  ULTRASONIC_PATH = "sensor/ultrasonic";
  PIR_PATH = "sensor/pir";


  if (Firebase.ready())
  {
          
      // Getting sensors readings from esp32
      gasValue = analogRead(gas_pin);
      ultrasonicValue = is_there_someone();
      fireValue = digitalRead(flame_pin);
      pirValue = digitalRead(pir_pin);
      ldrValue = analogRead(ldr_pin);
      
  
      // Getting current timestamp
      timestamp = getTime();
  
      // Setting new PATH
      LDR_PATH = PARENT_PATH + String(timestamp) + "/" + LDR_PATH;
      GAS_PATH = PARENT_PATH + String(timestamp) + "/"  + GAS_PATH;
      FLAME_PATH = PARENT_PATH + String(timestamp) + "/"  + FLAME_PATH;
      ULTRASONIC_PATH = PARENT_PATH + String(timestamp) + "/"  + ULTRASONIC_PATH;
      PIR_PATH = PARENT_PATH + String(timestamp) + "/"  + PIR_PATH;
   
          
      // Writing to RTDB
      writeDataInt(LDR_PATH, ldrValue);
      writeDataInt(GAS_PATH, gasValue);
      writeDataBool(FLAME_PATH, fireValue);
      writeDataBool(ULTRASONIC_PATH, ultrasonicValue);
      writeDataBool(PIR_PATH, pirValue);
      if (initial)
      {
        writeDataBool(LED_PATH, false);
        writeDataBool(SERVO_PATH,  false);
        initial = false;
      }
      Serial.println("\n");    
      // Checks if someone entered the house without unlocking the door properly i.e theif
      if (pirValue == HIGH && !doorOpened)
      {
          writeDataBool(THEIF_PATH, true);
          theif();
      }
      else
      {
          writeDataBool(THEIF_PATH, false);
      }
    
      // The smart door with keypad and ultrasonic authentication      
      if (is_there_someone())
      {
        digitalWrite(doorLedPin, HIGH);
        Serial.println("PRESS # TO ENTER PASSWORD: ");
        lcd.setCursor(0, 0);
        lcd.print("PRESS # TO ENTER");
        lcd.setCursor(0,1);
        lcd.print("YOUR PASSWORD: ");
        key = keypad.getKey();

        if (key == '#')
        {
          lcd.clear();
          lcd.print("Enter Password: ");
          Serial.println("Enter Your Password: ");
          if (valid_password())
            {
              lcd.clear();
              lcd.print("Correct Password");
              delay(250);
              Serial.println("Correct Password");
              motor.write(90);
              delay(3000);
              motor.write(0);
              invalidPassword = false;
              writeDataBool(INVALID_PASSWORD_PATH, invalidPassword);
              lcd.print("WELCOME HOME :)");
              delay(1000);
              lcd.clear();
              doorOpened = true;
            }
          else
          {
            lcd.clear();
            lcd.print("Wrong Password");
            Serial.println("Wrong Password!!");
            invalidPassword = true;
            writeDataBool(INVALID_PASSWORD_PATH, invalidPassword); 
            doorOpened = false;
            delay(500);
          
          } 
        }
      }
      else
      {
        digitalWrite(doorLedPin, LOW);
        lcd.clear();
      }
                  
      // Control home lighting using PIR sensor and LDR sensor 
      if (pirValue  == HIGH)
      {
        digitalWrite(leds_pin, HIGH); 
      }
      else
      {
         if (!ledCheck)
            digitalWrite(leds_pin, LOW); 
      }
        
       
       //Declare an emergency when there is a fire or gas leak
      if (gasValue > 1200)
          emergency();   
  }
}

/**
 * theif - turn on the buzzer and turn on and off the leds when someone breaks into the house
 * 
 * Return: NULL
 */
void theif()
{
   lcd.clear();
   Serial.println("THEIF!!");
   lcd.println("THEIF");
   while (digitalRead(pir_pin))
   {
      digitalWrite(leds_pin, HIGH);
      tone(buzzer_pin1, 100);
      tone(buzzer_pin2, 100);
      delay(100);
      digitalWrite(leds_pin, LOW);
      noTone(buzzer_pin1);
      noTone(buzzer_pin2);
      delay(100);
   }
   lcd.clear();
   lcd.print("THE HOME IS SAFE");
   delay(500);
 }




/**
 * Description: emergency- turn on the buzzer and turn off the lamps when fire or gas is detected
 * 
 * Return: NULL
 */
 void emergency()
 {
   lcd.clear();
   Serial.println("EMERGENY");
   lcd.println("EMERGENY");
   while (analogRead(gas_pin) > 1200)
   {
      Serial.printf("GAS = %d\n", analogRead(gas_pin) > 1200);
      digitalWrite(leds_pin, HIGH);
      tone(buzzer_pin1, 100);
      tone(buzzer_pin2, 100);
      delay(100);
      digitalWrite(leds_pin, LOW);
      noTone(buzzer_pin1);
      noTone(buzzer_pin2);
      delay(100);
      
   }
   lcd.clear();
   lcd.print("THE HOME IS SAFE");
   delay(500);
 }

/**
 * Description: vaild_password - cheack the entered password by user
 * Input: NULL
 * Return: 
 *      true if the entered password is correct 
 *      false otherwise
 */
bool valid_password()
{
  byte i = 0;
  char key;
  String pass = "";
  
  // take the password from the user
  while(1)
  {
    key = keypad.getKey();
    
    if (key)
    {
      // display the entered keys password
      lcd.setCursor(i, 1);
      lcd.print(key);   
      Serial.print(key);
      if (key == '#' or i == 16)
        break;
      if (key != password[i])
        break;
      i++;
      pass += key;
    }
  }
  return (pass == password);
}


/**
 * Description: is_there_someone - cheack if there someone in front of the door or not
 * Input:NULL 
 * Return:
 *      true if there is someone in front of the door and the distance between them is grater or equal 5
 *      false otherwise
 */
bool is_there_someone()
{
  // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse
  digitalWrite(trig_pin, LOW);
  delay(2);
  digitalWrite(trig_pin, HIGH);
  delay(5);
  digitalWrite(trig_pin, LOW);

  // Read the echo PW signals
  int duration = pulseIn(echo_pin, HIGH);

  // Calculate the distance in centimeters
  int distance = duration * 0.034 / 2;

  if (distance <= 50
  )
    return true;
  else
    return false;
}

/**
 * setPinsMode - Set pins mode for  esp32
 * 
 * Return: NULL
*/
void setPinsMode()
{
//  pinMode(ldr_pin, INPUT);
  pinMode(echo_pin, INPUT);
  pinMode(trig_pin, OUTPUT);
  pinMode(gas_pin, INPUT);
  pinMode(pir_pin, INPUT);  
  pinMode(leds_pin, OUTPUT);
  pinMode(doorLedPin, OUTPUT);
  pinMode(buzzer_pin1, OUTPUT);
  pinMode(buzzer_pin2, OUTPUT);
  motor.attach(motor_pin);
}

/**
 * getTime - Function that gets current epoch time
 * 
 * Return: 0 if failed to get time, else returns current time
*/
unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return (0);
  }
  time(&now);
  return now;
}

/**
 * writeDataInt - Writes data of type int to firebase
 * 
 * @path [String]: database path to write data for
 * 
 * @data [int]: data to write
 * 
 * Return: NULL
*/
void writeDataInt(String path, int data)
{
  // Getting current time
  timestamp = getTime();
  // Writing to RTDB
  if (Firebase.RTDB.setIntAsync(&fbdo, path, data))
  {
    Serial.printf("%i - SuccessFully Saved to : %s\n", data, fbdo.dataPath().c_str());          
  }
  // if writing fails
  else
  {
    Serial.println("FAILED TO WRITE: " + String(fbdo.errorReason().c_str()));  
  }
}

/**
 * writeDataBool - Writes data of type boolean to firebase
 * 
 * @path [String]: database path to write data for
 * 
 * @data [bool]: data to write
 * 
 * Return: NULL
*/
void writeDataBool(String path, bool data)
{
  // Getting current time
  timestamp = getTime();
  // Writing to RTDB
  if (Firebase.RTDB.setBoolAsync(&fbdo, path, data))
  {
    Serial.printf("%i - SuccessFully Saved to : %s\n", data, fbdo.dataPath().c_str());          
  }
  // if writing fails
  else
  {
    Serial.println("FAILED TO WRITE: " + String(fbdo.errorReason().c_str()));  
  }
}

/**
 * initWiFi - Initializes a wifi connection with esp32
 * 
 * Return: NULL
*/
void initWiFi()
{
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting To WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
      Serial.print(".");
      delay(250);  
  }
  Serial.println();
  Serial.printf("Connected with IP: %s\n\n", String(WiFi.localIP()));
}


 /**
 * streamCallback - Global function that handles stream data
 * 
 * @data [StreamData]: Updated data from firebase
 * 
 * Return: NULL
*/
void streamCallback(StreamData data)
{
  bool value;
  // Getting data value
  value = data.boolData();
  // Print out all information
  Serial.println("Stream Data...");
  Serial.printf("stream path, %s\nevent path, %s\ndata type, %s\nevent type, %s\n\n",
                data.streamPath().c_str(),
                data.dataPath().c_str(),
                data.dataType().c_str(),
                data.eventType().c_str());
  handleStream(data.dataPath().c_str(), value);
}


/**
 * streamTimeoutCallback - Global function that notifies when stream connection lost
 * 
 * @timeout [bool]: if connection timeout
 * 
 * Return: NULL
*/
void streamTimeoutCallback(bool timeout)
{
  if (timeout)
    Serial.println("stream timed out, resuming...\n");

  if (!stream.httpConnected())
    Serial.printf("error code: %d, reason: %s\n\n", stream.httpCode(), stream.errorReason().c_str());
}

/**
 * handleStream - Makes an action depending on the reading from firebase
 * 
 * @path [String]: path where data changed in firebase
 * 
 * @value [bool]: New value
 * 
 * Return: NULL
*/
void handleStream(String path, bool value)
{
    
    if (path == "/led")
    {
      if (value == true)
      {
          digitalWrite(leds_pin, HIGH);
          ledCheck = true;
      }
      else
      {
        digitalWrite(leds_pin, LOW);
        ledCheck = false;
      }
        
    }     
    else if (path == "/door")
    {
      if (value == true)
        motor.write(90);
      else
        motor.write(0);
    }        
}
