/*                                       +-----+
         +----[PWR]-------------------| USB |--+
         |                            +-----+  |
         |         GND/RST2  [ ][ ]            |
         |       MOSI2/SCK2  [ ][ ]  A5/SCL[ ] |    
         |          5V/MISO2 [ ][ ]  A4/SDA[ ] |    
         |                             AREF[ ] |
         |                              GND[ ] |
         | [ ]N/C                    SCK/13[ ] |   
         | [ ]IOREF                 MISO/12[ ] |   
         | [ ]RST                   MOSI/11[X]~|   
         | [ ]3V3    +---+               10[X]~|   
         | [ ]5v    -| A |-               9[X]~|   
         | [ ]GND   -| R |-               8[X] |  
         | [ ]GND   -| D |-                    |
         | [ ]Vin   -| U |-               7[ ] |   
         |          -| I |-               6[X]~|   
         | [X]A0    -| N |-               5[X]~|   
         | [ ]A1    -| O |-               4[ ] |   
         | [X]A2     +---+           INT1/3[X]~|   
         | [X]A3                     INT0/2[X] |   
         | [ ]A4/SDA  RST SCK MISO     TX>1[ ] |   
         | [ ]A5/SCL  [ ] [ ] [ ]      RX<0[ ] |   
         |            [ ] [ ] [ ]              |
         |  UNO_R3    GND MOSI 5V  ____________/
          \_______________________/ */
//Motor PINS
const byte stdby = 10; //standby
const byte ain1 = 9; //Direction
const byte ain2 = 8; //Direction
const byte pwma = 3; //Speed control 
//LED PINS
const byte redLed = 5;    // RED pin of the LED to PWM pin 4
const byte greenLed = 6;  // GREEN pin of the LED to PWM pin 5
const byte blueLed = 11;   // BLUE pin of the LED to PWM pin 6
//Light
const byte lightSensor = 0;  
//Button
const byte manualControl = 2;
//Door Bottom
const byte bottomDoor = A2;
//Door Top
const byte topDoor = A3;

//Global
enum doorActionState {
  DOOR_IDLE,
  DOOR_OPENING,
  DOOR_CLOSEING,
};


volatile byte doorInProgress = 0;
volatile byte systemError = 1;
volatile long lastDoorCommand = 0;
volatile byte lastDoorAction = DOOR_IDLE;
unsigned long lastLightCheck = 0;
unsigned long lastErrorCorrection = 0;
const unsigned long lightTogglePeriod = 260000;
const unsigned long errorCorrectResetTime = 3600000;
const byte dayTimeLight = 60;
const byte nightTimeLight = 30;
const unsigned long  startUpDelay = 60000;
volatile byte errorAttempts = 0;


#define ENABLE_DEBUG 1

