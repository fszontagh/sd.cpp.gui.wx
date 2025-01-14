#ifndef __SDGUI_LIBS_SNOWFLAKEIDGENERATOR_H
#define __SDGUI_LIBS_SNOWFLAKEIDGENERATOR_H

#include <chrono>
#include <functional>
#include <string>

namespace sd_gui_utils {

    class SnowflakeIDGenerator {
    public:
        SnowflakeIDGenerator(const std::string& server_id, const std::string& ip = "", uint16_t port = 0, uint64_t epoch = 1609459200000)
            : epoch(epoch), sequence(0) {
            std::hash<std::string> hasher;
            serverHash = hasher(server_id);

            if (!ip.empty() && port != 0) {
                std::string ipPortStr = ip + ":" + std::to_string(port);
                ipPortHash            = hasher(ipPortStr);
            } else {
                ipPortHash = 0;
            }

            lastTimestamp = -1;
        }

        uint64_t generateID(size_t increment) {
            uint64_t timestamp = currentTimestamp();

            if (timestamp == lastTimestamp) {
                sequence = (sequence + 1) & sequenceMask;
                if (sequence == 0) {
                    timestamp = waitNextMillis(lastTimestamp);
                }
            } else {
                sequence = 0;
            }

            lastTimestamp = timestamp;

            // The struct of the ID: timestamp | serverHash | ipPortHash | sequence | increment
            uint64_t id = ((timestamp - epoch) << timestampShift) | (serverHash << serverHashShift) | (ipPortHash << ipPortHashShift) | sequence | (increment & incrementMask);
            return id;
        }

    private:
        uint64_t serverHash;
        uint64_t ipPortHash;
        uint64_t epoch;
        uint64_t sequence;
        uint64_t lastTimestamp;

        static const uint64_t serverHashBits = 16;
        static const uint64_t ipPortHashBits = 12;
        static const uint64_t sequenceBits   = 12;

        static const uint64_t serverHashShift = sequenceBits + ipPortHashBits;
        static const uint64_t ipPortHashShift = sequenceBits;

        static const uint64_t timestampShift = sequenceBits + serverHashBits + ipPortHashBits;

        static const uint64_t sequenceMask = (1 << sequenceBits) - 1;

        static const uint64_t incrementMask = (1ULL << timestampShift) - 1;

        uint64_t currentTimestamp() {
            return std::chrono::duration_cast<std::chrono::milliseconds>(
                       std::chrono::system_clock::now().time_since_epoch())
                .count();
        }

        uint64_t waitNextMillis(uint64_t lastTimestamp) {
            uint64_t timestamp = currentTimestamp();
            while (timestamp <= lastTimestamp) {
                timestamp = currentTimestamp();
            }
            return timestamp;
        }
    };
}
#endif  // __SDGUI_LIBS_SNOWFLAKEIDGENERATOR_H