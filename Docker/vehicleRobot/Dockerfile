FROM ros2_humble_image
ARG WD=/inspector-vehicle/ros2_ws

RUN git clone https://github.com/Nilasstohr/inspector-vehicle.git
COPY KeepContainerRunning.sh $WD
WORKDIR /inspector-vehicle/ros2_ws
RUN chmod +x build.sh && ./Build.sh
