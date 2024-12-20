// 引入I2C通信库
#include <Arduino.h>
#include <Wire.h> 
// MPU6050 地址
const int MPU_ADDR = 0x68;
// 存储加速度和陀螺仪数据的变量
#define OFFSET_COUNT 200

typedef struct{
    int16_t AcX, AcY, AcZ; // 加速度
    int16_t GyX, GyY, GyZ; // 陀螺仪
    float Roll;//解算所得角度
    float Pitch;
    float Yaw;
 
    float Roll_a;//加速度计计算得到的角度
    float Yaw_a;
    float Pitch_a;
    float Roll_g;//陀螺仪计算得到的角速度
    float Yaw_g;
    float Pitch_g;
 
    float lastRoll;//上次的解算角度
    float lastPitch;
    float lastYaw;
 
    int offset_gx;//陀螺仪零漂值
    int offset_gy;
    int offset_gz;
}IMU;
 
IMU IMU_Data;
int first_open;
void IMU_offset(); //校准陀螺仪零漂值
float acc_real(int acc);//加速度计真实值
float gyro_real(int gyro,int gyro_offset);//陀螺仪真实值
float FOCF(float acc_m,float gyro_m,float* last_angle,int* angleChanged);//角度解算

int acc_precision=16384;//加速度计精度
float gyro_precision=16.4;//陀螺仪精度


//pin参数
int pin_trigger1=11;
int pin_vibmotor1=10;
int pin_vibmotor2=9;
int pin_hapitcmotorspeed=5;
int pin_flywheelmotorspeed=6;
//直流电机速度变量
int dcmotorspeed=0;
int dcmotorspeed_temp=0;
//上下沿变量
int trigger1_prev;
int trigger1_cur;

//时间记录
int counter=0;
int counter_start=0;

int ishit=0;

