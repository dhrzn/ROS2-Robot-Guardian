#define trigger_pin 9   //this is what fires the soundwaves
#define echo_recieve_pin 10 //this is what reads the time it took back for soundwave to come back

#include <Servo.h>
#define buzzer_pin 4
#define LED_pin 6
#define servo_pin 5 //where our servo pin is at

Servo myServo;  //defining our servo


void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  pinMode(trigger_pin,OUTPUT); //output because it sends soundwaves
  pinMode(echo_recieve_pin,INPUT); //input because this is what READS the time
  pinMode(buzzer_pin, OUTPUT);
  pinMode(LED_pin,OUTPUT);
  myServo.attach(servo_pin); //tells ardunio at which pin it can control the servo (has access to PWM)
  myServo.write(0); //sets the starting position of the servo at zero

}

void loop() {
  // put your main code here, to run repeatedly:
  
/// this is what we are sending to ROS


  digitalWrite(trigger_pin,LOW); //clean reset to start fresh
  delayMicroseconds(2);  // waiting
  digitalWrite(trigger_pin,HIGH); // sending soundwaves
  delayMicroseconds(10);  // duration for how long trigger_pin fires the soundwaves
  digitalWrite(trigger_pin,LOW);  //ending soundwaves

  
  //reading the time it took back for waves to come back (using float for storing time measurements since we are in microseconds)
  long duration = pulseIn(echo_recieve_pin,HIGH); 

  //calculating the distance said object is (IN CM)
  float distance = (duration/2.0)* 0.0343;

//(what we are sending to ros)
  Serial.println(distance);  //makes it easy for ros2 bridge to detect vaild distance messages



  // this is what we are recieving back from ROS

  if (Serial.available()>0) {   //this checks to see if anything is waiting to be read
    String incoming = Serial.readStringUntil('\n'); //this reads one line of data, and per iteration if it reads all the way up to '/n'. after it spots the \n  it to moves and reads the following line
   
   //defining what happens when it recieves the alert message
    if (incoming.startsWith("ALERT:")) { //this just checks that the message starts with "ALERT" before we process it and its basically a saftey check to avoid random noises coming through the serial. we ONLY want to react to real ALERT messages {
      float distance = incoming.substring(6).toFloat();  //"Alert:" is only 6 charecter with the colon. so this is us starting from index 6 and gives us the distance number. then ".toFloat" converts that string to an actual float
      //(so "Alert:15.32") becomes just 15.32
      int intensity = map(distance, 0, 30, 255,0); //this takes distance which is between 0 and 30, and map it to a value between 255 and 0. which means (closer = higher value = more intense)
      intensity = constrain(intensity, 0, 255);  //tis is a saftey net, because if sensor mistakenly reads -1 or 300, the constraint keeps intensity locked between 0 and 255 no matter what


//this is where the LED_pin and buzzer would react given and proportional to the intensity

      analogWrite(LED_pin,intensity);  
      analogWrite(buzzer_pin, intensity);
      myServo.write(map(intensity, 0 , 255, 0, 180)); //servo needs a seperate one because it runs on position 
    }



  // defining what happens when it recieves the clear message

  else if (incoming.startsWith("CLEAR")){
    analogWrite(LED_pin, 0); //making sure led fades when above threshold
    analogWrite(buzzer_pin, 0); //making sure buzzer fades 
    myServo.write(0); //setting and keeping servo at origional position if and when distance above threshold
  }
    
  }
delay(50);
}
