//lcd
#include<LiquidCrystal.h>
LiquidCrystal lcd(9, 8, 7, 6, 5, 4);

//keypad
#include<Keypad.h>
#define rows 4
#define cols 4
char Keys[rows][cols] = {
  {'7', '8', '9', '/'},
  {'4', '5', '6', 'X'},
  {'1', '2', '3', '-'},
  {'C', '0', '=', '+'}
};
byte pin_row[rows] = {14, 15, 16, 17};
byte pin_col[cols] = {18, 19, 20, 21};
Keypad keypad = Keypad(makeKeymap(Keys), pin_row, pin_col, rows, cols) ;

//eeprom
#include<EEPROM.h>

//buzzer
#define Buzzer 10

// MQ 2 sensor
#define MQ_A 2 //mq_2



// DHT11 sensor
#include<dht.h>
#define dht_apin A0
dht DHT;


//LDR sensor
#define LDR A1
int  ldr ;
float temp ;
float humidity;


//UltraSoinc sensor
#define Echo 11
#define Trig 12
float distance_m, duration;
int check ;
int check1 = 0;

//Pir sensor
#define Pir 22

//fan motor
#define int1 41
#define int2 40
//Servo motor
#include <Servo.h>
#define servo 24
Servo motor ;

// button to turn of the buzzer of iterrupt
#define button 25

// light
#define led_A 26 //first floor
#define led_B 27 // second floor

//mills
unsigned long Time = 0 ;
int mask = 0 ;

// Sound sensor
#define Sound A3
#define threshold 220




void setup() {
  //lcd
  lcd.begin(16, 2);
  lcd.clear();
  Serial.begin(9600);

  pinMode(Buzzer, OUTPUT); //buzzer
  pinMode(LDR, INPUT);
  pinMode(Pir, INPUT);
  pinMode(Echo, INPUT);
  pinMode(Trig, OUTPUT);
  pinMode(button, INPUT_PULLUP);
  pinMode(led_B, OUTPUT);
  pinMode(led_A, OUTPUT);
  pinMode(Sound, INPUT);



  motor.attach(servo);

  digitalWrite(led_A, 0);
  digitalWrite(led_B, 0);


  attachInterrupt(digitalPinToInterrupt(MQ_A), interrupt, RISING); //inerrupt for mq_2

  Time = millis();

}
//Function of write password in eeprom
void write_password(int adress_len , String password)
{
  int len = password.length();
  EEPROM.write(adress_len, len);
  for (int i = 0 ; i < len ; i++)
  {
    EEPROM.write(adress_len + 1 + i, password[i]);
  }
}



//Function of read the password from eeprom
String Read_password(int adress_len)
{
  int len = EEPROM.read(adress_len);
  char data[len + 1];
  for (int i = 0 ; i < len ; i++)
  {
    data[i] = EEPROM.read(adress_len + 1 + i);
  }
  data[len] = '\0' ;

  return String(data) ;
}

//function interrupt for Emergency
void interrupt()
{
  while (true)
  {
    digitalWrite(Buzzer, HIGH);
    lcd.clear();
    lcd.print("Emergency");
    Serial.println("Emergency");
    int x = digitalRead(button);
    char data1 = Serial.read();
    if (data1 == 'a' || x == 0)
    {
      digitalWrite(Buzzer, LOW);
      break;
    }

  }
}




