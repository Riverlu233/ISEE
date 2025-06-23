#include <EEPROM.h>
#define TIMEMAX 6000

int segmentPins[] = {3,2,19,16,18,4,5,17}; //means the "a-g & dp" of one digital 7-segment digital tube
int displayPins[] = {6,15,7,14};//the control signal of the four 7-segment digital tube
int times=10; //initial time of every setup
int reserve;//used to record the time when pause is executed
int timerMinute;
int timerSecond;
int buzzerPin = 11;
int aPin = 8;
int bPin = 10;
int buttonPin = 9;
int stop = 1;//to represent the state of the counter

byte digits[11][8] = {
 {1,1,1,1,1,1,0,0},//0
 {0,1,1,0,0,0,0,0},//1
 {1,1,0,1,1,0,1,0},//2
 {1,1,1,1,0,0,1,0},//3
 {0,1,1,0,0,1,1,0},//4
 {1,0,1,1,0,1,1,0},//5
 {1,0,1,1,1,1,1,0},//6
 {1,1,1,0,0,0,0,0},//7
 {1,1,1,1,1,1,1,0},//8
 {1,1,1,1,0,1,1,0},//9
 {1,1,1,1,1,1,1,1} //None
};

//variables that checking the button state
int buttonState = HIGH;
int lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;
unsigned long lastPressTime = 0;
int pressCount = 0;
const unsigned long doubleClickDelay = 500;
int change_state=1;
int pause_flag=1;
int pause=0;

void setup() {
    for(int i=0;i<8;i++) pinMode(segmentPins[i],OUTPUT);
    for(int i=0;i<4;i++) pinMode(displayPins[i],OUTPUT);
    pinMode(buzzerPin,OUTPUT);
    pinMode(buttonPin,INPUT_PULLUP);
    pinMode(aPin,INPUT);
    pinMode(bPin,INPUT);
    timerMinute = times/100;
    timerSecond = times%100;
}

void loop() {
    checkButton();
    updateDisplay();
}

void checkButton() {
    int reading = digitalRead(buttonPin);
    if (reading != lastButtonState) {
        lastDebounceTime = millis(); //when state_change,first record time to keep waiting
    }
    if ((millis() - lastDebounceTime) > debounceDelay) {//Sampling only after the flutter period finished
        if (reading != buttonState) {
            buttonState = reading;//when Sampling a different state, we believe the state change,record
            if (buttonState == LOW) {
                unsigned long currentMillis = millis();
                if (currentMillis - lastPressTime > doubleClickDelay) {
                //The number of times the counting buttons are pressed during an double
                // click delay time is accumulated
                    pressCount = 1;
                } else {
                    pressCount++;
                }
                lastPressTime = currentMillis;
            }
        }
    }
    lastButtonState = reading;
    if (pressCount > 0 && (millis() - lastPressTime) >= doubleClickDelay) {
        if (pressCount == 1) {//only pressed once
            handleSingleClick();
        } else if (pressCount >= 2) {//pressed twice
            handleDoubleClick();
        }
        pressCount = 0;
    }
}

void handleSingleClick() {
    stop = !stop;//pause or start form reset;
    if (pause_flag){ //if it is not a pause,just a start
      pause_flag=0;//to change it to 0,showing it isn't pause
    }
    else{ //it is paused
      pause=!pause;//lift the suspension,continue
    }
    if (change_state){
        reserve=times;//record the set time when start from reset
        change_state=0;//ensure that reverse was saved just once at the beginning
    }
    times=timerMinute*100+timerSecond;
}

void handleDoubleClick() {
    stop = 1;// reset to stop
    pause_flag=1;//reset this variable
    pause=0; //show that it is reset state but not pause state
    change_state=1;//next time set time will be recorded
    times=reserve;//show the time that set last time
    timerMinute = times/100;
    timerSecond = times%100;
    digitalWrite(buzzerPin, LOW);//stop the ringing
}

void updateDisplay(){ //to decide the content of display
    int minsecs = timerMinute*100+timerSecond;
    int v = minsecs;
    //Considering that the format is "minute:second", the algorithm is valid
    for(int i=0;i<4;i++){ 
        int digit=v%10;
        setDigit(i); //delight the LED in turn
        setSegments(digit);  //show the number digit
        v=v/10; //next decimal
        process(); //update or change the set
    }
    setDigit(5);// all digits off to prevent uneven illumination
}

void process(){
    for(int i=0;i<100; i++)//tweak this number between flicker and blur
    {
        int change=getEncoderTurn(); //detect whether the set time changes
        if (stop){ //when the clock isn't counting down
            changeSetTime(change); //change the set time
        }   
        else{
            updateCountingTime(); //when it's working, you can't set time and just keep counting down
        }
    }
    if (timerMinute == 0 && timerSecond == 0){ //Countdown has over, ring the buzz
        digitalWrite(buzzerPin,HIGH);
    }
}

void changeSetTime(int change){
    if (change==1){//yes we should add time now
        times++;
        if (times>TIMEMAX){//when time hit the max, back to 1 (0 makes no sense)
            times=1;
        }
        if (times%100>59){//when seconds hit 60,back to 0 and minute plus 1
            times=(times/100+1)*100;
        }

    }
    else if (change==-1){//yes we should minus time now
        times--;
        if (times==0){//same as case +
            times=TIMEMAX;
        }
        if (times%100>59){
            times=times-40;
        }
    }
    //change = 1->set time increase; Insteadily change = 0->set time decrease
    timerMinute=times/100;
    timerSecond=times%100;
}

void updateCountingTime(){
    static unsigned long lastMillis;
    unsigned long m=millis(); //get the time takes from the start
    if (m>(lastMillis+1000) && (timerSecond>0 || timerMinute>0)){
        //Means (at least) 1s has passed since the last update
        //Also, the time can't be 00:00, when we should stop
        digitalWrite(buzzerPin,HIGH); //to produce the tick-tock
        delay(10);
        digitalWrite(buzzerPin,LOW);
        if(timerSecond==0){
            timerSecond=59; //when second = 0, minute - 1, second to 59
            timerMinute--;
        }
        else{
            timerSecond--; //update the second(minus 1) when second > 0
        }
        lastMillis = m; //record the last time of update
    }
}

void setDigit(int digit){ //set the digit-th(-1) segment pins to be high
    for(int i=0;i<4;i++){
        digitalWrite(displayPins[i],(digit!=i));
    }
}

void setSegments(int n){ //set LED to display number 'n'
    for(int i=0;i<8; i++){
        digitalWrite (segmentPins[i], !digits[n][i]); //for the LED, low votage->validate
    }
}

int getEncoderTurn(){
    // return -1，0，or +1
    static int oldA=LOW; //Initialize the previous state (only execute once)
    static int oldB=LOW; //Same
    int result = 0;
    int newA=digitalRead(aPin); //Read in the state of rotary encoder right now
    int newB=digitalRead(bPin);
    if (newA != oldA || newB != oldB){ //Means the state has changed
        if (oldA == HIGH && newA==LOW){ //When A change the state
            if (newB == newA){ //A=B->counter clock wise
                result = 1;
            }
            else if (newB != newA){ //A!=B->clock wise
                result = -1;
            }
        }
    }
    oldA = newA; //update the previous state
    oldB = newB;
    if (pause==1){//when pause, you cannot change the time
      result=0;
    }
    return result; //0: do not change ; 1: counter clock wise ; -1: clock wise
}