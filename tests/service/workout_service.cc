// #include "service/workout_service.h"

// #include <gtest/gtest.h>

// #include <memory>

// #include "domain/entities/workout.h"
// #include "domain/repositories//sqlite_workout_repository.h"
// #include "domain/repositories/i_workout_repository.h"
// class MockRepository : public gymbot::domain::IWorkoutRepository {
//  public:
//   int save_count_ = 0;
//   gymbot::domain::Workout last_saved_;

//   int64_t SaveWorkout(const gymbot::domain::Workout& workout) override {
//     last_saved_ = workout;
//     return ++save_count_;
//   }

//   std::vector<gymbot::domain::Workout> GetWorkouts(int64_t user_id) override
//   {
//     return {};
//   }

//   std::vector<gymbot::domain::Workout> (int64_t user_id) override {
//     return {};
//   }
// };
// TEST(WorkoutServiceTest, RecordsWorkoutThroughRepository) {
//   gymbot::service::WorkoutService
//   service(std::make_unique<MockRepository>());

//   EXPECT_EQ(service.SaveWorkout({123, "Chest"}), 1);
// }

// TEST(WorkoutServiceTest, GetWorkouts_FiltersByUserId) {
//   gymbot::service::WorkoutService service(
//       std::make_unique<gymbot::infra::SqliteWorkoutRepository>(":memory:"));
//   service.SaveWorkout({0, "chest"});
//   service.SaveWorkout({1, "legs"});
//   service.SaveWorkout({0, "back"});
//   auto workouts = service.GetUserWorkouts(0);

//   EXPECT_EQ(workouts.size(), 2);
//   EXPECT_EQ(workouts[0].muscle_group_, "chest");
//   EXPECT_EQ(workouts[1].muscle_group_, "back");
// }