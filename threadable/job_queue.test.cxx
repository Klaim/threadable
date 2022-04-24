#include <threadable/job_queue.hxx>
#include <threadable/doctest_include.hxx>

using namespace threadable;

SCENARIO("job_queue: push, pop, steal")
{
  auto queue = job_queue{};

  GIVEN("queue is empty")
  {
    WHEN("steal one job")
    {
      auto* job = queue.steal();
      THEN("no job is returned")
      {
        REQUIRE(!job);
      }
    }
  }
  GIVEN("push two jobs")
  {
    queue.push([]{});
    queue.push([]{});
    WHEN("pop two jobs")
    {
      auto& job1 = queue.pop();
      auto& job2 = queue.pop();
      THEN("queue is empty")
      {
        REQUIRE(queue.size() == 0);
      }
    }
    WHEN("pop one job")
    {
      auto& job1 = queue.pop();
      AND_WHEN("steal one job")
      {
        auto* job2 = queue.steal();
        THEN("queue is empty")
        {
          REQUIRE(queue.size() == 0);
        }
      }
    }
  }
}

SCENARIO("job_queue: execution")
{
  auto queue = job_queue{};
  std::vector<int> order;
  
  GIVEN("push two jobs")
  {
    queue.push([&order]{ order.push_back(1); });
    queue.push([&order]{ order.push_back(2); });
    WHEN("pop two jobs")
    {
      auto& job1 = queue.pop();
      auto& job2 = queue.pop();
      THEN("jobs are ordered LIFO")
      {
        REQUIRE(job1);
        job1();
        job2();
        REQUIRE(order.size() == 2);
        REQUIRE(order[0] == 2);
        REQUIRE(order[1] == 1);
      }
    }
    WHEN("steal two jobs")
    {
      auto* job1 = queue.steal();
      auto* job2 = queue.steal();
      THEN("jobs are ordered FIFO")
      {
        (*job1)();
        (*job2)();
        REQUIRE(order.size() == 2);
        REQUIRE(order[0] == 1);
        REQUIRE(order[1] == 2);
      }
    }
    WHEN("pop one job")
    {
      auto& job1 = queue.pop();
      AND_WHEN("steal one job")
      {
        auto* job2 = queue.steal();
        THEN("jobs are ordered LIFO")
        {
          job1();
          (*job2)();
          REQUIRE(order.size() == 2);
          REQUIRE(order[0] == 2);
          REQUIRE(order[1] == 1);
        }
      }
    }
  }
}
