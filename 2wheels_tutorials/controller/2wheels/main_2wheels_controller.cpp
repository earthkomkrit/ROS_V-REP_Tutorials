//*******************************************
//*                                         *
//*             main controller	            *
//*                                         *
//*                                         *
//*******************************************
// author: Komkrit Thongbunchu
// contact: earthkmutt59@gmail.com

/*
This program is main controller for 2 wheels with Q Learning.
Specific 2  wheels :
	-	2 sensors left (variable in this program is sensor_left) and right (variable in this program is sensor_right). when 1 is detect found and 0 is detect not found
	-	motorSig[1] is Motor right : Drive with velocity
	-	motorSig[2] is Motor left  : Drive with velocity
 */


//standard library
#include <ros/ros.h>
#include "std_msgs/Bool.h"
#include "std_msgs/Float32.h"
#include <std_msgs/Int32.h>
#include "std_msgs/MultiArrayLayout.h"
#include "std_msgs/MultiArrayDimension.h"
#include "std_msgs/Float32MultiArray.h"
#include <rosgraph_msgs/Clock.h>

#define RATE 400 //reflesh rate of ros


//*******************************************
//*                                         *
//*            global variable              *
//*                                         *
//*******************************************

std_msgs::Float32MultiArray wheelsignal;
std_msgs::Float32MultiArray Plotsignal;

float motorSig[3] = {0};
float sensor[5] = {0};
float sensor_left = 0.0;
float sensor_right = 0.0;


//*******************************************
//*                                         *
//*            global function              *
//*                                         *
//*******************************************


//Function Read sensor 

void infraCallback(const std_msgs::Float32MultiArray::ConstPtr& array)
{
    int i = 1;
    for(std::vector<float>::const_iterator it = array->data.begin(); it != array->data.end(); ++it)
    {
        sensor[i] = *it;
        i++;
    }
    return;
}


//***************************************
//*										*
//*				main program			*
//*										*
//***************************************

struct timeval tv;
__time_t currentTime_updatedByTopicSubscriber=0;

int main(int argc, char *argv[]){
	// initializa ros node
	if (gettimeofday(&tv, nullptr)==0)
		currentTime_updatedByTopicSubscriber=(tv.tv_sec*1000+tv.tv_usec/1000)&0x00ffffff;\
	std::string nodeName("wheelsnode");
	ros::init(argc,argv,nodeName);

	// check robot operating system
	if(!ros::master::check())
		ROS_ERROR("ros::master::check() did not pass!");
	ros::NodeHandle node("~");

	ROS_INFO("simROS just started!");

	// set reflesh rate
	ros::Rate* rate;
	rate = new ros::Rate(RATE);

	// create publisher 
	ros::Publisher outputdrive;
	outputdrive = node.advertise<std_msgs::Float32MultiArray>("Outputdrive",1);

	ros::Publisher plotSignal;
	plotSignal = node.advertise<std_msgs::Float32MultiArray>("SignalPlot",1);

	// Create subscriber 
	ros::Subscriber inputinfrared;
	inputinfrared = node.subscribe("inputinfraTopic", 10, infraCallback);

	//	While loop when ros ok
 	while(ros::ok())
 	{	
		
		//Define Sensor Variable 
		sensor_right = sensor[1];
		sensor_left = sensor[2];

		// Hard code 
		if(sensor_right == 1 && sensor_left == 0){
			motorSig[1] = 5;
			motorSig[2] = -5;
		}else if(sensor_right == 0 && sensor_left == 1){
			motorSig[1] = -5;
			motorSig[2] = 5;
		}else if(sensor_right == 0 && sensor_left == 0){
			motorSig[1] = 5;
			motorSig[2] = 5;
		}else if(sensor_right == 1 && sensor_left == 1){
			motorSig[1] = -5;
			motorSig[2] = 5;
		}
		
		//Clear data before publish
		wheelsignal.data.clear();	
		Plotsignal.data.clear();

		//Pack data to wheelsignal before publish
		for(int j=1;j<=2;j++)
		{
			wheelsignal.data.push_back(motorSig[j]);
			Plotsignal.data.push_back(motorSig[j]);
		}		

		//By the way for pack data (Don't use loop)
		//wheelsignal.data.push_back(motorSig[1]);
		//wheelsignal.data.push_back(motorSig[2]);
		//...

		// Publish data
		outputdrive.publish(wheelsignal);
		plotSignal.publish(Plotsignal);

		// wait
		ros::spinOnce();
		bool rateMet = rate->sleep();
		/////////////////////////////////////////////////////
					
	} // main loop -> ros::ok
    return 0;
}