void setup() {
  //pin初始化
  pinMode(pin_trigger1,INPUT);
  pinMode(pin_vibmotor1,OUTPUT);
  pinMode(pin_vibmotor2,OUTPUT);
  pinMode(pin_flywheelmotorspeed,OUTPUT);
  pinMode(pin_hapitcmotorspeed,OUTPUT);
  
  //初始化上下沿变量
  trigger1_prev=1;
  trigger1_cur=1;

  //初始化时间变量
  counter=0;
  counter_start=0;
  dcmotorspeed=0;
  first_open=1;

  ishit=0;

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
  Wire.begin();
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B); // 从寄存器地址 0x3B 开始读取数据
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 14, true); // 请求14字节数据

  // 读取加速度数据
  IMU_Data.AcX = Wire.read() << 8 | Wire.read();
  IMU_Data.AcY = Wire.read() << 8 | Wire.read();
  IMU_Data.AcZ = Wire.read() << 8 | Wire.read();


  // 跳过温度数据（可选）
  Wire.read();
  Wire.read();

  // 读取陀螺仪数据
  IMU_Data.GyX = Wire.read() << 8 | Wire.read();
  IMU_Data.GyY = Wire.read() << 8 | Wire.read();
  IMU_Data.GyZ = Wire.read() << 8 | Wire.read();

  Serial.begin(9600);
  if(first_open){
    IMU_offset();
    first_open=0;
  }
  Serial.print("first_open:");
  Serial.print(first_open);

  IMU_Data.Roll_a=atan2(acc_real(IMU_Data.AcY),acc_real(IMU_Data.AcZ))*180/PI;
  IMU_Data.Yaw_a=atan2(acc_real(IMU_Data.AcX),acc_real(IMU_Data.AcZ))*180/PI;
  IMU_Data.Pitch=atan2(acc_real(IMU_Data.AcX),acc_real(IMU_Data.AcY))*180/PI;
  
  IMU_Data.Roll_g=gyro_real(IMU_Data.GyX,IMU_Data.offset_gx);
  IMU_Data.Yaw_g=gyro_real(IMU_Data.GyZ,IMU_Data.offset_gz);
  IMU_Data.Pitch_g=gyro_real(IMU_Data.GyY,IMU_Data.offset_gy);

  int RollChanged,PitchChanged,YawChanged;
  IMU_Data.Roll=FOCF(IMU_Data.Roll_a,IMU_Data.Roll_g,&IMU_Data.lastRoll,&RollChanged);
  IMU_Data.Pitch=FOCF(IMU_Data.Pitch_a,IMU_Data.Pitch_g,&IMU_Data.lastPitch,&PitchChanged);
  IMU_Data.Yaw=FOCF(IMU_Data.Yaw_a,IMU_Data.Yaw_g,&IMU_Data.lastYaw,&YawChanged);

  //读取串口发送的数据
  if(Serial.available()>0){
    ishit=Serial.parseInt();
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
  }else{
    if(counter>0)counter--;
  }

  //电机控制
  int charging_state=counter_start;
  int punching_state=!counter_start&&counter>0;
  int rest_state=!counter_start&&counter==0;
  
  if(charging_state){
    if(dcmotorspeed<255){
      dcmotorspeed++;
      dcmotorspeed_temp=dcmotorspeed;
    }
  }

  if(punching_state){
    if(YawChanged||PitchChanged){
      if(dcmotorspeed<255){
        dcmotorspeed++;
      }
    }else{
      if(dcmotorspeed>dcmotorspeed_temp){
        dcmotorspeed--;
      }
    }
    if(ishit==1){
      dcmotorspeed=0;
      dcmotorspeed_temp=0;
    }
  }

  if(rest_state){
    dcmotorspeed=0;
    dcmotorspeed_temp=0;
  }

  //haptic force控制
  if(ishit==1){
    analogWrite(pin_hapitcmotorspeed,150);
    delay(50);
    }else{
    analogWrite(pin_hapitcmotorspeed,0);
    }
  


  analogWrite(pin_flywheelmotorspeed,dcmotorspeed);
  
  // Serial.print(" counter_start");
  Serial.print(counter_start);
  Serial.print(" Roll");
  Serial.print(IMU_Data.Roll);
  Serial.print(" Pitch:");
  Serial.print(IMU_Data.Pitch);
  Serial.print(" Yaw:");
  Serial.print(IMU_Data.Yaw);
  Serial.print(" Roll_changed:");
  Serial.print(RollChanged);
  Serial.print(" Pitch_changed:");
  Serial.print(PitchChanged);
  Serial.print(" Yaw_changed:");
  Serial.print(YawChanged);
  Serial.print(" trigger1_cur: ");
  Serial.print(trigger1_cur);
  Serial.print(" counter: ");
  Serial.print(counter);
  Serial.print(" dcmotorspeed:");
  Serial.print(dcmotorspeed);
  Serial.print(" dcmotorspeed_temp:");
  Serial.print(dcmotorspeed_temp);
  Serial.print(" charging_state:");
  Serial.print(charging_state);
  Serial.print(" punching_state:");
  Serial.print(punching_state);
  Serial.print(" ishit:");
  Serial.print(ishit);
  Serial.print("\n");

Serial.end();
delay(100);

}


void IMU_offset(){
    for(int i=0;i<OFFSET_COUNT;i++){
      delay(10);
        if(IMU_Data.GyX==IMU_Data.GyY){
            i--;
        }
        else{
            IMU_Data.offset_gx+=IMU_Data.GyX;
            IMU_Data.offset_gy+=IMU_Data.GyY;
            IMU_Data.offset_gz+=IMU_Data.GyZ;
        }
    }
    IMU_Data.offset_gx/=OFFSET_COUNT;
    IMU_Data.offset_gy/=OFFSET_COUNT;
    IMU_Data.offset_gz/=OFFSET_COUNT;

}

float acc_real(int acc){
    return (acc)/acc_precision;
}

float gyro_real(int gyro,int gyro_offset){
    return -(gyro-gyro_offset)/gyro_precision;
}


#define Ka 0.80  //加速度解算权重
#define dt 0.005 //采样间隔（单位：秒）
 
float FOCF(float acc_m,float gyro_m,float* last_angle,int* angleChanged){
    float temp_angle;
    temp_angle=Ka*acc_m+(1-Ka)*(*last_angle+gyro_m*dt);
    if(temp_angle-*last_angle>0.5||temp_angle-*last_angle<-0.5){
        if(temp_angle-*last_angle>0.5){*angleChanged=1;}
        if(temp_angle-*last_angle<-0.5){*angleChanged=-1;}
    }
    else{
        *angleChanged=0;
    }
    *last_angle=temp_angle;
    return temp_angle;
}
