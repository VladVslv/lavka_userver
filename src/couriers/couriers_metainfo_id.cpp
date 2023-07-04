#include "couriers_metainfo_id.hpp"
namespace enrollment_template {

namespace {

class CouriersMetainfoId
    : public userver::server::handlers::HttpHandlerJsonBase {
 private:
  userver::formats::json::Value Get(int id, std::string start,
                                    std::string end) const {
    CreateCourierDto courier;
    double rating = 0;
    int earnings = 0;
    int C = 2;
    auto req = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "SELECT * FROM yl_schema.couriers WHERE courier_id=$1 "
        "LIMIT 1;",
        id);
    if (req.Size() == 0) {
      return userver::formats::json::FromString(
          R"({"code":"400","message":"The courier has not completed any orders."})");
    }
    courier =
        req.AsSingleRow<CreateCourierDto>(userver::storages::postgres::kRowTag);
    if (courier.courier_type == "BIKE") {
      C = 3;
    }
    if (courier.courier_type == "AUTO") {
      C = 4;
    }

    req = (pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "SELECT CAST(EXTRACT(EPOCH FROM (TIMESTAMP \'" + end +
            "\')-(TIMESTAMP \'" + start +
            "\')) AS INTEGER) as time,coalesce(SUM(o.cost),0) as cost, "
            "coalesce(COUNT(*), 0) as count "
            "FROM  yl_schema.orders o, yl_schema.completed_orders co "
            "WHERE co.courier_id=" +
            std::to_string(id) +
            " AND o.order_id=co.order_id AND "
            "o.completed_time>=(TIMESTAMP \'" +
            start + "\') AND o.completed_time<(TIMESTAMP \'" + end + "\');"));
    if (req.Size() == 0) {
      return userver::formats::json::FromString(
          R"({"code":"400","message":"The courier has not completed any orders."})");
    }
    rating = req.Front()["count"].As<int>() * 3600;
    rating /= req.Front()["time"].As<int>();
    earnings = req.Front()["cost"].As<int>();

    rating *= (5 - C);
    earnings *= C;
    userver::formats::json::ValueBuilder builder(courier);
    builder["rating"] = rating;
    builder["earnings"] = earnings;
    return builder.ExtractValue();
  }

 public:
  static constexpr std::string_view kName = "handler-couriers-metainfo-id";

  CouriersMetainfoId(
      const userver::components::ComponentConfig& config,
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
    int id = stoi(request.GetPathArg("courier_id"));
    std::string startDate = request.GetArg("startDate");
    std::string endDate = request.GetArg("endDate");
    LOG_INFO() << std::to_string(id) + "\n\n\n";
    return Get(id, startDate, endDate);
  }

  userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace

void AppendCouriersMetainfoId(
    userver::components::ComponentList& component_list) {
  component_list.Append<CouriersMetainfoId>();
}

}  // namespace enrollment_template
