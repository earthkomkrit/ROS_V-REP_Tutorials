#include <ros/ros.h>
#include <my_publisher/MyMsg.h>

int main(int argc, char **argv){

	ros::init(argc, argv, "my_publisher");
	ros::NodeHandle nh;
	ros::Publisher my_publisher = nh.advertise<my_publisher::MyMsg>("my_publishment", 100);
	ros::Rate loop_rate(50);
	my_publisher::MyMsg msg;
	int counter = 0;
	while(ros::ok())
	{
		msg.data = counter;
		ROS_INFO("%d", msg.data);
		my_publisher.publish(msg);
		loop_rate.sleep();
		counter++;
	}	
	return 0;
}
