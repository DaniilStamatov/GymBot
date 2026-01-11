#include <gtest/gtest.h>

#include "domain/entities/exercise.h"
#include "domain/entities/workout.h"
#include "domain/repositories/sqlite_workout_repository.h"

using namespace gymbot;

class SqliteWorkoutRepoTest : public ::testing::Test {
 protected:
  static constexpr auto DB_PATH = ":memory:";
  std::unique_ptr<gymbot::infra::SqliteWorkoutRepository> repo;

  void SetUp() override {
    repo = std::make_unique<gymbot::infra::SqliteWorkoutRepository>(DB_PATH);
  }

  domain::Exercise create_test_exercise(
      const std::string& name = "Bench Press",
      const std::string& muscle_group = "Chest",
      domain::ExerciseType type = domain::ExerciseType::kStrengthWeighed) {
    domain::Exercise ex;
    ex.name = name;
    ex.muscle_group = muscle_group;
    ex.type = type;
    ex.description = "Test exercise: " + name;
    return ex;
  }

  domain::Workout create_test_workout(
      int64_t user_id, const std::vector<int64_t>& exercise_ids = {}) {
    domain::Workout workout;
    workout.user_id_ = user_id;
    workout.muscle_group_ = "Test Group";
    workout.description_ = "Test workout for user " + std::to_string(user_id);

    for (size_t i = 0; i < exercise_ids.size(); ++i) {
      domain::WorkoutExercise wex;
      wex.exercise_id_ = exercise_ids[i];
      wex.sets_ = 3 + i;
      wex.reps_ = 8 + i;
      wex.weight_ = 50 + i * 10;
      workout.exercises_.push_back(wex);
    }

    return workout;
  }
};

// ==================== Exercise Tests ====================

TEST_F(SqliteWorkoutRepoTest, CreateExercise_ReturnsValidId) {
  auto ex = create_test_exercise();

  int64_t id = repo->CreateExercise(ex);

  EXPECT_GT(id, 0) << "Exercise ID should be positive";
}

TEST_F(SqliteWorkoutRepoTest, CreateExercise_Duplicate_ReturnsSameId) {
  auto ex = create_test_exercise("Squat", "Legs");

  int64_t id1 = repo->CreateExercise(ex);
  int64_t id2 = repo->CreateExercise(ex);

  EXPECT_EQ(id1, id2) << "Duplicate exercise should return same ID";
}

TEST_F(SqliteWorkoutRepoTest, FindExerciseByName_FindsCreatedExercise) {
  auto ex = create_test_exercise("Deadlift", "Back");
  int64_t created_id = repo->CreateExercise(ex);

  auto found = repo->FindExerciseByName("Deadlift");

  ASSERT_TRUE(found.has_value()) << "Should find exercise by name";
  EXPECT_EQ(found->id, created_id);
  EXPECT_EQ(found->name, "Deadlift");
  EXPECT_EQ(found->muscle_group, "Back");
  EXPECT_EQ(found->type, domain::ExerciseType::kStrengthWeighed);
}

TEST_F(SqliteWorkoutRepoTest, FindExerciseByName_NotFound_ReturnsNullopt) {
  auto found = repo->FindExerciseByName("NonExistentExercise");

  EXPECT_FALSE(found.has_value()) << "Should not find non-existent exercise";
}

TEST_F(SqliteWorkoutRepoTest, GetExercise_FindsById) {
  auto ex = create_test_exercise("Pull-ups", "Back",
                                 domain::ExerciseType::kBodyWeight);
  int64_t id = repo->CreateExercise(ex);

  auto found = repo->GetExercise(id);

  ASSERT_TRUE(found.has_value());
  EXPECT_EQ(found->name, "Pull-ups");
  EXPECT_EQ(found->type, domain::ExerciseType::kBodyWeight);
}

TEST_F(SqliteWorkoutRepoTest, GetExercise_InvalidId_ReturnsNullopt) {
  auto found = repo->GetExercise(999999);

  EXPECT_FALSE(found.has_value());
}

