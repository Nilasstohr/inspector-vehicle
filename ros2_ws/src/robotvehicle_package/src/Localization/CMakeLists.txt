add_library(LocalizationLib
        Estimation.cpp
        Matches.cpp
        Matching.cpp
        MeasurementPrediction.cpp
        Observations.cpp
        PredictionDifferentialDrive.cpp
        Odom.cpp
        RStack.cpp
        SensorRecorder.cpp
        Incremental.cpp
        LineStack.cpp
        Line.cpp
        HStack.cpp
        KalmanLocalization.cpp
        Lines.cpp
        BatchGradientDecent.cpp
)
ament_target_dependencies(LocalizationLib rclcpp sensor_msgs)
target_link_libraries (LocalizationLib Eigen3::Eigen UtilitiesLib SensorLib)
