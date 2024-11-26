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
*/

static int display_height = 63;
static int display_width = 127;

int num_games = 2;

int score = 0;

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
  display.setCursor(0, 15);
  display.print("Great");

  display.setCursor(0, 30);
  display.print("Job!");
  display.display();

  //checks if the second switch is enabled.
  if(Yboard.get_switch(2)){

    //if it is then play a success sound.
    Yboard.play_notes("O5 A8 D8");

  }
  
  delay(40);

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

  delay(6000);

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
    display.setCursor(0,20);
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
  //creates a blank boolean array with 20 entries to store the directions the user needs to input
  bool dir[20];

  //sets the length of to either 15 or the square root of the score plus 2, whichever is shorter
  int length = min(static_cast<int>(sqrt(score/1.5)) + 3, 15);

  bool no_button = true;

  
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

  //if they made it to the end it was a success.
  return true;

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

