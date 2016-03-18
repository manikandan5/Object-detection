/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Utility.h
 * Author: rakhasan
 *
 * Created on March 11, 2016, 5:07 PM
 */

#ifndef UTILITY_H
#define UTILITY_H
#include <string>
#include <iostream>
#include <sstream>
#include <sys/stat.h>

class Utility
{
public:
  // following method was copied from http://stackoverflow.com/questions/5590381/easiest-way-to-convert-int-to-string-in-c
  template <typename T>
  static string NumberToString ( T Number )
  {
     ostringstream ss;
     ss << Number;
     return ss.str();
  }
  
  static void CreateDirectory(const string& dir_name)
  {       
    struct stat sb;

    if (stat(dir_name.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode))
    {
        cout<<dir_name<<" already exists."<<endl;
        return;
    }
    const int dir_err = mkdir(dir_name.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if (-1 == dir_err)
    {
        cout<<"Error creating directory: "<<dir_name<<endl;
        exit(1);
    }
  }
  

};

#endif /* UTILITY_H */

