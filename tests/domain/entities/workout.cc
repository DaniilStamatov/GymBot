#include "domain/entities/workout.h"

#include <gtest/gtest.h>
using namespace gymbot::domain;
TEST(WorkoutEntity, DefaultConstructor) {
  Workout w;
  EXPECT_EQ(w.id_, 0);
}

TEST(WorkoutTest, FullConstructor) {
  std::vector<Exercise> exercises = {{"Жим лежа", 4, 8, kStrengthWeighed, 80},
                                     {"Разводка", 3, 12, kStrengthWeighed, 12}};

  Workout w{12345, "chest", exercises};

  EXPECT_EQ(w.user_id_, 12345);
  EXPECT_EQ(w.muscle_group_, "chest");
  EXPECT_EQ(w.exercises_.size(), 2);
  EXPECT_EQ(w.exercises_[0].name_, "Жим лежа");
  EXPECT_EQ(w.exercises_[0].sets, 4);
  EXPECT_EQ(w.exercises_[0].reps, 8);
  EXPECT_EQ(w.exercises_[0].weight_, 80);
  EXPECT_GT(w.timestamp_ms_, 0);
}

TEST(WorkoutTest, TimestampRecent) {
  auto before = Workout::now_ms();
  Workout w{12345, "back"};
  auto after = Workout::now_ms();

  EXPECT_GE(w.timestamp_ms_, before);
  EXPECT_LE(w.timestamp_ms_, after);
}