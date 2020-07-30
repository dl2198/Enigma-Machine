#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include "enigma.h"
#include "errors.h"

Enigma::Enigma(int argc, char** argv)
{
  errorcode = setup(argc, argv);
  cerr_enigma();
  if (errorcode == NO_ERROR)
    {
      errorcode = encrypt_message();
      cerr_enigma();
    }
}

int Enigma::setup(int argc, char** argv)
{
  if (argc < 4)
    return INSUFFICIENT_NUMBER_OF_PARAMETERS;

  //create plugboard, check errors
  pb_ptr = new Plugboard(argv[1]);
  if (pb_ptr->get_pb_error() != NO_ERROR)
    return pb_ptr->get_pb_error();

  //create reflector, check errors
  rf_ptr = new Reflector(argv[2]);
  if (rf_ptr->get_rf_error() != NO_ERROR)
    return rf_ptr->get_rf_error();

  std::ifstream in;

  n_rotors = argc - 4;

  //Start of validation for starting position
  in.open(argv[argc-1]);

  int count = 0;
  int starting_position;

  if (in.fail())
    {
      cerr_startpos(ERROR_OPENING_CONFIGURATION_FILE, count, argv[argc-1]);
      return ERROR_OPENING_CONFIGURATION_FILE;
    }
  
  if (in)
    {
      in >> starting_position;
      while (!in.eof())
        {
          if (in.fail())
            {
              cerr_startpos(NON_NUMERIC_CHARACTER, count, argv[argc-1]);
              return NON_NUMERIC_CHARACTER;
            }

          count++;
          in >> starting_position;
        }

      if (count < n_rotors)
        {
          cerr_startpos(NO_ROTOR_STARTING_POSITION, count, argv[argc-1]);
          return NO_ROTOR_STARTING_POSITION;
        }

    }
  in.close();
  //End of validation

  //Create rotors, add to list, check for errors
  if (n_rotors > 0)
    {
      rot_ptr = new RotorList();
      for (int count = 0; count < n_rotors; count++)
        {
          Rotor* new_rot_ptr;
          new_rot_ptr = new Rotor(argv[count+3]);
          if (new_rot_ptr->get_rot_error() != NO_ERROR)
            return new_rot_ptr->get_rot_error();
          rot_ptr->append_rotor(new_rot_ptr);
          existing_rotors++;
        }
      //Set rotors to start position
      in.open(argv[argc-1]);
      
      if (in)
        {
          in >> starting_position;
          Rotor* current = rot_ptr->find_leftmost();
          current->starting_position = starting_position;
          current->start();
          while (current->right != nullptr)
            {
              current = current->right;
              in >> starting_position;
              current->starting_position = starting_position;
              current->start();
            }
        }
      in.close();
    }

  return NO_ERROR;          
}

int Enigma::encrypt_message()
{
  std::string message;

  std::getline (std::cin,message);

  //Strip message of any whitespace
  message.erase(std::remove(message.begin(), message.end(), ' '),
                message.end());
  message.erase(std::remove(message.begin(), message.end(), '\n'),
                message.end());
  message.erase(std::remove(message.begin(), message.end(), '\r'),
                message.end());
  message.erase(std::remove(message.begin(), message.end(), '\t'),
                message.end());
  message.erase(std::remove(message.begin(), message.end(), '\v'),
                message.end());
  message.erase(std::remove(message.begin(), message.end(), '\f'),
                message.end());

  for (char& letter : message)
    {
      if (letter < 'A' || letter > 'Z')
        {
          std::cerr << letter;
          return INVALID_INPUT_CHARACTER;
        }
      letter = encrypt(letter);
      std::cout << letter;
    }

  return NO_ERROR;
}

char Enigma::encrypt(char letter)
{
  keypress();

  letter = pb_ptr->pb_encrypt(letter);

  if (n_rotors > 0)
    {
      Rotor* current = rot_ptr->find_rightmost();

      while (current != nullptr)
        {
          letter = current->rot_fw_encrypt(letter);
          current = current->left;
        }

      letter = rf_ptr->rf_encrypt(letter);

      current = rot_ptr->find_leftmost();

      while (current != nullptr)
        {
          letter = current->rot_bw_encrypt(letter);
          current = current->right;
        }

      letter = pb_ptr->pb_encrypt(letter);

      return letter;
    }
  
  letter = rf_ptr->rf_encrypt(letter);

  letter = pb_ptr->pb_encrypt(letter);

  return letter;
}

void Enigma::keypress()
{
  if (n_rotors > 0)
    {
      Rotor* current = rot_ptr->find_rightmost();
      current->rotate();
    }
}

void Enigma::cerr_startpos(int err, int rotor, char configuration[])
{
  switch(err)
    {
    case NO_ERROR:
      break;
    case ERROR_OPENING_CONFIGURATION_FILE:
      std::cerr << "Error opening rotor position file" << configuration << "\n";
      break;
    case NON_NUMERIC_CHARACTER:
      std::cerr << "Non-numeric character in rotor positions file "
                << configuration << "\n";
      break;
    case INVALID_INDEX:
      std::cerr << "Invalid index in rotor position file " << configuration
                << "\n";
      break;
    case NO_ROTOR_STARTING_POSITION:
      std::cerr << "No starting position for rotor " << rotor << " in rotor p"
                << "osition file: " << configuration << "\n";
    }
}

void Enigma::cerr_enigma()
{
  switch(errorcode)
    {
    default:
      break;
    case INSUFFICIENT_NUMBER_OF_PARAMETERS:
      std::cerr << "usage: enigma plugboard-file reflector-file (<rotor-file>)"
                << "* " << "rotor-positions\n";
      break;
    case INVALID_INPUT_CHARACTER:
      std::cerr << " is not a valid input character (input characters must be u"
                << "pper case letters A-Z)!\n";
    } 
}

int Enigma::get_enigma_error()
{
  return errorcode;
}

Enigma::~Enigma()
{
  if (pb_ptr != nullptr)
    delete pb_ptr;

  if (rf_ptr != nullptr)
    delete rf_ptr;

  if (existing_rotors > 0)
    {
      Rotor* current = rot_ptr->find_leftmost();
      while (current->right != nullptr)
        {
          current = current->right;
          delete current->left;
        }
      delete current;
      delete rot_ptr;
    }
}
