#include "workout_service.h"

#include "domain/entities/workout.h"

namespace gymbot::service {
WorkoutService::WorkoutService(std::unique_ptr<domain::IWorkoutRepository> repo)
    : repository_(std::move(repo)) {}

int64_t WorkoutService::SaveWorkout(const domain::Workout& workout) {
  return repository_->SaveWorkout(workout);
}

std::vector<domain::Workout> WorkoutService::GetUserWorkouts(int64_t user_id) {
  return repository_->GetWorkouts(user_id);
}

int64_t WorkoutService::CreateExercise(const domain::Exercise& exercise) {
  return repository_->CreateExercise(exercise);
}
std::optional<domain::Exercise> WorkoutService::FindExercise(
    const std::string& name) {
  return repository_->FindExerciseByName(name);
}

}  // namespace gymbot::service