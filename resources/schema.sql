DROP TABLE IF EXISTS workout_exercises;
DROP TABLE IF EXISTS workouts;
DROP TABLE IF EXISTS exercises;

-- Справочник упражнений (шаблоны)
CREATE TABLE IF NOT EXISTS exercises (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT UNIQUE NOT NULL,
    muscle_group TEXT NOT NULL,
    type INTEGER NOT NULL,
    description TEXT,
    UNIQUE(name, muscle_group)
);

-- Тренировки пользователей
CREATE TABLE IF NOT EXISTS workouts (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id INTEGER NOT NULL,
    muscle_group TEXT NOT NULL,
    description TEXT,
    timestamp_ms INTEGER NOT NULL DEFAULT 0
);

-- Выполненные упражнения в тренировках
CREATE TABLE IF NOT EXISTS workout_exercises (
    workout_id INTEGER NOT NULL,
    exercise_id INTEGER NOT NULL,
    exercise_index INTEGER NOT NULL,
    sets INTEGER NOT NULL,
    reps INTEGER NOT NULL,
    weight INTEGER NOT NULL,
    PRIMARY KEY (workout_id, exercise_index),
    FOREIGN KEY(workout_id) REFERENCES workouts(id) ON DELETE CASCADE,
    FOREIGN KEY(exercise_id) REFERENCES exercises(id)
);

CREATE INDEX IF NOT EXISTS idx_workouts_user ON workouts(user_id);
CREATE INDEX IF NOT EXISTS idx_workouts_time ON workouts(timestamp_ms);
CREATE INDEX IF NOT EXISTS idx_exercises_name ON exercises(name);