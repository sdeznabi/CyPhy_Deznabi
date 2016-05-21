// This program is written by Sina Deznabi as part of an assignment for the course Cyber Physical Systems

#include "Arduino.h"
#include <SoftwareSerial.h>

// Include Temperature and Humidity sensor library
#include "DHT.h"

// Sensor pin
#define DHTPIN A0 
 
// The temperature and humidity sensor type
#define DHTTYPE DHT11 // DHT 11

// Define the temperature and humidity sampling and sending rate in milliseconds
#define rate 2000 

// Define the threshold for temperature to send alert SMS
#define tmp_thr 28


// Define the continuous duration for the temperature to be higher than threshold
// in order to send the emergency SMS in seconds
#define duration 10

// Define the Sensor ID which is given from the website
#define sensor_id 7


DHT dht(DHTPIN, DHTTYPE);
 

// Create a software serial port to use for lcd
SoftwareSerial lcd = SoftwareSerial(6,7);

// Temperature and Humidity
float h,t;
int t_int;

// GSM answer
int8_t answer;

char response[100];  
unsigned long previous;

int onModulePin= 2;

// Auxiliary string to manipulate and create AT commands
char aux_str[100];

int x = 0;

// SIM pin code
char pin[]="1234";

// GPRS Parameters
char apn[]="internet";
char user_name[]="";
char password[]="";

// Server URL
//char url[ ]="52.18.212.113:5000";
char url[ ] = "52.18.212.113:5000/api/v1/readings";


// This is the number to send SMS to if the temperature is higher than 
// a threshold for some continuous duration
char phone_number[]="0466143520";   

// Duration in seconds


int counter = 0;

// Is SMS sent
// We want to send SMS only once
boolean issent = 0;
 

void setup() {

    // Iitialize the LCD
    init_lcd();

    // Start the sensor
    dht.begin();
  
    // SIM900 power pin
    pinMode(onModulePin, OUTPUT);
    
    Serial.begin(115200);   

    Serial.println("Starting...");
    
    // Turn on SIM900
    power_on();

    delay(3000);
   
    //sets the PIN code
    snprintf(aux_str, sizeof(aux_str), "AT+CPIN=%s", pin);
    sendATcommand(aux_str, "OK", 2000);

    delay(3000);

    while (sendATcommand2("AT+CREG?", "+CREG: 0,1", "+CREG: 0,5", 2000) == 0);

    Serial.print("Setting SMS mode...");
    sendATcommand("AT+CMGF=1", "OK", 1000);    // sets the SMS mode to text
    
    // sets APN , user name and password
    sendATcommand("AT+SAPBR=3,1,\"Contype\",\"GPRS\"", "OK", 2000);
    snprintf(aux_str, sizeof(aux_str), "AT+SAPBR=3,1,\"APN\",\"%s\"", apn);
    sendATcommand(aux_str, "OK", 2000);
    
    snprintf(aux_str, sizeof(aux_str), "AT+SAPBR=3,1,\"USER\",\"%s\"", user_name);
    sendATcommand(aux_str, "OK", 2000);
    
    snprintf(aux_str, sizeof(aux_str), "AT+SAPBR=3,1,\"PWD\",\"%s\"", password);
    sendATcommand(aux_str, "OK", 2000);

    // Make the GPRS module to register an IP for itself
    while (sendATcommand("AT+SAPBR=1,1", "OK", 20000) == 0)
    {
        delay(5000);
    }
    
  
}

void loop() {

    if (millis () % rate == 0) 
    {
      // Reading temperature or humidity takes about 250 milliseconds!
      h = dht.readHumidity();
      t = dht.readTemperature();

      // check if returns are valid, if they are NaN (not a number) then something went wrong!
      if (isnan(t) || isnan(h))
      {
          lcd.println("Failed to read from DHT");
      }
      else
      {
          
          // Assign float to int
          t_int = t;
          
          
          // Check if the temperature is higher than the threshold
          // And add the counter to see if it has been high for 
          // designated continuous amount of time 
          if (t_int >= tmp_thr)
          {
             
             counter++;
             Serial.println("TEMPERATURE HIGHER THAN THRESHOLD");
             if (counter >= (duration*1000)/rate)
             {
               if (!issent)
               {
                 snprintf(aux_str, sizeof(aux_str), "temperature is %d. It has been higher than threshold for %d seconds", t_int, duration);
                 send_sms(aux_str);
                 issent = 1;
                 counter = 0;
               }
             }
          }
          else
          {
            counter = 0;
          }
          
          // clear screen
          lcd.write(0xFE);
          lcd.write(0x58);
          delay(10);   

    
          lcd.print("Humid: ");
          lcd.println(h);
          //lcd.print(" %\t");
          lcd.print("Temp: ");
          lcd.print(t);
          lcd.print(" *C");
          
          
         // Initializes HTTP service
         answer = sendATcommand("AT+HTTPINIT", "OK", 10000);
         if (answer == 1)
         {
            // Sets CID parameter
            answer = sendATcommand("AT+HTTPPARA=\"CID\",1", "OK", 5000);
            if (answer == 1)
            {
                // Sets url 
            
                snprintf(aux_str, sizeof(aux_str), "AT+HTTPPARA=\"URL\",\"%s\"", url);
            
                answer = sendATcommand(aux_str, "OK", 5000);
                if (answer == 1)
                {

                    // Set the content type
                    // The data will be put in the content-type                   
                    Serial.print("Temperature is ");
                    Serial.println(t_int);
                    
                    //snprintf(aux_str, sizeof(aux_str), "AT+HTTPPARA=\"CONTENT\",\"%d\"",t_int);
                    snprintf(aux_str, sizeof(aux_str), "AT+HTTPPARA=\"CONTENT\",\"Temperature: %d Sensor-Id: %d\"",t_int, sensor_id); 
                    sendATcommand(aux_str, "OK", 10000);
                    //sendATcommand("AT+HTTPPARA=\"CONTENT\",\"text\"", "OK", 10000);

                    // Send the POST request containing the data
                    answer = sendATcommand("AT+HTTPACTION=1", "OK", 10000);
                
                    // Start reading the confirmation from the module that the request was successful
                    memset(response, '\0', 100);    // Initialize the string
                    x = 0;
                    previous = millis();
                    do{
                         if(Serial.available() != 0){    
                             response[x] = Serial.read();
                             x++;
              
                          }

                    }while ((x < 100) && (millis()-previous < 10000));
               
                    Serial.println(response);
                }
                else
                {
                    Serial.println("Error setting the url");
                }
            }
            else
            {
                Serial.println("Error setting the CID");
            }    
        }
        else
        {
            Serial.println("Error initializing");
        }
    
        // End the HTTP session
        sendATcommand("AT+HTTPTERM", "OK", 5000);          
      }
       
    }  
}



