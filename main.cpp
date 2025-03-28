#include <LCDWIKI_GUI.h> //Core graphics library
#include <LCDWIKI_SPI.h> //Hardware-specific library
#include <ft6336g.h> //touch library
//paramters define
#define MODEL ILI9341
#define CS   10    
#define CD   9
#define RST  8
#define MOSI  11
#define MISO  12
#define SCK   13
#define LED  5   //if you don't need to control the LED pin,you should set it to -1 and set it to 3.3V
//touch screen paramters define
#define OTT_MAX_TOUCH  2 
#define INT  7
#define CRST 6 
#define SCL  A5
#define SDA  A4
#define BLACK   0xFFFF
#define BLUE    0xFFE0
#define RED     0x07FF
#define GREEN   0xF81F
#define CYAN    0xF800
#define MAGENTA 0x07E0
#define YELLOW  0x001F
#define WHITE   0x0000
//the definiens of software spi mode as follow:
//if the IC model is known or the modules is unreadable,you can use this constructed function
LCDWIKI_SPI my_lcd(MODEL,CS,CD,MISO,MOSI,RST,SCK,LED); //model,cs,dc,miso,mosi,reset,sck,led

//the definiens of touch mode as follow:
FT6336 my_tp(INT,CRST,SCL,SDA);
bool alarm_active = false;
int input_code[4] = {0, 0, 0, 0};  // Stores entered code
int index = 0;                     // Tracks how many digits have been entered
const int correct_code[4] = {2, 2, 1, 1};  // Correct passcode


bool alarm_deactivated = false;  // New flag to stop cycling once alarm is off

void AlarmON() {
  pinMode(2, OUTPUT);
  pinMode(3, INPUT_PULLUP);
  int val = digitalRead(3);

  if (alarm_active && val == HIGH && memcmp(input_code, correct_code, sizeof(correct_code)) == 0) {
    digitalWrite(2, LOW);         // Turn off the buzzer/LED
    alarm_active = false;         // Stop the alarm
    alarm_deactivated = true;     // Set flag to prevent future checks
     my_lcd.Fill_Screen(WHITE);
    show_string("Alarm OFF", my_lcd.Get_Display_Width() - 210, 140, 3, BLACK, WHITE, 1);
    delay(2000);
    my_lcd.Fill_Screen(BLACK);
    digitalWrite(LED, LOW);
  } else if (!alarm_deactivated && val == HIGH) {  // Alarm only activates if not deactivated
    alarm_active = true;
    digitalWrite(2, HIGH);
  }
}

void show_string(uint8_t *str,int16_t x,int16_t y,uint8_t csize,uint16_t fc, uint16_t bc,boolean mode)
{
    my_lcd.Set_Text_Mode(mode);
    my_lcd.Set_Text_Size(csize);
    my_lcd.Set_Text_colour(fc);
    my_lcd.Set_Text_Back_colour(bc);
    my_lcd.Print_String(str,x,y);
}
// Rectangle bounds (x1, y1, x2, y2)
struct Rect {
  int x1, y1, x2, y2, num;  // Rect coordinates and number
};
Rect rectangles[] = {
  {100, 70, 140, 110, 2}, {40, 140, 80, 180, 4}, {40, 70, 80, 110, 1},
  {160, 70, 200, 110, 3}, {160, 140, 200, 180, 6}, {100, 140, 140, 180, 5},
  {40, 210, 80, 250, 7}, {100, 210, 140, 250, 8}, {160, 210, 200, 250, 9},
  {100, 270, 140, 310, 0}
};

void draw_circle_at_index(int idx) {
  my_lcd.Draw_Circle((60 + idx * 50) - 15, 20 + 15+10, 10);  // Shift left by 15 and down by 15
}


void clear_input() {
  my_lcd.Fill_Rectangle(40, 0, 250, 40);  // Clear feedback area
  index = 0;  // Reset input index
}

void show_message(const char *message) {
  my_lcd.Fill_Screen(WHITE);  // Clear the screen
  show_string((uint8_t *)message, 40, 140, 5, BLACK, WHITE, 1);
  delay(2000);                // Display message for 2 seconds
  my_lcd.Fill_Screen(WHITE);  // Clear the screen again after the message
}

