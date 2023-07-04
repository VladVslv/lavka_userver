#include "orders.hpp"

namespace enrollment_template {

namespace {

class Orders : public userver::server::handlers::HttpHandlerJsonBase {
 private:
  userver::formats::json::Value Put(
      const userver::formats::json::Value& json) const {
    auto orders = json["orders"].As<std::vector<CreateOrderDto>>(
        std::vector<CreateOrderDto>(0));
    for (auto& order : orders) {
      auto result = pg_cluster_->Execute(
          userver::storages::postgres::ClusterHostType::kMaster,
          "INSERT INTO yl_schema.orders(weight, regions, "
          "delivery_hours, cost) VALUES($1, $2, $3, $4) RETURNING order_id; ",
          order.weight, order.regions, order.delivery_hours, order.cost);
      order.order_id = result.AsSingleRow<int>();
    }
    return userver::formats::json::ValueBuilder(orders).ExtractValue();
  }
  userver::formats::json::Value Get(int limit, int offset) const {
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "SELECT *  FROM yl_schema.orders LIMIT $1 OFFSET $2;", limit, offset);
    std::vector<CreateOrderDto> orders;
    for (const auto& order : result) {
      orders.push_back(
          order.As<CreateOrderDto>(userver::storages::postgres::kRowTag));
    }
    return userver::formats::json::ValueBuilder(orders).ExtractValue();
  }

 public:
  static constexpr std::string_view kName = "handler-orders";

  Orders(const userver::components::ComponentConfig& config,
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

void AppendOrders(userver::components::ComponentList& component_list) {
  component_list.Append<Orders>();
}

}  // namespace enrollment_template
