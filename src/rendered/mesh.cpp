#include "mesh.h"

namespace craft{

    geometry::geometry(const std::vector<vertex> &vertices_, const std::vector<uint32_t> &indices_) {
        vertices = vertices_;
        indices = indices_;

        uint32_t highest = *std::max_element(indices.begin(),indices.end());

        if(highest == 0)
         LOG("Indices of a geometry are zero",5,0)

        if(highest >= vertices.size())
            LOG("Indices of a geometry are higher then the count of vertices",5,0)
    }

    mesh::mesh(const char *deviceName, geometry &geometry_p) {
        m_geometry = std::make_unique<geometry>(geometry_p);

        m_vao = vao(m_geometry->vertices,vk_graphic_device::get().getDeviceAbstraction(deviceName),vk_graphic_device::get().getPhysicalDevice());
        m_ebo = ebo(m_geometry->indices,vk_graphic_device::get().getDeviceAbstraction(deviceName),vk_graphic_device::get().getPhysicalDevice());
        m_vao_data[0] = m_vao.getBuffer();
    }

    void mesh::free() {
        m_ebo.free();
        m_vao.free();
    }

    vao &mesh::getVao() {
        return m_vao;
    }

    ebo &mesh::getEbo() {
        return m_ebo;
    }

    mesh::~mesh() {
        m_ebo.free();
        m_vao.free();
    }

    std::array<VkBuffer, 1> &mesh::getVaoArray() {
        return m_vao_data;
    }

    size_t mesh::getIndicesCount() {
        m_geometry->indices.size();
    }

}