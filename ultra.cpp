#include <stdio.h>
#include <stdlib.h>
#include <ros/ros.h>
#include <wiringPi.h>
#include <std_msgs/Int16.h>
#include <geometry_msgs/Twist.h>

#define Trig 4
#define Echo 5

using namespace std;


class Ultra
{
public:
  Ultra()
  {
  pub  = nh.advertise<geometry_msgs::Twist>("/cmd_vel",5);
  pub2 = nh.advertise<std_msgs::Int16>("/ultra_forward_arrival", 1);
  pub3 = nh.advertise<std_msgs::Int16>("/ultra_backward_arrival", 1);

  sub  = nh.subscribe("/ultra", 1, &Ultra::subUltraSonic , this);
  }
  //로봇 전진 정지
  void robotStop()
  {
    ROS_INFO("STOP");
    cmd_vel.linear.x = 0.0;
    pub.publish(cmd_vel);
    fin.data = 2;
    pub2.publish(fin);
  }
  //로봇 후진 정지
  void robotStop2()
  {
    ROS_INFO("STOP");
    cmd_vel.linear.x = 0.0;
    pub.publish(cmd_vel);
    fin2.data = 4;
    pub3.publish(fin2);
  }
  //로봇 전진
  void robotGo()
  {
    cmd_vel.linear.x = 0.4; //선속도
    pub.publish(cmd_vel);
  }
  //로봇 후진
  void robotBack()
  {
    cmd_vel.linear.x = -0.4;
    pub.publish(cmd_vel);
  }
  //초음파 센서(전진)
  void robotUltra_forward(){
    int start_time, end_time;
    double distance;

    pinMode(Trig, OUTPUT);
    pinMode(Echo, INPUT);

    while(ros::ok())
    {
      digitalWrite(Trig, LOW);
      delay(1000);
      digitalWrite(Trig, HIGH);
      delayMicroseconds(10);
      digitalWrite(Trig, LOW);

      while (digitalRead(Echo) == 0);
      start_time = micros();
      while (digitalRead(Echo) == 1);
      end_time = micros();
      distance = (end_time - start_time)/29./2.;
      //로봇 이동
      if(distance > 10){ //거리가 10cm 초과이면
        printf("distance: %.2f cm\n", distance);
        this->robotGo(); //로봇 전진
      }
      else{
        this->robotStop(); //10cm 이하이면 로봇 정지
        delayMicroseconds(10);
        break;
      }
    }
  }
  //초음파 센서(후진)
  void robotUltra_backward()
  {
    int start_time, end_time;
    double distance;

    pinMode(Trig, OUTPUT);
    pinMode(Echo, INPUT);

    while(ros::ok())
    {
      digitalWrite(Trig, LOW);
      delay(1000);
      digitalWrite(Trig, HIGH);
      delayMicroseconds(10);
      digitalWrite(Trig, LOW);

      while (digitalRead(Echo) == 0);
      start_time = micros();
      while (digitalRead(Echo) == 1);
      end_time = micros();
      distance = (end_time - start_time)/29./2.;
      //로봇 이동
      if(distance < 10){ //거리가 10cm 미만이면
        printf("distance: %.2f cm\n", distance);
        this->robotBack(); //로봇 후진
      }
      else{
        this->robotStop2();  //10cm 이상이면 로봇 정지
        delayMicroseconds(10);
        break;
      }
    }
  }

  //콜백 함수
  void subUltraSonic(const std_msgs::Int16 subUltraSonic)
  {
    switch(subUltraSonic.data)
    {
    case 1:
      ROS_INFO("Forward Received!!");
      ROS_INFO("UltraSensor is Activating");
      robotUltra_forward();
      break;
    case 2:
      ROS_INFO("Backward Received!!");
      ROS_INFO("UltraSensor is Activating");
      robotUltra_backward();
      break;
    }
  }

private:
  //노드 핸들 초기화
  ros::NodeHandle nh;
  //퍼블리셔
  ros::Publisher pub;
  ros::Publisher pub2;
  ros::Publisher pub3;
  //서브스크라이버
  ros::Subscriber sub;
  //변수선언
  std::string name;
  geometry_msgs::Twist cmd_vel;
  std_msgs::Int16 fin;
  std_msgs::Int16 fin2;
};

int main(int argc, char **argv)
{
  //Initiate ROS
  ros::init(argc, argv, "UltraSensor");

  //Create an object of class Ultra
  Ultra ultra;

  ROS_INFO("Waiting for robot...");
  wiringPiSetup();

  ros::spin();
  return 0;
}
