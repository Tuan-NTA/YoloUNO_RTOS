#ifndef MY_OTA_FIRMWARE_H
#define MY_OTA_FIRMWARE_H

#include <ArduinoJson.h>          // Hoặc <ArduinoJson.h>, <json.hpp>... tuỳ bạn parse JSON
#include "OTA_Handler.h"          // Từ ThingsBoard SDK
#include "OTA_Update_Callback.h"  // Từ ThingsBoard SDK

// Ví dụ, bạn cần Logger, Callback... tuỳ thuộc SDK cũ
// #include "Logger.h"
// #include "Callback.h"

// Topic để nhận chunk firmware trả về
static constexpr char FIRMWARE_RESPONSE_SUBSCRIBE_TOPIC[] = "v2/fw/response/+";
// Format topic gửi request chunk: "v2/fw/request/%u/chunk/%u"

/// @brief Lớp này cho phép bạn tự quản lý shared attributes 
/// (tự subscribe ở code ngoài) và chỉ việc gọi hàm ProcessFirmwareJson()
/// để kích hoạt OTA chunk-based có sẵn.
class MyOtaFirmware {
public:
    /// @brief Constructor
    MyOtaFirmware();

    /// @brief Destructor
    ~MyOtaFirmware();

    // ========== Thiết lập các callback MQTT cần thiết ==========

    /// @brief Thiết lập các callback cần cho OTA (gửi JSON string, subscribe, unsubscribe, v.v.)
    /// @param send_json_string_callback  Hàm callback gửi payload JSON dạng string lên topic MQTT
    /// @param subscribe_topic_callback   Hàm callback subscribe một topic
    /// @param unsubscribe_topic_callback Hàm callback unsubscribe một topic
    /// @param get_receive_size_callback  Lấy kích thước buffer nhận (để điều chỉnh nếu cần chunk lớn)
    /// @param get_send_size_callback     Lấy kích thước buffer gửi
    /// @param set_buffer_size_callback   Thiết lập buffer size
    /// @param get_request_id_callback    Lấy ID request OTA (tăng dần, đảm bảo unique)
    void SetClientCallbacks(
        Callback<bool, const char * const, const char * const>::function send_json_string_callback,
        Callback<bool, const char * const>::function subscribe_topic_callback,
        Callback<bool, const char * const>::function unsubscribe_topic_callback,
        Callback<uint16_t>::function get_receive_size_callback,
        Callback<uint16_t>::function get_send_size_callback,
        Callback<bool, uint16_t, uint16_t>::function set_buffer_size_callback,
        Callback<size_t *>::function get_request_id_callback
    );

    // ========== Quản lý quá trình OTA ==========

    /// @brief Hàm chính để bạn "truyền" dữ liệu firmware (lấy từ shared attributes) vào.
    /// Thay thế cho hàm Firmware_Shared_Attribute_Received() gốc.
    /// @param data JSON chứa các key: "fw_title", "fw_version", "fw_checksum", "fw_checksum_algorithm", "fw_size"
    /// @param callback Cấu hình OTA (OTA_Update_Callback) - bạn tự tạo & truyền vào
    /// @return true nếu bắt đầu OTA thành công, false nếu lỗi
    bool ProcessFirmwareJson(JsonObjectConst data, OTA_Update_Callback const & callback);

    /// @brief Dừng OTA giữa chừng
    void StopFirmwareUpdate();

    /// @brief Kiểm tra xem topic MQTT có phải topic firmware response không
    bool CompareResponseTopic(const char *topic) const;

    /// @brief Xử lý payload chunk firmware khi nhận từ topic "v2/fw/response/..."
    /// Bạn gọi hàm này từ callback MQTT onMessage.
    void ProcessResponse(const char *topic, uint8_t *payload, unsigned int length);

    /// @brief Nếu SDK ThingsBoard yêu cầu vòng lặp (không dùng timer), bạn gọi hàm này trong loop()
    void Loop();

private:
    // ========== Các hàm nội bộ ==========

    /// @brief Đăng ký topic "v2/fw/response/+" để nhận chunk firmware
    bool FirmwareOtaSubscribe();

    /// @brief Hủy đăng ký topic "v2/fw/response/+"
    bool FirmwareOtaUnsubscribe();

    /// @brief Gửi request chunk firmware lên MQTT
    bool PublishChunkRequest(size_t request_id, size_t chunk_index);

    /// @brief Gửi state firmware (DOWNLOADING, FAILED, v.v.) lên ThingsBoard
    bool FirmwareSendState(const char *state, const char *error = "");

    /// @brief Chuyển đổi chuỗi "MD5", "SHA256" ... thành mbedtls_md_type_t
    mbedtls_md_type_t ConvertChecksumAlgorithm(const char *algoStr);

private:
    // ====== Callback MQTT (copy từ OTA_Firmware_Update) ======
    Callback<bool, const char * const, const char * const> m_send_json_string_callback;
    Callback<bool, const char * const>                     m_subscribe_topic_callback;
    Callback<bool, const char * const>                     m_unsubscribe_topic_callback;
    Callback<uint16_t>                                     m_get_receive_size_callback;
    Callback<uint16_t>                                     m_get_send_size_callback;
    Callback<bool, uint16_t, uint16_t>                     m_set_buffer_size_callback;
    Callback<size_t *>                                     m_get_request_id_callback;

    // ====== Đối tượng OTA_Update_Callback & OTA_Handler ======
    OTA_Update_Callback               m_fw_callback; // Thông tin cấu hình OTA
    OTA_Handler<Logger>              m_ota;          // Xử lý chunk-based (ghi flash, checksum, ...)

    // ====== Biến tạm ======
    char    m_response_topic[64];
    bool    m_changed_buffer_size;
    uint16_t m_previous_buffer_size;
};

#endif // MY_OTA_FIRMWARE_H
