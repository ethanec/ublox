#include "ros/ros.h"

#include <gps_common/GPSFix.h>
#include <ublox_msgs/NavPVT.h>
#include <sensor_msgs/NavSatFix.h>
#include <novatel_gps_msgs/Inspva.h>
#include <novatel_gps_msgs/Gprmc.h>
#include <novatel_gps_msgs/Gpgga.h>
#include <math.h>


gps_common::GPSFix RTK;
novatel_gps_msgs::Inspva pva;
novatel_gps_msgs::Gprmc rmc;
novatel_gps_msgs::Gpgga gga;


void fixCallback(const sensor_msgs::NavSatFix &msg)
{
  RTK.header = msg.header;
  pva.header = msg.header;
  rmc.header = msg.header;
  gga.header = msg.header;
  gga.header.seq = msg.header.seq;
  

  RTK.latitude = msg.latitude;
  RTK.longitude = msg.longitude;
  RTK.altitude = msg.altitude;
  gga.gps_qual = (msg.position_covariance[0]<0.0002)? 5:((msg.position_covariance[0]<0.01)? 4:1);  

}

void navpvtCallback(const ublox_msgs::NavPVT &msg)
{
  
  RTK.track = msg.heading * pow(10,-5)*1.0;
  pva.azimuth = msg.heading * pow(10,-5)*1.0;
  rmc.track = msg.heading * pow(10,-5)*1.0;

}
  
int main(int argc, char **argv)
{
  ros::init(argc, argv, "trans_msgs");
  ros::NodeHandle n; 
  
  ros::Publisher pub = n.advertise<gps_common::GPSFix>("/novatel/gps", 100000);
  ros::Publisher pub_2 = n.advertise<novatel_gps_msgs::Inspva>("/novatel/inspva",100000);
  ros::Publisher pub_3 = n.advertise<novatel_gps_msgs::Gprmc>("/novatel/gprmc",100000);
  ros::Publisher pub_4 = n.advertise<novatel_gps_msgs::Gpgga>("/novatel/gpgga",100000);

  ros::Subscriber sub1 = n.subscribe("/ublox_f9k/fix", 1, fixCallback);
  ros::Subscriber sub2 = n.subscribe("/ublox_f9k/navpvt", 1, navpvtCallback);
  
  ros::Rate loop_rate(10);
  while (ros::ok())
  {

    pub.publish(RTK);
    pub_2.publish(pva);
    pub_3.publish(rmc);
    pub_4.publish(gga);
    
    ros::spinOnce(); 
    loop_rate.sleep();
    
  }
  //ros::spin();
  return 0;
}
