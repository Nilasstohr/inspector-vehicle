#pragma once
#include <mutex>

/// Shared state written by the Nav2 action callbacks and read by the BT nodes.
///
/// Thread-safety: all public methods are guarded by mutex_ so queue_goal()
/// is safe to call from any thread (e.g. a service callback, a UI thread,
/// or a MultiThreadedExecutor callback group).
///
/// Design note: the BT nodes access the data members directly because they
/// always run on the single BT-tick thread. If you ever move them off that
/// thread, use the accessor methods instead and hold the lock.
struct NavState {
    enum class Phase {
        IDLE,        ///< No active goal
        NAVIGATING,  ///< Goal sent, waiting for result
        SUCCEEDED,   ///< Nav2 reported SUCCEEDED
        FAILED       ///< Nav2 reported ABORTED / CANCELLED
    };

    // ── Data – accessed directly by BT nodes (same thread as timer) ──────
    Phase  phase    = Phase::IDLE;
    double goal_x   = 0.0;
    double goal_y   = 0.0;
    double goal_yaw = 0.0;
    bool   has_goal = false;   ///< a goal has been queued but not yet sent

    // ── Thread-safe API – call from any context ───────────────────────────

    /// Queue a new goal. Safe to call from any thread.
    void queue_goal(double x, double y, double yaw = 0.0) {
        std::lock_guard<std::mutex> lock(mutex_);
        goal_x   = x;
        goal_y   = y;
        goal_yaw = yaw;
        phase    = Phase::IDLE;
        has_goal = true;
    }

    /// Write the navigation result. Safe to call from any thread
    /// (e.g. a ROS action result callback on a different executor thread).
    void set_phase(Phase p) {
        std::lock_guard<std::mutex> lock(mutex_);
        phase = p;
    }

    /// Atomically consume the queued goal. Returns false if no goal pending.
    /// Used by NavigateNode on its first tick.
    bool consume_goal(double & x, double & y, double & yaw) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!has_goal) { return false; }
        x        = goal_x;
        y        = goal_y;
        yaw      = goal_yaw;
        has_goal = false;
        phase    = Phase::NAVIGATING;
        return true;
    }

    std::mutex mutex_;
};

