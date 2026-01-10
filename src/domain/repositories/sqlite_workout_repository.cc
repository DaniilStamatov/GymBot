#include "sqlite_workout_repository.h"

#include <sqlite3.h>

#include <boost/json.hpp>
#include <boost/json/serializer.hpp>
namespace json = boost::json;
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
    auto tx_stmt = make_stmt(connection_, "BEGIN TRANSACTION");
    sqlite3_step(tx_stmt.get());
    try {
      // 1. Сохраняем основную тренировку
      auto workout_stmt = make_stmt(connection_,
                                    "INSERT INTO workouts (user_id, "
                                    "muscle_group, description, timestamp_ms) "
                                    "VALUES (?, ?, ?, ?)");

      sqlite3_bind_int64(workout_stmt.get(), 1, workout.user_id_);
      sqlite3_bind_text(workout_stmt.get(), 2, workout.muscle_group_.c_str(),
                        -1, SQLITE_TRANSIENT);
      sqlite3_bind_text(workout_stmt.get(), 3, workout.description_.c_str(), -1,
                        SQLITE_TRANSIENT);
      sqlite3_bind_int64(workout_stmt.get(), 4, workout.timestamp_ms_);

      if (sqlite3_step(workout_stmt.get()) != SQLITE_DONE) {
        throw std::runtime_error("Workout insertion failed");
      }

      int64_t workout_id = sqlite3_last_insert_rowid(connection_.get());
      std::ranges::
    }
  }

  std::vector<domain::Workout> GetWorkouts(int64_t user_id) {
    std::vector<domain::Workout> result;

    auto stmt = make_stmt(
        connection_,
        "SELECT id, muscle_group, description, exercises_json, timestamp_ms "
        "FROM workouts WHERE user_id = ? ORDER BY timestamp_ms DESC");

    sqlite3_bind_int64(stmt.get(), 1, user_id);

    while (sqlite3_step(stmt.get()) == SQLITE_ROW) {
      int64_t id = sqlite3_column_int64(stmt.get(), 0);
      const char* group =
          reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 1));
      const char* desc =
          reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 2));

      domain::Workout workout;
      workout.id_ = id;
      workout.user_id_ = user_id;
      workout.muscle_group_ = group ? std::string(group) : "";
      workout.description_ = desc ? std::string(desc) : "";
      // Парсим JSON упражнений если нужно
      // TODO: добавить десериализацию exercises_json в workout.exercises_

      result.push_back(std::move(workout));
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
    auto stmt1 = make_stmt(connection_, R"(
    CREATE TABLE IF NOT EXISTS workouts (
      id INTEGER PRIMARY KEY AUTOINCREMENT,
      user_id INTEGER NOT NULL,
      muscle_group TEXT NOT NULL,
      description TEXT,
      timestamp_ms INTEGER NOT NULL DEFAULT 0,
      INDEX idx_workouts_user (user_id),
      INDEX idx_workouts_time (timestamp_ms)
    )
  )");
    sqlite3_step(stmt1.get());

    auto stmt2 = make_stmt(connection_, R"(
    CREATE TABLE IF NOT EXISTS workout_exercises (
      workout_id INTEGER NOT NULL,
      exercise_index INTEGER NOT NULL,
      name TEXT NOT NULL,
      sets INTEGER NOT NULL,
      reps INTEGER NOT NULL,
      type INTEGER NOT NULL,
      weight INTEGER NOT NULL,
      PRIMARY KEY (workout_id, exercise_index),
      FOREIGN KEY(workout_id) REFERENCES workouts(id) ON DELETE CASCADE
    )
  )");
  }
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