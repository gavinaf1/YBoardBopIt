#include "accel_led.h"
#include "accelerometer.h"
#include "accelerometer.cpp"
#include "yboard.h"
#include <Arduino.h>
#include <cmath>

/*
-------------------------IMPORTANT NOTES-------------------------
1.) Whenever a new micro game is completed it will need to be
added to the switch statement just like shake_it() and 
simon_says().

2.) The int num_games needs to equal the number of games in the 
switch statement. If num games isn't high enough some of the
games won't get played at all. if num_games is too small
then the default: case will trigger more often the higher 
num_games is, resulting in one game being played over and over.

3.) The comments I wrote should be quite helpful. You should
probably read over them.

4.) You can do as many or as few games as you want, and they
can be as simple or complex as you want them to be. I think they
are fun to make, so as long as you are enjoying yourself feel
free to keep going.

5.) I've been adding a check for the second switch any time
I make sound. You can see an example in either success(), or
game_over(). That way you can mute the board if you flick off
the second switch. I'm happy to change that behavoir if you
want to use the switch for a game. Just let me know.

6.) You can colapse functions (in case you didn't already
know) which will make everything look a little cleaner.
It's the drop down arrow all the way to the left, past the
line number next to any set of brackets.

7.) You can update the Readme if you want. You can put
whatever you want in there. I just put that we worked
on it, but I don't know your last name. Feel free to add
it or remove it altogether.

8.) Don't worry about adding comments. I'm pretty decent
at reading code so I mostly put all the comments for other
people. I almost never comment my own code.

9.) Feel free to ask if you have any questions or need a pointer.
I'm always happy to help out.
*/


//these give you the display height and width, which can be very useful for rendering things on the screen.
static int display_height = 63;
static int display_width = 127;

//change this to the number of games currently added
int num_games = 5;

//this is the score of the person playing the game.
int score = 0;

//used later to check if the player won their micro game.
bool game_won = true;


//this initializes the display and accelerometer for use later.
void setup() {
  Serial.begin(115200); // Serial
  Serial.println("Starting up...");
  Yboard.setup();  // Y-Board
  setup_display(); // Display
  setup_axes();    // Axes
  Serial.println("All set, ready to go!");
}


//this function will draw the score text at the x/y that is passed in defaulting to the top left.
void draw_score(int x = 0, int y = 0){  

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Score: ");
  display.print(score);

}


//this function is similar to the draw score but requires the time to be passed in.
void draw_time(float time, int x = 0, int y = 0){ 

  display.setTextSize(1);
  display.setCursor(x, y);
  display.print("Time: ");
  display.print(time);
}


//this will return a number of seconds that decreases the higher the score gets. The maximum time being the value you passed in.
//you can input any real number of seconds like 10 or 2.5 or 9/4 as the maximum value.
//don't forget to save the return value to a variable of type float.
float get_time(float seconds){  

  return (20.0/(static_cast<float>(score)+20.0))*1000.0*seconds;

}


//This function tells the player they have successfully completed a game and increases their score.
void success(){

  Yboard.set_all_leds_color(0, 255, 0);

  score++;

  display.clearDisplay();
  display.setCursor(0, 10);
  display.print("Great");

  display.setCursor(0, 35);
  display.print("Job!");
  display.display();

  //checks if the second switch is enabled.
  if(Yboard.get_switch(2)){

    //if it is then play a success sound.
    Yboard.play_notes("O5 A8 D8");

  }
  
  delay(200);

  Yboard.set_all_leds_color(0, 0, 0);

}


//This function tells the player they lost and their score before restarting the game again from the beginning.
void game_over(){

  Yboard.set_all_leds_color(255, 0, 0);

  display.setTextSize(3);
  display.clearDisplay();
  display.setCursor(0, 10);
  display.print("GAME");
  display.setCursor(0,40);
  display.print("OVER!");

  draw_score();

  display.display();

  score = 0;

  //checks if the second switch is enabled
  if(Yboard.get_switch(2)){

    //if it is, play a sad little jingle.
    Yboard.play_notes("O5 D8 C4 A8 O4 F4 E8 F8");

  }

  while(!Yboard.get_button(1) && !Yboard.get_button(2)){

    delay(15);

  }

  Yboard.set_all_leds_color(0, 0, 0);

}


//this is an unused shell for an empty micro game that can be coppied to make a new game. You can remove the time requirements if you want.
//all games should have a return type of bool and a return of both true for wining and false for losing.
//there can be more than one return statememt of each type.
bool example_game(){

  float time = get_time(5); //the number in get time is the maximum seconds allowed at score 0, not all games need to use time;

  //here you can declare variables and run setup for the game



  while(true){

    delay(20);  //delays the loop for 20ms
    time -= 30; //removes 30ms from time. The extra 10 is to account for computation time, though that is only an estimate.

    //here is where you put the game loop



    if(time <= 0){

      return false; //return false if they lose the game or return true if they win.

    }

  }

}


