#include "enigma.h"

RotorList::RotorList()
{
  first = nullptr;
  last = nullptr;
}

void RotorList::append_rotor(Rotor* new_rotor_ptr)
{
  new_rotor_ptr->right = nullptr;

  if (first == nullptr)
    {
      new_rotor_ptr->left = nullptr;
      first = new_rotor_ptr;
      return;
    }

  last = find_rightmost();

  last->right = new_rotor_ptr;
  new_rotor_ptr->left = last;
}

Rotor* RotorList::find_leftmost()
{
  Rotor* current = first;

  while (current->left != nullptr)
    current = current->left;

  return current;
}

Rotor* RotorList::find_rightmost()
{
  Rotor* current = first;

  while (current->right != nullptr)
    current = current->right;

  return current;
}
