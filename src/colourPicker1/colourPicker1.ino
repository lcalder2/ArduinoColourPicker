#define SERIAL_BAUD 115200
#include <stdlib.h>
#include <ArduinoJson.h>


#define COLOUROUT 46
#define COLOURS2 44
#define COLOURS3 45


#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;
#include <TouchScreen.h>
#define MINPRESSURE 200
#define MAXPRESSURE 1000

// screen calibration
const int XP = 8, XM = A2, YP = A3, YM = 9; //ID=0x9341
const int TS_LEFT = 924, TS_RT = 113, TS_TOP = 74, TS_BOT = 899;

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

Adafruit_GFX_Button picker_btn, clear_btn, adj_btn, brighten_btn, darken_btn, cool_btn, warm_btn, back_btn;
const int palette_size = 6;
const int rgb = 3;
int screen_width = 240;
int screen_height = 320;

// Colour class / storage

class Colour {
  
  private:
  uint8_t r = 255;
  uint8_t g = 255;
  uint8_t b = 255;
  bool adj = false;
  int well_pos = -1 ;
  bool full = false;
  
  public:
  void set_full(bool value);
  bool get_full();
  void set_adj(bool value);
  void set_well_pos(int value);
  void set_r(uint8_t value);
  void set_g(uint8_t value);
  void set_b(uint8_t value);
  void add(uint8_t value[3]);
  uint8_t get_r();
  uint8_t get_g();
  uint8_t get_b();
  uint8_t get_complement_r();
  uint8_t get_complement_g();
  uint8_t get_complement_b();
};

bool Colour::get_full(){
  return this->full;
}

void Colour::add(uint8_t value[3]){
  this->r += value[0]/2;
  this->g += value[1]/2;
  this->b += value[2]/2;
}

void Colour::set_full(bool value){
  this->full = value;
}

void Colour::set_adj(bool value){
  this->adj = value;
}

void Colour::set_well_pos(int value){
  this->well_pos = value;
}

void Colour::set_r(uint8_t value){
  this->r = value;
}

void Colour::set_g(uint8_t value){
  this->g = value;
}

void Colour::set_b(uint8_t value){
  this->b = value;
}

uint8_t Colour::get_r(){
  return this->r;
}

uint8_t Colour::get_g(){
  return this->g;
}

uint8_t Colour::get_b(){
  return this->b;
}

uint8_t Colour::get_complement_r(){
  uint8_t max_colour = 255;
  uint8_t r_c = max_colour - this->r;
  return r_c;
}

uint8_t Colour::get_complement_g(){
  uint8_t max_colour = 255;
  uint8_t g_c = max_colour - this->g;
  return g_c;
}

uint8_t Colour::get_complement_b(){
  uint8_t max_colour = 255;
  uint8_t b_c = max_colour - this->b;
  return b_c;
}

Colour palette[palette_size];
Colour blend_colour;
Adafruit_GFX_Button palette_buttons[palette_size];
Adafruit_GFX_Button blend_button;
float angles = 360.0 / palette_size; 
bool adjust = false;

// touchscreen setup

int pixel_x, pixel_y;     //Touch_getXY() updates global vars
bool Touch_getXY(void)
{
    TSPoint p = ts.getPoint();
    pinMode(YP, OUTPUT);      //restore shared pins
    pinMode(XM, OUTPUT);
    digitalWrite(YP, HIGH);   //because TFT control pins
    digitalWrite(XM, HIGH);
    bool pressed = (p.z > MINPRESSURE && p.z < MAXPRESSURE);
    if (pressed) {
        pixel_x = map(p.x, TS_LEFT, TS_RT, 0, tft.width()); //.kbv makes sense to me
        pixel_y = map(p.y, TS_TOP, TS_BOT, 0, tft.height());
    }
    return pressed;
}

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

int selected_well = 1;
int action = 0; /** Actions: Choose well:  0
                             Clear Well:   1
                             adj Colour:   2
                             Remove Well:  3
                **/

