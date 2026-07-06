
#include <real/core/event.hpp>
#include "real/resource/resource.hpp"
#include "real/core/uuid.hpp"
#include "real/resource/resource_database.hpp"
#include "real/resource/resource_handle.hpp"
#include <gtest/gtest.h>
#include <optional>
#include <string>
#include <vector>

using namespace real;

class TestResource : public Resource {
public:
	TestResource(int _i=0)
		: Resource(nullptr), i(_i) {}

	~TestResource() {}

public:
	int i;
};

TEST(Resources, ResourceHandleMemoryLeaks) {
}

TEST(Resources, ResourceHandleArray) {
}

TEST(Resources, ResourceDatabase) {
	ResourceDatabase *db = new ResourceDatabase(nullptr);

	for (int i = 0; i < 1000; i++) {
		db->register_resource(new TestResource(i), std::to_string(i));
	}

	{
		auto handle = db->get_resource<TestResource>("80");
		EXPECT_EQ(handle.get()->i, 80);
		handle.free();
	}

	{
		auto handle = db->get_resource<TestResource>("80");
		EXPECT_EQ(handle.get_state(), ResourceState::Unloaded);
		// EXPECT_EQ(db->get_entry(handle.get_uuid()).name, "80");
	}
	delete db;
}

