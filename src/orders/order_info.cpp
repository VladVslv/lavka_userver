#include "order_info.hpp"

CreateOrderDto Parse(const userver::formats::json::Value& json,
                     userver::formats::parse::To<CreateOrderDto>) {
  auto result =
      CreateOrderDto{json["order_id"].As<int64_t>(0), json["weight"].As<int>(1),
                     json["regions"].As<int>(1),
                     json["delivery_hours"].As<std::vector<std::string>>(
                         std::vector<std::string>(0)),
                     json["cost"].As<int>(1)};
  if (json.HasMember("completed_time")) {
    result.completed_time =
        json["completed_time"]
            .As<std::chrono::time_point<std::chrono::system_clock>>();
  }
  return result;
}

userver::formats::json::Value Serialize(
    const CreateOrderDto& data,
    userver::formats::serialize::To<userver::formats::json::Value>) {
  userver::formats::json::ValueBuilder builder;
  builder["order_id"] = data.order_id;
  builder["weight"] = data.weight;
  builder["regions"] = data.regions;
  builder["delivery_hours"] = data.delivery_hours;
  builder["cost"] = data.cost;
  if (data.completed_time.has_value()) {
    builder["completed_time"] = data.completed_time;
  }
  return builder.ExtractValue();
}