// end touchscreen


/*******************************************
	In this code we have used "[]" to surround our command codes
	As a bit of a proof of concept for how to use the XC4411 board
*********************************************/
const int led_pin = 13;

String receivedCommand = "";
bool dataIn = false;

// function to swap the the position of two elements
void swap(Colour *a, Colour *b) {
  Colour temp = *a;
  *a = *b;
  *b = temp;
}

bool arrangeColours(){
  
  for (int step = 0; step < palette_size - 1; step++) {
    int min_idx = step;
    for (int i = step + 1; i < palette_size; i++) {
      if (palette[i].get_r() < palette[min_idx].get_r()){
        min_idx = i;
      }
    }
    // put min at the correct position
    swap(&palette[min_idx], &palette[step]);
  }
  
  return true;
}

void setup()
{
	// put your setup code here, to run once:

	Serial.begin(SERIAL_BAUD); //same as ESP baud
	pinMode(led_pin, OUTPUT);

 // Colour sensor
 
 pinMode(COLOUROUT,INPUT);
 pinMode(COLOURS2,OUTPUT);
 pinMode(COLOURS3,OUTPUT);
 
 //End colour sensor

 
 //Touchscreen

  uint16_t ID = tft.readID();
  Serial.print("Booted");
  if (ID == 0xD3D3) ID = 0x9486; // write-only shield
  tft.begin(ID);
  tft.setRotation(0);            //PORTRAIT
  tft.fillScreen(WHITE);

// Draw starting buttons
  
  // Draw a circle of buttons
  
  for (int i = 0; i < palette_size; i++){
    drawButton(i);
  }
  
  picker_btn.initButton(&tft, (screen_width / 2), 20, screen_width, 50, BLACK, WHITE, RED, "", 100); 
  picker_btn.drawButton(true);
  clear_btn.initButton(&tft, 190, 290, 80, 40, BLACK, BLACK, WHITE, "Clear", 1); 
  clear_btn.drawButton(true);
  adj_btn.initButton(&tft, 50, 290, 80, 40, BLACK, BLACK , WHITE, "Adjust", 1); 
  adj_btn.drawButton(true);
  
  blend_button.initButton(&tft, (screen_width / 2), (screen_height / 2), 50, 50,  BLACK, WHITE, WHITE, "", 100); 
  blend_button.drawButton(true);
  //End touchscreen



} 