void check_input() {
  if (memcmp(input_code, correct_code, sizeof(correct_code)) == 0) {
    AlarmON();
  } else {
    my_lcd.Fill_Screen(WHITE);
    show_string("Wrong Pin",my_lcd.Get_Display_Width()-210,140,3,BLACK, WHITE,1);
  }
  clear_input();  // Reset input after checking
  setup();
}  

void setup(void) 
{
  pinMode(2, OUTPUT);            // Set pin 2 as output (for buzzer or LED)
  pinMode(3, INPUT_PULLUP);
  digitalWrite(2,LOW);
  while (digitalRead(2) == LOW) {
    AlarmON();
  }
  if (digitalRead(2) == HIGH){
  Serial.begin(9600);
  my_lcd.Set_Rotation(0);  
  my_lcd.Init_LCD();
  my_lcd.Fill_Screen(WHITE);
  Serial.println(my_lcd.Read_ID(), HEX);
  show_string("WARDEN",my_lcd.Get_Display_Width()-210,0,5,BLACK, WHITE,1);
  my_lcd.Set_Draw_color(BLACK);
  my_lcd.Print_Number_Int(1,50,70,1,1,10);
  my_lcd.Print_Number_Int(2,110,70,1,1,10);
  my_lcd.Print_Number_Int(3,170,70,1,1,10);
  my_lcd.Print_Number_Int(4,50,140,1,1,10);
  my_lcd.Print_Number_Int(5,110,140,1,1,10);
  my_lcd.Print_Number_Int(6,170,140,1,1,10);
  my_lcd.Print_Number_Int(7,50,210,1,1,10);
  my_lcd.Print_Number_Int(8,110,210,1,1,10);
  my_lcd.Print_Number_Int(9,170,210,1,1,10);
  my_lcd.Print_Number_Int(0,110,270,1,1,10);
  my_lcd.Draw_Rectangle(100, 140, 140, 180);
  my_lcd.Draw_Rectangle(100, 70, 140, 110);
  my_lcd.Draw_Rectangle(40, 70, 80, 110);
  my_lcd.Draw_Rectangle(160, 70, 200, 110);
  my_lcd.Draw_Rectangle(40, 140, 80, 180);
  my_lcd.Draw_Rectangle(160, 140, 200, 180);
  my_lcd.Draw_Rectangle(40, 210, 80, 250);
  my_lcd.Draw_Rectangle(100, 210, 140, 250);
  my_lcd.Draw_Rectangle(160, 210, 200, 250);
  my_lcd.Draw_Rectangle(100, 270, 140, 310);
    if(my_tp.FT6336_Init(my_lcd.Get_Rotation(),my_lcd.Get_Display_Width(),my_lcd.Get_Display_Height()))
  {
    return;  
  }
  }
}

void loop() {
  
uint16_t lastpos[2] = {0xFFFF, 0xFFFF};  // Initialize with invalid touch positions
  while (1) {
    my_tp.FT6336_Scan();  // Scan for touch input
       // Check if a touch is detected
    if (my_tp.ctp_status & (1 << 0)) {
      // Get the touch coordinates
      int x = my_tp.x[0];
      int y = my_tp.y[0];

      // Check if the coordinates are within the display bounds
       if (x < my_lcd.Get_Display_Width() && y < my_lcd.Get_Display_Height()) {
      for (int i = 0; i < 10; i++) {
        if (x >= rectangles[i].x1 && x <= rectangles[i].x2 && y >= rectangles[i].y1 && y <= rectangles[i].y2) {
          input_code[index] = rectangles[i].num;  // Store the digit based on the touched rectangle
          draw_circle_at_index(index);            // Draw a circle to indicate input
          index++;
          if (index == 4) {
            check_input();  // Check input once 4 digits are entered
          }
          break;
        }
      }
    }
    }
    AlarmON();
    delay(1000);  // Small delay to prevent spamming the touchscreen scan
  }
}
