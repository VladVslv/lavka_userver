#include <userver/clients/http/component.hpp>
#include <userver/components/minimal_server_component_list.hpp>
#include <userver/server/handlers/ping.hpp>
#include <userver/server/handlers/tests_control.hpp>
#include <userver/testsuite/testsuite_support.hpp>
#include <userver/utils/daemon_run.hpp>


#include "orders/orders.hpp"
#include "orders/orders_id.hpp"
#include "orders/orders_complete.hpp"
#include "couriers/couriers.hpp"
#include "couriers/couriers_id.hpp"
#include "couriers/couriers_metainfo_id.hpp"

int main(int argc, char* argv[]) {
  auto component_list = userver::components::MinimalServerComponentList()
                            .Append<userver::server::handlers::Ping>()
                            .Append<userver::components::TestsuiteSupport>()
                            .Append<userver::components::HttpClient>()
                            .Append<userver::server::handlers::TestsControl>();

  enrollment_template::AppendOrders(component_list);
  enrollment_template::AppendOrdersId(component_list);
  enrollment_template::AppendOrdersComplete(component_list);
  enrollment_template::AppendCouriers(component_list);
  enrollment_template::AppendCouriersId(component_list);
  enrollment_template::AppendCouriersMetainfoId(component_list);
  component_list.Append<userver::components::Postgres>("postgres-db-1");
  component_list.Append<userver::clients::dns::Component>();

  return userver::utils::DaemonMain(argc, argv, component_list);
}