void loop()
{
  // Colour sensor
  
  colourred();
  uint8_t r = getintensity();
  r = (255 * r) / (85-8);

  colourgreen();
  uint8_t g = getintensity();
  g = (255 * g) / (65-8);

  colourblue();
  uint8_t b = getintensity();
  b = (255 * b) / (120-22);
                        
  delay(200);

  if (not adjust){
    uint16_t hex = convertcolour(r, g, b);   
    picker_btn.initButton(&tft, (screen_width / 2), 20, screen_width, 50, BLACK, hex, hex, "", 100);
    picker_btn.drawButton();
  }
  
  
  //end colour sensor

	delay(10);

  //Touchscreen
  bool down = Touch_getXY();
    picker_btn.press(down && picker_btn.contains(pixel_x, pixel_y)); 
    clear_btn.press(down && clear_btn.contains(pixel_x, pixel_y)); 
    adj_btn.press(down && adj_btn.contains(pixel_x, pixel_y)); 
    blend_button.press(down && blend_button.contains(pixel_x, pixel_y)); 
    back_btn.press(down && back_btn.contains(pixel_x, pixel_y));
    brighten_btn.press(down && brighten_btn.contains(pixel_x, pixel_y)); 
    darken_btn.press(down && darken_btn.contains(pixel_x, pixel_y));  
    warm_btn.press(down && warm_btn.contains(pixel_x, pixel_y));
    cool_btn.press(down && cool_btn.contains(pixel_x, pixel_y));  

    // Blend 
    if (blend_button.justPressed()){
        uint8_t i_colour[rgb];
        i_colour[0] = palette[selected_well].get_r();
        i_colour[1] = palette[selected_well].get_g();
        i_colour[2] = palette[selected_well].get_b();
        blend_colour.add(i_colour);
        uint8_t bcolour[rgb];
        bcolour[0] = blend_colour.get_r();
        bcolour[1] = blend_colour.get_g();
        bcolour[2] = blend_colour.get_b();
        uint16_t well_colour = convertcolour(bcolour[0], bcolour[1], bcolour[2]);
        blend_button.initButton(&tft, (screen_width / 2), (screen_height / 2), 50, 50,  BLACK, well_colour, well_colour, "", 100); 
        blend_button.drawButton(true);
        
        StaticJsonBuffer<200> jsonBuffer;

        JsonObject& root = jsonBuffer.createObject();
        root["r"] = blend_colour.get_r();
        root["g"] = blend_colour.get_g();
        root["b"] = blend_colour.get_b();
              
        root.printTo(Serial);
        Serial.println();
      }

    if (adj_btn.justPressed()){
      adjust = true;
      tft.fillRect(0,0,screen_width,screen_height,WHITE);
      back_btn.initButton(&tft, (screen_width / 2), 20, screen_width, 50, BLACK, BLACK, WHITE, "Back", 1); 
      back_btn.drawButton(true);
      darken_btn.initButton(&tft, 70, 100, 80, 40, BLACK, BLACK, WHITE, "Darken", 1); 
      darken_btn.drawButton(true);
      brighten_btn.initButton(&tft, 180, 100, 80, 40, BLACK, BLACK, WHITE, "Brighten", 1); 
      brighten_btn.drawButton(true);
      warm_btn.initButton(&tft, 70, 150, 80, 40, BLACK, BLACK , WHITE, "Warmer", 1); 
      warm_btn.drawButton(true);
      cool_btn.initButton(&tft, 180, 150, 80, 40, BLACK, BLACK , WHITE, "Cooler", 1); 
      cool_btn.drawButton(true);
      for (int i = 0; i < palette_size; i++){
         drawSquares(i);
      }
    }

    if (back_btn.justPressed() and adjust){
      adjust = false;
      cool_btn.drawButton(false);
      warm_btn.drawButton(false);
      darken_btn.drawButton(false);
      brighten_btn.drawButton(false);
      tft.fillRect(0,0,screen_width,screen_height,WHITE);
      for (int i = 0; i < palette_size; i++){
          drawButton(i);
      }
      clear_btn.initButton(&tft, 190, 290, 80, 40, BLACK, BLACK, WHITE, "Clear", 1); 
      clear_btn.drawButton(true);
      adj_btn.initButton(&tft, 50, 290, 80, 40, BLACK, BLACK , WHITE, "Adjust", 1); 
      adj_btn.drawButton(true);
      blend_button.initButton(&tft, (screen_width / 2), (screen_height / 2), 50, 50,  BLACK, WHITE, WHITE, "", 100); 
      blend_button.drawButton(true);
    }

    if (brighten_btn.justPressed() and adjust){
      for (int i = 0; i < palette_size; i++){
          if (palette[i].get_full()){
            if (palette[i].get_r() < 235){
              palette[i].set_r(palette[i].get_r() + 25);
            }
            if (palette[i].get_g() < 235){
              palette[i].set_g(palette[i].get_g() + 25);
            }
            if (palette[i].get_b() < 235){
              palette[i].set_b(palette[i].get_b() + 25);
            }
          }
      }

      for (int i = 0; i < palette_size; i++){
         drawSquares(i);
      }
    }

    if (darken_btn.justPressed() and adjust){
      for (int i = 0; i < palette_size; i++){
          if (palette[i].get_full()){
            if (palette[i].get_r() >25){
              palette[i].set_r(palette[i].get_r() - 25);
            }
            if (palette[i].get_g() >25){
              palette[i].set_g(palette[i].get_g() - 25);
            }
            if (palette[i].get_b() >25){
              palette[i].set_b(palette[i].get_b() - 25);
            }
          }
      }

      for (int i = 0; i < palette_size; i++){
         drawSquares(i);
      }
    }

    if (cool_btn.justPressed() and adjust){
      for (int i = 0; i < palette_size; i++){
          if (palette[i].get_full()){
            if (palette[i].get_b() < 240){
              palette[i].set_b(palette[i].get_b() + 15);
            }
            if (palette[i].get_r() > 15){
              palette[i].set_r(palette[i].get_r() - 15);
            }
            if (palette[i].get_g() < 240){
              palette[i].set_g(palette[i].get_g() + 15);
            }
          }
      }
      for (int i = 0; i < palette_size; i++){
         drawSquares(i);
      }
    }

    if (warm_btn.justPressed() and adjust){
      for (int i = 0; i < palette_size; i++){
          if (palette[i].get_full()){
            if (palette[i].get_r() < 240){
              palette[i].set_r(palette[i].get_r() + 15);
            }
            if (palette[i].get_b() > 15){
              palette[i].set_b(palette[i].get_b() - 15);
            }
            if (palette[i].get_g() >15){
              palette[i].set_g(palette[i].get_g() - 15);
            }
          }
      }
      for (int i = 0; i < palette_size; i++){
         drawSquares(i);
      }
    }
    
    // Select well
    for (int i = 0; i < palette_size; i++){
      palette_buttons[i].press(down && palette_buttons[i].contains(pixel_x, pixel_y));
    }
    
    for (int i = 0; i < palette_size; i++){
      if (palette_buttons[i].justPressed() and not adjust){
        if (action == 0){
            if (palette[i].get_full()){
              selected_well = i;
              // Well selected, send data to serial port as JSON object
              StaticJsonBuffer<200> jsonBuffer;

              JsonObject& root = jsonBuffer.createObject();
              root["r"] = palette[i].get_r();
              root["g"] = palette[i].get_g();
              root["b"] = palette[i].get_b();
              
              root.printTo(Serial);
              Serial.println();
              
              tft.fillRect(0,0,screen_width,260,WHITE);
              int rect_x = 75* cos(radians(angles)*selected_well) + screen_width / 2;
              int rect_y = 75* sin(radians(angles)*selected_well) + screen_height / 2;
              tft.fillRoundRect(rect_x -27, rect_y-27, 54, 54,10, RED);
              arrangeColours();
              
              for (int i = 0; i < palette_size; i++){
                drawButton(i);
              }
              
              uint8_t i_colour[rgb];
              i_colour[0] = blend_colour.get_r();
              i_colour[1] = blend_colour.get_g();
              i_colour[2] = blend_colour.get_b();
              uint16_t well_colour = convertcolour(i_colour[0], i_colour[1], i_colour[2]);
              blend_button.initButton(&tft, (screen_width / 2), (screen_height / 2), 50, 50,  BLACK, well_colour, well_colour, "", 100); 
              blend_button.drawButton(true);
              
            }
            else{
              int opposite = (palette_size/ 2) + i;
              
              if (opposite > palette_size){
                opposite = opposite % palette_size;
              }
              if (palette[opposite].get_full()){
                palette[i].set_r(255 -palette[opposite].get_r());
                palette[i].set_g(255 -palette[opposite].get_g());
                palette[i].set_b(255-palette[opposite].get_b());
                palette[i].set_full(true);
                for (int i = 0; i < palette_size; i++){
                  drawButton(i);
                }
              }
            }
          }
        
        if (action == 1){
          palette[i].set_full(false);
          uint8_t white = 255;
          palette[i].set_r(white);
          palette[i].set_g(white);
          palette[i].set_b(white);
          tft.fillRect(0,0,screen_width,270,WHITE);
          for (int i = 0; i < palette_size; i++){
            drawButton(i);
          }
          blend_button.initButton(&tft, (screen_width / 2), (screen_height / 2), 50, 50,  BLACK, WHITE, WHITE, "", 100); 
          blend_button.drawButton(true);
        }
      }
    }
    
    if (picker_btn.justReleased())
        picker_btn.drawButton();
        
    if (picker_btn.justPressed() and not adjust) {
      if (action == 0){
          bool set = false;
          for (int i = 0; i < palette_size; i++){
            if (not palette[i].get_full() and set == false){
              palette[i].set_full(true);
              palette[i].set_r(r);
              palette[i].set_g(g);
              palette[i].set_b(b);
              arrangeColours();
              for (int i = 0; i < palette_size; i++){
                drawButton(i);
              }
              set = true;
              
            }
          }
          picker_btn.drawButton(true);
      }

    }

    if (clear_btn.justPressed()) {
      if (action == 0){
        clear_btn.initButton(&tft, 190, 290, 80, 40, BLACK, BLACK, GREEN, "Clear", 1); 
        clear_btn.drawButton(true);
        action = 1;
      }
      else{
        clear_btn.initButton(&tft, 190, 290, 80, 40, BLACK, BLACK, WHITE, "Clear", 1); 
        clear_btn.drawButton(true);
        action = 0;
      }
      }
    //End Touchscreen
}

