#include "MyOtaUpdate.h"
#include <string.h>  // strlen, strncmp, ...
// #include "Logger.h" // Tùy bạn, nếu cần in log

// Một số macro gợi ý, copy từ OTA_Firmware_Update.h
static constexpr char FW_CHKS_ALGO_NOT_SUPPORTED[] = "Checksum algorithm not supported";
static constexpr char FW_STATE_FAILED[]           = "FAILED";
static constexpr char FW_STATE_DOWNLOADING[]      = "DOWNLOADING";
static constexpr char FW_STATE_UPDATED[]          = "UPDATED";
static constexpr char FW_STATE_UPDATING[]         = "UPDATING";

////////////////////////////////////////////////////////////////////////////////////////
// Constructor/Destructor
////////////////////////////////////////////////////////////////////////////////////////

MyOtaFirmware::MyOtaFirmware()
: m_send_json_string_callback()
, m_subscribe_topic_callback()
, m_unsubscribe_topic_callback()
, m_get_receive_size_callback()
, m_get_send_size_callback()
, m_set_buffer_size_callback()
, m_get_request_id_callback()
, m_fw_callback()
, m_ota(
    // publish_callback -> Request chunk
    [this](size_t const & req_id, size_t const & chunk_idx) {
      return this->PublishChunkRequest(req_id, chunk_idx);
    },
    // send_fw_state_callback
    [this](char const * state, char const * error) {
      return this->FirmwareSendState(state, error);
    },
    // finish_callback -> Khi OTA kết thúc
    [this]() {
      // Ở đây bạn có thể unsubscribe topic nếu muốn
      this->FirmwareOtaUnsubscribe();
      return true;
    }
  )
, m_changed_buffer_size(false)
, m_previous_buffer_size(0)
{
    // Mặc định để topic rỗng
    snprintf(m_response_topic, sizeof(m_response_topic), "v2/fw/response/0/chunk/");
}

MyOtaFirmware::~MyOtaFirmware() {
    // Dọn dẹp nếu cần
}

////////////////////////////////////////////////////////////////////////////////////////
// Thiết lập callback MQTT
////////////////////////////////////////////////////////////////////////////////////////

void MyOtaFirmware::SetClientCallbacks(
    Callback<bool, const char * const, const char * const>::function send_json_string_callback,
    Callback<bool, const char * const>::function subscribe_topic_callback,
    Callback<bool, const char * const>::function unsubscribe_topic_callback,
    Callback<uint16_t>::function get_receive_size_callback,
    Callback<uint16_t>::function get_send_size_callback,
    Callback<bool, uint16_t, uint16_t>::function set_buffer_size_callback,
    Callback<size_t *>::function get_request_id_callback
) {
    m_send_json_string_callback.Set_Callback(send_json_string_callback);
    m_subscribe_topic_callback.Set_Callback(subscribe_topic_callback);
    m_unsubscribe_topic_callback.Set_Callback(unsubscribe_topic_callback);
    m_get_receive_size_callback.Set_Callback(get_receive_size_callback);
    m_get_send_size_callback.Set_Callback(get_send_size_callback);
    m_set_buffer_size_callback.Set_Callback(set_buffer_size_callback);
    m_get_request_id_callback.Set_Callback(get_request_id_callback);
}

////////////////////////////////////////////////////////////////////////////////////////
// Hàm chính xử lý JSON firmware (thay cho Firmware_Shared_Attribute_Received)
////////////////////////////////////////////////////////////////////////////////////////