TEST_F(SqliteWorkoutRepoTest, GetAllExercises_ReturnsAll) {
  repo->CreateExercise(create_test_exercise("Exercise1", "Group1"));
  repo->CreateExercise(create_test_exercise("Exercise2", "Group1"));
  repo->CreateExercise(create_test_exercise("Exercise3", "Group2"));

  auto all = repo->GetAllExercises();

  EXPECT_GE(all.size(), 3) << "Should return all created exercises";

  if (all.size() >= 3) {
    EXPECT_TRUE(all[0].name <= all[1].name);
    EXPECT_TRUE(all[1].name <= all[2].name);
  }
}

// ==================== Workout Tests ====================

TEST_F(SqliteWorkoutRepoTest, SaveWorkout_EmptyExercises_SavesSuccessfully) {
  auto workout = create_test_workout(100);

  int64_t workout_id = repo->SaveWorkout(workout);

  EXPECT_GT(workout_id, 0);
}

TEST_F(SqliteWorkoutRepoTest, SaveWorkout_WithExercises_SavesAll) {
  auto ex1 = create_test_exercise("Bench Press", "Chest");
  auto ex2 = create_test_exercise("Squat", "Legs");

  int64_t ex1_id = repo->CreateExercise(ex1);
  int64_t ex2_id = repo->CreateExercise(ex2);
  auto workout = create_test_workout(200, {ex1_id, ex2_id});

  int64_t workout_id = repo->SaveWorkout(workout);
  EXPECT_GT(workout_id, 0);

  auto workouts = repo->GetWorkouts(200);
  ASSERT_EQ(workouts.size(), 1);

  const auto& saved = workouts[0];
  EXPECT_EQ(saved.exercises_.size(), 2);
  EXPECT_EQ(saved.exercises_[0].exercise_id_, ex1_id);
  EXPECT_EQ(saved.exercises_[0].sets_, 3);
  EXPECT_EQ(saved.exercises_[0].reps_, 8);
  EXPECT_EQ(saved.exercises_[0].weight_, 50);

  EXPECT_EQ(saved.exercises_[1].exercise_id_, ex2_id);
  EXPECT_EQ(saved.exercises_[1].sets_, 4);
  EXPECT_EQ(saved.exercises_[1].reps_, 9);
  EXPECT_EQ(saved.exercises_[1].weight_, 60);
}

TEST_F(SqliteWorkoutRepoTest, SaveWorkout_MultipleWorkouts_SeparateUsers) {
  // User 1
  auto ex1 = create_test_exercise("Push-ups", "Chest",
                                  domain::ExerciseType::kBodyWeight);
  int64_t ex1_id = repo->CreateExercise(ex1);

  auto workout1 = create_test_workout(300, {ex1_id});
  auto workout2 = create_test_workout(300, {});

  // User 2
  auto workout3 = create_test_workout(400, {});

  repo->SaveWorkout(workout1);
  repo->SaveWorkout(workout2);
  repo->SaveWorkout(workout3);

  auto user300_workouts = repo->GetWorkouts(300);
  auto user400_workouts = repo->GetWorkouts(400);
  auto user500_workouts = repo->GetWorkouts(500);

  EXPECT_EQ(user300_workouts.size(), 2);
  EXPECT_EQ(user400_workouts.size(), 1);
  EXPECT_EQ(user500_workouts.size(), 0);
}

TEST_F(SqliteWorkoutRepoTest, GetWorkouts_ReturnsInChronologicalOrder) {
  domain::Workout workout1;
  workout1.user_id_ = 500;
  workout1.timestamp_ms_ = 1000;

  domain::Workout workout2;
  workout2.user_id_ = 500;
  workout2.timestamp_ms_ = 2000;

  domain::Workout workout3;
  workout3.user_id_ = 500;
  workout3.timestamp_ms_ = 1500;

  repo->SaveWorkout(workout1);
  repo->SaveWorkout(workout2);
  repo->SaveWorkout(workout3);

  auto workouts = repo->GetWorkouts(500);
  ASSERT_EQ(workouts.size(), 3);

  EXPECT_GE(workouts[0].timestamp_ms_, workouts[1].timestamp_ms_);
  EXPECT_GE(workouts[1].timestamp_ms_, workouts[2].timestamp_ms_);
  EXPECT_EQ(workouts[0].timestamp_ms_, 2000);
  EXPECT_EQ(workouts[2].timestamp_ms_, 1000);
}

