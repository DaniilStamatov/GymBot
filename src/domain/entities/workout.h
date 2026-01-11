#pragma once
#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

namespace gymbot {
namespace domain {

struct WorkoutExercise {
  int64_t exercise_id_;
  std::string exercise_name_;
  int sets_;
  int reps_;
  int weight_;
};

struct Workout {
  int64_t id_ = 0;
  int64_t user_id_ = 0;
  std::string muscle_group_;
  std::string description_;
  std::vector<WorkoutExercise> exercises_;
  int64_t timestamp_ms_ = 0;

  Workout() : timestamp_ms_(now_ms()) {}

  Workout(int64_t user_id, std::string_view muscle_group,
          std::vector<WorkoutExercise> exercises, std::string_view description)
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
          std::vector<WorkoutExercise> exercises)
      : user_id_(user_id),
        muscle_group_(muscle_group),
        exercises_(std::move(exercises)),
        timestamp_ms_(now_ms()) {}
  void AddExercise(const WorkoutExercise& ex) { exercises_.push_back(ex); }
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
