#ifndef DIFFDRIVE_ARDUINO_ARDUINO_COMMS_HPP
#define DIFFDRIVE_ARDUINO_ARDUINO_COMMS_HPP

// #include <cstring>
#include <sstream>
// #include <cstdlib>
#include <libserial/SerialPort.h>
#include <iostream>


LibSerial::BaudRate convert_baud_rate(int baud_rate)
{
  // Just handle some common baud rates
  switch (baud_rate)
  {
    case 1200: return LibSerial::BaudRate::BAUD_1200;
    case 1800: return LibSerial::BaudRate::BAUD_1800;
    case 2400: return LibSerial::BaudRate::BAUD_2400;
    case 4800: return LibSerial::BaudRate::BAUD_4800;
    case 9600: return LibSerial::BaudRate::BAUD_9600;
    case 19200: return LibSerial::BaudRate::BAUD_19200;
    case 38400: return LibSerial::BaudRate::BAUD_38400;
    case 57600: return LibSerial::BaudRate::BAUD_57600;
    case 115200: return LibSerial::BaudRate::BAUD_115200;
    case 230400: return LibSerial::BaudRate::BAUD_230400;
    default:
      std::cout << "Error! Baud rate " << baud_rate << " not supported! Default to 57600" << std::endl;
      return LibSerial::BaudRate::BAUD_57600;
  }
}

class ArduinoComms
{

public:

  ArduinoComms() = default;

  void connect(const std::string &serial_device, int32_t baud_rate, int32_t timeout_ms)
  {  
    timeout_ms_ = timeout_ms;
    serial_conn_.Open(serial_device);
    serial_conn_.SetBaudRate(convert_baud_rate(baud_rate));
  }

  void disconnect()
  {
    serial_conn_.Close();
  }

  bool connected() const
  {
    return serial_conn_.IsOpen();
  }


  std::string send_msg(const std::string &msg_to_send, bool print_output = false)
  {
    serial_conn_.FlushIOBuffers(); // Just in case
    serial_conn_.Write(msg_to_send);

    std::string response = "";
    try
    {
      // Responses end with \r\n so we will read up to (and including) the \n.
      serial_conn_.ReadLine(response, '\n', timeout_ms_);
    }
    catch (const LibSerial::ReadTimeout&)
    {
        std::cerr << "The ReadByte() call has timed out." << std::endl ;
    }

    if (print_output)
    {
      std::cout << "Sent: " << msg_to_send << " Recv: " << response << std::endl;
    }

    return response;
  }


  void send_empty_msg()
  {
    std::string response = send_msg("\r");
  }

  void read_encoder_values(int &val_1, int &val_2)
  {
    std::string response = send_msg("e\r");

    std::string delimiter = " ";
    size_t del_pos = response.find(delimiter);
    std::string token_1 = response.substr(0, del_pos);
    std::string token_2 = response.substr(del_pos + delimiter.length());

    val_1 = std::atoi(token_1.c_str());
    val_2 = std::atoi(token_2.c_str());
  }

  void read_robot_state(int &val_1, int &val_2, int &val_3, int &val_4, int &val_5, int &val_6)
  {
    // Send the command "z\r" to the hardware
    std::string response = send_msg("z\r");
    // Check if the response is empty
    if (response.empty())
    {
      throw std::runtime_error("No response received from the hardware.");
    }
  
    // Use a stringstream to parse the response
    std::istringstream iss(response);
    std::vector<int> values;
    int value;
    // Extract integers from the response
    while (iss >> value)
    {
      values.push_back(value);
    }
  
    // Check if we received exactly six values
    if (values.size() != 6)
    {
      throw std::runtime_error("Expected 6 values, but received " + std::to_string(values.size()) + " values.");
    }
  
    // Assign the values to the output parameters
    val_1 = values[0];
    val_2 = values[1];
    val_3 = values[2];
    val_4 = values[3];
    val_5 = values[4];
    val_6 = values[5];
  }

  void set_motor_values(int val_1, int val_2)
  {
    std::stringstream ss;
    ss << "m " << val_1 << " " << val_2 << "\r";
    send_msg(ss.str());
  }

  void set_pid_values(int k_p, int k_d, int k_i, int k_o)
  {
    std::stringstream ss;
    ss << "u " << k_p << ":" << k_d << ":" << k_i << ":" << k_o << "\r";
    send_msg(ss.str());
  }

  void analog_read_pin(int pin, int &val)
  {
    std::stringstream ss;
    ss << "a " << pin << "\r";
    std::string response = send_msg(ss.str());
    // Check if the response is empty
    if (response.empty())
    {
      throw std::runtime_error("No response received from the hardware.");
    }
  
    // Use a stringstream to parse the response
    std::istringstream iss(response);
    std::vector<int> values;
    int value;
    // Extract integers from the response
    while (iss >> value)
    {
      values.push_back(value);
    }
  
    // Check if we received exactly six values
    if (values.size() != 1)
    {
      throw std::runtime_error("Expected 1 values, but received " + std::to_string(values.size()) + " values.");
    }
  
    // Assign the values to the output parameters
    val = values[0];
  }

