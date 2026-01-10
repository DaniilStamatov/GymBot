#include "sqlite_workout_repository.h"

#include <sqlite3.h>

#include <boost/json.hpp>
#include <boost/json/serializer.hpp>
#include <filesystem>
#include <fstream>
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
  int64_t SaveWorkout(const domain::Workout& workout) {}

  std::vector<domain::Workout> GetWorkouts(int64_t user_id) {}

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