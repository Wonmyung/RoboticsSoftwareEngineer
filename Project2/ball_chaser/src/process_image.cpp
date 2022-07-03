#include "ros/ros.h"
#include <sensor_msgs/Image.h>
#include "ball_chaser/DriveToTarget.h"

// Define global client
ros::ServiceClient client;

void drive_robot(float lin_x, float ang_z)
{
    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;

    if(client.call(srv))
    {
        ROS_INFO("[Make requst] linear_x: %.2f , angular_z: %.2f", lin_x, ang_z);
    }
    else
    {
        ROS_ERROR("Failed to call service drive_robot");
    }
    
}

void process_image_callback(const sensor_msgs::Image img)
{
    int white_pixel = 255;
    int trgt_imgStep = -1;
    int left_th = img.width * 0.3;
    int right_th = img.width * 0.7;
    float lin_x = 0.f;
    float ang_z = 0.f;
    
    for(int i = 0; i < (img.height * img.step); i+=3)
    {
        if((img.data[i] == white_pixel) && (img.data[i+1] == white_pixel) && (img.data[i+2] == white_pixel))
        {
            trgt_imgStep = i % img.step;
            break;
        }
    }

    if(trgt_imgStep > right_th) //right side
    {
        ang_z = -0.5f;
    }
    else if(trgt_imgStep >= left_th) //forward
    {
        lin_x = 0.5f; 
    }
    else if(trgt_imgStep != -1) //left side
    {
        ang_z = 0.5f;
    }

    drive_robot(lin_x, ang_z);

    if(trgt_imgStep == -1)
    {
        ROS_INFO_STREAM("Cannot find white ball in image!");
    }
    else { /* do nothing */ }
}

int main(int argc, char** argv)
{
    ros::init(argc, argv, "process_image");

    ros::NodeHandle n;

    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

	ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    ros::spin();

    return 0;
}