#ifndef ENIGMA_H
#define ENIGMA_H
#include <fstream>
#include "errors.h"

//global constants for configuration arrays
int const ALPHA_SIZE = 26;
int const MIN_INDEX = ALPHA_SIZE - ALPHA_SIZE;
int const MAX_INDEX = ALPHA_SIZE - 1;

class Plugboard {

  int errorcode;

  //array of characters mapping letters to other letters
  //letters are represented as indexes 0-25
  //for each index (corresponding to a letter) there is a character
  //which corresponds to the letter it is mapped to
  char pb_mapping[ALPHA_SIZE];

  //function to set up plugboard mapping
  //configuration[] is mapping file
  //returns errorcode
  int setup(char configuration[]);

  //helper function to initialize mapping to trivial configuration
  void initialize_pb_mapping();

  //function to check for invalid config
  //index is index in mapping array
  //letter is letter being checked for double occurrence
  //returns true if a letter appears twice in mapping, false otherwise
  bool repetition(int index, char letter);

  //function for plug errors
  //err is errorcode used to print informative message to errorstream
  void cerr_pb(int err, char configuration[]);

 public:
  Plugboard(char configuration[]);

  //function to encrypt a letter
  //letter is letter to encrypt
  //returns encrypted letter
  char pb_encrypt(char letter);

  //getter function for errorcode
  int get_pb_error();
  
};

class Reflector {

  int errorcode;

  //array of characters mapping letters to other letters
  //letters are represented as indexes 0-25
  //for each index (corresponding to a letter) there is a character
  //which corresponds to the letter it is mapped to
  char rf_mapping[ALPHA_SIZE];

  //function to set up reflector mapping
  //configuration[] is mapping file
  //returns errorcode
  int setup(char configuration[]);

  //helper function to initialize mapping to trivial configuration
  void initialize_rf_mapping();

  //function to check for invalid config
  //index is index in mapping array
  //letter is letter being checked for double occurrence
  //returns true if a letter appears twice in mapping, false otherwise
  bool repetition(int index, char letter);

  //function for refl errors
  //err is errorcode used to print informative message to errorstream
  //count is number of input values from configuration file used
  //to return error message accordingly
  void cerr_rf(int err, int count, char configuration[]);

 public:
  
  Reflector(char configuration[]);

  //function to encrypt a letter
  //letter is letter to encrypt
  //returns encrypted letter
  char rf_encrypt(char letter);

  //getter function for errorcode
  int get_rf_error();
  
};

class Rotor {
  
  int errorcode;

  //arrays of integers mapping letters to other letters
  //letters are represented as indexes 0-25
  //for each index (corresponding to a letter) there is an integer
  //which corresponds to the offset of the mapping: to obtain the
  //output letter, it is sufficient to add the integer to the index
  int fw_map[ALPHA_SIZE];
  int bw_map[ALPHA_SIZE];

  //array of integers representing notches
  //there can be max 26 notches
  //a null notch is represented by a -1
  //letters are represented as indexes 0-25
  int notches[ALPHA_SIZE];

  //counter of rotations used to check whether a notch is reached
  int rotations;

  //function to set up rotor mappings
  //configuration[] is mapping file
  //returns errorcode
  int setup(char configuration[]);

  //helper function to initialize mapping to trivial configuration
  //input_value[] is storage for values from config file
  void initialize_rot_arrays(int input_value[]);

  //helper function to rotate the rotor
  //array[] the array to be rotated
  void rotate_array(int array[]);

 public:
  
  Rotor(char configuration[]);
  Rotor* left;
  Rotor* right;
  int starting_position;

  //function to rotate the rotor
  void rotate();

  //functions to encrypt a character using fw and bw mappings
  //letter is letter to encrypt
  //return encrypted letter
  char rot_fw_encrypt(char letter);
  char rot_bw_encrypt(char letter);

  //function to check if a notch is at the top position
  bool is_notch();

  //function to position the rotor to its starting position
  void start();

  //functions for rot err
  //err is errorcode used to print informative message to errorstream
  //count, output1 and output2 are used to print specific mapping error messages
  void cerr_rot(int err, char configuration[]);
  void cerr_rot_map(int count, int output1, int output2, char configuration[]);

  //getter function for errorcode
  int get_rot_error();
  
};

class RotorList {
  
 public:

  RotorList();

  Rotor* first;
  Rotor* last;
    
  //function to add a new rotor to the list
  //new_rotor_ptr is pointer to the new rotor to be added
  void append_rotor(Rotor* new_rotor_ptr);

  //function to find the leftmost and rightmost rotor
  Rotor* find_leftmost();
  Rotor* find_rightmost();

};

class Enigma {
  
  int errorcode;
  
  //number of rotors required by command
  int n_rotors = 0;
  
  //number of successfully created rotors
  int existing_rotors = 0;
  
  Plugboard* pb_ptr = nullptr;
  Reflector* rf_ptr = nullptr;
  RotorList* rot_ptr;

  //function to set up enigma components
  //argc is argument counter
  //argv is pointer to c-style strings containing name of config files
  //returns errorcode
  int setup(int argc, char** argv);

  //function to encrypt a message letter by letter
  //letter is letter to encrypt
  //returns encrypted letter
  char encrypt(char letter);

  //function to rotate the rotors when a key is pressed
  void keypress();

  //functions to return errorcodes to main prog
  //err is errorcode used to print informative messages to errorstream
  //rotor is rotor without starting position 
  void cerr_startpos(int err, int rotor, char configuration[]);
  void cerr_enigma();

 public:
  
  Enigma(int argc, char** argv);
  ~Enigma();

  //function to encrypt a message from std input stream
  int encrypt_message();
  
  //getter function for errorcode
  int get_enigma_error();
  
};

#endif
