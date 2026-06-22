#include "voxelslam_pc2.hpp"

#include <cstring>
#include <sstream>

#include <rviz_common/display_context.hpp>
#include <rviz_common/properties/status_property.hpp>
#include <rviz_common/validate_floats.hpp>
#include <rviz_default_plugins/displays/pointcloud/point_cloud_common.hpp>
#include <rviz_default_plugins/displays/pointcloud/point_cloud_helpers.hpp>

namespace voexlslam_pointcloud2
{

PointCloud2Display::PointCloud2Display()
: point_cloud_common_(std::make_unique<rviz_default_plugins::PointCloudCommon>(this))
{
}

PointCloud2Display::~PointCloud2Display()
{
  unsubscribe();
}

void PointCloud2Display::onInitialize()
{
  MFDClass::onInitialize();
  point_cloud_common_->initialize(context_, scene_node_);
}

void PointCloud2Display::processMessage(sensor_msgs::msg::PointCloud2::ConstSharedPtr cloud)
{
  auto filtered = std::make_shared<sensor_msgs::msg::PointCloud2>();
  int32_t xi = rviz_default_plugins::findChannelIndex(cloud, "x");
  int32_t yi = rviz_default_plugins::findChannelIndex(cloud, "y");
  int32_t zi = rviz_default_plugins::findChannelIndex(cloud, "z");

  if (xi == -1 || yi == -1 || zi == -1) {
    return;
  }

  const uint32_t xoff = cloud->fields[xi].offset;
  const uint32_t yoff = cloud->fields[yi].offset;
  const uint32_t zoff = cloud->fields[zi].offset;
  const uint32_t point_step = cloud->point_step;
  const size_t point_count = cloud->width * cloud->height;

  if (point_count * point_step != cloud->data.size()) {
    std::stringstream ss;
    ss << "Data size (" << cloud->data.size() << " bytes) does not match width ("
       << cloud->width << ") times height (" << cloud->height
       << ") times point_step (" << point_step << "). Dropping message.";
    setStatusStd(rviz_common::properties::StatusProperty::Error, "Message", ss.str());
    return;
  }

  filtered->data.resize(cloud->data.size());
  uint32_t output_count = 0;
  if (point_count != 0) {
    uint8_t * output_ptr = filtered->data.data();
    const uint8_t * ptr = cloud->data.data();
    const uint8_t * ptr_end = ptr + cloud->data.size();
    const uint8_t * ptr_init = nullptr;
    size_t points_to_copy = 0;

    for (; ptr < ptr_end; ptr += point_step) {
      float x = *reinterpret_cast<const float *>(ptr + xoff);
      float y = *reinterpret_cast<const float *>(ptr + yoff);
      float z = *reinterpret_cast<const float *>(ptr + zoff);

      if (rviz_common::validateFloats(x) && rviz_common::validateFloats(y) &&
          rviz_common::validateFloats(z)) {
        if (points_to_copy == 0) {
          ptr_init = ptr;
          points_to_copy = 1;
        } else {
          ++points_to_copy;
        }
      } else if (points_to_copy != 0) {
        std::memcpy(output_ptr, ptr_init, point_step * points_to_copy);
        output_ptr += point_step * points_to_copy;
        points_to_copy = 0;
      }
    }

    if (points_to_copy != 0) {
      std::memcpy(output_ptr, ptr_init, point_step * points_to_copy);
      output_ptr += point_step * points_to_copy;
    }
    output_count = static_cast<uint32_t>((output_ptr - filtered->data.data()) / point_step);
  }

  filtered->header = cloud->header;
  filtered->fields = cloud->fields;
  filtered->data.resize(output_count * point_step);
  filtered->height = 1;
  filtered->width = output_count;
  filtered->is_bigendian = cloud->is_bigendian;
  filtered->point_step = point_step;
  filtered->row_step = output_count * point_step;
  filtered->is_dense = true;

  if (output_count > 0) {
    point_cloud_common_->addMessage(filtered);
  } else {
    point_cloud_common_->reset();
  }
}

void PointCloud2Display::update(float wall_dt, float ros_dt)
{
  point_cloud_common_->update(wall_dt, ros_dt);
}

void PointCloud2Display::reset()
{
  MFDClass::reset();
  point_cloud_common_->reset();
}

}  // namespace voexlslam_pointcloud2

#include <pluginlib/class_list_macros.hpp>
PLUGINLIB_EXPORT_CLASS(voexlslam_pointcloud2::PointCloud2Display, rviz_common::Display)
