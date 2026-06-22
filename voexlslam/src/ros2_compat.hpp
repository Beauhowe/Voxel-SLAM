#pragma once

#include <rclcpp/rclcpp.hpp>

#include <algorithm>
#include <string>

template <typename T>
void ros2_param(const rclcpp::Node::SharedPtr &node, const std::string &name, T &value, const T &default_value)
{
  std::string ros2_name = name;
  std::replace(ros2_name.begin(), ros2_name.end(), '/', '.');
  if (!node->has_parameter(ros2_name)) {
    value = node->declare_parameter<T>(ros2_name, default_value);
    return;
  }
  value = node->get_parameter(ros2_name).get_value<T>();
}

extern rclcpp::Node::SharedPtr g_node;

inline double ros2_now_seconds(const rclcpp::Node::SharedPtr &node)
{
  return node->now().seconds();
}

template <typename MessageT>
inline double ros2_stamp_seconds(const MessageT &message)
{
  return rclcpp::Time(message.header.stamp).seconds();
}
