#ifndef __SDGUI_HELPERS_QUEUE_ITEM_STATS__
#define __SDGUI_HELPERS_QUEUE_ITEM_STATS__
struct QueueStatsStepItem {
    int step;
    int steps;
    float time;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(QueueStatsStepItem, step, steps, time)
struct QueueItemStats {
    float time_min = 0.f, time_max = 0.f, time_avg = 0.f, time_total = 0.f;
    std::vector<QueueStatsStepItem> time_per_step = {};
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(QueueItemStats, time_min, time_max, time_avg, time_total, time_per_step)
#endif