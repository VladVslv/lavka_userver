#include "courier_info.hpp"

CreateCourierDto Parse(const userver::formats::json::Value& json,
                       userver::formats::parse::To<CreateCourierDto>) {
  return CreateCourierDto{json["courier_id"].As<int64_t>(0),
                          json["courier_type"].As<std::string>(""), json["regions"].As<std::vector<int>>(std::vector<int>(0)),
                          json["working_hours"].As<std::vector<std::string>>(
                              std::vector<std::string>(0))};
}

userver::formats::json::Value Serialize(
    const CreateCourierDto& data,
    userver::formats::serialize::To<userver::formats::json::Value>) {
  userver::formats::json::ValueBuilder builder;
  builder["courier_id"] = data.courier_id;
  builder["courier_type"] = data.courier_type;
  builder["regions"] = data.regions;
  builder["working_hours"] = data.working_hours;
  return builder.ExtractValue();
}