bool MyOtaFirmware::ProcessFirmwareJson(JsonObjectConst data, OTA_Update_Callback const & callback) {
    // Kiểm tra các key quan trọng
    if (!data.containsKey("fw_title")  ||
        !data.containsKey("fw_version")||
        !data.containsKey("fw_checksum")||
        !data.containsKey("fw_checksum_algorithm")||
        !data.containsKey("fw_size"))
    {
        // Logger::printfln("Firmware info not enough, aborting OTA");
        return false;
    }

    // Lấy giá trị
    const char* fw_title    = data["fw_title"];
    const char* fw_version  = data["fw_version"];
    const char* fw_checksum = data["fw_checksum"];
    const char* fw_algo_str = data["fw_checksum_algorithm"];
    size_t fw_size          = data["fw_size"];

    // Gán callback
    m_fw_callback = callback;

    // Lấy request ID
    size_t * p_request_id = m_get_request_id_callback.Call_Callback();
    if (p_request_id == nullptr) {
        // Logger::printfln("Request ID null");
        return false;
    }
    (*p_request_id)++;
    m_fw_callback.Set_Request_ID(*p_request_id);

    // Tạo topic response
    snprintf(m_response_topic, sizeof(m_response_topic), "v2/fw/response/%u/chunk/", (unsigned int)(*p_request_id));

    // Kiểm tra nếu firmware version giống version đang có => bỏ qua
    if ((strncmp(m_fw_callback.Get_Firmware_Title(),   fw_title,   strlen(fw_title))   == 0) &&
        (strncmp(m_fw_callback.Get_Firmware_Version(), fw_version, strlen(fw_version)) == 0))
    {
        // Logger::printfln("Firmware same version => no update needed");
        FirmwareSendState(FW_STATE_UPDATED, "Already updated");
        return true; // Dù sao cũng return true, vì không lỗi
    }

    // Chuyển chuỗi thuật toán -> mbedtls_md_type_t
    mbedtls_md_type_t fw_algorithm = ConvertChecksumAlgorithm(fw_algo_str);
    if (fw_algorithm == mbedtls_md_type_t{}) {
        // Logger::printfln(FW_CHKS_ALGO_NOT_SUPPORTED);
        FirmwareSendState(FW_STATE_FAILED, FW_CHKS_ALGO_NOT_SUPPORTED);
        return false;
    }

    // Bắt đầu OTA: trước hết subscribe topic firmware
    if (!FirmwareOtaSubscribe()) {
        // Logger::printfln("Subscribe firmware topic failed");
        return false;
    }

    // Tăng buffer size nếu cần
    uint16_t cur_size = m_get_receive_size_callback.Call_Callback();
    m_previous_buffer_size = cur_size;
    uint16_t chunk_size = m_fw_callback.Get_Chunk_Size();
    m_changed_buffer_size = (cur_size < (chunk_size + 50U));

    if (m_changed_buffer_size) {
        // Tăng buffer
        if (!m_set_buffer_size_callback.Call_Callback(chunk_size + 50U, m_get_send_size_callback.Call_Callback())) {
            // Logger::printfln("Not enough RAM to set new buffer size");
            FirmwareSendState(FW_STATE_FAILED, "Not enough RAM for bigger buffer");
            return false;
        }
    }

    // Gọi Start_Firmware_Update trong OTA_Handler
    m_ota.Start_Firmware_Update(m_fw_callback, fw_size, fw_checksum, fw_algorithm);
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////
// Hỗ trợ OTA
////////////////////////////////////////////////////////////////////////////////////////

bool MyOtaFirmware::CompareResponseTopic(const char *topic) const {
    // So sánh với m_response_topic
    // m_response_topic dạng "v2/fw/response/<req_id>/chunk/"
    // Cần so sánh prefix
    return (strncmp(topic, m_response_topic, strlen(m_response_topic)) == 0);
}

void MyOtaFirmware::ProcessResponse(const char *topic, uint8_t *payload, unsigned int length) {
    // Tìm chunk index
    // 1) Tạo 1 buffer
    char response_topic[128];
    snprintf(response_topic, sizeof(response_topic), "%s", m_response_topic);

    // 2) Tính chunk index => ta có sẵn code parse chunk trong Helper, 
    //   nhưng ở đây ta làm đơn giản, assume chunk index là phần sau topic
    //   (Hoặc copy Helper::parseRequestId(...) cũ tuỳ bạn)
    //   Giả sử chunk index = 0 => "v2/fw/response/<req_id>/chunk/0"
    //   => Tìm chuỗi "/chunk/" => parse int
    //   Ở đây ta làm đơn giản:
    const char *ptr = strstr(topic, "/chunk/");
    size_t chunk_idx = 0;
    if (ptr) {
        chunk_idx = atoi(ptr + strlen("/chunk/"));
    }

    // Gửi payload cho m_ota
    m_ota.Process_Firmware_Packet(chunk_idx, payload, length);
}

void MyOtaFirmware::Loop() {
#if !THINGSBOARD_USE_ESP_TIMER
    m_ota.update();
#endif
}

void MyOtaFirmware::StopFirmwareUpdate() {
    m_ota.Stop_Firmware_Update();
}

////////////////////////////////////////////////////////////////////////////////////////
// Các hàm private
////////////////////////////////////////////////////////////////////////////////////////

bool MyOtaFirmware::FirmwareOtaSubscribe() {
    // Subscribe topic "v2/fw/response/+"
    if (!m_subscribe_topic_callback.Call_Callback(FIRMWARE_RESPONSE_SUBSCRIBE_TOPIC)) {
        FirmwareSendState(FW_STATE_FAILED, "Failed to subscribe firmware response");
        return false;
    }
    return true;
}

bool MyOtaFirmware::FirmwareOtaUnsubscribe() {
    // Khôi phục buffer size nếu đã thay đổi
    if (m_changed_buffer_size) {
        m_set_buffer_size_callback.Call_Callback(m_previous_buffer_size, m_get_send_size_callback.Call_Callback());
    }
    // Hủy sub
    return m_unsubscribe_topic_callback.Call_Callback(FIRMWARE_RESPONSE_SUBSCRIBE_TOPIC);
}

bool MyOtaFirmware::PublishChunkRequest(size_t request_id, size_t chunk_index) {
    // Tạo topic "v2/fw/request/<req_id>/chunk/<chunk_index>"
    char topic[128];
    snprintf(topic, sizeof(topic), "v2/fw/request/%u/chunk/%u", (unsigned int)request_id, (unsigned int)chunk_index);

    // Chunk size = m_fw_callback.Get_Chunk_Size(), convert to string
    char chunkSizeStr[10];
    snprintf(chunkSizeStr, sizeof(chunkSizeStr), "%u", (unsigned int)m_fw_callback.Get_Chunk_Size());

    // Gửi chunk size dưới dạng JSON string
    return m_send_json_string_callback.Call_Callback(topic, chunkSizeStr);
}

bool MyOtaFirmware::FirmwareSendState(const char *state, const char *error) {
    // Tuỳ bạn có muốn gửi state kiểu JSON: {"fw_state": "DOWNLOADING", "fw_error": ""}
    // Ở đây ta chỉ demo: topic = "telemetry", payload = ...
    // Hoặc tuỳ callback
    // Tối giản:
    // Logger::printfln("FW State: %s, error: %s", state, error);
    return true;
}

mbedtls_md_type_t MyOtaFirmware::ConvertChecksumAlgorithm(const char *algoStr) {
    if (strcmp(algoStr, "MD5") == 0) {
        return MBEDTLS_MD_MD5;
    } else if (strcmp(algoStr, "SHA256") == 0) {
        return MBEDTLS_MD_SHA256;
    } else if (strcmp(algoStr, "SHA384") == 0) {
        return MBEDTLS_MD_SHA384;
    } else if (strcmp(algoStr, "SHA512") == 0) {
        return MBEDTLS_MD_SHA512;
    }
    return mbedtls_md_type_t{}; // Trả về rỗng => báo lỗi
}
