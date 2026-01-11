#pragma once
#include <cstdint>
#include <string>
namespace gymbot::domain {
enum ExerciseType { kStrengthWeighed = 0, kBodyWeight = 1, kCardio = 2 };

struct Exercise {
  int64_t id;
  std::string name;
  std::string muscle_group;
  ExerciseType type;
  std::string description;
};
}  // namespace gymbot::domain