TEST_F(SqliteWorkoutRepoTest, GetWorkouts_LoadsExerciseNames) {
  auto ex = create_test_exercise("Test Exercise", "Test Group");
  int64_t ex_id = repo->CreateExercise(ex);

  auto workout = create_test_workout(600, {ex_id});

  int64_t workout_id = repo->SaveWorkout(workout);
  EXPECT_GT(workout_id, 0);

  auto workouts = repo->GetWorkouts(600);
  ASSERT_EQ(workouts.size(), 1);

  ASSERT_EQ(workouts[0].exercises_.size(), 1);
  EXPECT_EQ(workouts[0].exercises_[0].exercise_name_, "Test Exercise");
}

TEST_F(SqliteWorkoutRepoTest, InvalidExerciseId_ShouldHandleGracefully) {
  domain::Workout workout;
  workout.user_id_ = 700;

  domain::WorkoutExercise wex;
  wex.exercise_id_ = 999999;
  wex.sets_ = 3;
  wex.reps_ = 10;
  wex.weight_ = 50;

  workout.exercises_.push_back(wex);
  EXPECT_ANY_THROW({ repo->SaveWorkout(workout); })
      << "Should throw on foreign key violation";
}

TEST_F(SqliteWorkoutRepoTest, WorkoutExerciseOrder_Preserved) {
  std::vector<int64_t> exercise_ids;
  for (int i = 1; i <= 5; ++i) {
    auto ex = create_test_exercise("Exercise" + std::to_string(i), "Group");
    exercise_ids.push_back(repo->CreateExercise(ex));
  }

  domain::Workout workout;
  workout.user_id_ = 800;

  for (int i = exercise_ids.size() - 1; i >= 0; --i) {
    domain::WorkoutExercise wex;
    wex.exercise_id_ = exercise_ids[i];
    wex.sets_ = i + 1;
    wex.reps_ = 10;
    wex.weight_ = i * 10;
    workout.exercises_.push_back(wex);
  }

  repo->SaveWorkout(workout);

  auto workouts = repo->GetWorkouts(800);
  ASSERT_EQ(workouts.size(), 1);

  const auto& saved = workouts[0];
  ASSERT_EQ(saved.exercises_.size(), 5);

  // Порядок должен сохраниться
  for (size_t i = 0; i < saved.exercises_.size(); ++i) {
    EXPECT_EQ(saved.exercises_[i].exercise_id_, exercise_ids[4 - i]);
    EXPECT_EQ(saved.exercises_[i].sets_, static_cast<int>(4 - i + 1));
  }
}

TEST_F(SqliteWorkoutRepoTest, MultipleWorkouts_DifferentMuscleGroups) {
  std::vector<std::string> muscle_groups = {"Chest", "Back", "Legs", "Arms"};

  for (size_t i = 0; i < muscle_groups.size(); ++i) {
    domain::Workout workout;
    workout.user_id_ = 900;
    workout.muscle_group_ = muscle_groups[i];
    workout.description_ = "Workout for " + muscle_groups[i];

    repo->SaveWorkout(workout);
  }

  auto workouts = repo->GetWorkouts(900);
  EXPECT_EQ(workouts.size(), muscle_groups.size());

  std::set<std::string> unique_groups;
  for (const auto& w : workouts) {
    unique_groups.insert(w.muscle_group_);
  }

  EXPECT_EQ(unique_groups.size(), muscle_groups.size());
}

