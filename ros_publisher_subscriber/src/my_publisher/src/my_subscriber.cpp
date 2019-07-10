#include <ros/ros.h>
#include <my_publisher/MyMsg.h>

void msgCallback(const my_publisher::MyMsg::ConstPtr& msg)
{
	ROS_INFO("%d", msg->data);
}

int main(int argc, char **argv)
{
	ros::init(argc, argv, "my_subscriber");
	ros::NodeHandle nh;
	ros::Subscriber my_subscriber = nh.subscribe("my_publishment", 100, msgCallback);
	ros::spin();
	return 0;
}
