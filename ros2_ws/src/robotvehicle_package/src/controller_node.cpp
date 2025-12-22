#include "rclcpp/rclcpp.hpp"
#include "Utilities/SerialInterface.h"
#include "Configurations.h"
#include "ControllerNode.h"

using Eigen::MatrixXd;
using Eigen::Vector3d;

#include <execinfo.h>
#include <unistd.h>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <exception>
#include <cxxabi.h>

using std::placeholders::_1;

#define ROS_INFO RCUTILS_LOG_INFO
#define RAD2DEG(x) ((x)*180./M_PI)

void print_stacktrace();
void termination_handler();

int main(const int argc, char ** argv)
{
    std::set_terminate(termination_handler);

    rclcpp::init(argc, argv);
    const auto node = std::make_shared<ControllerNode>(new SerialInterface(CONFIG_ROBOT_DRIVER_DEVICE_NAME));
    spin(node);
    rclcpp::shutdown();
    return 0;
}

void termination_handler() {
    std::cerr << "\n=== UNHANDLED EXCEPTION ===" << std::endl;

    // Try to get the current exception that caused termination
    std::exception_ptr eptr = std::current_exception();
    if (eptr) {
        try {
            // Rethrow the exception to inspect it
            std::rethrow_exception(eptr);
        } catch (const std::exception& e) {
            // Demangle the C++ type name to make it human-readable
            int status;
            char* demangled = abi::__cxa_demangle(typeid(e).name(), nullptr, nullptr, &status);
            std::cerr << "Exception type: " << (status == 0 ? demangled : typeid(e).name()) << std::endl;
            std::cerr << "Message: " << e.what() << std::endl;
            if (status == 0) free(demangled);
        } catch (...) {
            std::cerr << "Exception type: Unknown (not derived from std::exception)" << std::endl;
        }
    }

    // Print the stack trace to show where the exception originated
    print_stacktrace();
    std::abort();
}

void print_stacktrace() {
    // Capture the current call stack (up to 50 frames)
    void *array[50];
    size_t size = backtrace(array, 50);
    char **symbols = backtrace_symbols(array, size);

    // Get the path to the current executable
    std::string exe_path(1024, '\0');
    ssize_t len = readlink("/proc/self/exe", &exe_path[0], exe_path.size());
    if (len <= 0) exe_path.clear();
    else exe_path.resize(len);

    // Find the base address where the executable is loaded in memory
    // This is needed to convert absolute addresses to offsets for addr2line
    std::string base_addr_str;
    if (!exe_path.empty()) {
        std::ifstream maps("/proc/self/maps");
        std::string line;
        while (std::getline(maps, line)) {
            // Find the memory mapping for our executable
            if (line.find(exe_path) != std::string::npos) {
                auto dash = line.find('-');
                if (dash != std::string::npos) base_addr_str = line.substr(0, dash);
                break;
            }
        }
    }

    std::cerr << "\n=== STACK TRACE ===" << std::endl;
    for (size_t i = 0; i < size; ++i) {
        if (exe_path.empty()) continue;

        // Convert the return address to an offset relative to the executable base
        uintptr_t addr = reinterpret_cast<uintptr_t>(array[i]);
        std::string addr_arg;
        if (!base_addr_str.empty()) {
            uintptr_t base = static_cast<uintptr_t>(std::stoull(base_addr_str, nullptr, 16));
            // Subtract 1 because stack contains return address (after the call), not call site
            uintptr_t offset = addr - base - 1;
            char buf[32];
            std::snprintf(buf, sizeof(buf), "0x%zx", offset);
            addr_arg = buf;
        } else {
            char buf[32];
            std::snprintf(buf, sizeof(buf), "0x%zx", addr - 1);
            addr_arg = buf;
        }

        // Use addr2line to convert address to function name and file location
        // -f: show function names, -C: demangle C++ names, -e: executable path
        std::string cmd = std::string("addr2line -f -C -e ") + exe_path + " " + addr_arg;
        FILE *fp = popen(cmd.c_str(), "r");
        if (fp) {
            std::string function_name, file_location;
            char outbuf[512];

            // First line from addr2line is the function name
            if (fgets(outbuf, sizeof(outbuf), fp) != nullptr) {
                function_name = outbuf;
                function_name.erase(function_name.find_last_not_of("\n\r") + 1);
            }
            // Second line is the file path and line number
            if (fgets(outbuf, sizeof(outbuf), fp) != nullptr) {
                file_location = outbuf;
                file_location.erase(file_location.find_last_not_of("\n\r") + 1);

                // Remove compiler-generated discriminator annotations
                size_t discriminator_pos = file_location.find(" (discriminator");
                if (discriminator_pos != std::string::npos) {
                    file_location.erase(discriminator_pos);
                }
            }
            pclose(fp);

            // Skip internal stack trace handling functions to avoid clutter
            if (function_name.find("termination_handler") != std::string::npos ||
                function_name.find("print_stacktrace") != std::string::npos) {
                continue;
            }

            // Only display frames from our project code
            if (file_location.find("/robotvehicle_package/src/") != std::string::npos) {
                std::cerr  << file_location << " " << function_name << std::endl;
            }
        }
    }

    free(symbols);
}




