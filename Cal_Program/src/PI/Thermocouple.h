

#include <iostream>
#include <vector>
#include <string.h>



#include "StringH.h"
#include "Data.h"





// Data Structure class to hold measurements read from Pi
class Thermocouple
{
public:
  Thermocouple()
  {
      thermocouple1_tempC = 0;
      thermocouple2_tempC = 0;
      thermocouple3_tempC = 0;
  }

  Thermocouple(char* pi_string)
  {
      this->Parse_Thermocouple_Data(pi_string);
  }

  void Parse_Thermocouple_Data(char* pi_string);

// Member variables
    double thermocouple1_tempC;
    double thermocouple2_tempC;
    double thermocouple3_tempC;
};



class Thermocouple_Data
{
public:
    Thermocouple_Data(){}
    ~Thermocouple_Data();

    void Add_Sample(char* pi_string);
    Data_Group* Get_Thermocouple1_Data();   
    Data_Group* Get_Thermocouple2_Data();
    Data_Group* Get_Thermocouple3_Data();

    void Calcs();
    void Print_Thermcouple_Data();

    float avg1, avg2, avg3;
    float stdev1, stdev2, stdev3;

// Member variables
    std::vector<Thermocouple*> data;
};