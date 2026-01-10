#include "domain/entities/workout.h"

#include <gtest/gtest.h>
using namespace gymbot::domain;
TEST(WorkoutEntity, DefaultConstructor) {
  Workout w;
  EXPECT_EQ(w.id_, 0);
}

TEST(WorkoutEntity, FromDtoSetsFields) {
  auto w = Workout::FromDto(30, "Spine");
  EXPECT_EQ(w.user_id_, 30);
  EXPECT_EQ(w.muscle_group_, "Spine");
}