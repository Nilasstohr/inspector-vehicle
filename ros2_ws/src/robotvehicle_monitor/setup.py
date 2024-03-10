from setuptools import setup

package_name = 'robotvehicle_monitor'

setup(
    name=package_name,
    version='0.0.0',
    packages=[package_name],
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='robot1',
    maintainer_email='Nilasstohr@hotmail.com',
    description='TODO: Package description',
    license='TODO: License declaration',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [
        	'laserscan = robotvehicle_monitor.subscriber_laserscan:main',
            'laserscan = robotvehicle_monitor.monitor_node:main',
        	'control_client = robotvehicle_monitor.client_member_function:main',
        ],
    }, 
)
