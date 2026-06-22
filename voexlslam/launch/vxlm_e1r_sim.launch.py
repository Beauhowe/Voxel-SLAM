from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.conditions import IfCondition
from launch.substitutions import LaunchConfiguration, PathJoinSubstitution
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare


def generate_launch_description():
    rviz = LaunchConfiguration("rviz")
    use_sim_time = LaunchConfiguration("use_sim_time")
    pkg_share = FindPackageShare("voexlslam")
    config = PathJoinSubstitution([pkg_share, "config", "e1r_sim.yaml"])
    rviz_config = PathJoinSubstitution([pkg_share, "rviz_cfg", "e1r_sim.rviz"])

    return LaunchDescription([
        DeclareLaunchArgument("rviz", default_value="true"),
        DeclareLaunchArgument("use_sim_time", default_value="true"),
        Node(
            package="voexlslam",
            executable="voxelslam",
            name="cmn_voxel",
            output="screen",
            parameters=[config, {"finish": False, "use_sim_time": use_sim_time}],
        ),
        Node(
            condition=IfCondition(rviz),
            package="rviz2",
            executable="rviz2",
            name="rviz2",
            arguments=["-d", rviz_config],
            parameters=[{"use_sim_time": use_sim_time}],
        ),
    ])
