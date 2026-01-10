#include "sqlite_workout_repository.h"

#include <sqlite3.h>

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
  PImpl() : connection_(make_sqlite_connection()) { create_tables(); }
  int64_t SaveWorkout(const domain::Workout& workout) {
    auto stmt =
        make_stmt(connection_,
                  "INSERT INTO workouts (user_id, muscle_group) VALUES (?, ?)");
    sqlite3_bind_int64(stmt.get(), 1, workout.user_id_);
    sqlite3_bind_text(stmt.get(), 2, workout.muscle_group_.c_str(), -1,
                      SQLITE_STATIC);
    if (sqlite3_step(stmt.get()) != SQLITE_DONE) {
      throw std::runtime_error("Workout insertion failed");
    }
    return sqlite3_last_insert_rowid(connection_.get());
  }
  std::vector<domain::Workout> GetWorkouts(int64_t user_id) {
    std::vector<domain::Workout> result;
    auto stmt = make_stmt(
        connection_, "SELECT id, muscle_group FROM workouts WHERE user_id = ?");

    sqlite3_bind_int64(stmt.get(), 1, user_id);

    while (sqlite3_step(stmt.get()) == SQLITE_ROW) {
      int64_t id = sqlite3_column_int64(stmt.get(), 0);
      const char* group =
          reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 1));
      result.emplace_back(domain::Workout{id, std::string(group)});
    }
    return result;
  }

 private:
  SqliteConnection connection_;

  SqliteConnection make_sqlite_connection() {
    sqlite3* raw = nullptr;
    sqlite3_open("gymbot.db", &raw);
    return SqliteConnection(raw);
  }

  static SqliteStmt make_stmt(SqliteConnection& conn, const char* sql) {
    sqlite3_stmt* raw = nullptr;
    sqlite3_prepare_v2(conn.get(), sql, -1, &raw, nullptr);
    return SqliteStmt(raw);
  }

  void create_tables() {
    auto stmt = make_stmt(connection_,
                          "CREATE TABLE IF NOT EXISTS workouts ("
                          "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                          "user_id INTEGER, "
                          "muscle_group TEXT)");
    sqlite3_step(stmt.get());
  }
};

SqliteWorkoutRepository::SqliteWorkoutRepository()
    : impl_(std::make_unique<PImpl>()) {}

SqliteWorkoutRepository::~SqliteWorkoutRepository() = default;

int64_t SqliteWorkoutRepository::SaveWorkout(const domain::Workout& workout) {
  return impl_->SaveWorkout(workout);
}

std::vector<domain::Workout> SqliteWorkoutRepository::GetWorkouts(
    int64_t user_id) {
  return impl_->GetWorkouts(user_id);
}

}  // namespace gymbot::infra