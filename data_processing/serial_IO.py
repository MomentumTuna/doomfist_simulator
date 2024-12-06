import serial
import json
#需要发送的数据变量
#i: gyro变量：ax,ay,az,gx,gy,gz
#   开关变量：trigger trigger按下时间需要和电机转动时间匹配
#o: 是否击中：hit


# 设置串口连接
# 替换 '/dev/ttyACM0' 为你的 Arduino 串口，Windows 上通常是 'COM3' 或 'COM4'

def readString():
    data=ser.readline()
    return data.decode('utf-8').strip()

ser = serial.Serial('COM3', 9600, bytesize=8, parity='N', stopbits=1, timeout=1)
print("串口连接成功")
# 读取Arduino发送的数据
dcmotor_speed=0
try:
    while True:
        if ser.in_waiting > 0:  # 检查串口缓冲区是否有数据
            serialdata=readString()
            print(serialdata)
            print("\n")
            dataset=serialdata.split(" ")
            MPU6050={"ax":dataset[0],"ay":dataset[1],"az":dataset[2],"gx":dataset[3],"gy":dataset[4],"gz":dataset[5]}
            
            trigger=dataset[6]
            counter=dataset[7]
            
except KeyboardInterrupt:
    print("程序终止")
finally:
    ser.close()  # 关闭串口

def readString():
    data=ser.readline()
    return data.decode('utf-8').strip()
    
def dcmotorspeed_control(MPU6050,trigger,counter,ishit):
    dcmotorspeed=0
    if trigger==1:
        dcmotorspeed=counter/160000
    else:
        if counter>0:
            if ishit==1:
                dcmotorspeed=0
            else:
                dcmotorspeed=counter/160000
            #根据gyro的数据控制转速
    return dcmotorspeed
    