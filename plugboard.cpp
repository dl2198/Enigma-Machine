#include <iostream>
#include <fstream>
#include "errors.h"
#include "enigma.h"

Plugboard::Plugboard(char configuration[])
{
  errorcode = setup(configuration);
}

void Plugboard::initialize_pb_mapping()
{
  char letter;
  for (int i = MIN_INDEX; i <= MAX_INDEX; i++)
    {
      letter = i + 'A';
      pb_mapping[i] = letter;
    }
}

int Plugboard::setup(char configuration[])
{
  initialize_pb_mapping();
  
  std::ifstream in;
  int input1;
  int input2;

  in.open(configuration);

  //Start of validation
  if (in.fail())
    {
      cerr_pb(ERROR_OPENING_CONFIGURATION_FILE, configuration);
      return ERROR_OPENING_CONFIGURATION_FILE;
    }

  if (in)
    {
      in >> input1;
      while (!in.eof())
        {
          if (in.fail())
            {
              cerr_pb(NON_NUMERIC_CHARACTER, configuration);
              return NON_NUMERIC_CHARACTER;
            }

          if (input1 < MIN_INDEX || input1 > MAX_INDEX)
            {
              cerr_pb(INVALID_INDEX, configuration);
              return INVALID_INDEX;
            }

          in >> input2;

          if (in.eof())
            break;

          if (in.fail())
            {
              cerr_pb(NON_NUMERIC_CHARACTER, configuration);
              return NON_NUMERIC_CHARACTER;
            }

          if (input2 < MIN_INDEX || input2 > MAX_INDEX)
            {
              cerr_pb(INVALID_INDEX, configuration);
              return INVALID_INDEX;
            }

          if (input1 == input2)
            {
              cerr_pb(IMPOSSIBLE_PLUGBOARD_CONFIGURATION, configuration);
              return IMPOSSIBLE_PLUGBOARD_CONFIGURATION; //check if a letter is mapped
              //to itself
            }

          char letter1 = input1 + 'A';
          char letter2 = input2 + 'A';
          pb_mapping[input1]= letter2;
          pb_mapping[input2]= letter1; //store the values

          //check if letters are mapped to more than one other
          if (repetition(input1, letter2))
            {
              cerr_pb(IMPOSSIBLE_PLUGBOARD_CONFIGURATION, configuration);
              return IMPOSSIBLE_PLUGBOARD_CONFIGURATION;
            }
          
          if (repetition(input2, letter1))
            {
              cerr_pb(IMPOSSIBLE_PLUGBOARD_CONFIGURATION, configuration);
              return IMPOSSIBLE_PLUGBOARD_CONFIGURATION;
            }
                  
          in >> input1;
        }
    }
  in.close();
  
  in.open(configuration);
  int count = 0;

  if (in)
    {
      in >> input1;
      while (!in.eof())
        {
          count++;
          in >> input1;
        }
      if (count % 2)
        {
          cerr_pb(INCORRECT_NUMBER_OF_PLUGBOARD_PARAMETERS, configuration);
          return INCORRECT_NUMBER_OF_PLUGBOARD_PARAMETERS;
        }
    }
  in.close();
  //End of validation

  return NO_ERROR;
}

bool Plugboard::repetition(int index, char letter)
{
  //check the array for double occurrences,
  //but ignore the value of the index passed
  switch(index)
    {
    case MIN_INDEX:
      for (int i = index + 1; i <= MAX_INDEX; i++)
        if (pb_mapping[i] == letter)
          return true;
      break;
    case MAX_INDEX:
      for (int i = MIN_INDEX; i < index; i++)
        if (pb_mapping[i] == letter)
          return true;
      break;
    default:
      for (int i = MIN_INDEX; i < index; i++)
        if (pb_mapping[i] == letter)
          return true;
      for (int i = index + 1; i <= MAX_INDEX; i++)
        if (pb_mapping[i] == letter)
          return true;
    }
  return false;
}

char Plugboard::pb_encrypt(char letter)
{
  int index = letter - 'A';
  letter = pb_mapping[index];
  return letter;
}

void Plugboard::cerr_pb(int err, char configuration[])
{
  switch(err)
    {
    case NO_ERROR:
      break;
    case INVALID_INDEX:
      std::cerr << "Invalid index in plugboard file " << configuration << "\n";
      break;
    case INCORRECT_NUMBER_OF_PLUGBOARD_PARAMETERS:
      std::cerr << "Incorrect number of parameters in plugboard file "
                << configuration << "\n";
      break;
    case IMPOSSIBLE_PLUGBOARD_CONFIGURATION:
      std::cerr << "Impossible configuration in plugboard file "
                << configuration << "\n";
      break;
    case NON_NUMERIC_CHARACTER:
      std::cerr << "Non-numeric character in plugboard file " << configuration
                << "\n";
      break;
    case ERROR_OPENING_CONFIGURATION_FILE:
      std::cerr << "Error opening plugboard file " << configuration << "\n";
    }
}

int Plugboard::get_pb_error()
{
  return errorcode;
}
