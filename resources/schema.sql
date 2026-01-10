CREATE TABLE IF NOT EXISTS workouts (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id INTEGER NOT NULL,
    muscle_group TEXT NOT NULL,
    description TEXT,
    timestamp_ms INTEGER NOT NULL DEFAULT 0
);

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
);

CREATE INDEX IF NOT EXISTS idx_workouts_user ON workouts(user_id);
CREATE INDEX IF NOT EXISTS idx_workouts_time ON workouts(timestamp_ms);
