from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        
        # Reads distance from Arduino over serial and publishes to /distance topic
        Node(
            package='my_robot_pkg',
            executable='ardunio_bridge_node',
            name='ardunio_bridge_node'
        ),
        
        # Monitors /distance topic and calls alert service when object is too close
        Node(
            package='my_robot_pkg',
            executable='monitor_node',
            name='monitor_node'
        ),
        
        # Handles alert service and sends commands back to Arduino via /alert_command
        Node(
            package='my_robot_pkg',
            executable='alert_server',
            name='alert_server'
        ),
        
    ])