//this is the shake it game where you need to shake the board to score a point.
bool shake_it(){
  //first you get the time available, starting with a max of 4 seconds. You can do 4.5 or 1/3 or anything like that. The input is is seconds.
  float time = get_time(4);

  //this gets the accelerometer and saves it to a variable.
  accelerometer_data accel = Yboard.get_accelerometer();

  //this creates a variable that tracks the number of times the acceleration has flipped
  //from positive to negative or negative to positive on any axis
  int axis_flips = 0;

  //this saves the previous data of the accelerometer so it can be compared to see if a flip occured.
  accelerometer_data prev_data = accel;

  //this will loop until it encounters either break; or in this case return true;/return false;
  while(true){

    //updates the previous accelerometer data. In the first loop they will be equal to each other.
    prev_data = accel;

    //delays for 20ms to prevent it from running too fast.
    delay(20);
    //removes 30ms from the remaining time. The extra 10ms is to account for the computation time.
    //this is not very accurate and there are smarter ways to do this but for sake of simplicity this is how i'm doing it for now.
    time -= 30;

    //this resets the display to black.
    display.clearDisplay();
    //draws the time in the top left. You can also do draw_time(time, 10, 20); to offset the draw 10 pixels right and 20 down.
    draw_time(time);

    //updates the accelerometer
    accel = Yboard.get_accelerometer();


    //draws the text for "Shake it!" on the screen.
    //set cursor sets the next text drawing location
    display.setCursor(0,10);
    //set text size sets the font size
    display.setTextSize(3);
    //and print draws the text at the cursor location
    display.print("Shake");
    display.setCursor(0, 40);
    display.print("it!");

    //each of these if statements is checking if an axis flipped and adding it to the total.
    if((accel.x > 0 && prev_data.x <= 0) || (accel.x < 0 && prev_data.x >= 0)){
      axis_flips++;
    }
    if((accel.y > 0 && prev_data.y <= 0) || (accel.y < 0 && prev_data.y >= 0)){
      axis_flips++;
    }
    if((accel.z > 0 && prev_data.z <= 0) || (accel.z < 0 && prev_data.z >= 0)){
      axis_flips++;
    }

    //if there are greater than 8 flips in the time allowed they succeed.
    //there are more accurate ways to measure this. If someone holds it really steady and close to level they will
    //usually get it really quickly but shaking it hard works fine, so for now this is an adequate solution.
    if(axis_flips > 8){

      //return true tells the main loop they won the game and to run the success function.
      return true;
    }

    //this checks if they have run out of time.
    if(time <= 0){

      //returning false tells the main loop that they failed the game and to restart.
      return false;
    }

    //finally this draws everything to the screen.
    display.display();

  };
}


//this is simon says. Press the left and right buttons in the right order to 
bool simon_says(){
  //creates a blank boolean array with 10 entries to store the directions the user needs to input
  bool dir[10];

  //sets the length of to either 10 or the square root of the score plus 2, whichever is shorter
  int length = min(static_cast<int>(sqrt(score/2)) + 3, 10);

  //draws the title on the screen
  display.clearDisplay();
  display.setCursor(0,10);
  display.setTextSize(3);
  display.print("Simon");
  display.setCursor(0, 40);
  display.print("Says!");
  display.display();

  //resets all the leds to black
  Yboard.set_all_leds_color(0, 0, 0);

  //this will loop 15 times if length is 15 or 5 times if length is 5
  for(int i = 0; i < length; i++){

    //this sets a random value to every spot in the array up to the length limit
    dir[i] = rand()%2;

    //this displays to the user what they need to remember.
    //it will show right first then left
    //see if you can figure out how it works. (if you want to that is)
    //by the way the % operator is modulus which is remainder devision.
    if(dir[i]){

      for(int q = 7; q <= 17; q++){

        Yboard.set_led_color(q, 255 * dir[i], 255 * dir[i], 255 * dir[i]);

      }

    }else{

      for(int q = 17; q <= 27; q++){

        Yboard.set_led_color(q % 20, 255, 255, 255);

      }

    }

    //waits for a while so you can see what lights turned on. This is dependant on score.
    delay(static_cast<int>(16000 / (20 + score)));

    //sets black and waits half as long.
    Yboard.set_all_leds_color(0, 0, 0);
    delay(static_cast<int>(8000 / (20 + score)));

  }
    //loops again, this time looking for user input.
  for(int i = 0; i < length; i++){

    //sets the color of the next led so you can tell how far in the sequence you are
    Yboard.set_led_color(i, 255, 255, 255);

    //loops until you press a button
    while(true){
      
      //if you press the first button
      if(Yboard.get_button(1)){

        //wait until they release the button so you don't get a repeat button press
        while(Yboard.get_button(1)){

          delay(5);

        }

        //when it was the right button then continue, if it was the wrong one, game over.
        if(!dir[i]){
          break;
        }else{
          return false;
        }

      }

      //same as button 1
      if(Yboard.get_button(2)){

        while(Yboard.get_button(2)){

          delay(10);

        }

        if(dir[i]){
          break;
        }else{
          return false;
        }

      }

    }

  }

  //resets leds again.
  Yboard.set_all_leds_color(0, 0, 0);

  //if they made it to the end it was a success.
  return true;

}


