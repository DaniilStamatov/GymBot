#pragma once

#include <vector>

#include "domain/entities/workout.h"
namespace gymbot {
namespace domain {
class IWorkoutRepository {
 public:
  virtual ~IWorkoutRepository() = default;
  virtual int64_t SaveWorkout(const Workout& workout) = 0;
  virtual std::vector<Workout> GetWorkouts(int64_t user_id) = 0;
};
}  // namespace domain
}  // namespace gymbot