#include "couriers.hpp"

namespace enrollment_template {

namespace {

class Couriers : public userver::server::handlers::HttpHandlerJsonBase {
 private:
  userver::formats::json::Value Put(
      const userver::formats::json::Value& json) const {
    auto couriers = json["couriers"].As<std::vector<CreateCourierDto>>(
        std::vector<CreateCourierDto>(0));
    for (auto& courier : couriers) {
      auto result = pg_cluster_->Execute(
          userver::storages::postgres::ClusterHostType::kMaster,
          "INSERT INTO yl_schema.couriers(courier_type, regions, "
          "working_hours) VALUES($1, $2, $3) RETURNING courier_id; ",
          courier.courier_type, courier.regions, courier.working_hours);
      courier.courier_id = result.AsSingleRow<int>();
    }
    userver::formats::json::ValueBuilder builder;
    builder["couriers"] = couriers;
    return builder.ExtractValue();
  }
  userver::formats::json::Value Get(int limit, int offset) const {
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "SELECT *  FROM yl_schema.couriers LIMIT $1 OFFSET $2;", limit, offset);
    std::vector<CreateCourierDto> couriers;
    for (const auto& courier : result) {
      couriers.push_back(
          courier.As<CreateCourierDto>(userver::storages::postgres::kRowTag));
    }
    userver::formats::json::ValueBuilder builder;
    builder["couriers"] = couriers;
    builder["limit"] = limit;
    builder["offset"] = offset;
    return builder.ExtractValue();
  }

 public:
  static constexpr std::string_view kName = "handler-couriers";

  Couriers(const userver::components::ComponentConfig& config,
           const userver::components::ComponentContext& component_context)
      : HttpHandlerJsonBase(config, component_context),
        pg_cluster_(
            component_context
                .FindComponent<userver::components::Postgres>("postgres-db-1")
                .GetCluster()) {}

  userver::formats::json::Value HandleRequestJsonThrow(
      const userver::server::http::HttpRequest& request,
      const userver::formats::json::Value& json,
      userver::server::request::RequestContext&) const override {
    if (request.GetMethod() == userver::server::http::HttpMethod::kPost) {
      return Put(json);
    }
    int limit = request.HasArg("limit") ? stoi(request.GetArg("limit")) : 1;
    int offset = request.HasArg("offset") ? stoi(request.GetArg("offset")) : 0;

    return Get(limit, offset);
  }

  userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace

void AppendCouriers(userver::components::ComponentList& component_list) {
  component_list.Append<Couriers>();
}

}  // namespace enrollment_template
