#include "../include/word.h"

uint16_t wordTo16(Word *word) { return word->low | (word->high << 8); }

Word Word::operator+(int other) {
  return Word(((static_cast<uint16_t>(high) << 8) | low) + other);
}

Word Word::operator-(int other) {
  return Word(((static_cast<uint16_t>(high) << 8) | low) - other);
}

void Word::operator+=(int other) {
  auto newThis = *this + other;
  *this = newThis;
}
