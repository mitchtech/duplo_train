#include <Lpf2Hub.h>      //legoino
#include <Lpf2HubConst.h> //legoino
#include <Bounce2.h>      //bounce2

#define MAX_THROTTLE 2900  // set to maximum pot value
#define MIN_THROTTLE 1100  // set to minimum pot value
#define MAX_SPEED 64
#define MIN_SPEED -64

#define BTN_MUSIC 25
#define BTN_LIGHT 26
#define BTN_WATER 27
#define BTN_STOP 14
#define POT_SPEED 15

Lpf2Hub myHub;
byte port = (byte)PoweredUpHubPort::A;

Bounce btnMusic = Bounce();
Bounce btnLight = Bounce();
Bounce btnWater = Bounce();
Bounce btnStop = Bounce();

static int throttle = 0;
static int speed = 0;
static short color = NONE;

void pollThrottle()
{
  throttle = analogRead(POT_SPEED);
  int _speed = map(throttle, MIN_THROTTLE, MAX_THROTTLE, MIN_SPEED, MAX_SPEED);

  Serial.print("Throttle: ");
  Serial.print(throttle);
  Serial.print(" Speed: ");
  Serial.println(_speed);

  if(_speed != speed)
  { 
    if(speed == 0 && _speed > 0)
    {
        myHub.playSound((byte)DuploTrainBaseSound::STATION_DEPARTURE);
        delay(100);
    }
    speed = _speed;
    myHub.setBasicMotorSpeed(port, speed);
  }
}

Color nextColor()
{
    if(color == 255 || color >= NUM_COLORS - 1) {
        color = 0;
    } else {
        color++;
    }
    return (Color)color;
} 

void pollButtons()
{
  if(btnMusic.update())
  {
    if(btnMusic.fell())
    {
      myHub.playSound((byte)DuploTrainBaseSound::HORN);
      delay(100);
    }
  }
  if(btnLight.update())
  {
    if(btnLight.fell())
    {
      myHub.setLedColor(nextColor());
      delay(100);
    }
  }
  if(btnWater.update())
  {
    if(btnWater.fell())
    {
      myHub.playSound((byte)DuploTrainBaseSound::WATER_REFILL);
      delay(100);  
    }
  }
  if(btnStop.update())
  {
    if(btnStop.fell())
    {
      myHub.setBasicMotorSpeed(port, 0);
      delay(100);
      myHub.playSound((byte)DuploTrainBaseSound::BRAKE);
    }
  }
}

void setup() {
  Serial.begin(115200);
  //define Pin Modes
  btnMusic.attach(BTN_MUSIC, INPUT_PULLUP);
  btnLight.attach(BTN_LIGHT, INPUT_PULLUP);
  btnWater.attach(BTN_WATER, INPUT_PULLUP);
  btnStop.attach(BTN_STOP, INPUT_PULLUP);

  myHub.init();
}

void loop() {

    if (myHub.isConnecting()) {
        myHub.connectHub();
        if (myHub.isConnected()) {
            Serial.println("We are now connected to the HUB");
        } else {
            Serial.println("We have failed to connect to the HUB");
        }
    }
    if (myHub.isConnected()) {
        pollButtons();
        pollThrottle();
    } 
    delay(50);
}