//this game requires you to his a button at the right time.
bool time_it(){

  //this prevents the game starting while a button is pressed so you can't lose from holding a button from a previous game
  while(Yboard.get_button(0) || Yboard.get_button(1)){

    delay(10);

  }

  //the ammount of time to delay before changing the light.
  int d = static_cast<int>(get_time(0.15));

  //draws the title on the screen
  display.clearDisplay();
  display.setCursor(0,10);
  display.setTextSize(3);
  display.print("Time");
  display.setCursor(0, 40);
  display.print("it!");
  display.display();
  
  //keeps track of which light is currently on
  int light = 0;

  //resets the leds
  Yboard.set_all_leds_color(0, 0, 0);

  //loops until the game ends
  while(true){

    //sets the bottom light green
    Yboard.set_led_color(7, 0, 255, 0);

    //sets the bottom light white.
    Yboard.set_led_color(light, 255, 255, 255);

    //if either button is pressed
    if(Yboard.get_button(1) || Yboard.get_button(2)){
      
      //check if the current light is within one light of the bottom one
      if(light >= 6 && light <= 8){

        return true;  //you win if it is

      }else{

        return false; //you lose if it isn't

      }

    }

    delay(d);

    //turn off the light
    Yboard.set_led_color(light % 20, 0, 0, 0);

    //move to the next light
    light++;

    //thsis does the same thing as if light is greater than 20 then subtract 20 in a loop, but far more efficiently
    light = light % 20;

  }

}


bool line_it_up(){

  float time = get_time(8);

  int target = random()%100;
  int pos = 0;
  
  int dur = 0;

  while(true){

    pos = Yboard.get_knob();

    display.clearDisplay();

    display.drawCircle(64, 32, 16, ON);

    display.drawCircle(64 + 16*cos(PI*static_cast<double>(target + 15)/65), 
                       32 + 16*sin(PI*static_cast<double>(target + 15)/65),
                       8, ON);

    display.fillCircle(64 + 16*cos(PI*static_cast<double>(pos + 15)/65), 
                       32 + 16*sin(PI*static_cast<double>(pos + 15)/65),
                       4, ON);

    if(pos < target + 5 && pos > target - 5){

      dur++;
      if(dur >= 30){

        return true;

      }

    }else{

      dur = 0;

    }

    display.setCursor(0,15);
    display.print("Line it up!");
    draw_time(time);
    display.display();

    delay(20);  //delays the loop for 20ms
    time -= 30; //removes 30ms from time. The extra 10 is to account for computation time, though that is only an estimate.

    //here is where you put the game loop



    if(time <= 0){

      return false; //return false if they lose the game or return true if they win.

    }

  }

}


//amy's stuff - still messing around with it
#include <cstdlib>
#include <algorithm>

extern int score;

bool guess_the_button() {
  srand(static_cast<unsigned int>(time(nullptr)));

  while (Yboard.get_button(1) || Yboard.get_button(2)) {
    delay(10);
  }

  display.clearDisplay();
  display.setCursor(0, 10);
  display.setTextSize(2);
  display.print("Guess the");
  display.setCursor(0, 40);
  display.print("Button!");
  display.display();

  Yboard.set_all_leds_color(0, 0, 0);
  int score = 0;
  
  int timeLimit = get_time(3);

  while (true) {

  bool correctButton = rand() % 2;

  int timeRemaining = timeLimit;

  bool guessed_correctly = false;
    
  while (timeRemaining > 0) {
    delay(20);
    timeRemaining -= 20;

    if (Yboard.get_button(1)) {
      while (Yboard.get_button(1)) delay (10);
      if (!correctButton) {
        guessed_correctly = true;
      }
      break;
    }
    
    if (Yboard.get_button(2)) {
      while (Yboard.get_button(2)) delay(10);
      if (correctButton) {
        guessed_correctly = true;
      }
      break;
    }
  }

  if (guessed_correctly) {
    return true;
  }else{
    return false;
  }
    
  score++;
  timeLimit = std::max(500, static_cast<int>(timeLimit * 0.9));
    
  }

}


//this is the main game loop that handles everything outside a micro game.
void loop() {
  
  //a switch statement is like a bunch of if else statements strung together.
  //it will take the input which in this case is rand() % num_games which is a random number from 0 to num_games - 1.
  //to add another just plop another case in just like the ones below.
  switch(rand() % (num_games)){
    
    //a case is one of the situations where you want something to happen
    //in this case if the random number is 0
    case 0:
      //inside a case you put the code you want to run.
      //in all our cases we want to run our game and save the result to game_won
      game_won = shake_it();

      //we need to break our cases or they will fall through and run the code in the next case as well.
      //this behavior can sometimes be useful but for this we always want to break
      break;

    case 1:
      game_won = simon_says();
      break;

    case 2:
      game_won = time_it();
      break;

    case 3:
      game_won = guess_the_button();
      break;

    case 4:
      game_won = line_it_up();
      break;

    //this is run if none of the other cases match
    default:
      game_won = shake_it();
      break;

  }

  //if the last micro game was won
  if(game_won){

    //tell the player they did it successfully
    success();

  }else{

    //if they lost then reset the game and start over.
    game_over();

  }
  
}

