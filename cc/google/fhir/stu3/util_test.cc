// Copyright 2018 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "google/fhir/stu3/util.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "absl/time/time.h"
#include "google/fhir/testutil/proto_matchers.h"
#include "proto/stu3/codes.pb.h"
#include "proto/stu3/datatypes.pb.h"
#include "proto/stu3/metadatatypes.pb.h"
#include "proto/stu3/resources.pb.h"
#include "tensorflow/core/lib/core/errors.h"

namespace google {
namespace fhir {
namespace stu3 {

namespace {

using ::google::fhir::stu3::proto::ContainedResource;
using ::google::fhir::stu3::proto::Date;
using ::google::fhir::stu3::proto::DateTime;
using ::google::fhir::stu3::proto::Encounter;
using ::google::fhir::testutil::EqualsProto;

TEST(GetTimeFromTimelikeElement, DateTime) {
  DateTime with_time;
  with_time.set_value_us(1517166000000000L);  // 2018-01-28 19:00:00
  DateTime time_not_set;

  EXPECT_EQ(absl::ToUnixMicros(
                google::fhir::stu3::GetTimeFromTimelikeElement(with_time)),
            1517166000000000L);
  EXPECT_EQ(absl::ToUnixMicros(
                google::fhir::stu3::GetTimeFromTimelikeElement(time_not_set)),
            0L);
}

TEST(GetTimeFromTimelikeElement, Date) {
  Date with_time;
  with_time.set_value_us(1517166000000000L);  // 2018-01-28 19:00:00
  Date time_not_set;

  EXPECT_EQ(absl::ToUnixMicros(
                google::fhir::stu3::GetTimeFromTimelikeElement(with_time)),
            1517166000000000L);
  EXPECT_EQ(absl::ToUnixMicros(
                google::fhir::stu3::GetTimeFromTimelikeElement(time_not_set)),
            0L);
}

TEST(WrapContainedResource, Valid) {
  Encounter encounter;
  encounter.mutable_id()->set_value("47");

  ContainedResource expected;
  *(expected.mutable_encounter()) = encounter;
  auto result = WrapContainedResource(encounter);
  ASSERT_TRUE(result.status().ok());
  EXPECT_THAT(result.ValueOrDie(), EqualsProto(expected));
}

TEST(WrapContainedResource, InvalidType) {
  DateTime datetime;
  EXPECT_EQ(WrapContainedResource(datetime).status(),
            ::tensorflow::errors::InvalidArgument(
                "Resource type DateTime not found in "
                "fhir::Bundle::Entry::resource"));
}

TEST(SetContainedResource, Valid) {
  Encounter encounter;
  encounter.mutable_id()->set_value("47");

  ContainedResource expected;
  *(expected.mutable_encounter()) = encounter;

  ContainedResource result;
  ASSERT_TRUE(SetContainedResource(encounter, &result).ok());
  EXPECT_THAT(result, EqualsProto(expected));
}

TEST(SetContainedResource, InvalidType) {
  DateTime datetime;
  ContainedResource result;
  EXPECT_EQ(SetContainedResource(datetime, &result),
            ::tensorflow::errors::InvalidArgument(
                "Resource type DateTime not found in "
                "fhir::Bundle::Entry::resource"));
}

}  // namespace
}  // namespace stu3
}  // namespace fhir
}  // namespace google
