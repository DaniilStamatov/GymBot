#include "domain/entities/workout.h"

#include <gtest/gtest.h>

using namespace gymbot::domain;

TEST(WorkoutEntity, DefaultConstructor) {
  Workout w;
  EXPECT_EQ(w.id_, 0);
  EXPECT_EQ(w.user_id_, 0);
  EXPECT_TRUE(w.muscle_group_.empty());
  EXPECT_TRUE(w.description_.empty());
  EXPECT_TRUE(w.exercises_.empty());
  EXPECT_GT(w.timestamp_ms_, 0);
}

TEST(WorkoutTest, ConstructorWithUserAndMuscleGroup) {
  Workout w{12345, "chest"};

  EXPECT_EQ(w.user_id_, 12345);
  EXPECT_EQ(w.muscle_group_, "chest");
  EXPECT_TRUE(w.exercises_.empty());
  EXPECT_TRUE(w.description_.empty());
  EXPECT_GT(w.timestamp_ms_, 0);
}

TEST(WorkoutTest, ConstructorWithExercises) {
  std::vector<WorkoutExercise> exercises = {{1, "Bench Press", 4, 8, 80},
                                            {2, "Cable Fly", 3, 12, 20}};

  Workout w{12345, "chest", exercises, "Chest day"};

  EXPECT_EQ(w.user_id_, 12345);
  EXPECT_EQ(w.muscle_group_, "chest");
  EXPECT_EQ(w.description_, "Chest day");
  EXPECT_EQ(w.exercises_.size(), 2);

  EXPECT_EQ(w.exercises_[0].exercise_id_, 1);
  EXPECT_EQ(w.exercises_[0].exercise_name_, "Bench Press");
  EXPECT_EQ(w.exercises_[0].sets_, 4);
  EXPECT_EQ(w.exercises_[0].reps_, 8);
  EXPECT_EQ(w.exercises_[0].weight_, 80);

  EXPECT_EQ(w.exercises_[1].exercise_id_, 2);
  EXPECT_EQ(w.exercises_[1].exercise_name_, "Cable Fly");
  EXPECT_EQ(w.exercises_[1].sets_, 3);
  EXPECT_EQ(w.exercises_[1].reps_, 12);
  EXPECT_EQ(w.exercises_[1].weight_, 20);

  EXPECT_GT(w.timestamp_ms_, 0);
}

TEST(WorkoutTest, ConstructorWithoutDescription) {
  std::vector<WorkoutExercise> exercises = {{1, "Squat", 5, 5, 100}};

  Workout w{999, "legs", exercises};

  EXPECT_EQ(w.user_id_, 999);
  EXPECT_EQ(w.muscle_group_, "legs");
  EXPECT_EQ(w.exercises_.size(), 1);
  EXPECT_TRUE(w.description_.empty());
  EXPECT_GT(w.timestamp_ms_, 0);
}

TEST(WorkoutTest, AddExerciseMethod) {
  Workout w{123, "back"};

  EXPECT_TRUE(w.exercises_.empty());
  w.AddExercise({1, "Pull-ups", 3, 10, 0});

  EXPECT_EQ(w.exercises_.size(), 1);
  EXPECT_EQ(w.exercises_[0].exercise_id_, 1);
  EXPECT_EQ(w.exercises_[0].exercise_name_, "Pull-ups");
  EXPECT_EQ(w.exercises_[0].sets_, 3);
  EXPECT_EQ(w.exercises_[0].reps_, 10);
  EXPECT_EQ(w.exercises_[0].weight_, 0);

  w.AddExercise(WorkoutExercise{2, "Deadlift", 5, 3, 120});

  EXPECT_EQ(w.exercises_.size(), 2);
  EXPECT_EQ(w.exercises_[1].exercise_id_, 2);
  EXPECT_EQ(w.exercises_[1].exercise_name_, "Deadlift");
}
