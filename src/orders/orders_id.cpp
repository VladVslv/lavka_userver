#include "orders_id.hpp"

namespace enrollment_template {

namespace {

class OrdersId : public userver::server::handlers::HttpHandlerJsonBase {
 private:
  userver::formats::json::Value Get(int id) const {
    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "SELECT *  FROM yl_schema.orders WHERE order_id=$1 LIMIT 1;", id);
    if (result.Size() == 0) {
      return userver::formats::json::FromString(
          R"({"code":"400","message":"There is no order with this id."})");
    }
    return userver::formats::json::ValueBuilder(
               result[0].As<CreateOrderDto>(
                   userver::storages::postgres::kRowTag))
        .ExtractValue();
  }

 public:
  static constexpr std::string_view kName = "handler-orders-id";

  OrdersId(const userver::components::ComponentConfig& config,
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
    return Get(stoi(request.GetPathArg("order_id")));
  }

  userver::storages::postgres::ClusterPtr pg_cluster_;
};  // namespace

}  // namespace

void AppendOrdersId(userver::components::ComponentList& component_list) {
  component_list.Append<OrdersId>();
}

}  // namespace enrollment_template