//Try to get out of error mode..Basically get the door to hit a reed switch and then system check will take care of the prober position
void errorCorrection()
{

  if(errorAttempts == 0)
  {
    errorAttempts++;
    lastErrorCorrection = millis();
    Serial.println(F("errorCorrection::Error Correction Mode")); 
    noInterrupts();
  
    if(lastDoorAction == DOOR_OPENING)
    {
  
      //Close it all the way for now...no harm in doing this
      CloseDoor();
      delay(5000);
      StopDoor();
      ledError();
  
    }
    else if(lastDoorAction == DOOR_CLOSEING)
    {
      //Makes me nervous of destroying door only try a little.
      OpenDoor(); 
      delay(1000);
      StopDoor();
      
      //Did we get some where?
      if(! isDoorClosed() && ! isDoorOpen())
      {
        CloseDoor();
        delay(3000);
        StopDoor();
      }
      
      ledError();
  
    }
  
   
    interrupts();
  }
    
}
//Monitor the Door
byte systemCheck()
{

    //Reset LED, the error could be cleared
    if(systemError)
    {
      ledOff();
      if(isDoorOpen())
      {
        ledOpen();
      }
      else if(isDoorClosed())
      {
        ledClosed();
      }
    }
    
    systemError = 0;
    Serial.println(F("systemCheck::Performing Normal System Checks")); 
      
    if(! isDoorOpen() && ! isDoorClosed())
    {
        //Where are the doors!!
        Serial.println(F("Error: We don't have contact")); 
        ledError();
        systemError = 1;
        return 0;
    }
    else if(isDoorOpen()&& isDoorClosed())
    {
        //WTF
        Serial.println(F("Error: We have to many contacts")); 
        ledError();
        systemError = 1;
        return 0;       
    }
    
    if(isDoorClosed() && isItDayTime())
    {

        Serial.println(F("systemCheck::Open Door")); 
        //We should be open
        OpenDoor();
        return 1;
    }
    else if(isDoorOpen() && isItNightTime())
    {
        //We should be closed
        Serial.println(F("systemCheck::Close Door")); 
        CloseDoor();
        return 1;
    }
    
    Serial.println(F("systemCheck::All Systems Go.")); 
    return 1;
}
void loop()
{
        //Every 3 minuites check light level and act
        if((millis() - lastLightCheck) >= lightTogglePeriod)
        {
            lastLightCheck = millis();
            systemCheck();
          //  errorCorrection
        }

        //Reset errorAttemtps to allow door to correct itself...once an hour for now.
        if((millis() - lastErrorCorrection) >= errorCorrectResetTime)
        {
            errorAttempts = 0;
        }
        

  #ifdef ENABLE_DEBUG 
    debug();
  #endif
}
byte getLightLevel()
{
    return analogRead(lightSensor);
}
byte isItDayTime()
{
    if(analogRead(lightSensor) / 4> dayTimeLight)
    {
        return 1;
    }
    
    return 0;
}
byte isItNightTime()
{
    if(analogRead(lightSensor) / 4 < nightTimeLight)
    {
        return 1;
    }
    
    return 0;
}
byte getTopDoor()
{
    return digitalRead(topDoor);
}
byte getBottomDoor()
{
    return digitalRead(bottomDoor);
}
byte isDoorOpen()
{
    if(getTopDoor() == HIGH)
    {
        return 1;
    }
    
    return 0;
}
byte isDoorClosed()
{

    if(getBottomDoor() == HIGH)
    {
        return 1;
    }
    
    return 0;
}
void userInput()
{

  if(doorInProgress == DOOR_IDLE)
  {
    if(isDoorClosed())
    {
        OpenDoor();
    }
    else if(isDoorOpen())
    {
        CloseDoor();
    }
  }

}
void OpenDoor()
{
    ledMovement();
    lastDoorAction = DOOR_OPENING;
    lastDoorCommand = millis();
    doorInProgress = 1;
    move(255, 1);
}
void CloseDoor()
{
    ledMovement();
    lastDoorAction = DOOR_CLOSEING;
    lastDoorCommand = millis();
    doorInProgress = 2;
    move(255, 0);
}
void StopDoor()
{
    lastDoorCommand = millis();
    doorInProgress = 0;
    stop();
}
void StopDoorError()
{
    lastDoorAction = DOOR_OPENING;
    lastDoorCommand = millis();
    doorInProgress = 0;
    stop();
}
void ledOff()
{
    analogWrite(redLed, 0);
    analogWrite(greenLed, 0);
    analogWrite(blueLed, 0);
}
void ledError()
{
    ledOff();
    analogWrite(redLed, 150);
    analogWrite(greenLed, 100);
    analogWrite(blueLed, 100);
}
void ledOpen()
{
    ledOff();
    analogWrite(redLed, 150);
}
void ledClosed()
{
    ledOff();
    analogWrite(greenLed, 150);
}
void ledMovement()
{
    ledOff();
    analogWrite(blueLed, 150);
}

//Move specific motor at speed and direction
//speed: 0 is off, and 255 is full speed
//direction: 0 clockwise, 1 counter-clockwise
void move(byte speed, byte direction)
{
    digitalWrite(stdby, HIGH); //disable standby

    boolean inPin1 = LOW;
    boolean inPin2 = HIGH;

    if(direction == 1)
    {
        inPin1 = HIGH;
        inPin2 = LOW;
    }

    digitalWrite(ain1, inPin1);
    digitalWrite(ain2, inPin2);
    analogWrite(pwma, speed);
    
}
void stop()
{
    //enable standby  
    digitalWrite(stdby, LOW); 
}

