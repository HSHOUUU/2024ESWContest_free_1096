#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define mode_monstera 0
#define mode_tableyaza 1
#define mode_dayooki 2
#define mode_sanhosoo 3

#define SOIL_WATER_SENSOR A0
#define SOIL_WATER_SENSOR_MAX 900
#define SOIL_WATER_SENSOR_MIN 420

#define SOIL_WATER_REF_monstera 98
#define SOIL_WATER_REF_tableyaza 60
#define SOIL_WATER_REF_dayooki 1
#define SOIL_WATER_REF_sanhosoo 40

#define MIN_WATER_STANDARD_monstera 99
#define MIN_WATER_STANDARD_tableyaza 70
#define MIN_WATER_STANDARD_dayooki 5
#define MIN_WATER_STANDARD_sanhosoo 60

#define PUMP 2
#define BUTTON_PINS {7, 8, 11, 12}

LiquidCrystal_I2C lcd(0x27, 16,2);
int Pump_Value=0;
const int buttonPin[] = BUTTON_PINS;

int check=1;
int mode = -1;          //키패드로 입력받은 모드
int current_mode=-1;    //현재 작동중인 모드
int current_soil_water_ref;
int current_min_water_standard;

void keypad_read();
void water_pump();
void water_process(int water);

int water;

int water_pin = A3;    //꼿은 핀번호로 A0위치 조정
int water_level = 200; //수위 레벨 실험해보고 조정

bool warningDisplayed = false;
bool LightDisplayed = false;

int light;
int daytime = 100; // 수정해야댐.
int evening = 500;
int night = 900;

int led1 = 4; // led선 가에
int led2 = 3; //led선 중앙

int AA=5;
int AB=6;
void setup() {
  //Initializig LCD
  lcd.init();
  lcd.backlight();

  //Print on lcd
  lcd.setCursor(0,0);
  lcd.print("Personal");
  lcd.setCursor(0,1);
  lcd.print("Smart Farm");
  
  //Initializing Pump Pin
  pinMode(AA, OUTPUT);
  digitalWrite(AA, LOW);
  pinMode(AB, OUTPUT);
  digitalWrite(AB, LOW);

  for(int i = 0; i < 4; i++)
  {
    pinMode(buttonPin[i], INPUT_PULLUP);
  }
  
  //Set Baud rate by 9600
  Serial.begin(9600);

  pinMode(led1, OUTPUT); //led1 제어
  pinMode(led2, OUTPUT); //led2 제어
  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
  
  //time delay for Print on LCD
  delay(3000);

  //모드 설정
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Set Mode");

 }

void loop() {
  //스마트화분 기능 시작

  //수위레벨 체크
  if(analogRead(A3) > water_level){
    keypad_read();                        //키패드 값 입력받기
    water = analogRead(SOIL_WATER_SENSOR); // 토양 습도센서
    water_process(water);  //물 공급
    warningDisplayed = false;
    LightDisplayed = false;   
  }
  else {
    if(!warningDisplayed){
      digitalWrite(AA,LOW);
      digitalWrite(AB,LOW);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Warning!! ");
      lcd.setCursor(0, 1);
      lcd.print("Fill water!!");

      // LightDisplayed = true;
      // warningDisplayed = true; 
    }
  }
  light = analogRead(A1);
  light_process(light);
  
  //수위, 조도값 체크
   Serial.print("수위:");
   Serial.println(analogRead(A3));
  // Serial.print("조도:");
  // Serial.println(analogRead(A1));
}

//키패드 입력받기 & 모드 설정 
void keypad_read(){
  //키패드 입력받기
  for(int x=0 ; x < 4; x++){
    if(digitalRead(buttonPin[x]) == LOW)
    {
      mode=x;
      break;
    } 
  }

  // 모드 설정
  if(mode != -1 && mode != current_mode){
    current_mode = mode;

    lcd.clear();
    lcd.setCursor(0, 0);

    switch (current_mode) {
      case mode_monstera:
        current_soil_water_ref = SOIL_WATER_REF_monstera;
        current_min_water_standard = MIN_WATER_STANDARD_monstera;
        lcd.print("Mode: Monstera");
        break;

      case mode_tableyaza:
        current_soil_water_ref = SOIL_WATER_REF_tableyaza;
        current_min_water_standard = MIN_WATER_STANDARD_tableyaza;
        lcd.print("Mode: Tableyaza");
        break;

      case mode_dayooki:
        current_soil_water_ref = SOIL_WATER_REF_dayooki;
        current_min_water_standard = MIN_WATER_STANDARD_dayooki;
        lcd.print("Mode: Dayooki");
        break;

      case mode_sanhosoo:
        current_soil_water_ref = SOIL_WATER_REF_sanhosoo;
        current_min_water_standard = MIN_WATER_STANDARD_sanhosoo;
        lcd.print("Mode: Sanhosoo");
        break;
    }

  // 모드값 체크 
  //  Serial.print("Mode :");
  //  Serial.println(current_mode + 1);
  //  Serial.print("SOIL_WATER_REF :");
  //  Serial.println(current_soil_water_ref);
  //  Serial.print("MIN_WATER_STANDARD");
  //  Serial.println(current_min_water_standard);
  }
}

//펌프 작동 시스템
void water_pump(){
  digitalWrite(AA,HIGH);
  digitalWrite(AB,LOW);
  Pump_Value = 1;
}
//펌프 중지
void stop_water(){
  digitalWrite(AA, LOW);
  digitalWrite(AB, LOW);
}

//물 공급 시스템
void water_process(int water){
  if (current_mode == -1){
    return;
  }

  int soil_water=map(water,SOIL_WATER_SENSOR_MAX, SOIL_WATER_SENSOR_MIN,0 ,100);
  Serial.print("습도");
  Serial.println(soil_water);

  lcd.setCursor(0, 1);
  
  if (soil_water >= current_min_water_standard){
    stop_water();
    lcd.print("        ");
    lcd.setCursor(0, 1);
    lcd.print("enough");
    Pump_Value=0;
    check=1;
  }
  else {
    if (soil_water <= current_soil_water_ref){
      water_pump();
      lcd.print("        ");
      lcd.setCursor(0, 1);
      lcd.print("Watering");
      Pump_Value=1;
      check=0;
    }
    else if (check==0 && (soil_water >= current_soil_water_ref) && (soil_water <= current_min_water_standard)){
      water_pump();
      lcd.print("        ");
      lcd.setCursor(0, 1);
      lcd.print("Watering");
      Pump_Value=1;
      check=0;
    }
    else {      
      stop_water();
      lcd.print("        ");
      lcd.setCursor(0, 1);
      lcd.print("enough");
      Pump_Value=0;
      check=1;
    }  
  }
}

// 식물 생장등 시스템
void light_process(int light){
  if (current_mode == -1){
    return;
  }

  lcd.setCursor(9,1);

  if(!warningDisplayed){
    if(light >= daytime && light <= evening){
      digitalWrite(led1, LOW);
      digitalWrite(led2, LOW);
      lcd.print("morning");
    }
    else if(light > evening && light <= night){
      digitalWrite(led1, HIGH);
      digitalWrite(led2, LOW);
      lcd.print("evening");
        }
    else if(light > night){
      digitalWrite(led1,HIGH);
      digitalWrite(led2,HIGH);
      lcd.print("night");
    }
  }
}
