#pragma once
#include <memory>

#include "domain/repositories/i_workout_repository.h"
namespace gymbot {
namespace service {
class WorkoutService {
 public:
  explicit WorkoutService(
      std::unique_ptr<domain::IWorkoutRepository> repository)
      : repo_(std::move(repository)) {}
  int64_t SaveWorkout(const domain::Workout& workout);
  std::vector<domain::Workout> GetUserWorkouts(int64_t user_id);

 private:
  std::unique_ptr<domain::IWorkoutRepository> repo_;
};
}  // namespace service
}  // namespace gymbot
