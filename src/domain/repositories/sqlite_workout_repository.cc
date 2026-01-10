#include "sqlite_workout_repository.h"

#include <sqlite3.h>

#include <boost/json.hpp>
#include <boost/json/serializer.hpp>
#include <filesystem>
#include <fstream>
#include <ranges>
namespace gymbot::infra {
class SqliteWorkoutRepository::PImpl {
  using SqliteConnection =
      std::unique_ptr<sqlite3,
                      decltype([](sqlite3* ptr) { sqlite3_close(ptr); })>;

  using SqliteStmt =
      std::unique_ptr<sqlite3_stmt, decltype([](sqlite3_stmt* stmt) {
                        sqlite3_finalize(stmt);
                      })>;

 public:
  PImpl(const std::string& db_path)
      : connection_(make_sqlite_connection(db_path)) {
    create_tables();
  }
  int64_t SaveWorkout(const domain::Workout& workout) {
    auto workout_stmt =
        make_stmt(connection_,
                  "INSERT INTO workouts (user_id, muscle_group, description, "
                  "timestamp_ms) VALUES (?, ?, ?, ?)");

    sqlite3_bind_int64(workout_stmt.get(), 1, workout.user_id_);
    sqlite3_bind_text(workout_stmt.get(), 2, workout.muscle_group_.c_str(), -1,
                      SQLITE_TRANSIENT);
    sqlite3_bind_text(workout_stmt.get(), 3, workout.description_.c_str(), -1,
                      SQLITE_TRANSIENT);
    sqlite3_bind_int64(workout_stmt.get(), 4, workout.timestamp_ms_);

    sqlite3_step(workout_stmt.get());

    auto workout_id = sqlite3_last_insert_rowid(connection_.get());

    for (const auto& [idx, ex] : workout.exercises_ | std::views::enumerate) {
      auto ex_stmt = make_stmt(
          connection_,
          "INSERT INTO workout_exercises (workout_id, exercise_index, name, "
          "sets, reps, type, weight) VALUES (?, ?, ?, ?, ?, ?, ?)");
      sqlite3_bind_int64(ex_stmt.get(), 1, workout_id);
      sqlite3_bind_int64(ex_stmt.get(), 2, idx);
      sqlite3_bind_text(ex_stmt.get(), 3, ex.name_.c_str(), -1,
                        SQLITE_TRANSIENT);
      sqlite3_bind_int64(ex_stmt.get(), 4, ex.sets_);
      sqlite3_bind_int64(ex_stmt.get(), 5, ex.reps_);
      sqlite3_bind_int64(ex_stmt.get(), 6, static_cast<int>(ex.type_));
      sqlite3_bind_int64(ex_stmt.get(), 7, ex.weight_);
      sqlite3_step(ex_stmt.get());
    }

    return workout_id;
  }

  std::vector<domain::Workout> GetWorkouts(int64_t user_id) {
    std::vector<domain::Workout> result;

    auto stmt =
        make_stmt(connection_,
                  "SELECT id, muscle_group, description, timestamp_ms FROM "
                  "workouts WHERE user_id = ? ORDER BY timestamp_ms DESC");

    sqlite3_bind_int64(stmt.get(), 1, user_id);

    while (sqlite3_step(stmt.get()) == SQLITE_ROW) {
      int64_t id = sqlite3_column_int64(stmt.get(), 0);
      const char* group =
          reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 1));
      const char* desc =
          reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 2));

      domain::Workout w;
      w.id_ = id;
      w.user_id_ = user_id;
      w.muscle_group_ = group ? std::string(group) : "";
      w.description_ = desc ? std::string(desc) : "";
      w.timestamp_ms_ = sqlite3_column_int64(stmt.get(), 3);
      w.exercises_ = get_exercises_for_workout(id);
      result.push_back(w);
    }
    return result;
  }

 private:
  SqliteConnection connection_;

  SqliteConnection make_sqlite_connection(const std::string& db_path) {
    sqlite3* raw = nullptr;
    sqlite3_open(db_path.c_str(), &raw);
    return SqliteConnection(raw);
  }

  static SqliteStmt make_stmt(SqliteConnection& conn, const char* sql) {
    sqlite3_stmt* raw = nullptr;
    sqlite3_prepare_v2(conn.get(), sql, -1, &raw, nullptr);
    return SqliteStmt(raw);
  }

  void create_tables() {
    auto schema_path = get_schema_path();
    std::ifstream schema_file(schema_path);

    if (!schema_file.is_open()) {
      throw std::runtime_error("Schema file not found " + schema_path.string());
    }

    std::string schema_sql((std::istreambuf_iterator<char>(schema_file)),
                           std::istreambuf_iterator<char>());
    char* err = nullptr;
    if (sqlite3_exec(connection_.get(), schema_sql.c_str(), nullptr, nullptr,
                     &err) != SQLITE_OK) {
      std::string error = err ? err : "unknown";
      sqlite3_free(err);
      throw std::runtime_error("Schema init failed: " + error);
    }
  }

  std::vector<domain::Exercise> get_exercises_for_workout(int64_t workout_id) {
    std::vector<domain::Exercise> result;

    auto stmt = make_stmt(
        connection_,
        "SELECT name, sets, reps, type, weight FROM workout_exercises WHERE "
        "workout_id = ? ORDER BY exercise_index");
    sqlite3_bind_int64(stmt.get(), 1, workout_id);
    while (sqlite3_step(stmt.get()) == SQLITE_ROW) {
      domain::Exercise ex;
      ex.name_ =
          reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 0));
      ex.sets_ = sqlite3_column_int(stmt.get(), 1);
      ex.reps_ = sqlite3_column_int(stmt.get(), 2);
      ex.type_ =
          static_cast<domain::ExerciseType>(sqlite3_column_int(stmt.get(), 3));
      ex.weight_ = sqlite3_column_int(stmt.get(), 4);
      result.push_back(ex);
    }

    return result;
  }

  std::filesystem::path get_schema_path() { return SCHEMA_PATH; }
};

SqliteWorkoutRepository::SqliteWorkoutRepository(const std::string& db_path)
    : impl_(std::make_unique<PImpl>(db_path)) {}

SqliteWorkoutRepository::~SqliteWorkoutRepository() = default;

int64_t SqliteWorkoutRepository::SaveWorkout(const domain::Workout& workout) {
  return impl_->SaveWorkout(workout);
}

std::vector<domain::Workout> SqliteWorkoutRepository::GetWorkouts(
    int64_t user_id) {
  return impl_->GetWorkouts(user_id);
}

}  // namespace gymbot::infra