void loop() {
  lcd.clear();
  // code for Lm and LDR
  ldr = analogRead(LDR);
  DHT.read11(dht_apin);
  temp = DHT.temperature;
  humidity = DHT.humidity;
  if (ldr >= 400 && DHT.temperature >= 40 )
  {
    while (true)
      Serial.println("Fire");
    //    interrupt();

  }





  //new password

  if (EEPROM.read(0) != 1)
  {
lable:
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("New password :");
    lcd.setCursor(0, 1);
    lcd.print("=>");
    String password = "";
    String con_password = "";
    bool finish = false;

    while (true)
    {
      char Key = keypad.getKey();

      if (Key == '=')
      {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("password again:");
        lcd.setCursor(0, 1);
        lcd.print("=>");
        while (true)
        {
          char Keya = keypad.getKey();
          if (Keya == '=')
          {
            if (con_password.equals(password))
            {
              EEPROM.write(0, 1);
              EEPROM.write(1, 0);
              write_password(2, password);
              finish = true;
              lcd.clear();
              lcd.print("The Password is");
              lcd.setCursor(0, 1);
              lcd.print("set Successfully");

              break;

            } else
            {
              lcd.clear();
              lcd.print("The Password is");
              lcd.setCursor(0, 1);
              lcd.print("Not Successfully");
              delay(1000);
              goto lable ;

            }
          } else if (Keya)
          {
            con_password += String(Keya);
            lcd.print("*");
          }

        }
      }
      else if (Key)
      {
        password += String(Key);
        lcd.print("*");
      }
      if (finish)
      {
        break;
      }
    }


  }
  //have password
  else
  {
    int Try = EEPROM.read(1);
    String password = Read_password(2);
    bool is_correct = false;
    for (int i = Try ; i < 3 ; i++)
    {
      delay(1000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Enter password :");
      lcd.setCursor(0, 1);
      lcd.print("=>");
      String userpassword = "";
      while (true)
      {
        char Key = keypad.getKey();

        if (Key == '=')
        {
          if (userpassword.equals(password))
          {
            lcd.clear();
            lcd.print("Correct!");
            delay(1000);
            is_correct = true;
            break;

          } else
          {
            lcd.clear();
            lcd.print("inCorrect!");
            is_correct = false;
            EEPROM.write(1, Try + 1);
            break;

          }

        }
        if (Key)
        {
          userpassword += String(Key);
          lcd.print("*");
        }
      }
      if (is_correct)
      {
        break;
      }
    }
    if (is_correct)
    {
      lcd.clear();

      while (true) {



        ldr = analogRead(LDR);
        temp = DHT.temperature ;
        if (ldr >= 400 && DHT.temperature >= 40 )
        {

          Serial.println("Fire");
          interrupt();

        }





        // desplay tempreture & humidty
        lcd.clear();
        lcd.print("T: " + String(DHT.temperature) + " & H: " + String(DHT.humidity));



        char Data = Serial.read();
        switch (Data)
        {

          case 'o':  // open door
            motor.write(180);
            check1 = 1;
            break;
          case 'c': // close door
            motor.write(90);
            check1 = 0;
            break;

          case 'n': // turn on the light first floor
            digitalWrite(led_A, HIGH);
            break;
          case 'f':
            digitalWrite(led_A, LOW);
            break;
          case 's': // turn on the light Second floor
            digitalWrite(led_B, HIGH);
            break;
          case 'k':
            digitalWrite(led_B, LOW);
            break;

          case 'F': // turn off the light of the house
            digitalWrite(led_A, LOW);
            digitalWrite(led_B, LOW);
            break;

        }
        //sound light on
        int soundval = analogRead(Sound);
        if ( Data == 'N' || soundval >= threshold )
        {
          digitalWrite(led_A, HIGH);
          digitalWrite(led_B, HIGH);
        }
        //fan on
        if (temp >= 20 && temp < 40)
        {
          digitalWrite(int1, 1);
          digitalWrite(int2, 0);
        }
        bool pir = digitalRead(Pir);
        digitalWrite(Trig, LOW);
        delayMicroseconds(10);
        digitalWrite(Trig, HIGH);
        delayMicroseconds(10);
        digitalWrite(Trig, LOW);
        duration = pulseIn(Echo, HIGH);
        distance_m = 0.00034 * 0.5 * duration;
        if (check1 == 0 && pir == true)
        {
          if ( distance_m < 0.5)
          {
            motor.write(180);
            check1 = 1;
          }
          else if (distance_m > 0.5 || distance_m < 0)
          {
            if (mask == 0)
            {
              Time = millis();
            }
            if (check == 1)
            {
              mask = 1 ;
              if (millis() - Time > 5000)
              {
                motor.write(90);
                Time = millis();
                mask = 0;
                check = 0;
              }
            }

          }

        }else if(check1 == 0 && pir == false)
        {
          if (mask == 0)
            {
              Time = millis();
            }
            if (check == 1)
            {
              mask = 1 ;
              if (millis() - Time > 5000)
              {
                motor.write(90);
                Time = millis();
                mask = 0;
                check = 0;
              }
            }
          
        }


      }
    }
    else {
      lcd.clear();
      lcd.print("ALARM!");
      Serial.println("Thief"); //
      digitalWrite(Buzzer, HIGH);
      while (true)
      {
        char data2 = Serial.read();
        int y = digitalRead(button);
        if (data2 == 'a' || y == 0)
        {
          digitalWrite(Buzzer, LOW);
          EEPROM.write(0, 0);
          break;
        }
      }



    }
  }


}
