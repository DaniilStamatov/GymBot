#include <gtest/gtest.h>

#include "domain/repositories/sqlite_workout_repository.h"
using namespace gymbot;

class SqliteWorkoutRepositoryTest : public ::testing::Test {
 protected:
  gymbot::infra::SqliteWorkoutRepository repo;
};
TEST_F(SqliteWorkoutRepositoryTest, SaveWorkout_ReturnsId) {
  domain::Workout workout{0, "chest"};

  int64_t id = repo.SaveWorkout(workout);
  EXPECT_EQ(id, 1);
}

TEST_F(SqliteWorkoutRepositoryTest, SaveMultipleWorkouts_IncrementsId) {
  domain::Workout workout1{0, "chest"};
  domain::Workout workout2{0, "legs"};

  int64_t id1 = repo.SaveWorkout(workout1);
  int64_t id2 = repo.SaveWorkout(workout2);

  EXPECT_EQ(id2, id1 + 1);
}

TEST_F(SqliteWorkoutRepositoryTest, SaveWorkout_NegativeUserId_StillWorks) {
  domain::Workout workout{-1, "back"};

  int64_t id = repo.SaveWorkout(workout);
  EXPECT_GT(id, 0);
}

TEST_F(SqliteWorkoutRepositoryTest, SaveWorkout_EmptyMuscleGroup_StillWorks) {
  domain::Workout workout{0, ""};

  int64_t id = repo.SaveWorkout(workout);
  EXPECT_GT(id, 0);
}

TEST_F(SqliteWorkoutRepositoryTest, GetWorkouts_FiltersByUserId) {
  repo.SaveWorkout({1, "chest"});
  repo.SaveWorkout({2, "legs"});
  repo.SaveWorkout({1, "back"});

  auto workouts = repo.GetWorkouts(1);

  EXPECT_EQ(workouts.size(), 2);
  EXPECT_EQ(workouts[0].muscle_group_, "chest");
  EXPECT_EQ(workouts[1].muscle_group_, "back");
}
