#ifndef DIFFDRIVE_ARDUINO_ROBOT_HPP
#define DIFFDRIVE_ARDUINO_ROBOT_HPP

#include <string>


class Robot
{
    public:

    std::string name = "";
    double cmd_ = 0;
    double mode_ = 0;
    double e_state_ = 0;
    double e_stop_ = 0;
    double battery_ = 0;
    double USS_left_ = 0;
    double USS_right_ = 0;
    double d_write_ = 0;
    double a_write_ = 0;
    double pin_ = 0;
    double US_Sensors_[2] = {0, 0};
    double io_ = 0;
    int cmd = 0;
    int mode = 0;
    int e_state = 0;
    int e_stop = 0;
    int battery = 0;
    int USS_left = 0;
    int USS_right = 0;
    int d_write = 0;
    int a_write = 0;
    int pin = 0;
    int US_Sensors[2] = {0, 0};
    int io = 0;

    Robot() = default;

    Robot(const std::string &robot_name)
    {
      setup(robot_name);
    }

    
    void setup(const std::string &robot_name)
    {
      name = robot_name;
    }



};


#endif // DIFFDRIVE_ARDUINO_ROBOT_HPP
