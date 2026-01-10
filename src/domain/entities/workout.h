#pragma once
#include <string>
namespace gymbot {
namespace domain {
struct Workout {
  int64_t id_ = 0;
  int64_t user_id_ = 0;
  std::string muscle_group_;

  Workout() = default;

  Workout(int64_t user_id, std::string_view muscle_group)
      : user_id_(user_id), muscle_group_(muscle_group) {}
  static Workout FromDto(int64_t user_id, std::string_view muscle_group) {
    Workout w{user_id, muscle_group};
    return w;
  }
};
}  // namespace domain
}  // namespace gymbot
