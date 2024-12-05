// 引入I2C通信库
#include <Arduino.h>
#include <Wire.h> 
// MPU6050 地址
const int MPU_ADDR = 0x68;
// 存储加速度和陀螺仪数据的变量
int16_t AcX, AcY, AcZ; // 加速度
int16_t GyX, GyY, GyZ; // 陀螺仪

//pin参数
int pin_trigger1=13;
int pin_trigger2=12;
int pin_trigger3=11;
int pin_vibmotor1=10;
int pin_vibmotor2=9;
int pin_dcmotorspeed=6;
int pin_dcmotordirc=7;

//直流电机速度变量
int dcmotorspeed=0;

//上下沿变量
int trigger1_prev;
int trigger1_cur;

//时间记录
int counter=0;
int counter_start=0;

void setup() {
  //pin初始化
  pinMode(pin_trigger1,INPUT);
  pinMode(pin_trigger2,INPUT);
  pinMode(pin_trigger3,INPUT);
  pinMode(pin_vibmotor1,OUTPUT);
  pinMode(pin_vibmotor2,OUTPUT);
  pinMode(pin_dcmotordirc,OUTPUT);
  pinMode(pin_dcmotorspeed,OUTPUT);
  
  //初始化上下沿变量
  trigger1_prev=1;
  trigger1_cur=1;

  //初始化时间变量
  counter=0;
  counter_start=0;
  dcmotorspeed=0;

  Wire.begin(); // 初始化I2C通信

  // 初始化 MPU6050
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B); // 访问电源管理寄存器
  Wire.write(0);    // 设置为0唤醒传感器
  Wire.endTransmission(true);

  Serial.println("MPU6050 初始化完成");
}

void loop() {
  // 读取加速度和陀螺仪数据
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B); // 从寄存器地址 0x3B 开始读取数据
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 14, true); // 请求14字节数据

  // 读取加速度数据
  AcX = Wire.read() << 8 | Wire.read();
  AcY = Wire.read() << 8 | Wire.read();
  AcZ = Wire.read() << 8 | Wire.read();

  // 跳过温度数据（可选）
  Wire.read();
  Wire.read();

  // 读取陀螺仪数据
  GyX = Wire.read() << 8 | Wire.read();
  GyY = Wire.read() << 8 | Wire.read();
  GyZ = Wire.read() << 8 | Wire.read();

  //读取串口发送的数据
  if(Serial.available()>0){
    int isHit=Serial.parseInt();
  }

  //判断开关情况
  trigger1_prev=trigger1_cur;
  trigger1_cur=digitalRead(pin_trigger1);
  int trigger1_negedge=!trigger1_prev&&trigger1_cur;
  int trigger1_posedge=trigger1_prev&&!trigger1_cur;

  //震动电机控制
  if(trigger1_cur){
    digitalWrite(pin_vibmotor1,LOW);
    digitalWrite(pin_vibmotor2,LOW);
  }
  else{
    digitalWrite(pin_vibmotor1,HIGH);
    digitalWrite(pin_vibmotor2,HIGH);
  }

  //按下时长记录
  if(trigger1_posedge){
    counter_start=1;
  }
  if(trigger1_negedge){
    counter_start=0;
  }

  if(counter_start){
    counter++;
  }

  //飞轮直流电机控制
  if(!trigger1_cur){
    if(counter%160000&&dcmotorspeed<127)dcmotorspeed++;
  }else{
    if(counter%160000&&dcmotorspeed>0)dcmotorspeed--;
  }
  analogWrite(pin_dcmotorspeed,dcmotorspeed);
  
  if(digitalRead(pin_trigger2)){
    digitalWrite(pin_dcmotordirc,HIGH);}
  else{
    digitalWrite(pin_dcmotordirc,LOW);
    }

  if(counter>0&&!counter_start){
    // digitalWrite(pin_vibmotor1,HIGH);
    // digitalWrite(pin_vibmotor2,HIGH);
    counter--;
  }
  else{
    // digitalWrite(pin_vibmotor1,LOW);
    // digitalWrite(pin_vibmotor2,LOW);
  }


  Serial.begin(9600);
  Serial.println(AcX);
  Serial.println(AcY);
  Serial.println(AcZ);
  Serial.println(GyX);
  Serial.println(GyY);
  Serial.println(GyZ);
  Serial.end();

}
