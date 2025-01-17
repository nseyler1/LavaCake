#include "UniformBuffer.h"


namespace LavaCake {
	namespace Framework {

		void UniformBuffer::end() {
			m_modified = std::vector<bool>(m_variables.size());
			LavaCake::Framework::Device* d = LavaCake::Framework::Device::getDevice();
			VkDevice logical = d->getLogicalDevice();
			VkPhysicalDevice physical = d->getPhysicalDevice();

			for (uint32_t i = 0; i < m_variables.size(); i++) {
				VkDeviceSize s = sizeof(m_variables[i][0]) * m_variables[i].size();
				m_typeSizeOffset.push_back(std::pair<VkDeviceSize, VkDeviceSize>(s, m_bufferSize));
				m_bufferSize += s;
			}

      VkPhysicalDeviceProperties* p = new  VkPhysicalDeviceProperties();
      vkGetPhysicalDeviceProperties(physical,
                                    p);
      
      VkDeviceSize padding = p->limits.nonCoherentAtomSize - m_bufferSize % p->limits.nonCoherentAtomSize;
      
      //adding empty value at the end of the buffer to match the atomic size of a buffer;
      m_variables.push_back(std::vector<int>(padding/ sizeof(int), 0));
      m_typeSizeOffset.push_back(std::pair<VkDeviceSize, VkDeviceSize>(padding, m_bufferSize));
      m_bufferSize+=padding;
      
      
      
			m_stagingBuffer.allocate(m_bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

			m_buffer.allocate(m_bufferSize, (VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT), VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

			copyToStageMemory(true);
		}

		void UniformBuffer::update(CommandBuffer& commandBuffer, bool all) {
			copyToStageMemory(all);

			m_buffer.setAccess(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_TRANSFER_WRITE_BIT);

			std::vector<VkBufferCopy> regions = {
				{
					0,                        // VkDeviceSize     srcOffset
					0,                        // VkDeviceSize     dstOffset
					m_bufferSize                // VkDeviceSize     size
				}
			};

			m_stagingBuffer.copyToBuffer(commandBuffer, m_buffer, regions);


			m_buffer.setAccess(commandBuffer, VK_PIPELINE_STAGE_VERTEX_SHADER_BIT, VK_ACCESS_UNIFORM_READ_BIT);

		}

		VkBuffer& UniformBuffer::getHandle() {
			return m_buffer.getHandle();
		};

		void UniformBuffer::copyToStageMemory(bool all) {
			LavaCake::Framework::Device* d = LavaCake::Framework::Device::getDevice();
			VkDevice logical = d->getLogicalDevice();
			VkPhysicalDevice physical = d->getPhysicalDevice();
			std::vector<int> variable;
			VkDeviceSize size = 0;
			for (uint32_t i = 0; i < m_variables.size(); i++) {
				size += m_typeSizeOffset[i].first;
				for (uint32_t j = 0; j < m_variables[i].size(); j++) {
					variable.push_back(m_variables[i][j]);
				}
			}


			m_stagingBuffer.write(variable);
		}

		void UniformBuffer::addArray(std::string name, std::vector<int>& value) {
			if (m_variableNames.find(name) != m_variableNames.end()) {
				ErrorCheck::setError((char*)"The variable allready exist in this UniformBuffer");
				return;
			}
			int i = int(m_variables.size());
			m_variables.push_back(value);
			m_variableNames.insert(std::pair<std::string, int>(name, i));
		}

		void UniformBuffer::setArray(std::string name, std::vector<int>& value) {
			if (m_variableNames.find(name) == m_variableNames.end()) {
				ErrorCheck::setError((char*)"The variable does not exist in this UniformBuffer");
				return;
			}
			int i = m_variableNames[name];
			std::vector<int> v = m_variables[i];
			if (v.size() != value.size()) {
				ErrorCheck::setError((char*)"The new value does not match the type of the one currently stored in this UniformBuffer");
				return;
			}

			m_variables[i] = value;
			if (m_modified.size() > 0) {
				m_modified[i] = true;
			}
		}
	}
}
