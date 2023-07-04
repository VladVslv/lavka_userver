#pragma once

#include <userver/formats/serialize/common_containers.hpp>
#include <iostream>
#include <userver/formats/parse/common_containers.hpp>
#include <vector>
#include <string>
#include <string_view>
#include <fmt/format.h>
#include <userver/clients/dns/component.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/server/handlers/http_handler_json_base.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/utils/assert.hpp>
#include <userver/formats/json.hpp>
#include <userver/server/http/http_method.hpp>
#include <userver/components/component_list.hpp>

struct CreateOrderDto {
  int64_t order_id;
  int weight;
  int regions;
  std::vector<std::string> delivery_hours;
  int cost;
  std::optional<std::chrono::time_point<std::chrono::system_clock>>
      completed_time;
};

CreateOrderDto Parse(const userver::formats::json::Value& json,
                     userver::formats::parse::To<CreateOrderDto>);

userver::formats::json::Value Serialize(
    const CreateOrderDto& data,
    userver::formats::serialize::To<userver::formats::json::Value>);