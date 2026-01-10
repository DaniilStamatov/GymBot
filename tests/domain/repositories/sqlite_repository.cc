#include <gtest/gtest.h>

#include "domain/repositories/sqlite_workout_repository.h"
using namespace gymbot;

class SqliteWorkoutRepoTest : public ::testing::Test {
 protected:
  static constexpr auto DB_PATH = ":memory:";

  std::unique_ptr<gymbot::infra::SqliteWorkoutRepository> repo;
  gymbot::domain::Workout make_test_workout(int64_t user_id) {
    gymbot::domain::Workout workout;
    workout.user_id_ = user_id;
    workout.muscle_group_ = "Chest";
    workout.description_ = "Bench Press Day";
    workout.timestamp_ms_ = 1640995200000;

    workout.exercises_ = {
        domain::Exercise{"Bench Press", 4, 8, domain::ExerciseType::kBodyWeight,
                         80},
        domain::Exercise{"Incline DB", 3, 10,
                         domain::ExerciseType::kStrengthWeighed, 25},
        domain::Exercise{"Cable Fly", 3, 12, domain::ExerciseType::kCardio,
                         20}};
    return workout;
  }

  void SetUp() override {
    repo = std::make_unique<gymbot::infra::SqliteWorkoutRepository>(DB_PATH);
  }
};

TEST_F(SqliteWorkoutRepoTest, SaveWorkout_CreatesRecord) {
  auto workout = make_test_workout(123);

  auto id = repo->SaveWorkout(workout);

  EXPECT_GT(id, 0);
}

TEST_F(SqliteWorkoutRepoTest, SaveWorkout_WithExercises_CreatesAllRecords) {
  auto workout = make_test_workout(123);

  auto workout_id = repo->SaveWorkout(workout);

  auto all_workouts = repo->GetWorkouts(123);
  ASSERT_EQ(all_workouts.size(), 1);

  const auto& saved = all_workouts[0];
  EXPECT_EQ(saved.exercises_.size(), 3);
  EXPECT_EQ(saved.exercises_[0].name_, "Bench Press");
  EXPECT_EQ(saved.exercises_[0].sets_, 4);
  EXPECT_EQ(saved.exercises_[0].weight_, 80);
}

TEST_F(SqliteWorkoutRepoTest, GetWorkouts_ReturnsCorrectUserWorkouts) {
  // 3 разных юзера
  repo->SaveWorkout(make_test_workout(1));
  repo->SaveWorkout(make_test_workout(2));
  repo->SaveWorkout(make_test_workout(1));  // Тот же юзер

  auto user1_workouts = repo->GetWorkouts(1);
  auto user2_workouts = repo->GetWorkouts(2);

  EXPECT_EQ(user1_workouts.size(), 2);
  EXPECT_EQ(user2_workouts.size(), 1);
}

TEST_F(SqliteWorkoutRepoTest, TransactionRollback_OnExerciseError) {
  // TODO: тест на rollback при ошибке в упражнениях
  // Нужно мокать sqlite3_step чтобы вернуть ошибку
}

TEST_F(SqliteWorkoutRepoTest, MultipleWorkouts_SavesCorrectly) {
  auto workout1 = make_test_workout(123);
  workout1.muscle_group_ = "Back";

  auto workout2 = make_test_workout(123);
  workout2.muscle_group_ = "Legs";

  repo->SaveWorkout(workout1);
  repo->SaveWorkout(workout2);

  auto workouts = repo->GetWorkouts(123);
  ASSERT_EQ(workouts.size(), 2);

  // Проверяем что группы разные
  std::set<std::string> groups;
  for (const auto& w : workouts) {
    groups.insert(w.muscle_group_);
  }
  EXPECT_EQ(groups.size(), 2);
}

TEST_F(SqliteWorkoutRepoTest, EmptyExercises_SavesCorrectly) {
  gymbot::domain::Workout empty_workout{123, "Cardio"};

  auto id = repo->SaveWorkout(empty_workout);
  EXPECT_GT(id, 0);
}
