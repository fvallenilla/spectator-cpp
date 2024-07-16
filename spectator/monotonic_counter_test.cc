#include "stateless_meters.h"
#include "test_publisher.h"
#include <gtest/gtest.h>

namespace {

using spectator::Id;
using spectator::MonotonicCounter;
using spectator::Tags;
using spectator::TestPublisher;

TEST(MonotonicCounter, Set) {
  TestPublisher publisher;
  auto id = std::make_shared<Id>("ctr", Tags{});
  auto id2 = std::make_shared<Id>("ctr2", Tags{{"key", "val"}});
  MonotonicCounter<TestPublisher> c{id, &publisher};
  MonotonicCounter<TestPublisher> c2{id2, &publisher};

  c.Set(42);
  c2.Set(2);
  c.Set(43);
  std::vector<std::string> expected = {"C:ctr:42", "C:ctr2,key=val:2",
                                       "C:ctr:43"};
  EXPECT_EQ(publisher.SentMessages(), expected);
}

TEST(MonotonicCounter, InvalidTags) {
  TestPublisher publisher;
  // test with a single tag, because tags order is not guaranteed in a flat_hash_map
  auto id = std::make_shared<Id>("test`!@#$%^&*()-=~_+[]{}\\|;:'\",<.>/?foo",
                                 Tags{{"tag1,:=", "value1,:="}});
  MonotonicCounter c{id, &publisher};
  EXPECT_EQ("C:test______^____-_~______________.___foo,tag1___=value1___:", c.GetPrefix());
}
}  // namespace
