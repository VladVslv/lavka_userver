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

struct CreateCourierDto {
  int64_t courier_id;
  std::string courier_type;
  std::vector<int> regions;
  std::vector<std::string> working_hours;
};

CreateCourierDto Parse(const userver::formats::json::Value& json,
                     userver::formats::parse::To<CreateCourierDto>);

userver::formats::json::Value Serialize(
    const CreateCourierDto& data,
    userver::formats::serialize::To<userver::formats::json::Value>);