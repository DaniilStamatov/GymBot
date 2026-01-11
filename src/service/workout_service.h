#pragma once

#include <memory>
#include <vector>

#include "domain/repositories/i_workout_repository.h"

namespace gymbot::service {
class WorkoutService {
 public:
  explicit WorkoutService(std::unique_ptr<domain::IWorkoutRepository> repo);

  int64_t SaveWorkout(const domain::Workout& workout);
  std::vector<domain::Workout> GetUserWorkouts(int64_t user_id);

  int64_t CreateExercise(const domain::Exercise& exercise);
  std::optional<domain::Exercise> FindExercise(const std::string& name);

 private:
  std::unique_ptr<domain::IWorkoutRepository> repository_;
};
}  // namespace gymbot::service