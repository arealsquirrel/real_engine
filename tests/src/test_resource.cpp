
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

/*

class TestResource : public Resource {
public:
	TestResource(int _i=0)
		: Resource(nullptr), i(_i) {}

	~TestResource() {}

public:
	int i;
};

TEST(Resources, ResourceHandleMemoryLeaks) {
	ResourceHandle<TestResource> r(nullptr, new TestResource(), ResourceState::Loaded, UUID());

	{
		ResourceHandle<TestResource> i(r);
		EXPECT_EQ(i.get_count(), 2);
		i.set_state(ResourceState::NeverDelete);
	}

	EXPECT_EQ(r.get_count(), 1);
	EXPECT_EQ(r.get_state(), ResourceState::Unreferenced);
}

TEST(Resources, ResourceHandleArray) {
	ResourceDatabase *db = new ResourceDatabase(nullptr);
	
	std::vector<ResourceHandle<Resource>> arr;

	{
		ResourceHandle<TestResource> r(db, new TestResource(), ResourceState::Loaded, UUID());
		arr.push_back(r);
		arr[0].set_state(ResourceState::NeverDelete);
		EXPECT_EQ(r.get_state(), ResourceState::NeverDelete);
	} {
		ResourceHandle<TestResource> r(db, new TestResource(), ResourceState::Loaded, UUID());
		arr.push_back(r);
		arr[1].set_state(ResourceState::NeverDelete);
		EXPECT_EQ(r.get_state(), ResourceState::NeverDelete);
	}

	arr[1].free();

	delete db;
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
*/

