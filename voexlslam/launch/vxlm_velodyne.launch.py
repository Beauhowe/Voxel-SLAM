from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.conditions import IfCondition
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare
from launch.substitutions import PathJoinSubstitution


def generate_launch_description():
    rviz = LaunchConfiguration("rviz")
    pkg_share = FindPackageShare("voexlslam")
    config = PathJoinSubstitution([pkg_share, "config", "velodyne_ros2.yaml"])
    rviz_config = PathJoinSubstitution([pkg_share, "rviz_cfg", "back.rviz"])

    return LaunchDescription([
        DeclareLaunchArgument("rviz", default_value="true"),
        Node(
            package="voexlslam",
            executable="voxelslam",
            name="cmn_voxel",
            output="screen",
            parameters=[config, {"finish": False}],
        ),
        Node(
            condition=IfCondition(rviz),
            package="rviz2",
            executable="rviz2",
            name="rviz2",
            arguments=["-d", rviz_config],
        ),
    ])
