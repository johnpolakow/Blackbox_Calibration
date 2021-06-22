
#ifndef _STRING_H_
#define _STRING_H_

#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <math.h>


//#include "Time2.h"

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) ( sizeof(x) / sizeof((x)[0]) )    // macro to get number of items in an object array
#endif

class StringH
{
public:
  StringH();

  static char* Get_Substring(const char* full_string, int start_index, int end_index);
  static int Parse_Int(char* integer_string);
  static double Parse_Double(char* double_string);
  static char* Parse_IP(char* line);
  static char* Parse_MAC(char* line);
  static char* Parse_MAC_Addr(char* line);
  static int parse_USBTMC_Number(char* path);
  static void Trim_Leading_WhiteSpace(char* trim_string);
  static void Trim_Trailing_WhiteSpace(char * trim_string);
  static void Trim_WhiteSpace(char * trim_string);
  static void Erase_String(char* target_string);
  static void Erase_Num_Chars(char* string, int length);
  static char* Extract_Value_in_String(char* line);
  static int Index_First_Comma( char* string );
  static int Index_First_Colon( char* string );
  static int Index_First_Period( char* string );
  static int Index_nTH_Delimiter( char* string, int n, char delimiter );
  static void To_Upper(char* lowercase_string);
  static bool Is_Hex_Group(char* cstring, int index);
  static bool Is_MAC_Delim(char character);

  static int Index_Second_Whitespace( char* socket_string );
  static int Index_First_Whitespace( char* socket_string );
  static int Index_First_Sample(char* parse_string );
  static int Index_Second_Sample(char* parse_string );
  static bool String_Contains(char* haystack, const char* needle);
  static double Parse_Scientific_Notation( char* notation_string);
  static bool String_Contains_Ignore_Case(char* haystack, const char* needle);
  static int Index_of_E( char* string );
  static void Free_String_Array_Memory(char** string_array);
  static int Get_String_Array_Length(char** string_array);
  static void Write_Chars(char* write_buffer, int* column_index, const char* print_buffer, int MAX_CHARS);
  static void Advance_to_Index(char* write_buffer, int* index_ptr, int target_index);
  static bool Contains_Only_Digits(char* digit_string);
  static bool String_Is_TimeStamp(char* time_string);
  static char* Get_File_Extension(char* filepath);
  static char* Get_File_Name(char* filepath);
  static char* Get_File_Directory(char* filepath);
  static void Remove_File_Extension(char* filepath);
  static bool Line_Is_Whitespace(char* line);
  static bool Line_Is_Date(char* line);
  static bool Line_Is_Time(char* line);
  static bool Line_Is_TimeStamp(char* line);
  static void Convert_Character(char* line, char orig_char, char change_char);
  static bool Strings_Match(const char* string1, const char* string2);
  static char* Parse_First_Token(char* readline);
  static char* Parse_Second_Token(char* readline);
  static char* Parse_Third_Token(char* readline);
  static char* Get_First_Token(char* line);
  static int Get_Last_Index_Of(char* str, char find_char);


};

#endif
