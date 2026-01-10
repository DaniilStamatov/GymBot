#pragma once

#include <cstdint>
#include <memory>

#include "domain/entities/workout.h"
#include "i_workout_repository.h"
namespace gymbot::infra {

class SqliteWorkoutRepository : public domain::IWorkoutRepository {
 public:
  SqliteWorkoutRepository();
  ~SqliteWorkoutRepository();
  int64_t SaveWorkout(const domain::Workout& workout) override;

  std::vector<domain::Workout> GetWorkouts(int64_t user_id) override;

 private:
  class PImpl;
  std::unique_ptr<PImpl> impl_;
};
}  // namespace gymbot::infra