void pciSetup(byte pin) 
{
    *digitalPinToPCMSK(pin) |= bit (digitalPinToPCMSKbit(pin));  // enable pin
    PCIFR  |= bit (digitalPinToPCICRbit(pin)); // clear any outstanding interrupt
    PCICR  |= bit (digitalPinToPCICRbit(pin)); // enable interrupt for the group 
}
//Singal when door changed to open or closed.
ISR (PCINT1_vect) // handle pin change interrupt for A0 to A5 here
{
    byte topDoorStatus = getTopDoor();
    Serial.print(F("Top Door Interrupt: "));
    Serial.print(topDoorStatus);  

    byte bottomDoorStatus = getBottomDoor();
    Serial.print(F(" -- Bottom Door Interrupt: "));
    Serial.println(bottomDoorStatus);

    if(doorInProgress == DOOR_OPENING && topDoorStatus)
    {
        //Sometimes the reed triggers a little early...go up a Tad.
        delay(1000);
        StopDoor();
        ledOpen();
    }
    else if(doorInProgress == DOOR_CLOSEING && bottomDoorStatus)
    {
        //Give extra slack to lock door
        delay(6000);   
        StopDoor();
        ledClosed();
    }
    else if(doorInProgress == DOOR_CLOSEING && topDoorStatus)
    {
        StopDoorError();
        ledError();
    }
}
void setup()
{

    //Some wierdness when power was quickly cut..lets delay so we dont a bad reading.
 //   delay(startUpDelay);
    
    Serial.begin(9600);
    //Motor
    pinMode(stdby, OUTPUT);
    pinMode(pwma, OUTPUT);
    pinMode(ain1, OUTPUT);
    pinMode(ain2, OUTPUT);
    //Door Top
    pinMode(topDoor, INPUT);    // sets the digital pin as input to read switch
    pciSetup(topDoor);
    //Door Bottom
    pinMode(bottomDoor, INPUT);    // sets the digital pin as input to read switch
    pciSetup(bottomDoor);
    //manualControl
    pinMode(manualControl, INPUT);
    attachInterrupt(digitalPinToInterrupt(manualControl), userInput, FALLING);
    //Light
    pinMode(lightSensor, OUTPUT);    // sets the digital pin as input to read switch
    //LED
    pinMode(redLed, OUTPUT);
    pinMode(greenLed, OUTPUT);
    pinMode(blueLed, OUTPUT);
  
    // turn on interrupts
    interrupts();

    systemCheck();
    
    lastLightCheck = millis();
}

#ifdef ENABLE_DEBUG
void debug()
{

    if(Serial.available())
    {
        byte incoming = Serial.read();
        if(incoming == 'U') //UP
        {

          Serial.println(F("Moving")); 
          move(255, 1);
          delay(2500);
          stop();
          Serial.println(F("Done")); 

        }
        else if(incoming == 'D') //Down
        {
          Serial.println(F("Moving other")); 
          move(255, 0); 
          delay(2500);
          stop();
          Serial.println(F("Done")); 
        }
        else if(incoming == 'R') //Reeds
        {
          
          Serial.print(F("Door Top"));
          Serial.println(digitalRead(topDoor));

          Serial.print(F("Door Bottom"));
          Serial.println(digitalRead(bottomDoor));
        }
        else if(incoming == 'L') //LED
        {
            Serial.println(F("LED"));
            ledError();
            delay(2500);
            ledOpen();
            delay(2500);
            ledClosed();
            delay(2500);
            ledMovement();
        }
        else if(incoming == 'S') //Light Sensor
        {

          Serial.print(F("Light"));
          Serial.println(analogRead(lightSensor) / 4);
        }
        else if(incoming == 'Z') //User Control
        {

          Serial.print(F("manualControl"));
          Serial.println(digitalRead(manualControl));
        }
  }
}
#endif

