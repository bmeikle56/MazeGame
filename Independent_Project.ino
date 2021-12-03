#include <Keypad.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
char hexaKeys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {9, 8, 7, 6}; //connect to the row pinouts of the keyboard
byte colPins[COLS] = {A0, A1, A2, A3}; //connect to the column pinouts of the keyboard
Keypad customKeyboard = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 


const int buttonPin = A5;
const int ledPin = 13;
const int piezoPin = 10;
int buttonRead;

long randomCol;
long randomRow;
int currentRow;
int currentCol;

int timer;

char grid[2][16]; // 2 by 16 board for the game
static int location[2]; // used for updating the location of the *, [0] = row, [1] = column

void setup(){
  
  Serial.begin(9600);
  
  pinMode(ledPin, OUTPUT);
  pinMode(piezoPin, OUTPUT);

  lcd.begin(16, 2);
  
  lcd.setCursor(2, 0);
  lcd.print("Press '*' to");
  lcd.setCursor(1, 1);
  lcd.print("begin the game");

  timer = 0;
}








void loop()
{
  char keyPressed = customKeyboard.getKey();
  
  if (timer == 0 && keyPressed == '*')
  {
    newGame();
    timer++;
  }
  
  if (keyPressed)
  {
    // what did the key do
    switch (keyPressed)
    {
      case '5':
        Serial.println("5 was pressed, new game");
        newGame();
      break;
      case '2':
        Serial.println("2 was pressed, move up");
        movePlayer(2);
      break;
      case '6':
        Serial.println("6 was pressed, move right");
        movePlayer(6);
      break;
      case '4':
        Serial.println("4 was pressed, move left");
        movePlayer(4);
      break;
      case '8':
        Serial.println("8 was pressed, move down");
        movePlayer(8);
      break;
      default: // do nothing
        Serial.println("Press 2, 4, 6, 8 to move or 5 to restart game");
      break;
    }
  }
  
  buttonRead = analogRead(buttonPin);
  
  if (buttonRead > 920) // if the button is pressed to check to see if the player is at correct location
  {
    delay(300);
    
    if (correct())
    {
      tone(piezoPin, 1000, 100); // "win" sound
      
      Serial.print("You won! The winning square was at row: ");
      Serial.print(currentRow);
      Serial.print(", column: ");
      Serial.println(currentCol);

      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print("Press 5 to");
      lcd.setCursor(4, 1);
      lcd.print("play again");
      if (keyPressed && keyPressed == '5')
      {
        Serial.println("5 was pressed, new game");
        newGame();
      }
    }
    else
    {
      Serial.print("Wrong! It is not at row: ");
      Serial.print(currentRow);
      Serial.print(", column: ");
      Serial.println(currentCol);
      
      digitalWrite(ledPin, HIGH);
      tone(piezoPin, 350, 100); // "lose" sound
      delay(100);
      digitalWrite(ledPin, LOW);
      getLocation();
      lcd.setCursor(location[1], location[0]);
      lcd.print("X");
    }
  }
}





void getLocation() // updates location of the *
{
  for (int i = 0; i < 16; i++) // col
  {
    for (int j = 0; j < 2 ; j++) // row
    {
      if (grid[j][i] == '*')
      {
        // store in global 1D array of {row, column}
        location[1] = i;
        location[0] = j;
      }
    }
  }
  return;
}





boolean correct() // checks to see if the * is at the correct spot
{
  getLocation(); // update the location
  currentRow = location[0];
  currentCol = location[1];
  
  // if the player is at the right location, they win
  if (currentCol == randomCol && currentRow == randomRow)
    return true;
  return false;
}





void resetLocation() // sets the user back to (0, 0)
{
  location[0] = 0;
  location[1] = 0;
}




void newGame() // begins new game
{
  Serial.println("Game started");
  
  resetLocation();

  // update the lcd
  lcd.clear();
  lcd.setCursor(currentCol, currentRow);
  lcd.print("*");

  // update the grid
  grid[0][0] = '*';
  
  randomRow = random(2); // random number 0 to 1
  randomCol = random(16); // random number 0 to 15
  while (randomRow == 0 && randomCol == 0)
  {
    // while the random numbers aren't both 0
    randomRow = random(2);
    randomCol = random(16);
  }
  // not random for whatever reason
  Serial.print("Winning row: ");
  Serial.println(randomRow);
  Serial.print("Winning column: ");
  Serial.println(randomCol);
}










void movePlayer(int direct) // move the player to a location adjacent to it up down left right
{
  getLocation();
  currentRow = location[0];
  currentCol = location[1];

  // clear the asterisk from grid
  grid[currentRow][currentCol] = ' ';

  // now the lcd
  lcd.setCursor(currentCol, currentRow);
  lcd.print(" ");

  // which way does the player want to move?
  switch (direct)
  {
    case 2: // move up
      currentRow--;
    break;
    case 8: // move down
      currentRow++;
    break;
    case 4: // move left
      currentCol--;
    break;
    case 6: // move right
      currentCol++;
    break;
    default: // do nothing
    break;
  }

  // update grid
  grid[currentRow][currentCol] = '*';

  // update lcd
  lcd.setCursor(currentCol, currentRow);
  lcd.print("*");
}
