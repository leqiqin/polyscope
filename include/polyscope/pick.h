#pragma once

#include "polyscope/structure.h"

#include "geometrycentral/vector3.h"

#include <cstdint>

namespace polyscope {
namespace pick {


// The currently selected index from the pick buffer, if one is selected.
extern size_t currPickInd;
extern bool haveSelection;
extern bool pickWasDoubleClick;

// Request 'count' contiguous indices for drawing a pick buffer. The return value is the start of the range.
size_t requestPickBufferRange(Structure* requestingStructure, size_t count);

// Get the currently pick index, and the structure that index is mapped to.
// Index is returned in "local" indices for that structure, such that '0' corresponds to the start of the range that was
// retned by requestPickBufferRange().
// If nothing is selected, returns nullptr and max_int.
Structure* getCurrentPickElement(size_t& localInd);

void setCurrentPickElement(size_t pickInd, bool wasDoubleClick);

// If something from this structure is selected, clear the selection (useful if a structure is being deleted)
void clearPickIfStructureSelected(Structure* s);

// Clear out picking related data
void resetPick();

// Constant for bit-bashing functions below
const int bitsForPickPacking = 22;
// const int bitsForPickPacking = 7; // useful for testing

// Convert indices to color and back
inline geometrycentral::Vector3 indToVec(size_t ind) {

  // Can comfortably fit a 22 bit integer exactly in a single precision float
  size_t factor = 1 << bitsForPickPacking;
  size_t mask = factor - 1;
  double factorF = factor;

  size_t low = ind & mask;
  ind = ind >> bitsForPickPacking;
  size_t med = ind & mask;
  ind = ind >> bitsForPickPacking;
  size_t high = ind;

  return geometrycentral::Vector3{static_cast<double>(low) / factorF, static_cast<double>(med) / factorF,
                                  static_cast<double>(high) / factorF};
}
inline size_t vecToInd(geometrycentral::Vector3 vec) {

  size_t factor = 1 << bitsForPickPacking;
  double factorF = factor;

  size_t low = static_cast<size_t>(factorF * vec.x);
  size_t med = static_cast<size_t>(factorF * vec.y);
  size_t high = static_cast<size_t>(factorF * vec.z);

  // Debug check
  if (low != (factorF * vec.x) || med != (factorF * vec.y) || high != (factorF * vec.z)) {
    throw std::logic_error("Float to index conversion failed, bad value in float.");
  }

  size_t ind = (high << (2 * bitsForPickPacking)) + (med << bitsForPickPacking) + low;
  return ind;
}

} // namespace pick
} // namespace polyscope