#include <iostream>
#include <fstream>
#include "errors.h"
#include "enigma.h"

Reflector::Reflector(char configuration[])
{
  errorcode = setup(configuration);
}

void Reflector::initialize_rf_mapping()
{
  char letter;
  for (int i = MIN_INDEX; i <= MAX_INDEX; i++)
    {
      letter = i + 'A';
      rf_mapping[i] = letter;
    }
}

int Reflector::setup(char configuration[])
{
  initialize_rf_mapping();

  int count = 0;
  
  std::ifstream in;
  int input1;
  int input2;

  in.open(configuration);

  //Start of validation
  if (in.fail())
    {
      cerr_rf(ERROR_OPENING_CONFIGURATION_FILE, count,
                           configuration);
      return ERROR_OPENING_CONFIGURATION_FILE;
    }

  if (in)
    {
      in >> input1;
      while (!in.eof())
        {
          if (in.fail())
            {
              cerr_rf(NON_NUMERIC_CHARACTER, count, configuration);
              return NON_NUMERIC_CHARACTER;
            }

          if (input1 < MIN_INDEX || input1 > MAX_INDEX)
            {
              cerr_rf(INVALID_INDEX, count, configuration);
              return INVALID_INDEX;
            }

          in >> input2;

          if (in.eof())
            break;

          if (in.fail())
            {
              cerr_rf(NON_NUMERIC_CHARACTER, count, configuration);
              return NON_NUMERIC_CHARACTER;
            }

          if (input2 < MIN_INDEX || input2 > MAX_INDEX)
            {
              cerr_rf(INVALID_INDEX, count, configuration);
              return INVALID_INDEX;
            }

          if (input1 == input2)
            {
              cerr_rf(INVALID_REFLECTOR_MAPPING, count,
                                   configuration);
              return INVALID_REFLECTOR_MAPPING; //check if letter is mapped
              //to itself
            }

          char letter1 = input1 + 'A';
          char letter2 = input2 + 'A';
          rf_mapping[input1]= letter2;
          rf_mapping[input2]= letter1; //store the values

          //check if letters are mapped to more than one other
          if (repetition(input1, letter2))
            {
              cerr_rf(INVALID_REFLECTOR_MAPPING, count,
                                   configuration);
              return INVALID_REFLECTOR_MAPPING;
            }

          if (repetition(input2, letter1))
            {
              cerr_rf(INVALID_REFLECTOR_MAPPING, count,
                                   configuration);
              return INVALID_REFLECTOR_MAPPING;
            }
                  
          in >> input1;
        }
    }
  in.close();

  in.open(configuration);

  if (in)
    {
      in >> input1;
      while (!in.eof())
        {
          count++;
          in >> input1;
        }
      if (count != 13*2)
        {
          cerr_rf(INCORRECT_NUMBER_OF_REFLECTOR_PARAMETERS, count,
                               configuration);
          return INCORRECT_NUMBER_OF_REFLECTOR_PARAMETERS;
        }
    }
  in.close();
  //End of validation

  return NO_ERROR;
}

bool Reflector::repetition(int index, char letter)
{
  //check the array for double occurreces,
  //but ignore the value of the index passed
  switch(index)
    {
    case MIN_INDEX:
      for (int i = index + 1; i <= MAX_INDEX; i++)
        if (rf_mapping[i] == letter)
          return true;
      break;
    case MAX_INDEX:
      for (int i = MIN_INDEX; i < index; i++)
        if (rf_mapping[i] == letter)
          return true;
      break;
    default:
      for (int i = MIN_INDEX; i < index; i++)
        if (rf_mapping[i] == letter)
          return true;
      for (int i = index + 1; i <= MAX_INDEX; i++)
        if (rf_mapping[i] == letter)
          return true;
    }
  return false;
}

char Reflector::rf_encrypt(char letter)
{
  int index = letter - 'A';
  letter = rf_mapping[index];
  return letter;
}

void Reflector::cerr_rf(int err, int count, char configuration[])
{
  switch(err)
    {
    case NO_ERROR:
      break;
    case INVALID_INDEX:
      std::cerr << "Invalid index in reflector file " << configuration << "\n";
      break;
    case INCORRECT_NUMBER_OF_REFLECTOR_PARAMETERS:
      if (count % 2)
        std::cerr << "Incorrect (odd) number of parameters in reflector file "
                  << configuration << "\n";
      if (count < 13*2 && !(count % 2))
        std::cerr << "Insufficient number of mappings in reflector file: "
                  << configuration << "\n";
      break;
    case INVALID_REFLECTOR_MAPPING:
      std::cerr << "Invalid mapping in reflector file " << configuration
                << "\n";
      break;
    case NON_NUMERIC_CHARACTER:
      std::cerr << "Non-numeric character in reflector file " << configuration
                << "\n";
      break;
    case ERROR_OPENING_CONFIGURATION_FILE:
      std::cerr << "Error opening reflector file " << configuration << "\n";
    }
}

int Reflector::get_rf_error()
{
  return errorcode;
}
