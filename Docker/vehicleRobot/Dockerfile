FROM ros2_humble_image
ARG WD=/inspector-vehicle/ros2_ws

RUN git clone https://github.com/Nilasstohr/inspector-vehicle.git
COPY KeepContainerRunning.sh $WD
WORKDIR $WD
RUN chmod +x Build.sh && ./Build.sh
#RUN chmod +x RunTests.sh && ./RunTests.sh

