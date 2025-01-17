#pragma once
#include "AllHeaders.h"
#include "VulkanDestroyer.h"
#include "Queue.h"
#include "Device.h"
#include "Geometry/mesh.h"
#include "Buffer.h"

namespace LavaCake {
	namespace Framework {

		class VertexBuffer {
		public:
			
			VertexBuffer(std::vector<LavaCake::Geometry::Mesh_t*> m, uint32_t binding = 0,  VkVertexInputRate inputRate = VK_VERTEX_INPUT_RATE_VERTEX);


			void allocate(Queue* queue, CommandBuffer& cmdBuff, VkBufferUsageFlags otherUsage = VkBufferUsageFlags(0) );
			
			Buffer& getVertexBuffer();
			
			Buffer& getIndexBuffer();

			void swapMeshes(std::vector<LavaCake::Geometry::Mesh_t*>				m);

			std::vector<VkVertexInputAttributeDescription>& getAttributeDescriptions();

			std::vector<VkVertexInputBindingDescription>& getBindingDescriptions();
			
			VkPrimitiveTopology primitiveTopology() {
				switch (m_topology)
				{
				case LavaCake::Geometry::POINT:
					return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
					break;
				case LavaCake::Geometry::LINE:
					return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
					break;
				case LavaCake::Geometry::TRIANGLE:
					return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
					break;
				default:
					return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
					break;
				}
			}

			
			VkPolygonMode polygonMode() {
				switch (m_topology)
				{
				case LavaCake::Geometry::POINT:
					return VK_POLYGON_MODE_POINT;
					break;
				case LavaCake::Geometry::LINE:
					return VK_POLYGON_MODE_LINE;
					break;
				case LavaCake::Geometry::TRIANGLE:
					return VK_POLYGON_MODE_FILL;
					break;
				default:
					return VK_POLYGON_MODE_FILL;
					break;
				}
			}

			size_t getIndicesNumber() {
				return m_indices.size();
			}

			size_t getVerticiesNumber() {
				return m_vertices.size() / m_stride;
			}
			
			uint32_t getStrideSize() {
				return m_stride;
			}

			uint32_t getByteStrideSize() {
				return m_stride * sizeof(float);
			}

			bool isIndexed();

			~VertexBuffer() {
				Device* d = Device::getDevice();
				VkDevice logical = d->getLogicalDevice();
			}

		private :


			std::vector<VkVertexInputAttributeDescription>			m_attributeDescriptions;
			std::vector<VkVertexInputBindingDescription>				m_bindingDescriptions;
			Buffer																							m_vertexBuffer;
			std::vector<float>																	m_vertices;
			uint32_t																						m_stride;
			Buffer																							m_indexBuffer;
			std::vector<uint32_t>																m_indices;
			bool																								m_indexed;
			LavaCake::Geometry::topology												m_topology;
		};

	}
}