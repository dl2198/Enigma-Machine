#include "enigma.h"
#include "errors.h"

int main(int argc, char** argv)
{

  Enigma enigma(argc, argv);

  return enigma.get_enigma_error();
}