// Colour sensor

void colourred(){ 
  //select red
  digitalWrite(COLOURS2,LOW);
  digitalWrite(COLOURS3,LOW);
  }
  
void colourblue(){            
  //select blue
  digitalWrite(COLOURS2,LOW);
  digitalWrite(COLOURS3,HIGH);
  }
  
void colourwhite(){           
  //select white
  digitalWrite(COLOURS2,HIGH);
  digitalWrite(COLOURS3,LOW);
  }
  
void colourgreen(){           
  //select green
  digitalWrite(COLOURS2,HIGH);
  digitalWrite(COLOURS3,HIGH);
  }
  
int getintensity(){           
  //measure intensity with oversamplingint a=0;
  int b=255;
  int a=0;
  for(int i=0;i<10;i++){
    a=a+pulseIn(COLOUROUT,LOW);
  }
  if(a>9){
    b=7000/a;
  }
  b = b;
  return b; 
}

// end colour sensor

// Helper functions

uint16_t convertcolour(uint8_t r, uint8_t g, uint8_t b) { 
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3); 
  }

bool drawButton(int i){
    
    uint8_t i_colour[rgb];
    i_colour[0] = palette[i].get_r();
    i_colour[1] = palette[i].get_g();
    i_colour[2] = palette[i].get_b();
    uint16_t well_colour = convertcolour(i_colour[0], i_colour[1], i_colour[2]);
    int button_x = 75* cos(radians(angles)*i) + screen_width / 2;
    int button_y = 75* sin(radians(angles)*i) + screen_height / 2;
    palette_buttons[i].initButton(&tft, button_x, button_y, 50, 50, BLACK, well_colour, well_colour, "", 100); 
    palette_buttons[i].drawButton(true);

  return(true);
}


bool drawSquares(int i){
    
    uint8_t i_colour[rgb];
    i_colour[0] = palette[i].get_r();
    i_colour[1] = palette[i].get_g();
    i_colour[2] = palette[i].get_b();
    uint16_t well_colour = convertcolour(i_colour[0], i_colour[1], i_colour[2]);
    int button_x;
    int button_y;
    if (i > palette_size / 2-1){
      button_y = 250;
      button_x = 80 + 50 * (i - palette_size / 2);
    }
    else{
      button_y = 200;
      button_x = 80 + 50 * i;
    }
    palette_buttons[i].initButton(&tft, button_x, button_y, 50, 50, BLACK, well_colour, well_colour, "", 100); 
    palette_buttons[i].drawButton(true);

  return(true);
}
