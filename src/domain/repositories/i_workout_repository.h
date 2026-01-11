#pragma once

#include <vector>

#include "domain/entities/exercise.h"
#include "domain/entities/workout.h"

namespace gymbot {
namespace domain {
class IWorkoutRepository {
 public:
  virtual ~IWorkoutRepository() = default;
  virtual int64_t SaveWorkout(const Workout& workout) = 0;
  virtual std::vector<Workout> GetWorkouts(int64_t user_id) = 0;
  virtual int64_t CreateExercise(const domain::Exercise& exercise) = 0;
  virtual std::optional<domain::Exercise> GetExercise(int64_t id) = 0;
  virtual std::optional<domain::Exercise> FindExerciseByName(
      const std::string& name) = 0;
  virtual std::vector<domain::Exercise> GetAllExercises() = 0;
};
}  // namespace domain
}  // namespace gymbot