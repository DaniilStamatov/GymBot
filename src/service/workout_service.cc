#include "workout_service.h"

#include "domain/entities/workout.h"

namespace gymbot::service {
int64_t WorkoutService::SaveWorkout(const domain::Workout& workout) {
  return repo_->SaveWorkout(workout);
}

std::vector<domain::Workout> WorkoutService::GetUserWorkouts(int64_t user_id) {
  return repo_->GetWorkouts(user_id);
}

}  // namespace gymbot::service