  void read_battery(double &val)
  {
    std::string response = send_msg("b\r");
    // Check if the response is empty
    if (response.empty())
    {
      throw std::runtime_error("No response received from the hardware.");
    }
  
    // Use a stringstream to parse the response
    std::istringstream iss(response);
    std::vector<double> values;
    double value;
    // Extract integers from the response
    while (iss >> value)
    {
      values.push_back(value);
    }
  
    // Check if we received exactly six values
    if (values.size() != 1)
    {
      throw std::runtime_error("Expected 1 values, but received " + std::to_string(values.size()) + " values.");
    }
  
    // Assign the values to the output parameters
    val = values[0];
  }

  void set_pin_mode(int pin, int val)
  {
    std::stringstream ss;
    ss << "c " << pin << " " << val << "\r";
    send_msg(ss.str());
  }

  void digital_read_pin(int pin, int &val)
  {
    std::stringstream ss;
    ss << "d " << pin << "\r";
    std::string response = send_msg(ss.str());
    // Check if the response is empty
    if (response.empty())
    {
      throw std::runtime_error("No response received from the hardware.");
    }
  
    // Use a stringstream to parse the response
    std::istringstream iss(response);
    std::vector<int> values;
    int value;
    // Extract integers from the response
    while (iss >> value)
    {
      values.push_back(value);
    }
  
    // Check if we received exactly six values
    if (values.size() != 1)
    {
      throw std::runtime_error("Expected 1 values, but received " + std::to_string(values.size()) + " values.");
    }
  
    // Assign the values to the output parameters
    val = values[0];
  }

  void motor_brake()
  {
    send_msg("l\r");
  }

  void release_brake()
  {
    send_msg("n\r");
  }

  void ping_ultrasonics(double &val_1, double &val_2)
  {
    // Send the command "z\r" to the hardware
    std::string response = send_msg("p\r");
    // Check if the response is empty
    if (response.empty())
    {
      throw std::runtime_error("No response received from the hardware.");
    }
  
    // Use a stringstream to parse the response
    std::istringstream iss(response);
    std::vector<double> values;
    double value;
    // Extract integers from the response
    while (iss >> value)
    {
      values.push_back(value);
    }
  
    // Check if we received exactly six values
    if (values.size() != 2)
    {
      throw std::runtime_error("Expected 2 values, but received " + std::to_string(values.size()) + " values.");
    }
  
    // Assign the values to the output parameters
    val_1 = values[0];
    val_2 = values[1];
  }

  void reset_encoders()
  {
    send_msg("r\r");
  }

  void e_state(int &val)
  {
    std::string response = send_msg("s\r");
    // Check if the response is empty
    if (response.empty())
    {
      throw std::runtime_error("No response received from the hardware.");
    }
  
    // Use a stringstream to parse the response
    std::istringstream iss(response);
    std::vector<int> values;
    int value;
    // Extract integers from the response
    while (iss >> value)
    {
      values.push_back(value);
    }
  
    // Check if we received exactly six values
    if (values.size() != 1)
    {
      throw std::runtime_error("Expected 1 values, but received " + std::to_string(values.size()) + " values.");
    }
  
    // Assign the values to the output parameters
    val = values[0];
  }

  void robot_mode(int &val)
  {
    std::string response = send_msg("y\r");
    // Check if the response is empty
    if (response.empty())
    {
      throw std::runtime_error("No response received from the hardware.");
    }
  
    // Use a stringstream to parse the response
    std::istringstream iss(response);
    std::vector<int> values;
    int value;
    // Extract integers from the response
    while (iss >> value)
    {
      values.push_back(value);
    }
  
    // Check if we received exactly six values
    if (values.size() != 1)
    {
      throw std::runtime_error("Expected 1 values, but received " + std::to_string(values.size()) + " values.");
    }
  
    // Assign the values to the output parameters
    val = values[0];
  }

  void temp_read(double &val_1, double &val_2)
  {
    std::string response = send_msg("t\r");
    // Check if the response is empty
    if (response.empty())
    {
      throw std::runtime_error("No response received from the hardware.");
    }
  
    // Use a stringstream to parse the response
    std::istringstream iss(response);
    std::vector<double> values;
    double value;
    // Extract integers from the response
    while (iss >> value)
    {
      values.push_back(value);
    }
  
    // Check if we received exactly six values
    if (values.size() != 2)
    {
      throw std::runtime_error("Expected 2 values, but received " + std::to_string(values.size()) + " values.");
    }
  
    // Assign the values to the output parameters
    val_1 = values[0];
    val_2 = values[1];
  }

  void e_stop()
  {
    send_msg("v\r");
  }

  void digital_write(int pin, int val)
  {
    std::stringstream ss;
    ss << "w " << pin << " " << val << "\r";
    send_msg(ss.str());
  }

  void analog_write(int pin, int val)
  {
    std::stringstream ss;
    ss << "x " << pin << " " << val << "\r";
    send_msg(ss.str());
  }

private:
    LibSerial::SerialPort serial_conn_;
    int timeout_ms_;
};

#endif // DIFFDRIVE_ARDUINO_ARDUINO_COMMS_HPP