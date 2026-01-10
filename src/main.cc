#include <tgbot/tgbot.h>

#include "domain/entities/workout.h"
#include "domain/repositories/sqlite_workout_repository.h"
#include "service/workout_service.h"

int main() {
  gymbot::service::WorkoutService service(
      std::make_unique<gymbot::infra::SqliteWorkoutRepository>("gymbot.db"));

  // 2. Telegram Bot
  TgBot::Bot bot("8572912737:AAFMR9sbdLQx67H9vB5iiRG3Z8fspUre3kY");

  // 3. /start
  bot.getEvents().onCommand("start", [&bot](TgBot::Message::Ptr msg) {
    bot.getApi().sendMessage(msg->chat->id,
                             "💪 GymBot\n\n/record chest\n/record legs\n/list");
  });

  // 4. /record chest
  bot.getEvents().onCommand("record", [&service,
                                       &bot](TgBot::Message::Ptr msg) {
    std::string full_text = msg->text;  // "/record chest"
    size_t first_space = full_text.find(' ');
    std::string muscle = (first_space != std::string::npos)
                             ? full_text.substr(first_space + 1)
                             : "";

    std::cout << "DEBUG muscle='" << muscle << "'" << std::endl;  // ← Добавь!

    if (muscle.empty()) {
      bot.getApi().sendMessage(msg->chat->id, "❌ /record <группа мышц>");
      return;
    }

    gymbot::domain::Workout workout{msg->from->id, muscle};
    try {
      int64_t id = service.SaveWorkout(workout);
      bot.getApi().sendMessage(msg->chat->id,
                               "✅ #" + std::to_string(id) + " " + muscle);
    } catch (...) {
      bot.getApi().sendMessage(msg->chat->id, "❌ Ошибка сохранения");
    }
  });

  // 5. /list
  bot.getEvents().onCommand("list", [&service, &bot](TgBot::Message::Ptr msg) {
    auto workouts = service.GetUserWorkouts(msg->from->id);
    if (workouts.empty()) {
      bot.getApi().sendMessage(msg->chat->id, "📭 Нет тренировок");
      return;
    }

    std::string list = "📋 Твои тренировки:\n";
    for (const auto& w : workouts) {
      list += "• " + w.muscle_group_ + "\n";
    }
    bot.getApi().sendMessage(msg->chat->id, list);
  });

  std::cout << "🤖 Bot запущен!" << std::endl;
  TgBot::TgLongPoll longPoll(bot);
  while (true) {
    try {
      longPoll.start();
    } catch (std::exception& e) {
      std::cerr << "💥 LongPoll died: " << e.what() << std::endl;
      std::cout << "🔄 Restarting in 5s..." << std::endl;
      sleep(5);
    }
  }
  std::cout << "🔥 REACHED END OF MAIN!" << std::endl;
}
