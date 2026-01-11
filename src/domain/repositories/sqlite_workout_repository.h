#pragma once

#include <cstdint>
#include <memory>

#include "domain/entities/exercise.h"
#include "domain/entities/workout.h"
#include "i_workout_repository.h"
namespace gymbot::infra {

class SqliteWorkoutRepository : public domain::IWorkoutRepository {
 public:
  SqliteWorkoutRepository(const std::string& db_path = ":memory");
  ~SqliteWorkoutRepository();
  int64_t SaveWorkout(const domain::Workout& workout) override;

  std::vector<domain::Workout> GetWorkouts(int64_t user_id) override;
  int64_t CreateExercise(const domain::Exercise& exercise) override;
  std::optional<domain::Exercise> FindExerciseByName(
      const std::string& name) override;
  std::optional<domain::Exercise> GetExercise(int64_t id) override;

  std::vector<domain::Exercise> GetAllExercises() override;

 private:
  class PImpl;
  std::unique_ptr<PImpl> impl_;
};
}  // namespace gymbot::infra