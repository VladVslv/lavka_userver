#include "orders_complete.hpp"

namespace enrollment_template {

namespace {
struct CompletedOrder {
  int64_t courier_id;
  int64_t order_id;
  std::chrono::time_point<std::chrono::system_clock> complete_time;
};
CompletedOrder Parse(const userver::formats::json::Value& json,
                     userver::formats::parse::To<CompletedOrder>) {
  return CompletedOrder{
      json["courier_id"].As<int64_t>(0), json["order_id"].As<int64_t>(0),
      json["complete_time"]
          .As<std::chrono::time_point<std::chrono::system_clock>>()};
}

class OrdersComplete : public userver::server::handlers::HttpHandlerJsonBase {
 private:
  userver::formats::json::Value Put(
      const userver::formats::json::Value& json) const {
    auto orders = json["complete_info"].As<std::vector<CompletedOrder>>(
        std::vector<CompletedOrder>(0));

    std::vector<CreateOrderDto> res;
    for (auto& order : orders) {
      if ((pg_cluster_->Execute(
               userver::storages::postgres::ClusterHostType::kMaster,
               "SELECT * FROM yl_schema.orders WHERE order_id=$1 LIMIT 1",
               order.order_id))
              .Size() == 0) {
        return userver::formats::json::FromString(
            R"({"code":"400","message":"There is no order with required id."})");
      }

      if ((pg_cluster_->Execute(
               userver::storages::postgres::ClusterHostType::kMaster,
               "SELECT * FROM yl_schema.couriers WHERE courier_id=$1 LIMIT 1",
               order.courier_id))
              .Size() == 0) {
        return userver::formats::json::FromString(
            R"({"code":"400","message":"There is no courier with required id."})");
      }

      if ((pg_cluster_->Execute(
               userver::storages::postgres::ClusterHostType::kMaster,
               "SELECT * FROM yl_schema.completed_orders WHERE order_id=$1 "
               "LIMIT 1",
               order.order_id))
              .Size() != 0) {
        return userver::formats::json::FromString(
            R"({"code":"400","message":"The order with required id has already been completed."})");
      }
    }
    for (auto& order : orders) {
      auto result = pg_cluster_->Execute(
          userver::storages::postgres::ClusterHostType::kMaster,
          "INSERT INTO yl_schema.completed_orders(courier_id, order_id) "
          "VALUES($1, $2);",
          order.courier_id, order.order_id);

      result = pg_cluster_->Execute(
          userver::storages::postgres::ClusterHostType::kMaster,
          "UPDATE yl_schema.orders "
          "SET completed_time=$2  WHERE order_id=$1 RETURNING *;",
          order.order_id, order.complete_time);
      res.push_back(
          result[0].As<CreateOrderDto>(userver::storages::postgres::kRowTag));
    }
    return userver::formats::json::ValueBuilder(res).ExtractValue();
  }

 public:
  static constexpr std::string_view kName = "handler-orders-complete";

  OrdersComplete(const userver::components::ComponentConfig& config,
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
    return Put(json);
  }

  userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace

void AppendOrdersComplete(userver::components::ComponentList& component_list) {
  component_list.Append<OrdersComplete>();
}

}  // namespace enrollment_template
