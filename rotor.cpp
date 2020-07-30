#include <iostream>
#include <fstream>
#include <string>
#include "errors.h"
#include "enigma.h"

Rotor::Rotor(char configuration[])
{
  errorcode = setup(configuration);
}

int Rotor::setup(char configuration[])
{
  int input_values[2*ALPHA_SIZE]; //values from config file
  initialize_rot_arrays(input_values);

  std::ifstream in;
  in.open(configuration);

  //Start of validation
  if (in.fail())
    {
      cerr_rot(ERROR_OPENING_CONFIGURATION_FILE, configuration);
      return ERROR_OPENING_CONFIGURATION_FILE;
    }

  if (in)
    {
      int count = 0;
      int input;
      in >> input;
      while (!in.eof())
        {
          if (in.fail())
            {
              cerr_rot(NON_NUMERIC_CHARACTER, configuration);
              return NON_NUMERIC_CHARACTER;
            }

          if (input < MIN_INDEX || input > MAX_INDEX)
            {
              cerr_rot(INVALID_INDEX, configuration);
              return INVALID_INDEX;
            }

          if (count < ALPHA_SIZE)
            for (int i = 0; i < ALPHA_SIZE; i++)
              if (input_values[i] == input)
                {
                  cerr_rot_map(count, input, i, configuration);
                  return INVALID_ROTOR_MAPPING; //check if letter is mapped
                  //already
                }

          input_values[count] = input; //store the value
          
          count++;
          in >> input;
        }
      count--;
      if (count < ALPHA_SIZE || count > 2*ALPHA_SIZE) //max 26 notches
        {
          cerr_rot_map(count, -1, -1, configuration);
          return INVALID_ROTOR_MAPPING;
        }
    }
  in.close();
  //End of validation

  //set up mappings
  for (int i = 0; i <= MAX_INDEX; i++)
    {
      fw_map[i] = (input_values[i] - i);
      bw_map[input_values[i]] = (i - input_values[i]);
    }

  //set up notches
  for (int i = ALPHA_SIZE; i < 2*ALPHA_SIZE; i++)
    if (input_values[i] != -1)
      notches[i-ALPHA_SIZE] = input_values[i];
  
  return NO_ERROR;
}

void Rotor::rotate()
{
  rotate_array(fw_map);
  rotate_array(bw_map);
  rotations++;
  rotations %= 26; //so that when we reach 27 rotations we start over from 1
  if (is_notch())
    if (left != nullptr)
      left->rotate();
}

void Rotor::rotate_array(int array[])
{
  //leftshift
  int first = array[MIN_INDEX];
  for (int i = MIN_INDEX; i <= MAX_INDEX; i++)
    array[i] = array[i+1];
  array[MAX_INDEX] = first;
}

void Rotor::initialize_rot_arrays(int input_values[])
{
  for (int i = MIN_INDEX; i <= 2*ALPHA_SIZE; i++)
    input_values[i] = -1; //null input
  for (int i = MIN_INDEX; i < ALPHA_SIZE; i++)
    {
      fw_map[i] = i;
      bw_map[i] = i;
      notches[i] = -1; //null notches
    }
}

bool Rotor::is_notch()
{
  for (int i = 0; i <= MAX_INDEX; i++)
    if (rotations == notches[i])
      return true;
  return false;
}

void Rotor::start()
{
  for (int rotations_count = 0; rotations_count < starting_position;
       rotations_count++)
    rotate();
}

char Rotor::rot_fw_encrypt(char letter)
{
  int index = letter - 'A';
  //works for both positive and negative offsets
  index = (ALPHA_SIZE + ((index + fw_map[index])%ALPHA_SIZE))%ALPHA_SIZE;
  letter = index + 'A';
  return letter;
}

char Rotor::rot_bw_encrypt(char letter)
{
  int index = letter - 'A';
  index = (ALPHA_SIZE + ((index + bw_map[index])%ALPHA_SIZE))%ALPHA_SIZE;
  letter = index + 'A';
  return letter;
}

void Rotor::cerr_rot(int err, char configuration[])
{
  switch(err)
    {
    case NO_ERROR:
      break;
    case ERROR_OPENING_CONFIGURATION_FILE:
      std::cerr << "Error opening rotor file" << configuration
                << "\n";
      break;
    case NON_NUMERIC_CHARACTER:
      std::cerr << "Non-numeric character for mapping in rotor file "
                << configuration << "\n";
      break;
    case INVALID_INDEX:
      std::cerr << "Invalid index in rotor file" << configuration
                << "\n";
    }
}

void Rotor::cerr_rot_map(int count, int output1, int output2,
                                 char configuration[])
{
  if (count < ALPHA_SIZE)
    {
      if (output1 != -1 && output2 != -1)
        {
          std::cerr << "Invalid mapping of input " << count << " to output "
                    << output1 << " (output " << output1 << " is already mapped"
                    << " to from input " << output2 << ") in rotor file: "
                    << configuration << "\n";
        }
      else {
        std::cerr << "Not all inputs mapped in rotor file: " << configuration
                  << "\n";
      }
    }
  if (count > 2*ALPHA_SIZE)
    std::cerr << "Too many notches in rotor file " << configuration << "\n";
}

int Rotor::get_rot_error()
{
  return errorcode;
}