TEST_F(SqliteWorkoutRepoTest, EmptyDescription_HandledCorrectly) {
  domain::Workout workout;
  workout.user_id_ = 1000;
  workout.muscle_group_ = "Test";
  workout.description_ = "";

  int64_t id = repo->SaveWorkout(workout);
  EXPECT_GT(id, 0);

  auto workouts = repo->GetWorkouts(1000);
  ASSERT_EQ(workouts.size(), 1);

  EXPECT_TRUE(workouts[0].description_.empty() ||
              workouts[0].description_ == "");
}

// ==================== Integration Tests ====================

TEST_F(SqliteWorkoutRepoTest, FullIntegration_CreateExerciseAndUseInWorkout) {
  domain::Exercise new_exercise;
  new_exercise.name = "Custom Exercise";
  new_exercise.muscle_group = "Custom Group";
  new_exercise.type = domain::ExerciseType::kCardio;
  new_exercise.description = "A custom exercise for testing";

  int64_t exercise_id = repo->CreateExercise(new_exercise);
  EXPECT_GT(exercise_id, 0);

  auto found_ex = repo->GetExercise(exercise_id);
  ASSERT_TRUE(found_ex.has_value());
  EXPECT_EQ(found_ex->name, "Custom Exercise");

  domain::Workout workout;
  workout.user_id_ = 1100;
  workout.muscle_group_ = "Custom Group";
  workout.description_ = "Workout with custom exercise";

  domain::WorkoutExercise wex;
  wex.exercise_id_ = exercise_id;
  wex.sets_ = 5;
  wex.reps_ = 12;
  wex.weight_ = 30;

  workout.exercises_.push_back(wex);

  int64_t workout_id = repo->SaveWorkout(workout);
  EXPECT_GT(workout_id, 0);

  auto workouts = repo->GetWorkouts(1100);
  ASSERT_EQ(workouts.size(), 1);

  const auto& saved_workout = workouts[0];
  EXPECT_EQ(saved_workout.muscle_group_, "Custom Group");
  EXPECT_EQ(saved_workout.description_, "Workout with custom exercise");

  ASSERT_EQ(saved_workout.exercises_.size(), 1);
  EXPECT_EQ(saved_workout.exercises_[0].exercise_id_, exercise_id);
  EXPECT_EQ(saved_workout.exercises_[0].exercise_name_, "Custom Exercise");
  EXPECT_EQ(saved_workout.exercises_[0].sets_, 5);
  EXPECT_EQ(saved_workout.exercises_[0].reps_, 12);
  EXPECT_EQ(saved_workout.exercises_[0].weight_, 30);
}

TEST_F(SqliteWorkoutRepoTest, StressTest_MultipleOperations) {
  const int NUM_EXERCISES = 10;
  const int NUM_WORKOUTS = 5;

  std::vector<int64_t> exercise_ids;
  for (int i = 0; i < NUM_EXERCISES; ++i) {
    auto ex = create_test_exercise("StressExercise" + std::to_string(i),
                                   "StressGroup");
    exercise_ids.push_back(repo->CreateExercise(ex));
  }

  for (int w = 0; w < NUM_WORKOUTS; ++w) {
    domain::Workout workout;
    workout.user_id_ = 1200 + w;
    workout.muscle_group_ = "StressGroup";

    for (int i = 0; i < 3; ++i) {
      domain::WorkoutExercise wex;
      wex.exercise_id_ = exercise_ids[(w + i) % NUM_EXERCISES];
      wex.sets_ = 3;
      wex.reps_ = 10;
      wex.weight_ = i * 20;
      workout.exercises_.push_back(wex);
    }

    repo->SaveWorkout(workout);
  }

  auto all_exercises = repo->GetAllExercises();
  EXPECT_GE(all_exercises.size(), NUM_EXERCISES);

  for (int w = 0; w < NUM_WORKOUTS; ++w) {
    auto workouts = repo->GetWorkouts(1200 + w);
    EXPECT_EQ(workouts.size(), 1);
    EXPECT_EQ(workouts[0].exercises_.size(), 3);
  }
}