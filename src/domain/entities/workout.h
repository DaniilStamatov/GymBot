#pragma once
#include <chrono>
#include <cstdint>
#include <string>
#include <vector>
namespace gymbot {
namespace domain {
enum ExerciseType { kStrengthWeighed = 0, kBodyWeight = 1, kCardio = 2 };

struct Exercise {
  std::string name_;
  uint32_t sets = 0;
  uint32_t reps = 0;
  ExerciseType type_ = kStrengthWeighed;
  uint32_t weight_;
};

struct Workout {
  int64_t id_ = 0;
  int64_t user_id_ = 0;
  std::string muscle_group_;
  std::string description_;
  std::vector<Exercise> exercises_;
  int64_t timestamp_ms_ = 0;

  Workout() : timestamp_ms_(now_ms()) {}

  Workout(int64_t user_id, std::string_view muscle_group,
          std::vector<Exercise> exercises, std::string_view description)
      : user_id_(user_id),
        muscle_group_(muscle_group),
        description_(description),
        exercises_(std::move(exercises)),
        timestamp_ms_(now_ms()) {}

  Workout(int64_t user_id, std::string_view muscle_group)
      : user_id_(user_id),
        muscle_group_(muscle_group),
        timestamp_ms_(now_ms()) {}

  Workout(int64_t user_id, std::string_view muscle_group,
          std::vector<Exercise> exercises)
      : user_id_(user_id),
        muscle_group_(muscle_group),
        exercises_(std::move(exercises)),
        timestamp_ms_(now_ms()) {}

  static int64_t now_ms() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::system_clock::now().time_since_epoch())
        .count();
  }

  static Workout FromDto(int64_t user_id, std::string_view muscle_group) {
    return {user_id, muscle_group};
  }
};
}  // namespace domain
}  // namespace gymbot
