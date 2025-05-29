#pragma once

#include <cstdint>

namespace mcs::protocol::ip
{
    /**
     * @brief scope_id_type的作用
区分网络接口
IPv6的链路本地地址（如fe80::/10）在同一主机可能存在于多个网络接口（如Wi-Fi、以太网等）中。scope_id通过标识具体的接口索引（如%eth0），确保数据包正确路由到目标接口26。

解决地址歧义
若同一链路本地地址未指定作用域ID，操作系统无法确定目标接口，可能导致通信失败。例如，在无线路由器和有线网络同时存在时，需通过scope_id明确目标路径8。

兼容RFC标准
根据RFC
4007，作用域ID是IPv6地址的标准组成部分，尤其在链路本地和站点本地地址中必须包含。C++标准库通过scope_id_type实现对此规范的遵循46。
     *
     */
    using scope_id_type = std::uint_least32_t;
}; // namespace mcs::protocol::ip