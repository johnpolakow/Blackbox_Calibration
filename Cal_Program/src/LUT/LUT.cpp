#include "LUT.h"

void Yokogawa_samples::Add_Yokogawa_sample(double value)
{
    if(this->cur_index < ARRAY_SIZE(data_equal_time))
    {
        this->data_equal_time[this->cur_index] = value;
        ++this->cur_index;
    }
}

int Yokogawa_samples::Get_Num_Samples()
{
    return this->cur_index;
}

double* Yokogawa_samples::Get_Yokogawa_samples()
{
    return this->data_equal_time;
}

LUT_Data::~LUT_Data()
{
        for(int i = 0; i< this->num_points; ++i)
        {
            delete this->data_matches[i];
            this->data_matches[i] = NULL;
        }
        this->num_points = 0;
        free(data_matches);
}

void LUT_Data::Clear_All_Matches()
{
    for(int i = 0; i< this->num_points; ++i)
    {
        delete this->data_matches[i];
        this->data_matches[i] = NULL;
    }
    this->num_points = 0;
}

void LUT_Data::Match_Time_Samples(AC_Data_Group* pi_data_group, AC_Data_Group* Yokogawa_data_group)
{
      if(pi_data_group == NULL || Yokogawa_data_group == NULL)
          return;

      int pi_index = 0;
      int Yokogawa_index = 0;

      for(int pi_index = 0; pi_index < pi_data_group->num_points; ++pi_index)
      {

          AC_Data_Point* pi_point = pi_data_group->data_points[ pi_index ];
          AC_Data_Point* Yokogawa_point;
          Time* pi_time = &pi_point->data_timestamp->time;
          //printf(COLOR_MAGENTA "\nPI timestamp: %s\n" COLOR_RESET, pi_time->toString() );

          Yokogawa_samples* Yokogawa_time_matches = new Yokogawa_samples();

          for(Yokogawa_index = 0; Yokogawa_index < Yokogawa_data_group->num_points; ++Yokogawa_index)
          {
              Yokogawa_point = Yokogawa_data_group->data_points[ Yokogawa_index ];
              Time* Yokogawa_time = &(Yokogawa_point->data_timestamp->time);

              if( Yokogawa_time->Equals(*pi_time) )
              {
                  Yokogawa_time_matches->Add_Yokogawa_sample(Yokogawa_point->value);
              }
          }

          // create the new match
          if(Yokogawa_time_matches->Get_Num_Samples() > 0 )
          {
              LUT_Match* new_match = new LUT_Match(pi_point->value, Yokogawa_time_matches->Get_Yokogawa_samples(), pi_point->data_timestamp, pi_data_group->units);
              //printf("\nAdding Match:\n");
              //LUT_Match::Print_Match(new_match);
              this->Add_Match(new_match);
          }

          delete Yokogawa_time_matches;
      }
      return;
  }

void LUT_Data::Add_Match(LUT_Match* new_match)
{
    if(this->num_points < MAX_MATCHES)
    {
        this->data_matches[this->num_points] = new_match;
        ++this->num_points;
    }
}

void LUT_Data::Print_Matches(LUT_Data* matches)
{
    // print header:
    // Description
    // time pi(unit)    BK(unit)    BK Avg
    //                              BK spread
    //                              BK Std Dev

    printf(COLOR_BOLD_YELLOW "\n\n\t\t%s\n" COLOR_RESET, matches->description);
    //printf("num matches: %d\n", matches->num_points);
    printf("timestamp   PI %s        Yoko %s \n", matches->data_matches[0]->units, matches->data_matches[0]->units);
    printf("--------------------------------------------------------------------------\n");

    for(int i = 0; i < matches->num_points; ++i)
    {
        LUT_Match* match = matches->data_matches[i];
        LUT_Match::Print_Match(match);
        printf("----------------------\n");
    }
}




void LUT_Data::Write_Data_To_File(LUT_Match** matches, int num_matches)
{
    // create proper filename: COOL_MA_AC_DATA
    // write description of the file, and legend header
    //  write all entries
}












void LUT_Data::Clean_Up_Data(LUT_Match** noisy_data)
{
    // Filter BK/PI Data based on :
        // time since test began (dont use first 15 seconds or so)
        // spread of the BK data compared to the values
        // STD deviation of the BK data
        // # of BK data points (min 3)
        // slope to points on either side of this BK point
        // slope to points on either side of this PI point
        // how large of a percentage of the value this would create in the LUT
}