/*************************************************************************************/
/*************************************FUNCTIONS***************************************/
/*************************************************************************************/

// LCD INITIALIZATION
void init_lcd (){
  lcd.begin(9600);  
  
  // set the size of the display
  lcd.write(0xFE);
  lcd.write(0xD1);
  lcd.write(16);  // 16 columns
  lcd.write(2);   // 2 rows
  delay(10);       
  // put delays after each command to make sure the data 
  // is sent and the LCD is updated.

  // set the contrast
  lcd.write(0xFE);
  lcd.write(0x50);
  lcd.write(200);
  delay(10);       
  
  // set the brightness - we'll max it (255 is max brightness)
  lcd.write(0xFE);
  lcd.write(0x99);
  lcd.write(255);
  delay(10);       
  
  // turn off cursors
  lcd.write(0xFE);
  lcd.write(0x4B);
  lcd.write(0xFE);
  lcd.write(0x54);
  
  
  // clear screen
  lcd.write(0xFE);
  lcd.write(0x58);
  delay(10);   // delays after each command 
  
  // go 'home'
  lcd.write(0xFE);
  lcd.write(0x48);
  delay(10);   
 }
 
 
 // SIM900 POWER 
 void power_on(){

    uint8_t answer=0;

    // checks if the module is started
    answer = sendATcommand("AT", "OK", 2000);
    if (answer == 0)
    {
        // power on pulse
        digitalWrite(onModulePin,HIGH);
        delay(3000);
        digitalWrite(onModulePin,LOW);

        // waits for an answer from the module
        while(answer == 0){  
            // Send AT every two seconds and wait for the answer   
            answer = sendATcommand("AT", "OK", 2000);    
        }
    }

}


// SEND AT COMMAND WITH ONE POSSIBLE ANSWER 
int8_t sendATcommand(char* ATcommand, char* expected_answer1, unsigned int timeout){

    uint8_t x=0,  answer=0;
    char response[100];
    unsigned long previous;

    memset(response, '\0', 100);    // Initialize the string

    delay(100);

    while( Serial.available() > 0) Serial.read();    // Clean the input buffer

    Serial.println(ATcommand);    // Send the AT command 


        x = 0;
    previous = millis();

    // this loop waits for the answer
    do{
        if(Serial.available() != 0){    
            response[x] = Serial.read();
            x++;
            // check if the desired answer is in the response of the module
            if (strstr(response, expected_answer1) != NULL)    
            {
                answer = 1;
            }
        }
        // Waits for the asnwer with time out
    }
    while((answer == 0) && ((millis() - previous) < timeout));    

    return answer;
}


// SEND AT COMMAND WITH TWO POSSIBLE ANSWERS
int8_t sendATcommand2(char* ATcommand, char* expected_answer1, char* expected_answer2, unsigned int timeout){

    uint8_t x=0,  answer=0;
    char response[100];
    unsigned long previous;

    memset(response, '\0', 100);    // Initialize the string

    delay(100);

    while( Serial.available() > 0) Serial.read();    // Clean the input buffer

    Serial.println(ATcommand);    // Send the AT command 


        x = 0;
    previous = millis();

    // this loop waits for the answer
    do{        
        if(Serial.available() != 0){    
            response[x] = Serial.read();
            x++;
            // check if the desired answer 1 is in the response of the module
            if (strstr(response, expected_answer1) != NULL)    
            {
                answer = 1;
            }
            // check if the desired answer 2 is in the response of the module
            if (strstr(response, expected_answer2) != NULL)    
            {
                answer = 2;
            }
        }
        // Waits for the asnwer with time out
    }while((answer == 0) && ((millis() - previous) < timeout));    

    return answer;
}
    

// Send SMS to the globally defined phone number
void send_sms(char* text){  
    // Auxiliary string to manipulate and create AT commands
    char aux_str1[100];  
    sprintf(aux_str1,"AT+CMGS=\"%s\"", phone_number);
    answer = sendATcommand(aux_str1, ">", 2000);    // send the SMS number
    if (answer == 1)
    {
        Serial.println(text);
        Serial.write(0x1A);
        answer = sendATcommand("", "OK", 20000);
        if (answer == 1)
        {
            Serial.print("Sent ");    
        }
        else
        {
            Serial.print("error ");
        }
    }
    else
    {
        Serial.print("error ");
        Serial.println(answer, DEC);
    } 
}
 
